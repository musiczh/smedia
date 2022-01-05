//
// Created by wansu on 2021/12/11.
//

#ifndef SMEDIA_IMAGEFRAME_H
#define SMEDIA_IMAGEFRAME_H
#include "IFrame.h"
#include <jni.h>
#include <memory>
/**
 * cpu数据帧
 */
namespace smedia {
    class ImageFrame : public IFrame {
    public:
        ImageFrame(int width,int height,uint8_t* pixelBuffer,FrameFormat format);

    public:
        std::unique_ptr<uint8_t[]> pixelBuffer;
    };
}



#endif //SMEDIA_IMAGEFRAME_H
