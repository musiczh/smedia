//
// Created by wansu on 2021/12/28.
//

#ifndef SMEDIA_ADJUSTCOLORFUNCTOR_H
#define SMEDIA_ADJUSTCOLORFUNCTOR_H
#include "IGLRenderFunctor.h"
#include "InputHandler.h"
#include "GLBufferFrame.h"
/**
 * 基本属性调节：brightness亮度 (0.1-x)，contrast对比度、saturation饱和度（1.0-x)
 */
namespace smedia {
    class AdjustColorFunctor : public IGLRenderFunctor{
    public:
        virtual void onInit(InputHandler &inputHandler);

        virtual void onDraw(GLBufferFrame *bufferFrame,GLFrame&);

        virtual void unInitialize(FunctorContext *context);

        virtual ~AdjustColorFunctor() = default;

    private:
        float mSaturation;
        float mContrast;
        float mBrightness;
        std::unique_ptr<Program> mProgram;
    };
}



#endif //SMEDIA_ADJUSTCOLORFUNCTOR_H
