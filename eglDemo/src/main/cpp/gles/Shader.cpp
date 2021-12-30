//
// Created by wansu on 2021/11/29.
//

#include "Shader.h"

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
        std::cout << "compile " << type << " shader error. msg: \n" << info <<std::endl;
    }
    LOG_DEBUG << "compile shader success";
    return success != 0;
}


Shader::~Shader() {
    glDeleteShader(mShader);
}

unsigned int Shader::get() {
    return mShader;
}
