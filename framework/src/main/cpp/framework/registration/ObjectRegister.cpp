//
// Created by wansu on 2022/2/18.
//
#include "ObjectRegister.h"

namespace smedia {

    __Register_Task_::__Register_Task_(int) {
        // do nothing
    }

    int __Register_Task_::__register_func_(const std::string &name,
                                           std::function<std::unique_ptr<DynamicObject>()> func) {
        _ObjectRegister::registerFunction(name,func);
        return 0;
    }
}
