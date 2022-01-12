//
// Created by wansu on 2022/1/11.
//

#ifndef SMEDIA_GLTEXTURE_H
#define SMEDIA_GLTEXTURE_H
#include <memory>
#include "GLContext.h"
/**
 * 此类的设计主要是为了能够释放纹理资源，防止内存泄露
 * 外部使用GLTextureRef，利用共享指针在合适的时机释放
 */
namespace smedia {
    class GLContext;
    class GLTexture {
    public:
        GLTexture(GLContextRef glContext,unsigned int textureId,int width,int height);
        ~GLTexture();

    public:
        unsigned int textureId;
        int width;
        int height;
        GLContextRef glContext;
    };

    using GLTextureRef = std::shared_ptr<GLTexture>;
}



#endif //SMEDIA_GLTEXTURE_H
