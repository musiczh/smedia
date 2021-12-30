//
// Created by wansu on 2021/11/5.
//

#ifndef SMEDIA_EXECUTOR_H
#define SMEDIA_EXECUTOR_H
#include "GraphConfig.h"
#include "TaskQueue.h"


namespace smedia {
    class Executor {
    public:
        virtual bool initialize(ExecutorConfig& config) = 0;
        virtual ~Executor()= default;
        void addTask(TaskQueue* queue){
            execute([queue](){
                queue->executeNextTask();
            });
        }
        virtual void execute(std::function<void()> task) = 0;
    };

}


#endif //SMEDIA_EXECUTOR_H
