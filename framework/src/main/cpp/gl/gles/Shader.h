//
// Created by wansu on 2021/11/29.
//

#ifndef LEARNOPENGL_SHADER_H
#define LEARNOPENGL_SHADER_H
#include <GLES3/gl3.h>
#include "Logger.h"
namespace smedia {
    class Shader {
    public:
        explicit Shader(const std::string &code, unsigned int shaderType);

        ~Shader();

        bool compile();

        unsigned int get();

    private:
        std::string mShaderCode;
        unsigned int type;
        char info[512];
        unsigned int mShader;
    };
}


#endif //LEARNOPENGL_SHADER_H
