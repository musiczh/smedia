//
// Created by wansu on 2021/11/14.
//

#include "ThreadPoolExecutor.h"
namespace smedia{
    bool ThreadPoolExecutor::initialize(ExecutorConfig& config) {
        int threadNum;
        OptionMap map = config.options;
        if (map.find("threadNum") != map.end() && map[OPTION_THREAD_NUM].getData(threadNum)) {
            // 参数保护
            LOG_INFO << "threadNum = " << threadNum;
            threadNum = threadNum < 1 ? 1 : threadNum;
            m_threadPool = make_unique<ThreadPool>(threadNum);
            m_threadPool->start();
            LOG_INFO << "init ThreadPoolExecutor success";
            return true;
        }
        return false;
    }

    void ThreadPoolExecutor::execute(std::function<void()> task) {
        m_threadPool->addTask(task);
    }

    ThreadPoolExecutor::~ThreadPoolExecutor() {
        LOG_INFO << "thread pool destruct";
    }

    REGISTER_EXECUTOR(ThreadPoolExecutor)
}

