#include "SpaceChunk.h"


#include "Log.h"
#include "SpaceCollectionResult.h"
#include "SpaceItemResult.h"
#include "SpaceToolResult.h"
#include "Sc2000DataStream.h"

namespace StData {

SpaceChunk::SpaceChunk(Space* space2d, St2000DataStream* stream) :
    m_space2d(space2d),
    m_code(0),
    m_pos(0),
    m_size(0),
    m_stream(stream),
    m_result(NULL)
{}

SpaceChunk::~SpaceChunk()
{
    if(m_result)
        delete m_result;
}

void SpaceChunk::read()
{
    if(m_stream->fileVersion() >= 0x300)
    {
        m_pos = m_stream->pos();
        m_code = m_stream->readInt16();
        m_size = m_stream->readInt32();
        m_pos += m_size;
    }
    else
    {
        m_code = m_stream->readInt16();
        m_pos = 0;
    }
    m_type = static_cast<Type>(m_code);
}

SpaceChunkResult* SpaceChunk::load()
{
    SpaceChunkResult* result = nullptr;
    switch(m_type)
    {
        case SomeCollection:
            SystemLog::instance().warning("load of SomeCollection not supported");
            break;

        case PenCollection:
            result = new PenCollectionResult(m_space2d, m_stream);
            break;

        case BrushCollection:
            result = new BrushCollectionResult(m_space2d, m_stream);
            break;

        case DibCollection:
            result = new TextureCollectionResult(m_space2d, m_stream);
            break;

        case DoubleDibCollection:
            result = new MaskedTextureCollectionResult(m_space2d, m_stream);
            break;

        case FontCollection:
            result = new FontCollectionResult(m_space2d, m_stream);
            break;

        case StringCollection:
            result = new StringCollectionResult(m_space2d, m_stream);
            break;

        case RasterTextCollection:
            result = new RasterTextCollectionResult(m_space2d, m_stream);
            break;

        case RichTextElementCollection:
            break;

        case SpaceItemsCollection:
            result = new SpaceItemCollectionResult(m_space2d, m_stream);
            break;

        case PrimaryCollection:
            result = new ZOrderCollectionResult(m_space2d, m_stream);
            break;

        case ItemDataCollection:
            result = new HyperDataCollectionResult(m_space2d, m_stream);
            break;

        case GroupItem:
            result = new GroupItemResult(m_space2d, m_stream);
            break;

        case GroupItem2d:
            result = new GroupItemResult(m_space2d, m_stream);
            break;

        case RGroupItem2d:
            result = new GroupItemResult(m_space2d, m_stream);
            break;

        case LineItem2d:
            result = new PolylineResult(m_space2d, m_stream);
            break;

        case BitmapItem2d:
            result = new TextureItem2dResult(m_space2d, m_stream);
            break;

        case DoubleBitmapItem2d:
            result = new MaskedTextureItem2dResult(m_space2d, m_stream);
            break;

        case TextItem2d:
            result = new TextItem2dResult(m_space2d, m_stream);
            break;

        case UserItem2d:
            result = new UserObjectResult(m_space2d, m_stream);
            break;

        case ControlItem2d:
            result = new ControlItem2dResult(m_space2d, m_stream);
            break;

        case EditFrame:
            m_stream->readInt16();
            m_stream->readPointF();
            SystemLog::instance().warning("load of EditFrame not supported");
            break;

        case RotateCenter:
            m_stream->readPointF();
            m_stream->readPointF();
            SystemLog::instance().warning("load of RotateCenter not supported");
            break;

        case PenTool:
            result = new PenResult(m_space2d, m_stream);
            break;

        case BrushTool:
            result = new BrushResult(m_space2d, m_stream);
            break;

        case DibTool:
            result = new TextureResult(m_space2d, m_stream);
            break;

        case DoubleDibTool:
            result = new MaskedTextureResult(m_space2d, m_stream);
            break;

        case FontTool:
            result = new FontResult(m_space2d, m_stream);
            break;

        case StringTool:
            result = new StringResult(m_space2d, m_stream);
            break;

        case RichTextTool:
            result = new RasterTextResult(m_space2d, m_stream);
            break;

        case RefToDib2d:
            result = new TextureRefResult(m_space2d, m_stream);
            break;

        case RefToDoubleDib2d:
            result = new MaskedTextureRefResult(m_space2d, m_stream);
            break;

        case Space2dType:
            SystemLog::instance().warning("load of Space2dType not supported");
            break;

        default:
            SystemLog::instance().warning("undefined chunk loading. Chunk type code: " + m_type);
            break;
    }

    if(result)
    {
        result->setLoadingType(m_type);
        result->load(this);
    }

    toEndChunk();

    return result;
}

void SpaceChunk::toEndChunk()
{
    if(m_stream->fileVersion() >= 0x300)
    {
        if(m_stream->pos() != m_pos)
            m_stream->device()->seek(m_pos);
    }
}

void SpaceChunk::skip()
{
    read();

    if(m_stream->fileVersion() < 0x300)
    {
        SpaceChunkResult* result = load();
        if(result)
            delete result;
    }
    else if(m_stream->pos() != m_pos)
            m_stream->device()->seek(m_pos);
}

SpaceChunkResult* SpaceChunk::load(Space* space, St2000DataStream* stream)
{
    SpaceChunk chunk(space, stream);
    chunk.read();
    return chunk.load();
}

SpaceChunkResult::SpaceChunkResult(Space* space2d, St2000DataStream* stream) :
    m_space2d(space2d),
    m_stream(stream)
{}

}
