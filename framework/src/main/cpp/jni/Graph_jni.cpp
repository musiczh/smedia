#include <jni.h>
#include "Graph.h"
#include "JsonGraphLoader.h"
#include <android/native_window_jni.h>
#include <android/log.h>
#include "JNIService.h"
#include "GLFrame.h"
#include "Logger.h"
#include "JNIBridge.h"
#include "MyExpander.h"
using namespace smedia;

// 调用java层的日志打印
void logInJava(LoggerLevel level,const char * s){
    static auto clazz =
            reinterpret_cast<jclass>(JNIService::getEnv()->NewGlobalRef(JNIService::getEnv()->FindClass("com/example/egldemo/util/LogUtil")));
    static auto method = JNIService::getEnv()->GetStaticMethodID(
            clazz,"Log","(Ljava/lang/String;Ljava/lang/String;I)V");
    jstring tag = JNIService::getEnv()->NewStringUTF("[native] huan");
    jstring msg = JNIService::getEnv()->NewStringUTF(s);
    int level_ = 0;
    switch (level) {
        case ERROR:level_ = 3;
            break;
        case WARN:level_ = 2;
            break;
        case INFO:level_ = 1;
            break;
        case DEBUG:level_ = 0;
            break;
    }
    JNIService::getEnv()->CallStaticVoidMethod(clazz,method,tag,msg,level_);

    JNIService::getEnv()->DeleteLocalRef(tag);
    JNIService::getEnv()->DeleteLocalRef(msg);
}

// 初始化jni相关的工具类
void initJNIUtils(JNIEnv* env) {
    // 初始化jni工具类，必须在其他的jni操作之前初始化
    JNIService::init(env);
    // 配置java层的日志打印
    Logger::setLogFunction(logInJava);
    // 初始化jni类加载,由于classLoader不在实例化的的类型中，这里需要额外单独处理
    jclass clazz = env->FindClass("com/example/frameword/framework/ClassLoaderLib");
    jmethodID methodId = env->GetStaticMethodID(clazz,"onNativeGetClassLoader","()Ljava/lang/ClassLoader;");
    jobject res = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId);
    JNIObject jniObject(res);
    // 需要预加载的类
    std::vector<std::string> v;
    JClassManager::initJavaClass(jniObject.getJObject(), v);
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_frameword_framework_Graph_initNativeGraph(JNIEnv *env, jobject thiz, jstring json,
                                                           jobjectArray keys, jobjectArray values,jint length) {
    initJNIUtils(env);
    auto* graph = new Graph;
    auto* jsonLoader = new smedia::JsonGraphLoader;
    jsonLoader->parseJson(JNIData::jStringToCString(json));
    // todo 这里jni仅实现了部分类型的转换，其他均转为JNIObject
    OptionMap options;
    for (int i=0;i < length;i++) {
        auto key = reinterpret_cast<jstring>(JNIService::getEnv()->GetObjectArrayElement(keys,i));
        Data data = JNIData::jObjectToData(JNIService::getEnv()->GetObjectArrayElement(values,i));
        options[JNIData::jStringToCString(key)] = data;
    }
    graph->initialize(*jsonLoader->getGraphConfig(),options,
                      std::unique_ptr<Expander>(new MyExpander));
    return reinterpret_cast<int64_t>(graph);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_Graph_runNativeGraph(JNIEnv *env, jobject thiz,
                                                          jlong native_graph_handle) {
    auto* graph = reinterpret_cast<Graph*>(native_graph_handle);
    graph->run();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_Graph_setOptionNative(JNIEnv *env, jobject thiz,
                                                           jlong native_graph_handle,jstring nodeName, jstring key,
                                                           jobject value) {
    // todo 这里jni仅实现了部分类型的转换，其他均转为JNIObject
    auto* graph = reinterpret_cast<Graph*>(native_graph_handle);
    Data data = JNIData::jObjectToData(value);
    std::string c_nodeName = JNIData::jStringToCString(nodeName);
    std::string c_keyName = JNIData::jStringToCString(key);
    graph->setOption(c_nodeName,{{c_keyName,data}});
    //LOG_DEBUG << "set option success,node=" << nodeName <<",tag=" << c_keyName ;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_Graph_releaseNativeGraph(JNIEnv *env, jobject thiz,
                                                              jlong native_graph_handle) {
    auto* graph = reinterpret_cast<Graph*>(native_graph_handle);
    graph->release();
    delete graph;
    JClassManager::unInitJavaClass();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_frameword_framework_Graph_stopNativeGraph(JNIEnv *env, jobject thiz,
                                                           jlong native_graph_handle) {
    auto* graph = reinterpret_cast<Graph*>(native_graph_handle);
    graph->stop();
}




