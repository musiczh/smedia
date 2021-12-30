//
// Created by wansu on 2021/11/5.
//

#ifndef SMEDIA_NODECONTEXT_H
#define SMEDIA_NODECONTEXT_H
#include "../data/typeDef.h"
#include "GraphConfig.h"

namespace smedia {
class NodeContext {
public:
    NodeContext(EdgeMap& edgesMap,OptionMap & globalConfig,NodeConfig& globalService);
    EdgeMap& edgesMap;
    OptionMap & globalService;
    NodeConfig nodeConfig;
};

}


#endif //SMEDIA_NODECONTEXT_H
