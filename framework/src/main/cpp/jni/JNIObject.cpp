//
// Created by wansu on 2021/12/23.
//

#include "JNIObject.h"

namespace smedia {
    JNIObject::JNIObject(const jobject object) {
        objectPtr = std::shared_ptr<_jobject>(JNIService::getEnv()->NewGlobalRef(object),[](jobject p){
            JNIService::getEnv()->DeleteGlobalRef(p);
        });
    }

    JNIObject::JNIObject(const JNIObject & object) {
        objectPtr = std::shared_ptr<_jobject>(object.objectPtr);
    }

    jobject JNIObject::getJObject() {
        return objectPtr.get();
    }

    JNIObject &JNIObject::operator=(const JNIObject & jniObject) {
        if (&jniObject == this) {
            return *this;
        }
        objectPtr = jniObject.objectPtr;
        return *this;
    }


}

