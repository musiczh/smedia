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
 * 主要做c++层调用java层方法: JNIInvoker
 * jni属性转化为c++属性: JNIData
 * java数组对象转化为vector: JArrayToVector
 */
namespace smedia {
    //------------------JNIData---------------------
    class JNIData {
    public:
        /**
         * 把java的object对象转化为Data 目前仅支持基础数据类型，其他均转为JNIObject
         * @param object
         * @return 存储的Data对象
         */
        static Data jObjectToData(jobject object);

        static JNIObjectRef DataToJNIObject(Data data);

        // 把jString转化为c++的string
        static std::string jStringToCString(jstring object);

        static std::vector<float> jFloatArrayToVector(jfloatArray floatArray);

    };

    //--------------JArrayToVector---------------------
    template<typename T>
    struct JArrayToVector {
    };

    template<>
    struct JArrayToVector<float> {
        static std::vector<float> convert(jfloatArray floatArray){
            if (!floatArray) {
                return std::vector<float>();
            }
            JNIEnv* jniEnv = JNIService::getEnv();
            jsize size = jniEnv->GetArrayLength(floatArray);
            std::vector<float> result(size);
            jniEnv->GetFloatArrayRegion(floatArray, 0, size, (jfloat*)&result[0]);
            return result;
        };
    };


    //--------------------------JNIInvoker---------------------------------

    /**
     * JNICaller的设计是希望能有一个统一的接口可以提供给外部，通过设置模板来决定方法的返回值以及参数
     * 由于参数是可变的，所以这里不能够使用函数，因为函数不能部分特例化
     * 同时针对不同的返回值类型需要有不同的处理，这里采用特例化来分开处理
     * 注意，这里所有的模板类型都是采用c++层的属性类型，内部会自动做转化，如
     * JNIObject object = JNIInvoker<JNIObject,int,std::string>::InvokeObjectMethod(object,"methodName",1,"argName");
     *
     * 外部传进来的是c++层的数据类型，调用jni方法时需要转化为java的数据类型，所以这里采用SignatureParam
     * 把可变属性都转化为java属性并传入
     *
     *
     * todo 这里的模板方法并不支持所有数据类型，在调用的时候非基本数据类型需要确定一下是否支持
     * 默认的返回值是JNIObject，特例化了几个常用基本数据类型:int、float、long、string、void、bool、JNIObject、double
     * @tparam Args 函数参数模板，第一个泛型指定为返回值类型
     */
    template<typename ...Args>
    struct JNIInvoker {};

