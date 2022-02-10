//
// Created by wansu on 2021/12/12.
//

#ifndef SMEDIA_GLCONTEXT_H
#define SMEDIA_GLCONTEXT_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <memory>
#include "ServiceRegister.h"
#include "EGLCore.h"
#include "GLThread.h"
#include "Data.h"


/**
 * 渲染上下文，封装了和渲染相关的内容，包括egl、渲染线程
 * GLContext本身不可被拷贝，使用GLContextRef来进行共享
 */
namespace smedia {
    class GLTexturePool;

    class GLContext : public IService{
    public:
        GLContext();
        ~GLContext();

        bool init(OptionMap options) override;

        void init(EGLContext eglContext);
        // todo 读取Data中的EGLContext，这个接口后续剥离更好，不然和外部耦合了
        void init(Data data);

        bool runInRenderThread(std::function<bool()> task);
        // 不需要返回值的方法
        void runInRenderThreadV(const std::function<void()>& task);

        EGLCore* getEglCore();
        // 纹理池
        GLTexturePool* getGLTexturePool();
        EGLInfo getEglInfo();

    private:
        std::unique_ptr<EGLCore> mEglCore;
        std::unique_ptr<GLThread> mGLThread;
        std::unique_ptr<GLTexturePool> mGLTexturePool;
    };

    // 全局采用share指针，防止glContext提前被释放
    using GLContextRef = std::shared_ptr<GLContext>;
}




#endif //SMEDIA_GLCONTEXT_H
