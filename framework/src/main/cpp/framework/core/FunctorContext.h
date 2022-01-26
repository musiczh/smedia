//
// Created by wansu on 2021/11/19.
//

#ifndef SMEDIA_FUNCTORCONTEXT_H
#define SMEDIA_FUNCTORCONTEXT_H
#include "Data.h"
#include "Edge.h"
#include "DataStreamManager.h"
#include "typeDef.h"
#include <set>
namespace smedia {
    class Node;
    class FunctorContext {
    public:
        FunctorContext(std::vector<std::string>& inputEdges,
                       std::vector<std::string>& outputEdges,
                       EdgeMap& edgeMap,
                       OptionMap& globalService,
                       Node* node);
        Data getInput(const std::string& tag,int index = 0);
        Data popInput(const std::string& tag,int index = 0);
        void setOutput(Data data, const std::string& tag,int index = -1);
        int getInputTagCount(const std::string& tag);
        int getOutputTagCount(const std::string& tag);
        std::unique_ptr<std::set<std::string>> getInputTags();
        std::unique_ptr<std::set<std::string>> getOutputTags();
         /**
         * 获取当前节点输入数据队列中最前的一个数据key
         * 若当前队列为空则返回{"emptyStreamKey",-2}
         * @return 对应的边的streamKey
         */
        DataStreamManager::StreamKey getFrontStreamKey() ;

        void setExecuteSelfHandler(std::function<void()> function);
        void setExecuteConnectNodeHandler(std::function<void()> function);
        /**
         * 调度自身
         */
        void executeSelf();
        /**
         * 调度接下来所连接的节点，仅在异步functor内有效，即isAsynchronous()方法需要返回true
         * 用于异步functor
         */
        void executeConnectNode();

        Data getGlobalService(const std::string& name);

        std::string getNodeName();

    private:
        DataStreamManager m_inputManager;
        DataStreamManager m_outputManager;

        std::function<void()> m_executeSelfHandler;
        std::function<void()> m_executeConnectNodeHandler;

        OptionMap mGlobalService;
        Node* mNode;
    };
}



#endif //SMEDIA_FUNCTORCONTEXT_H
