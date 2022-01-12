//
// Created by wansu on 2021/12/3.
//

#ifndef SMEDIA_EGLCORE_H
#define SMEDIA_EGLCORE_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <memory>
#include "glUtil.h"
#include "Logger.h"
#include "Program.h"

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
        void release();

        EGLSurface createWindowSurface(EGLNativeWindowType window);

        EGLSurface createPBufferSurface();

        bool makeCurrentContext(EGLSurface eglSurface);

        void swapBuffer();

        EGLInfo getEGLInfo();

    private:
        EGLDisplay mDisplay;
        EGLContext mEglContext;
        EGLSurface mCurrentSurface;
        EGLConfig mEglConfig{nullptr};
    };

}


#endif //SMEDIA_EGLCORE_H
