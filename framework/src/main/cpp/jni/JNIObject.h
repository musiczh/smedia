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
 * c++中所有的jobject都采用此类，包括模板
 */
namespace smedia {
    class JNIObject {
    public:
        JNIObject() = default;
        JNIObject(const jobject object);
        JNIObject(const JNIObject&);
        JNIObject& operator=(const JNIObject&);
        jobject getJObject() const;
        jobject obtainNewLocalRef();
    private:
        std::shared_ptr<_jobject> objectPtr;
    };

    // 再加一层共享指针的原因是，在Data中，如果所有Data被析构则对象会被释放
    // 如果不希望资源被释放得采用二级指针，或者copy Data对象
    using JNIObjectRef = std::shared_ptr<JNIObject>;
}



#endif //SMEDIA_JNIOBJECT_H
