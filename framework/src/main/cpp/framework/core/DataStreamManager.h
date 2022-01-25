//
// Created by wansu on 2021/11/19.
//

#ifndef SMEDIA_DATASTREAMMANAGER_H
#define SMEDIA_DATASTREAMMANAGER_H
#include "DataStream.h"
#include "../data/Data.h"
#include <string>
#include <map>
#include <internal.h>
#include <unordered_map>
namespace smedia {

    class DataStreamManager {
    public:
        struct StreamKey {
            std::string tag;
            int index;
            // 实现==运算符和getHash方法，用于unordered_map
            bool operator==(const StreamKey& key) const;
            bool operator!=(const StreamKey& key) const;
            size_t getHash() const;
        };
        DataStreamManager();
        // isObserve参数表示是否监听其数据变化
        void setDataStream(DataStream* dataStream,const std::string& tag,int index,bool isObserve);
        // 没有输入index表示functor仅支持一个输入tag，所以取第0个即可
        // 若有多个tag，外部需要使用循环来遍历所有的输入
        Data GetFront(const std::string& tag,int index = 0);
        Data PopFront(const std::string& tag,int index = 0);
        StreamKey getFrontStreamKey() ;
        int getTagCount(const std::string& tag) const;
        std::unique_ptr<std::vector<std::string>> getTags() const;
        void PushData(Data& data,const std::string& tag,int index = -1);
    public:
       void onDataStreamUpdate(DataStream* dataStream);

    private:
        // 自定义key的map,方便从StreamKey来获取DataStream*
        std::unordered_map<StreamKey,DataStream*,std::function<size_t(StreamKey)>,std::function<bool(StreamKey,StreamKey)>> m_KeyToStreamMap;
        std::map<DataStream*,StreamKey> m_StreamToKeyMap;

        // 提供两种方式来获取输入：使用tag+index ，or 从streamKeyQueue中pop
        // 使用前者每次获取一次data会从这里pop出来一个key
        // 所以一般一个functor不要两种方式混用，否则可能出现pop出来的key找不到数据的情况
        std::queue<StreamKey> m_updateStreams;
        std::mutex m_streamKeyLock;

    private:
        Data getDataInner(const std::string& tag,int index,bool pop);
        StreamKey getFrontStreamKeyInner(bool isPop) ;
        void pushStreamKeyInner(StreamKey& key);
    };
}



#endif //SMEDIA_DATASTREAMMANAGER_H
