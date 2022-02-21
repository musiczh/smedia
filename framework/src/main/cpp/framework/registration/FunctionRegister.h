//
// Created by wansu on 2021/11/8.
//

#ifndef SMEDIA_FUNCTIONREGISTER_H
#define SMEDIA_FUNCTIONREGISTER_H
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include "Logger.h"

/**
 * 函数注册类
 * 每个类型的函数会具体化FunctorRegister，通过调用静态方法来进行注册和获取
 * 模板参数T表示返回值类型，Args表示参数类型
 */
namespace smedia {
template<typename T,typename ... Args>
class FunctionRegister {
    using Function = std::function<T(Args...)>;

public:
    static void registerFunction(const std::string& name,Function function){
        std::unique_lock<std::mutex> lock(mainLock);
        if (!functionMap.insert(std::make_pair(name,function)).second) {
            // todo 这里在静态属性初始化时调用，如果创建对象会出现类还没加载的情况导致闪退
            //LOG_INFO << name << " function has register";
        }
    }

    static bool isRegister(const std::string& name) {
        std::unique_lock<std::mutex> lock(mainLock);
        return functionMap.find(name) != functionMap.end();
    }

    static std::vector<std::string> getRegisterName() {
        std::unique_lock<std::mutex> lock(mainLock);
        std::vector<std::string> result;
        for (auto i = functionMap.begin();i != functionMap.begin();i++) {
            result.emplace_back(i->first);
        }
        return result;
    }

    static bool unRegister(const std::string& name) {
        std::unique_lock<std::mutex> lock(mainLock);
        return functionMap.erase(name) != 0;
    }

    static void unRegisterAll() {
        std::unique_lock<std::mutex> lock(mainLock);
        functionMap.clear();
    }

    static T invokeFunction(const std::string& name,Args&& ... args) {
        std::unique_lock<std::mutex> lock(mainLock);
        if (functionMap.find(name) != functionMap.end()) {
            return (functionMap[name])(std::forward<Args>(args) ...);
        }
        LOG_ERROR << "function:" << name << " has not register,can not invoke";
        // todo 这里返回nullptr意味着只对指针类型的返回值数据有效，后续版本再考虑支持值类型的返回值类型函数
        return nullptr;
    }

private:
    static std::unordered_map<std::string,Function> functionMap;
    static std::mutex mainLock;
};

// 静态属性需要在类外进行初始化
template<typename T,typename ... Args>
std::mutex FunctionRegister<T,Args...>::mainLock;
template<typename T,typename ... Args>
std::unordered_map<std::string,std::function<T(Args...)>> FunctionRegister<T,Args...>::functionMap;


}



#endif //SMEDIA_FUNCTIONREGISTER_H
