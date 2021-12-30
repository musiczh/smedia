//
// Created by wansu on 2021/12/7.
//
#include "glUtil.h"
namespace smedia {
    // 返回true表示有错误，false表示正常
    
    bool checkEGLError(const std::string &operation) {
        int state = eglGetError();
        if (state != EGL_SUCCESS) {
            LOG_ERROR << operation << " error,code = " << state;
            return true;
        }
        return false;
    }

    bool checkGLError(const std::string &operation) {
        int state = glGetError();
        if (state != GL_NO_ERROR) {
            LOG_ERROR << operation << " error,code = " << state;
            return true;
        }
        return false;
    }
}
