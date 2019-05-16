#include "StDataGlobal.h"

#include <QTextCodec>
#include <stdexcept>

namespace StData {

QTextCodec* textCodec = nullptr;


QColor intToColor(int agbr)
{
    int r = agbr & 255;
    int g = (agbr >> 8) & 255;
    int b = (agbr >> 16) & 255;
    int a = (agbr >> 24) & 255;

    if(a == 0)
        a = 255;

    return QColor(r, g, b, a);
}

QColor intToTextColor(int agbr)
{
    int r = agbr & 255;
    int g = (agbr >> 8) & 255;
    int b = (agbr >> 16) & 255;
    int a = ((agbr & 0x01000000l) != 0) ? 0 : 255;

    return QColor(r, g, b, a);
}

int colorToInt(const QColor& color)
{
    int abgr = color.red();
    abgr |= (color.green() << 8);
    abgr |= (color.blue() << 16);
    abgr |= (color.alpha() << 24);

    return abgr;
}

int rgbaToInt(int r, int g, int b, int a)
{
    int abgr = r;
    abgr |= (g << 8);
    abgr |= (b << 16);
    abgr |= (a << 24);

    return abgr;
}


void initTextCodec()
{
    textCodec = QTextCodec::codecForName("windows-1251");
}

QTextCodec* getTextCodec()
{
    return QTextCodec::codecForName("windows-1251");
}

QString fromWindows1251(const QByteArray& data)
{
    if(textCodec == nullptr)
        throw std::runtime_error("Cannot convert data to unicode. TextCodec not found");

    int length = static_cast<int>(strlen(data.constData()));
    return textCodec->toUnicode(data, length);
//    return QString::fromLocal8Bit(data);
}

QString fromWindows1251(const QByteArray& data, int length)
{
    if(textCodec == nullptr)
        throw std::runtime_error("Cannot convert data to unicode. TextCodec not found");

    return textCodec->toUnicode(data, length);
//    return QString::fromLocal8Bit(data, length);
}

QString fromWindows1251(char* data, int length)
{
    if(textCodec == nullptr)
        throw std::runtime_error("Cannot convert data to unicode. TextCodec not found");

    return textCodec->toUnicode(data, length);
//    return QString::fromLocal8Bit(data, length);
}

QByteArray toWindows1251(const QString& data)
{
    if(textCodec == nullptr)
        throw std::runtime_error("Cannot convert data to local. TextCodec not found");

    return textCodec->fromUnicode(data);
//    return data.toLocal8Bit();
}

}
