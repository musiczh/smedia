//
// Created by wansu on 2022/1/11.
//

#ifndef SMEDIA_TEXTURE_H
#define SMEDIA_TEXTURE_H
#include <memory>
#include "GLContext.h"
#include "GLHelper.h"
/**
 * 此类的设计主要是为了能够释放纹理资源，防止内存泄露。本身负责申请构建纹理以及自动释放纹理
 * 外部使用GLTextureRef，利用共享指针在合适的时机释放
 * 这里没有进行2D和OES纹理区分，统一为纹理
 */
namespace smedia {
    // 给纹理设置pixel时指定的像素类型
    enum PixelFormat {
        PIXEL_RGB,PIXEL_RGBA,
    };
    enum TextureType {
        TEXTURE_TYPE_2D,
        TEXTURE_TYPE_OES
    };

    class Texture {
    public:
        /**
         * 构建一个纹理对象
         * @param glContext
         * @param width
         * @param height
         * @param textureId 默认自动申请一个纹理，当textureId不为0，则不申请纹理，使用存在的纹理构建对象
         */
        Texture(GLContext* glContext, int width, int height, TextureType textureType,
                unsigned int textureId = 0);

        // 构建一个纹理并传递数据
        Texture(GLContext* glContext, int width, int height, TextureType textureType,
                PixelFormat format, const unsigned char* pixelData);

        ~Texture();

        void setPixelData(PixelFormat format,const unsigned char* pixelData);
        void setAutoRelease(bool autoReleaseTex);
        void bind();
        void unbind();

    private:
        inline GLint getGLTextureType(TextureType type);
        inline unsigned int createTexture(TextureType type,int width,int height,
                                          const unsigned char* pixelData = nullptr);

    public:
        unsigned int mTextureId{0};
        int mWidth;
        int mHeight;
        TextureType mTextureType;
        GLContext* mGLContext;
        volatile bool mAutoRelease{true};

    };

    using TextureRef = std::shared_ptr<Texture>;
}



#endif //SMEDIA_TEXTURE_H
