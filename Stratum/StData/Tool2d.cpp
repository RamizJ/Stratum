#include "Tool2d.h"
#include "Log.h"
#include "StDataGlobal.h"
#include "ToolVisitorBase.h"

#include <QTextDocument>

namespace StData {

Tool::Tool(int handle) :
    QObject(),
    HandleItem(handle),
    m_refCount(0)
{}

Tool::Tool(const Tool& other) :
    QObject(),
    HandleItem(other.handle())
{}

void Tool::incRef()
{
    m_refCount++;
}

void Tool::decRef()
{
    if(m_refCount > 0)
        m_refCount--;
}

/*---------------------------------------------------------------------------------------------------*/
PenTool::PenTool(int handle) :
    Tool(handle),
    m_style(0),
    m_rop(0),
    m_width(0)
{
    m_pen.setJoinStyle(Qt::RoundJoin);
    m_pen.setCapStyle(Qt::RoundCap);
}

PenTool::PenTool(const PenTool& other) :
    Tool(other),
    m_style(other.style()),
    m_rop(other.rop()),
    m_rgba(other.rgba()),
    m_width(other.width()),
    m_pen(other.pen())
{
}

void PenTool::setRop(const qint16& rop)
{
    m_rop = rop;
    emit changed();
}

void PenTool::setRgba(int rgba)
{
    m_rgba = rgba;
    m_pen.setColor(intToColor(m_rgba));
    emit changed();

}

void PenTool::setWidth(double width)
{
    m_width = width;
    m_pen.setWidthF(fabs(m_width));
    emit changed();

}

Qt::PenStyle PenTool::toQPenStyle(qint16 penStyle)
{
    switch(static_cast<PenStyle>(penStyle))
    {
        case PenStyle::Solid: return Qt::SolidLine;
        case PenStyle::Dash: return Qt::DashLine;
        case PenStyle::Dot: return Qt::DotLine;
        case PenStyle::DashDot: return Qt::DashDotLine;
        case PenStyle::DashDotDot: return Qt::DashDotDotLine;
        default: return Qt::NoPen;
    }
}

PenTool::PenStyle PenTool::toStPenStyle(Qt::PenStyle penStyle)
{
    switch(penStyle)
    {
        case Qt::SolidLine: return PenStyle::Solid;
        case Qt::DashLine: return PenStyle::Dash;
        case Qt::DotLine: return PenStyle::Dot;
        case Qt::DashDotLine: return PenStyle::DashDot;
        case Qt::DashDotDotLine: return PenStyle::DashDotDot;
        default: return PenStyle::Null;
    }
}

void PenTool::setStyle(const qint16& style)
{
    m_style = style;
    Qt::PenStyle penStyle = toQPenStyle(m_style);
    m_pen.setStyle(penStyle);
    emit changed();
}

void PenTool::setData(int rgba, double width, qint16 style, qint16 rop)
{
    m_rgba = rgba;
    m_width = width;
    m_style = style;
    m_rop = rop;

    m_pen.setWidthF(fabs(m_width));
    m_pen.setColor(intToColor(m_rgba));
    m_pen.setStyle(toQPenStyle(m_style));

    emit changed();
}

void PenTool::accept(ToolVisitorBase* visitor)
{
    visitor->visit(this);
}

bool PenTool::operator ==(const PenTool& other) const
{
    return m_rgba == other.rgba() &&
            m_width == other.width() &&
            m_style == other.style() &&
            m_rop == other.rop();
}

bool PenTool::operator !=(const PenTool& other) const
{
    return !(*this == other);
}

/*---------------------------------------------------------------------------------------------------*/
BrushTool::BrushTool(int handle) :
    Tool(handle),
    m_texture(nullptr),
    m_textureHandle(0),
    m_rop(0),
    m_style(0),
    m_hatch(0),
    m_rgba(0)
{}

BrushTool::BrushTool(const BrushTool& other) :
    Tool(other),
    m_texture(other.texture()),
    m_rop(other.rop()),
    m_style(other.style()),
    m_hatch(other.hatch()),
    m_rgba(other.rgba()),
    m_brush(other.brush())
{}

//QBrush BrushInfo::brush() const
//{
//    return m_brush;
//}

//void BrushInfo::setBrush(const QBrush& brush)
//{
//    if(m_brush != brush)
//    {
//        m_brush = brush;
//        m_rgba = m_brush.color().rgb();
//        m_hatch = toStHatchPattern(m_brush.style());
//        m_style = toStBrushStyle(m_brush.style());

//        emit changed();
//    }
//}

void BrushTool::setRop(const qint16& rop)
{
    if(m_rop != rop)
    {
        m_rop = rop;
        emit changed();
    }
}

void BrushTool::setStyle(qint16 style)
{
    m_style = style;
    m_brush.setStyle(toQBrushStyle(m_style, m_hatch));
    emit changed();
}

void BrushTool::setHatch(qint16 hatch)
{
    m_hatch = hatch;
    m_brush.setStyle(toQBrushStyle(m_style, m_hatch));
    emit changed();
}

void BrushTool::setRgba(int rgba)
{
    if(m_rgba != rgba)
    {
        m_rgba = rgba;
        m_brush.setColor(intToColor(m_rgba));
        emit changed();
    }
}

void BrushTool::setData(int rgba, qint16 style, qint16 hatch, qint16 rop, TextureTool* texture)
{
    m_rgba = rgba;
    m_style = style;
    m_hatch = hatch;
    m_rop = rop;
    m_texture = texture;

    m_brush.setColor(intToColor(m_rgba));
    m_brush.setStyle(toQBrushStyle(m_style, m_hatch));
    if(m_texture)
        m_brush.setTexture(m_texture->pixmap());

    emit changed();
}

void BrushTool::accept(ToolVisitorBase* visitor)
{
    visitor->visit(this);
}

bool BrushTool::operator ==(const BrushTool& other) const
{
    return m_rgba == other.m_rgba &&
            m_style == other.m_style &&
            m_rop == other.m_rop &&
            m_hatch == other.m_hatch &&
            m_texture == other.m_texture;
}

bool BrushTool::operator !=(const BrushTool& other) const
{
    return !(*this == other);
}

Qt::BrushStyle BrushTool::toQBrushStyle(qint16 brushStyle, qint16 hatchPattern)
{
    switch(static_cast<BrushStyle>(brushStyle))
    {
        case BrushStyle::Solid: return Qt::SolidPattern;
        case BrushStyle::Hatched:
            switch(static_cast<HatchPattern>(hatchPattern))
            {
                case HatchPattern::Horizontal: return Qt::HorPattern;
                case HatchPattern::Vertical: return Qt::VerPattern;
                case HatchPattern::Diagonal: return Qt::FDiagPattern;
                case HatchPattern::BackDiagonal: return Qt::BDiagPattern;
                case HatchPattern::Cross: return Qt::CrossPattern;
                case HatchPattern::CrossDiagonal: return Qt::DiagCrossPattern;
            }
            break;
        case BrushStyle::DibPattern: return Qt::TexturePattern;
        default: return Qt::NoBrush;
    }
    return Qt::NoBrush;
}

BrushTool::BrushStyle BrushTool::toStBrushStyle(Qt::BrushStyle style)
{
    switch(style)
    {
        case Qt::SolidPattern: return BrushStyle::Solid;
        case Qt::TexturePattern: return BrushStyle::DibPattern;
        default: return Null;
    }
}

qint16 BrushTool::toStHatchPattern(Qt::BrushStyle style)
{
    switch(style)
    {
        case Qt::HorPattern: return HatchPattern::Horizontal;
        case Qt::VerPattern: return HatchPattern::Vertical;
        case Qt::FDiagPattern: return HatchPattern::Diagonal;
        case Qt::BDiagPattern: return HatchPattern::BackDiagonal;
        case Qt::CrossPattern: return HatchPattern::Cross;
        case Qt::DiagCrossPattern: return HatchPattern::CrossDiagonal;
        default: return 0;
    }
    //    return 0;
}

int BrushTool::textureHandle() const
{
    return m_textureHandle;
}

void BrushTool::setTextureHandle(int textureHandle)
{
    m_textureHandle = textureHandle;
}

void BrushTool::setTexture(TextureTool* texture)
{
    if(texture != m_texture)
    {
        if(m_texture)
            m_texture->decRef();

        m_texture = texture;
        m_textureHandle = 0;

        if(m_texture)
        {
            m_textureHandle = m_texture->handle();
            m_texture->incRef();
            m_brush.setTexture(m_texture->pixmap());
        }

        emit changed();
    }
}

/*---------------------------------------------------------------------------------------------------*/
TextureTool::TextureTool(int handle) :
    Tool(handle)
{}

TextureTool::TextureTool(const TextureTool& other) :
    Tool(other),
    m_pixmap(other.pixmap())
{}

void TextureTool::setPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
    emit changed();
}

