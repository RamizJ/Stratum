#ifndef SPACETOOLRESULT_H
#define SPACETOOLRESULT_H

#include "SpaceChunk.h"

#include <QFont>
#include <QPen>
#include <QMap>
#include <QPixmap>

namespace StData {

class St2000DataStream;
class PenTool;
class BrushTool;
class TextureTool;
class MaskedTextureTool;
class FontTool;
class StringTool;
class RasterTextTool;
class TextFragment;
class TextureRefTool;

class SpaceToolResult : public SpaceChunkResult
{
public:
    explicit SpaceToolResult(StData::Space* space, StData::St2000DataStream* stream);
    ~SpaceToolResult()
    {}

    int handle() const { return m_handle; }

    void load(SpaceChunk* parentChunk);

protected:
    virtual void loadTool() = 0;

private:
    int m_handle;
};

/*----------------------------------------------------------------------------------------------*/
class PenResult : public SpaceToolResult
{
public:
    explicit PenResult(StData::Space* space, StData::St2000DataStream* stream);

    int rgba() const { return m_rgba; }
    qint16 style() const { return m_style; }
    qint16 width() const { return m_width; }
    qint16 rop() const { return m_rop; }

protected:
    void loadTool();

private:
    int m_rgba;
    qint16 m_style;
    qint16 m_width;
    qint16 m_rop;
};

/*----------------------------------------------------------------------------------------------*/
class BrushResult : public SpaceToolResult
{
public:
    explicit BrushResult(StData::Space* space, StData::St2000DataStream* stream);

    int rgba() const { return m_rgba; }
    qint16 style() const { return m_style; }
    qint16 hatch() const { return m_hatch; }
    qint16 rop() const { return m_rop; }
    qint16 textureHandle() const { return m_textureHandle; }

protected:
    void loadTool();

private:
    int m_rgba;
    qint16 m_style;
    qint16 m_hatch;
    qint16 m_rop;
    qint16 m_textureHandle;
};

/*----------------------------------------------------------------------------------------------*/
class TextureResult : public SpaceToolResult
{
public:
    explicit TextureResult(StData::Space* space, StData::St2000DataStream* stream);

    QPixmap pixmap() const { return m_pixmap; }

protected:
    virtual void loadTool();
    QPixmap loadTexture();

protected:
    QPixmap m_pixmap;
};

/*----------------------------------------------------------------------------------------------*/
class TextureRefResult : public TextureResult
{
public:
    explicit TextureRefResult(StData::Space* space, StData::St2000DataStream* stream);

    QString reference() const { return m_reference; }

protected:
    void loadTool();

private:
    QString m_reference;
};

/*----------------------------------------------------------------------------------------------*/
class MaskedTextureResult : public TextureResult
{
public:
    explicit MaskedTextureResult(StData::Space* space, StData::St2000DataStream* stream);

protected:
    void loadTool();

private:
    QBitmap loadMask();
};

/*----------------------------------------------------------------------------------------------*/
class MaskedTextureRefResult : public MaskedTextureResult
{
public:
    explicit MaskedTextureRefResult(StData::Space* space, StData::St2000DataStream* stream);

protected:
    void loadTool();

private:
    QString m_reference;
};


/*----------------------------------------------------------------------------------------------*/
class FontResult : public SpaceToolResult
{
public:
    explicit FontResult(StData::Space* space, StData::St2000DataStream* stream);

    QFont font() const { return m_font; }
    bool isSizeInPixels() const { return m_isSizeInPixels; }

protected:
    void loadTool();

private:
    QFont::Weight getFontWeight(qint16 weightValue);

private:
    QFont m_font;
    bool m_isSizeInPixels;
};

/*----------------------------------------------------------------------------------------------*/
class StringResult : public SpaceToolResult
{
public:
    explicit StringResult(StData::Space* space, StData::St2000DataStream* stream);

    QString str() const { return m_string; }

protected:
    void loadTool();

private:
    QString m_string;
};

/*----------------------------------------------------------------------------------------------*/
class TextResult;
class RasterTextResult : public SpaceToolResult
{
public:
    explicit RasterTextResult(Space* space, St2000DataStream* stream);
    ~RasterTextResult();

    QList<TextResult*> textResults() const { return m_textResults; }

protected:
    void loadTool();

private:
    QList<TextResult*> m_textResults;
};

/*----------------------------------------------------------------------------------------------*/
class TextResult : public SpaceToolResult
{
public:
    TextResult(Space* space, St2000DataStream* stream);

    void loadTool();

    QColor bgColor() const { return m_bgColor; }
    QColor fgColor() const { return m_fgColor; }

    int stringHandle() const { return m_stringHandle; }
    int fontHandle() const { return m_fontHandle; }

private:
    QColor m_bgColor;
    QColor m_fgColor;
    int m_stringHandle;
    int m_fontHandle;
};

}

#endif // SPACETOOLRESULT_H
