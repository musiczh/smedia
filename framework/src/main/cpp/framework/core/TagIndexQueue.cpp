//
// Created by huan on 2022/3/18.
//

#include "TagIndexQueue.h"

#include <utility>
namespace smedia {

    void TagIndexQueue::offerUpdateEdge(Edge *edge) {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        auto& key = edge->outputPort->portKey;
        if (mMap.find(key) == mMap.end()) {
            auto newNode = std::unique_ptr<Node>(new Node);
            auto tailNode = std::unique_ptr<Node>(new Node);
            newNode->typeNext = tailNode.get();
            tailNode->typeBefore = newNode.get();
            newNode->tail = std::move(tailNode);
            mMap.insert({key,std::move(newNode)});
        }
        auto& typeHead = mMap[key];

        auto* node = new Node;
        node->edge = edge;

        // 时间链表
        node->timeNext = mHead->tail.get();
        node->timeBefore = mHead->tail->timeBefore;
        mHead->tail->timeBefore->timeNext = node;
        mHead->tail->timeBefore = node;


        // tag+index链表
        node->typeNext = typeHead->tail.get();
        node->typeBefore = typeHead->tail->typeBefore;
        typeHead->tail->typeBefore->typeNext = node;
        typeHead->tail->typeBefore = node;

        mLength++;
        if (mLength > mThreshold) {
            // 如果达到了阈值，长度不再增加，弹出旧的输入，避免长度无限累积导致oom
            deleteNode(mHead->timeNext);
        }
    }

    TagIndexQueue::TagIndexQueue() {
        // 初始化时间链表的头尾节点
        mHead = std::unique_ptr<Node>(new Node);
        auto* tailNode = new Node;

        mHead->timeNext = tailNode;
        tailNode->timeBefore = mHead.get();

        mHead->tail = std::unique_ptr<Node>(tailNode);
    }

    Data TagIndexQueue::getInputFront(PortKey &key, bool pop) {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        if (mLength == 0) {
            return Data();
        }
        Edge* edge =  mHead->timeNext->edge;
        key = edge->outputPort->portKey;
        Data res;
        if (pop) {
            edge->popData(res);
            deleteNode(mHead->timeNext);
        } else {
            edge->getFront(res);
        }
        return res;
    }

    void TagIndexQueue::deleteNode(Node *node) {
        // 从时间链表中删除节点
        node->timeBefore->timeNext = node->timeNext;
        node->timeNext->timeBefore = node->timeBefore;
        // 从tag+index链表中删除节点
        node->typeNext->typeBefore = node->typeBefore;
        node->typeBefore->typeNext = node->typeNext;
        // 释放节点内存
        delete node;
        // 长度减一
        mLength--;
    }

    Data TagIndexQueue::getInputData(const PortKey &key, bool pop) {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        // 没有该类型的链表
        if (mMap.find(key) == mMap.end()) {
            return Data();
        }
        auto& head = mMap[key];
        // 链表为空
        if (head->tail.get() == head->typeNext) {
            return Data();
        }
        Data res;
        if (pop) {
            head->typeNext->edge->popData(res);
            Node* node = head->typeNext;
            // 从时间链表中删除节点
            node->timeBefore->timeNext = node->timeNext;
            node->timeNext->timeBefore = node->timeBefore;
            // 从tag+index链表中删除节点
            node->typeNext->typeBefore = node->typeBefore;
            node->typeBefore->typeNext = node->typeNext;
            // 释放节点内存
            delete node;
            // 长度减一
            mLength--;
        } else {
            head->typeNext->edge->getFront(res);
        }
        return res;
    }

    TagIndexQueue::~TagIndexQueue() {
        if (mLength == 0) {
            return;
        }
        // 释放所有节点
        std::unique_lock<std::mutex> lock(mQueueMutex);
        while(mHead->timeNext!=mHead->tail.get()) {
            deleteNode(mHead->timeNext);
        }
    }
}