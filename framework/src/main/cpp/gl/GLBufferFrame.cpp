//
// Created by wansu on 2021/12/28.
//

#include "GLBufferFrame.h"

#include <utility>
namespace smedia {
    std::unique_ptr<GLBufferFrame>
    GLBufferFrame::Create(GLContext *glContext, int width, int height) {
        std::unique_ptr<GLBufferFrame> bufferFrame = std::unique_ptr<GLBufferFrame>(
                new GLBufferFrame(glContext,width,height));
        return bufferFrame;
    }

    void GLBufferFrame::setTexture(GLTextureRef glTexture) {
        mGLTexture = std::move(glTexture);
    }

    GLBufferFrame::GLBufferFrame(GLContext *glContext, int width, int height) {
        mGLContext = glContext;
        mWidth = width;
        mHeight = height;
        // 创建FBO
        mGLContext->runInRenderThreadV([this](){
            CHECK_GL_CODE(glGenFramebuffers(1,&mFBO);)
        });
    }

    void GLBufferFrame::bind(bool createTex,int width, int height) {
        if (createTex) {
            mGLTexture.reset();
            if (width > 0 && height > 0) {
                mWidth = width;
                mHeight = height;
            }
            createTexInner(mWidth,mHeight);
        }

        mGLTexture->bind();
        mGLContext->runInRenderThreadV([this](){
            // 绑定纹理和fbo
            GL_CODE(glBindFramebuffer(GL_FRAMEBUFFER,mFBO))
            if (mGLTexture != nullptr) {
                GL_CODE(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                     GL_TEXTURE_2D, mGLTexture->getTextureId(), 0))
            } else {
                LOG_ERROR << "the texture in GLBufferFrame is null";
            }
        });
    }

    void GLBufferFrame::createTexInner(int width, int height) {
        mGLTexture = GLTexture::Create(mGLContext, width, height, TEXTURE_TYPE_2D);
    }

    GLTextureRef GLBufferFrame::unBind(bool releaseTex) {
        // mGLTexture有可能被释放，这里先保存起来
        GLTextureRef res = mGLTexture;
        mGLContext->runInRenderThreadV([this,releaseTex](){
            CHECK_GL_CODE(glBindFramebuffer(GL_FRAMEBUFFER,0);)
            if (releaseTex) {
                // 释放纹理
                mGLTexture->unbind();
                mGLTexture.reset();
            }
        });
        return res;
    }

    unsigned int GLBufferFrame::getFBOId() const {
        return mFBO;
    }

    GLTextureRef GLBufferFrame::getTexture() {
        return mGLTexture;
    }

    unsigned char *GLBufferFrame::readRGBAPixelData(int width,int height) const{
        if (width == -1 && height == -1) {
            width = mWidth;
            height = mHeight;
        }
        auto *buffer = new uint8_t [width*height*4];
        CHECK_GL_CODE(glPixelStorei(GL_PACK_ALIGNMENT,4);)
        CHECK_GL_CODE(glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,buffer);)
        return buffer;
    }

    void GLBufferFrame::setSize(int width, int height) {
        mWidth = width;
        mHeight = height;
    }
}

