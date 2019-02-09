#include "FontHelper.h"

#include <QApplication>
#include <QFontDatabase>
#include <QFontInfo>
#include <QFontMetrics>
#include <QScreen>

#include "Log.h"

namespace StData {

QString FontHelper::getCleanName(QString name)
{
    return name.replace(" CYR", "", Qt::CaseInsensitive).trimmed();
}

double FontHelper::convertPixelToPointSize(double pixelSize)
{
    //Работает для Android
    double dpi = QApplication::primaryScreen()->logicalDotsPerInch();
    double pointSize = pixelSize * 96 / dpi;

    SystemLog::instance().info(QString("dpi: %1; pointsize: %2").arg(dpi).arg(pointSize));

    return pointSize;
}

double FontHelper::convertPointToPixelSize(double pointSize)
{
    //Работает для Android
    double dpi = QApplication::primaryScreen()->logicalDotsPerInchY();
    double pixelSize = pointSize * dpi / 96;
    return pixelSize;
}



}
