//
// Created by wansu on 2021/12/29.
//

#ifndef SMEDIA_GLIMAGEREADERFUNCTOR_H
#define SMEDIA_GLIMAGEREADERFUNCTOR_H

#include <Program.h>
#include "IFunctor.h"
#include "InputHandler.h"
#include "IGLRenderFunctor.h"

/**
 * enable决定输出的数据类型
 * ratio决定输出的图形的比例，-1表示原始数据比例
 * todo 目前仅支持读取到RGBA，后续再进行拓展
 */
namespace smedia {
    class GLImageReaderFunctor : public IGLRenderFunctor{
    public:
        void onInit(InputHandler &inputHandler) override;

        void onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) override;

        void unInitialize(FunctorContext *context) override;

        std::string getFragmentCode() override;

        ~GLImageReaderFunctor() override = default;

    private:
        volatile bool mEnableRGBA;
        volatile float mRatio;
    };
}



#endif //SMEDIA_GLIMAGEREADERFUNCTOR_H
