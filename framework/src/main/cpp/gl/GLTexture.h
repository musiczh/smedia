//
// Created by wansu on 2022/1/14.
//

#ifndef SMEDIA_GLTEXTURE_H
#define SMEDIA_GLTEXTURE_H
#include "Texture.h"

/**
 * 纹理包装类，有两种使用方式：
 * 1. 包装已经存在的纹理id，一般用于包装外部传进来的纹理，这种纹理一般不需要我们自动释放，也不需要复用，需要调用setAutoOption来关闭
 * 2. 自动创建一个纹理，并在使用完成之后交给纹理池复用，纹理池释放的时候会自动释放纹理。
 * 正常的functor渲染需要渲染到一个fbo中，可以结合GLBufferFrame使用GLTexture来创建新的纹理，使用完成之后自动回收释放
 *
 */
namespace smedia {
    class GLTexture;
    using GLTextureRef = std::shared_ptr<GLTexture>;

    class GLTexture {
    public:
        // 静态方法构建GLTexture
        static GLTextureRef Create(GLContext* glContext,int width,int height,
                                   TextureType type, int textureId = 0);
        // 静态方法构建GLTexture,主要用于构建已经存在的纹理，默认不自动释放和回收
        static GLTextureRef Create(GLContext* glContext,TextureType type, int textureId);

    public:
        GLTexture(GLContext* glContext,int width,int height,TextureType type, int textureId);
        ~GLTexture();

    public:
        /**
         * 设置数据给纹理。一般用于读取外部图片数据并传输到纹理中
         * @param format pixel的数据类型
         * @param pixelData 数据
         * @return 是否输入成功
         */
        void setPixelData(PixelFormat format,const unsigned char* pixelData);
        // 设置是否自动释放纹理；有些纹理不需要自动释放，例如外部的oes纹理由外部负责，这里可以进行设置
        /**
         * 设置自动属性,默认是自动释放和回收
         * @param autoRelease 是否自动释放
         * @param autoReuse 是否回收复用
         */
        void setAutoOption(bool autoRelease,bool autoReuse);
        void bind();
        void unbind();

        int getWidth();
        int getHeight();
        unsigned int getTextureId();

    private:
        GLContext* mGLContext;
        TextureRef mTexture;
        volatile bool mAutoReuse;

        int mWidth;
        int mHeight;
        unsigned int mTextureId;
    };


}



#endif //SMEDIA_GLTEXTURE_H
