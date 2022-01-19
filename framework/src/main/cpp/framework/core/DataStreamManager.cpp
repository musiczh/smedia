//
// Created by wansu on 2021/11/19.
//

#include "DataStreamManager.h"
namespace smedia {

    DataStreamManager::DataStreamManager()
    :m_KeyToStreamMap(4,
                      [](const StreamKey& k){
        return k.getHash();},
        [](const StreamKey& k1,const StreamKey &k2){
        return k1 == k2;
    })
    {
        // 空构造器
    }

    Data DataStreamManager::GetFront(const std::string &tag, int index) {
        return getDataInner(tag,index, false);
    }

    Data DataStreamManager::getDataInner(const std::string &tag, int index, bool pop) {
        StreamKey key = {tag,index};
        DataStream* dataStream = m_KeyToStreamMap[key];
        Data newData;
        if (dataStream != nullptr && !dataStream->empty()) {
            if (pop) {
                // todo 这里无论tag和index的类型如何，都是从streamKeyQueue的队列头弹出一个
                //  所以如果使用指定tag+index和popFront方法混用的话，容易出现前者得到的Data为空，
                //  而有些DataStream的数据却无法被弹出，造成内存泄露。所以这里暂时不能两者或者数据混用，只能使用一种。
                //  后续再做优化
//                StreamKey popKey = getFrontStreamKeyInner(true);
//                // 遍历整个队列，找到相同的streamKey对象并弹出
//                while (key != popKey && popKey.tag != "emptyStreamKey") {
//                    pushStreamKeyInner(popKey);
//                    popKey = getFrontStreamKeyInner(true);
//                }
//                if (popKey.tag != "emptyStreamKey") {
//                    dataStream->popData(newData);
//                }
                // 获取一个data并弹出一个streamKey
                getFrontStreamKeyInner(true);
                dataStream->popData(newData);
            } else {
                dataStream->getFront(newData);
            }
        }
        if (newData.isEmpty()) {
            // 如果上面没有获取到数据，这里返回空数据
            LOG_INFO << "return null data";
        }
        return newData;
    }

    Data DataStreamManager::PopFront(const std::string &tag, int index) {
        return getDataInner(tag,index, true);
    }

    DataStreamManager::StreamKey DataStreamManager::getFrontStreamKey(){
        return getFrontStreamKeyInner(false);
    }

    void DataStreamManager::onDataStreamUpdate(DataStream *dataStream) {
        if (m_StreamToKeyMap.find(dataStream) != m_StreamToKeyMap.end()) {
            pushStreamKeyInner(m_StreamToKeyMap[dataStream]);
        } else {
            LOG_INFO << "the update Stream is not register in DataStreamManager";
        }
    }

    void DataStreamManager::PushData(Data &data, const std::string &tag, int index) {
        if (index != -1) {
            StreamKey key{tag,index};
            if (m_KeyToStreamMap.find(key) != m_KeyToStreamMap.end()) {
                m_KeyToStreamMap[key]->pushData(data);
            }
            return ;
        }

        // index == -1表示输出到所有的tag
        for (auto & entry : m_KeyToStreamMap) {
            if (entry.first.tag == tag) {
                entry.second->pushData(data);
            }
        }
    }

    void DataStreamManager::setDataStream(DataStream *dataStream, const std::string &tag, int index, bool isObserve) {
        StreamKey key{tag,index};
        // 保存到两个映射中
        m_KeyToStreamMap[key] = dataStream;
        m_StreamToKeyMap[dataStream] = key;
        if (isObserve) {
            dataStream->setDataListener([this](DataStream* updateStream){
                this->onDataStreamUpdate(updateStream);
            });
        }
    }

    int DataStreamManager::getTagCount(const std::string &tag) const {
        int count = 0;
        for (auto& entry : m_StreamToKeyMap) {
            if (entry.second.tag == tag) {
                count++;
            }
        }
        return count;
    }

    DataStreamManager::StreamKey DataStreamManager::getFrontStreamKeyInner(bool isPop) {
        std::unique_lock<std::mutex> lock(m_streamKeyLock);
        if (!m_updateStreams.empty()) {
            StreamKey& key = m_updateStreams.front();
            if (isPop) {
                m_updateStreams.pop();
            }
            return key;
        }
        LOG_INFO << "return blank stream key";
        return {"emptyStreamKey",-2};
    }

    void DataStreamManager::pushStreamKeyInner(DataStreamManager::StreamKey& key) {
        std::unique_lock<std::mutex> lock(m_streamKeyLock);
        m_updateStreams.emplace(key);
    }

    bool DataStreamManager::StreamKey::operator==(const StreamKey& key) const{
        return index == key.index && tag == key.tag;
    }

    size_t DataStreamManager::StreamKey::getHash() const{
        return hash(index) + hash(tag);
    }

    bool DataStreamManager::StreamKey::operator!=(const DataStreamManager::StreamKey &key) const {
        return !(*this == key);
    }

}