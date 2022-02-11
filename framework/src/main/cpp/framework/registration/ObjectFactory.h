//
// Created by wansu on 2022/2/11.
//

#ifndef SMEDIA_OBJECTFACTORY_H
#define SMEDIA_OBJECTFACTORY_H
namespace smedia {
    /**
     * todo 这个类的构想是，任何需要反射的类，只需要继承此类即可。但是考虑到这些对象都有一个共同的基类，如Functor都继承自IFunctor
     *  无法将创建之后的对象都存储为IFunctor类型指针
     * @tparam T
     */
    template<typename T>
    class ObjectFactory {
    public:
        virtual ~ObjectFactory() = default;
        // 在构造函数中调用一下静态变量，触发其初始化
        ObjectFactory();

        // 利用静态变量的初始化来注册类
        struct _ObjectRegisterTask {
            _ObjectRegisterTask();
            void doNothing();
        };
        static _ObjectRegisterTask _mRegisterTask;
    };

    using _ObjectRegister = FunctionRegister<std::unique_ptr<ObjectFactory>>;

    template<typename T>
    inline std::unique_ptr<T> CreateObjectByName(const std::string& name) {
        return std::unique_ptr<T>(dynamic_cast<T*>(_ObjectRegister::invokeFunction(name).release()));
    }

    template<typename T>
    typename ObjectFactory<T>::_ObjectRegisterTask ObjectFactory<T>::_mRegisterTask{};


    template<typename T>
    ObjectFactory<T>::ObjectFactory() {
        _mRegisterTask.doNothing();
    }

    template<typename T>
    typename ObjectFactory<T>::_ObjectRegisterTask::_ObjectRegisterTask() {
        // 获取类型的全限定名称，如smedia::Functor
        std::string typeName = getTypeName(abi::__cxa_demangle(typeid(T).name(),
                                                               nullptr,
                                                               nullptr,
                                                               nullptr));
        if (!typeName.empty()) {
            _ObjectRegister::registerFunction(typeName, make_unique<T>);
        }
    }

    template<typename T>
    typename ObjectFactory<T>::_ObjectRegisterTask::doNothing() {
    }
}

#endif //SMEDIA_OBJECTFACTORY_H
