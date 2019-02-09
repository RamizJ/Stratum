#include "GraphicsHelper.h"
#include "Sc2000DataStream.h"
#include "Tool2d.h"
#include "StDataGlobal.h"

#include <QBitArray>
#include <QFile>
#include <QFileInfo>

namespace StData {

QColor GraphicsHelper::colorFromBytes(const QByteArray& bytes)
{
    quint8 r = 0, g = 0, b = 0, a = 255;

    if(bytes.count() >= 3)
    {
        r = bytes[0];
        g = bytes[1];
        b = bytes[2];
    }
    if(bytes.count() == 4 && (bytes[3] == 1 || bytes[3] == -1))
        a = 0;

    return QColor(r, g, b, a);
}

QColor GraphicsHelper::colorFromTemplate(QString colorStr, const QColor& defColor)
{
    QColor c = defColor;
    if(colorStr.contains("rgb(", Qt::CaseInsensitive))
    {
        QStringList rgbValues = colorStr.remove(0, 4).remove(')').split(',');
        if(rgbValues.count() == 3)
            c = QColor(rgbValues[0].toInt(), rgbValues[1].toInt(), rgbValues[2].toInt());
    }
    else if(colorStr.contains("Transparent", Qt::CaseInsensitive))
        c = QColor(Qt::transparent);

    return c;
}

int GraphicsHelper::rgbaFromBytes(const QByteArray& bytes)
{
    quint8 r = 0, g = 0, b = 0, a = 255;

    if(bytes.count() >= 3)
    {
        r = bytes[0];
        g = bytes[1];
        b = bytes[2];
    }
    if(bytes.count() == 4 && bytes[3] == 1)
        a = 0;

    return rgbaToInt(r, g, b, a);
}

QImage GraphicsHelper::imageFromData(St2000DataStream* stream)
{
    QImage result;

    int pos = stream->pos();
    QByteArray bfType = stream->readBytes(2);
    if(bfType == "BM")
    {
        qint32 bfSize = stream->readInt32();
        stream->seek(pos);
        QByteArray imageData = stream->readBytes(bfSize);
        result = QImage::fromData(imageData, "BMP");
    }
//    else if(bfType == "GI")
//    {
//        qint16 type = stream->readInt16();
//        qint16 size = stream->readInt32();
//        stream->seek(pos);
//        QByteArray imageData = stream->readBytes(size);
//        result = QImage::fromData(imageData, "GIF");
//    }
    return result;

//    bfType= stream->readBytes(2);
//    /*qint32 bfSize = */stream->readInt32();
//    /*qint16 bfReserved1 = */stream->readInt16();
//    /*qint16 bfReserved2 = */stream->readInt16();
//    qint32 bfOffBits = stream->readInt32();

//    if(bfType == "BM" && bfOffBits > 0)
//    {
//        /*qint32 headerSize = */stream->readInt32();

//        qint32 width = stream->readInt32();
//        qint32 height = stream->readInt32();
//        /*qint16 planes = */stream->readInt16();
//        qint16 bitCount = stream->readInt16();
//        /*qint32 compression = */stream->readInt32();
//        qint32 sizeImage = stream->readInt32();
//        /*qint32 xPelsPerMeter = */stream->readInt32();
//        /*qint32 yPelsPerMeter = */stream->readInt32();
//        qint32 clrUsed = stream->readInt32();
//        /*qint32 clrImportant = */stream->readInt32();

//        if(bitCount == 24)
//        {
//            qint32 emptyBytes = (sizeImage - (width * height * 3)) / height;
//            QByteArray colorBytes = stream->readBytes(sizeImage);
//            result = QImage(width, height, QImage::Format_RGB888);
//            int colorIndex = 0;
//            for(int i = 0; i < height; i++)
//            {
//                for(int j = 0; j < width; j++)
//                {
//                    quint8 blue = colorBytes[colorIndex++];
//                    quint8 green = colorBytes[colorIndex++];
//                    quint8 red = colorBytes[colorIndex++];

//                    result.setPixel(j, height - i - 1, qRgb(red, green, blue));
//                }

//                if(emptyBytes >= 0)
//                    colorIndex += emptyBytes;
//            }
//        }
//        else if(bitCount == 8)
//        {
//            qint32 emptyBytes = (sizeImage - (width * height)) / height;
//            result = QImage(width, height, QImage::Format_Indexed8);

//            clrUsed = (1 << bitCount);
//            QByteArray colorBytes = stream->readBytes(clrUsed * 4);

//            int j = clrUsed - 1;
//            for(int i = colorBytes.count() - 1; i >= 0; i-=4, j--)
//            {
//                quint8 red = colorBytes[i-1];
//                quint8 green = colorBytes[i-2];
//                quint8 blue = colorBytes[i-3];
//                result.setColor(j, qRgb(red, green, blue));
//            }

//            QByteArray colorIndexes = stream->readBytes(sizeImage);
//            int colorIndex = 0;
//            for(int i = 0; i < height; i++)
//            {
//                for(int j = 0; j < width; j++, colorIndex++)
//                    result.setPixel(j, height - i - 1, quint8(colorIndexes[colorIndex]));

//                if(emptyBytes >= 0)
//                    colorIndex += emptyBytes;
//            }
//        }
//        else if(bitCount == 4)
//        {
//            qint32 emptyBytes = (sizeImage*2 - (width * height)) / height;
//            result = QImage(width, height, QImage::Format_Indexed8);

//            clrUsed = (1 << bitCount);
//            QByteArray colorBytes = stream->readBytes(clrUsed * 4);

//            int j = clrUsed - 1;
//            for(int i = colorBytes.count() - 1; i >= 0; i-=4, j--)
//            {
//                quint8 red = colorBytes[i-1];
//                quint8 green = colorBytes[i-2];
//                quint8 blue = colorBytes[i-3];
//                result.setColor(j, qRgb(red, green, blue));
//            }

//            QByteArray colorIndexes = stream->readBytes(sizeImage);
//            int colorIndex = 0;
//            for(int i = 0; i < height; i++)
//            {
//                for(int j = 0; j < width; j+=2, colorIndex++)
//                {
//                    quint8 index = colorIndexes[colorIndex];
//                    quint8 index1 = index >> 4;
//                    quint8 index2 = index & 0xf;

//                    result.setPixel(j, height - i - 1, index1);
//                    result.setPixel(j+1, height - i - 1, index2);
//                }
//                if(emptyBytes >= 0)
//                    colorIndex += emptyBytes;
//            }
//        }
//        else if(bitCount == 1)
//        {
//            qint64 pos = stream->pos();
//            qint64 size = stream->device()->size();
//            sizeImage = qMin(qint32(size - pos - 8), sizeImage);

//            qint32 emptyItems = (sizeImage*8 - (width * height)) / height;
//            QByteArray colorBytes = stream->readBytes(8);
//            QByteArray maskBytes = stream->readBytes(sizeImage);
//            QBitArray maskBits(maskBytes.count() * 8);
//            for(int i = 0; i < maskBytes.count(); i++)
//                for(int b = 0; b < 8; b++)
//                    maskBits.setBit(i*8+b, maskBytes.at(i) & (1<<(7-b)));

//            result = QImage(width, height, QImage::Format_Mono);

//            if(colorBytes.count() == 8)
//            {
//                result.setColor(0, qRgb(colorBytes[0],colorBytes[1],colorBytes[2]));
//                result.setColor(1, qRgb(colorBytes[4],colorBytes[5],colorBytes[6]));
//            }
//            else
//            {
//                result.setColor(0, qRgb(0,0,0));
//                result.setColor(1, qRgb(255,255,255));
//            }

//            int colorIndex = 0;
//            for(int i = 0; i < height; i++)
//            {
//                for(int j = 0; j < width; j++, colorIndex++)
//                    result.setPixel(j, height - i - 1, maskBits.at(colorIndex) ? 1 : 0);

//                if(emptyItems >= 0)
//                    colorIndex += emptyItems;
//            }
//        }
//    }
//    return result;
}

QImage GraphicsHelper::imageFromBits(QByteArray& colorIndexes)
{
    QImage result;

    qint32 width = 32;
    qint32 height = 32;
    qint16 bitCount = 4;
    qint32 clrUsed = 16;

    if(bitCount == 4)
    {
        result = QImage(width, height, QImage::Format_Indexed8);

        quint8 colorData[] =
        {
            0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x80,0x00,
            0x80,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x80,0x00,0x00,0xC0,0xC0,0xC0,0x00,
            0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
            0xFF,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00
        };
        QByteArray colorBytes((char*)colorData, 64);

        int j = clrUsed - 1;
        for(int i = colorBytes.count() - 1; i >= 0; i-=4, j--)
        {
            quint8 red = colorBytes[i-1];
            quint8 green = colorBytes[i-2];
            quint8 blue = colorBytes[i-3];
            result.setColor(j, qRgb(red, green, blue));
        }

        int colorIndex = 0;
        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j+=2, colorIndex++)
            {
                quint8 index = colorIndexes[colorIndex];
                quint8 index1 = index >> 4;
                quint8 index2 = index & 0xf;

                result.setPixel(j, height - i - 1, index1);
                result.setPixel(j+1, height - i - 1, index2);
            }
        }
    }
    return result;
}

QPixmap GraphicsHelper::loadMaskedPixmap(const QString& fileName)
{
    if(QFileInfo(fileName).suffix().contains("dbm", Qt::CaseInsensitive))
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly))
        {
            St2000DataStream stream(&file);
            QImage image = imageFromData(&stream);
            if(!image.isNull())
            {
                QImage maskImage = imageFromData(&stream);
                if(!maskImage.isNull())
                {
                    QPixmap pixmap = QPixmap::fromImage(image);
                    pixmap.setMask(QBitmap::fromImage(maskImage));
                    return pixmap;
                }
            }
        }
    }

    return QPixmap(fileName);

    //QPixmap pixmap(fileName);
    //if(!pixmap.mask().isNull())
    //    return pixmap;

//    QFile file(fileName);
//    if(file.open(QIODevice::ReadOnly))
//    {
//        St2000DataStream stream(&file);
//        QImage image = imageFromData(&stream);
//        if(!image.isNull())
//        {
//            QImage maskImage = imageFromData(&stream);
//            if(!maskImage.isNull())
//            {
//                pixmap = QPixmap::fromImage(image);
//                pixmap.setMask(QBitmap::fromImage(maskImage));
//            }
//        }
//    }
//    return QPixmap(fileName);
}


}
