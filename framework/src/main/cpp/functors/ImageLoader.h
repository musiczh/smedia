//
// Created by wansu on 2021/12/31.
//

#ifndef SMEDIA_IMAGELOADER_H
#define SMEDIA_IMAGELOADER_H
#include "IFunctor.h"
#include "InputHandler.h"
#include "JNIObject.h"
#include "JNIBridge.h"
#include "ImageFrame.h"

/**
 * 将RGBA数据读取到平台层的图片对象，android是读取到bitmap中
 * Input：
 *  - data: 只能是ImageFrame
 * output:
 *  - data: JNIObject，可回调给java层
 */
namespace smedia {
    class ImageLoader : public IFunctor {
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

        ~ImageLoader() override = default;

    private:
        JNIObject readPixelBuffer(ImageFrame& imageFrame);

        JNIObject readAsBitmap(ImageFrame& imageFrame);

    private:
        FunctorContext* mFunctorContext;
        InputHandler mInputHandler;
    };
}



#endif //SMEDIA_IMAGELOADER_H
