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
            // todo jni逻辑这里应该是不能包含业务逻辑
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

    JNIObjectRef JNIData::DataToJNIObject(Data data) {
        if (data.isEmpty()) {
            return nullptr;
        }
        // 这里由于无法直接拿到data中的模板类型，所以只能通过typeid的方式在外部进行类型选择
        // 如果把这块逻辑放到Data中会造成代码耦合，所以采用此方法
        long typeId = data.getTypeId();
        if (typeId == typeid(float).hash_code()) {
            return TypeUtil<float>::convertToJNIObject(*data.getData<float>());
        } else if (typeId == typeid(int).hash_code()) {
            return TypeUtil<int>::convertToJNIObject(*data.getData<int>());
        } else if (typeId == typeid(double).hash_code()) {
            return TypeUtil<double>::convertToJNIObject(*data.getData<double>());
        } else if (typeId == typeid(long).hash_code()) {
            return TypeUtil<long>::convertToJNIObject(*data.getData<long>());
        } else if (typeId == typeid(short).hash_code()) {
            return TypeUtil<short>::convertToJNIObject(*data.getData<short>());
        } else if (typeId == typeid(char).hash_code()) {
            return TypeUtil<char>::convertToJNIObject(*data.getData<char>());
        } else if (typeId == typeid(bool).hash_code()) {
            return TypeUtil<bool>::convertToJNIObject(*data.getData<bool>());
        } else if (typeId == typeid(std::string).hash_code()) {
            return TypeUtil<std::string>::convertToJNIObject(*data.getData<std::string>());
        } else if (typeId == typeid(JNIObjectRef).hash_code()) {
            return TypeUtil<JNIObjectRef>::convertToJNIObject(*data.getData<JNIObjectRef>());
        } else {
            LOG_ERROR << "DataToJNIObject is not support type";
            return nullptr;
        }
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