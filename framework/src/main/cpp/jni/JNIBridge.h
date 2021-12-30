//
// Created by wansu on 2021/12/24.
//

#ifndef SMEDIA_JNIBRIDGE_H
#define SMEDIA_JNIBRIDGE_H
#include "Data.h"
#include <jni.h>
#include "JNIObject.h"
#include "JClassManager.h"
#include "JNISignature.h"
#include "vector"

/**
 * 主要做c++层调用java层方法，以及jni属性转化为c++属性
 */
namespace smedia {
    class JNIData {
    public:
        /**
         * 把java的object对象转化为Data 目前仅支持基础数据类型，其他均转为JNIObject
         * @param object
         * @return 存储的Data对象
         */
        static Data jObjectToData(jobject object);

        // 把jString转化为c++的string
        static std::string jStringToCString(jstring object);

        static std::vector<float> jFloatArrayToVector(jfloatArray floatArray);
    };

    /**
     * JNICaller的设计是希望能有一个统一的接口可以提供给外部，通过设置模板来决定方法的返回值以及参数
     * 由于参数是可变的，所以这里不能够使用函数，因为函数不能部分特例化
     * 同时针对不同的返回值类型需要有不同的处理，这里采用特例化来分开处理
     *
     * 默认的返回值是JNIObject，特例化了几个常用基本数据类型:int、float、long、string、void、JNIObject，其他默认为JNIObject
     * @tparam Args 函数参数模板，第一个泛型指定为返回值类型，若指定的类型不在特例化范围内，则默认返回值类型为JNIObject
     */
    template<typename... Args>
    class JNICaller {
    public:
        static JNIObject callStaticMethod(jclass clazz,jmethodID methodId,Args... args){
            jobject res = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,args...);
            JNIObject resObject(res);
            return resObject;
        };
        static JNIObject callObjectMethod(jobject object,jmethodID methodId,Args... args){
            jobject res = JNIService::getEnv()->CallObjectMethod(object,methodId,args...);
            JNIObject resObject(res);
            return resObject;
        };
        static JNIObject getObjectField(jobject object,jfieldID fieldId,Args... args){
            jobject res = JNIService::getEnv()->GetObjectField(object,fieldId,args...);
            JNIObject resObject(res);
            return resObject;
        };
    };

    template<typename... Args>
    class JNICaller<JNIObject,Args...> {
    public:
        static JNIObject callStaticMethod(jclass clazz,jmethodID methodId,Args... args){
            jobject res = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,args...);
            JNIObject resObject(res);
            return resObject;
        };
        static JNIObject callObjectMethod(jobject object,jmethodID methodId,Args... args){
            jobject res = JNIService::getEnv()->CallObjectMethod(object,methodId,args...);
            JNIObject resObject(res);
            return resObject;
        };
        static JNIObject getObjectField(jobject object,jfieldID fieldId,Args... args){
            jobject res = JNIService::getEnv()->GetObjectField(object,fieldId,args...);
            JNIObject resObject(res);
            return resObject;
        };
    };

    template<typename... Args>
    class JNICaller<void,Args...> {
    public:
        static void callStaticMethod(jclass clazz,jmethodID methodId,Args... args){
            JNIService::getEnv()->CallStaticVoidMethod(clazz,methodId,args...);
        };
        static void callObjectMethod(jobject object,jmethodID methodId,Args... args){
            JNIService::getEnv()->CallVoidMethod(object,methodId,args...);
        };
    };

    template<typename... Args>
    class JNICaller<float,Args...> {
    public:
        static float callStaticMethod(jclass clazz,jmethodID methodId,Args... args){
            return JNIService::getEnv()->CallStaticFloatMethod(clazz,methodId,args...);
        };
        static float callObjectMethod(jobject object,jmethodID methodId,Args... args){
            return JNIService::getEnv()->CallFloatMethod(object,methodId,args...);
        };
        static float getObjectField(jobject object,jfieldID fieldId,Args... args){
            return JNIService::getEnv()->GetFloatField(object,fieldId,args...);
        };
    };

    template<typename... Args>
    class JNICaller<int,Args...> {
    public:
        static int callStaticMethod(jclass clazz,jmethodID methodId,Args... args){
            return JNIService::getEnv()->CallStaticIntMethod(clazz,methodId,args...);
        };
        static int callObjectMethod(jobject object,jmethodID methodId,Args... args){
            return JNIService::getEnv()->CallIntMethod(object,methodId,args...);
        };
        static int getObjectField(jobject object,jfieldID fieldId,Args... args){
            return JNIService::getEnv()->GetIntField(object,fieldId,args...);
        };
    };

    template<typename... Args>
    class JNICaller<long,Args...> {
    public:
        static long callStaticMethod(jclass clazz,jmethodID methodId,Args... args){
            return JNIService::getEnv()->CallStaticLongMethod(clazz,methodId,args...);
        };
        static long callObjectMethod(jobject object,jmethodID methodId,Args... args){
            return JNIService::getEnv()->CallLongMethod(object,methodId,args...);
        };
        static long getObjectField(jobject object,jfieldID fieldId,Args... args){
            return JNIService::getEnv()->GetLongField(object,fieldId,args...);
        };
    };

    template<typename... Args>
    class JNICaller<std::string,Args...> {
    public:
        static std::string callStaticMethod(jclass clazz,jmethodID methodId,Args... args){
            jobject object = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,args...);
            jstring res = reinterpret_cast<jstring>(object);
            return JNIData::jStringToCString(res);
        };
        static std::string callObjectMethod(jobject object,jmethodID methodId,Args... args){
            jobject _object = JNIService::getEnv()->CallObjectMethod(object,methodId,args...);
            jstring res = reinterpret_cast<jstring>(_object);
            return JNIData::jStringToCString(res);
        };
        static std::string getObjectField(jobject object,jfieldID fieldId,Args... args){
            jobject _object = JNIService::getEnv()->GetObjectField(object,fieldId,args...);
            jstring res = reinterpret_cast<jstring>(_object);
            return JNIData::jStringToCString(res);
        };
    };

}



#endif //SMEDIA_JNIBRIDGE_H
