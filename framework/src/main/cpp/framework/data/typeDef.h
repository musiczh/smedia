//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_TYPEDEF_H
#define SMEDIA_TYPEDEF_H
#include "map"
#include "memory"
#include "deque"
#include "iostream"
#include "vector"
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
