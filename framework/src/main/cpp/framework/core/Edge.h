//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_EDGE_H
#define SMEDIA_EDGE_H
#include "../data/typeDef.h"
#include "DataStream.h"
namespace smedia {
    struct Edge {
        struct Port {
            std::string tag;
            std::string node;
            int index;
        };
        std::string name;
        std::unique_ptr<Port> inputPort;
        std::unique_ptr<Port> outputPort;
        std::unique_ptr<DataStream> dataStream;
    };
}

#endif //SMEDIA_EDGE_H
