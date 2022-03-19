//
// Created by wansu on 2022/2/14.
//

#include "MyExpander.h"
#include "JNIBridge.h"
#include "AndroidBaseFunctor.h"
namespace smedia {
    std::unique_ptr <IFunctor> MyExpander::createExpandFunctor(const std::string &name) {
        auto* androidFunctor = new AndroidBaseFunctor();
        if (!androidFunctor->init(name)) {
            delete androidFunctor;
            return nullptr;
        }
        return std::unique_ptr<IFunctor>(androidFunctor);
    }

    std::unique_ptr<IService> MyExpander::createExpandService(const std::string &name) {
        return nullptr;
    }

}