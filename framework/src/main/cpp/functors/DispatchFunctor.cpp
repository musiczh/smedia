//
// Created by wansu on 2021/12/31.
//

#include "DispatchFunctor.h"
#include "ObjectRegister.h"
#include "IFrame.h"
namespace smedia {

    bool DispatchFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        mImageSignal = false;
        parseGraphTag();
        registerInputHandler();
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

    void DispatchFunctor::registerInputHandler() {
        for (auto& tag : mTagList) {
            mInputHandler.registerHandler(tag,[this](InputData inputData)->bool {
                mDispatchMap[inputData.data.getTime()] = inputData.tag;
                return true;
            });
        }
        mInputHandler.registerHandler("dispatchSignal",[this](InputData inputData)->bool {
            if (inputData.data.getData(mImageSignal) && mImageSignal) {
                return true;
            }
            return false;
        });
        mInputHandler.registerHandler("master",[this](InputData inputData)->bool {
            if (mImageSignal) {
                mFunctorContext->setOutput(inputData.data,"subSignal");
                mImageSignal = false;
                return true;
            }
            // 判断是会否有相同的时间戳，有的话进行分流
            if (mDispatchMap.find(inputData.data.getTime()) != mDispatchMap.end()) {
                mFunctorContext->setOutput(inputData.data,mDispatchMap[inputData.data.getTime()]);
                mDispatchMap.erase(inputData.data.getTime());
                return true;
            }
            mFunctorContext->setOutput(inputData.data,"master");
            return true;
        });
    }

    bool DispatchFunctor::parseGraphTag() {
        auto inputTags = mFunctorContext->getInputTags();
        auto outputTags = mFunctorContext->getOutputTags();
        // 主要的输入端口先去除掉
        inputTags->erase("data");

        for (auto& item : *inputTags) {
            if (outputTags->find(item) == outputTags->end()) {
                LOG_ERROR << "no match input tag";
                // 输如输出tag不匹配
                return false;
            }
            mTagList.push_back(item);
            outputTags->erase(item);
        }
        return true;
    }


    REGISTER_CLASS(DispatchFunctor)
}