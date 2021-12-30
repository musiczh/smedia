//
// Created by wansu on 2021/12/16.
//

#ifndef SMEDIA_GLHELPER_H
#define SMEDIA_GLHELPER_H
#include <string>
#include "../../framework/core/FunctorContext.h"
#include "GLContext.h"
namespace smedia {
    extern const std::string EGL_SHARED_CONTEXT;
    extern const std::string GL_CONTEXT;
    extern const std::string NATIVE_WINDOW ;

    extern bool getGLContext(FunctorContext* functorContext,GLContext& result);

}



#endif //SMEDIA_GLHELPER_H
