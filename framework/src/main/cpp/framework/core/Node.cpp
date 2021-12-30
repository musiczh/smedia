//
// Created by wansu on 2021/11/3.
//

#include "Node.h"

namespace smedia {

    Node::Node(NodeContext *context) {
        name = context->nodeConfig.name;
        m_nodeContext.reset(context);
        m_state = CREATED;
    }

    bool Node::initialize() {
        std::unique_lock<std::mutex> lock(m_lock);
        NodeConfig& config = m_nodeContext->nodeConfig;
        for (auto& input : config.inputs) {
            m_inputEdges.emplace_back(input.name);
        }
        for (auto& output : config.outputs) {
            m_outputEdges.emplace_back(output.name);
        }

        // 把node边的所有DataStream添加到functorContext中，让functorContext去进行管理
        std::unique_ptr<IFunctor> functor = createFunctorByName(m_nodeContext->nodeConfig.functor);
        if (functor == nullptr) {
            LOG_ERROR << "[functor:" << m_nodeContext->nodeConfig.functor << "] create failed";
            return false;
        }
        auto functorContext = make_unique<FunctorContext>(m_inputEdges,m_outputEdges,
                                                          m_nodeContext->edgesMap,
                                                          m_nodeContext->globalService);

        m_functorContext = std::move(functorContext);
        m_Functor = std::move(functor);
        return true;
    }

    bool Node::run() {
        m_functorContext->setExecuteSelfHandler(m_executeSelfHandler);
        m_functorContext->setExecuteConnectNodeHandler([this](){
            if (m_state == RUNNING && m_executeConnectHandler != nullptr && m_Functor->isAsynchronous()) {
                m_executeConnectHandler();
            }
        });
        // 一般而言，对于source node来说，init之后就会开始产生数据，所以把node的初始化延迟到run的时候。
        m_Functor->initialize(m_functorContext.get());
        // 修改状态并把之前的option全部设置给functor
        std::unique_lock<std::mutex> lock(m_lock);
        m_state = RUNNING;
        if (m_Functor != nullptr) {
            for (auto& option : m_runtimeOptions) {
                m_Functor->setOption(option.first,option.second);
            }
        }
        return true;
    }

    void Node::setOption(const OptionMap &options) {
        switch (m_state) {
            case TERMINATED: {
                LOG_INFO << "node has stop";
                return;
            }
            case RUNNING: {
                for (auto& option : options) {
                    m_Functor->setOption(option.first,option.second);
                }
                break;
            }
            case CREATED: {
                std::unique_lock<std::mutex> lock(m_lock);
                // node刚创建的时候，option先保存起来
                for (auto& option : options) {
                    m_runtimeOptions.emplace(option);
                }
            }
        }
    }

    Node::~Node() {
        m_Functor->unInitialize(m_functorContext.get());
        LOG_INFO << "node:" << name << " destruct" ;
    }

    void Node::stop() {
        m_state = TERMINATED;
        m_functorContext->setExecuteSelfHandler(nullptr);

    }

    const std::string &Node::getName() const {
        return name;
    }

    const std::vector<std::string> &Node::getOutputs() const {
        return m_outputEdges;
    }

    const std::vector<std::string> &Node::getInputs() const {
        return m_inputEdges;
    }

    void Node::setExecuteSelfHandler(std::function<void()> handler) {
        m_executeSelfHandler = std::move(handler);
    }

    void Node::setExecuteConnectNodeHandler(std::function<void()> handler) {
        m_executeConnectHandler = std::move(handler);
    }

    void Node::execute() {
        if (m_state != RUNNING) {
            LOG_INFO << "node is not running";
            return ;
        }

        bool isSucceed = false;
        if (m_Functor != nullptr && m_functorContext != nullptr){
           isSucceed = m_Functor->execute(m_functorContext.get());
        }

        // 执行成功之后检查并执行后续的node
        if (m_state == RUNNING && m_executeConnectHandler != nullptr && isSucceed && !m_Functor->isAsynchronous()) {
            m_executeConnectHandler();
        }
    }
}

