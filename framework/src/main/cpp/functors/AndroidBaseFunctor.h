//
// Created by wansu on 2022/2/15.
//

#ifndef SMEDIA_ANDROIDBASEFUNCTOR_H
#define SMEDIA_ANDROIDBASEFUNCTOR_H
#include "JNIObject.h"
#include "IFunctor.h"
namespace smedia {
    class AndroidBaseFunctor : public IFunctor{
    public:
        bool initialize(FunctorContext *context) override;

        void unInitialize(FunctorContext *context) override;

        bool execute(FunctorContext *context) override;

        void setOption(FunctorContext *context, const std::string &key, Data value) override;

        ~AndroidBaseFunctor() override = default;

        AndroidBaseFunctor() = default;

        bool init(const std::string& name);

    private:
        JNIObjectRef mAndroidFunctor;
        std::string mFunctorName;
    };
}



#endif //SMEDIA_ANDROIDBASEFUNCTOR_H
