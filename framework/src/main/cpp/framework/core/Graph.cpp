//
// Created by wansu on 2021/11/3.
//

#include "Graph.h"
#include "GLTexturePool.h"
#include "ObjectRegister.h"
#include "Executor.h"

namespace smedia {
    Graph::Graph() {
        m_state = CREATED;
        m_executeManger = std::unique_ptr<ExecuteManager>(new ExecuteManager);
    }

    bool Graph::initialize(GraphConfig &config,const OptionMap& options,
                           std::unique_ptr<Expander> expander) {
        std::unique_lock<std::mutex> lock(m_mutex);
        mExpander = std::move(expander);
        if (m_state != CREATED) {
            LOG_INFO << "graph has init";
            return false;
        }
        if (!initService(config,options)) {
            LOG_ERROR << "init global service error";
            return false;
        }
        if (!initializeExecutor(config)) {
            LOG_ERROR << "Graph init executors error";
            return false;
        }
        if (!initializeNode(config)){
            LOG_ERROR << "Graph init nodes error";
            return false;
        }
        if (!validateEdges()) {
            LOG_ERROR << "Graph init edges fail";
            return false;
        }
        m_state = INITIALIZED;
        LOG_INFO << "graph initialize success";
        return true;
    }

    bool Graph::initializeNode(GraphConfig &config) {
        std::deque<NodeConfig>& nodes = config.nodes;

        for (NodeConfig& nodeConfig : nodes) {
            parseNodeTag(nodeConfig, true);
            parseNodeTag(nodeConfig, false);
            Node *newNode = new Node(new NodeContext(m_edgesMap, nodeConfig ,
                                                     *mGlobalServiceManager,mExpander.get()));
            if (!newNode -> initialize()){
                LOG_ERROR << "functor:" << nodeConfig.functor << " name:" << nodeConfig.name << " init fail";
                return false;
            }
            m_executeManger->assignNodeToExecutor(newNode,nodeConfig.executor);
            m_nodesMap[nodeConfig.name] = std::unique_ptr<Node>(newNode);
            LOG_DEBUG << "add node:" << nodeConfig.name;
        }

        // ???????????????node???????????????????????????????????????node???option?????????node
        if (!m_optionsSave.empty()) {
            for (auto& option : m_optionsSave) {
                setOptionToNode(option.begin()->first,option.begin()->second);
            }
        }
        return true;
    }

    bool Graph::parseNodeTag(const NodeConfig& config , bool isInput) {
        std::string nodeName = config.name;
        auto& target = isInput ? config.inputs : config.outputs;

        std::map<std::string,int> tagNum;
        for (auto& tagIndexName : target) {
            int index = tagIndexName.index;
            if (index == -1) {
                // ??????????????????index?????????port??????index??????????????????????????????index???tag???DataStreamManager?????????????????????????????????
                index = tagNum[tagIndexName.tag];
                tagNum[tagIndexName.tag]++;
            }
            auto* newPort = new Edge::Port{tagIndexName.tag,nodeName,index};
            if (m_edgesMap.find(tagIndexName.name) == m_edgesMap.end()) {
                Edge* edge = new Edge;
                edge->name = tagIndexName.name;
                edge->dataStream = make_unique<DataStream>();
                m_edgesMap[edge->name] = std::unique_ptr<Edge>(edge);
            }
            std::unique_ptr<Edge>& edge = m_edgesMap[tagIndexName.name];
            (isInput ? edge->outputPort : edge->inputPort) = std::unique_ptr<Edge::Port>(newPort);
        }
        return true;
    }

