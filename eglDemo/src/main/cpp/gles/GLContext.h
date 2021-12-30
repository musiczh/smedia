//
// Created by wansu on 2021/12/10.
//

#ifndef SMEDIA_GLCONTEXT_H
#define SMEDIA_GLCONTEXT_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <memory>
#include "EGLCore.h"
#include "RenderCore.h"

struct GLContext {
    std::unique_ptr<EGLCore> eglCore;
    std::unique_ptr<RenderCore> renderCore;
};


#endif //SMEDIA_GLCONTEXT_H
