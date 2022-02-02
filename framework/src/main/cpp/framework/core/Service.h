//
// Created by wansu on 2022/1/28.
//

#ifndef SMEDIA_SERVICE_H
#define SMEDIA_SERVICE_H
#include "typeDef.h"
namespace smedia {
    /**
     * 可供全局functor获取的一些相关服务，通过initGraph时传入的option进行初始化
     * 外部用户自行实现，此处为接口
     */
    class Service {
    public:
        virtual bool init(OptionMap options) = 0;

        // 转换为具体的类对象
        template<class T>
        T& convert() {
            T* value = dynamic_cast<T*>(this);
            return *value;
        };
    };
}



#endif //SMEDIA_SERVICE_H