    bool Graph::initializeExecutor(GraphConfig &config) {
        auto& executors = config.executors;
        bool newExecutor = false;
        for (auto &executorConfig : executors) {
            auto executorName = executorConfig.executor;
            auto executor = CreateObjectByName<Executor>(executorName);
            if (executor == nullptr) {
                LOG_INFO << "Executor:" << executorName << " create fail";
                continue;
            }
            if (!executor->initialize(executorConfig)) {
                LOG_INFO << "Executor:" << executorName << " create fail";
                continue;
            }
            m_executeManger->addExecutor(executorName, executor.release(), executorConfig.name == "default");
            newExecutor = true;
        }

        // ????????????????????????executor????????????????????????executor
        if (!newExecutor) {
            auto defaultExecutor = CreateObjectByName<Executor>("ThreadPoolExecutor");
            if (defaultExecutor == nullptr) {
                LOG_ERROR << "no executor,init default executor error";
                return false;
            }
            ExecutorConfig _config;
            _config.executor = "ThreadPoolExecutor";
            _config.name = "default";
            _config.options.insert({"threadNum", Data::create(new int(1))});
            defaultExecutor->initialize(_config);
        }
        return true;
    }

    bool Graph::validateEdges() {
        for (auto & i : m_edgesMap) {
            if (i.second->outputPort == nullptr || i.second->inputPort == nullptr) {
                return false;
            }
        }
        return true;
    }

    Graph::~Graph() {
        // ???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
        LOG_DEBUG << "graph destroy";
        // ????????????node???service??????????????????????????????????????????service????????????????????????node??????????????????????????????????????????
        // ????????????????????????Graph???NodeMap?????????????????????????????????????????????????????????????????????????????????????????????????????????node????????????
        m_nodesMap.clear();
    }

    bool Graph::run() {
        m_executeManger->start();
        for (auto& p : m_nodesMap) {
            p.second->setExecuteSelfHandler(std::bind(&Graph::executeNode,this,p.second.get()));
            p.second->setExecuteConnectNodeHandler(std::bind(&Graph::executeConnectNode,this,p.second.get()));
            if (!p.second->run()) {
                LOG_ERROR << "run graph error";
                return false;
            }
        }
        m_state = RUNNING;
        LOG_INFO << "run graph success";
        return true;
    }

    void Graph::executeNode(Node *node) {
        m_executeManger->executeNode(node);
    }

    void Graph::executeConnectNode(Node *node) {
        const std::vector<std::string>& outputs = node->getOutputs();
        for (const std::string &edgeName : outputs) {
            if (m_edgesMap.find(edgeName) == m_edgesMap.end()) {
                LOG_ERROR << "[node:" << node->getName() << ",edge:" << edgeName << "] not found" ;
                continue;
            }
            std::unique_ptr<Edge>& edge = m_edgesMap[edgeName];
            if (edge->dataStream->empty()) {
                // ????????????????????????????????????????????????
                continue;
            }
            Node* nextNode = m_nodesMap[edge->outputPort->node].get();
            m_executeManger->executeNode(nextNode);
        }
    }

    bool Graph::stop() {
        if (m_state == TERMINATED) {
            return true;
        }
        // ??????????????????????????????????????????
        m_executeManger->cancel();
        // ???????????????node
        for (auto& p : m_nodesMap) {
            p.second->stop();
        }
        // ????????????
        m_executeManger->stop();
        m_state = TERMINATED;
        return true;
    }

    void Graph::setOption(const std::string &nodeName,const OptionMap options) {
        if (m_state != RUNNING) {
            std::map<std::string,OptionMap> m;
            m[nodeName] = options;
            m_optionsSave.emplace_back(m);
        } else {
            setOptionToNode(nodeName,options);
        }
    }

    void Graph::setOptionToNode(const std::string &nodeName, const OptionMap &options) {
        if (m_nodesMap.find(nodeName) == m_nodesMap.end()) {
            LOG_ERROR << "setOption fail." << nodeName << " not found";
            return;
        }
        std::unique_ptr<Node>& node = m_nodesMap[nodeName];
        node->setOption(options);
    }

    bool Graph::initService(GraphConfig& config,const OptionMap& options) {
        mGlobalServiceManager = std::unique_ptr<ServiceManager>(new ServiceManager);
        if (!mGlobalServiceManager->init(config,options)) {
            return false;
        }
        LOG_DEBUG << "init global service success";
        return true;
    }

    bool Graph::release() {
        return true;
    }


}