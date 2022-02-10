//
// Created by wansu on 2022/1/28.
//

#ifndef SMEDIA_SERVICEREGISTER_H
#define SMEDIA_SERVICEREGISTER_H
#include <string>
#include "FunctionRegister.h"
#include "IService.h"
#include "internal.h"

namespace smedia {
    void registerService(const std::string& name,std::function<std::unique_ptr<IService>()> function);
    std::unique_ptr<IService> CreateServiceByName(const std::string& name);
    bool IsRegisterService(const std::string& name);

    // Service实现类声明此宏来定义注册方法
#define REGISTER_SERVICE(name) \
void register_service_##name() { \
    registerService(#name,make_unique<name>);\
}

}



#endif //SMEDIA_SERVICEREGISTER_H
