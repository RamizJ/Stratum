#ifndef SPACEITEMRESULT_H
#define SPACEITEMRESULT_H

#include "SpaceChunk.h"
#include "StDataTypes.h"

#include <QMap>
#include <QPolygon>

namespace StData {

/*--------------------------------------------------------------------------------------------*/
class SpaceItemResult : public SpaceChunkResult
{
public:
    SpaceItemResult(StData::Space* space, StData::St2000DataStream* stream);

    void load(SpaceChunk* parentChunk);

    qint16 handle() const { return m_handle; }
    qint16 options() const { return m_options; }
    QString name() const { return m_name; }

    QMap<int, HyperDataItem*> hyperDataMap() const { return m_hyperDataMap; }

protected:
    virtual void loadSpaceItem() = 0;

private:
    qint16 m_handle;
    qint16 m_options;
    QString m_name;
    QMap<int, HyperDataItem*> m_hyperDataMap;
};

/*--------------------------------------------------------------------------------------------*/
class SpaceItem2dResult : public SpaceItemResult
{
public:
    SpaceItem2dResult(Space* space, St2000DataStream* stream);

    QPointF origin() const { return m_origin; }
    QSizeF size() const { return m_size; }

protected:
    void loadSpaceItem();
    virtual void loadSpaceItem2d() = 0;

private:
    QPointF m_origin;
    QSizeF m_size;
};

/*--------------------------------------------------------------------------------------------*/
class ShapeItem2dResult : public SpaceItem2dResult
{
public:
    ShapeItem2dResult(StData::Space* space, StData::St2000DataStream* stream);

    qint16 penHandle() const { return m_penHandle; }
    qint16 brushHandle() const { return m_brushHandle; }

protected:
    void loadSpaceItem2d();
    virtual void loadShapeItem2d() = 0;

private:
    qint16 m_penHandle;
    qint16 m_brushHandle;
};

/*--------------------------------------------------------------------------------------------*/
class PolylineResult : public ShapeItem2dResult
{
public:
    PolylineResult(StData::Space* space, StData::St2000DataStream* stream);

    QPolygonF polyline() const { return m_polyline; }

    PolylineArrowPtr arrow() const { return m_arrow; }
    void setArrow(const PolylineArrowPtr& arrow) { m_arrow = arrow; }

protected:
    void loadShapeItem2d();

private:
    QPolygonF m_polyline;
    PolylineArrowPtr m_arrow;
};

/*--------------------------------------------------------------------------------------------*/
class UserObjectResult : public SpaceItem2dResult
{
public:
    explicit UserObjectResult(StData::Space* space, StData::St2000DataStream* stream);

    QString typeName() const { return m_typeName; }
    QByteArray data() const { return m_data; }

protected:
    void loadSpaceItem2d();

private:
    QString m_typeName;
    QByteArray m_data;
};

/*--------------------------------------------------------------------------------------------*/
class GroupItemResult : public SpaceItemResult
{
public:
    GroupItemResult(StData::Space* space, StData::St2000DataStream* stream);

    QList<int> itemsHandles() const {return m_itemsHandles;}

protected:
    void loadSpaceItem();

private:
    QList<int> m_itemsHandles;
};


/*--------------------------------------------------------------------------------------------*/
class TextItem2dResult : public SpaceItem2dResult
{
public:
    TextItem2dResult(StData::Space* space, StData::St2000DataStream* stream);

    qint16 rasterTextHandle() const { return m_rasterTextHandle; }
    QPointF delta() const { return m_delta; }
    double textAngle() const { return m_textAngle; }

protected:
    void loadSpaceItem2d();

private:
    qint16 m_rasterTextHandle;
    QPointF m_delta;
    double m_textAngle;
};

/*--------------------------------------------------------------------------------------------*/
class TextureItem2dResult : public SpaceItem2dResult
{
public:
    TextureItem2dResult(StData::Space* space, StData::St2000DataStream* stream);

    QPointF imageOrigin() const { return m_imageOrigin; }
    QSizeF imageSize() const { return m_imageSize; }
    double angle() const { return m_angle; }
    qint16 textureHandle() const { return m_pixmapHandle; }

protected:
    void loadSpaceItem2d();

private:
    QPointF m_imageOrigin;
    QSizeF m_imageSize;
    double m_angle;
    qint16 m_pixmapHandle;
};

/*--------------------------------------------------------------------------------------------*/
class MaskedTextureItem2dResult : public TextureItem2dResult
{
public:
    MaskedTextureItem2dResult(StData::Space* space, StData::St2000DataStream* stream);
};

class ControlItem2dResult : public SpaceItem2dResult
{
public:
    ControlItem2dResult(Space* space, St2000DataStream* stream);

    QString className() const { return m_className; }
    QString text() const { return m_text; }
    QSize size() const { return m_size; }
    int dwStyle() const { return m_dwStyle; }
    int exStyle() const { return m_exStyle; }
    qint16 id() const { return m_id; }

protected:
    void loadSpaceItem2d();

private:
    QString m_className;
    QString m_text;
    int m_dwStyle;
    int m_exStyle;
    qint16 m_id;
    QSize m_size;
};

}

#endif // SPACEITEMRESULT_H
