//
// Created by wansu on 2021/11/19.
//

#include "DataStreamManager.h"
namespace smedia {

    DataStreamManager::DataStreamManager() {
        //
    }

    Data DataStreamManager::getInput(const std::string &tag, int index, bool pop) {
        PortKey key{tag,index};
        return mInputQueue.getInputData(key,pop);
    }

    void DataStreamManager::pushOutput(Data data, const std::string &tag, int index) {
        PortKey key{tag,index};
        // 找不到端口
        if (mOutputPortEdges.find(key) == mOutputPortEdges.end()) {
            LOG_ERROR << "setOutput error.Can not find port"
            <<"tag = " <<tag << " index = " << index;
            return;
        }
        // 输出到端口连接的所有的边
        auto& edgeSet = mOutputPortEdges[key];
        for(auto* edge : edgeSet) {
            edge->pushData(data);
        }
    }

    void DataStreamManager::addEdge(Edge *edge, bool isInput) {
        if (isInput) {
            // 注意这里输入边要取输出端口，输出边要取输入端口
            PortKey key = edge->outputPort->portKey;
            if (mInputPortEdges.find(key) == mInputPortEdges.end()) {
                mInputPortEdges.insert({key,std::set<Edge*>()});
            }
            mInputPortEdges[key].insert(edge);
            edge->setDataListener([this](Edge* edge){
                onEdgeDataUpdate(edge);
            });
        } else {
            PortKey key = edge->inputPort->portKey;
            if (mOutputPortEdges.find(key) == mOutputPortEdges.end()) {
                mOutputPortEdges.insert({key,std::set<Edge*>()});
            }
            mOutputPortEdges[key].insert(edge);
        }
    }

    std::set<PortKey> DataStreamManager::getPortInfo(bool isInput) const {
        std::set<PortKey> m;
        // 选择输入或者输出的端口信息
        auto& ports = (isInput? mInputPortEdges : mOutputPortEdges);
        for (auto& item : ports) {
            auto& tag = item.first.tag;
            auto index = item.first.index;
            m.insert(PortKey{tag,index});
        }
        return m;
    }

    void DataStreamManager::onEdgeDataUpdate(Edge *edge) {
        mInputQueue.offerUpdateEdge(edge);
    }

    Data DataStreamManager::getFront(std::string &tag, int &index, bool pop) {
        PortKey key;
        Data data = mInputQueue.getInputFront(key,pop);
        tag = key.tag;
        index = key.index;
        return data;
    }

}