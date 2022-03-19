//
// Created by huan on 2022/3/18.
//

#ifndef SMEDIA_TAGINDEXQUEUE_H
#define SMEDIA_TAGINDEXQUEUE_H
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "Data.h"
#include "Edge.h"
/**
 * 这个类主要是用于管理节点端口输入的缓存队列，支持按时间顺序和tag+index获取输入data
 */
namespace smedia {
    class TagIndexQueue {
    public:
        class Node {
        public:
            Edge* edge;
            Node* timeNext;
            Node* timeBefore;
            Node* typeNext;
            Node* typeBefore;
            std::unique_ptr<Node> tail;
        };

    public:
        TagIndexQueue();
        // 添加data到queue中
        void offerUpdateEdge(Edge* edge);
        // 获取队列头的data，并将tag+index信息放在
        Data getInputFront(PortKey& key,bool pop);
        Data getInputData(const PortKey& key,bool pop);
        ~TagIndexQueue();

    private:
        void deleteNode(Node* node);

    private:
        std::mutex mQueueMutex{};
        int mThreshold = 15;
        std::unique_ptr<Node> mHead{};
        std::map<PortKey,std::unique_ptr<Node>> mMap{};
        int mLength{0};
    };
}



#endif //SMEDIA_TAGINDEXQUEUE_H
