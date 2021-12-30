//
// Created by wansu on 2021/11/29.
//

#ifndef LEARNOPENGL_PROGRAM_H
#define LEARNOPENGL_PROGRAM_H
#include "Shader.h"
#include "Logger.h"

namespace smedia {
    class Program {
    public:
        Program();

        // 内部处理日志的打印
        bool addShader(unsigned int shaderType, const std::string &code);

        bool link();

        unsigned int get();

        void use();

        void setInt(const std::string &name, int value);
        void setFloat(const std::string &name, float value);
        void setMat4(const std::string &name,const float* value);

    private:
        unsigned int mProgram;
        char info[512];
    };
}


#endif //LEARNOPENGL_PROGRAM_H
