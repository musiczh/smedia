//
// Created by wansu on 2021/11/18.
//

#include "DataStream.h"

namespace smedia {
    bool DataStream::empty() {
        return m_dataQueue.empty();
    }

    void DataStream::pushData(Data& data) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_dataQueue.push(data);
        if (m_listener != nullptr) {
            m_listener(this);
        }
    }

    bool DataStream::getFront(Data &data) {
        std::unique_lock<std::mutex> lock(m_lock);
        if (m_dataQueue.empty()) {
            return false;
        }
        data = m_dataQueue.front();
        return true;
    }

    bool DataStream::popData(Data &data) {
        std::unique_lock<std::mutex> lock(m_lock);
        if (m_dataQueue.empty()) {
            return false;
        }
        data = m_dataQueue.front();
        m_dataQueue.pop();
        return true;
    }

    void DataStream::setDataListener(DataListener listener) {
        m_listener = std::move(listener);
    }


}

