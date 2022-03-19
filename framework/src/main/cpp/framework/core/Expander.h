//
// Created by wansu on 2022/2/14.
//

#ifndef SMEDIA_EXPANDER_H
#define SMEDIA_EXPANDER_H
#include <memory>
namespace smedia {
    class IFunctor;
    class IService;
    class Expander {
    public:
        virtual std::unique_ptr<IFunctor> createExpandFunctor(const std::string& name) = 0;
        virtual std::unique_ptr<IService> createExpandService(const std::string& name) = 0;
        virtual ~Expander() = default;
    };
}


#endif //SMEDIA_EXPANDER_H
