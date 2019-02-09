#ifndef SPACEITEM_H
#define SPACEITEM_H

#include "stdata_global.h"

#include "HandleItem.h"
#include "StDataTypes.h"

#include <QtGlobal>
#include <QPointF>
#include <QSizeF>
#include <QString>
#include <QLineF>
#include <QPolygon>

namespace StData {

class STDATASHARED_EXPORT SpaceItem : public QObject, public HandleItem
{
    Q_OBJECT
public:
    enum Options {HardDontSelect = 4, DontSelect = 8};
    enum OptionsMode {Set = 1, Reset, Put};
    enum Type {Type_Graphics, Type_Object, Type_Link, Type_Udefined};

public:
    SpaceItem(Space* space);
    SpaceItem(const SpaceItem& other);
    virtual ~SpaceItem(){}

    GroupItem* ownerItem() const;
    SpaceItem* rootOwnerItem();
    void setOwnerItem(GroupItem* ownerItem);
    bool isChildFor(SpaceItem* parentItem);

    QString name() const {return m_name;}
    void setName(const QString& name) {m_name = name;}

    qint16 options() const {return m_options;}
    void setOptions(const qint16 options);
    void setOptions(OptionsMode mode, qint16 options);

    int layer() const {return m_layer;}
    void setLayer(int layer) {m_layer = layer;}

    Space* space() const {return m_space;}
    virtual void setSpace(Space* space) { m_space = space; }

    bool isItemVisible();
    bool isLayerVisible();
    bool isVisible();
    void setItemVisible(bool visible) { m_visible = visible; }

    bool isSelectable() const;

    double opacity() const {return m_opacity;}
    void setOpacity(double opacity) {m_opacity = opacity;}

    int zOrder();

    Type typeOnSpace();
    void setTypeOnSpace(Type typeOnSpace);
//    virtual GraphicsType graphicsType() = 0;

    int loadingType() const { return m_loadingType; }
    void setLoadingType(int loadingType) { m_loadingType = loadingType; }

    virtual SpaceItem* deepCopy() = 0;
    virtual void accept(SpaceItemVisitorBase* visitor) = 0;

    SpaceItemHyperKeyPtr hyperKey() const {return m_hyperKey;}
    SpaceItemHyperKeyPtr actualHyperKey() const;
    void setHyperKey(const SpaceItemHyperKeyPtr& hyperKey) { m_hyperKey = hyperKey; }

    void invokeHyperJump();

signals:
    void hyperJumpInvoked();

private:
    Space* m_space;
    GroupItem* m_ownerItem;

    QString m_name;
    qint16 m_options;
    int m_layer;
    bool m_visible;
    double m_opacity; // 0..1

