//
// Created by wansu on 2022/2/11.
//

#ifndef SMEDIA_DYNAMICOBJECT_H
#define SMEDIA_DYNAMICOBJECT_H
#include <string>
#include "FunctionRegister.h"
#include "DynamicObject.h"
#include "internal.h"
#include <cxxabi.h>
namespace smedia {
    /**
    * 所有需求通过类名创建对象的类的基类
    */
    class DynamicObject {

    public:
        virtual ~DynamicObject() = default;
    };


}
#endif //SMEDIA_DYNAMICOBJECT_H
