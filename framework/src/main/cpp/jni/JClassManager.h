//
// Created by wansu on 2021/12/22.
//

#ifndef SMEDIA_JCLASSMANAGER_H
#define SMEDIA_JCLASSMANAGER_H
#include <unordered_map>
#include <map>
#include <jni.h>
#include <vector>
#include "JNIService.h"

/**
 * 负责加载并缓存java类对象，需要初始化后使用
 */
namespace smedia {
    // 枚举java基本数据类型和数组类型
    enum JClassType {
        Boolean,Short,Float,Integer,Double,Long,Byte,String,Char,
        StringArray,IntArray,FloatArray,DoubleArray,LongArray,ByteArray,ShortArray,
        Object,NativeGLFrame,NativeImageFrame
    };

    class JClassManager {
    public:
        /**
         * 注册类加载器以及需要预加载的java类
         * @param classLoader 类加载器
         * @param javaClasses 需要预加载的java类
         */
        static void initJavaClass(jobject classLoader,std::vector<std::string>& javaClasses);

        /**
         * 释放持有的类对象以及类加载资源
         */
        static void unInitJavaClass();

        /**
         * 判断对象是否为某个类
         * @param object java对象
         * @param classType 预置的类型
         * @return 判断结果
         */
        static bool isInstanceOf(jobject object,JClassType classType);

        /**
         * 获取java类对象
         * @param className java类的全限定名称
         * @return java类对象
         */
        static jclass getJavaClass(const std::string& className);

        /**
         * 获取预设的类名
         * @param classType
         * @return
         */
        static std::string getJClassName(JClassType classType);

        static jclass createJClass(const std::string&);

    private:
        static void releaseAllJavaClass();



    private:
        static std::map<std::string,jclass> classMap;
        static jobject globalClassLoader;
        static jmethodID globalLoadClassMethodId;
        // 加锁保证数据的及时更新以及防止多线程竞争导致未知问题
        static std::mutex dataLock;
    };
}



#endif //SMEDIA_JCLASSMANAGER_H
