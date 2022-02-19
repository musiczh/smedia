//
// Created by wansu on 2022/2/15.
//

#include "AndroidBaseFunctor.h"

#include <utility>
#include "FunctorContext.h"
#include "Logger.h"
#include "JNIBridge.h"

namespace smedia {

#define JAVA_REGISTER_CLASS "com/example/frameword/framework/AndroidFunctorRegister"
#define JAVA_REGISTER_METHOD "onNativeCreateAndroidFunctor"

    bool AndroidBaseFunctor::initialize(FunctorContext *context) {
        if (mAndroidFunctor == nullptr) {
            return false;
        }
        bool res = JNIInvoker<bool,long,std::string>::InvokeObjectMethod(mAndroidFunctor->getJObject(),
                                                  "onNativeInitialize",
                                                  reinterpret_cast<long>(context),mFunctorName);
        if (!res) {
            LOG_ERROR << "initialize "<< mFunctorName << "error";
            return false;
        }
        LOG_DEBUG << "initialize "<< mFunctorName << "success";
        return true;
    }

    void AndroidBaseFunctor::unInitialize(FunctorContext *context) {
        JNIInvoker<void,long>::InvokeObjectMethod(mAndroidFunctor->getJObject(),
                                                  "onNativeUnInitialize",
                                                  reinterpret_cast<long>(context));
    }

    bool AndroidBaseFunctor::execute(FunctorContext *context) {
        return JNIInvoker<bool,long>::InvokeObjectMethod(mAndroidFunctor->getJObject(),
                                                         "onNativeExecute",
                                                         reinterpret_cast<long>(context));;
    }

    void
    AndroidBaseFunctor::setOption(FunctorContext *context, const std::string &key, Data value) {
        auto data = JNIData::DataToJNIObject(value);
        if (data == nullptr) {
            LOG_ERROR << "setOption convert data to jniObject fail";
            return;
        }
        JNIInvoker<void,long,std::string,JNIObjectRef>::InvokeObjectMethod(
                mAndroidFunctor->getJObject(),"onNativeSetOption",reinterpret_cast<long>(context),
                key,data);
    }

    bool AndroidBaseFunctor::init(const std::string& name) {
        mAndroidFunctor = JNIInvoker<JNIObjectRef,std::string>::InvokeStaticMethod(
                JAVA_REGISTER_CLASS,JAVA_REGISTER_METHOD,name);
        return mAndroidFunctor->getJObject() != nullptr;
    }
}