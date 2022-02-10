//
// Created by wansu on 2021/11/19.
//

#ifndef SMEDIA_FUNCTORCONTEXT_H
#define SMEDIA_FUNCTORCONTEXT_H
#include "Data.h"
#include "Edge.h"
#include "ServiceManager.h"
#include "DataStreamManager.h"
#include "typeDef.h"
#include <set>
namespace smedia {
    class Node;
    class FunctorContext {
    public:
        FunctorContext(std::vector<std::string>& inputEdges,
                       std::vector<std::string>& outputEdges,
                       ServiceManager& serviceManager,
                       EdgeMap& edgeMap,
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

        /**
         * 获取全局服务
         * @tparam T 具体的service实现类
         * @param name service的名称
         * @return 返回该对象的指针
         */
        template<class T>
        T* getService(const std::string name);

        std::string getNodeName();

    private:
        DataStreamManager m_inputManager;
        DataStreamManager m_outputManager;

        std::function<void()> m_executeSelfHandler;
        std::function<void()> m_executeConnectNodeHandler;

        ServiceManager& mServiceManager;
        Node* mNode;
    };

    template<class T>
    T *FunctorContext::getService(const std::string name) {
        auto* service = mServiceManager.getService(name);
        T* ptr = nullptr;
        if (service == nullptr || (ptr = dynamic_cast<T*>(service)) == nullptr) {
            LOG_ERROR << "getService error. service is null or dynamic fail";
        }
        return ptr;
    }

}




#endif //SMEDIA_FUNCTORCONTEXT_H
