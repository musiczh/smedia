//
// Created by wansu on 2021/12/3.
//

#include "EGLCore.h"


EGLCore::EGLCore() {
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
    if (!eglInitialize(mDisplay,&majorVersion,&minVersion)) {
        LOG_ERROR << "egl init error:" << eglGetError();
        return false;
    }

    EGLint attribList[] = {
            EGL_NONE
    };
    const int maxConfigs = 1;
    int numConfigs;
    EGLConfig eglConfigs[1];
    if (!eglChooseConfig(mDisplay,attribList,eglConfigs,maxConfigs,&numConfigs)) {
        LOG_ERROR << "choose config error" << eglGetError();
        return false;
    }
    mEglConfig = eglConfigs[0];

    int attrib3_list[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    mEglContext = eglCreateContext(mDisplay,mEglConfig,shareContext,attrib3_list);
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
    EGLSurface  eglSurface = eglCreateWindowSurface(mDisplay,mEglConfig,window, surfaceAttribs);
    if (eglSurface == nullptr) {
        LOG_ERROR << "create window error:" << eglGetError();
        return nullptr;
    }

    int width;
    int height;
    if (!eglQuerySurface(mDisplay,eglSurface,EGL_WIDTH,&width) ||
        !eglQuerySurface(mDisplay,eglSurface,EGL_HEIGHT,&height)) {
        LOG_ERROR << "allocate surface error" << eglGetError();
        return nullptr;
    }

    checkEGLError("createWindowSurface");
    LOG_INFO << "create window surface success,width=" << width << ",height=" << height;
    return eglSurface;
}

EGLSurface EGLCore::createPBufferSurface() {
    EGLint surfaceAttribs[] = {
            EGL_NONE
    };
    EGLSurface  eglSurface = eglCreatePbufferSurface(mDisplay,mEglConfig,surfaceAttribs);
    if (eglSurface == nullptr) {
        LOG_ERROR << "create pBuffer error:" << eglGetError();
        return nullptr;
    }

    int width;
    int height;
    if (!eglQuerySurface(mDisplay,eglSurface,EGL_WIDTH,&width) ||
        !eglQuerySurface(mDisplay,eglSurface,EGL_HEIGHT,&height)) {
        LOG_ERROR << "allocate surface error" << eglGetError();
        return nullptr;
    }

    checkEGLError("createPBufferSurface");
    LOG_INFO << "create PBuffer surface success,width=" << width << ",height=" << height;
    return eglSurface;
}

bool EGLCore::makeCurrentContext(EGLSurface eglSurface) {
    if (eglSurface == nullptr) {
        LOG_ERROR << "eglSurface == null,make context fail";
        return false;
    }
    if (!eglMakeCurrent(mDisplay,eglSurface,eglSurface,mEglContext)){
        LOG_ERROR << "make current context error:" << eglGetError();
        return false;
    }
    mCurrentSurface = eglSurface;
    checkEGLError("makeCurrentContext");
    LOG_DEBUG << "make current eglSurface success";
    return true;
}

std::string fragmentShader = "#version 300 es\n"
                             "#extension GL_OES_EGL_image_external_essl3 : require;\n"
                             "precision mediump float;\n"
                             "out vec4 FragColor;\n"
                             "// 顶点着色传进来的纹理坐标\n"
                             "in vec2 otPos;\n"
                             "\n"
                             "// 采样器，外部可以把纹理赋值给此\n"
                             "uniform samplerExternalOES boxTexture;\n"
                             "\n"
                             "void main(){\n"
                             "    // 传入坐标和采样器，获取颜色并返回\n"
                             "    FragColor = texture(boxTexture,otPos);\n"
                             "}";

void EGLCore::swapBuffer() {
    eglSwapBuffers(mDisplay,mCurrentSurface);
}






