//
// Created by wansu on 2021/11/5.
//

#include "NodeContext.h"

namespace smedia {
    NodeContext::NodeContext(EdgeMap &edgesMap,
                             NodeConfig &nodeConfig,
                             ServiceManager& globalServiceManager,
                             Expander* expander):
                             edgesMap(edgesMap),nodeConfig(nodeConfig),
                             globalServiceManager(globalServiceManager),
                             expander(expander){}
}
