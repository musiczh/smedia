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
    };
}



#endif //SMEDIA_GLFRAME_H
