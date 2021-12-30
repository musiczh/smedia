//
// Created by wansu on 2021/12/12.
//

#ifndef SMEDIA_GLFRAME_H
#define SMEDIA_GLFRAME_H
#include "GLContext.h"
#include "IFrame.h"
namespace smedia {
    class GLFrame : public IFrame {
    public:
        unsigned int textureId;
    };
}



#endif //SMEDIA_GLFRAME_H
