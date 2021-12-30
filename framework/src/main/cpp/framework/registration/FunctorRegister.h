//
// Created by wansu on 2021/11/17.
//

#ifndef SMEDIA_FUNCTORREGISTER_H
#define SMEDIA_FUNCTORREGISTER_H

#include <string>
#include "FunctionRegister.h"
#include "internal.h"
#include "IFunctor.h"
namespace smedia {
    class IFunctor;

    void registerFunctor(const std::string& nodeName, std::function<std::unique_ptr<IFunctor>()> function);
    std::unique_ptr<IFunctor> createFunctorByName(std::string &nodeName);
    bool isRegisterFunctor(std::string &nodeName);

    // 每个functor都需要定义这个宏，FunctorRegister调用每个functor的方法来注册functor
#define REGISTER_FUNCTOR(functorName) \
void register_functor_##functorName (){\
    registerFunctor(#functorName,make_unique<functorName>);\
} \


}



#endif //SMEDIA_FUNCTORREGISTER_H
