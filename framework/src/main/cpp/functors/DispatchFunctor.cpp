//
// Created by wansu on 2021/12/31.
//

#include "DispatchFunctor.h"
#include "FunctorRegister.h"
namespace smedia {

    bool DispatchFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mImageSignal = false;
        mInputHandler.registerHandler("imageSignal",[this](InputData inputData)->bool {
            if (inputData.data.getData(mImageSignal) && mImageSignal) {
                return true;
            }
            return false;
        });
        mInputHandler.registerHandler("Data",[this](InputData inputData)->bool {
            if (mImageSignal) {
                mFunctorContext->setOutput(inputData.data,"Image");
                mImageSignal = false;
            }
            mFunctorContext->setOutput(inputData.data,"Master");
            return true;
        });
        return true;
    }

    void DispatchFunctor::unInitialize(FunctorContext *context) {

    }

    bool DispatchFunctor::execute(FunctorContext *context) {
        return mInputHandler.runExecuteHandler(context);
    }

    void DispatchFunctor::setOption(FunctorContext *context, const std::string &key, Data value) {
        mInputHandler.runOptionsHandler(context,key,value);
    }


    REGISTER_FUNCTOR(DispatchFunctor)
}