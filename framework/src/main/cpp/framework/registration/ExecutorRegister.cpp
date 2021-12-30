//
// Created by wansu on 2021/11/17.
//

#include "ExecutorRegister.h"


namespace smedia {
    #define REGISTER_EXECUTOR_LAZY(name) { \
    extern void register_executor_##name(); \
    register_executor_##name();\
    } \


    using Register = FunctionRegister<std::unique_ptr<Executor>>;
    bool hasRegisterExecutors = false;
    void lazyRegisterAllExecutors();

    void registerExecutor(const std::string& name,std::function<std::unique_ptr<Executor>()> function){
        Register::registerFunction(name,std::move(function));
    }

    bool isRegisterExecutor(const std::string& name) {
        if (!hasRegisterExecutors) {
            lazyRegisterAllExecutors();
            hasRegisterExecutors = true;
        }
        return Register::isRegister(name);
    }

    std::unique_ptr<Executor> createExecutorByName(const std::string& name) {
        if (!hasRegisterExecutors) {
            lazyRegisterAllExecutors();
            hasRegisterExecutors = true;
        }
        return Register::invokeFunction(name);
    }

    void lazyRegisterAllExecutors() {
        REGISTER_EXECUTOR_LAZY(ThreadPoolExecutor)

    }


}

