//
// Created by wansu on 2021/12/16.
//

#include "GLHelper.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include "Logger.h"

namespace smedia {
    const std::string EGL_SHARED_CONTEXT = "EGLSharedContext";
    const std::string GL_CONTEXT = "GLContext";
    const std::string NATIVE_WINDOW = "nativeWindow";


    bool checkEGLError(const std::string &operation) {
        int state = eglGetError();
        if (state != EGL_SUCCESS) {
            LOG_ERROR << "egl error,code = " << state << ",operation= ";
            LOG_ERROR << operation  ;
            return true;
        }
        return false;
    }

    bool checkGLError(const std::string &operation) {
        int state = glGetError();
        if (state != GL_NO_ERROR) {
            LOG_ERROR << "gl error,code = " << state << ",operation= ";
            LOG_ERROR << operation  ;
            return true;
        }
        return false;
    }

    unsigned int Create2DGLTexture(int width, int height, const unsigned char* pixel){
        LOG_DEBUG<< "create GL 2D Texture,size = " << width << "x" << height;
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE, pixel);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (checkGLError("create2DTexture")) {
            return -1;
        }
        LOG_DEBUG << "create texture success,id = " << texture;
        return texture;
    }

    unsigned int CreateOESTexture(int width, int height, const unsigned char* pixel) {
        LOG_DEBUG<< "create OES 2D Texture,size = " << width << "x" << height;
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
        // todo 这里不知道可不可以这样操作
        glTexImage2D(GL_TEXTURE_EXTERNAL_OES,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE, pixel);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
        CHECK_GL_ERROR(createOESTexture)
        LOG_DEBUG << "create texture success";
        return texture;
    }

    void DeleteGLTexture(unsigned int textureId) {
        GLuint array[] = {textureId};
        CHECK_GL_CODE(glDeleteTextures(1,array);)
    }

    void DeleteGLFBO(unsigned int fboId) {
        GLuint array[] = {fboId};
        CHECK_GL_CODE(glDeleteFramebuffers(1,array);)
    }

    unsigned int CreateFrameBuffer() {
        unsigned int frameBufferId;
        CHECK_GL_CODE(glGenFramebuffers(1,&frameBufferId);)
        return frameBufferId;
    }

    void BindTextureInFrameBuffer(unsigned int frameBuffer, unsigned int textureId) {
        CHECK_GL_CODE(glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);)
        CHECK_GL_CODE(glBindTexture(GL_TEXTURE_2D,textureId);)
        CHECK_GL_CODE(glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textureId,0);)
        CHECK_GL_CODE(glBindFramebuffer(GL_FRAMEBUFFER,0);)
        CHECK_GL_CODE(glBindTexture(GL_TEXTURE_2D,0);)
    }

}