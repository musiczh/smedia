//
// Created by wansu on 2021/12/28.
//

#include "InputHandler.h"

#include <utility>
namespace smedia {

    void InputHandler::registerHandler(const std::string &key, Handler handler) {
        registerHandler(key,0,std::move(handler));
    }

    void InputHandler::registerHandler(const std::string &key, int index,Handler handler) {
        PortKey portKey{key,index};
        mHandlerMap.insert({portKey,std::move(handler)});
    }

    void InputHandler::registerDefaultHandler(Handler defaultHandler) {
        mDefaultHandler = std::move(defaultHandler);
    }


    bool InputHandler::handle(InputData inputData) {
        PortKey portKey{inputData.tag,inputData.index};
        if (mHandlerMap.find(portKey) != mHandlerMap.end()) {
            return mHandlerMap[portKey](inputData);
        }
        if (mDefaultHandler != nullptr) {
            return mDefaultHandler(std::move(inputData));
        }
        LOG_ERROR << "can not found handler to run,nodeName = " << inputData.nodeName
                << ",tag = " << inputData.tag << ",index=" << inputData.index;
        return false;
    }

    bool InputHandler::runExecuteHandler(FunctorContext * functorContext) {
        std::string tag;
        int index;
        auto data = functorContext->getInputFront(tag,index, true);
        if (data.isEmpty()) {
            LOG_DEBUG << "handle input data is null";
            return false;
        }
        return handle({functorContext->getNodeName(),data,tag,index,functorContext});
    }

    bool InputHandler::runOptionsHandler(FunctorContext *functorContext, const std::string &key, Data data) {
        if (data.isEmpty()) {
            LOG_DEBUG << "run handle data is empty";
            return false;
        }
        return handle({functorContext->getNodeName(),data,key,0,functorContext});
    }

}














