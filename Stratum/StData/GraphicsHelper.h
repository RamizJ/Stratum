#ifndef GRAPHICSHELPER_H
#define GRAPHICSHELPER_H

#include "stdata_global.h"

#include <QColor>
#include <QImage>
#include <QPixmap>

namespace StData {

class St2000DataStream;

class STDATASHARED_EXPORT GraphicsHelper
{
public:
    static QColor colorFromBytes(const QByteArray& bytes);
    static QColor colorFromTemplate(QString colorStr, const QColor& defColor = Qt::black);
    static int rgbaFromBytes(const QByteArray& bytes);

    static QImage imageFromData(St2000DataStream* stream);
    static QImage imageFromBits(QByteArray& colorIndexes);

    static QPixmap loadMaskedPixmap(const QString& fileName);

private:
    GraphicsHelper()
    {}
};

}

#endif // GRAPHICSHELPER_H
