//
// Created by wansu on 2021/11/10.
//

#include "ThreadPool.h"

namespace smedia {
    ThreadPool::ThreadPool(int threadNum) {
        _state = ThreadPoolState::CREATED;
        _threadNum = threadNum <= 0 ? 1 : threadNum;
    }

    void ThreadPool::start() {
        for (int i = 0; i < _threadNum; ++i) {
            auto *worker = new Worker("");
            worker->doWork([this](){
                this->runWorker();
            });
            _workerQueue.emplace_back(std::unique_ptr<Worker>(worker));
        }
        // 标记为running状态，可以接收任务
        _state = ThreadPoolState::RUNNING;
    }

    void ThreadPool::runWorker() {
        while(_state < ThreadPoolState::STOPPED) {
            Task newTask;
            {
                std::unique_lock<std::mutex> lock(_mainLock);
                while(_taskQueue.empty() && _state == ThreadPoolState::RUNNING){
                    _condition.wait(lock);
                }
                if (_taskQueue.empty() || _state > ThreadPoolState::STOPPING) {
                    // 如果队列为空表示此时线程池状态不是运行状态，直接退出worker
                    // 或者状态大于stopping，直接退出
                    break;
                }
                newTask = _taskQueue.front();
                _taskQueue.pop_front();
            }
            newTask();
        }
        // 线程退出时，线程数减一。当线程数为0时，标记线程池为stopped状态
        _threadNum--;
        if (_threadNum == 0) {
            _state = ThreadPoolState::STOPPED;
        }
    }

    void ThreadPool::addTask(const ThreadPool::Task& newTask) {
        if (_state != ThreadPoolState::RUNNING) {
            LOG_INFO << "threadPool is not running";
            return ;
        }
        std::unique_lock<std::mutex> lock(_mainLock);
        _taskQueue.emplace_back(newTask);
        _condition.notify_one();
    }

    ThreadPool::~ThreadPool() {
        _state = ThreadPoolState::STOPPED;
        _condition.notify_all();
        // 采用join的方式，采用detach的话会导致ThreadPool被释放，无法获取到ThreadPool的数据
        joinWorker();
    }

    void ThreadPool::detachWorker() {
        std::for_each(_workerQueue.begin(),_workerQueue.end(),[](std::unique_ptr<Worker>& ptr){
            ptr->_thread->detach();
        });
    }

    void ThreadPool::joinWorker() {
        std::for_each(_workerQueue.begin(),_workerQueue.end(),[](std::unique_ptr<Worker>& ptr){
            ptr->_thread->join();
        });
    }


    void ThreadPool::Worker::doWork(const std::function<void()>& work) {
        _thread = std::unique_ptr<std::thread>(new std::thread([this,work](){
            LOG_INFO << this->_workerName << " start" ;
            work();
            LOG_INFO << this->_workerName << " stop";
        }));
    }

    ThreadPool::Worker::Worker(std::string threadName):_workerName(std::move(threadName)) {
    }


}




