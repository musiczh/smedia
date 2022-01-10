//
// Created by wansu on 2021/11/17.
//

#include "FunctorRegister.h"
namespace smedia {
    #define REGISTER_FUNCTOR_LAZY(functorName) \
    extern void register_functor_##functorName(); \
    register_functor_##functorName();\

    using Register = FunctionRegister<std::unique_ptr<IFunctor>>;
    bool hasRegisterFunctors = false;
    void lazyRegisterAllFunctors();

    void registerFunctor(const std::string& nodeName, std::function<std::unique_ptr<IFunctor>()> function) {
        LOG_DEBUG << "register functor:" << nodeName;
        Register::registerFunction(nodeName,std::move(function));
    }

    std::unique_ptr<IFunctor> createFunctorByName(std::string &nodeName) {
        if (!hasRegisterFunctors) {
            lazyRegisterAllFunctors();
            hasRegisterFunctors = true;
        }
        // todo 这里需要支持java层的functor，后续版本再支持
        return Register::invokeFunction(nodeName);
    }

    bool isRegisterFunctor(std::string &nodeName) {
        if (!hasRegisterFunctors) {
            lazyRegisterAllFunctors();
            hasRegisterFunctors = true;
        }
        return Register ::isRegister(nodeName);
    }

    void lazyRegisterAllFunctors() {
        // 处理每个functor的注册
        REGISTER_FUNCTOR_LAZY(OESTexReaderFunctor)
        REGISTER_FUNCTOR_LAZY(BrightnessFunctor)
        REGISTER_FUNCTOR_LAZY(RenderFunctor)
        REGISTER_FUNCTOR_LAZY(ContrastFunctor)
        REGISTER_FUNCTOR_LAZY(SaturationFunctor)
        REGISTER_FUNCTOR_LAZY(AdjustColorFunctor)
        REGISTER_FUNCTOR_LAZY(BeautyFunctor)
        REGISTER_FUNCTOR_LAZY(DeNoiseFunctor)
        REGISTER_FUNCTOR_LAZY(SharpenFunctor)
        REGISTER_FUNCTOR_LAZY(WhiteBalanceFunctor)
        REGISTER_FUNCTOR_LAZY(HighLightShadowFunctor)
        REGISTER_FUNCTOR_LAZY(CallbackFunctor)
        REGISTER_FUNCTOR_LAZY(DispatchFunctor)
        REGISTER_FUNCTOR_LAZY(ImageLoader)
        REGISTER_FUNCTOR_LAZY(GLImageReaderFunctor)
        REGISTER_FUNCTOR_LAZY(ImageSourceFunctor)
    }
}