void TextureTool::accept(ToolVisitorBase* visitor)
{
    visitor->visit(this);
}

bool TextureTool::operator ==(const TextureTool& other) const
{
    return m_pixmap.toImage() == other.m_pixmap.toImage();
}

bool TextureTool::operator !=(const TextureTool& other) const
{
    return m_pixmap.toImage() != other.m_pixmap.toImage();
}

/*---------------------------------------------------------------------------------------------------*/
TextureRefTool::TextureRefTool(int handle) :
    TextureTool(handle)
{}

TextureRefTool::TextureRefTool(const TextureRefTool& other) :
    TextureTool(other),
    m_texturePath(other.reference())
{}

void TextureRefTool::setPixmapPath(const QString& pixmapPath)
{
    if(m_texturePath != pixmapPath)
    {
        m_texturePath = pixmapPath;
        emit changed();
    }
}


/*---------------------------------------------------------------------------------------------------*/
FontTool::FontTool(qint16 handle) :
    Tool(handle)
{}

FontTool::FontTool(const FontTool& other) :
    Tool(other),
    m_font(other.font()),
    m_sizeInPixels(other.sizeInPixels())
{}

void FontTool::setFont(const QFont& font)
{
    m_font = font;
    emit changed();
}

void FontTool::setFamily(const QString& fontName)
{
    m_font.setFamily(fontName);
    emit changed();
}

