//
// Created by wansu on 2021/11/5.
//

#include "NodeContext.h"

namespace smedia {
    NodeContext::NodeContext(smedia::EdgeMap &edgesMap, smedia::OptionMap &globalService,
                                     smedia::NodeConfig &nodeConfig)
                                     : edgesMap(edgesMap), globalService(globalService), nodeConfig(nodeConfig){}
}
