//
// Created by wansu on 2021/12/3.
//

#ifndef SMEDIA_EGLCORE_H
#define SMEDIA_EGLCORE_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <memory>
#include "util.h"
#include "Logger.h"
#include "Program.h"


class EGLCore {
public:
    EGLCore();
    bool initEGL(EGLContext shareContext);
    EGLSurface createWindowSurface(EGLNativeWindowType window);
    EGLSurface createPBufferSurface();
    bool makeCurrentContext(EGLSurface eglSurface);
    void swapBuffer();

private:
    EGLDisplay mDisplay;
    EGLContext mEglContext;
    EGLSurface mCurrentSurface;
    EGLConfig mEglConfig;
};


#endif //SMEDIA_EGLCORE_H
