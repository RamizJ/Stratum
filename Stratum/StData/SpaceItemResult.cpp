#include "SpaceItemResult.h"

#include "PolylineArrow.h"
#include "Sc2000DataStream.h"
#include "Space.h"
#include "SpaceChunk.h"
#include "SpaceCollectionResult.h"
#include "SpaceItem.h"
#include "StDataGlobal.h"

#include <QDebug>

namespace StData {

SpaceItemResult::SpaceItemResult(Space* space2d, St2000DataStream* stream) :
    SpaceChunkResult(space2d, stream)
{}

void SpaceItemResult::load(SpaceChunk* parentChunk)
{
    m_handle = stream()->readInt16();
    m_options = stream()->readInt16();

    if(stream()->fileVersion() >= 0x0102 && stream()->fileVersion() < 0x0300)
        m_name = stream()->readString();

    loadSpaceItem();

    if(stream()->fileVersion() >= 0x300)
    {
        while(parentChunk->pos() > stream()->pos() && stream()->status() == QDataStream::Ok)
        {
            SpaceChunk chunk(space(), stream());
            chunk.read();
            if(chunk.code() == SpaceChunk::SpaceItemName)
            {
                if(chunk.size() > 6)
                {
//                    m_name = QString::fromLocal8Bit(stream()->readBytes(chunk.size() - 6));
                    QByteArray data = stream()->readBytes(chunk.size() - 6);
//                    m_name = QString::fromLocal8Bit(data);
                    m_name = StData::fromWindows1251(data);
                }
            }
            else if(chunk.code() == SpaceChunk::ItemDataCollection)
            {
                auto itemDataCollectionResult = static_cast<HyperDataCollectionResult*>(chunk.load());
                m_hyperDataMap = itemDataCollectionResult->hyperDataMap();
            }
            chunk.toEndChunk();
        }
    }
}



/*--------------------------------------------------------------------------------------------*/
SpaceItem2dResult::SpaceItem2dResult(Space* space2d, St2000DataStream* stream) :
    SpaceItemResult(space2d, stream)
{}

void SpaceItem2dResult::loadSpaceItem()
{
    m_origin = m_stream->readPointF();
    m_size = m_stream->readSizeF();

    loadSpaceItem2d();
}

/*--------------------------------------------------------------------------------------------*/
ShapeItem2dResult::ShapeItem2dResult(Space* space2d, St2000DataStream* stream) :
    SpaceItem2dResult(space2d, stream)
{}

void ShapeItem2dResult::loadSpaceItem2d()
{
    m_penHandle = stream()->readInt16();
    m_brushHandle = stream()->readInt16();

    loadShapeItem2d();
}

/*--------------------------------------------------------------------------------------------*/
PolylineResult::PolylineResult(Space* space2d, St2000DataStream* stream) :
    ShapeItem2dResult(space2d, stream)
{}

void PolylineResult::loadShapeItem2d()
{
    qint16 pointCount = stream()->readInt16();
    for(int i = 0; i < pointCount; i++)
        m_polyline << stream()->readPointF();

    if(stream()->fileVersion() > 0x0200)
    {
        quint8 size = stream()->readUint8();
        if(size)
        {
            QByteArray arrowBytes = stream()->readBytes(size);
            if(arrowBytes.count() >= 50)
            {
                St2000DataStream stream(arrowBytes);

                m_arrow = std::make_shared<PolylineArrow>();

                m_arrow->setEndLength(stream.readDouble());
                m_arrow->setEndAngle(stream.readDouble());
                m_arrow->setEndDistance(stream.readDouble());

                m_arrow->setStartLength(stream.readDouble());
                m_arrow->setStartAngle(stream.readDouble());
                m_arrow->setStartDistance(stream.readDouble());

                m_arrow->setFlags(stream.readInt16());
            }
        }
    }
}

/*--------------------------------------------------------------------------------------------*/
UserObjectResult::UserObjectResult(Space* space2d, St2000DataStream* stream) :
    SpaceItem2dResult(space2d, stream)
{}

void UserObjectResult::loadSpaceItem2d()
{
    m_typeName = stream()->readString();

    qint32 bytesCount = stream()->readInt32();
    m_data = stream()->readBytes(bytesCount);
}

/*--------------------------------------------------------------------------------------------*/
GroupItemResult::GroupItemResult(Space* space2d, St2000DataStream* stream) :
    SpaceItemResult(space2d, stream)
{}

void GroupItemResult::loadSpaceItem()
{
    SpaceChunk chunk(space(), stream());
    chunk.read();
    if(chunk.code() == SpaceChunk::PrimaryCollection)
    {
        ZOrderCollectionResult result(space(), stream());
        result.load();

        m_itemsHandles.append(result.items());
    }
    chunk.toEndChunk();
}

/*--------------------------------------------------------------------------------------------*/
TextItem2dResult::TextItem2dResult(Space* space2d, St2000DataStream* stream) :
    SpaceItem2dResult(space2d, stream)
{}

void TextItem2dResult::loadSpaceItem2d()
{
    m_rasterTextHandle = stream()->readInt16();
    m_delta = stream()->readPointF();
    m_textAngle = stream()->readAngle() * 10;
}

/*--------------------------------------------------------------------------------------------*/
TextureItem2dResult::TextureItem2dResult(Space* space2d, St2000DataStream* stream) :
    SpaceItem2dResult(space2d, stream)
{}

void TextureItem2dResult::loadSpaceItem2d()
{
    m_imageOrigin = stream()->readPointF();
    m_imageSize = stream()->readSizeF();
    m_angle = stream()->readAngle();
    m_pixmapHandle = stream()->readInt16();
}

/*--------------------------------------------------------------------------------------------*/
MaskedTextureItem2dResult::MaskedTextureItem2dResult(Space* space2d, St2000DataStream* stream) :
    TextureItem2dResult(space2d, stream)
{}

ControlItem2dResult::ControlItem2dResult(Space* space, St2000DataStream* stream) :
    SpaceItem2dResult(space, stream)
{}

void ControlItem2dResult::loadSpaceItem2d()
{
    m_className = stream()->readString();
    m_text = stream()->readString();
    m_dwStyle = stream()->readInt32();
    m_exStyle = stream()->readInt32();

    int w = stream()->readInt16();
    int h = stream()->readInt16();
    m_size = QSize(w, h);

    stream()->readInt16();
}

}
