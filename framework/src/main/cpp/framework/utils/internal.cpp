//
// Created by wansu on 2021/11/9.
//

#include "internal.h"
#include <iostream>
namespace smedia {
    std::string getTypeName(std::string fullName) {
        // 找到最后一个::的位置，如果没找到返回-1
        // todo 这里后续还需支持内部类的形式
        int pos = fullName.find_last_of("::");
        pos+=1;
        // 返回该类型的短名称
        return fullName.substr(pos,fullName.size()-pos);
    }
}