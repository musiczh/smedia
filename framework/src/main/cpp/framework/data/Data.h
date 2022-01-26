//
// Created by wansu on 2021/11/3.
//

#ifndef SMEDIA_DATA_H
#define SMEDIA_DATA_H
#include <memory>
#include "Logger.h"
namespace smedia {
    class DataHolderBase;
    template<typename C>
    class DataHolder;

    /**
     * 存储一个数据指针，本身无泛型，支持模板构建与读取；支持值拷贝。
     * 注意，内部使用shared_ptr来存储，若相关Data被全部析构，则内部保存的数据也会被析构。
     * 如果需要数据不被析构，则需要使用二级指针，这样内部析构的就是一级指针，数据得以保留。
     * 当然，为避免造成内存泄露，必要时再采取此方法
     */
    class Data {
    public:
        template<class U> static Data create(U* data,double timeStamp = 0);
        Data() = default;
        Data(const Data& data);
        Data(Data&& data) noexcept;
        Data& operator=(const Data& data);
        Data& operator=(Data&& data) noexcept;

    public:
        template<class T> bool getData(T& t);
        template<class T> bool isTypeOf();
        template<class T> T* getData();
        bool isEmpty();
        void setTime(double timeStamp);
        double getTime();

    private:
        double mTimeStamp;
        // 支持多个Data同时持有资源，让Data可以使用值传递。最后释放的Data会释放资源
        std::shared_ptr<DataHolderBase> m_dataHolderPtr;

    };

    // 父类不存在泛型，让子类去实现泛型。上层的Data类将是无类型的，可以将泛型的逻辑封装到内部
    // 外部只有存储和取的时候需要提供泛型
    class DataHolderBase {
    public:
        DataHolderBase() = default;
        // DataHolderBase不允许拷贝和赋值，也就是不允许两个对象同时持有一份资源
        // 这和unique_ptr类似
        DataHolderBase(const DataHolderBase&) = delete;
        DataHolderBase& operator= (const DataHolderBase&) = delete;
        // 析构函数必须为虚函数，让子类的析构函数得以被调用
        virtual ~DataHolderBase() = default;

        // 子类实现。获取泛型类型的typeId
        // typeid(T).hashcode();
        virtual size_t getTypeId() const = 0;

        // 转化为某个指定类型的DataHolder
        // 返回指针的原因是可以用过返回null来表示类型不匹配
        // 返回两种数据类型：可修改与不可修改
        template<class T> DataHolder<T>* as();
        template<class T> const DataHolder<T>* as() const;
    };


    template<typename T>
    class DataHolder : public DataHolderBase {
    public:
        explicit DataHolder(T* t);
        size_t getTypeId() const override;

        // 这里不需要做类型判断了，在父类的as方法能成功转换就保证了类型正确
        const T& getData();
        T* getDataPtr();
        ~DataHolder() override;
    private:
        T* m_data;

        template<class K = T>
        inline void deleteData(typename std::enable_if<std::is_array<K>::value>::type* = nullptr);
        template<class K = T>
        inline void deleteData(typename std::enable_if<!std::is_array<K>::value>::type* = nullptr);
    };



    //-------------------------------------------------------


    template<class U>
    Data Data::create(U *data,double timeStamp) {
        Data newData;
        newData.setTime(timeStamp);
        auto* dataHolder = new DataHolder<U>(data);
        newData.m_dataHolderPtr = std::shared_ptr<DataHolderBase>(dataHolder);
        return newData;
    }

    template<class T>
    bool Data::getData(T &t) {
        if (m_dataHolderPtr == nullptr) {
            LOG_DEBUG << "dataHolder is null";
            return false;
        }
        DataHolder<T>* dataHolder = m_dataHolderPtr->as<T>();
        if (dataHolder == nullptr) {
            LOG_ERROR << "data type is not match";
            return false;
        }
        t = dataHolder->getData();
        return true;
    }

    template<class T>
    bool Data::isTypeOf() {
        auto * ptr = m_dataHolderPtr->as<T>();
        return ptr != nullptr;
    }

    template<class T>
    T *Data::getData() {
        DataHolder<T> * ptr = m_dataHolderPtr->as<T>();
        if (ptr == nullptr) {
            return nullptr;
        }
        return ptr->getDataPtr();
    }

    template<class T>
    DataHolder<T> *DataHolderBase::as() {
        if (typeid(T).hash_code() == getTypeId()) {
            return static_cast<DataHolder<T>*>(this);
        }
        return nullptr;
    }

    template<class T>
    const DataHolder<T> *DataHolderBase::as() const{
        if (typeid(T).hash_code() == getTypeId()) {
            // 代码逻辑保证了类型转换的安全，这里用static_cast可以提高性能
            const auto* ptr = static_cast<const DataHolder<T>*>(this);
            return ptr;
        }
        return nullptr;
    }

    template<typename T>
    size_t DataHolder<T>::getTypeId() const {
        return typeid(T).hash_code();
    }

    template<typename T>
    const T &DataHolder<T>::getData() {
        return *m_data;
    }

    template<typename T>
    T *DataHolder<T>::getDataPtr() {
        return m_data;
    }

    template<typename T>
    DataHolder<T>::~DataHolder() {
        deleteData();
    }

    template<typename T>
    template<typename K>
    void DataHolder<T>::deleteData(typename std::enable_if<std::is_array<K>::value>::type *) {
        delete[] m_data;
    }

    template<typename T>
    template<typename K>
    void DataHolder<T>::deleteData(typename std::enable_if<!std::is_array<K>::value>::type *) {
        delete m_data;
    }

    template<typename T>
    DataHolder<T>::DataHolder(T *t) {
        m_data = t;
    }

}


#endif //SMEDIA_DATA_H
