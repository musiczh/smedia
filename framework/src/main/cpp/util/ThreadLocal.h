//
// Created by wansu on 2022/2/21.
//

#ifndef SMEDIA_THREADLOCAL_H
#define SMEDIA_THREADLOCAL_H
#include <functional>
#include <jni.h>
#include <pthread.h>
#include "Logger.h"
namespace smedia {

    /**
     * 类似于Java中ThreadLocal的作用。一般将此类对象存储到全局可访问区域，不同的线程可以访问到自己存储的数据
     * 注意，存储的数据不会自动释放，需要在destructor析构函数中自己负责释放
     * @tparam T 数据类型
     */
    template<typename T>
    class ThreadLocal {
        struct DataInfo {
            T* value;
            ThreadLocal<T>* threadLocal;
            ~DataInfo();
        };
    public:
        ThreadLocal(std::function<void(T*)> destructor,T* value = nullptr);

    public:
        T* get();
        bool set(T* value);

    private:
        pthread_key_t mKey{};
        std::function<void(T*)> mDestructor;
    };

    template<typename T>
    ThreadLocal<T>::DataInfo::~DataInfo() {
        threadLocal->mDestructor(value);
    }

    template<typename T>
    ThreadLocal<T>::ThreadLocal(std::function<void(T *)> destructor,T* value) {
        mDestructor = destructor;
        pthread_key_create(&mKey,[](void* object){
            auto* info = reinterpret_cast<ThreadLocal<T>::DataInfo *>(object);
            delete info;
        });
        set(value);
    }

    template<typename T>
    bool ThreadLocal<T>::set(T *value) {
        if (value == nullptr) {
            return false;
        }
        auto* info = reinterpret_cast<ThreadLocal<T>::DataInfo*>(pthread_getspecific(mKey));
        if (info == nullptr) {
            info = new DataInfo{value,this};
        } else {
            info->value = value;
        }
        return pthread_setspecific(mKey,info) == 0;
    }

    template<typename T>
    T *ThreadLocal<T>::get() {
        void* object = pthread_getspecific(mKey);
        if (object == nullptr) {
            return nullptr;
        }
        auto* info = reinterpret_cast<ThreadLocal<T>::DataInfo*>(object);
        return info->value;
    }
}



#endif //SMEDIA_THREADLOCAL_H
