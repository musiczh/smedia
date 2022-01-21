//
// Created by wansu on 2021/12/28.
//

#ifndef SMEDIA_ADJUSTCOLORFUNCTOR_H
#define SMEDIA_ADJUSTCOLORFUNCTOR_H
#include "IGLRenderFunctor.h"
#include "InputHandler.h"
#include "GLBufferFrame.h"
#include "Render.h"
/**
 * 基本属性调节：brightness亮度 (0.1-x)，contrast对比度、saturation饱和度（1.0-x)
 */
namespace smedia {
    class AdjustColorFunctor : public IGLRenderFunctor{
    public:
        bool onInit(InputHandler &inputHandler) override;

        bool onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) override;

        std::string getFragmentCode() override;

        virtual void unInitialize(FunctorContext *context) override;

        virtual ~AdjustColorFunctor() = default;

    private:
        float mSaturation;
        float mContrast;
        float mBrightness;
    };
}



#endif //SMEDIA_ADJUSTCOLORFUNCTOR_H
