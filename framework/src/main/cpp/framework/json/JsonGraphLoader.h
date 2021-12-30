//
// Created by wansu on 2021/11/23.
//

#ifndef SMEDIA_JSONGRAPHLOADER_H
#define SMEDIA_JSONGRAPHLOADER_H
#include <string>
#include <GraphConfig.h>
#include "GraphBuilder.h"
#include "Logger.h"
#include "../framework/data/Data.h"
#include <memory>
#include "json.hpp"

namespace smedia {
    class JsonGraphLoader {
    public:
        void parseJson(const std::string& jsonGraph);
        std::unique_ptr<GraphConfig> getGraphConfig();

    private:
        void parseExecutor(nlohmann::json& jsonObject);
        void parseNode(nlohmann::json& jsonObject);

    private:
        static void parseNodeOptions(nlohmann::json& nodeJson,NodeConfig& nodeConfig);
        static void parseNodeInputs(nlohmann::json& nodeJson,NodeConfig& nodeConfig);
        static void parseNodeOutput(nlohmann::json& nodeJson,NodeConfig& nodeConfig);
        static void parseTagIndexName(std::vector<PortTag>& v,
                                      const std::string& tag,
                                      nlohmann::json& nodeJson,
                                      std::string& name);
        static void StrSplit(std::vector<std::string> &vecs, const std::string &str, const char *cset);

    private:
        GraphBuilder graphBuilder;
    };
}



#endif //SMEDIA_JSONGRAPHLOADER_H
