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
     */
#ifndef REGISTER_CLASS
#define REGISTER_CLASS(Type)\
    _ObjectRegisterTask<Type> _register_task_##Type{}; // NOLINT
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

    /**
     * 这里采用静态对象的初始化来对类型进行注册
     * @tparam T 需要被注册的类型
     */
    template<class T>
    struct _ObjectRegisterTask {
        _ObjectRegisterTask();
    };

    template<typename T>
    _ObjectRegisterTask<T>::_ObjectRegisterTask() {
        // 获取类型的全限定名称，如smedia::Functor
        std::string typeName = getTypeName(abi::__cxa_demangle(typeid(T).name(),
                                                               nullptr,
                                                               nullptr,
                                                               nullptr));
        if (!typeName.empty()) {
            _ObjectRegister::registerFunction(typeName, make_unique<T>);
        }
    }

}
#endif //SMEDIA_OBJECTREGISTER_H
