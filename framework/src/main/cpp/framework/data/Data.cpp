//
// Created by wansu on 2021/11/3.
//

#include "Data.h"
namespace smedia {
    bool Data::isEmpty() {
        return m_dataHolderPtr == nullptr;
    }

    Data::Data(const Data& data):m_dataHolderPtr(data.m_dataHolderPtr) {
    }

    Data::Data(Data &&data) noexcept{
        m_dataHolderPtr = std::move(data.m_dataHolderPtr);
    }

    Data& Data::operator=(const Data& data){
        if (&data != this) {
            m_dataHolderPtr = data.m_dataHolderPtr;
        }
        return *this;
    }

    Data& Data::operator=(Data&& data) noexcept{
        if (&data != this) {
            m_dataHolderPtr = std::move(data.m_dataHolderPtr);
            data.m_dataHolderPtr = nullptr;
        }
        return *this;
    }


}