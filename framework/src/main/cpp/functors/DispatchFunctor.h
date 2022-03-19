//
// Created by wansu on 2021/12/31.
//

#ifndef SMEDIA_DISPATCHFUNCTOR_H
#define SMEDIA_DISPATCHFUNCTOR_H
#include "IFunctor.h"
#include "InputHandler.h"
#include <map>
#include <deque>
/**
 * 分流器,从主干流中分出来。
 * 输入主干为Data，其他的输入tag由图来自定义
 * 如    {
      "functor": "DispatchFunctor",
      "name": "dispatchNode",
      "inputs": [
        "master:v6",
        "image:video_dispatch"
      ],
      "outputs": [
        "master:m",
        "image:image_dispatch"
      ]
    }
    从video_dispatch的输入会记录时间戳，当master的输入时间相同，则会把该输入转发到同端口名的输出，
    即image_dispatch
 */
namespace smedia {
    class DispatchFunctor : public IFunctor{
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

        ~DispatchFunctor() override = default;

    private:
        bool parseGraphTag();

    public:
        FunctorContext* mFunctorContext;
        InputHandler mInputHandler;
        std::map<double,PortKey> mDispatchMap;
    };
}



#endif //SMEDIA_DISPATCHFUNCTOR_H
