//
// Created by wansu on 2021/12/24.
//

#ifndef SMEDIA_JNISIGNATURE_H
#define SMEDIA_JNISIGNATURE_H
#include <jni.h>
#include <iostream>
#include "JNIObject.h"
/**
 * 获取方法和属性的签名，使用模板统一做映射
 */
namespace smedia {

    /**
     * SignatureParam是一个结构体，使用模板特例化对每个类型进行定义，包含两个静态方法：
     * value() : 获取属性签名
     * convert() : 从c++属性转化为java的属性类型
     * 顶层采用可变模板参数，可以接收多个参数转化为属性签名字符串
     */
    // todo 目前仅支持基础数据类型和字符串
    template<typename ...T>
    struct SignatureParam{};

    // SignatureParam递归拼接所有的参数签名
    template<typename T>
    struct SignatureParam<T> {
        static std::string value() {
            return "unSupportType";
        }
    };

    template<typename T,typename ...Args>
    struct SignatureParam<T,Args...> {
        static std::string value() {
            return SignatureParam<T>::value() + SignatureParam<Args...>::value();
        }
    };

    template<> struct SignatureParam<float> {
        static std::string value() {
            return "F";
        }
        static jfloat convert(float value) {
            return value;
        }
    };
    template<> struct SignatureParam<int> {
        static std::string value() {
            return "I";
        }
        static jint convert(int value) {
            return value;
        }
    };
    template<> struct SignatureParam<void> {
        static std::string value() {
            return "V";
        }
        static void convert() {
        }
    };
    template<> struct SignatureParam<double> {
        static std::string value() {
            return "D";
        }
        static jdouble convert(double value) {
            return value;
        }
    };
    template<> struct SignatureParam<char> {
        static std::string value() {
            return "C";
        }
        static jchar convert(char value) {
            return (jchar)value;
        }
    };
    template<> struct SignatureParam<long> {
        static std::string value() {
            return "J";
        }
        static jlong convert(long value) {
            return value;
        }
    };
    template<> struct SignatureParam<short> {
        static std::string value() {
            return "S";
        }
        static jshort convert(short value) {
            return value;
        }
    };
    template<> struct SignatureParam<bool> {
        static std::string value() {
            return "Z";
        }
        static jboolean convert(bool value) {
            return value;
        }
    };
    template<> struct SignatureParam<std::string> {
        static std::string value() {
            return "Ljava/lang/String;";
        }
        static jstring convert(std::string value) {
            return JNIService::getEnv()->NewStringUTF(value.c_str());
        }
    };
    template<> struct SignatureParam<JNIObject> {
        static std::string value() {
            return "Ljava/lang/Object;";
        }
        static jobject convert(JNIObject value) {
            return value.getJObject();
        }
    };
    template<> struct SignatureParam<JNIObjectRef> {
        static std::string value() {
            return "Ljava/lang/Object;";
        }
        static jobject convert(JNIObjectRef value) {
            return value->getJObject();
        }
    };

    /**
     * 获取方法签名，包括返回值签名和参数签名
     * 第一个泛型为返回值类型，其他为参数类型，对返回值以及参数进行拼接返回方法签名
     */
    template<typename ...T>
    struct MethodSignature {};

    template<typename T,typename ...Args>
    struct MethodSignature<T,Args...> {
        static std::string value() {
            std::string s1 = "(";
            std::string s2 = ")";
            std::string paramSignature = SignatureParam<Args...>::value();
            std::string returnSignature = SignatureParam<T>::value();
            return s1+paramSignature+s2+returnSignature;
        }
    };

    template<typename T>
    struct MethodSignature<T> {
        static std::string value() {
            std::string s1 = "()";
            std::string returnSignature = SignatureParam<T>::value();
            return s1+returnSignature;
        }
    };

    /**
     * 获取方法和属性id
     */
    struct JNISignature {
        template<typename T,typename ...Args>
        static jmethodID getMethodId(jclass clazz,const std::string& methodName) {
            std::string s = MethodSignature<T,Args...>::value();
            return JNIService::getEnv()->GetMethodID(clazz,methodName.c_str(),s.c_str());
        }

        template<typename T,typename ...Args>
        static jmethodID getStaticMethodId(jclass clazz,const std::string& methodName) {
            std::string s = MethodSignature<T,Args...>::value();
            return JNIService::getEnv()->GetStaticMethodID(clazz,methodName.c_str(),s.c_str());
        }

        template<typename T>
        static jfieldID getObjectFiledId(jclass clazz,const std::string&filedName) {
            std::string filedSignature = SignatureParam<T>::value();
            return JNIService::getEnv()->GetFieldID(clazz,filedName.c_str(),filedSignature.c_str());
        }

        template<typename T>
        static jfieldID getStaticFiledId(jclass clazz,const std::string& filedName) {
            std::string filedSignature = SignatureParam<T>::value();
            return JNIService::getEnv()->GetStaticFieldID(clazz,filedName.c_str(),filedSignature.c_str());
        }
    };
}



#endif //SMEDIA_JNISIGNATURE_H
