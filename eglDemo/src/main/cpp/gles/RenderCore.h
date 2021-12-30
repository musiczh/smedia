//
// Created by wansu on 2021/12/10.
//

#ifndef SMEDIA_RENDERCORE_H
#define SMEDIA_RENDERCORE_H
#include <string>
#include "Program.h"
#include "../util/util.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>


class RenderCore {
public:
    RenderCore();
    bool initGLES();
    Program* createProgram(const std::string& fragmentShaderSource);
    unsigned int createTexFBO(int width,int height);
    unsigned int createOESTexture();
    unsigned int create2DTexture();
    void draw(int textureType,unsigned int textureId,Program* program);

public:
    unsigned int VAO;
};


#endif //SMEDIA_RENDERCORE_H
