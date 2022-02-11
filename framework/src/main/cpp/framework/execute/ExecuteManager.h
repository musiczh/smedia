//
// Created by wansu on 2021/11/3.
//
/**
 * ExecutorManager和ExecuteQueue绑定，每一个ExecuteQueue绑定一个Executor。
 * ExecutorManager通过塞任务给Queue来实现让Executor执行Node的任务
 */

#ifndef SMEDIA_EXECUTEMANAGER_H
#define SMEDIA_EXECUTEMANAGER_H
#include "typeDef.h"
#include "ExecuteQueue.h"
#include "Logger.h"
namespace smedia {
    class Executor;

    class ExecuteManager {
        enum State {
            CREATED,PAUSE,RUNNING,STOPPING,TERMINATED
        };
    public:
        ExecuteManager();
        ExecuteManager(const ExecuteManager&) = delete;
        ExecuteManager& operator=(const ExecuteManager&) = delete;
        ~ExecuteManager();

        // 开始执行，并提交所有等待执行的node
        void start();
        // 恢复运行，此时所有等待执行的node会被执行
        void resume();
        // 暂停运行，这时候新添加的node不会运行
        void pause();
        // 清空所有任务，停止接收新的任务
        void stop();
        // 用与在暂停阶段关闭，这时候需要提交所有的待提交任务
        // 调用后停止接收新node
        void cancel();

        void addExecutor(const std::string& name,Executor* executor,bool isDefault);
        void assignNodeToExecutor(Node* node,const std::string& executorName);

        void executeNode(Node* node);

    private:
        void setQueueState(bool state);
        void clearQueue();
        void submitQueueWaitingTask();
    private:
        std::unique_ptr<ExecuteQueue> m_defaultExecuteQueue;
        std::map<std::string,std::unique_ptr<ExecuteQueue>> m_queueMap;
        std::map<Node*,std::string> m_nodeMapping;

        volatile State m_state;
    };
}



#endif //SMEDIA_EXECUTEMANAGER_H
