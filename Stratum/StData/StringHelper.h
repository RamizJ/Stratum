#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <QString>
#include "stdata_global.h"

namespace StData {

class STDATASHARED_EXPORT StringHelper
{
public:
    static QString TrimRight(const QString& str);
    static QString TrimLeft(const QString& str);
    static QString TrimAll(const QString& str);
};

}

#endif // STRINGHELPER_H
