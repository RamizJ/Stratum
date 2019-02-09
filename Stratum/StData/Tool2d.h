#ifndef TOOL2D_H
#define TOOL2D_H

#include "HandleItem.h"
#include "StDataTypes.h"

#include <QBitmap>
#include <QPen>
#include <QTextDocument>

namespace StData {

class STDATASHARED_EXPORT Tool : public QObject, public HandleItem
{
    Q_OBJECT
public:
    enum ToolType {Undefined = 0, Pen, Brush, Texture, MaskedTexture, Font, String, Text, Space3d, Material3d};

    Tool(int handle = 0);
    Tool(const Tool& other);

    void incRef();
    void decRef();

    virtual int toolType() const = 0;
    virtual void accept(ToolVisitorBase* visitor) = 0;

    int refCount() const {return m_refCount; }

private:
    int m_refCount;
};

class STDATASHARED_EXPORT PenTool : public Tool
{
    Q_OBJECT
public:
    enum PenStyle {Solid, Dash, Dot, DashDot, DashDotDot, Null};
    enum EndCapStyle {EndCapRound, EndCapSquare, EndCapFlat};
    enum JoinStyle {JoinRound, JoinBevel, JoinMitter};
    enum RopStyle
    {
        RS_Black = 1, RS_NotMergePen = 2, RS_MaskNotPen = 3, RS_NotCopyPen = 4, RS_MaskPenNot = 5,
        RS_Not = 6, RS_XorPen = 7, RS_NotMaskPen = 8, RS_MaskPen = 9, RS_NotXorPen = 10,
        RS_Nop = 11, RS_MergeNotPen = 12, RS_CopyPen = 13, RS_MergePenNot = 14, RS_MergePen = 15, RS_White = 16
    };

public:
    explicit PenTool(int handle = 0);
    PenTool(const PenTool& penInfo);

    qint16 rop() const { return m_rop; }
    void setRop(const qint16& rop);

    int rgba() const { return m_rgba; }
    void setRgba(int rgba);

    double width() const { return m_width; }
    void setWidth(double width);

    qint16 style() const { return m_style; }
    void setStyle(const qint16& style);

    void setData(int rgba, double width, qint16 style, qint16 rop);

    QPen pen() const { return m_pen; }

    int toolType() const {return Tool::Pen; }
    void accept(ToolVisitorBase* visitor);

    bool operator == (const PenTool& other) const;
    bool operator != (const PenTool& other) const;

signals:
    void changed();

public:
    static Qt::PenStyle toQPenStyle(qint16 penStyle);
    static PenStyle toStPenStyle(Qt::PenStyle penStyle);

private:
    qint16 m_style;
    qint16 m_rop;
    int m_rgba;
    double m_width;
    QPen m_pen;
};

/*---------------------------------------------------------------------------------------------------*/
class TextureTool;
class STDATASHARED_EXPORT BrushTool: public Tool
{
    Q_OBJECT
public:
    enum BrushStyle {Solid, Null, Hatched, Pattern, Indexed, DibPattern};
    enum HatchPattern {Horizontal, Vertical, Diagonal, BackDiagonal, Cross, CrossDiagonal};

public:
    explicit BrushTool(int handle = 0);
    BrushTool(const BrushTool& other);

    qint16 rop() const { return m_rop; }
    void setRop(const qint16& rop);

    qint16 style() const {return m_style;}
    void setStyle(qint16 style);

    qint16 hatch() const {return m_hatch;}
    void setHatch(qint16 hatch);

    int rgba() const { return m_rgba; }
    void setRgba(int rgba);

    TextureTool* texture() const { return m_texture; }
    void setTexture(TextureTool* texture);
    int textureHandle() const;
    void setTextureHandle(int textureHandle);

    void setData(int rgba, qint16 style, qint16 hatch, qint16 rop, TextureTool* texture);

    QBrush brush() const { return m_brush; }

    int toolType() const {return Tool::Brush; }
    void accept(ToolVisitorBase* visitor);

    bool operator == (const BrushTool& other) const;
    bool operator != (const BrushTool& other) const;

signals:
    void changed();

public:
    static Qt::BrushStyle toQBrushStyle(qint16 brushStyle, qint16 hatchPattern);
    static BrushStyle toStBrushStyle(Qt::BrushStyle style);
    static qint16 toStHatchPattern(Qt::BrushStyle style);

private:
    TextureTool* m_texture;
    int m_textureHandle;

    qint16 m_rop;
    qint16 m_style;
    qint16 m_hatch;
    int m_rgba;

