#include "Sc2000DataStream.h"
#include "SpaceCollectionResult.h"
#include "SpaceItem.h"
#include "SpaceLoader.h"
#include "SpaceToolResult.h"

namespace StData {

SpaceLoader::SpaceLoader(Space* space, QObject* parent) :
    QObject(parent),
    m_space(space)
{}

SpaceLoader::~SpaceLoader()
{
    dispose();
}

bool SpaceLoader::load(St2000DataStream& stream)
{
    clear();

    qint16 code = stream.readInt16();
    if(code != 17458)
        return false;

    int pos = stream.pos();
    stream.readFileVersion();
    int version = stream.readInt16();

    if(version > 0x0300)
        return false;

    if(stream.fileVersion() < 0x0300)
        loadOldVersion(stream, pos);
    else
        loadLastVersion(stream);

    return true;
}

void SpaceLoader::loadLastVersion(St2000DataStream& stream)
{
    SpaceChunk spaceChunk(m_space, &stream);
    spaceChunk.read();

    m_spaceOrg = stream.readPointF();
    m_scaleDiv = stream.readPointF();
    m_scaleMul = stream.readPointF();
    m_state = stream.readInt16();

    /*BkBrush=(PBrush2d)(void*)MAKELONG(ps->ReadWord(),0);*/
    m_brushHandle = stream.readInt16();

    m_layers = stream.readUint32();
    m_defaultFlags = stream.readInt16();

    while(spaceChunk.pos() > stream.pos() && stream.isOkStatus())
        loadNextCollection(stream);
}

void SpaceLoader::loadOldVersion(St2000DataStream& stream, int pos)
{
    long mainPos = stream.readInt32() + pos;
    long toolPos = stream.readInt32() + pos;

    m_spaceOrg = stream.readPointF();
    m_scaleDiv = stream.readPointF();
    m_scaleMul = stream.readPointF();
    stream.readString();
    m_state = stream.readInt16();

    m_name = stream.readString();
    m_author = stream.readString();
    m_description = stream.readString();

    SpaceChunk spaceChunk(m_space, &stream);
    //RotateCenter skip
    spaceChunk.skip();
    //EditFrame skip
    spaceChunk.skip();
    stream.readInt16();

    stream.seek(mainPos);

    m_zOrderHandles = loadChunk<ZOrderCollectionResult*>(m_space, &stream)->items();
    m_spaceItemResults = loadChunk<SpaceItemCollectionResult*>(m_space, &stream)->items();

    stream.seek(toolPos);
    qint16 toolsCount = stream.readInt16();
    for(int i = 0; i < toolsCount; i++)
        loadNextCollection(stream);

    if(stream.fileVersion() > 0x0102)
    {
        m_layers = stream.readInt32();
        m_defaultFlags = stream.readInt16();

        qint16 code = stream.readInt16();
        while(code)
        {
            switch(code)
            {
                case 1:
                {
                    int objectHandle = stream.readInt16();
                    if(auto itemHyperDataResult = loadChunk<HyperDataCollectionResult*>(m_space, &stream))
                        m_itemsHyperDataMap[objectHandle] = itemHyperDataResult->hyperDataMap();
                }break;

                case 2:
                {
                    if(auto spaceHyperDataResult = loadChunk<HyperDataCollectionResult*>(m_space, &stream))
                        m_spaceHyperData = spaceHyperDataResult->hyperDataMap();
                }break;

                case 3:
                    //coord system
                    qint16 size = stream.readInt16();
                    stream.readBytes(size);
                    break;
            }
            code = stream.readInt16();
        }
    }

    QString eofStr = stream.readString();
    if(eofStr != "End Of File <!>")
    {
//        dispose();
//        throw std::runtime_error(tr("Неверный формат файла пространства. Неверный конец файла").toStdString());
    }
}

void SpaceLoader::loadNextCollection(St2000DataStream& stream)
{
    SpaceChunk itemChunk(m_space, &stream);
    itemChunk.read();
    if(SpaceChunkResult* result = itemChunk.load())
    {
        switch(itemChunk.code())
        {
            case SpaceChunk::PenCollection:
                m_penResults = static_cast<PenCollectionResult*>(result)->items();
                break;

            case SpaceChunk::BrushCollection:
                m_brushResults = static_cast<BrushCollectionResult*>(result)->items();
                break;

            case SpaceChunk::DibCollection:
                m_textureResults = static_cast<TextureCollectionResult*>(result)->items();
                break;

            case SpaceChunk::DoubleDibCollection:
                m_maskedTextureResults = static_cast<MaskedTextureCollectionResult*>(result)->items();
                break;

            case SpaceChunk::FontCollection:
                m_fontResults = static_cast<FontCollectionResult*>(result)->items();
                break;

            case SpaceChunk::StringCollection:
                m_stringResults = static_cast<StringCollectionResult*>(result)->items();
                break;

            case SpaceChunk::RasterTextCollection:
                m_rasterTextResults = static_cast<RasterTextCollectionResult*>(result)->items();
                break;

            case SpaceChunk::PrimaryCollection:
                m_zOrderHandles = static_cast<ZOrderCollectionResult*>(result)->items();
                break;

            case SpaceChunk::SpaceItemsCollection:
                m_spaceItemResults = static_cast<SpaceItemCollectionResult*>(result)->items();
                break;

            case SpaceChunk::ItemDataCollection:
                m_spaceHyperData = static_cast<HyperDataCollectionResult*>(result)->hyperDataMap();
                break;
        }
        delete result;
    }
}

void SpaceLoader::clear()
{
    m_penResults.clear();
    m_brushResults.clear();
    m_textureResults.clear();
    m_maskedTextureResults.clear();
    m_fontResults.clear();
    m_stringResults.clear();
    m_rasterTextResults.clear();
    m_spaceItemResults.clear();
    m_zOrderHandles.clear();
}

void SpaceLoader::dispose()
{
    qDeleteAll(m_penResults);
    qDeleteAll(m_brushResults);
    qDeleteAll(m_textureResults);
    qDeleteAll(m_maskedTextureResults);
    qDeleteAll(m_fontResults);
    qDeleteAll(m_stringResults);
    qDeleteAll(m_rasterTextResults);
    qDeleteAll(m_spaceItemResults);
//    qDeleteAll(m_itemsHyperDataMap);
//    qDeleteAll(m_spaceHyperDataMap);

    clear();
}





















}
