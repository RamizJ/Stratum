#ifndef FONTHELPER_H
#define FONTHELPER_H

#include "stdata_global.h"

#include <QString>

namespace StData {

class STDATASHARED_EXPORT FontHelper
{
public:
    static QString getCleanName(QString name);
    static double convertPixelToPointSize(double pixelSize);
    static double convertPointToPixelSize(double pointSize);
};

}

#endif // FONTHELPER_H
