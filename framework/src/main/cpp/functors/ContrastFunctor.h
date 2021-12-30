//
// Created by wansu on 2021/12/27.
//

#ifndef SMEDIA_CONTRASTFUNCTOR_H
#define SMEDIA_CONTRASTFUNCTOR_H
#include <ostream>
#include <Program.h>
#include "IFunctor.h"
#include "FunctorRegister.h"
#include "GLContext.h"
#include "GLFrame.h"

namespace smedia {
    class ContrastFunctor : public IFunctor{
    public:
        void initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(const std::string &key, Data value) override;

        ~ContrastFunctor() override = default;

    private:
        GLContext mGLContext{};
        unsigned int mTextureId;
        unsigned int mFBO;
        volatile float mContrast;
        std::unique_ptr<Program> mProgram;
        FunctorContext* mFunctorContext;
    };
}



#endif //SMEDIA_CONTRASTFUNCTOR_H