    Type m_spaceType;
    int m_loadingType;
    SpaceItemHyperKeyPtr m_hyperKey;
};

/*------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT SpaceItem2d : public SpaceItem
{
public:
    SpaceItem2d(Space* space);
    SpaceItem2d(const SpaceItem2d& other);

    virtual QPointF origin() const {return m_origin;}
    virtual void setOrigin(const QPointF& origin) {m_origin = origin;}

    virtual QSizeF size() const {return m_size;}
    virtual void setSize(const QSizeF& size) { m_size = size; }

    virtual QRectF rect() const;

    qreal angle() const {return m_angle;}
    void setAngle(const double& angle) {m_angle = angle;}

    virtual bool containsPoint(double x, double y) const;
//    virtual bool getDistanceToPoint(const QPointF& point);

public:
    const int MAX_DISTANCE = 32760;

private:
    QPointF m_origin;
    QSizeF m_size;
    qreal m_angle; //In radians
};

/*------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT ShapeItem2d : public SpaceItem2d
{
public:
    ShapeItem2d(Space* space2d);
    ShapeItem2d(const ShapeItem2d& other);
    ~ShapeItem2d();

    PenTool* penTool() const { return m_penTool; }
    void setPenTool(PenTool* penTool);
    //    void setPenTool(const QPen& penTool);

    BrushTool* brushTool() const { return m_brushTool; }
    void setBrushTool(BrushTool* brushTool);
    void setBrushData(const QBrush& brushTool, StData::TextureTool* pixmapInfo = 0);

    QPen getPen();
    QBrush getBrush();

    int penHandle() const { return m_penHandle; }
    void setPenHandle(int penHandle) { m_penHandle = penHandle; }

    int brushHandle() const { return m_brushHandle; }
    void setBrushHandle(int brushHandle) { m_brushHandle = brushHandle; }

private:
    PenTool* m_penTool;
    BrushTool* m_brushTool;
    int m_penHandle;
    int m_brushHandle;
};

/*------------------------------------------------------------------------------------*/
//class STDATASHARED_EXPORT Line2dItem : public ShapeItem2d
//{
//public:
//    Line2dItem(Space* space2d) :ShapeItem2d(space2d)
//    {}
////    GraphicsType graphicsType() {return SpaceItem::GT_Line;}

//    QLineF line() const {return m_line;}
//    void setLine(const QLineF& line) {m_line = line;}

//private:
//    QLineF m_line;
//};

/*------------------------------------------------------------------------------------*/
//class STDATASHARED_EXPORT Ellipse2dItem : public ShapeItem2d
//{
//public:
//    Ellipse2dItem(Space* space2d) : ShapeItem2d(space2d)
//    {}

////    GraphicsType graphicsType() {return SpaceItem::GT_Ellipse;}

//    QRectF field() const {return m_field;}
//    void setField(const QRectF& field) {m_field = field;}

//private:
//    QRectF m_field;
//};

class STDATASHARED_EXPORT PolylineItem2d : public ShapeItem2d
{
public:
    PolylineItem2d(Space* space2d);
    PolylineItem2d(const PolylineItem2d& other);

    QPolygonF polyline() const {return m_polyline;}
    void setPolyline(const QPolygonF& polyline);

    PolylineArrowPtr arrow() const { return m_arrow; }
    void setArrow(const PolylineArrowPtr& arrows) { m_arrow = arrows; }

    int pointsCount();
    void addPoint(const QPointF& point);
    void insertPoint(int index, const QPointF& point);
    QPointF getPoint(int index) const;
    void setPoint(int index, const QPointF& point);
    void removePoint(int index);
    void removePoint(const QPointF& point);

    bool isPoint() const;
    bool isClosed() const;

    int fillRule() const { return m_fillRule; }
    void setFillRule(int fillRule) { m_fillRule = fillRule; }

    void setLineArrows(double aa, double al, double as, double ba, double bl, double bs);

    QRectF boundingRect() const;
    void correctBoundingRect(const QPointF& point);

    bool containsPoint(double x, double y) const;
//    int getDistanceToPoint(const QPointF& p) const;

    SpaceItem* deepCopy();
    void accept(SpaceItemVisitorBase* visitor);

