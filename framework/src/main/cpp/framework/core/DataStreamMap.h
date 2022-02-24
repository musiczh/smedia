//
// Created by wansu on 2022/2/22.
//

#ifndef SMEDIA_DATASTREAMMAP_H
#define SMEDIA_DATASTREAMMAP_H
#include "DataStream.h"
#include "Data.h"
#include <string>
#include <map>
#include <vector>
#include <internal.h>
#include <unordered_map>
#include <set>

namespace smedia {
    class DataStreamMap {
    public:
        struct StreamKey {
            std::string tag;
            int index;
            // 实现==运算符和getHash方法，用于unordered_map
            bool operator==(const StreamKey& key) const;
            bool operator!=(const StreamKey& key) const;
            size_t getHash() const;
        };
        // 从StreamKey映射到DataStream
        using KeyToQueueMap =  std::unordered_map<StreamKey,
                                                std::queue<DataStream*>,
                                                std::function<size_t(StreamKey)>,
                                                std::function<bool(StreamKey,StreamKey)>>;
        using KeyToVectorMap =  std::unordered_map<StreamKey,
                                                std::vector<DataStream*>,
                                                std::function<size_t(StreamKey)>,
                                                std::function<bool(StreamKey,StreamKey)>>;
    public:
        DataStreamMap();
        void setDataStream(DataStream* dataStream,const std::string& tag,int index);

        void onDataStreamUpDate(DataStream* dataStream);
        DataStream* getUpdateDataStream(const std::string& tag,int index,bool pop);
        DataStream* getUpdateFrontStream(std::string& tag,int index,bool pop);

        std::vector<DataStream*> getPortStream(const std::string& tag,int index);

        int getTagCount(const std::string& tag) const;
        std::unique_ptr<std::set<std::string>> getTags() const;

    private:
        std::map<DataStream*,StreamKey> mStreamToKey{};
        KeyToVectorMap mPortStreamsMap;
        KeyToQueueMap mUpdateQueueMap;
    };
}



#endif //SMEDIA_DATASTREAMMAP_H
