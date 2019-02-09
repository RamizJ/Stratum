#ifndef HANDLEITEM_H
#define HANDLEITEM_H

#include "stdata_global.h"
#include <QObject>

namespace StData {

class STDATASHARED_EXPORT HandleItem
{
public:
    HandleItem(int handle = 0);
    HandleItem(const HandleItem& other);

    int handle() const {return m_handle;}
    void setHandle(int handle) {m_handle = handle;}

private:
    int m_handle;
};

}

#endif // HANDLEOBJECT_H
