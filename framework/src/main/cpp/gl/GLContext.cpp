//
// Created by wansu on 2021/12/12.
//

#include "GLContext.h"
#include "GLTexturePool.h"
#include "ObjectRegister.h"

namespace smedia {
    void GLContext::init(Data data) {
        EGLContext eglContext;
        long eglContextHandle;
        if (data.isEmpty() || !data.getData(eglContextHandle)) {
            init(nullptr);
            return;
        }
        eglContext = reinterpret_cast<EGLContext>(eglContextHandle);
        init(eglContext);
    }

    void GLContext::init(EGLContext shareContext) {
        if (shareContext == nullptr) {
            LOG_DEBUG << "create GLContext with no shareContext";
        }
        mGLThread = std::unique_ptr<GLThread>(new GLThread);
        mGLTexturePool = std::unique_ptr<GLTexturePool>(new GLTexturePool(this));
        runInRenderThread([this,shareContext]()->bool{
            // 初始化egl环境
            auto *eglCore = new EGLCore;
            eglCore->initEGL(shareContext);
            EGLSurface surface = eglCore->createPBufferSurface();
            eglCore->makeCurrentContext(surface);
            mEglCore = std::unique_ptr<EGLCore>(eglCore);
            return true;
        });
        LOG_DEBUG << "GL_Context init success";
    }


    bool GLContext::runInRenderThread(std::function<bool()> task) {
        return mGLThread->runSync(std::move(task));
    }

    void GLContext::runInRenderThreadV(const std::function<void()>& task) {
        mGLThread->runSync([task]()->bool{
            task();
            return true;
        });
    }

    EGLCore* GLContext::getEglCore() {
        return mEglCore.get();
    }

    EGLInfo GLContext::getEglInfo() {
        return mEglCore->getEGLInfo();
    }

    GLContext::~GLContext() {
        // 析构的时候释放egl环境
        mGLTexturePool->release();
        mGLThread->runSync([this]()->bool{
            mEglCore->release();
            return true;
        });
        LOG_DEBUG << "destroy GLContext";
    }

    GLTexturePool *GLContext::getGLTexturePool() {
        return mGLTexturePool.get();
    }

    GLContext::GLContext() {
        LOG_DEBUG << "create gl context";
    }

    bool GLContext::init(OptionMap options) {
        Data data;
        if (options.find("EGLSharedContext") != options.end()) {
            data = options["EGLSharedContext"];
        }
        EGLContext eglContext;
        long eglContextHandle;
        if (data.isEmpty() || !data.getData(eglContextHandle)) {
            init(nullptr);
        } else {
            eglContext = reinterpret_cast<EGLContext>(eglContextHandle);
            init(eglContext);
        }
        return true;
    }

    REGISTER_CLASS(GLContext)

}

