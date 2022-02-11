//
// Created by wansu on 2021/11/14.
//

#ifndef SMEDIA_THREADPOOLEXECUTOR_H
#define SMEDIA_THREADPOOLEXECUTOR_H
#include "Executor.h"
#include "ThreadPool.h"
#include "internal.h"
#include "Data.h"
#include "ObjectRegister.h"
namespace smedia {
    #define OPTION_THREAD_NUM "threadNum"
    class ThreadPoolExecutor: public Executor{
    public:
        bool initialize(ExecutorConfig& config) override;
        void execute(std::function<void()> task) override;
        ~ThreadPoolExecutor() override;

    private:
        std::unique_ptr<ThreadPool> m_threadPool;
    };
}



#endif //SMEDIA_THREADPOOLEXECUTOR_H
