//
// Created by wansu on 2022/1/28.
//

#ifndef SMEDIA_SERVICEMANAGER_H
#define SMEDIA_SERVICEMANAGER_H
#include "ServiceRegister.h"
#include "Service.h"
#include "GraphConfig.h"
namespace smedia {
    /**
     * 管理全局functor需要的service。这里的service指的是全局functor共享的类对象，需要继承service接口
     * 在json中进行配置
     */
    class ServiceManager {
    public:
        ServiceManager() = default;
        ~ServiceManager();
        /**
         * 根据json中的配置来初始化service
         * @param graphConfig 图的json配置
         * @param options 这里的options来自两个地方，json和init Graph的时候传入的map
         * @return 是否初始化成功
         */
        bool init(GraphConfig& graphConfig,const OptionMap& options);

        /**
         * 根据名称获取一个service对象
         * @param name
         * @return 返回service指针，这里不返回引用的原因是外部还需要二次处理，引用还需要再做转指针处理
         */
        Service* getService(const std::string& name);

    private:
        // 这里使用共享指针的好处是清空map自动释放service
        std::map<std::string,std::shared_ptr<Service>> mServiceMap;
    };
}



#endif //SMEDIA_SERVICEMANAGER_H
