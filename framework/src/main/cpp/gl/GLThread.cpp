//
// Created by wansu on 2021/12/14.
//

#include "GLThread.h"
namespace smedia {

    GLThread::GLThread() : mRenderThread([this](){
        this->runBody();
    }),mQuit(false){
        // 设置线程的名称，方便debug
        pthread_setname_np(mRenderThread.native_handle(),"native_render_thread");
        LOG_DEBUG << "GLThread create success";
    }

    GLThread::~GLThread() {
        mQuit = true;
        std::unique_lock<std::mutex> lock(mQueueLock);
        // 处理完所有的任务再退出
        mQueueCondition.notify_all();
        lock.unlock();
        mRenderThread.join();
        LOG_DEBUG << "GLThread destroy";
    }

    bool GLThread::runSync(std::function<bool()> task) {
        if (mRenderThread.get_id() == std::this_thread::get_id()) {
            // 如果当前是渲染线程则直接执行，无需塞到队列中去执行
            return task();
        }
        bool hasDone = false;
        bool res = false;
        std::unique_lock<std::mutex> lock(mLock);
        addTask([task,this,&hasDone,&res](){
            res = task();
            hasDone = true;
            std::unique_lock<std::mutex> lock(this->mLock);
            // 无法保证等待顺序，因此需要唤醒所有的线程
            this->mRunCondition.notify_all();
        });
        while (!hasDone) {
            mRunCondition.wait(lock);
        }
        return res;
    }

    void GLThread::runBody() {
        while (true) {
            auto task = getTask();
            if (task == nullptr){
                break;
            }
            task();
        }
        LOG_DEBUG << "quit render thread";
    }

    void GLThread::addTask(GLThread::Task task) {
        std::unique_lock<std::mutex> lock(mQueueLock);
        mTaskQueue.push_back(std::move(task));
        mQueueCondition.notify_all();
    }

    GLThread::Task GLThread::getTask() {
        std::unique_lock<std::mutex> lock(mQueueLock);
        while (mTaskQueue.empty() && !mQuit.load()) {
            mQueueCondition.wait(lock);
        }
        if (mQuit.load()) {
            return nullptr;
        }
        Task newTask = mTaskQueue.front();
        mTaskQueue.pop_front();
        return newTask;
    }

}