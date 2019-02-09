#ifndef VISIBLEITEMSRESULT_H
#define VISIBLEITEMSRESULT_H

#include "HandleCollection.h"
#include "SpaceChunk.h"

#include <QMap>
#include <QObject>
#include <QPen>

namespace StData {

class St2000DataStream;
class SpaceItem;
class PenTool;
class BrushTool;
class TextureTool;
class MaskedTextureTool;
class FontTool;
class StringTool;
class TextFragment;
class RasterTextTool;
class HyperDataItem;

class SpaceToolResult;
class RasterTextResult;

class SpaceCollectionResult : public SpaceChunkResult
{
public:
    explicit SpaceCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void load(SpaceChunk* parentChunk = NULL);

    virtual void loadItem() = 0;

protected:
    qint16 m_count;
    qint16 m_limit;
    qint16 m_delta;
};

/*----------------------------------------------------------------------------------------------------------*/
class SortedCollectionResult : public SpaceCollectionResult
{
public:
    explicit SortedCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

private:
    quint8 m_duplicates;
};

/*----------------------------------------------------------------------------------------------------------*/
class HandleCollectionResult : public SortedCollectionResult
{
public:
    explicit HandleCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

private:
    qint16 m_current;
};

/*----------------------------------------------------------------------------------------------------------*/
class ZOrderCollectionResult : public SpaceCollectionResult
{
public:
    explicit ZOrderCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void loadItem();

    QList<int> items() const {return m_itemHandles;}

private:
    QList<int> m_itemHandles;
};

/*----------------------------------------------------------------------------------------------------------*/
class SpaceItemResult;
class SpaceItemCollectionResult : public HandleCollectionResult
{
public:
    explicit SpaceItemCollectionResult(StData::Space* space, StData::St2000DataStream* stream);
    void loadItem();

    QList<SpaceItemResult*> items() const {return m_spaceItemsResults;}

private:
    QList<SpaceItemResult*> m_spaceItemsResults;
};

class HyperDataCollectionResult : public SortedCollectionResult
{
public:
    explicit HyperDataCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    QMap<int, HyperDataItem*> hyperDataMap() const {return m_hyperDataMap;}

    void loadItem();

private:
    QMap<int, HyperDataItem*> m_hyperDataMap;
};

/*----------------------------------------------------------------------------------------------------------*/
class ToolCollectionResult : public HandleCollectionResult
{
public:
    explicit ToolCollectionResult(StData::Space* space, StData::St2000DataStream* stream);
    void loadItem();
    virtual void addItem(SpaceToolResult* result) = 0;
};

/*----------------------------------------------------------------------------------------------------------*/
class PenResult;
class PenCollectionResult : public ToolCollectionResult
{
public:
    explicit PenCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void addItem(SpaceToolResult* result);

    QList<PenResult*> items() const { return m_penResults; }

private:
    QList<PenResult*> m_penResults;
};

/*----------------------------------------------------------------------------------------------------------*/
class BrushResult;
class BrushCollectionResult : public ToolCollectionResult
{
public:
    explicit BrushCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void addItem(SpaceToolResult* result);

    QList<BrushResult*> items() const { return m_brushResults; }

private:
    QList<BrushResult*> m_brushResults;
};

/*----------------------------------------------------------------------------------------------------------*/
class TextureResult;
class TextureCollectionResult : public ToolCollectionResult
{
public:
    explicit TextureCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void addItem(SpaceToolResult* result);

    QList<TextureResult*> items() const { return m_textureResults; }

private:
    QList<TextureResult*> m_textureResults;
};

/*----------------------------------------------------------------------------------------------------------*/
class MaskedTextureResult;
class MaskedTextureCollectionResult : public ToolCollectionResult
{
public:
    explicit MaskedTextureCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void addItem(SpaceToolResult* result);

    QList<MaskedTextureResult*> items() const { return m_maskedTextureResults; }

private:
    QList<MaskedTextureResult*> m_maskedTextureResults;
};

/*----------------------------------------------------------------------------------------------------------*/
class FontResult;
class FontCollectionResult : public ToolCollectionResult
{
public:
    explicit FontCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void addItem(SpaceToolResult* result);

    QList<FontResult*> items() const { return m_fontResults; }

private:
    QList<FontResult*> m_fontResults;
};

/*----------------------------------------------------------------------------------------------------------*/
class StringResult;
class StringCollectionResult : public ToolCollectionResult
{
public:
    explicit StringCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void addItem(SpaceToolResult* result);

    QList<StringResult*> items() const { return m_stringResults; }

private:
    QList<StringResult*> m_stringResults;
};

/*----------------------------------------------------------------------------------------------------------*/
class TextResult;
class RasterTextElementCollectionResult : public SpaceCollectionResult
{
public:
    explicit RasterTextElementCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void loadItem();

    QList<StData::TextResult*> textInfoResults() const {return m_textInfoResults;}

private:
    QList<StData::TextResult*> m_textInfoResults;
};

/*----------------------------------------------------------------------------------------------------------*/
class RasterTextCollectionResult : public ToolCollectionResult
{
public:
    explicit RasterTextCollectionResult(StData::Space* space, StData::St2000DataStream* stream);

    void addItem(SpaceToolResult* result);

    QList<RasterTextResult*> items() {return m_rasterTextResults;}

private:
    QList<RasterTextResult*> m_rasterTextResults;
};

}

#endif // VISIBLEITEMSRESULT_H
