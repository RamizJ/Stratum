#include "SpaceCollectionResult.h"

#include "Sc2000DataStream.h"
#include "SpaceItem.h"
#include "SpaceChunk.h"

#include "Log.h"
#include "SpaceItemResult.h"
#include "SpaceToolResult.h"
#include "Tool2d.h"
#include "GraphicsHelper.h"

#include <QByteArray>

namespace StData {

SpaceCollectionResult::SpaceCollectionResult(Space* space2d, St2000DataStream* stream) :
    SpaceChunkResult(space2d, stream)
{
    m_count = stream->readInt16();
    m_limit = stream->readInt16();
    m_delta = stream->readInt16();
}

void SpaceCollectionResult::load(SpaceChunk* /*parentChunk*/)
{
    for(int i = 0; i < m_count; i++)
        loadItem();
}

/*----------------------------------------------------------------------------------------------------------*/
SortedCollectionResult::SortedCollectionResult(Space* space2d, St2000DataStream* stream) :
    SpaceCollectionResult(space2d, stream)
{
    m_duplicates = m_stream->readUint8();
}

/*----------------------------------------------------------------------------------------------------------*/
HandleCollectionResult::HandleCollectionResult(Space* space2d, St2000DataStream* stream) :
    SortedCollectionResult(space2d, stream)
{
    m_current = m_stream->readInt16();
}

/*----------------------------------------------------------------------------------------------------------*/
ZOrderCollectionResult::ZOrderCollectionResult(Space* space2d, St2000DataStream* stream) :
    SpaceCollectionResult(space2d, stream)
{}

void ZOrderCollectionResult::loadItem()
{
    m_itemHandles << m_stream->readInt16();
}

/*----------------------------------------------------------------------------------------------------------*/
SpaceItemCollectionResult::SpaceItemCollectionResult(Space* space2d, St2000DataStream* stream) :
    HandleCollectionResult(space2d, stream)
{}

void SpaceItemCollectionResult::loadItem()
{
    SpaceChunk chunk(space(), m_stream);
    chunk.read();

    SpaceItemResult* result = nullptr;
    switch (chunk.code())
    {
        case SpaceChunk::GroupItem2d:
        case SpaceChunk::BitmapItem2d:
        case SpaceChunk::DoubleBitmapItem2d:
        case SpaceChunk::TextItem2d:
        case SpaceChunk::UserItem2d:
        case SpaceChunk::ControlItem2d:
        case SpaceChunk::LineItem2d:
        case SpaceChunk::Group3d:
        case SpaceChunk::Object3d:
        case SpaceChunk::Camera3d:
        case SpaceChunk::Light3d:
        case SpaceChunk::View3d:
        case SpaceChunk::Material3d:
            result = static_cast<SpaceItemResult*>(chunk.load());
            m_spaceItemsResults << result;
            break;

        default:
            SystemLog::instance().warning("space object chunk not recognized");
            break;
    }
}

HyperDataCollectionResult::HyperDataCollectionResult(Space* space2d, St2000DataStream* stream) :
    SortedCollectionResult(space2d, stream)
{}

void HyperDataCollectionResult::loadItem()
{
    qint16 id = m_stream->readInt16();
    qint16 size = m_stream->readInt16();
    QByteArray data = m_stream->readBytes(size);

    HyperDataItem* dataObject = new HyperDataItem();
    dataObject->setHandle(id);
    dataObject->setData(data);

    m_hyperDataMap[id] = dataObject;
}


/*----------------------------------------------------------------------------------------------------------*/
ToolCollectionResult::ToolCollectionResult(Space* space2d, St2000DataStream* stream) :
    HandleCollectionResult(space2d, stream)
{}

void ToolCollectionResult::loadItem()
{
    SpaceChunk chunk(space(), m_stream);
    chunk.read();
    {
        SpaceChunkResult* result = NULL;

        switch (chunk.code())
        {
            case SpaceChunk::PenTool:
            case SpaceChunk::BrushTool:
            case SpaceChunk::DibTool:
            case SpaceChunk::DoubleDibTool:
            case SpaceChunk::FontTool:
            case SpaceChunk::StringTool:
            case SpaceChunk::RichTextTool:
            case SpaceChunk::RefToDib2d:
            case SpaceChunk::RefToDoubleDib2d:
                result = chunk.load();
                break;

            default:
                SystemLog::instance().error("load tool collection: item is not a tool");
                break;
        }

        if(result)
        {
            SpaceToolResult* toolResult = static_cast<SpaceToolResult*>(result);
            addItem(toolResult);
        }
    }
    chunk.toEndChunk();
}

/*----------------------------------------------------------------------------------------------------------*/
PenCollectionResult::PenCollectionResult(Space* space2d, St2000DataStream* stream) :
    ToolCollectionResult(space2d, stream)
{}

void PenCollectionResult::addItem(SpaceToolResult* result)
{
    m_penResults << static_cast<PenResult*>(result);
}

/*----------------------------------------------------------------------------------------------------------*/
BrushCollectionResult::BrushCollectionResult(Space* space2d, St2000DataStream* stream) :
    ToolCollectionResult(space2d, stream)
{}

void BrushCollectionResult::addItem(SpaceToolResult* result)
{
    m_brushResults << static_cast<BrushResult*>(result);
}



/*----------------------------------------------------------------------------------------------------------*/
TextureCollectionResult::TextureCollectionResult(Space* space2d, St2000DataStream* stream) :
    ToolCollectionResult(space2d, stream)
{}

void TextureCollectionResult::addItem(SpaceToolResult* result)
{
    m_textureResults << static_cast<TextureResult*>(result);
}

/*----------------------------------------------------------------------------------------------------------*/
MaskedTextureCollectionResult::MaskedTextureCollectionResult(Space* space2d, St2000DataStream* stream) :
    ToolCollectionResult(space2d, stream)
{}

void MaskedTextureCollectionResult::addItem(SpaceToolResult* result)
{
    m_maskedTextureResults << static_cast<MaskedTextureResult*>(result);
}

/*----------------------------------------------------------------------------------------------------------*/
FontCollectionResult::FontCollectionResult(Space* space2d, St2000DataStream* stream) :
    ToolCollectionResult(space2d, stream)
{}

void FontCollectionResult::addItem(SpaceToolResult* result)
{
    m_fontResults << static_cast<FontResult*>(result);
}

/*----------------------------------------------------------------------------------------------------------*/
StringCollectionResult::StringCollectionResult(Space* space2d, St2000DataStream* stream) :
    ToolCollectionResult(space2d, stream)
{}

void StringCollectionResult::addItem(SpaceToolResult* result)
{
    m_stringResults << static_cast<StringResult*>(result);
}

/*----------------------------------------------------------------------------------------------------------*/
RasterTextElementCollectionResult::RasterTextElementCollectionResult(Space* space2d, St2000DataStream* stream) :
    SpaceCollectionResult(space2d, stream)
{}

void RasterTextElementCollectionResult::loadItem()
{
    TextResult* textInfoResult = new TextResult(space(), stream());
    textInfoResult->loadTool();

    m_textInfoResults << textInfoResult;

//    QByteArray fgColorBytes = stream()->readBytes(4);
//    QByteArray bgColorBytes = stream()->readBytes(4);
//    qint16 fontId = stream()->readInt16();
//    qint16 stringId = stream()->readInt16();

//    TextInfo* logicsText = new TextInfo();
//    logicsText->setFgColor(GraphicsHelper::colorFromBytes(fgColorBytes));
//    logicsText->setBgColor(GraphicsHelper::colorFromBytes(bgColorBytes));
//    logicsText->setFontHandle(fontId);
//    logicsText->setStringHandle(stringId);

//    m_logicTextItems << logicsText;
}

/*----------------------------------------------------------------------------------------------------------*/
RasterTextCollectionResult::RasterTextCollectionResult(Space* space2d, St2000DataStream* stream) :
    ToolCollectionResult(space2d, stream)
{}

void RasterTextCollectionResult::addItem(SpaceToolResult* result)
{
    m_rasterTextResults << static_cast<RasterTextResult*>(result);
//    RasterTextResult* rasterTextResult = static_cast<RasterTextResult*>(result);

//    RasterText* rasterText = rasterTextResult->rasterText();
//    m_rasterTextItems.insert(rasterText, true);

//    QList<TextInfo*> textInfoItems;
//    for(TextInfo* textInfo : rasterTextResult->textInfoItems())
//    {
//        m_textInfoItems.insert(textInfo);
//        textInfoItems << textInfo;
//    }
//    rasterText->setTextInfoItems(textInfoItems);
}

}
