//
// Created by wansu on 2021/12/28.
//

#ifndef SMEDIA_SATURATIONFUNCTOR_H
#define SMEDIA_SATURATIONFUNCTOR_H
#include <ostream>
#include <Program.h>
#include "IFunctor.h"
#include "FunctorRegister.h"
#include "GLContext.h"
#include "GLFrame.h"
/**
 * 饱和度调节functor
 */
namespace smedia {
    class SaturationFunctor : public IFunctor{
    public:
        void initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(const std::string &key, Data value) override;

        ~SaturationFunctor() override = default;

    private:
        GLContext mGLContext{};
        unsigned int mTextureId;
        unsigned int mFBO;
        volatile float mSaturation;
        std::unique_ptr<Program> mProgram;
        FunctorContext* mFunctorContext;
    };
}



#endif //SMEDIA_SATURATIONFUNCTOR_H
