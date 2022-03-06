//
// Created by wansu on 2022/2/14.
//

#ifndef SMEDIA_EXPANDER_H
#define SMEDIA_EXPANDER_H
#include "IFunctor.h"
#include <memory>
namespace smedia {
    class Expander {
    public:
        virtual std::unique_ptr<IFunctor> createExpandFunctor(const std::string& name) = 0;
        virtual ~Expander() = default;
    };
}


#endif //SMEDIA_EXPANDER_H
