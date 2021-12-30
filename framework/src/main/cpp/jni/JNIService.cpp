//
// Created by wansu on 2021/12/17.
//

#include "JNIService.h"
namespace smedia {
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


    JNIEnv *JNIService::getEnv() {
        static pthread_once_t once_process = PTHREAD_ONCE_INIT;
        static pthread_key_t thread_key;
        // pthread_once创建一只执行一次的函数
        // pthread_key_create添加一个当前线程退出时必须执行的回调，这里调用detach来分离线程。
        // 否则被attach的线程的引用无法被释放。回调的参数由pthread_setspecific来设置
        pthread_once(&once_process,[]{
            pthread_key_create(&thread_key,[](void* value){
                auto* env = reinterpret_cast<JNIEnv*>(value);
                JavaVM* vm;
                env->GetJavaVM(&vm);
                vm->DetachCurrentThread();
            });
        });
        // 读取存储在当前线程的env指针，读取不到则表示当前线程还未设置env
        // 读取不到有两种情况：已attach线程、未attach线程
        auto* env = reinterpret_cast<JNIEnv*>(pthread_getspecific(thread_key));
        if (env == nullptr) {
            std::unique_lock<std::mutex> lock(vmMutex);
            int envStatus = javaVm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
            switch (envStatus) {
                case JNI_OK:
                    LOGD("current thread had attach")
                    break;
                case JNI_EDETACHED:
                    LOGD("thread has not attach:start attach")
                    if (javaVm->AttachCurrentThread(&env, nullptr) != 0) {
                        LOGE("attach error");
                        break;
                    }
                    LOGD("attach success")
                    break;
                case JNI_EVERSION:
                    LOGE("get env error,cause of java version")
                    break;
                default:
                    LOGE("get env error,cause of unKnow reason")
                    break;
            }
            // 设置当前线程thread_key的存储指针，类似java的Thread_Local的作用，在线程退出回调时被使用
            pthread_setspecific(thread_key,env);
        }
        return env;
    }
}

