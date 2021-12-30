//
// Created by wansu on 2021/11/29.
//

#include "Program.h"

Program::Program() {
    mProgram = glCreateProgram();
}

bool Program::addShader(unsigned int shaderType, const std::string &code) {
    Shader shader(code,shaderType);
    if (!shader.compile()) {
        return false;
    }
    glAttachShader(mProgram,shader.get());
    return true;
}

bool Program::link() {
    int success;
    glLinkProgram(mProgram);
    glGetProgramiv(mProgram,GL_LINK_STATUS,&success);
    if (!success) {
        glGetProgramInfoLog(mProgram,512, nullptr,info);
        std::cout << "error:program link error. msg: \n" << info << std::endl;
        return false;
    }
    LOG_DEBUG << "link program success";
    return true;
}

void Program::use() {
    glUseProgram(mProgram);
}

unsigned int Program::get() {
    return mProgram;
}

void Program::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(mProgram,name.c_str()),value);
}

