//
// Created by wansu on 2021/11/19.
//

#include "DataStreamManager.h"
namespace smedia {

    DataStreamManager::DataStreamManager(bool isInput) {
        mIsInput = isInput;
    }

    int DataStreamManager::getTagCount(const std::string &tag) const {
        return mDataStreamMap.getTagCount(tag);
    }

    void
    DataStreamManager::setDataStream(DataStream *dataStream, const std::string &tag, int index) {
        if (mIsInput) {
            dataStream->setDataListener([this](DataStream* s) {
                this->onDataStreamUpdate(s);
            });
        }
        std::unique_lock<std::mutex> lock(mMapLock);
        mDataStreamMap.setDataStream(dataStream,tag,index);
    }

    std::unique_ptr<std::set<std::string>> DataStreamManager::getTags() const {
        return mDataStreamMap.getTags();
    }

    Data DataStreamManager::getInputData(const std::string &tag, int index, bool pop) {
        auto *s = mDataStreamMap.getUpdateDataStream(tag,index,pop);
        if (s == nullptr) {
            return Data();
        }
        Data data;
        if (pop) {
            s->popData(data);
        } else {
            s->getFront(data);
        }
        return data;
    }

    void DataStreamManager::pushOutputData(Data data, const std::string &tag, int index) {
        auto v = mDataStreamMap.getPortStream(tag,index);
        for (auto& dataStream : v) {
            dataStream->popData(data);
        }
    }


}