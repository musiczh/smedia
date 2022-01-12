//
// Created by wansu on 2022/1/7.
//

#ifndef SMEDIA_DYNAMICOBJECT_H
#define SMEDIA_DYNAMICOBJECT_H
#include "FunctionRegister.h"
#include "internal.h"
class IFunctor;

namespace smedia {
    template<typename T>
    class DynamicObject {
        using Register = FunctionRegister<std::unique_ptr<IFunctor>>;

        struct InnerStruct {
            InnerStruct() {
                //Register::registerFunction(typeid(T).name(),make_unique<T>());
            }
        };

        static InnerStruct innerStruct;
    };

    template<typename T>
    typename DynamicObject<T>::InnerStruct DynamicObject<T>::innerStruct{};
}



#endif //SMEDIA_DYNAMICOBJECT_H
