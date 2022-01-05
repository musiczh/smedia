//
// Created by wansu on 2021/12/29.
//

#ifndef SMEDIA_CALLBACKFUNCTOR_H
#define SMEDIA_CALLBACKFUNCTOR_H
#include "IFunctor.h"
#include "InputHandler.h"
#include "JNIBridge.h"
#include <map>
#include <jni.h>
namespace smedia {
    class CallbackFunctor : public IFunctor{
    public:
        void initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(const std::string &key, Data value) override;

        ~CallbackFunctor() override = default;

    private:
        std::map<std::string,JNIObject> mJniCallbackMap;
        FunctorContext *mFunctorContext;
        InputHandler mInputHandler;
    };
}



#endif //SMEDIA_CALLBACKFUNCTOR_H
