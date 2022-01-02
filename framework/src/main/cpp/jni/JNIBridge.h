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

    //--------------------------JNICaller---------------------------------

    /**
     * JNICaller的设计是希望能有一个统一的接口可以提供给外部，通过设置模板来决定方法的返回值以及参数
     * 由于参数是可变的，所以这里不能够使用函数，因为函数不能部分特例化
     * 同时针对不同的返回值类型需要有不同的处理，这里采用特例化来分开处理
     *
     * 默认的返回值是JNIObject，特例化了几个常用基本数据类型:int、float、long、string、void、bool、JNIObject，其他默认为JNIObject
     * @tparam Args 函数参数模板，第一个泛型指定为返回值类型，若指定的类型不在特例化范围内，则默认返回值类型为JNIObject
     */

    //----------------------JNIInvoker-------------------------
    //----新接口，采用更加易用的接口
    // 目前实现的类型：JNIObject、int、long、bool、std::string、float、double、void
    template<typename ...Args>
    struct JNIInvoker {};

    template<typename ...Args>
    struct JNIInvoker<void,Args...> {
        static void InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<void,Args...>(clazz,methodName);
            JNIService::getEnv()->CallVoidMethod(instance,methodId,args...);
        }
        static void InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<JNIObject,Args...>(clazz,methodName);
            JNIService::getEnv()->CallStaticVoidMethod(clazz,methodId,args...);
        }
    };
    template<typename ...Args>
    struct JNIInvoker<JNIObject,Args...> {
        static JNIObject InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<JNIObject,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallObjectMethod(instance,methodId,args...);
            return JNIObject(objectRes);
        }
        static JNIObject InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<JNIObject,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,args...);
            return JNIObject(objectRes);
        }
        static JNIObject GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<JNIObject>(clazz,fieldName);
            jobject res = JNIService::getEnv()->GetObjectField(instance.getJObject(),fieldId);
            return JNIObject(res);
        }
        static JNIObject GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<JNIObject>(clazz,fieldName);
            jobject res = JNIService::getEnv()->GetStaticObjectField(clazz,fieldId);
            return JNIObject(res);
        }
    };
    template<typename ...Args>
    struct JNIInvoker<long,Args...> {
        static long InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<long,Args...>(clazz,methodName);
            jlong objectRes = JNIService::getEnv()->CallLongMethod(instance,methodId,args...);
            return objectRes;
        }
        static long InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<long,Args...>(clazz,methodName);
            jlong objectRes = JNIService::getEnv()->CallStaticLongMethod(clazz,methodId,args...);
            return objectRes;
        }
        static long GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<long>(clazz,fieldName);
            jlong res = JNIService::getEnv()->GetLongField(instance.getJObject(),fieldId);
            return res;
        }
        static long GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<JNIObject>(clazz,fieldName);
            jlong res = JNIService::getEnv()->GetStaticLongField(clazz,fieldId);
            return res;
        }
    };
    template<typename ...Args>
    struct JNIInvoker<int,Args...> {
        static int InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<int,Args...>(clazz,methodName);
            jint objectRes = JNIService::getEnv()->CallIntMethod(instance,methodId,args...);
            return objectRes;
        }
        static int InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<int,Args...>(clazz,methodName);
            jint objectRes = JNIService::getEnv()->CallStaticIntMethod(clazz,methodId,args...);
            return objectRes;
        }
        static int GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<int>(clazz,fieldName);
            jint res = JNIService::getEnv()->GetIntField(instance.getJObject(),fieldId);
            return res;
        }
        static int GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<JNIObject>(clazz,fieldName);
            jint res = JNIService::getEnv()->GetStaticIntField(clazz,fieldId);
            return res;
        }
    };
    template<typename ...Args>
    struct JNIInvoker<bool,Args...> {
        static bool InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<bool,Args...>(clazz,methodName);
            jboolean objectRes = JNIService::getEnv()->CallBooleanMethod(instance,methodId,args...);
            return objectRes;
        }
        static bool InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<bool,Args...>(clazz,methodName);
            jboolean objectRes = JNIService::getEnv()->CallStaticBooleanMethod(clazz,methodId,args...);
            return objectRes;
        }
        static bool GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<bool>(clazz,fieldName);
            jboolean res = JNIService::getEnv()->GetBooleanField(instance.getJObject(),fieldId);
            return res;
        }
        static bool GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<bool>(clazz,fieldName);
            jboolean res = JNIService::getEnv()->GetStaticBooleanField(clazz,fieldId);
            return res;
        }
    };
    template<typename ...Args>
    struct JNIInvoker<std::string,Args...> {
        static std::string InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<std::string,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallObjectMethod(instance,methodId,args...);
            jstring stringRes = reinterpret_cast<jstring>(objectRes);
            auto res = JNIData::jStringToCString(stringRes);
            return res;
        }
        static std::string InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<std::string,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,args...);
            jstring stringRes = reinterpret_cast<jstring>(objectRes);
            return JNIData::jStringToCString(stringRes);
        }
        static std::string GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<std::string>(clazz,fieldName);
            jobject res = JNIService::getEnv()->GetObjectField(instance.getJObject(),fieldId);
            jstring stringRes = reinterpret_cast<jstring>(res);
            return JNIData::jStringToCString(stringRes);
        }
        static std::string GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<std::string>(clazz,fieldName);
            jobject res = JNIService::getEnv()->GetStaticObjectField(clazz,fieldId);
            jstring stringRes = reinterpret_cast<jstring>(res);
            return JNIData::jStringToCString(stringRes);
        }
    };
    template<typename ...Args>
    struct JNIInvoker<float,Args...> {
        static float InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<float,Args...>(clazz,methodName);
            jfloat objectRes = JNIService::getEnv()->CallFloatMethod(instance,methodId,args...);
            return objectRes;
        }
        static float InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<float,Args...>(clazz,methodName);
            jfloat objectRes = JNIService::getEnv()->CallStaticFloatMethod(clazz,methodId,args...);
            return objectRes;
        }
        static float GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<float>(clazz,fieldName);
            jfloat res = JNIService::getEnv()->GetFloatField(instance.getJObject(),fieldId);
            return res;
        }
        static float GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<float>(clazz,fieldName);
            jfloat res = JNIService::getEnv()->GetStaticFloatField(clazz,fieldId);
            return res;
        }
    };
    template<typename ...Args>
    struct JNIInvoker<double,Args...> {
        static double InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<double,Args...>(clazz,methodName);
            jdouble objectRes = JNIService::getEnv()->CallDoubleMethod(instance,methodId,args...);
            return objectRes;
        }
        static double InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getMethodId<double,Args...>(clazz,methodName);
            jdouble objectRes = JNIService::getEnv()->CallStaticDoubleMethod(clazz,methodId,args...);
            return objectRes;
        }
        static double GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<double>(clazz,fieldName);
            jdouble res = JNIService::getEnv()->GetDoubleField(instance.getJObject(),fieldId);
            return res;
        }
        static double GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<double>(clazz,fieldName);
            jdouble res = JNIService::getEnv()->GetStaticDoubleField(clazz,fieldId);
            return res;
        }
    };

}



#endif //SMEDIA_JNIBRIDGE_H
