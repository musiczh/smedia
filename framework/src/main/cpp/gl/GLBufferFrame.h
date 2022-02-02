//
// Created by wansu on 2021/12/28.
//

#ifndef SMEDIA_GLBUFFERFRAME_H
#define SMEDIA_GLBUFFERFRAME_H
#include <memory>
#include "GLContext.h"
#include "GLTexture.h"
/**
 * 封装了FBO相关的操作，简化上层逻辑。包含了纹理和FBO的操作
 */
namespace smedia {
    class GLContext;
    class GLBufferFrame;

    class GLBufferFrame {
    public:
        /**
         * 构造一个frameBuffer对象
         * @param glContext
         * @param viewPort 视口，必须为一个长度为4的int数组
         * @return
         */
        static std::unique_ptr<GLBufferFrame> Create(GLContext* glContext,const int *viewPort);

    public:
        unsigned int getFBOId() const;

        // 设置viewPort和纹理
        void setViewPort(const int *viewPort);
        const int* getViewPort();

        void setTexture(GLTextureRef glTexture);
        GLTextureRef getTexture();

        /**
         * 绑定fbo以及绑定内部纹理到fbo中,默认会自动创建纹理(2D)，且长宽和viewPort一致
         * @param createTex 是否自动创建纹理，默认自动创建
         * @param width 纹理宽度
         * @param height 纹理高度
         */
        void bind(bool createTex = true,int width = 0,int height = 0);

        /**
         * 解绑纹理
         * @param releaseTex 是否释放内部纹理
         */
        GLTextureRef unBind(bool releaseTex = true);

        // 读取当前渲染缓冲中的RGB数据
        // 如果没有输入宽度和高度则采用viewPort的尺寸来进行读取
        unsigned char* readRGBAPixelData(int width = -1,int height = -1);

    private:
        void createTexInner(int width,int height);


    private:
        // 构造器私有，通过静态方法来创建，方便后续提供更多的构造器
        GLBufferFrame(GLContext* glContext,const int *viewPort);


    private:
        unsigned int mFBO{0};
        GLTextureRef mGLTexture;
        GLContext* mGLContext;
        int mViewPort[4]{0,0,0,0};
    };


}



#endif //SMEDIA_GLBUFFERFRAME_H
