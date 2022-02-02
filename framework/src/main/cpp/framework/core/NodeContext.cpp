//
// Created by wansu on 2021/11/5.
//

#include "NodeContext.h"

namespace smedia {
    NodeContext::NodeContext(smedia::EdgeMap &edgesMap,
                                     smedia::NodeConfig &nodeConfig,GlobalServiceManager& globalServiceManager)
                                     : edgesMap(edgesMap),
                                     nodeConfig(nodeConfig),globalServiceManager(globalServiceManager){}
}
