//
// Created by wansu on 2021/12/23.
//

#ifndef SMEDIA_JNIOBJECT_H
#define SMEDIA_JNIOBJECT_H
#include <memory>
#include <jni.h>
#include "JNIService.h"
/**
 * 保存jobject的封装类，使用share指针来让所有对象被析构之后，删除jobject的全局引用，避免忘记删除导致的内存泄露
 *
 */
namespace smedia {
    class JNIObject {
    public:
        JNIObject() = default;
        JNIObject(const jobject object);
        JNIObject(const JNIObject&);
        JNIObject& operator=(const JNIObject&);
        jobject getJObject();
    private:
        std::shared_ptr<_jobject> objectPtr;
    };
}



#endif //SMEDIA_JNIOBJECT_H
