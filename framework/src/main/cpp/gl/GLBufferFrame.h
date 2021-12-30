//
// Created by wansu on 2021/12/28.
//

#ifndef SMEDIA_GLBUFFERFRAME_H
#define SMEDIA_GLBUFFERFRAME_H
#include "RenderCore.h"
/**
 * 封装了FBO相关的操作，简化上层逻辑.操作必须放在渲染线程
 */
namespace smedia {
    class GLBufferFrame {
    public:
        GLBufferFrame(RenderCore* renderCore);
        void bindTexture(unsigned int textureId);
        void createTextureAttach(int width,int height);
        int getWidth();
        int getHeight();
        unsigned int getTextureId();
        unsigned int getFBOId();
        ~GLBufferFrame();

    private:
        int mWidth;
        int mHeight;
        RenderCore* mRenderCore;
        unsigned int mFBO;
        unsigned int mTextureId;
    };
}



#endif //SMEDIA_GLBUFFERFRAME_H
