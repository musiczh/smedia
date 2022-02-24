//
// Created by wansu on 2021/11/19.
//

#include "FunctorContext.h"
#include "Node.h"
#include <utility>
namespace smedia {

    FunctorContext::FunctorContext(std::vector<std::string> &inputEdges, std::vector<std::string> &outputEdges,
                                   ServiceManager& serviceManager, EdgeMap &edgeMap, Node* node)
                                   : mNode(node),mServiceManager(serviceManager),
                                   m_outputManager(false),m_inputManager(true){
        // 解析输入输出边，并绑定DataStream
        for (auto& edgeName : inputEdges) {
            if (edgeMap.find(edgeName) != edgeMap.end()) {
                Edge* edge = edgeMap[edgeName].get();
                m_inputManager.setDataStream(edge->dataStream.get(),
                                             edge->outputPort->tag,
                                             edge->outputPort->index);}
        }
        for (auto& edgeName : outputEdges) {
            if (edgeMap.find(edgeName) != edgeMap.end()) {
                Edge* edge = edgeMap[edgeName].get();
                m_outputManager.setDataStream(edge->dataStream.get(),
                                              edge->inputPort->tag,
                                              edge->inputPort->index);}
        }
    }

    Data FunctorContext::getInput(const std::string &tag, int index) {
        return m_inputManager.getInputData(tag,index, false);
    }

    Data FunctorContext::popInput(const std::string &tag, int index) {
        return m_inputManager.getInputData(tag,index, true);
    }

    void FunctorContext::setOutput(Data data,const std::string &tag, int index) {
        m_outputManager.pushOutputData(data,tag,index);
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

    void FunctorContext::setExecuteConnectNodeHandler(std::function<void()> function) {
        m_executeConnectNodeHandler = std::move(function);
    }

    void FunctorContext::executeConnectNode() {
        m_executeConnectNodeHandler();
    }

    std::unique_ptr<std::set<std::string>> FunctorContext::getInputTags() {
        return m_inputManager.getTags();
    }

    std::unique_ptr<std::set<std::string>> FunctorContext::getOutputTags() {
        return m_outputManager.getTags();
    }

    std::string FunctorContext::getNodeName() {
        return mNode->getName();
    }

    Data FunctorContext::getFrontInout(const std::string &tag, int &index, bool pop) {

    }


}