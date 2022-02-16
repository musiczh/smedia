//
// Created by wansu on 2022/2/11.
//

#ifndef SMEDIA_OBJECTREGISTER_H
#define SMEDIA_OBJECTREGISTER_H
#include <string>
#include "FunctionRegister.h"
#include "DynamicObject.h"
#include "internal.h"
#include <cxxabi.h>
namespace smedia {
    using _ObjectRegister = FunctionRegister<std::unique_ptr<DynamicObject>>;

    /**
     * 需要从类名创建对象的类要在源文件中声明此宏，并继承DynamicObject基类
     * 这里采用静态对象的初始化来对类型进行注册
     */
#ifndef REGISTER_CLASS
#define REGISTER_CLASS(Type)\
struct _ObjectRegisterTask##Type { \
    _ObjectRegisterTask##Type() { \
        _ObjectRegister::registerFunction(#Type, make_unique<Type>); \
    }; \
} _task##Type; // NOLINT
#endif

    /**
     * 调用此方法通过类名来创建对象，并自动转化为指定的类型
     * @tparam T 创建的对象类型
     * @param name 类名
     * @return 返回创建的对象的unique指针，若创建失败返回空指针
     */
    template<typename T>
    inline std::unique_ptr<T> CreateObjectByName(const std::string& name) {
        return std::unique_ptr<T>(dynamic_cast<T*>(_ObjectRegister::invokeFunction(name).release()));
    }

}
#endif //SMEDIA_OBJECTREGISTER_H
