//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_GRAPH_H
#define SMEDIA_GRAPH_H
#include "GraphConfig.h"
#include "ExecuteManager.h"
#include "Logger.h"
#include "Data.h"
#include "ServiceManager.h"
#include <mutex>
#include <map>
#include <vector>
#include <memory>
#include "GLHelper.h"
#include "GLContext.h"


namespace smedia {

class Graph {
    enum State {CREATED,INITIALIZED,RUNNING,TERMINATED};
public:
    Graph();
    ~Graph();
    /**
     * 初始化图
     * @param config 初始化配置
     * @param options 一些全局的配置，如egl环境、渲染窗口等
     */
    bool initialize(GraphConfig& config,const OptionMap& options);
    bool run();
    bool stop();
    bool release();
    void setOption(const std::string &nodeName,const OptionMap options);

private:
    NodeMap m_nodesMap; // 节点映射
    EdgeMap m_edgesMap; // 全局边映射
    std::unique_ptr<ServiceManager> mGlobalServiceManager; // 全局节点服务对象
    std::vector<std::map<std::string,OptionMap>> m_optionsSave; // 缓存用户设置的option
    std::unique_ptr<ExecuteManager> m_executeManger; // 负责调度节点

    std::mutex m_mutex; // 操作队列的时候需要加锁
    volatile State m_state; // graph状态

private:
    bool initializeExecutor(GraphConfig& config);
    bool initializeNode(GraphConfig& config);
    bool initService(GraphConfig& config,const OptionMap& options);
    bool validateEdges();
    bool parseNodeTag(const NodeConfig& config,bool isInput);

    void executeNode(Node* node);
    void executeConnectNode(Node* node);
    void setOptionToNode(const std::string &nodeName,const OptionMap& options);
};
}

#endif //SMEDIA_GRAPH_H
