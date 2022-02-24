//
// Created by wansu on 2022/2/22.
//

#include "DataStreamMap.h"
namespace smedia {
    bool DataStreamMap::StreamKey::operator==(const StreamKey& key) const{
        return index == key.index && tag == key.tag;
    }

    size_t DataStreamMap::StreamKey::getHash() const{
        return hash(index) + hash(tag);
    }

    bool DataStreamMap::StreamKey::operator!=(const DataStreamMap::StreamKey &key) const {
        return !(*this == key);
    }

    void DataStreamMap::setDataStream(DataStream *dataStream, const std::string &tag, int index) {
        StreamKey key = {tag,index};
        mStreamToKey.insert({dataStream,key});
        if (mPortStreamsMap.find(key) == mPortStreamsMap.end()) {
            mPortStreamsMap.insert({key, std::vector<DataStream*>()});
        }
        mPortStreamsMap[key].emplace_back(dataStream);
    }

    DataStreamMap::DataStreamMap() : mUpdateQueueMap(1,[](const StreamKey& k){return k.getHash();},
                     [](const StreamKey& k1,const StreamKey &k2){return k1 == k2;}),
                                     mPortStreamsMap(1, [](const StreamKey& k){return k.getHash();},
                                                     [](const StreamKey& k1,const StreamKey &k2){return k1 == k2;}){
    }

    void DataStreamMap::onDataStreamUpDate(DataStream *dataStream) {
        if (mStreamToKey.find(dataStream) == mStreamToKey.end()) {
            return ;
        }
        auto& key = mStreamToKey[dataStream];
        // 如果端口的更新队列还没创建，则先创建
        if (mUpdateQueueMap.find(key) == mUpdateQueueMap.end()) {
            mUpdateQueueMap.insert({key,std::queue<DataStream*>()});
        }
        auto& queue = mUpdateQueueMap[key];
        queue.push(dataStream);
    }

    DataStream *DataStreamMap::getUpdateDataStream(const std::string &tag, int index, bool pop) {
        StreamKey key{tag,index};
        if (mUpdateQueueMap.find(key) == mUpdateQueueMap.end()) {
            return nullptr;
        }

        auto& queue = mUpdateQueueMap[key];
        if (queue.empty()) {
            return nullptr;
        }
        auto* res = queue.front();
        if (pop) {
            queue.pop();
        }
        return res;
    }

    std::vector<DataStream *> DataStreamMap::getPortStream(const std::string &tag, int index) {
        if (index != -1) {
            StreamKey key = {tag,index};
            if (mPortStreamsMap.find(key) == mPortStreamsMap.end()) {
                return std::vector<DataStream*>();
            }
            return mPortStreamsMap[key];
        }
        std::vector<DataStream*> res;
        for (auto& item : mPortStreamsMap) {
            if (item.first.tag != tag) {
                continue;
            }
            for (auto* s : item.second) {
                res.emplace_back(s);
            }
        }
        return res;
    }

    int DataStreamMap::getTagCount(const std::string &tag) const{
        int count = 0;
        for (auto& item : mPortStreamsMap) {
            if (item.first.tag == tag) {
                count++;
            }
        }
        return count;
    }

    std::unique_ptr<std::set<std::string>> DataStreamMap::getTags() const{
        auto* res = new std::set<std::string>;
        for (auto& item : mPortStreamsMap) {
            res->insert(item.first.tag);
        }
        return std::unique_ptr<std::set<std::string>>(res);
    }

    DataStream *DataStreamMap::getUpdateFrontStream(std::string &tag, int index, bool pop) {

    }

}