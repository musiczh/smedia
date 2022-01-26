//
// Created by wansu on 2021/12/12.
//

#ifndef SMEDIA_GLFRAME_H
#define SMEDIA_GLFRAME_H
#include "GLContext.h"
#include "IFrame.h"
#include "GLTexture.h"
namespace smedia {
    class GLFrame : public IFrame {
    public:
        // 纹理对象共享指针
        GLTextureRef glTextureRef;
        // 这里补充textured的原因是，在JNI场景下没有GLContext无法构建GLTexture，采用此字段来传递纹理id
        unsigned int textureId;
    };
}



#endif //SMEDIA_GLFRAME_H
