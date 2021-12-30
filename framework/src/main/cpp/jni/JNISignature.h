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
    // 使用模板根据T的类型来返回类型签名
    // todo 目前仅支持基础数据类型和字符串
    template<typename ...T>
    struct SignatureParam{};

    template<> struct SignatureParam<float> {
        static std::string value() {
            return "F";
        }
    };
    template<> struct SignatureParam<int> {
        static std::string value() {
            return "I";
        }
    };
    template<> struct SignatureParam<void> {
        static std::string value() {
            return "V";
        }
    };
    template<> struct SignatureParam<double> {
        static std::string value() {
            return "D";
        }
    };
    template<> struct SignatureParam<char> {
        static std::string value() {
            return "C";
        }
    };
    template<> struct SignatureParam<long> {
        static std::string value() {
            return "J";
        }
    };
    template<> struct SignatureParam<short> {
        static std::string value() {
            return "S";
        }
    };
    template<> struct SignatureParam<bool> {
        static std::string value() {
            return "Z";
        }
    };
    template<> struct SignatureParam<std::string> {
        static std::string value() {
            return "Ljava/lang/String;";
        }
    };
    template<> struct SignatureParam<JNIObject> {
        static std::string value() {
            return "Ljava/lang/Object;";
        }
    };

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

    // 方法签名，包括返回值签名和参数签名
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
     * 获取方法和属性的签名
     */
    struct JNISignature {
        template<typename T,typename ...Args>
        static jmethodID getMethodId(jclass clazz,const char* methodName) {
            std::string s = MethodSignature<T,Args...>::value();
            return JNIService::getEnv()->GetMethodID(clazz,methodName,s.c_str());
        }

        template<typename T,typename ...Args>
        static jmethodID getStaticMethodId(jclass clazz,const char* methodName) {
            std::string s = MethodSignature<T,Args...>::value();
            return JNIService::getEnv()->GetStaticMethodID(clazz,methodName,s.c_str());
        }

        template<typename T>
        static jfieldID getObjectFiledId(jclass clazz,const char *filedName) {
            std::string filedSignature = SignatureParam<T>::value();
            return JNIService::getEnv()->GetFieldID(clazz,filedName,filedSignature.c_str());
        }

        template<typename T>
        static jfieldID getStaticFiledId(jclass clazz,const char *filedName) {
            std::string filedSignature = SignatureParam<T>::value();
            return JNIService::getEnv()->GetStaticFieldID(clazz,filedName,filedSignature.c_str());
        }
    };
}



#endif //SMEDIA_JNISIGNATURE_H
