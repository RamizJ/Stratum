#ifndef STDATAGLOBAL_H
#define STDATAGLOBAL_H

#include <QColor>
#include <QtMath>

#include "stdata_global.h"

namespace StData {

extern QTextCodec* textCodec;

extern STDATASHARED_EXPORT int colorToInt(const QColor& color);
extern STDATASHARED_EXPORT int rgbaToInt(int r, int g, int b, int a = 255);
extern STDATASHARED_EXPORT QColor intToColor(int agbr);
extern STDATASHARED_EXPORT QColor intToTextColor(int agbr);

extern STDATASHARED_EXPORT void initTextCodec();
extern STDATASHARED_EXPORT QTextCodec* getTextCodec();

extern STDATASHARED_EXPORT QString fromWindows1251(const QByteArray& data);
extern STDATASHARED_EXPORT QString fromWindows1251(const QByteArray& data, int length);
extern STDATASHARED_EXPORT QString fromWindows1251(char* data, int length);

extern STDATASHARED_EXPORT QByteArray toWindows1251(const QString& data);

//extern STDATASHARED_EXPORT QString toUnicode(const char* data, int length);

}

#endif // STDATAGLOBAL_H
