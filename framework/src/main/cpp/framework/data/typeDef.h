//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_TYPEDEF_H
#define SMEDIA_TYPEDEF_H
#include "../../../../../../../../Library/Android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/c++/v1/map"
#include "../../../../../../../../Library/Android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/c++/v1/memory"
#include "../../../../../../../../Library/Android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/c++/v1/deque"
#include "../../../../../../../../Library/Android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/c++/v1/iostream"
#include "../../../../../../../../Library/Android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/c++/v1/vector"
#include "Data.h"

namespace smedia {
    class Edge;
    class Node;
    class Data;

    using EdgeMap = std::map<std::string,std::unique_ptr<Edge>>;
    using NodeMap = std::map<std::string,std::unique_ptr<Node>>;
    using OptionMap = std::map<std::string,Data>;
}


#endif //SMEDIA_TYPEDEF_H
