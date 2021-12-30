//
// Created by wansu on 2021/12/12.
//

#ifndef SMEDIA_GLCONTEXT_H
#define SMEDIA_GLCONTEXT_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <memory>
#include <thread>
#include "EGLCore.h"
#include "RenderCore.h"
#include "GLThread.h"
#include "Data.h"
/**
 * 渲染上下文，封装了和渲染相关的内容，包括egl、gles、渲染线程
 *
 */
namespace smedia {
    class GLContext {
    public:
        GLContext() = default;
        void init(EGLContext eglContext);
        void init(Data data);
        ~GLContext() = default;
        bool runInRenderThread(std::function<bool()> task);

        EGLCore* getEglCore();
        RenderCore* getRenderCore();
        EGLInfo getEglInfo();

    private:
        // 使用share指针，可以让多个GLContext之间共享环境
        std::shared_ptr<EGLCore> mEglCore;
        std::shared_ptr<RenderCore> mRenderCore;
        std::shared_ptr<GLThread> mGLThread;
    };
}




#endif //SMEDIA_GLCONTEXT_H
