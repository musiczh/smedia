//
// Created by wansu on 2021/11/3.
//

#include "Edge.h"
namespace smedia {
    bool Edge::empty() {
        return m_dataQueue.empty();
    }

    void Edge::pushData(Data& data) {
        std::unique_lock<std::mutex> lock(m_lock);
        m_dataQueue.push(data);
        if (m_listener != nullptr) {
            m_listener(this);
        }
    }

    bool Edge::getFront(Data &data) {
        std::unique_lock<std::mutex> lock(m_lock);
        if (m_dataQueue.empty()) {
            return false;
        }
        data = m_dataQueue.front();
        return true;
    }

    bool Edge::popData(Data &data) {
        std::unique_lock<std::mutex> lock(m_lock);
        if (m_dataQueue.empty()) {
            return false;
        }
        data = m_dataQueue.front();
        m_dataQueue.pop();
        return true;
    }

    void Edge::setDataListener(DataListener listener) {
        m_listener = std::move(listener);
    }

    bool PortKey::operator<(const PortKey& key) const{
        return tag < key.tag || index < key.index;
    }
}