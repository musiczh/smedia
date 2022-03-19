//
// Created by wansu on 2021/12/28.
//

#ifndef SMEDIA_INPUTHANDLER_H
#define SMEDIA_INPUTHANDLER_H
#include "FunctorContext.h"
#include <functional>
/**
 * 统一处理functor的输入
 * 外部通过注册tag：handler，调用runHandler方法来执行对应tag的handler
 * 处理functor的输入调用单参数runHandler，setOption调用多参数
 */
namespace smedia {
    // 讲setOption和节点的输入一起封装到inputData中
    // option没有index，所以默认为0
    struct InputData {
        std::string nodeName;
        Data data;
        std::string tag;
        int index;
        FunctorContext* functorContext;
    };
    using Handler = std::function<bool(InputData)>;

    class InputHandler {
    public:
        void registerHandler(const std::string& key, Handler handler);
        void registerHandler(const std::string& key,int index,Handler handler);
        void registerDefaultHandler(Handler defaultHandler);

        // 给execute用，内部popInput
        bool runExecuteHandler(FunctorContext*);
        // 给setOption用
        bool runOptionsHandler(FunctorContext*,const std::string& key,Data data);

    private:
        bool handle(InputData);

    private:
        std::map<PortKey,Handler> mHandlerMap;
        Handler mDefaultHandler;
    };
}



#endif //SMEDIA_INPUTHANDLER_H
