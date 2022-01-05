//
// Created by wansu on 2021/12/11.
//

#include "ImageFrame.h"
namespace smedia {

    ImageFrame::ImageFrame(int width, int height, uint8_t *pixelBuffer, FrameFormat format) {
        this->width = width;
        this->height = height;
        this->format = format;
        this->pixelBuffer = std::unique_ptr<uint8_t[]>(pixelBuffer);
    }
}