//
// Created by wansu on 2021/11/3.
// 图的配置，json配置解析后生成此对象
//

#ifndef SMEDIA_GRAPHCONFIG_H
#define SMEDIA_GRAPHCONFIG_H
#include "../data/typeDef.h"

namespace smedia {
    struct PortTag {
        std::string tag;
        std::string name;
        int index;
    };
    struct NodeConfig {
        std::string name;
        std::string functor;
        std::string executor;
        OptionMap options;
        std::vector<PortTag> inputs;
        std::vector<PortTag> outputs;
    };
    struct ExecutorConfig {
        std::string executor;
        std::string name;
        OptionMap options;
    };

    struct GraphConfig {
        std::deque<NodeConfig> nodes;
        std::deque<ExecutorConfig> executors;
    };
}

#endif //SMEDIA_GRAPHCONFIG_H
