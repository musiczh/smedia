//
// Created by wansu on 2021/12/24.
//

#ifndef SMEDIA_JNISIGNATURE_H
#define SMEDIA_JNISIGNATURE_H
#include <jni.h>
#include <iostream>
#include <vector>
#include "JNIObject.h"
#include "JClassManager.h"
/**
 * 获取方法和属性的签名，使用模板统一做映射
 */
namespace smedia {

    /**
     * SignatureParam是一个结构体，使用模板特例化对每个类型进行定义，包含三个静态方法：
     * value() : 获取属性签名，顶层采用可变模板参数，可以接收多个参数转化为属性签名字符串
     * convert() : 从c++属性转化为java的属性，返回java数据
     * convertToC() : 从java属性转化为c++属性，返回c++数据
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
            return value.obtainNewLocalRef();
        }
    };
    template<> struct SignatureParam<JNIObjectRef> {
        static std::string value() {
            return "Ljava/lang/Object;";
        }
        static jobject convert(JNIObjectRef value) {
            return value->obtainNewLocalRef();
        }
    };
    template<> struct SignatureParam<std::vector<float>> {
        static std::string value() {
            return "[F";
        }
        static jfloatArray convert(const std::vector<float>& value) {
            auto* env = JNIService::getEnv();
            jfloatArray array = env->NewFloatArray(value.size());
            env->SetFloatArrayRegion(array,0,value.size(),(const float*)&value[0]);
            return array;
        }
    };
    template<> struct SignatureParam<std::vector<int>> {
        static std::string value() {
            return "[I";
        }
        static jintArray convert(const std::vector<int>& value) {
            auto* env = JNIService::getEnv();
            jintArray array = env->NewIntArray(value.size());
            env->SetIntArrayRegion(array,0,value.size(),(const int32_t *)&value[0]);
            return array;
        }
    };
    template<> struct SignatureParam<std::vector<double>> {
        static std::string value() {
            return "[D";
        }
        static jdoubleArray convert(const std::vector<double>& value) {
            auto* env = JNIService::getEnv();
            jdoubleArray array = env->NewDoubleArray(value.size());
            env->SetDoubleArrayRegion(array,0,value.size(),(const double*)&value[0]);
            return array;
        }
    };
    template<> struct SignatureParam<std::vector<char>> {
        static std::string value() {
            return "[C";
        }
        static jcharArray convert(const std::vector<char>& value) {
            auto* env = JNIService::getEnv();
            jcharArray array = env->NewCharArray(value.size());
            env->SetCharArrayRegion(array,0,value.size(),(const uint16_t *)&value[0]);
            return array;
        }
    };
    template<> struct SignatureParam<std::vector<long>> {
        static std::string value() {
            return "[J";
        }
        static jlongArray convert(const std::vector<long>& value) {
            auto* env = JNIService::getEnv();
            jlongArray array = env->NewLongArray(value.size());
            env->SetLongArrayRegion(array,0,value.size(),(const int64_t *)&value[0]);
            return array;
        }
    };
    template<> struct SignatureParam<std::vector<short>> {
        static std::string value() {
            return "[S";
        }
        static jshortArray convert(const std::vector<short>& value) {
            auto* env = JNIService::getEnv();
            jshortArray array = env->NewShortArray(value.size());
            env->SetShortArrayRegion(array,0,value.size(),(const int16_t *)&value[0]);
            return array;
        }
    };
    template<> struct SignatureParam<std::vector<bool>> {
        static std::string value() {
            return "[Z";
        }
        static jbooleanArray convert(const std::vector<bool>& value) {
            auto* env = JNIService::getEnv();
            jbooleanArray array = env->NewBooleanArray(value.size());
            uint8_t boolArray[value.size()];
            for (int i = 0; i < value.size(); ++i) {
                boolArray[i] = value[i];
            }
            env->SetBooleanArrayRegion(array,0,value.size(),(const uint8_t *)&boolArray[0]);
            return array;
        }
    };
    // todo 这里暂时不支持普通的ObjectArray
    template<> struct SignatureParam<std::vector<std::string>> {
        static std::string value() {
            return "[Ljava/lang/String;";
        }
        static jobjectArray convert(const std::vector<std::string> &value) {
            jobjectArray res = JNIService::getEnv()->NewObjectArray(
                    value.size(),
                    JClassManager::getJavaClass(JClassManager::getJClassName(String)),
                    nullptr);
            for (int i = 0; i < value.size(); ++i) {
                JNIService::getEnv()->SetObjectArrayElement(
                        res,i,SignatureParam<std::string>::convert(value[i]));
            }
            return res;
        }
    };

    // -------------------------------分割线------------------------------

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


    // -------------------------------分割线------------------------------

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