void FontTool::setSize(int size)
{
    if(m_sizeInPixels)
        m_font.setPixelSize(size);
    else
        m_font.setPointSize(size);

    emit changed();
}

int FontTool::style() const
{
    int style = m_font.italic() ? 1 : 0;
    style |= m_font.underline() ? 2 : 0;
    style |= m_font.strikeOut() ? 4 : 0;
    style |= m_font.bold() ? 8 : 0;
    return style;
}

void FontTool::setStyle(int style)
{
    m_font.setItalic(style & 1);
    m_font.setUnderline(style & 2);
    m_font.setStrikeOut(style & 4);
    m_font.setBold(style & 8);
    emit changed();
}

void FontTool::accept(ToolVisitorBase* visitor)
{
    visitor->visit(this);
}

bool FontTool::operator ==(const FontTool& other) const
{
    return m_font == other.m_font;
}

bool FontTool::operator !=(const FontTool& other) const
{
    return m_font != other.m_font;
}


/*---------------------------------------------------------------------------------------------------*/
StringTool::StringTool(qint16 handle) :
    Tool(handle)
{}

StringTool::StringTool(const StringTool& other) :
    Tool(other),
    m_string(other.string())
{}

void StringTool::setString(const QString& newString)
{
    if(m_string != newString)
    {
        QString oldString = m_string;

        emit changing(oldString, newString);
        m_string = newString;
        emit changed(oldString, newString);
    }
}

void StringTool::accept(ToolVisitorBase* visitor)
{
    visitor->visit(this);
}

bool StringTool::operator ==(const StringTool& other) const
{
    return m_string == other.m_string;
}

bool StringTool::operator !=(const StringTool& other) const
{
    return m_string != other.m_string;
}

/*---------------------------------------------------------------------------------------------------*/
RasterTextTool::RasterTextTool(int handle) :
    Tool(handle)
{}

RasterTextTool::RasterTextTool(const RasterTextTool& rasterText) :
    Tool(rasterText)
{
    for(TextFragment* textFragment : rasterText.textFragments())
    {
        TextFragment* textFragmentCopy = new TextFragment(*textFragment);
        m_textFragments << textFragmentCopy;

        connectTextFragment(textFragmentCopy);
    }
}

