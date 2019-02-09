#ifndef FONT_H
#define FONT_H

#include "HandleObject.h"

#include <QtGlobal>
#include <QFont>

#define LF_FACESIZE 32

namespace StCore {

////Структура шрифта WinApi
//struct FontInfoWin
//{
//    qint32 height;
//    qint32 width;
//    qint32 escapement;
//    qint32 orientation;
//    qint32 weight;

//    quint8 italic;
//    quint8 underline;
//    quint8 strikeOut;
//    quint8 charSet;
//    quint8 outPrecision;
//    quint8 clipPrecision;
//    quint8 quality;
//    quint8 pitchAndFamily;

//    quint8 faceName[LF_FACESIZE];
//};

////Расширеная структура шрифта WinApi в Sc2000
//struct FontInfoWinExtra
//{
//    quint16  size;

//    qint32 height;
//    qint32 width;
//    qint32 escapement;
//    qint32 orientation;
//    qint32 weight;

//    quint8 italic;
//    quint8 underline;
//    quint8 strikeOut;
//    quint8 charSet;
//    quint8 outPrecision;
//    quint8 clipPrecision;
//    quint8 quality;
//    quint8 pitchAndFamily;

//    qint8 faceName[LF_FACESIZE];
//    /* Структура аналогичная LOGFONT */
//    qint32 fontSize; // Двойной размер шрифта в пунктах
//    qint32 style;    // дополнительные установки
//};

struct FontInfo
{
    qint16 height;
    qint16 width;
    qint16 escapement;
    qint16 orientation;
    qint16 weight;

    quint8 italic;
    quint8 underline;
    quint8 strikeOut;
    quint8 charSet;
    quint8 outPrecision;
    quint8 clipPrecision;
    quint8 quality;
    quint8 pitchAndFamily;

    quint8  faceName[32];
};

class Font : public HandleObject
{
public:
    explicit Font(){}

    QFont font() const {return m_font;}
    void setFont(const QFont& font) {m_font = font;}

private:
    QFont m_font;
};

}

#endif // FONT_H
