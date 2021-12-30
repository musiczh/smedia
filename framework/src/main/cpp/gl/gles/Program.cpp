//
// Created by wansu on 2021/11/29.
//

#include "Program.h"
namespace smedia {
    Program::Program() {
        mProgram = glCreateProgram();
    }

    bool Program::addShader(unsigned int shaderType, const std::string &code) {
        Shader shader(code, shaderType);
        if (!shader.compile()) {
            return false;
        }
        glAttachShader(mProgram, shader.get());
        return true;
    }

    bool Program::link() {
        int success;
        char msg[512];
        glLinkProgram(mProgram);
        glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(mProgram, 512, nullptr, msg);
            const std::string s(msg);
            LOG_ERROR << "error:program link error. msg: " ;
            LOG_ERROR << s;
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
        glUniform1i(glGetUniformLocation(mProgram, name.c_str()), value);
    }

    void Program::setFloat(const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(mProgram,name.c_str()),value);
    }

    void Program::setMat4(const std::string &name,const float *value) {
        glUniformMatrix4fv(glGetUniformLocation(mProgram,name.c_str()),1,false,value);
    }


}

