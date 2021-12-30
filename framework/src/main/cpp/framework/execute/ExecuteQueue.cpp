//
// Created by wansu on 2021/11/15.
//

#include "ExecuteQueue.h"

namespace smedia {
    void smedia::ExecuteQueue::executeNextTask() {
        Node* node;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_queue.empty()) {
                return ;
            }
            node = m_queue.front();
            m_queue.pop_front();
        }
        node->execute();
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            // 如果队列空了，说明执行完此任务没有新的任务，队列进行空闲，调用idleCallback
            if (m_queue.empty() && m_idleCallback) {
               m_idleCallback(true);
               m_isIdle = true;
            }
        }
    }

    void ExecuteQueue::addNode(Node *node) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            // 队列从idle转为work状态，回调idleCallback
            if (m_isIdle && m_queue.empty()){
                m_idleCallback(false);
                m_isIdle = false;
            }
            m_queue.emplace_back(node);
            if (!m_running) {
                // 如果queue还没run，则先记录下来，不交给executor执行
                m_pendingTaskNum++;
                return;
            }
        }
        addTaskToExecutor();
    }

    void ExecuteQueue::setExecutor(Executor *executor) {
        m_executor = std::unique_ptr<Executor>(executor);
    }

    void ExecuteQueue::setRunning(bool isRunning) {
        // 加锁防止多个线程调用setRunning，or 在清空等待队列的时候有新的node添加进来导致错误
        std::unique_lock<std::mutex> lock(m_mutex);
        bool lastState = m_running;
        m_running = isRunning;
        lock.unlock();

        if (!lastState && m_running) {
            // 进行running状态，提交所有等待的任务
            submitWaitingTask();
        }
    }

    void ExecuteQueue::addTaskToExecutor() {
        // 如果queue不处于running状态，添加的新任务不会给executor执行
        if (m_executor!=nullptr) {
            m_executor->addTask(this);
        }
    }

    void ExecuteQueue::clear() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_queue.empty()) {
            m_queue.pop_front();
        }
    }

    void ExecuteQueue::setIdleCallback(IdleCallback &callback) {
        m_idleCallback = callback;
    }

    void ExecuteQueue::submitWaitingTask() {
        int taskNum;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            taskNum = m_pendingTaskNum;
            m_pendingTaskNum = 0;
        }
        for (int i = 0; i < taskNum; ++i) {
            addTaskToExecutor();
        }
    }

}

