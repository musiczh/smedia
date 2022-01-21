//
// Created by wansu on 2021/12/31.
//

#ifndef SMEDIA_DISPATCHFUNCTOR_H
#define SMEDIA_DISPATCHFUNCTOR_H
#include "IFunctor.h"
#include "InputHandler.h"
/**
 * 分流器,目前支持从主干流中分出一帧来读取bitmap
 */
namespace smedia {
    class DispatchFunctor : public IFunctor{
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

        ~DispatchFunctor() override = default;

    public:
        FunctorContext* mFunctorContext;
        InputHandler mInputHandler;
        volatile bool mImageSignal;
    };
}



#endif //SMEDIA_DISPATCHFUNCTOR_H