RasterTextTool::~RasterTextTool()
{
    qDeleteAll(m_textFragments);
}

TextFragment* RasterTextTool::textFragmentAt(int index) const
{
    return m_textFragments.at(index);
}

//void RasterTextTool::setTextTools(const QList<TextFragmentTool*> textTools)
//{
//    m_textFragments = textTools;
//    emit changed();
//}

void RasterTextTool::append(TextFragment* textFragment)
{
    if(textFragment)
    {
        m_textFragments << textFragment;

        connectTextFragment(textFragment);
        emit textFragmentAdded();
    }
}

void RasterTextTool::insertFragment(int index, FontTool* fontTool, StringTool* stringTool,
                                    const QColor& bgColor, const QColor& fgColor)
{
    if(index >= 0 && index <= m_textFragments.count())
    {
        TextFragment* textFragment = new TextFragment(fontTool, stringTool, bgColor, fgColor);
        m_textFragments.insert(index, textFragment);

        connectTextFragment(textFragment);
        emit textFragmentInserted(index);
    }
}

//void RasterTextTool::insertAt(int index, TextFragment* txtTool)
//{
//    if(txtTool && index >= 0 && index <= m_textFragments.count())
//    {
//        m_textFragments.insert(index, txtTool);
//        txtTool->incRef();

//        connectTextFragment(txtTool);
//        emit textFragmentInserted(index);
//    }
//}

void RasterTextTool::removeAt(int index)
{
    if(index >= 0 && index < m_textFragments.count())
    {
        TextFragment* textFrament = m_textFragments.takeAt(index);
        //        textFramentTool->decRef();
        //        disconnect(textFramentTool);
        emit textFragmentRemoved(index, textFrament );
        delete textFrament ;
    }
}

void RasterTextTool::clear()
{
    emit allTextFragmentsRemoved();

    qDeleteAll(m_textFragments);
    m_textFragments.clear();
}

int RasterTextTool::getTextPos(int index)
{
    int pos = 0;
    for(int i = 0; i < index; i++)
    {
        TextFragment* textFragmentTool = m_textFragments.at(i);
        pos += textFragmentTool->textLength();
    }
    return pos;
}

int RasterTextTool::getTextPos(TextFragment* textFragmentTool)
{
    int index = m_textFragments.indexOf(textFragmentTool);
    return getTextPos(index);
}

int RasterTextTool::getTextLength(int index)
{
    TextFragment* textFragmentTool = m_textFragments.at(index);
    return textFragmentTool->textLength();
}

int RasterTextTool::length()
{
    int len = 0;
    for(TextFragment* textFragment : m_textFragments)
        len += textFragment->textLength();

    return len;
}

void RasterTextTool::accept(ToolVisitorBase* visitor)
{
    visitor->visit(this);
}

void RasterTextTool::itemFgColorChanged()
{
    TextFragment* tool = qobject_cast<TextFragment*>(sender());
    emit textFragmentFgColorChanged(tool);
}

void RasterTextTool::itemBgColorChanged()
{
    TextFragment* tool = qobject_cast<TextFragment*>(sender());
    emit textFragmentBgColorChanged(tool);
}

void RasterTextTool::itemFontChanged()
{
    TextFragment* tool = qobject_cast<TextFragment*>(sender());
    emit textFragmentFontChanged(tool);
}

void RasterTextTool::itemStringChanged(const QString& oldStr, const QString& newStr)
{
    TextFragment* tool = qobject_cast<TextFragment*>(sender());
    emit textFragmentStringChanged(tool, oldStr, newStr);
}

void RasterTextTool::itemStringChanging(const QString& oldStr, const QString& newStr)
{
    TextFragment* tool = qobject_cast<TextFragment*>(sender());
    emit textFragmentStringChanging(tool, oldStr, newStr);
}

void RasterTextTool::connectTextFragment(TextFragment* textFragment)
{
    connect(textFragment, &TextFragment::bgColorChanged, this, &RasterTextTool::itemBgColorChanged);
    connect(textFragment, &TextFragment::fgColorChanged, this, &RasterTextTool::itemFgColorChanged);
    connect(textFragment, &TextFragment::fontChanged, this, &RasterTextTool::itemFontChanged);
    connect(textFragment, &TextFragment::stringChanged, this, &RasterTextTool::itemStringChanged);
    connect(textFragment, &TextFragment::stringChanging, this, &RasterTextTool::itemStringChanging);
}

