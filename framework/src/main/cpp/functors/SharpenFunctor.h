//
// Created by wansu on 2021/12/29.
//

#ifndef SMEDIA_SHARPENFUNCTOR_H
#define SMEDIA_SHARPENFUNCTOR_H
#include "IGLRenderFunctor.h"
/**
 * 调节锐化程度，参数：sharpen，范围0-2，大于2会导致图片失真
 */
namespace smedia {
    class SharpenFunctor : public IGLRenderFunctor {
    public:
        void onInit(InputHandler &inputHandler) override;

        void onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) override;

        void unInitialize(FunctorContext *context) override;

        ~SharpenFunctor() override = default;

    private:
        std::unique_ptr<Program> mProgram;
        volatile float mSharpen;
    };

}


#endif //SMEDIA_SHARPENFUNCTOR_H
