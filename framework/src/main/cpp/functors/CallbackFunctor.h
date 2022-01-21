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

/**
 * 回调给上层的functor，可设置回调
 * android只能接收JNIObject数据的输入，让java层去做转换
 * - input:
 *      - Data
 * - options:
 *      - callback,必须继承NativeCallback
 */
namespace smedia {
    class CallbackFunctor : public IFunctor{
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

        ~CallbackFunctor() override = default;

    private:
        std::map<std::string,JNIObject> mJniCallbackMap;
        FunctorContext *mFunctorContext;
        InputHandler mInputHandler;
    };
}



#endif //SMEDIA_CALLBACKFUNCTOR_H
