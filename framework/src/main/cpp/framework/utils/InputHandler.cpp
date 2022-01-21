//
// Created by wansu on 2021/12/28.
//

#include "InputHandler.h"
namespace smedia {

    void InputHandler::registerHandler(const std::string &key, Handler handler) {
        mHandlerMap[key] = std::move(handler);
    }

    void InputHandler::registerDefaultHandler(Handler defaultHandler) {
        mDefaultHandler = std::move(defaultHandler);
    }

    bool InputHandler::runHandler(FunctorContext *functorContext) {
        auto key = functorContext->getFrontStreamKey();
        if (key.tag == "emptyStreamKey") {
            LOG_DEBUG << "inputs is empty";
            return false;
        }
        Data data = functorContext->popInput(key.tag,key.index);
        if (data.isEmpty()) {
            LOG_DEBUG << "handle input data is null";
            return false;
        }
        return handle({data,key.tag,key.index,functorContext});
    }

    bool InputHandler::runHandler(FunctorContext *functorContext, const std::string &key, Data data) {
        if (data.isEmpty()) {
            LOG_DEBUG << "run handle data is empty";
            return false;
        }
        return handle({data,key,-1,functorContext});
    }

    bool InputHandler::handle(InputData inputData) {
        if (mHandlerMap.find(inputData.key) != mHandlerMap.end()) {
            return mHandlerMap[inputData.key](inputData);
        }
        if (mDefaultHandler != nullptr) {
            return mDefaultHandler(std::move(inputData));
        }
        LOG_DEBUG << "can not found handler to run";
        return false;
    }

    bool InputHandler::runExecuteHandler(FunctorContext * functorContext) {
        auto key = functorContext->getFrontStreamKey();
        if (key.tag == "emptyStreamKey") {
            LOG_DEBUG << "inputs is empty";
            return false;
        }
        Data data = functorContext->popInput(key.tag,key.index);
        if (data.isEmpty()) {
            LOG_DEBUG << "handle input data is null";
            return false;
        }
        return handle({data,key.tag,key.index,functorContext});
    }

    bool InputHandler::runOptionsHandler(FunctorContext *functorContext, const std::string &key, Data data) {
        if (data.isEmpty()) {
            LOG_DEBUG << "run handle data is empty";
            return false;
        }
        return handle({data,key,-1,functorContext});
    }
}














