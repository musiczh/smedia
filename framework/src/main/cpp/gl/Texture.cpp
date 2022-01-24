//
// Created by wansu on 2022/1/11.
//

#include "Texture.h"
#include "GLContext.h"
#include <GLES2/gl2ext.h>

namespace smedia {

    Texture::Texture(GLContext* glContext, int width, int height,
                     TextureType type, unsigned int textureId)
    : mGLContext(glContext), mWidth(width), mHeight(height),mTextureType(type),mAutoRelease(true) {
        if (textureId == 0) {
            mGLContext->runInRenderThreadV([this,type](){
                mTextureId = createTexture(type,mWidth,mHeight, nullptr);
                LOG_DEBUG << "create tex:" << mTextureId;
            });
        } else {
            mTextureId = textureId;
        }
    }

    Texture::Texture(GLContext* glContext, int width, int height, TextureType textureType,
                     PixelFormat format, const unsigned char* pixelData)
            : mGLContext(glContext), mWidth(width), mHeight(height),mTextureType(textureType){
        // todo 这里暂时只处理RGBA format，后续需要支持更多的format
        mGLContext->runInRenderThreadV([this,pixelData,textureType](){
            mTextureId = createTexture(textureType,mWidth,mHeight,pixelData);
        });
    }

    Texture::~Texture() {
        if (mAutoRelease) {
            mGLContext->runInRenderThreadV([this](){
                DeleteGLTexture(mTextureId);
                LOG_DEBUG << "delete texture" << mTextureId;
            });
        }
    }


    void Texture::setPixelData(PixelFormat format, const unsigned char *pixelData) {
        // todo 这里暂时只处理RGBA format，后续需要支持更多的format
        bind();
        mGLContext->runInRenderThreadV([this,pixelData](){
            // 填充图像数据
            GL_CODE(glTexSubImage2D(GL_TEXTURE_2D,0,0,0,mWidth,mHeight,GL_RGBA,GL_UNSIGNED_BYTE,pixelData))
        });
        unbind();
    }

    void Texture::bind() {
        mGLContext->runInRenderThreadV([this](){
            //GLenum type = getGLTextureType(mTextureType);
            switch (mTextureType) {
                case TEXTURE_TYPE_2D:
                    GL_CODE(glBindTexture(GL_TEXTURE_2D,mTextureId))
                    break;
                case TEXTURE_TYPE_OES:
                    GL_CODE(glBindTexture(GL_TEXTURE_EXTERNAL_OES,mTextureId))
                    break;
            }
        });
    }

    void Texture::unbind() {
        mGLContext->runInRenderThreadV([this](){
            GL_CODE(glBindTexture(getGLTextureType(mTextureType),0))
        });
    }

    inline GLint Texture::getGLTextureType(TextureType type) {
        switch (type) {
            case TEXTURE_TYPE_2D: return GL_TEXTURE_2D;
            case TEXTURE_TYPE_OES: return GL_TEXTURE_EXTERNAL_OES;
        }
    }

    inline unsigned int Texture::createTexture(TextureType type,int width,int height,
                                               const unsigned char* pixelData) {
        switch (type) {
            case TEXTURE_TYPE_2D: return Create2DGLTexture(width, height, pixelData);
            case TEXTURE_TYPE_OES: return CreateOESTexture(width, height);
        }
    }

    void Texture::setAutoRelease(bool autoReleaseTex) {
        mAutoRelease = autoReleaseTex;
    }


}