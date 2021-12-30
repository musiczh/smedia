//
// Created by wansu on 2021/12/28.
//

#ifndef SMEDIA_BEAUTYFUNCTOR_H
#define SMEDIA_BEAUTYFUNCTOR_H
#include <ostream>
#include <Program.h>
#include "IFunctor.h"
#include "FunctorRegister.h"
#include "GLContext.h"
#include "GLFrame.h"
#include "IGLRenderFunctor.h"
namespace smedia {
    class BeautyFunctor : public IGLRenderFunctor {
    public:
        void onInit(InputHandler &inputHandler) override;

        void onDraw(GLBufferFrame *bufferFrame,GLFrame&) override;

        void unInitialize(FunctorContext *context) override;

        ~BeautyFunctor() = default;

    private:
        volatile float mLevel;
        std::unique_ptr<Program> mProgram;
    };

}



#endif //SMEDIA_BEAUTYFUNCTOR_H
