//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_NODE_H
#define SMEDIA_NODE_H
#include "NodeContext.h"
#include "FunctorContext.h"
#include "IFunctor.h"
#include "Edge.h"
#include <map>
#include <memory>
namespace smedia{

    class Node {
        enum State {
            CREATED,RUNNING,TERMINATED
        };
    public:
        Node(EdgeMap &edgesMap,
              NodeConfig &nodeConfig,
              ServiceManager& globalServiceManager,
              Expander* expander);
        ~Node();
        bool initialize();
        bool run();
        void stop();

        void execute();
        void setOption(const OptionMap& options);

        const std::string& getName() const;
        const std::vector<std::string>& getOutputs() const;
        const std::vector<std::string>& getInputs() const;

        void setExecuteSelfHandler(std::function<void()> handler);
        void setExecuteConnectNodeHandler(std::function<void()> handler);

    private:
        std::string name;
        // 这个锁的作用是保证状态的正确判断与变化
        std::mutex m_lock;
        volatile State m_state;
        std::unique_ptr<FunctorContext> m_functorContext;
        std::unique_ptr<IFunctor> m_Functor;

        std::vector<std::string> m_inputEdges;
        std::vector<std::string> m_outputEdges;

        OptionMap m_runtimeOptions;
        std::function<void()> m_executeSelfHandler;
        std::function<void()> m_executeConnectHandler;

        EdgeMap& mEdgesMap;
        NodeConfig mNodeConfig;
        ServiceManager& mGlobalServiceManager;
        Expander* mExpander;

    };
}


#endif //SMEDIA_NODE_H