    template<typename ...Args>
    struct JNIInvoker<void,Args...> {
        static void InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<void,Args...>(clazz,methodName);
            // 外部传进来的是c++层的数据类型，调用jni方法时需要转化为java的数据类型，所以这里采用SignatureParam
            // 把可变属性都转化为java属性并传入，下同
            JNIService::getEnv()->CallVoidMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
        }
        static void InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<JNIObject,Args...>(clazz,methodName);
            JNIService::getEnv()->CallStaticVoidMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
        }
    };
    template<typename ...Args>
    struct JNIInvoker<JNIObject,Args...> {
        static JNIObject InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<JNIObject,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallObjectMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            JNIObject object(objectRes);
            // 删除局部引用，如果c++内部线程为自己创建，则在线程结束之前不会释放局部引用，导致内存暴涨，这里需要删除局部引用
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return object;
        }
        static JNIObject InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<JNIObject,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
            JNIObject object(objectRes);
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return object;
        }
        static JNIObject GetObjectFiled(JNIObject instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance.getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<JNIObject>(clazz,fieldName);
            jobject objectRes = JNIService::getEnv()->GetObjectField(instance.getJObject(),fieldId);
            JNIObject object(objectRes);
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return object;
        }
        static JNIObject GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<JNIObject>(clazz,fieldName);
            jobject objectRes = JNIService::getEnv()->GetStaticObjectField(clazz,fieldId);
            JNIObject object(objectRes);
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return object;
        }
    };
    template<typename ...Args>
    struct JNIInvoker<JNIObjectRef,Args...> {
        static JNIObjectRef InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<JNIObjectRef,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallObjectMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            JNIObjectRef res(new JNIObject(objectRes));
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return res;
        }
        static JNIObjectRef InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<JNIObjectRef,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
            JNIObjectRef res(new JNIObject(objectRes));
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return res;
        }
        static JNIObjectRef GetObjectFiled(JNIObjectRef instance,const std::string& fieldName) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance->getJObject());
            jfieldID fieldId = JNISignature::getObjectFiledId<JNIObjectRef>(clazz,fieldName);
            jobject objectRes = JNIService::getEnv()->GetObjectField(instance->getJObject(),fieldId);
            JNIObjectRef res(new JNIObject(objectRes));
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return res;
        }
        static JNIObjectRef GetStaticFiled(const std::string& className,const std::string& fieldName) {
            jclass clazz = JClassManager::getJavaClass(className);
            jfieldID fieldId = JNISignature::getStaticFiledId<JNIObjectRef>(clazz,fieldName);
            jobject objectRes = JNIService::getEnv()->GetStaticObjectField(clazz,fieldId);
            JNIObjectRef res(new JNIObject(objectRes));
            JNIService::getEnv()->DeleteLocalRef(objectRes);
            return res;
        }
    };
    template<typename ...Args>
    struct JNIInvoker<long,Args...> {
        static long InvokeObjectMethod(jobject instance,const std::string& methodName,Args... args) {
            jclass clazz = JNIService::getEnv()->GetObjectClass(instance);
            jmethodID methodId = JNISignature::getMethodId<long,Args...>(clazz,methodName);
            jlong objectRes = JNIService::getEnv()->CallLongMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            return objectRes;
        }
        static long InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<long,Args...>(clazz,methodName);
            jlong objectRes = JNIService::getEnv()->CallStaticLongMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
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
            jint objectRes = JNIService::getEnv()->CallIntMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            return objectRes;
        }
        static int InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<int,Args...>(clazz,methodName);
            jint objectRes = JNIService::getEnv()->CallStaticIntMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
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
            jboolean objectRes = JNIService::getEnv()->CallBooleanMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            return objectRes;
        }
        static bool InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<bool,Args...>(clazz,methodName);
            jboolean objectRes = JNIService::getEnv()->CallStaticBooleanMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
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
            jobject objectRes = JNIService::getEnv()->CallObjectMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            jstring stringRes = reinterpret_cast<jstring>(objectRes);
            auto res = JNIData::jStringToCString(stringRes);
            return res;
        }
        static std::string InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<std::string,Args...>(clazz,methodName);
            jobject objectRes = JNIService::getEnv()->CallStaticObjectMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
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
            jfloat objectRes = JNIService::getEnv()->CallFloatMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            return objectRes;
        }
        static float InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<float,Args...>(clazz,methodName);
            jfloat objectRes = JNIService::getEnv()->CallStaticFloatMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
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
            jdouble objectRes = JNIService::getEnv()->CallDoubleMethod(instance,methodId,SignatureParam<Args>::convert(args)...);
            return objectRes;
        }
        static double InvokeStaticMethod(const std::string& className, const std::string& methodName,Args... args) {
            jclass clazz = JClassManager::getJavaClass(className);
            jmethodID methodId = JNISignature::getStaticMethodId<double,Args...>(clazz,methodName);
            jdouble objectRes = JNIService::getEnv()->CallStaticDoubleMethod(clazz,methodId,SignatureParam<Args>::convert(args)...);
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


    // ------------------JNICreate-------------------------
    // 从类名来创建java对象
    template <typename ...Args>
    JNIObjectRef JNICreateInstance(const std::string& name,Args&&... args) {
        jclass cls = JClassManager::getJavaClass(name);
        jmethodID methodId = JNISignature::getMethodId<void,Args...>(cls,"<init>");
        jobject res = JNIService::getEnv()->NewObject(cls,methodId,std::forward<Args>(args)...);
        // todo 后续写demo测试这里是否返回null
        if (res == nullptr) {
            return nullptr;
        }
        auto* jniObject = new JNIObject(res);
        return JNIObjectRef(jniObject);
    }


    // -----------------TypeUtil---------------------------
    /**
     * 对类型进行分类处理，适用于各种数据转换等
     */
     template<typename T>
     struct TypeUtil {};

    template<>
    struct TypeUtil<float> {
        static JNIObjectRef convertToJNIObject(float value) {
            return JNICreateInstance(JClassManager::getJClassName(Float),value);
        }
        static float convertToC(jfloat value) {
            return value;
        }
    };

    template<>
    struct TypeUtil<int> {
        static JNIObjectRef convertToJNIObject(int value) {
            return JNICreateInstance(JClassManager::getJClassName(Integer),value);
        }
        static int convertToC(jint value) {
            return value;
        }
    };

    template<>
    struct TypeUtil<double> {
        static JNIObjectRef convertToJNIObject(double value) {
            return JNICreateInstance(JClassManager::getJClassName(Double),value);
        }
        static double convertToC(jdouble value) {
            return value;
        }
    };

    template<>
    struct TypeUtil<char> {
        static JNIObjectRef convertToJNIObject(char value) {
            return JNICreateInstance(JClassManager::getJClassName(Char),value);
        }
        static char convertToC(jchar value) {
            return value;
        }
    };

    template<>
    struct TypeUtil<long> {
        static JNIObjectRef convertToJNIObject(long value) {
            return JNICreateInstance(JClassManager::getJClassName(Long),value);
        }
        static long convertToC(jlong value) {
            return value;
        }
    };

    template<>
    struct TypeUtil<short> {
        static JNIObjectRef convertToJNIObject(short value) {
            return JNICreateInstance(JClassManager::getJClassName(Short),value);
        }
        static short convertToC(jshort value) {
            return value;
        }
    };

    template<>
    struct TypeUtil<bool> {
        static JNIObjectRef convertToJNIObject(bool value) {
            return JNICreateInstance(JClassManager::getJClassName(Boolean),value);
        }
        static bool convertToC(jboolean value) {
            return value != 0;
        }
    };

    template<>
    struct TypeUtil<std::string> {
        static JNIObjectRef convertToJNIObject(const std::string& value) {
            jstring res = JNIService::getEnv()->NewStringUTF(value.c_str());
            return JNIObjectRef(new JNIObject(res));
        }
        static std::string convertToC(jstring value) {
            if (!value) {
                return std::string();
            }
            JNIEnv* env = JNIService::getEnv();
            jboolean isCopy;
            const char* chars = env->GetStringUTFChars(value, &isCopy);
            std::string s;
            if (chars) {
                s = chars;
                env->ReleaseStringUTFChars(value, chars);
            }
            return s;
        }
    };

    template<>
    struct TypeUtil<JNIObjectRef> {
        static JNIObjectRef convertToJNIObject(JNIObjectRef value) {
            return value;
        }
        static JNIObjectRef convertToC(JNIObjectRef value) {
            return value;
        }
    };



}



#endif //SMEDIA_JNIBRIDGE_H
