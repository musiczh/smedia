//
// Created by wansu on 2021/12/31.
//

#include "DispatchFunctor.h"
#include "ObjectRegister.h"
#include "IFrame.h"
namespace smedia {

    bool DispatchFunctor::initialize(FunctorContext *context) {
        mFunctorContext = context;
        parseGraphTag();
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

    bool DispatchFunctor::parseGraphTag() {
        auto inputPortInfo = mFunctorContext->getPortInfo(true);
        auto outputPortInfo = mFunctorContext->getPortInfo(false);

        for (auto& item : inputPortInfo) {
            if (outputPortInfo.find(item) == outputPortInfo.end()) {
                LOG_ERROR << "no match input tag to output tag";
                // 输入输出tag不匹配
                return false;
            }
            if (item.tag == "master") {
                // 主输入端口
                continue;
            }
            // 对每个输入端口的输入记录时间戳
            mInputHandler.registerHandler(item.tag,item.index,[this](InputData inputData)->bool{
                mDispatchMap[inputData.data.getTime()] = {inputData.tag,inputData.index};
                return true;
            });
        }
        mInputHandler.registerHandler("master",[this](InputData inputData)->bool {
            // 判断是会否有相同的时间戳，有的话进行分流
            if (mDispatchMap.find(inputData.data.getTime()) != mDispatchMap.end()) {
                auto& key = mDispatchMap[inputData.data.getTime()];
                mFunctorContext->setOutput(inputData.data,key.tag,key.index);
                mDispatchMap.erase(inputData.data.getTime());
                return true;
            }
            // 输出到主输出端口
            mFunctorContext->setOutput(inputData.data,"master");
            return true;
        });
        return true;
    }


    REGISTER_CLASS(DispatchFunctor)
}