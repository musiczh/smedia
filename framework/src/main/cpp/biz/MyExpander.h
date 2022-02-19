//
// Created by wansu on 2022/2/14.
//

#ifndef SMEDIA_MYEXPANDER_H
#define SMEDIA_MYEXPANDER_H
#include "Expander.h"
#include <map>


namespace smedia {
    class MyExpander : public Expander{
    public:
        std::unique_ptr<IFunctor> createExpandFunctor(const std::string& name) override;

    };
}



#endif //SMEDIA_MYEXPANDER_H
