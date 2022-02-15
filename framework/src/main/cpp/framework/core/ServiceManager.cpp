//
// Created by wansu on 2022/1/28.
//

#include "ServiceManager.h"
#include "ObjectRegister.h"
#include "GLContext.h"
namespace smedia {

    bool ServiceManager::init(GraphConfig &graphConfig, const OptionMap& options) {
        if (graphConfig.services.empty()) {
            LOG_DEBUG << "no service";
            return true;
        }
        for (auto& item : graphConfig.services) {
            auto service = CreateObjectByName<IService>(item.service);
            if (service == nullptr) {
                LOG_ERROR << "create " << item.service << " service fail";
                return false;
            }
            // 将全局配置以及json中对service的配置合并在一起交给service进行初始化
            OptionMap optionConfig(options);
            for (auto& i : item.options) {
                optionConfig.insert({i.first,i.second});
            }
            service->init(optionConfig);
            mServiceMap.insert({item.name,std::unique_ptr<IService>(service.release())});
        }
        LOG_DEBUG << "init ServiceManager success";
        return true;
    }

    IService* ServiceManager::getService(const std::string& name) {
        if (mServiceMap.find(name) == mServiceMap.end()) {
            return nullptr;
        }
        return mServiceMap[name].get();
    }

    ServiceManager::~ServiceManager() {
        mServiceMap.clear();
    }
}