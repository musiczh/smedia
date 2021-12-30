//
// Created by wansu on 2021/12/24.
//

#include "JNIBridge.h"
namespace smedia {
    Data JNIData::jObjectToData(jobject object) {
        Data res;
        if (JClassManager::isInstanceOf(object,Float)) {
            jclass clazz = JClassManager::getJavaClass(JClassManager::getJClassName(Float));
            jmethodID methodId = JNISignature::getMethodId<float>(clazz,"floatValue");
            auto* value = new float(JNICaller<float>::callObjectMethod(object,methodId));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,Integer)) {
            jclass clazz = JClassManager::getJavaClass(JClassManager::getJClassName(Integer));
            jmethodID methodId = JNISignature::getMethodId<int>(clazz,"intValue");
            int* value = new int(JNICaller<int>::callObjectMethod(object,methodId));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,Long)) {
            jclass clazz = JClassManager::getJavaClass(JClassManager::getJClassName(Long));
            jmethodID methodId = JNISignature::getMethodId<long>(clazz,"longValue");
            auto* value = new long(JNICaller<long>::callObjectMethod(object,methodId));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,String)) {
            auto s_ = reinterpret_cast<jstring>(object);
            auto* value = new std::string(jStringToCString(s_));
            res = Data::create(value);
        }else {
            auto * value = new JNIObject(object);
            res = Data::create(value);
        }
        return res;
    }

    std::string JNIData::jStringToCString(jstring str) {
        if (!str) {
            return std::string();
        }
        JNIEnv* env = JNIService::getEnv();
        jboolean isCopy;
        const char* chars = env->GetStringUTFChars(str, &isCopy);
        std::string s;
        if (chars) {
            s = chars;
            env->ReleaseStringUTFChars(str, chars);
        }
        return s;
    }

    std::vector<float> JNIData::jFloatArrayToVector(jfloatArray floatArray) {
        if (!floatArray) {
            return std::vector<float>();
        }
        JNIEnv* jniEnv = JNIService::getEnv();
        jsize size = jniEnv->GetArrayLength(floatArray);
        std::vector<float> result(size);
        jniEnv->GetFloatArrayRegion(floatArray, 0, size, (jfloat*)&result[0]);
        return result;
    }
}