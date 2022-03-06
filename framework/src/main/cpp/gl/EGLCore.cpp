//
// Created by wansu on 2021/12/3.
//

#include "EGLCore.h"
namespace smedia {

    EGLCore::EGLCore():mCurrentSurface(EGL_NO_SURFACE),mDisplay(EGL_NO_DISPLAY),mEglContext(EGL_NO_CONTEXT) {
        LOG_INFO << "create EGLCore";
    }

    bool EGLCore::initEGL(EGLContext shareContext) {
        if (shareContext == nullptr) {
            shareContext = EGL_NO_CONTEXT;
            LOG_INFO << "shareContext is null";
        }

        mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (mDisplay == EGL_NO_DISPLAY) {
            LOG_ERROR << "egl get display error:" << eglGetError();
            return false;
        }

        EGLint majorVersion;
        EGLint minVersion;
        if (!eglInitialize(mDisplay, &majorVersion, &minVersion)) {
            LOG_ERROR << "egl init error:" << eglGetError();
            return false;
        }

        EGLint attribList[] = {
                EGL_NONE
        };
        const int maxConfigs = 1;
        int numConfigs;
        EGLConfig eglConfigs[1];
        if (!eglChooseConfig(mDisplay, attribList, eglConfigs, maxConfigs, &numConfigs)) {
            LOG_ERROR << "choose config error" << eglGetError();
            return false;
        }
        mEglConfig = eglConfigs[0];

        int attrib3_list[] = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
        };
        mEglContext = eglCreateContext(mDisplay, mEglConfig, shareContext, attrib3_list);
        if (mEglContext == EGL_NO_CONTEXT) {
            LOG_ERROR << "create egl context error" << eglGetError();
            return false;
        }

        checkEGLError("initEGL");
        LOG_INFO << "init egl success";
        return true;
    }

    EGLSurface EGLCore::createWindowSurface(EGLNativeWindowType window) {
        EGLint surfaceAttribs[] = {
                EGL_NONE
        };
        EGLSurface eglSurface = eglCreateWindowSurface(mDisplay, mEglConfig, window,
                                                       surfaceAttribs);
        if (eglSurface == nullptr) {
            LOG_ERROR << "create window error:" << eglGetError();
            return nullptr;
        }

        int width;
        int height;
        if (!eglQuerySurface(mDisplay, eglSurface, EGL_WIDTH, &width) ||
            !eglQuerySurface(mDisplay, eglSurface, EGL_HEIGHT, &height)) {
            LOG_ERROR << "allocate surface error" << eglGetError();
            return nullptr;
        }

        checkEGLError("createWindowSurface");
        LOG_INFO << "create window surface success,mWidth=" << width << ",mHeight=" << height;
        return eglSurface;
    }

    EGLSurface EGLCore::createPBufferSurface() {
        EGLint surfaceAttribs[] = {
                EGL_WIDTH, 1,
                EGL_HEIGHT, 1,
                EGL_NONE
        };
        EGLSurface eglSurface = eglCreatePbufferSurface(mDisplay, mEglConfig, surfaceAttribs);
        if (eglSurface == nullptr) {
            LOG_ERROR << "create pBuffer error:" << eglGetError();
            return nullptr;
        }

        int width;
        int height;
        if (!eglQuerySurface(mDisplay, eglSurface, EGL_WIDTH, &width) ||
            !eglQuerySurface(mDisplay, eglSurface, EGL_HEIGHT, &height)) {
            LOG_ERROR << "allocate surface error" << eglGetError();
            return nullptr;
        }

        checkEGLError("createPBufferSurface");
        LOG_INFO << "create PBuffer surface success,mWidth=" << width << ",mHeight=" << height;
        return eglSurface;
    }

    bool EGLCore::makeCurrentContext(EGLSurface eglSurface) {
        if (eglSurface == nullptr) {
            LOG_ERROR << "eglSurface == null,make context fail";
            return false;
        }
        if (mCurrentSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay,mCurrentSurface);
            mCurrentSurface = EGL_NO_SURFACE;
        }
        if (!eglMakeCurrent(mDisplay, eglSurface, eglSurface, mEglContext)) {
            LOG_ERROR << "make current context error:" << eglGetError();
            return false;
        }
        mCurrentSurface = eglSurface;
        checkEGLError("makeCurrentContext");
        LOG_DEBUG << "make current eglSurface success";
        return true;
    }

    void EGLCore::swapBuffer() {
        eglSwapBuffers(mDisplay, mCurrentSurface);
    }

    EGLInfo EGLCore::getEGLInfo() {
        return {mDisplay,mEglContext,mCurrentSurface,mEglConfig};
    }

    void EGLCore::release() {
        GL_CODE(eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT))
        GL_CODE(eglDestroySurface(mDisplay,mCurrentSurface))
        GL_CODE(eglDestroyContext(mDisplay,mEglContext))
        GL_CODE(eglReleaseThread())
        GL_CODE(eglTerminate(mDisplay))
        mDisplay = EGL_NO_DISPLAY;
        mEglContext = EGL_NO_CONTEXT;
        mCurrentSurface = EGL_NO_SURFACE;
        LOG_DEBUG << "release eglCore finish";
    }

    bool EGLCore::makeCurrentPBufferContext(int width, int height) {
        EGLint surfaceAttribs[] = {
                EGL_WIDTH, width,
                EGL_HEIGHT, height,
                EGL_NONE
        };
        // 创建surface
        CHECK_GL_CODE(EGLSurface eglSurface = eglCreatePbufferSurface(mDisplay, mEglConfig, surfaceAttribs);)
        if (eglSurface == nullptr) {
            LOG_ERROR << "create pBuffer error:" << eglGetError();
            LOG_ERROR << "pBuffer size = " << width << "x" << height;
            return false;
        }

        // 检查是否分配内存成功
        int _width;
        int _height;
        if (!eglQuerySurface(mDisplay, eglSurface, EGL_WIDTH, &width) ||
            !eglQuerySurface(mDisplay, eglSurface, EGL_HEIGHT, &height)) {
            LOG_ERROR << "allocate surface error" << eglGetError();
            return false;
        }

        CHECK_GL_ERROR(createPBufferSurface)
        LOG_DEBUG << "create PBuffer surface success,mWidth=" << width << ",mHeight=" << height;
        return makeCurrentContext(eglSurface);
    }

    bool EGLCore::makeCurrentWindowContext(EGLNativeWindowType window) {
        EGLint surfaceAttribs[] = {
                EGL_NONE
        };
        // 创建surface
        CHECK_GL_CODE(EGLSurface eglSurface = eglCreateWindowSurface(mDisplay, mEglConfig, window,
                                                                     surfaceAttribs);)
        if (eglSurface == nullptr) {
            LOG_ERROR << "create window error:" << eglGetError();
            return false;
        }
        // 检查内存是否分配成功
        int width;
        int height;
        if (!eglQuerySurface(mDisplay, eglSurface, EGL_WIDTH, &width) ||
            !eglQuerySurface(mDisplay, eglSurface, EGL_HEIGHT, &height)) {
            LOG_ERROR << "allocate surface error" << eglGetError();
            return false;
        }

        CHECK_GL_ERROR(createWindowSurface)
        LOG_DEBUG << "create window surface success,mWidth=" << width << ",mHeight=" << height;
        return makeCurrentContext(eglSurface);
    }


}






