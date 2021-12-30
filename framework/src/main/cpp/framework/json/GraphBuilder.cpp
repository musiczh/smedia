//
// Created by wansu on 2021/11/23.
//

#include "GraphBuilder.h"

namespace smedia {

    void GraphBuilder::addNode(NodeConfig &nodeConfig) {
        if (graphConfig == nullptr) {
            return;
        }

        if (nodeConfig.name.empty()) {
            // 加上前后缀，防止重名
            std::string unique_name = "__"+nodeConfig.functor + std::to_string(0);
            for (int i=1 ; nodeNameSet.find(unique_name) != nodeNameSet.end() ; i++) {
                unique_name += ("_" + std::to_string(i));
            }
            nodeConfig.name = unique_name;
        } else {
            if (nodeNameSet.find(nodeConfig.name) != nodeNameSet.end()) {
                LOG_ERROR << "can not set the same name to node";
                return;
            }
            nodeNameSet.insert(nodeConfig.name);
        }

        if (graphConfig != nullptr) {
            graphConfig->nodes.push_back(nodeConfig);
        }
    }

    void GraphBuilder::reset() {
        graphConfig = std::unique_ptr<GraphConfig>(new GraphConfig());
        nodeNameSet.clear();
    }

    std::unique_ptr<GraphConfig> GraphBuilder::buildGraph() {
        // 如果解析的过程出现错误，则config为null
        if (graphConfig == nullptr) {
            // 返回一个空的
            return nullptr;
        }
        return std::move(graphConfig);
    }

    void GraphBuilder::addExecutor(ExecutorConfig &executorConfig) {
        if (graphConfig == nullptr) {
            return;
        }

        if (executorNameSet.find(executorConfig.name) != executorNameSet.end()) {
            LOG_ERROR << "can not set the same executor name";
            graphConfig = nullptr;
            return ;
        } else {
            executorNameSet.insert(executorConfig.name);
        }

        if (graphConfig != nullptr) {
            graphConfig->executors.push_back(executorConfig);
        }
    }
}