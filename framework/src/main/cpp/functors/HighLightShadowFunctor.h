//
// Created by wansu on 2021/12/29.
//

#ifndef SMEDIA_HIGHLIGHTSHADOWFUNCTOR_H
#define SMEDIA_HIGHLIGHTSHADOWFUNCTOR_H
#include "IGLRenderFunctor.h"
/**
 * 高光和阴影调节，此functor暂时有bug，暂不使用
 * todo bugfix
 */
namespace smedia {
    class HighLightShadowFunctor : public IGLRenderFunctor{
    public:
        bool onInit(InputHandler &inputHandler) override;

        bool onDraw(GLBufferFrame *bufferFrame, Render *render, GLFrame &frame) override;

        void unInitialize(FunctorContext *context) override;

        std::string getFragmentCode() override;

        ~HighLightShadowFunctor() override = default;

    private:
        volatile float mHighLight;
        volatile float mShadow;
    };
}



#endif //SMEDIA_HIGHLIGHTSHADOWFUNCTOR_H
