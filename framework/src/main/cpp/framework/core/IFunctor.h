//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_IFUNCTOR_H
#define SMEDIA_IFUNCTOR_H
#include "FunctorContext.h"
namespace smedia {
    class IFunctor {
    public:
        virtual bool initialize(FunctorContext* context) = 0;
        virtual void unInitialize(FunctorContext* context) = 0;
        // 返回值表示是否执行后面的functor，返回false则数据不会执行接下来的functor
        virtual bool execute(FunctorContext* context) = 0;
        virtual void setOption(FunctorContext* context,const std::string& key,Data value) = 0;
        /**
         * 重写此方法，改变返回值表示是否异步执行execute,默认为false
         * @return 是否同步执行
         */
        virtual bool isAsynchronous();
        virtual ~IFunctor() = default;
    };

}

#endif //SMEDIA_IFUNCTOR_H
