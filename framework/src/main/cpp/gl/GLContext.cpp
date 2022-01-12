//
// Created by wansu on 2021/12/12.
//

#include "GLContext.h"

#include <utility>

namespace smedia {
    void GLContext::init(Data data) {
        EGLContext eglContext{};
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
        mGLThread = std::shared_ptr<GLThread>(new GLThread);
        runInRenderThread([this,shareContext]()->bool{
            auto *eglCore = new EGLCore;
            eglCore->initEGL(shareContext);
            EGLSurface surface = eglCore->createPBufferSurface();
            eglCore->makeCurrentContext(surface);

            auto* renderCore = new RenderCore;
            renderCore->initGLES();

            mEglCore = std::shared_ptr<EGLCore>(eglCore);
            mRenderCore = std::shared_ptr<RenderCore>(renderCore);
            return true;
        });
        LOG_DEBUG << "GL_Context init success";
    }


    bool GLContext::runInRenderThread(std::function<bool()> task) {
        return mGLThread->runSync(std::move(task));
    }

    EGLCore *GLContext::getEglCore() {
        return mEglCore.get();
    }

    RenderCore *GLContext::getRenderCore() {
        return mRenderCore.get();
    }



    EGLInfo GLContext::getEglInfo() {
        return mEglCore->getEGLInfo();
    }

    GLContext::~GLContext() {
        LOG_DEBUG << "destroy glContext";
    }

    void GLContext::release() {
        LOG_DEBUG << "glContext release";
        mGLThread->runSync([this]()->bool{
            mEglCore->release();
            return true;
        });
    }


}