/*---------------------------------------------------------------------------------------------------*/
TextFragment::TextFragment() :
    QObject(),
    m_fontTool(nullptr),
    m_stringTool(nullptr)
{}

TextFragment::TextFragment(FontTool* fontTool, StringTool* stringTool, const QColor& bgColor, const QColor& fgColor) :
    m_fontTool(nullptr),
    m_stringTool(nullptr)
{
    setData(fontTool, stringTool, bgColor, fgColor);
}

TextFragment::TextFragment(const TextFragment& other) :
    QObject(),
    m_bgColor(other.bgColor()),
    m_fgColor(other.fgColor()),
    m_fontTool(nullptr),
    m_stringTool(nullptr),
    m_fontHandle(0),
    m_stringHandle(0)
{
    if(other.fontTool())
        setFont(new FontTool(*other.fontTool()));

    if(other.stringTool())
        setString(new StringTool(*other.stringTool()));
}

TextFragment::~TextFragment()
{
    if(m_fontTool)
        m_fontTool->decRef();

    if(m_stringTool)
        m_stringTool->decRef();
}

void TextFragment::setFont(FontTool* fontTool)
{
    if(fontTool != m_fontTool)
    {
        if(m_fontTool)
        {
            m_fontTool->decRef();
            disconnect(m_fontTool);
        }

        m_fontTool = fontTool;
        m_fontHandle = 0;

        if(m_fontTool)
        {
            m_fontHandle = m_fontTool->handle();
            m_fontTool->incRef();
            connect(m_fontTool, &FontTool::changed, this, &TextFragment::fontChanged);
        }
        emit fontChanged();
    }
}

void TextFragment::setString(StringTool* stringTool)
{    
    if(stringTool != m_stringTool)
    {
        QString oldString;
        if(m_stringTool)
        {
            m_stringTool->decRef();
            oldString = m_stringTool->string();
            disconnect(m_stringTool);
        }

        m_stringTool = stringTool;
        m_stringHandle = 0;

        QString newString;
        if(m_stringTool)
        {
            m_stringHandle = m_stringTool->handle();
            connect(m_stringTool, &StringTool::changing, this, &TextFragment::stringChanging);
            m_stringTool->incRef();
            newString = m_stringTool->string();            
            connect(m_stringTool, &StringTool::changed, this, &TextFragment::stringChanged);
        }
        emit stringChanged(oldString, newString);
    }
}

void TextFragment::setBgColor(const QColor& bgColor)
{
    if(m_bgColor != bgColor)
    {
        m_bgColor = bgColor;
        emit bgColorChanged();
    }
}

void TextFragment::setFgColor(const QColor& fgColor)
{
    if(m_fgColor != fgColor)
    {
        m_fgColor = fgColor;
        emit fgColorChanged();
    }
}

void TextFragment::setData(FontTool* fontTool, StringTool* stringTool, const QColor& bgColor, const QColor& fgColor)
{
    setBgColor(bgColor);
    setFgColor(fgColor);
    setFont(fontTool);
    setString(stringTool);
}

QString TextFragment::text() const
{
    return m_stringTool ? m_stringTool->string() : "";
}

int TextFragment::textLength() const
{
    return m_stringTool ? m_stringTool->string().length() : 0;
}

bool TextFragment::operator ==(const TextFragment& other) const
{
    return m_stringTool == other.m_stringTool &&
            m_fontTool == other.m_fontTool &&
            m_fgColor == other.m_fgColor &&
            m_bgColor == other.m_bgColor;
}

bool TextFragment::operator !=(const TextFragment& other) const
{
    return !(*this == other);
}

int TextFragment::stringHandle() const
{
    return m_stringHandle;
}

void TextFragment::setStringHandle(int stringHandle)
{
    m_stringHandle = stringHandle;
}

int TextFragment::fontHandle() const
{
    return m_fontHandle;
}

void TextFragment::setFontHandle(int fontHandle)
{
    m_fontHandle = fontHandle;
}

}
