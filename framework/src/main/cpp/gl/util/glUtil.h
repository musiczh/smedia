//
// Created by wansu on 2021/12/10.
//

#ifndef SMEDIA_GLUTIL_H
#define SMEDIA_GLUTIL_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "Logger.h"
namespace smedia {

    bool checkEGLError(const std::string &operation);

    bool checkGLError(const std::string &operation);
}
#endif //SMEDIA_GLUTIL_H
