//
// Created by wansu on 2022/1/28.
//

#include "ServiceRegister.h"
namespace smedia {
    #define REGISTER_SERVICE_LAZY(name) { \
    extern void register_service_##name(); \
    register_service_##name();\
    } \


    using Register = FunctionRegister<std::unique_ptr<IService>>;
    bool hasRegisterService = false;
    void lazyRegisterAllService();

    void registerService(const std::string& name,std::function<std::unique_ptr<IService>()> function){
        Register::registerFunction(name,std::move(function));
    }

    bool IsRegisterService(const std::string& name) {
        if (!hasRegisterService) {
            lazyRegisterAllService();
            hasRegisterService = true;
        }
        return Register::isRegister(name);
    }

    std::unique_ptr<IService> CreateServiceByName(const std::string& name) {
        if (!hasRegisterService) {
            lazyRegisterAllService();
            hasRegisterService = true;
        }
        return Register::invokeFunction(name);
    }

    void lazyRegisterAllService() {
        REGISTER_SERVICE_LAZY(GLContext)

    }
}