//
// Created by wansu on 2021/12/15.
//

#ifndef SMEDIA_BRIGHTNESSFUNCTOR_H
#define SMEDIA_BRIGHTNESSFUNCTOR_H
/**
 * 调节亮度functor
 */
#include <ostream>
#include <Program.h>
#include "IFunctor.h"
#include "FunctorRegister.h"
#include "IGLRenderFunctor.h"
namespace smedia {
    class BrightnessFunctor : public IGLRenderFunctor{
    public:
        void onInit(InputHandler &inputHandler) override;

        void onDraw(GLBufferFrame *bufferFrame, GLFrame &frame) override;

        void unInitialize(FunctorContext *context) override;

        ~BrightnessFunctor() override = default;

    private:
        volatile float mBrightness;
        std::unique_ptr<Program> mProgram;
    };
}



#endif //SMEDIA_BRIGHTNESSFUNCTOR_H
