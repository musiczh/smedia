//
// Created by wansu on 2021/12/10.
//

#ifndef SMEDIA_RENDERCORE_H
#define SMEDIA_RENDERCORE_H
#include <string>
#include "Program.h"
#include "glUtil.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

namespace smedia {
    class RenderCore {
    public:
        RenderCore();
        bool initGLES();
        Program* createProgram(const std::string& fragmentShaderSource);
        unsigned int createOESTexture();
        unsigned int create2DTexture();
        unsigned int create2DTexture(int w,int h);
        void deleteTexture(unsigned int textureId);
        void deleteFBO(unsigned int fboId);
        unsigned int createFrameBuffer();
        void bindTextureInFrameBuffer(unsigned int frameBuffer,unsigned int textureId);

        void draw(int textureType,unsigned int textureId,Program* program,unsigned int fbo = 0);

    public:
        unsigned int VAO;
    };
}



#endif //SMEDIA_RENDERCORE_H