    // ISpaceItem2d interface
public:
    QPointF origin() const;
    void setOrigin(const QPointF& origin);
    QSizeF size() const;
    void setSize(const QSizeF& newSize);
    QRectF rect() const;

private:
    bool pointInRect(const QPointF& p, const QRectF& rect) const;

private:
    QPolygonF m_polyline;
    int m_fillRule;
    QRectF m_boundingRect;
    PolylineArrowPtr m_arrow;
};

/*------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT UserObject : public SpaceItem2d
{
public:
    explicit UserObject(Space* space2d) : SpaceItem2d(space2d)
    {}

    QString typeName() const {return m_typeName;}
    void setTypeName(const QString& typeName) {m_typeName = typeName;}

    QByteArray data() const {return m_data;}
    void setData(const QByteArray& data) {m_data = data;}

    SpaceItem* deepCopy();
    void accept(SpaceItemVisitorBase* visitor);

private:
    QString m_typeName;
    qint32 m_flags;

    QByteArray m_data;
};

/*------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT GroupItem : public SpaceItem2d
{
public:
    GroupItem(Space* space);
    GroupItem(const GroupItem& other);

//    GraphicsType graphicsType() {return SpaceItem::GT_Group;}
//    QList<int> itemHandles() const {return m_itemHandles;}


    void addItemHandle(int itemHandle);
    void addItemHandles(QList<int> itemHandles);
    void removeItemHandle(int itemHandle);

    QList<SpaceItem*> items() const { return m_items; }
    int itemsCount();
    SpaceItem* itemAt(int index);
    int indexOf(SpaceItem* item);
    void append(SpaceItem* item);
    void append(QList<SpaceItem*> items);
    void insertAt(int index, SpaceItem* item);
    void remove(SpaceItem* item);
    void removeAt(int index);
    SpaceItem* takeAt(int index);
    void clear();

    void prepareToRemoving();

    bool contains(SpaceItem* item);
    bool containsHandle(int itemHandle);
    bool isSelfOrAncestor(SpaceItem* ancestorItem);

    QRectF calculateGroupRect() const;

    SpaceItem* deepCopy();
    void accept(SpaceItemVisitorBase* visitor);
    virtual void setSpace(Space* space);

    // ISpaceItem2d interface
public:
    QPointF origin() const;
    void setOrigin(const QPointF& newOrg);

    QSizeF size() const;
    void setSize(const QSizeF& newSize);

private:
    QList<SpaceItem*> m_items;
};

/*------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT TextItem2d : public SpaceItem2d
{
public:
    explicit TextItem2d(Space* space);
    TextItem2d(const TextItem2d& other);
    ~TextItem2d();

    double textAngle() const { return m_textAngle; }
    void setTextAngle(double textAngle) { m_textAngle = textAngle; }

    RasterTextTool* rasterText() const { return m_rasterText; }
    void setRasterText(RasterTextTool* rasterText);
    int rasterTextHandle() const { return m_rasterTextHandle; }
    void setRasterTextHandle(int rasterTextHandle) { m_rasterTextHandle = rasterTextHandle; }

    SpaceItem* deepCopy();
    void accept(SpaceItemVisitorBase* visitor);

private:
    RasterTextTool* m_rasterText;
    double m_textAngle;
    int m_rasterTextHandle;
};

/*------------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT TextureItem2d : public SpaceItem2d
{
public:
    explicit TextureItem2d(Space* space);
    TextureItem2d(const TextureItem2d& other);
    ~TextureItem2d();

    TextureTool* texture() const { return m_textureTool; }
    void setTexture(TextureTool* texture);
    int textureHandle() const { return m_textureHandle; }
    void setTextureHandle(int textureHandle) { m_textureHandle = textureHandle; }

    QPointF textureOrigin() const {return m_textureOrigin;}
    void setTextureOrigin(const QPointF& imageOrigin) {m_textureOrigin = imageOrigin;}

    QSizeF textureSize() const {return m_textureSize;}
    void setTextureSize(const QSizeF& textureSize) {m_textureSize = textureSize;}

    QRectF offsetRect() const;

    virtual SpaceItem* deepCopy();
    void accept(SpaceItemVisitorBase* visitor);

private:
    TextureTool* m_textureTool;
    int m_textureHandle;

    QPointF m_textureOrigin;
    QSizeF m_textureSize;
};

class STDATASHARED_EXPORT MaskedTextureItem2d : public TextureItem2d
{
public:
    explicit MaskedTextureItem2d(Space* space2d) : TextureItem2d(space2d)
    {}
    MaskedTextureItem2d(const MaskedTextureItem2d& other);

    SpaceItem* deepCopy();
};

class STDATASHARED_EXPORT HyperDataItem : public HandleItem
{
public:
    explicit HyperDataItem(){}

    QByteArray data() const {return m_data;}
    void setData(const QByteArray& data) {m_data = data;}

private:
    QByteArray m_data;
};

class STDATASHARED_EXPORT ControlItem2d : public SpaceItem2d
{
public:
    enum WindowStyle
    {
        WS_BORDER = 0x00800000, WS_VSCROLL = 0x00200000, WS_HSCROLL = 0x00100000
    };
    enum ButtonStyle
    {
        BS_PUSHBUTTON = 0x00000000, BS_DEFPUSHBUTTON = 0x00000001, BS_AUTOCHECKBOX = 0x00000003,
        BS_CHECKBOX = 0x00000002, BS_RADIOBUTTON = 0x00000004, BS_3STATE = 0x00000005,
        BS_AUTO3STATE = 0x00000006, BS_GROUPBOX = 0x00000007, BS_AUTORADIOBUTTON = 0x00000009,
        BS_LEFTTEXT = 0x00000020
    };
    enum EditStyle
    {
        ES_AUTOHSCROLL = 0x0080, ES_AUTOVSCROLL = 0x0040, ES_CENTER = 0x0001, ES_LEFT = 0x0000,
        ES_LOWERCASE = 0x0010, ES_MULTILINE = 0x0004, ES_NOHIDESEL = 0x0100, ES_OEMCONVERT = 0x0400,
        ES_PASSWORD = 0x0020, ES_READONLY = 0x0800, ES_RIGHT = 0x0002, ES_UPPERCASE = 0x0008
    };
    enum ComboboxStyle
    {
        CBS_AUTOHSCROLL = 0x0040, CBS_DROPDOWN = 0x0003, CBS_DROPDOWNLIST = 0x0003,
        CBS_OEMCONVERT = 0x0080, CBS_SIMPLE = 0x0001, CBS_SORT = 0x0100
    };
    enum ListboxStyle
    {
        LBS_DISABLENOSCROLL = 0x1000, LBS_EXTENDEDSEL = 0x0800, LBS_MULTICOLUMN = 0x0200, LBS_MULTIPLESEL = 0x0008,
        LBS_NOINTEGRALHEIGHT = 0x0100, LBS_NOTIFY = 0x0001, LBS_SORT = 0x0002, LBS_USETABSTOPS = 0x0080,
        LBS_WANTKEYBOARDINPUT = 0x0400,
        LBS_STANDARD = LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER
    };
    enum ScrollbarStyle
    {
        SBS_VERT = 0x0001, SBS_HORZ = 0x0000, SBS_BOTTOMALIGN = 0x0004, SBS_LEFTALIGN = 0x0002, SBS_RIGHTALIGN = 0x0004,
        SBS_SIZEBOX = 0x0008, SBS_SIZEBOXBOTTOMRIGHTALIGN = 0x0004, SBS_SIZEBOXTOPLEFTALIGN = 0x0002, SBS_TOPALIGN = 0x0002
    };

    enum ClassType { Undefined, Button, CheckBox, RadioButton, GroupBox, Edit, TextEdit, Combobox, Listbox, Scrollbar };

public:
    static ControlItem2d* create(Space* space, const QString& className, int style,
                                 const QString& text, const QSizeF& size, int id);
    ControlItem2d(const ControlItem2d& other);

    int style() const { return m_style; }
    void setStyle(int style) { m_style = style; }

    ClassType classType() const { return m_classType; }

    QString text() const { return m_text; }
    void setText(const QString& text) { m_text = text; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    StData::SpaceItem* deepCopy();

    void accept(SpaceItemVisitorBase* visitor);

private:
    ControlItem2d(Space* space, ClassType classType,
                  int style, const QString& text, const QSizeF& size, int id);

private:
    ClassType m_classType;
    int m_style;
    QString m_text;
    bool m_enabled;
    int m_id;
};

}

#endif // SPACEITEM_H


