//
// Created by wansu on 2021/11/5.
//

#ifndef SMEDIA_NODECONTEXT_H
#define SMEDIA_NODECONTEXT_H
#include "typeDef.h"
#include "GraphConfig.h"
#include "GlobalServiceManager.h"

namespace smedia {
class NodeContext {
public:
    NodeContext(EdgeMap& edgesMap,NodeConfig& globalService,GlobalServiceManager& globalServiceManager);
    EdgeMap& edgesMap;
    NodeConfig nodeConfig;
    GlobalServiceManager& globalServiceManager;
};

}


#endif //SMEDIA_NODECONTEXT_H
