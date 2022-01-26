//
// Created by wansu on 2021/12/24.
//

#include "JNIBridge.h"
#include "GLFrame.h"
namespace smedia {
    static Data JNIGetGLFrame(jobject object) {
        auto* glFrame = new GLFrame{};
        glFrame->width = JNIInvoker<int>::GetObjectFiled(object,"width");
        glFrame->height = JNIInvoker<int>::GetObjectFiled(object,"height");
        glFrame->timeStamp = JNIInvoker<double>::GetObjectFiled(object,"timeStamp");
        glFrame->orientation = JNIInvoker<int>::GetObjectFiled(object,"orientation");
        glFrame->isMirrorY = JNIInvoker<bool>::GetObjectFiled(object,"isMirrorY");
        int textureId = JNIInvoker<int>::GetObjectFiled(object,"textureId");
        JNIObject jUVMatrix = JNIInvoker<JNIObject>::InvokeObjectMethod(object, "onNativeGetUVMatrix");
        glFrame->textureId = textureId;
        glFrame->format = FORMAT_TEXTURE_OES;
        std::vector<float> v = JNIData::jFloatArrayToVector(reinterpret_cast<jfloatArray>(jUVMatrix.getJObject()));
        for (int i=0;i<16;i++) {
            glFrame->UVMatrix[i] = v[i];
        }
        return Data::create(glFrame);
    }

    static Data JNIGetImageFrame(jobject object) {
        return Data();
    }

    Data JNIData::jObjectToData(jobject object) {
        Data res;
        if (JClassManager::isInstanceOf(object,Float)) {
            auto* value = new float(JNIInvoker<float>::InvokeObjectMethod(object,"floatValue"));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,Integer)) {
            auto* value = new int(JNIInvoker<int>::InvokeObjectMethod(object,"intValue"));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,Long)) {
            auto* value = new long(JNIInvoker<long>::InvokeObjectMethod(object,"longValue"));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,Boolean)) {
            auto* value = new bool(JNIInvoker<bool>::InvokeObjectMethod(object,"booleanValue"));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,String)) {
            auto s_ = reinterpret_cast<jstring>(object);
            auto *value = new std::string(jStringToCString(s_));
            res = Data::create(value);
        }else if (JClassManager::isInstanceOf(object,NativeGLFrame)) {
            res = JNIGetGLFrame(object);
        }else if (JClassManager::isInstanceOf(object,NativeImageFrame)) {
            res = JNIGetImageFrame(object);
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

    jstring JNIData::GetJNIString(const std::string& s) {
        jstring res = JNIService::getEnv()->NewStringUTF(s.c_str());
        return res;
    }
}