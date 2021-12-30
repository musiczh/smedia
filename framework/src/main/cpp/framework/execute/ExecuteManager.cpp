//
// Created by wansu on 2021/11/3.
//

#include "ExecuteManager.h"

namespace smedia {
    void ExecuteManager::start() {
        m_state = RUNNING;
        setQueueState(true);
    }

    ExecuteManager::ExecuteManager() {
        m_state = CREATED;
    }

    void ExecuteManager::resume() {
        if (m_state == PAUSE) {
            m_state = RUNNING;
            setQueueState(true);
        }
    }

    void ExecuteManager::pause() {
        if (m_state == RUNNING) {
            m_state = PAUSE;
            setQueueState(false);
        }
    }

    void ExecuteManager::stop() {
        if (m_state < TERMINATED) {
            m_state = TERMINATED;
            setQueueState(false);
            clearQueue();
        }
    }

    void ExecuteManager::cancel() {
        if (m_state < STOPPING && m_state != CREATED){
            m_state = STOPPING;
            submitQueueWaitingTask();
        }
    }

    ExecuteManager::~ExecuteManager() {
        LOG_INFO << "destruct" ;
    }

    void ExecuteManager::addExecutor(const std::string& name, Executor *executor, bool isDefault) {
        auto* newQueue = new ExecuteQueue();
        newQueue->setExecutor(executor);
        if (isDefault) {
            m_defaultExecuteQueue = std::unique_ptr<ExecuteQueue>(newQueue);
            return ;
        }
        m_queueMap[name] = std::unique_ptr<ExecuteQueue>(newQueue);
    }

    void ExecuteManager::assignNodeToExecutor(Node *node, const std::string& executorName) {
        if (m_queueMap.find(executorName) == m_queueMap.end()) {
            m_nodeMapping[node] = executorName;
        }
    }

    void ExecuteManager::executeNode(Node *node) {
        if (m_state >= STOPPING) {
            LOG_INFO << "ExecutorManager is close";
            return ;
        }
        if (m_nodeMapping.find(node) != m_nodeMapping.end()) {
            const std::string &executorName = m_nodeMapping.find(node)->second;
            if (m_queueMap.find(executorName) != m_queueMap.end()) {
                m_queueMap[m_nodeMapping[node]]->addNode(node);
            } else if (m_defaultExecuteQueue != nullptr && executorName == "default") {
                m_defaultExecuteQueue->addNode(node);
            }else {
                LOG_ERROR << "node:" << node->getName() << " can not execute";
            }
            return;
        }
        LOG_INFO << "node:" <<node->getName() << " has not assign to executor";
    }

    void ExecuteManager::setQueueState(bool state) {
        for (auto& p : m_queueMap) {
            p.second->setRunning(state);
        }
        if (m_defaultExecuteQueue != nullptr) {
            m_defaultExecuteQueue->setRunning(state);
        }
    }

    void ExecuteManager::clearQueue() {
        for (auto& p : m_queueMap) {
            p.second->clear();
        }
    }

    void ExecuteManager::submitQueueWaitingTask() {
        for (auto& p : m_queueMap) {
            p.second->submitWaitingTask();
        }
    }


}

