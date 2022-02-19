//
// Created by wansu on 2021/12/23.
//

#include "JNIObject.h"

namespace smedia {
    JNIObject::JNIObject(const jobject object) {
        if (object == nullptr) {
            objectPtr = nullptr;
            return;
        }
        objectPtr = std::shared_ptr<_jobject>(JNIService::getEnv()->NewGlobalRef(object),[](jobject p){
            JNIService::getEnv()->DeleteGlobalRef(p);
        });
    }

    JNIObject::JNIObject(const JNIObject & object) {
        objectPtr = std::shared_ptr<_jobject>(object.objectPtr);
    }

    jobject JNIObject::getJObject() const{
        return objectPtr.get();
    }

    JNIObject &JNIObject::operator=(const JNIObject & jniObject) {
        if (&jniObject == this) {
            return *this;
        }
        objectPtr = jniObject.objectPtr;
        return *this;
    }

    jobject JNIObject::obtainNewLocalRef() {
        return JNIService::getEnv()->NewLocalRef(objectPtr.get());
    }


}

