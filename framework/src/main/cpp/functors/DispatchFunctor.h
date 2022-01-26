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
 * 分流器,从主干流中分出一帧来读取bitmap
 * 输入主干为Data，其他的输入tag由图来自定义
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
        void registerInputHandler();
        bool parseGraphTag();

    public:
        FunctorContext* mFunctorContext;
        InputHandler mInputHandler;

        std::deque<std::string> mTagList;
        std::map<double,std::string> mDispatchMap;

        volatile bool mImageSignal;
    };
}



#endif //SMEDIA_DISPATCHFUNCTOR_H
