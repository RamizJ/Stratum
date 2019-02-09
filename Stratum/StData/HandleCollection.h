#ifndef HANDLECOLECTION_H
#define HANDLECOLECTION_H

#include <QList>
#include <QMap>

namespace StData {

template<class T>
class HandleCollection
{
public:
    HandleCollection(int startHandle = 1) :
        m_startHandle(startHandle)
    {}

    HandleCollection(const HandleCollection& other)
    {
        m_itemForHandleMap = other.itemForHandleMap();
        m_startHandle = other.m_startHandle;
    }

    void operator = (const HandleCollection& other)
    {
        m_itemForHandleMap = other.itemForHandleMap();
    }

    QList<int> handles() const { return m_itemForHandleMap.keys(); }
    QList<T*> items() const { return m_itemForHandleMap.values(); }
    QMap<int, T*> itemForHandleMap() const {return m_itemForHandleMap;}

    T* createItem()
    {
        int handle = getFreeHandle();

        T* t = new T(handle);
        m_itemForHandleMap[handle] = t;

        return t;
    }

    /* return item handle */
    int insert(T* item, bool keepHandle = false)
    {
        int handle = keepHandle ? item->handle() : getFreeHandle();

        if(keepHandle)
        {
            if(T* oldItem = m_itemForHandleMap.take(handle))
                delete oldItem;
        }
        else
            item->setHandle(handle);

        m_itemForHandleMap[handle] = item;
        return handle;
    }

    void insert(QList<T*> items, bool keepHandle = false)
    {
        for(T* item : items)
            insert(item, keepHandle);
    }

    int insertCopy(T* item)
    {
        int handle = getFreeHandle();
        T* copy = new T(*item);
        copy->setHandle(handle);

        m_itemForHandleMap[handle] = copy;
        return handle;
    }

//    void insert(T* item)
//    {
//        int handle = item->handle();

//        T* oldItem = m_itemForHandleMap.value(handle);
//        if(oldItem)
//            delete oldItem;

//        m_itemForHandleMap[handle] = item;
//    }

    T* getItem(int handle) const
    {
        return m_itemForHandleMap.value(handle, nullptr);
    }

    T* take(int handle)
    {
        return m_itemForHandleMap.take(handle);
    }

    void remove(int handle)
    {
        m_itemForHandleMap.remove(handle);
    }

//    void setItem(int handle, const T& item)
//    {
//        m_items[handle - 1] = item;
//    }

    int count() const
    {
        return m_itemForHandleMap.count();
    }

    bool isEmpty() const {return m_itemForHandleMap.isEmpty();}

    void deleteItem(int handle)
    {
        if(T* item = m_itemForHandleMap.take(handle))
            delete item;
    }

    void clear()
    {
        m_itemForHandleMap.clear();
    }

    void deleteAll()
    {
        qDeleteAll(m_itemForHandleMap);
        m_itemForHandleMap.clear();
    }

    int getNextHandle(int fromHandle)
    {
        if(fromHandle == 0)
            return m_itemForHandleMap.firstKey();

        auto it = m_itemForHandleMap.find(fromHandle);
        if(it == m_itemForHandleMap.end() || it == (m_itemForHandleMap.end()-1))
            return 0;

        return (++it).key();
    }

private:
    int getFreeHandle()
    {
        int handle = m_startHandle;
        for(; m_itemForHandleMap.contains(handle); handle++);
        return handle;
    }

private:
    QMap<int, T*> m_itemForHandleMap;
    int m_startHandle;
};


//template<class T>
//class HandleCollection
//{
//public:
//    typedef std::shared_ptr<QMap<int, T*>> HandleMapPtr;

//    HandleCollection()
//    {
////        m_itemForHandleMap = std::make_shared<QMap<int, T*>>();
//        m_itemForHandleMap = HandleMapPtr(new QMap<int, T*>);
//    }

//    HandleCollection(const HandleCollection& other)
//    {
//        m_itemForHandleMap = other.itemForHandleMap();
//    }

//    void operator = (const HandleCollection& other)
//    {
//        m_itemForHandleMap = other.itemForHandleMap();
//    }

//    QList<int> handles() const { return m_itemForHandleMap->keys(); }
//    QList<T*> items() const { return m_itemForHandleMap->values(); }
//    HandleMapPtr itemForHandleMap() const {return m_itemForHandleMap;}

//    T* createItem()
//    {
//        int handle = getFreeHandle();

//        T* t = new T(handle);
//        (*m_itemForHandleMap)[handle] = t;

//        return t;
//    }

//    void insert(T* item)
//    {
//        int handle = item->handle();

//        T* oldItem = m_itemForHandleMap->value(handle);
//        if(oldItem)
//            delete oldItem;

//        (*m_itemForHandleMap)[handle] = item;
//    }

//    T* getItem(int handle) const
//    {
//        return m_itemForHandleMap->value(handle);
//    }

//    T* take(int handle)
//    {
//        return m_itemForHandleMap->take(handle);
//    }

//    void remove(int handle)
//    {
//        m_itemForHandleMap->remove(handle);
//    }

////    void setItem(int handle, const T& item)
////    {
////        m_items[handle - 1] = item;
////    }

//    int count() const
//    {
//        return m_itemForHandleMap->count();
//    }

//    int getNextHandle(int fromHandle)
//    {
//        if(fromHandle == 0)
//            return m_itemForHandleMap->firstKey();

//        auto it = m_itemForHandleMap->find(fromHandle);
//        if(it == m_itemForHandleMap->end() || it == m_itemForHandleMap->end()-1)
//            return 0;

//        return (++it).key();
//    }

//    bool isEmpty() const {return m_itemForHandleMap->isEmpty();}

//    void removeOne(int handle)
//    {
//        m_itemForHandleMap->remove(handle);
//    }

//    void deleteItem(int handle)
//    {
//        T* item = m_itemForHandleMap->take(handle);
//        if(item)
//            delete item;

////        (*m_itemForHandleMap)[handle-1] = nullptr;
//    }

//    void clear()
//    {
//        m_itemForHandleMap->clear();
//    }

//    void deleteAll()
//    {
//        qDeleteAll(*m_itemForHandleMap);
//        m_itemForHandleMap->clear();
//    }

//    int getFreeHandle()
//    {
//        int handle = 1;
//        for(; m_itemForHandleMap->contains(handle); handle++);
//        return handle;
//    }

//private:
//    HandleMapPtr m_itemForHandleMap;
//};

}

#endif // HANDLECOLECTION_H
