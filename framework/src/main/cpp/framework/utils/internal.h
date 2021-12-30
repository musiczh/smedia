//
// Created by wansu on 2021/11/9.
//

#ifndef SMEDIA_INTERNAL_H
#define SMEDIA_INTERNAL_H
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
/**
 * 通用工具文件
 * 包含了一些通用的工具函数
 */
namespace smedia
{
    // 创建指定泛型的对象指针
    // 这里采用多模板参数的方式来泛化构造函数的参数
    // forward表示保持前面args参数的右值类型，减少参数copy
    template<typename T,typename... Args>
    T* make_pointer(Args&& ... args) {
        return new T(std::forward<Args>(args)...);
    }

    // 创建指定泛型的unique_ptr对象
//    template<typename T,typename ... Args>
//    std::unique_ptr<T> make_unique(Args&&...args) {
//        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
//    }

    template<class T> struct _Unique_if {
            typedef std::unique_ptr<T> _Single_object;
        };

    template<class T> struct _Unique_if<T[]> {
        typedef std::unique_ptr<T[]> _Unknown_bound;
    };

    template<class T, size_t N> struct _Unique_if<T[N]> {
        typedef void _Known_bound;
    };

    template<class T, class... Args>
    typename _Unique_if<T>::_Single_object
    make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T>
    typename _Unique_if<T>::_Unknown_bound
    make_unique(size_t n) {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>(new U[n]());
    }

    template<class T, class... Args>
    typename _Unique_if<T>::_Known_bound
    make_unique(Args&&...) = delete;

    // 采用静态局部变量减少创建hash实例
    template<typename T>
    size_t hash(const T& t) {
        static std::hash<T> _hash = std::hash<T>();
        return _hash(t);
    }
}



#endif //SMEDIA_INTERNAL_H
