//
// Created by wansu on 2021/12/29.
//

#include "CallbackFunctor.h"
#include "ObjectRegister.h"

namespace smedia {

    bool CallbackFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mInputHandler.registerHandler("callback",[this](InputData inputData)->bool {
            JNIObjectRef callbackObject;
            if (inputData.data.getData(callbackObject)) {
                mJniCallbackMap[inputData.tag] = *callbackObject.get();
                return true;
            }
            LOG_ERROR << "CallbackFunctor get callback fail";
            return false;
        });
        mInputHandler.registerHandler("data",[this](InputData inputData)->bool {
            JNIObjectRef value;
            if (inputData.data.getData(value) && mJniCallbackMap.find("callback")!=mJniCallbackMap.end()) {
                JNIObject callback = mJniCallbackMap["callback"];
                JNIInvoker<bool,std::string,std::string,JNIObjectRef>::InvokeObjectMethod(
                        callback.getJObject(),"onNativeCallback",
                        "callback","callback",value);
            }
            return true;
        });
        return true;
    }

    void CallbackFunctor::unInitialize(FunctorContext *context) {
        //
    }

    bool CallbackFunctor::execute(FunctorContext *context) {
        return mInputHandler.runExecuteHandler(context);
    }

    void CallbackFunctor::setOption(FunctorContext *context, const std::string &key, Data value) {
        mInputHandler.runOptionsHandler(context,key,value);
    }
    REGISTER_CLASS(CallbackFunctor)
}