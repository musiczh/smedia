//
// Created by wansu on 2021/12/14.
//

#ifndef SMEDIA_GLTHREAD_H
#define SMEDIA_GLTHREAD_H
#include <memory>
#include <mutex>
#include <thread>
#include <deque>
#include <atomic>
#include "Logger.h"

namespace smedia {

    class GLThread {
        using Task = std::function<void()>;
        using StatusTask = std::function<bool()>;
    public:
        GLThread();
        ~GLThread();
        bool runSync(StatusTask task);

    private:
        void addTask(Task task);
        Task getTask();
        void runBody();

    private:
        std::mutex mLock;
        std::mutex mQueueLock;
        std::condition_variable mRunCondition;
        std::condition_variable mQueueCondition;
        std::thread mRenderThread;
        std::deque<Task> mTaskQueue;
        std::atomic<bool> mQuit;
    };
}


#endif //SMEDIA_GLTHREAD_H