    QBrush m_brush;
};

/*---------------------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT TextureTool : public Tool
{
    Q_OBJECT
public:
    explicit TextureTool(int handle = 0);
    TextureTool(const TextureTool& other);

    virtual ~TextureTool(){}

    QPixmap pixmap() const {return m_pixmap;}
    void setPixmap(const QPixmap& pixmap);

    virtual int toolType() const {return Tool::Texture; }
    void accept(ToolVisitorBase* visitor);

    bool operator == (const TextureTool& other) const;
    bool operator != (const TextureTool& other) const;

signals:
    void changed();

private:
    QPixmap m_pixmap;
};

class STDATASHARED_EXPORT TextureRefTool : public TextureTool
{
public:
    explicit TextureRefTool(int handle = 0);
    TextureRefTool(const TextureRefTool& other);

    QString reference() const { return m_texturePath; }
    void setPixmapPath(const QString& reference);

private:
    QString m_texturePath;
};

/*---------------------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT MaskedTextureTool : public TextureTool
{
public:
    explicit MaskedTextureTool(int handle = 0) :
        TextureTool(handle)
    {}

    MaskedTextureTool(const MaskedTextureTool& pixmapInfo) :
        TextureTool(pixmapInfo)
    {}

    int toolType() const {return Tool::MaskedTexture; }
};

/*---------------------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT FontTool : public Tool
{
    Q_OBJECT
public:
    explicit FontTool(qint16 handle = 0);
    FontTool(const FontTool& other);

    QFont font() const {return m_font;}
    void setFont(const QFont& font);

    QString family() const {return m_font.family();}
    void setFamily(const QString& fontName);

    int size() const {return m_font.pixelSize();}
    void setSize(int size);

    int style() const;
    void setStyle(int style);

    bool sizeInPixels() const { return m_sizeInPixels; }
    void setSizeInPixels(bool sizeInPixels) { m_sizeInPixels = sizeInPixels; }

    int toolType() const {return Tool::Font;}
    void accept(ToolVisitorBase* visitor);

    bool operator == (const FontTool& other) const;
    bool operator != (const FontTool& other) const;

signals:
    void changed();

private:
    QFont m_font;
    bool m_sizeInPixels;
};

/*---------------------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT StringTool : public Tool
{
    Q_OBJECT
public:
    explicit StringTool(qint16 handle = 0);
    StringTool(const StringTool& other);

    QString string() const {return m_string;}
    void setString(const QString& newString);

    int toolType() const {return Tool::String; }
    void accept(ToolVisitorBase* visitor);

    bool operator == (const StringTool& other) const;
    bool operator != (const StringTool& other) const;

signals:
    void changing(const QString& oldString, const QString& newString);
    void changed(const QString& oldString, const QString& newString);

private:
    QString m_string;
};

/*---------------------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT TextFragment : public QObject
{
    Q_OBJECT
public:
    explicit TextFragment();
    TextFragment(FontTool* fontTool, StringTool* stringTool,
                 const QColor& bgColor, const QColor& fgColor);
    TextFragment(const TextFragment& other);
    ~TextFragment();

    FontTool* fontTool() const { return m_fontTool; }
    void setFont(FontTool* fontTool);
    int fontHandle() const;
    void setFontHandle(int fontHandle);

    StringTool* stringTool() const { return m_stringTool; }
    void setString(StringTool* stringTool);
    int stringHandle() const;
    void setStringHandle(int stringHandle);

    QColor bgColor() const {return m_bgColor;}
    void setBgColor(const QColor& bgColor);

    QColor fgColor() const {return m_fgColor;}
    void setFgColor(const QColor& fgColor);

    void setData(FontTool* fontTool, StringTool* stringTool, const QColor& bgColor, const QColor& fgColor);

    QString text() const;
    int textLength() const;

    bool operator == (const TextFragment& other) const;
    bool operator != (const TextFragment& other) const;

signals:
    //    void changed();
    void stringChanging(const QString& oldString, const QString& newString);
    void stringChanged(const QString& oldString, const QString& newString);
    void fontChanged();
    void bgColorChanged();
    void fgColorChanged();

private:
//    inline void setFontWithoutNotify(FontTool* fontTool);
//    inline void setStringWithoutNotify(StringTool* stringTool);

private:
    QColor m_bgColor;
    QColor m_fgColor;

    FontTool* m_fontTool;
    StringTool* m_stringTool;
    int m_fontHandle;
    int m_stringHandle;
};

/*---------------------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT RasterTextTool : public Tool
{
    Q_OBJECT
public:
    explicit RasterTextTool(int handle = 0);
    RasterTextTool(const RasterTextTool& richText);
    ~RasterTextTool();

    QList<TextFragment*> textFragments() const {return m_textFragments;}
    TextFragment* textFragmentAt(int index) const;
//    void setTextTools(const QList<TextFragmentTool*> textTools);

    void append(TextFragment* text);
    void insertFragment(int index, FontTool* fontTool, StringTool* stringTool,
                        const QColor& bgColor, const QColor& fgColor);
//    void insertAt(int index, TextFragment* txtTool);
    void removeAt(int index);
    void clear();

    int getTextPos(int index);
    int getTextPos(TextFragment* textFragmentTool);
    int getTextLength(int index);
    int length();

    int toolType() const {return Tool::Text; }
    void accept(ToolVisitorBase* visitor);

signals:
    void textFragmentAdded();
    void textFragmentInserted(int pos);
    void textFragmentRemoved(int pos, TextFragment* textFramentTool);
    void allTextFragmentsRemoved();

    void textFragmentFgColorChanged(TextFragment* textFramentTool);
    void textFragmentBgColorChanged(TextFragment* textFramentTool);
    void textFragmentFontChanged(TextFragment* textFramentTool);
    void textFragmentStringChanging(TextFragment* textFramentTool, const QString& oldStr, const QString& newStr);
    void textFragmentStringChanged(TextFragment* textFramentTool, const QString& oldStr, const QString newStr);

private slots:
    void itemFgColorChanged();
    void itemBgColorChanged();
    void itemFontChanged();
    void itemStringChanged(const QString& oldStr, const QString& newStr);
    void itemStringChanging(const QString& oldStr, const QString& newStr);

private:
    void connectTextFragment(TextFragment* textFragment);

private:
    QList<TextFragment*> m_textFragments;
};

}


#endif // TOOL2D_H
