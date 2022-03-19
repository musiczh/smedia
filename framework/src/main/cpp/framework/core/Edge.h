//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_EDGE_H
#define SMEDIA_EDGE_H
#include "typeDef.h"
#include <queue>
namespace smedia {
    struct PortKey {
        std::string tag;
        int index;
        bool operator<(const PortKey& key) const;
    };
    struct Port {
        PortKey portKey;
        std::string node;
    };
    class Edge {
        using DataListener = std::function<void(Edge*)>;
    public:
        bool empty();
        void pushData(Data& data);
        bool getFront(Data& data);
        bool popData(Data& data);
        void setDataListener(DataListener listener);

    private:
        DataListener m_listener;
        std::queue<Data> m_dataQueue;
        std::mutex m_lock;

    public:
        std::string name;
        std::unique_ptr<Port> inputPort;
        std::unique_ptr<Port> outputPort;
    };
}

#endif //SMEDIA_EDGE_H
