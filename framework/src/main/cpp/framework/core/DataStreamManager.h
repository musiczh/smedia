//
// Created by wansu on 2021/11/19.
//

#ifndef SMEDIA_DATASTREAMMANAGER_H
#define SMEDIA_DATASTREAMMANAGER_H
#include "DataStreamMap.h"
namespace smedia {
    class DataStreamManager {
    public:
        // 从DataStream映射到StreamKey
        DataStreamManager(bool isInput);
        // isObserve参数表示是否监听其数据变化。主要是区分输出和输入边，输出边不需要监听其数据变化
        void setDataStream(DataStream* dataStream,const std::string& tag,int index);
        Data getInputData(const std::string& tag,int index,bool pop);
        void pushOutputData(Data data,const std::string& tag,int index);
        int getTagCount(const std::string& tag) const;
        std::unique_ptr<std::set<std::string>> getTags() const;
    public:
       void onDataStreamUpdate(DataStream* dataStream);

    private:

        // 提供两种方式来获取输入：使用tag+index ，or 从streamKeyQueue中pop
        // 使用前者每次获取一次data会从这里pop出来一个key
        // 所以一般一个functor不要两种方式混用，否则可能出现pop出来的key找不到数据的情况
        std::mutex mMapLock;
        bool mIsInput{false};
        DataStreamMap mDataStreamMap{};

    private:
        Data getDataInner(const std::string& tag,int index,bool pop);
    };
}



#endif //SMEDIA_DATASTREAMMANAGER_H
