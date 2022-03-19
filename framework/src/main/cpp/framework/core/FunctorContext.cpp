//
// Created by wansu on 2021/11/19.
//

#include "FunctorContext.h"
#include "Node.h"
#include <utility>
namespace smedia {

    FunctorContext::FunctorContext(std::vector<std::string> &inputEdges, std::vector<std::string> &outputEdges,
                                   ServiceManager& serviceManager, EdgeMap &edgeMap, Node* node)
                                   : mNode(node),mServiceManager(serviceManager){
        // 解析输入输出边，并将Edge指针交给DataStreamManager处理
        for (auto& edgeName : inputEdges) {
            if (edgeMap.find(edgeName) != edgeMap.end()) {
                mDataStreamManager.addEdge(edgeMap[edgeName].get(),true);
            }
        }
        for (auto& edgeName : outputEdges) {
            if (edgeMap.find(edgeName) != edgeMap.end()) {
                mDataStreamManager.addEdge(edgeMap[edgeName].get(),false);
            }
        }
    }

    Data FunctorContext::getInput(const std::string &tag, int index) {
        return mDataStreamManager.getInput(tag,index,false);
    }

    Data FunctorContext::popInput(const std::string &tag, int index) {
        return mDataStreamManager.getInput(tag,index, true);
    }

    void FunctorContext::setOutput(Data data,const std::string &tag, int index) {
        mDataStreamManager.pushOutput(data,tag,index);
    }

    void FunctorContext::setExecuteSelfHandler(std::function<void()> function) {
        m_executeSelfHandler = std::move(function);
    }

    void FunctorContext::executeSelf() {
        if (m_executeSelfHandler != nullptr){
            m_executeSelfHandler();
        }
    }

    void FunctorContext::setExecuteConnectNodeHandler(std::function<void()> function) {
        m_executeConnectNodeHandler = std::move(function);
    }

    void FunctorContext::executeConnectNode() {
        m_executeConnectNodeHandler();
    }

    std::string FunctorContext::getNodeName() {
        return mNode->getName();
    }

    std::set<PortKey> FunctorContext::getPortInfo(bool isInput) const {
        return mDataStreamManager.getPortInfo(isInput);
    }

    Data FunctorContext::getInputFront(std::string& tag,int& index,bool pop) {
        return mDataStreamManager.getFront(tag,index,pop);
    };


}