//
// Created by wansu on 2021/11/29.
//

#include "Shader.h"

namespace smedia {
    Shader::Shader(const std::string &code,unsigned int shaderType) {
        mShaderCode = code;
        type = shaderType;
        mShader = glCreateShader(shaderType);
        const char* shaderCode = mShaderCode.c_str();
        glShaderSource(mShader,1,&shaderCode, nullptr);
    }

    bool Shader::compile() {
        glCompileShader(mShader);
        int success;
        glGetShaderiv(mShader,GL_COMPILE_STATUS,&success);
        if (!success) {
            glGetShaderInfoLog(mShader,512,NULL,info);
            LOG_ERROR << "compile " << type << " shader error. msg: " << info;
            return false;
        }
        LOG_DEBUG << "compile shader success";
        return true;
    }


    Shader::~Shader() {
        glDeleteShader(mShader);
    }

    unsigned int Shader::get() {
        return mShader;
    }
}

