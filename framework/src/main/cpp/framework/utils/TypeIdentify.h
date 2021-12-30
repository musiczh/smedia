//
// Created by wansu on 2021/11/15.
//

#ifndef SMEDIA_TYPEIDENTIFY_H
#define SMEDIA_TYPEIDENTIFY_H
#include <memory>
namespace internal {
    template<typename T >
    struct _is_array : std::false_type {};

    template<typename T>
    struct _is_array<T[]> : std::true_type {};

    template<typename T>
    bool is_array() {
        return _is_array<T>::value;
    }
}




#endif //SMEDIA_TYPEIDENTIFY_H
