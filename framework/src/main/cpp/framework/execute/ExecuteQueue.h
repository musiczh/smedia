//
// Created by wansu on 2021/11/15.
//

#ifndef SMEDIA_EXECUTEQUEUE_H
#define SMEDIA_EXECUTEQUEUE_H
#include "memory"
#include "Node.h"
#include "TaskQueue.h"
#include "Executor.h"

namespace smedia {
    // idleCallback，参数表示是否进入空闲。
    using IdleCallback = std::function<void(bool)>;
    class ExecuteQueue : public TaskQueue {
    public:
        ExecuteQueue() = default;
        // 设置队列的执行器
        void setExecutor(Executor* executor);
        // 添加一个node到队列中，准备被执行
        void addNode(Node* node);
        // 执行下个任务，该方法由Executor调用
        void executeNextTask() override;

        // 清空队列所有任务。线程池中的任务不会清空，但线程池执行过程会判断队列是否为空
        void clear();
        // 设置队列的执行状态
        void setRunning(bool isRunning);
        void submitWaitingTask();

        void setIdleCallback(IdleCallback & callback);
    private:
        std::deque<Node*> m_queue;
        std::unique_ptr<Executor> m_executor;
        std::mutex m_mutex;
        volatile int m_pendingTaskNum;
        volatile bool m_running;
        volatile bool m_isIdle;

        IdleCallback m_idleCallback;
    private:
        void addTaskToExecutor();

    };
}



#endif //SMEDIA_EXECUTEQUEUE_H
