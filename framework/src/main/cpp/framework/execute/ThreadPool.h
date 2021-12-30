//
// Created by wansu on 2021/11/10.
//

#ifndef SMEDIA_THREADPOOL_H
#define SMEDIA_THREADPOOL_H
#include <functional>
#include <deque>
#include <mutex>
#include <thread>
#include <memory>
#include "Logger.h"
namespace smedia {

    class ThreadPool {
        using Task = std::function<void()>;
        class Worker;
        enum ThreadPoolState {
            CREATED,RUNNING,STOPPING,STOPPED
        };

    public:
        explicit ThreadPool(int threadNum);
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool& threadPool) = delete;
        ~ThreadPool();

    public:
        void addTask(const Task& newTask);
        void start();

    private:
        void runWorker();
        void detachWorker();
        void joinWorker();

        std::mutex _mainLock;
        std::condition_variable _condition;
        volatile int _threadNum;
        volatile ThreadPoolState _state;
        std::deque<Task> _taskQueue;
        std::deque<std::unique_ptr<Worker>> _workerQueue;

    private:
        class Worker {
        public:
            explicit Worker(std::string workerName);
            Worker(const Worker& worker) = delete;
            Worker& operator=(Worker& worker) = delete;

        public:
            void doWork(const std::function<void()>& work);

        public:
            std::unique_ptr<std::thread> _thread;

        private:
            std::string _workerName;

        };

    };


}


#endif //SMEDIA_THREADPOOL_H
