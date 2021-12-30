//
// Created by wansu on 2021/12/10.
//

#ifndef SMEDIA_EFFECTMANAGER_H
#define SMEDIA_EFFECTMANAGER_H


class EffectManager {
public:
    void init();
    void addFilter(IFilter filter);
    void run();
    void release();


};


#endif //SMEDIA_EFFECTMANAGER_H
