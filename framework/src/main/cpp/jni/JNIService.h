//
// Created by wansu on 2021/12/17.
//

#ifndef SMEDIA_JNISERVICE_H
#define SMEDIA_JNISERVICE_H
#include <jni.h>
#include <string>
#include <pthread.h>
#include <mutex>
#include <android/log.h>
#include "Data.h"
/**
 * 提供一些基础的jni服务
 */
namespace smedia {


    class JNIService {
    public:
        // 初始化保存javaVM
        static bool init(JNIEnv* env);

        // 获取当前线程的env指针，若当前线程为c++创建的，会进行attach，同时在线程结束时自动detach
        static JNIEnv* getEnv();


    private:
        static JavaVM* javaVm;

        // 同步init和getEnv的vm数据，防止出现一个线程init了另一个线程拿不到vm对象
        static std::mutex vmMutex;

    };


}


#endif //SMEDIA_JNISERVICE_H
