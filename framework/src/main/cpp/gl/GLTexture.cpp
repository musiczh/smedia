//
// Created by wansu on 2022/1/14.
//

#include "GLTexture.h"
#include "GLTexturePool.h"
namespace smedia {

    GLTexture::GLTexture(GLContext* glContext, int width, int height,TextureType type, int textureId):
    mGLContext(glContext),mWidth(width),mHeight(height),mAutoReuse(true) {
        mTexture = mGLContext->getGLTexturePool()->obtainTexture(width, height, type, textureId);
        mTextureId = mTexture->mTextureId;
    }

    GLTexture::~GLTexture() {
        if (mAutoReuse) {
            mGLContext->getGLTexturePool()->recycleTexture(mTexture);
        }
    }

    void GLTexture::setPixelData(PixelFormat format,const unsigned char *pixelData) {
        mTexture->setPixelData(format, pixelData);
    }

    int GLTexture::getWidth() {
        return mWidth;
    }

    int GLTexture::getHeight() {
        return mHeight;
    }

    unsigned int GLTexture::getTextureId() {
        return mTexture->mTextureId;
    }

    GLTextureRef GLTexture::Create(GLContext* glContext,int width,int height,
                                   TextureType type, int textureId) {
        return std::make_shared<GLTexture>(glContext,width,height,type,textureId);
    }

    GLTextureRef GLTexture::Create(GLContext* glContext,TextureType type, int textureId) {
        auto* glTexture = new GLTexture(glContext,0,0,type,textureId);
        glTexture->setAutoOption(false, false);
        return GLTextureRef (glTexture);
    }

    void GLTexture::bind() {
        mGLContext->runInRenderThreadV([this](){
           mTexture->bind();
        });
    }

    void GLTexture::unbind() {
        mGLContext->runInRenderThreadV([this](){
            mTexture->unbind();
        });
    }

    void GLTexture::setAutoOption(bool autoRelease, bool autoReuse) {
        mTexture->setAutoRelease(autoRelease);
        mAutoReuse = autoReuse;
    }

}