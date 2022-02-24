//
// Created by wansu on 2021/11/3.
//

#include "Data.h"
namespace smedia {
    Data Data::CreateEmptyData() {
        return Data();
    }

    bool Data::isEmpty() {
        return m_dataHolderPtr == nullptr;
    }

    Data::Data(const Data& data):m_dataHolderPtr(data.m_dataHolderPtr),mTimeStamp(data.mTimeStamp) {
    }

    Data::Data(Data &&data) noexcept{
        mTimeStamp = data.mTimeStamp;
        m_dataHolderPtr = std::move(data.m_dataHolderPtr);
    }

    Data& Data::operator=(const Data& data){
        if (&data != this) {
            m_dataHolderPtr = data.m_dataHolderPtr;
            mTimeStamp = data.mTimeStamp;
        }
        return *this;
    }

    Data& Data::operator=(Data&& data) noexcept{
        if (&data != this) {
            mTimeStamp = data.mTimeStamp;
            m_dataHolderPtr = std::move(data.m_dataHolderPtr);
            data.m_dataHolderPtr = nullptr;
        }
        return *this;
    }

    void Data::setTime(double timeStamp) {
        mTimeStamp = timeStamp;
    }

    double Data::getTime() {
        return mTimeStamp;
    }

    long Data::getTypeId() {
        if (m_dataHolderPtr == nullptr) {
            return 0;
        }
        return m_dataHolderPtr->getTypeId();
    }


}