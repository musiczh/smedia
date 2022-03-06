//
// Created by wansu on 2021/12/17.
//

#include "JNIService.h"
#include "ThreadLocal.h"
namespace smedia {
    struct ThreadInfo {
        JNIEnv* env{nullptr};
        bool isAttach{false};
    };

    // 静态属性需要显式初始化
    JavaVM* JNIService::javaVm = nullptr;
    std::mutex JNIService::vmMutex;

    // android平台的log
    const static char* tag = "[huan native jni]";
#define LOGD(msg) __android_log_write(ANDROID_LOG_DEBUG,tag,msg);
#define LOGW(msg) __android_log_write(ANDROID_LOG_WARN,tag,msg);
#define LOGE(msg) __android_log_write(ANDROID_LOG_ERROR,tag,msg);
#define LOGI(msg) __android_log_write(ANDROID_LOG_INFO,tag,msg);



    bool JNIService::init(JNIEnv *env) {
        std::unique_lock<std::mutex> lock(vmMutex);
        if (env->GetJavaVM(&javaVm) != JNI_OK) {
            LOGE("get javaVM error")
            javaVm = nullptr;
            return false;
        }
        LOGD("get javaVM success")
        return true;
    }

    /**
     * 此类的设计是保存JavaVM指针，任何线程调用此函数时会返回当前线程的env指针。
     * 第一次获取会保存env到线程数据中，类似于ThreadLocal，调用的是pthread_getspecific函数
     * 1. 对于c++创建的线程，使用jni之前需要将其attach到javaVM中。而使用结束之后，需要手动detach
     * 2. 对于java创建的线程，这里只需要获取后env保存在线程数据中，线程自动attach和detach
     *
     * 这里的设计思路是保存线程的env和是否是我们主动attach的变量，并设置一个线程退出函数，在线程结束时被调用。此方法是
     * pthread_key_create。这里函数类似于Java的ThreadLocal，这个函数创建的是一个pthread_key_t以及其所对应的析构函数。
     * 所有线程可以拿这个pthread_key_t作为pthread_getspecific的key变量来保存数据，但是不同的线程使用此key读取到数据是不同的
     * 这个非常类似用于java的ThreadLocal，只是java做了封装，让我们创建一个ThreadLocal变量即可。当线程结束时，如果
     * 当前线程设置了pthread_key_t，那么其对应的析构函数会被调用
     *
     * 注意，非c++创建的线程退出时不能调用GetJavaVM等jni函数，因为java会自动detach，导致这里调用jni的时候提示线程未
     * attach env 而闪退。
     *
     * pthread_once指定的PTHREAD_ONCE_INIT表示当前线程只执行一次，所以pthread_key_create只会被执行一次
     * @return JNIEnv
     */
    JNIEnv *JNIService::getEnv() {
        static ThreadLocal<ThreadInfo> localThreadInfo([](ThreadInfo* info){
            if (info != nullptr && info->isAttach) {
                JavaVM* vm;
                info->env->GetJavaVM(&vm);
                vm->DetachCurrentThread();
            }
        });
        auto* info = localThreadInfo.get();
        if (info == nullptr) {
            std::unique_lock<std::mutex> lock(vmMutex);
            info = new ThreadInfo;
            int envStatus = javaVm->GetEnv(reinterpret_cast<void **>(&info->env), JNI_VERSION_1_6);
            switch (envStatus) {
                case JNI_OK:
                    LOGD("current thread had attach")
                    break;
                case JNI_EDETACHED:
                    LOGD("thread has not attach:start attach")
                    if (javaVm->AttachCurrentThread(&info->env, nullptr) != 0) {
                        LOGE("attach error");
                        break;
                    }
                    info->isAttach = true;
                    LOGD("attach success")
                    break;
                case JNI_EVERSION:
                    LOGE("get env error,cause of java version")
                    break;
                default:
                    LOGE("get env error,cause of unKnow reason")
                    break;
            }
            if (info->env != nullptr) {
                // 只有成功获取到env才保存起来
                localThreadInfo.set(info);
            }
        }
        return info->env;
    }
}

