#ifndef SORTEDCOLLECTION_H
#define SORTEDCOLLECTION_H

#include <stdata_global.h>
#include <QList>

namespace StData {

template <class T>
class DefaultComparer
{
public:
    int compare(const T& item1, const T& item2)
    {
        if(item1 == item2)
            return 0;

        return item1 < item2 ? -1 : 1;
    }
};

template<class T, class Comparer = DefaultComparer<T> >
bool binaryFindIndex(QList<T> items, const T& item, int& index,
                     bool duplicatingEnabled = false)
{
    {
        Comparer comparer;

        bool result = false;
        int first = 0;
        int last = items.count() - 1;
        int mid;

        while(first <= last)
        {
            mid = (first + last) >> 1;
            int  res = comparer.compare(items.at(mid), item);

            if(res < 0)
                first = mid + 1;

            else
            {
                last = mid - 1;
                if(res == 0)
                {
                    result = true;
                    if(!duplicatingEnabled)
                        first = mid;
                }
            }
        }
        index = first;
        return result;
    }

}

template<class T, class Comparer = DefaultComparer<T> >
class  SortedCollection
{
public:
    explicit SortedCollection(bool duplicatingEnabled = false) :
        m_isDuplicatingEnabled(duplicatingEnabled)
    {}

    virtual ~SortedCollection()
    {}

    QList<T> items() const {return m_items;}
    T at(int i){return m_items.at(i);}
    int count() const {return m_items.count();}

    int insert(const T& item);
    SortedCollection<T, Comparer>& operator << (const T& item);

protected:
    QList<T> m_items;
    Comparer* m_comparer;
    bool m_isDuplicatingEnabled;
};

template<class T, class Comparer>
int SortedCollection<T, Comparer>::insert(const T& item)
{
    int index;
    if(!binaryFindIndex<T, Comparer>(m_items, item, index, m_isDuplicatingEnabled) || m_isDuplicatingEnabled)
    {
        m_items.insert(index, item);
        return index;
    }
    return -1;
}

template<class T, class Comparer>
SortedCollection<T, Comparer>& SortedCollection<T, Comparer>::operator <<(const T& item)
{
    insert(item);
    return *this;
}

}

#endif // SORTEDCOLLECTION_H
