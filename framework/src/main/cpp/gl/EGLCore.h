//
// Created by wansu on 2021/12/3.
//

#ifndef SMEDIA_EGLCORE_H
#define SMEDIA_EGLCORE_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <memory>
#include "GLHelper.h"
#include "Logger.h"
#include "Program.h"

/**
 * 管理EGL环境相关。此类中包含了EGL相关的资源配置，如context、display、surface等
 * 注意在gl线程结束前需要调用release来释放资源
 *
 * 不与线程GLThread绑定的原因是一个线程可以绑定不同的context、而且外部可能有自己的独立渲染线程
 */
namespace smedia {
    struct EGLInfo {
        EGLDisplay eglDisplay;
        EGLContext eglContext ;
        EGLSurface eglSurface ;
        EGLConfig eglConfig;
    };

    class EGLCore {
    public:
        EGLCore();

    public:
        bool initEGL(EGLContext shareContext);
        // 释放egl资源，需要在egl线程结束前调用，否则会造成内存泄露
        void release();
        bool makeCurrentPBufferContext(int width,int height);
        bool makeCurrentWindowContext(EGLNativeWindowType window);
        void swapBuffer();
        EGLInfo getEGLInfo();

        // todo 废弃接口，后续去除掉
        EGLSurface createWindowSurface(EGLNativeWindowType window);
        EGLSurface createPBufferSurface();
        bool makeCurrentContext(EGLSurface eglSurface);

    private:
        EGLDisplay mDisplay;
        EGLContext mEglContext;
        EGLSurface mCurrentSurface;
        EGLConfig mEglConfig{nullptr};
    };

    using EGLCoreRef = std::shared_ptr<EGLCore>;

}


#endif //SMEDIA_EGLCORE_H
