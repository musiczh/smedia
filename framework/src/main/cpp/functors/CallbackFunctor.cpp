//
// Created by wansu on 2021/12/29.
//

#include "CallbackFunctor.h"
#include "FunctorRegister.h"
namespace smedia {

    void CallbackFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mInputHandler.registerHandler("callback",[this](InputData inputData)->bool {
            JNIObject callbackObject;
            if (inputData.data.getData(callbackObject)) {
                mJniCallbackMap[inputData.key] = callbackObject;
                return true;
            }
            LOG_ERROR << "CallbackFunctor get callback fail";
            return false;
        });
        mInputHandler.registerHandler("Data",[this](InputData inputData)->bool {
            JNIObject value;
            if (inputData.data.getData(value) && mJniCallbackMap.find("callback")!=mJniCallbackMap.end()) {
                JNIObject callback = mJniCallbackMap["callback"];
                JNIInvoker<bool,std::string,std::string,JNIObject>::InvokeObjectMethod(
                        callback.getJObject(),"onNativeCallback",
                        "callback","callback",value);
            }
            return true;
        });
    }

    void CallbackFunctor::unInitialize(FunctorContext *context) {

    }

    bool CallbackFunctor::execute(FunctorContext *context) {
        return mInputHandler.runHandler(context);
    }

    void CallbackFunctor::setOption(const std::string &key, Data value) {
        mInputHandler.runHandler(mFunctorContext,key,value);
    }
    REGISTER_FUNCTOR(CallbackFunctor)
}