//
// Created by wansu on 2021/11/15.
//

#ifndef SMEDIA_TASKQUEUE_H
#define SMEDIA_TASKQUEUE_H
namespace smedia {
    class TaskQueue {
    public:
        virtual void executeNextTask() = 0;
        virtual ~TaskQueue() = default;
    };

}
#endif //SMEDIA_TASKQUEUE_H
