//
// Created by wansu on 2021/11/18.
//

#ifndef SMEDIA_DATASTREAM_H
#define SMEDIA_DATASTREAM_H
#include "../data/Data.h"
#include <functional>
#include <queue>
#include <memory>
#include <mutex>
namespace smedia{
    /**
     * Edge中的数据流，数据承载者，本质上是一个数据队列
     * DataStream和常规的queue类似，加了队列空的保护，防止pop的时候抛出异常
     * 可以设置监听，在数据流收到数据的时候会触发监听
     */
    class DataStream {
        using DataListener = std::function<void(DataStream*)>;
    public:
        bool empty();
        void pushData(Data& data);
        bool getFront(Data& data);
        bool popData(Data& data);
        void setDataListener(DataListener listener);

    private:
        DataListener m_listener;
        std::queue<Data> m_dataQueue;
        std::mutex m_lock;
    };
}



#endif //SMEDIA_DATASTREAM_H
