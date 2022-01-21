//
// Created by wansu on 2021/12/29.
//

#ifndef SMEDIA_DENOISEFUNCTOR_H
#define SMEDIA_DENOISEFUNCTOR_H
#include "IGLRenderFunctor.h"
/**
 * sigma：降噪半径，1-6 ，数值太大gpu顶不住
 * kSigma：降噪效果程度，不要大于2，原因同上
 * threshold：羽化程度，大于0.1,小于0.1图片会出现异常
 */
namespace smedia {
    class DeNoiseFunctor : public IGLRenderFunctor{
    public:
        bool onInit(InputHandler &inputHandler) override;

        bool onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) override;

        std::string getFragmentCode() override;

        void unInitialize(FunctorContext *context) override;

        ~DeNoiseFunctor() override = default;

    private:
        std::unique_ptr<Program> mProgram;

        volatile float mSigma;
        volatile float mKSigma;
        volatile float mThreshold;
    };

}


#endif //SMEDIA_DENOISEFUNCTOR_H
