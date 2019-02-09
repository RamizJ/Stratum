#include "StringHelper.h"

namespace StData {

QString StringHelper::TrimRight(const QString& str)
{
    if(str.isNull() || str.isEmpty())
        return "";

    int i = str.length() - 1;
    for(; i >= 0 && str.at(i) == ' '; i--);

    if(i+1 > str.length())
        return "";

    return str.left(i+1);
}

QString StringHelper::TrimLeft(const QString& str)
{
    if(str.isNull() || str.isEmpty())
        return "";

    int i = 0;
    for(; i < str.length() && str.at(i) == QChar(' '); i++);

    if(i > str.length())
        return "";

    return str.mid(i);
}

QString StringHelper::TrimAll(const QString& str)
{
    return TrimLeft(TrimRight(str));
    //return str.trimmed();
}

}
