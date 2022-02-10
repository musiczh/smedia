//
// Created by wansu on 2022/1/28.
//

#ifndef SMEDIA_ISERVICE_H
#define SMEDIA_ISERVICE_H
#include "typeDef.h"
namespace smedia {
    /**
     * 可供全局functor获取的一些相关服务，通过initGraph时传入的option进行初始化
     * 外部用户自行实现，此处为接口
     */
    class IService {
    public:
        virtual bool init(OptionMap options) = 0;
    };
}



#endif //SMEDIA_ISERVICE_H
