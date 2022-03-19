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
        friend class Node;
    public:
        FunctorContext(std::vector<std::string>& inputEdges,
                       std::vector<std::string>& outputEdges,
                       ServiceManager& serviceManager,
                       EdgeMap& edgeMap,
                       Node* node);

    private:
        // 设置执行handler，节点可以请求调度自身、或者调度所连接的节点，
        void setExecuteSelfHandler(std::function<void()> function);
        void setExecuteConnectNodeHandler(std::function<void()> function);

        // 下面是对functor开放的的接口
    public:
        // 输入端口的tag+index获取端口的输入，若不输入端口号则默认为0
        // 有关输入输出的操作全部交给DataStreamManager，这里只做参数透传
        Data getInput(const std::string& tag,int index = 0);
        Data getInputFront(std::string& tag,int& index,bool pop);
        Data popInput(const std::string& tag,int index = 0);
        // 默认输出到同tag的第一个端口
        void setOutput(Data data, const std::string& tag,int index = 0);
        // 获取端口信息，key是tag，value是index集合
        std::set<PortKey> getPortInfo(bool isInput) const;

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
        DataStreamManager mDataStreamManager{};

        // 保存的两个调度handle，可以回调到graph来调度节点
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
