#include "Sc2000DataStream.h"
#include "SpaceToolResult.h"
#include "Tool2d.h"
#include "GraphicsHelper.h"
#include "SpaceChunk.h"
#include "SpaceCollectionResult.h"
#include "Space.h"
#include "IconManager.h"
#include "Project.h"
#include "Library.h"
#include "Class.h"
#include "PathManager.h"
#include "Log.h"
#include "FontHelper.h"

#include <QApplication>
#include <QBitArray>
#include <QFileInfo>
#include <QFontInfo>
#include <QFontMetrics>
#include <QScreen>
#include <QTextCursor>

namespace StData {

SpaceToolResult::SpaceToolResult(Space* space2d, St2000DataStream* stream) :
    SpaceChunkResult(space2d, stream),
    m_handle(0)
{}

void SpaceToolResult::load(SpaceChunk* /*parentChunk*/)
{
    m_stream->readInt16();
    m_handle = m_stream->readInt16();

    loadTool();
}

/*---------------------------------------------------------------------------------------------------*/
PenResult::PenResult(Space* space2d, St2000DataStream* stream) :
    SpaceToolResult(space2d, stream)
{}

//Qt::PenStyle PenResult::toQPenStyle(qint16 penStyle)
//{
//    switch(static_cast<PenStyle>(penStyle))
//    {
//        case PenStyle::Solid: return Qt::SolidLine;
//        case PenStyle::Dash: return Qt::DashLine;
//        case PenStyle::Dot: return Qt::DotLine;
//        case PenStyle::DashDot: return Qt::DashDotLine;
//        case PenStyle::DashDotDot: return Qt::DashDotLine;
//        default: return Qt::NoPen;
//    }
//}

void PenResult::loadTool()
{
    m_rgba = GraphicsHelper::rgbaFromBytes(m_stream->readBytes(4));
    m_style = m_stream->readInt16();
    m_width = m_stream->readInt16();
    m_rop = m_stream->readInt16();
}

/*---------------------------------------------------------------------------------------------------*/
BrushResult::BrushResult(Space* space2d, St2000DataStream* stream) :
    SpaceToolResult(space2d, stream)
{}

void BrushResult::loadTool()
{
    m_rgba = GraphicsHelper::rgbaFromBytes(m_stream->readBytes(4));
    m_style = m_stream->readInt16();
    m_hatch = m_stream->readInt16();
    m_rop = m_stream->readInt16();
    m_textureHandle = m_stream->readInt16();
}

/*---------------------------------------------------------------------------------------------------*/
TextureResult::TextureResult(Space* space2d, St2000DataStream* stream) :
    SpaceToolResult(space2d, stream)
{}

void TextureResult::loadTool()
{
    m_pixmap = loadTexture();
}

QPixmap TextureResult::loadTexture()
{
    QPixmap result;
    QImage img = GraphicsHelper::imageFromData(stream());
    result = QPixmap::fromImage(img);

    return result;
}

/*---------------------------------------------------------------------------------------------------*/
TextureRefResult::TextureRefResult(Space* space2d, St2000DataStream* stream) :
    TextureResult(space2d, stream)
{}

void TextureRefResult::loadTool()
{
    m_reference = stream()->readString();

    QString path = m_reference;
    if(QFileInfo(m_reference).isRelative())
    {
        QString nativeRef = PathManager::fromWinOsSeparators(m_reference);
        path = space()->spaceDir().absoluteFilePath(nativeRef);
        path = QDir::cleanPath(path);
    }
    m_pixmap = QPixmap(path);
}

/*---------------------------------------------------------------------------------------------------*/
MaskedTextureResult::MaskedTextureResult(Space* space2d, St2000DataStream* stream) :
    TextureResult(space2d, stream)
{}

void MaskedTextureResult::loadTool()
{
    TextureResult::loadTool();
    m_pixmap.setMask(loadMask());
}

QBitmap MaskedTextureResult::loadMask()
{
    QBitmap result;

    QImage img = GraphicsHelper::imageFromData(stream());
    result = QBitmap::fromImage(img);

    return result;
}

/*---------------------------------------------------------------------------------------------------*/
MaskedTextureRefResult::MaskedTextureRefResult(Space* space2d, St2000DataStream* stream) :
    MaskedTextureResult(space2d, stream)
{}

void MaskedTextureRefResult::loadTool()
{
    m_reference = stream()->readString();

    if(IconManager::instance().containsIconSet(m_reference))
        m_pixmap = IconManager::instance().iconSetByName(m_reference);

    else
    {
        QString path = m_reference;
        if(QFileInfo(m_reference).isRelative())
        {
            QString nativeRef = PathManager::fromWinOsSeparators(m_reference);
            path = space()->spaceDir().absoluteFilePath(nativeRef);
            path = QFileInfo(path).canonicalFilePath();
        }
        m_pixmap = GraphicsHelper::loadMaskedPixmap(path);
    }
}

/*---------------------------------------------------------------------------------------------------*/
FontResult::FontResult(Space* space2d, St2000DataStream* stream) :
    SpaceToolResult(space2d, stream)
{}

void FontResult::loadTool()
{
    qint16 height = stream()->readInt16();
    /*qint16 width = */stream()->readInt16();
    /*qint16 escapement = */stream()->readInt16();
    /*qint16 orientation = */stream()->readInt16();
    qint16 weight = stream()->readInt16();

    quint8 italic = stream()->readUint8();
    quint8 underline = stream()->readUint8();
    quint8 strikeOut = stream()->readUint8();
    /*quint8 charSet = */stream()->readUint8();
    /*quint8 outPrecision = */stream()->readUint8();
    /*quint8 clipPrecision = */stream()->readUint8();
    /*quint8 quality = */stream()->readUint8();
    /* quint8 pitchAndFamily = */stream()->readUint8();

    QString faceName = stream()->readBytes(32);
    faceName = faceName.replace(" CYR", "", Qt::CaseInsensitive).trimmed();

    qint32 fontSize = 0;
    if(stream()->fileVersion() >= 0x203)
    {
        fontSize = stream()->readInt32();
        //m_isSizeInPixels = fontSize <= 0;
        /*qint32 fontExtraStyle = */stream()->readInt32();
    }
    m_isSizeInPixels = fontSize <= 0;

    m_font.setWeight(getFontWeight(weight));
    m_font.setFamily(faceName);
    m_font.setItalic(italic != 0);
    m_font.setUnderline(underline != 0);
    m_font.setStrikeOut(strikeOut != 0);

    if(m_isSizeInPixels)
    {
        m_font.setPixelSize(-height);
    }
    else
    {
        //ВОзможно стоит использовать FontHelper::convert
#ifdef Q_OS_ANDROID
        double pointSize = FontHelper::convertPixelToPointSize(fontSize/2);
        m_font.setPointSizeF(pointSize - 0.2);
        //m_font.setPointSizeF((fontSize/2)*0.5);
#else
        m_font.setPointSizeF((fontSize/2) - 0.2);

#endif
    }
}

QFont::Weight FontResult::getFontWeight(qint16 weightValue)
{
    QFont::Weight weight = QFont::Normal;

    if(weightValue == 700)
        weight = QFont::Bold;

    else if(weightValue == 600)
        weight = QFont::DemiBold;

    else if(weightValue == 300)
        weight = QFont::Light;

    return weight;
}

/*---------------------------------------------------------------------------------------------------*/
StringResult::StringResult(Space* space2d, St2000DataStream* stream) :
    SpaceToolResult(space2d, stream)
{}

void StringResult::loadTool()
{
    m_string = stream()->readString();
}

/*---------------------------------------------------------------------------------------------------*/
RasterTextResult::RasterTextResult(Space* space, St2000DataStream* stream) :
    SpaceToolResult(space, stream)
{}

RasterTextResult::~RasterTextResult()
{
    qDeleteAll(m_textResults);
}

void RasterTextResult::loadTool()
{
    SpaceChunk chunk(space(), stream());
    chunk.read();
    {
        if(chunk.code() == SpaceChunk::RichTextElementCollection)
        {
            RasterTextElementCollectionResult result(space(), stream());
            result.load();

            m_textResults.append(result.textInfoResults());
        }
    }
    chunk.toEndChunk();
}

TextResult::TextResult(Space* space, St2000DataStream* stream) :
    SpaceToolResult(space, stream)
{}

void TextResult::loadTool()
{
    QByteArray fgBytes = stream()->readBytes(4);
    QByteArray bgBytes = stream()->readBytes(4);
    m_fgColor = GraphicsHelper::colorFromBytes(fgBytes);
    m_bgColor = GraphicsHelper::colorFromBytes(bgBytes);

    m_fontHandle = stream()->readInt16();
    m_stringHandle = stream()->readInt16();
}

}
