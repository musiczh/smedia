//
// Created by wansu on 2021/11/19.
//

#ifndef SMEDIA_DATASTREAMMANAGER_H
#define SMEDIA_DATASTREAMMANAGER_H
#include "Edge.h"
#include "TagIndexQueue.h"
#include <set>
#include <queue>
namespace smedia {
    class DataStreamManager {
    public:
        // 从DataStream映射到StreamKey
        DataStreamManager();
        // 添加边，解析端口信息，并进行分类管理
        void addEdge(Edge* edge,bool isInput);
        // 读取端口的输入
        Data getInput(const std::string& tag,int index,bool pop);
        Data getFront(std::string& tag,int& index,bool pop);
        // 输出数据到端口
        void pushOutput(Data data,const std::string& tag,int index);
        // 获取端口信息，返回的是tag:{1,2,3}的map结构
        std::set<PortKey> getPortInfo(bool isInput) const;
    public:
       void onEdgeDataUpdate(Edge* edge);

    private:
        TagIndexQueue mInputQueue;
        std::map<PortKey,std::set<Edge*>> mInputPortEdges;
        std::map<PortKey,std::set<Edge*>> mOutputPortEdges;
    };
}



#endif //SMEDIA_DATASTREAMMANAGER_H
