#ifndef SPACELOADER_H
#define SPACELOADER_H

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QPointF>

namespace StData {

class St2000DataStream;

class PenResult;
class BrushResult;
class TextureResult;
class MaskedTextureResult;
class FontResult;
class StringResult;
class RasterTextResult;
class SpaceItemResult;
class HyperDataCollectionResult;

class HyperDataItem;
class Space;

class SpaceLoader : public QObject
{
    Q_OBJECT
public:
    SpaceLoader(StData::Space* space, QObject* parent = nullptr);
    ~SpaceLoader();

    bool load(St2000DataStream& stream);

    QList<PenResult*> penResults() const { return m_penResults; }
    QList<BrushResult*> brushResults() const { return m_brushResults; }
    QList<TextureResult*> textureResults() const { return m_textureResults; }
    QList<MaskedTextureResult*> maskedTextureResults() const { return m_maskedTextureResults; }
    QList<FontResult*> fontResults() const { return m_fontResults; }
    QList<StringResult*> stringResults() const { return m_stringResults; }
    QList<RasterTextResult*> rasterTextResults() const { return m_rasterTextResults; }
    QList<SpaceItemResult*> spaceItemResults() const { return m_spaceItemResults; }
    QList<int> zOrderHandles() const { return m_zOrderHandles; }
    QMap<int, HyperDataItem*> spaceHyperData() const { return m_spaceHyperData; }
    QMap<int, QMap<int, HyperDataItem*>> itemsHyperData() const { return m_itemsHyperDataMap; }

    QPointF spaceOrg() const { return m_spaceOrg; }
    QPointF scaleDiv() const { return m_scaleDiv; }
    QPointF scaleMul() const { return m_scaleMul; }
    qint16 state() const { return m_state; }
    qint16 brushHandle() const { return m_brushHandle; }
    quint32 layers() const { return m_layers; }

    qint16 defaultFlags() const { return m_defaultFlags; }
    QString name() const { return m_name; }
    QString author() const { return m_author; }
    QString description() const { return m_description; }

private:
    void loadLastVersion(St2000DataStream& stream);
    void loadOldVersion(St2000DataStream& stream, int pos);

    void loadNextCollection(StData::St2000DataStream& stream);

    void clear();
    void dispose();

private:
    QList<PenResult*> m_penResults;
    QList<BrushResult*> m_brushResults;
    QList<TextureResult*> m_textureResults;
    QList<MaskedTextureResult*> m_maskedTextureResults;
    QList<FontResult*> m_fontResults;
    QList<StringResult*> m_stringResults;
    QList<RasterTextResult*> m_rasterTextResults;
    QList<SpaceItemResult*> m_spaceItemResults;
    QList<int> m_zOrderHandles;
    QMap<int, HyperDataItem*> m_spaceHyperData;
    QMap<int, QMap<int, HyperDataItem*>> m_itemsHyperDataMap;

    QPointF m_spaceOrg;
    QPointF m_scaleDiv;
    QPointF m_scaleMul;
    qint16 m_state;
    qint16 m_brushHandle;

    quint32 m_layers;
    qint16 m_defaultFlags;

    QString m_name;
    QString m_author;
    QString m_description;

    Space* m_space;
};

}

#endif // SPACELOADER_H
