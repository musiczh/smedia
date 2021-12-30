//
// Created by wansu on 2021/12/7.
//
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "Logger.h"

bool checkEGLError(const std::string& operation){
    int state = eglGetError();
    if (state != EGL_SUCCESS) {
        LOG_ERROR << operation << " error,code = " << state;
        return false;
    }
    return true;
}

bool checkGLError(const std::string& operation) {
    int state = glGetError();
    if (state != GL_NO_ERROR) {
        LOG_ERROR << operation << " error,code = " << state;
        return false;
    }
    return true;
}
