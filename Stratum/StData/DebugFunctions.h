#ifndef DEBUGFUNCTIONS_H
#define DEBUGFUNCTIONS_H

#include "stdata_global.h"

#include <QFile>
#include <QString>
#include <QTextStream>

class STDATASHARED_EXPORT DebugFunctions
{
public:
    static void SaveDataToFile(const QString& fileName, const QByteArray& data);
};


#endif // DEBUGFUNCTIONS_H
