//
// Created by wansu on 2021/12/29.
//

#ifndef SMEDIA_WHITEBALANCEFUNCTOR_H
#define SMEDIA_WHITEBALANCEFUNCTOR_H
#include "IGLRenderFunctor.h"
/**
 * 白平衡调节
 * @param temperature 调节色温，建议范围[-1,1]，也可以扩大到2，甚至更大，但是会失真
 * @param tint 着色，范围同上
 */
namespace smedia {
    class WhiteBalanceFunctor : public IGLRenderFunctor{
    public:
        bool onInit(InputHandler &inputHandler) override;

        bool onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) override;

        void unInitialize(FunctorContext *context) override;

        std::string getFragmentCode() override;

        ~WhiteBalanceFunctor() override = default;

    private:
        volatile float mTemperature;
        volatile float mTint;
    };
}



#endif //SMEDIA_WHITEBALANCEFUNCTOR_H
