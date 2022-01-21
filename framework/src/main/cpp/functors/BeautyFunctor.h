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
        bool onInit(InputHandler &inputHandler) override;

        bool onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) override;

        std::string getFragmentCode() override;

        void unInitialize(FunctorContext *context) override;

        ~BeautyFunctor() = default;

    private:
        volatile float mLevel;
        std::unique_ptr<Program> mProgram;
    };

}



#endif //SMEDIA_BEAUTYFUNCTOR_H
