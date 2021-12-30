//
// Created by wansu on 2021/11/19.
//

#include "FunctorContext.h"

#include <utility>
namespace smedia {

    FunctorContext::FunctorContext(std::vector<std::string> &inputEdges, std::vector<std::string> &outputEdges,
                                   EdgeMap &edgeMap,OptionMap& globalService) {
        // 解析输入输出边，并绑定DataStream
        for (auto& edgeName : inputEdges) {
            if (edgeMap.find(edgeName) != edgeMap.end()) {
                Edge* edge = edgeMap[edgeName].get();
                m_inputManager.setDataStream(edge->dataStream.get(),
                                             edge->outputPort->tag,
                                             edge->outputPort->index,true);}
        }
        for (auto& edgeName : outputEdges) {
            if (edgeMap.find(edgeName) != edgeMap.end()) {
                Edge* edge = edgeMap[edgeName].get();
                m_outputManager.setDataStream(edge->dataStream.get(),
                                              edge->inputPort->tag,
                                              edge->inputPort->index, false);}
        }
        for (auto& item : globalService) {
            mGlobalService[item.first] = item.second;
        }
    }

    Data FunctorContext::getInput(const std::string &tag, int index) {
        return m_inputManager.GetFront(tag,index);
    }

    Data FunctorContext::popInput(const std::string &tag, int index) {
        return m_inputManager.PopFront(tag,index);
    }

    void FunctorContext::setOutput(Data data,const std::string &tag, int index) {
        m_outputManager.PushData(data,tag,index);
    }

    int FunctorContext::getInputTagCount(const std::string &tag) {
        return m_inputManager.getTagCount(tag);
    }

    void FunctorContext::setExecuteSelfHandler(std::function<void()> function) {
        m_executeSelfHandler = std::move(function);
    }

    void FunctorContext::executeSelf() {
        if (m_executeSelfHandler != nullptr){
            m_executeSelfHandler();
        }
    }

    int FunctorContext::getOutputTagCount(const std::string &tag) {
        return m_outputManager.getTagCount(tag);
    }

    DataStreamManager::StreamKey FunctorContext::getFrontStreamKey() {
        return m_inputManager.getFrontStreamKey();
    }

    void FunctorContext::setExecuteConnectNodeHandler(std::function<void()> function) {
        m_executeConnectNodeHandler = std::move(function);
    }

    void FunctorContext::executeConnectNode() {
        m_executeConnectNodeHandler();
    }

    Data FunctorContext::getGlobalService(const std::string &name) {
        if (mGlobalService.find(name) != mGlobalService.end()) {
            return mGlobalService[name];
        }
        return Data{};
    }


}