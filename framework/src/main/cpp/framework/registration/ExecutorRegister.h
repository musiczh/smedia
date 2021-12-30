//
// Created by wansu on 2021/11/17.
//

#ifndef SMEDIA_EXECUTORREGISTER_H
#define SMEDIA_EXECUTORREGISTER_H
#include "FunctionRegister.h"
#include "Executor.h"
#include "internal.h"

namespace smedia {
    void registerExecutor(const std::string& name,std::function<std::unique_ptr<Executor>()> function);
    std::unique_ptr<Executor> createExecutorByName(const std::string& name);
    bool isRegisterExecutor(const std::string& name);

    // Executor实现类声明此宏来定义注册方法
#define REGISTER_EXECUTOR(name) \
void register_executor_##name() { \
    registerExecutor(#name,make_unique<name>);                                \
}


}


#endif //SMEDIA_EXECUTORREGISTER_H
