//
// Created by wansu on 2021/11/23.
//

#ifndef SMEDIA_GRAPHBUILDER_H
#define SMEDIA_GRAPHBUILDER_H
#include "GraphConfig.h"
#include <unordered_set>
namespace smedia {
    class GraphBuilder {
    public:
        void addNode(NodeConfig& nodeConfig);
        // 如果没有executor要补充一个默认的
        void addExecutor(ExecutorConfig& executorConfig);
        // 让外部拷贝一份，防止配置被改变
        std::unique_ptr<GraphConfig> buildGraph();
        void reset();
    private:
        std::unordered_set<std::string> nodeNameSet;
        std::unordered_set<std::string> executorNameSet;
        std::unique_ptr<GraphConfig> graphConfig;
    };
}



#endif //SMEDIA_GRAPHBUILDER_H
