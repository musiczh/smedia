//
// Created by wansu on 2021/11/23.
//

#include "JsonGraphLoader.h"

namespace smedia {
    void JsonGraphLoader::parseJson(const std::string &jsonGraph) {
        graphBuilder.reset();
        auto jsonObject = nlohmann::json::parse(jsonGraph);
        parseExecutor(jsonObject);
        parseNode(jsonObject);
        parseService(jsonObject);
    }

    std::unique_ptr<GraphConfig> JsonGraphLoader::getGraphConfig() {
        return graphBuilder.buildGraph();
    }

    void JsonGraphLoader::parseService(nlohmann::json &jsonObject) {
        if (!jsonObject.contains("services")) {
            LOG_DEBUG << "the json does not contain services";
            return ;
        }
        auto& jsonServices = jsonObject["services"];
        for (auto& serviceJson : jsonServices) {
            ServiceConfig serviceConfig;
            if (serviceJson.contains("service")) {
                serviceConfig.service = serviceJson["service"].get<std::string>();
            }
            if (serviceJson.contains("name")) {
                serviceConfig.name = serviceJson["name"].get<std::string>();
            }
            parseOptions(serviceJson,serviceConfig.options);
            graphBuilder.addService(serviceConfig);
        }
    }

    void JsonGraphLoader::parseExecutor(nlohmann::json &jsonObject) {
        if (!jsonObject.contains("executors")) {
            LOG_DEBUG << "the json does not contain executors";
            return ;
        }
        auto& jsonExecutors = jsonObject["executors"];
        for (auto& executorJson : jsonExecutors) {
            ExecutorConfig executorConfig ;
            if (executorJson.contains("name")) {
                executorConfig.name = executorJson["name"].get<std::string>();
            }
            if (executorJson.contains("executor")) {
                executorConfig.executor = executorJson["executor"].get<std::string>();
            }
            parseOptions(executorJson,executorConfig.options);
            graphBuilder.addExecutor(executorConfig);
        }
    }

    void JsonGraphLoader::parseNode(nlohmann::json &jsonObject) {
        if (!jsonObject.contains("nodes")) {
            LOG_INFO << "the json does not contain nodes";
            return ;
        }
        auto& jsonNodes = jsonObject["nodes"];
        for (auto& nodeJson : jsonNodes) {
            NodeConfig nodeConfig{};
            nodeConfig.functor = nodeJson["functor"].get<std::string>();
            if (nodeJson.contains("name")) {
                nodeConfig.name = nodeJson["name"].get<std::string>();
            }
            if (nodeJson.contains("executor")) {
                nodeConfig.executor = nodeJson["executor"].get<std::string>();
            }

            parseOptions(nodeJson,nodeConfig.options);
            parseNodeInputs(nodeJson,nodeConfig);
            parseNodeOutput(nodeJson,nodeConfig);

            graphBuilder.addNode(nodeConfig);
        }
    }

    void JsonGraphLoader::parseNodeInputs(nlohmann::json &nodeJson,NodeConfig& nodeConfig) {
        parseTagIndexName(nodeConfig.inputs,"inputs",nodeJson,nodeConfig.name);
    }

    void JsonGraphLoader::parseNodeOutput(nlohmann::json &nodeJson,NodeConfig& nodeConfig) {
        parseTagIndexName(nodeConfig.outputs,"outputs",nodeJson,nodeConfig.name);
    }

    void JsonGraphLoader::StrSplit(std::vector<std::string> &vecs, const std::string &str, const char *cset) {
        std::size_t sp = 0, np;

        //过滤掉前面的分割字符
        while(sp < str.size() && strchr(cset, str[sp])) ++sp;
        np = str.find_first_of(cset, sp);
        while (np != std::string::npos) {
            std::size_t len = np - sp;
            vecs.push_back(str.substr(sp, len));

            sp = np;
            while(sp < str.size() && strchr(cset, str[sp])) ++sp;
            np = str.find_first_of(cset, sp);
        }

        if (sp < str.size())
            vecs.push_back(str.substr(sp));
    }

    void JsonGraphLoader::parseTagIndexName(std::vector<PortTag> &v,
                                            const std::string &tag,
                                            nlohmann::json& nodeJson,
                                            std::string& nodeName) {
        if (!nodeJson.contains(tag) || nodeJson[tag].empty()) {
            //LOG_INFO << nodeName << " no " << tag ;
            return;
        }
        // 配置input和output以及options
        auto& nodeInputJson = nodeJson[tag];
        for (auto& inputJson : nodeInputJson) {
            const std::string & inputString = inputJson.get<std::string>();
            std::vector<std::string> splitStrings;
            StrSplit(splitStrings,inputString,":");

            int index = -1;
            std::string _tag;
            std::string name;
            switch (splitStrings.size()) {
                case 2: {
                    _tag = splitStrings[0];
                    name = splitStrings[1];
                    index = 0;
                    break;
                }
                case 3: {
                    _tag = splitStrings[0];
                    try {
                        index = std::stoi(splitStrings[1]);
                    }catch (std::invalid_argument&) {
                        LOG_ERROR << "json port index illegal,invalid argument";
                    }catch (std::out_of_range&) {
                        LOG_ERROR << "json port index illegal,out of range";
                    }
                    name = splitStrings[2];
                }
            }
            v.push_back({_tag, name, index});
        }
    }

    void JsonGraphLoader::parseOptions(nlohmann::json &jsonObject, OptionMap &optionMap) {
        if (!jsonObject.contains("options") || !jsonObject["options"].is_object()) {
            return;
        }
        auto& optionsJson = jsonObject["options"];
        for (auto ptr = optionsJson.begin();ptr != optionsJson.end();ptr++) {
            const std::string & key = ptr.key();
            Data data;
            auto& value = ptr.value();
            switch (value.type()) {
                case nlohmann::detail::value_t::null:
                    break;
                case nlohmann::detail::value_t::number_unsigned:
                case nlohmann::detail::value_t::number_integer : {
                    data = Data::create(new int(value.get<int>()));
                    break;
                }
                case nlohmann::detail::value_t::boolean: {
                    data = Data::create(new bool(value.get<bool>()));
                    break;
                }
                case nlohmann::detail::value_t::number_float: {
                    data = Data::create(new float (value.get<float>()));
                    break;
                }
                case nlohmann::detail::value_t::string: {
                    data = Data::create(new std::string(value.get<std::string>()));
                    break;
                }
                case nlohmann::detail::value_t::array:
                case nlohmann::detail::value_t::object:
                case nlohmann::detail::value_t::discarded:
                    break;
            }
            if (!data.isEmpty()) {
                optionMap[key] = data;
            }
        }
    }


}

