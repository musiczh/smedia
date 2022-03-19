//
// Created by wansu on 2021/11/23.
//

#ifndef SMEDIA_GRAPHBUILDER_H
#define SMEDIA_GRAPHBUILDER_H
#include "GraphConfig.h"
#include <unordered_set>
namespace smedia {
    /**
     * 从json解析出来的数据构建一个GraphConfig。
     * 主要负责做一些名字重名检测、默认值等
     */
    class GraphBuilder {
    public:
        void addNode(NodeConfig& nodeConfig);
        // 如果没有executor要补充一个默认的
        void addExecutor(ExecutorConfig& executorConfig);
        void addService(ServiceConfig& serviceConfig);
        // 让外部拷贝一份，防止配置被改变
        std::unique_ptr<GraphConfig> buildGraph();
        // 重写解析前需要调用此方法重置
        // 因为build完成之后、或者build过程中出现了错误，内部graphConfig均会被置空
        void reset();
        void onBuildError(const std::string& msg);

    private:
        // 检查输入输出配置是否正确
        void checkNodePort(NodeConfig& nodeConfig);

    private:
        std::unordered_set<std::string> nodeNameSet;
        std::unordered_set<std::string> executorNameSet;
        std::unordered_set<std::string> serviceNameSet;
        std::unique_ptr<GraphConfig> graphConfig;
    };
}



#endif //SMEDIA_GRAPHBUILDER_H
