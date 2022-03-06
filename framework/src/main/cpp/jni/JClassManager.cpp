//
// Created by wansu on 2021/12/22.
//

#include "JClassManager.h"
namespace smedia {
    std::map<std::string,jclass> JClassManager::classMap;
    jobject JClassManager::globalClassLoader;
    std::mutex JClassManager::dataLock;
    jmethodID JClassManager::globalLoadClassMethodId;

    void JClassManager::initJavaClass(jobject classLoader, std::vector<std::string>& javaClasses) {
        std::unique_lock<std::mutex> lock(dataLock);
        JNIEnv* env = JNIService::getEnv();
        // 需要用全局引用来进行保存应用类加载器
        globalClassLoader = env->NewGlobalRef(classLoader);

        // 保存loadClass方法的方法引用
        jclass rootClassLoader = env->FindClass("java/lang/ClassLoader");
        globalLoadClassMethodId = env->GetMethodID(rootClassLoader,"loadClass",
                                                   "(Ljava/lang/String;)Ljava/lang/Class;");
        env->DeleteLocalRef(rootClassLoader);

        releaseAllJavaClass();
        for (auto& className : javaClasses) {
            createJClass(className);
        }
    }

    void JClassManager::unInitJavaClass() {
        std::unique_lock<std::mutex> lock(dataLock);
        releaseAllJavaClass();
        // 释放class对象，类加载器不需要释放
//        JNIService::getEnv()->DeleteGlobalRef(globalClassLoader);
//        globalClassLoader = nullptr;
    }


    jclass JClassManager::getJavaClass(const std::string &className) {
        std::unique_lock<std::mutex> lock(dataLock);
        if (classMap.find(className)!=classMap.end()) {
            return classMap[className];
        }
        return createJClass(className);
    }

    void JClassManager::releaseAllJavaClass() {
        JNIEnv* env = JNIService::getEnv();
        for (auto& item : classMap) {
            env->DeleteGlobalRef(item.second);
            classMap[item.first] = nullptr;
        }
        classMap.clear();
    }

    jclass JClassManager::createJClass(const std::string & className) {
        JNIEnv* env = JNIService::getEnv();
        jstring name = env->NewStringUTF(className.c_str());
        // 调用类加载器的方法来加载类对象
        jobject clazz = env->CallObjectMethod(globalClassLoader,globalLoadClassMethodId,name);
        // 这里要创建一个全局引用进行保存
        auto newClass = reinterpret_cast<jclass>(env->NewGlobalRef(clazz));
        classMap[className] = newClass;
        env->DeleteLocalRef(clazz);
        env->DeleteLocalRef(name);
        return newClass;
    }

    bool JClassManager::isInstanceOf(jobject object, JClassType type) {
        auto* env = JNIService::getEnv();
        jclass clazz = getJavaClass(getJClassName(type));
        jboolean res = env->IsInstanceOf(object,clazz);
        return res;
    }

    std::string JClassManager::getJClassName(JClassType classType) {
        std::string name;
        switch (classType) {
            case Boolean:name = "java/lang/Boolean";break;
            case Short:name = "java/lang/Short";break;
            case Float:name = "java/lang/Float";break;
            case Integer:name = "java/lang/Integer";break;
            case Double:name = "java/lang/Double";break;
            case Long:name = "java/lang/Long";break;
            case Byte:name = "java/lang/Byte";break;
            case String:name = "java/lang/String";break;
            case Char:name = "java/lang/Character";break;
            case Object:name = "java/lang/Object";break;
            case StringArray:name = "[java/lang/String";break;
            case IntArray:name = "[T";break;
            case FloatArray:name = "[F";break;
            case DoubleArray:name = "[D";break;
            case LongArray:name = "[J";break;
            case ByteArray:name = "[B";break;
            case ShortArray:name = "[S";break;
            case NativeGLFrame:name = "com/example/frameword/framework/NativeGLFrame";break;
            case NativeImageFrame:name = "com/example/frameword/framework/NativeImageFrame";break;
            default:break;
        }
        return name;
    }
}


