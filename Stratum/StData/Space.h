#ifndef SPACE2D_H
#define SPACE2D_H

#include "HandleCollection.h"
#include "stdata_global.h"
//#include "HyperBase.h"
#include "StDataTypes.h"

#include <QBitArray>
#include <QDir>
#include <QHash>
#include <QMap>
#include <QObject>
#include <QPointF>

#include <memory>
#include <functional>

namespace StData {

typedef std::shared_ptr<SpaceHyperKey> SpaceHyperKeyPtr;

class STDATASHARED_EXPORT Space : public QObject
{
    Q_OBJECT
public:
    enum ObjectType { Group2d = 3, RGroup2d = 4, Line2d = 20, Bitmap2d = 21, DoubleBitmap2d = 22, Text2d = 23};

public:
    Space();
    ~Space();

    QDir spaceDir() const { return m_dir; }
    QString fileName() const { return m_fileName; }
    Scheme* scheme() const { return m_scheme; }
    Class* ownerClass() const;

    HandleCollection<SpaceItem> spaceItems() const {return m_items;}
//    SpaceItem* spaceItemByName(const QString& name);
//    QList<SpaceItem*> spaceItemsByName(const QString& name);

    QPointF org() const {return m_org;}
    void setOrg(const QPointF& org) {m_org = org;}

    QPointF scaleDiv() const {return m_scaleDiv;}
    void setScaleDiv(const QPointF& scaleDiv) {m_scaleDiv = scaleDiv;}

    QPointF scaleMul() const {return m_scaleMul;}
    void setScaleMul(const QPointF& scaleMul) {m_scaleMul = scaleMul;}

    double scale() const { return m_scale; }
    void setScale(double scale) { m_scale = scale; }

    quint32 layers() const {return m_layers;}
    void setLayers(const quint32& layers);
    bool isLayerVisible(int layerIndex);

    qint16 state() const {return m_state;}
    void setState(const qint16& state) {m_state = state;}

    qint16 defaultFlags() const {return m_defaultFlags;}
    void setDefaultFlags(const qint16& defaultFlags) {m_defaultFlags = defaultFlags;}

    SpaceHyperKeyPtr hyperKey() const { return m_hyperKey; }

    HandleCollection<PenTool> pens() const {return m_pens;}
    HandleCollection<BrushTool> brushes() const {return m_brushes;}
    HandleCollection<TextureTool> pixmaps() const {return m_textures;}
    HandleCollection<TextureTool> maskedPixmaps() const {return m_maskedTextures;}
    HandleCollection<FontTool> fonts() const {return m_fonts;}
    HandleCollection<StringTool> strings() const {return m_strings;}
    HandleCollection<RasterTextTool> texts() const {return m_rasterTexts;}

    PenTool* getPen(int handle) const {return m_pens.getItem(handle);}
    BrushTool* getBrush(int handle) const {return m_brushes.getItem(handle);}
    TextureTool* getTexture(int handle) const {return m_textures.getItem(handle);}
    TextureTool* getMaskedTexture(int handle) const {return m_maskedTextures.getItem(handle);}
    FontTool* getFont(int handle) const {return m_fonts.getItem(handle);}
    StringTool* getString(int handle) const {return m_strings.getItem(handle);}
//    TextFragmentTool* getText(int handle) const {return m_texts.getItem(handle);}
    RasterTextTool* getRasterText(int handle) const {return m_rasterTexts.getItem(handle);}

    BrushTool* getBkBrush() const {return m_brushes.getItem(m_brushHandle);}

    SpaceItem* getSpaceItem(int handle) { return m_items.getItem(handle); }
    void addItem(SpaceItem* spaceItem);

    QList<SpaceItem*> zOrderList() const {return m_zOrderList;}
    int getHandleByZOrder(int zOrderIndex) const;
    int getZOrderIndex(SpaceItem* item) const;
    int getZOrderIndexByHandle(int handle);

    void deleteObject(int handle);
    int getObjectType(int handle);

    int getObjectAttribute2d(int objectHandle);
    bool setObjectAttribute(int objectHandle, int attrMode, int attrValue);

    SpaceItem* getItemFromPoint2d(double x, double y, std::function<bool(int)> predicate = nullptr);

    //Z-Order
    bool objectToTop(int objectHandle);
    bool objectToBottom(int objectHandle);
    bool setObjectZOrder(int objectHandle, int zOrder);
    bool swapObjectsZOrder(int objectHandle1, int objectHandle2);

    //Tools
    int createPen(qint16 rop, int rgba, int width, int style);
    int createBrush(int style, int hatch, int rgba, int dibHandle, qint16 rop);
    int createFont(const QFont& font, bool sizeInPixels);
    int createString(const QString& data);
    int createText(int fontHandle, int stringHandle, const QColor& bgColor, const QColor& fgColor);
    int createDib(const QString& absoluteFileName);
    int createDib(const QPixmap& pixmap, bool autoConvert = true);
    int createRDib(QString fileName);
    int createDoubleDib(const QString& absoluteFileName);
    int createDoubleDib(const QPixmap& pixmap);
    int createDoubleRDib(QString fileName);

    bool insertText(int rasterTextHandle, int index, int fontHandle, int stringHandle, const QColor& bgColor, const QColor& fgColor);
    bool removeText(int rasterTextHandle, int index);

    PolylineItem2d* createPolyline(int penHandle, int brushHandle, const QPolygonF& polyline);
    TextItem2d* createRasterText(int textHandle, double x, double y, double angle);
    TextureItem2d* createPixmap(int dibHandle, double x, double y);
    MaskedTextureItem2d* createMaskedPixmap(int doublDibHandle, double x, double y);
    GroupItem* createGroup(const QVector<int>& objectHandles);
    ControlItem2d* createControl(const QString& className, const QString& text,
                                 int style, int x, int y, int width, int height, int id);

    int getDibPixelRgba(int dibHandle, int x, int y);
    bool setDibPixelColor(int dibHandle, int x, int y, int rgba);

    bool deleteGroup(int handle);
    bool addItemToGroup(int groupHandle, int itemHandle);
    bool deleteGroupItem(int groupHandle, int itemHandle);
    int getGroupItem(int groupHandle, int index);
    int getGroupItemsCount(int groupHandle);

    int getNextItemHandle(int handle);

    QList<SpaceItem*> copyItems();
    void pasteItems(Space* sourceSpace, const QList<SpaceItem*>& items, int startZOrder = -1);
    void pasteItem(SpaceItem* spaceItem, const QList<Tool*>& tools);
    void pasteItem(SpaceItem* spaceItem);
    void addItemTools(SpaceItem* spaceItem);

    bool deleteTool(int toolHandle, int toolType);
    int getToolRef2d(int toolHandle, int toolType);
    int getNextTool(int toolHandle, int toolType);

public:
    static Space* createFromScheme(Scheme* scheme);
    static Space* createFromFile(const QString& fileName);

    static QList<SpaceItem*> toTree(const QList<SpaceItem*>& linearList);
    static void toLinear(const QList<SpaceItem*>& treeList, QList<SpaceItem*>* linearList);

private:
    Space(Scheme* scheme);
    Space(const QString& fileName);

    bool load(St2000DataStream& stream);

    void setupTools(SpaceLoader* spaceLoader);
    void setupItems(SpaceLoader* spaceLoader, QMap<GroupItemResult*, GroupItem*>* groupForResult);
    void setupGroups(QMap<GroupItemResult*, GroupItem*> groupForResult);
    void setupHyperBase(SpaceLoader* spaceLoader);

    void controlsToTop();

private:
    Scheme* m_scheme;
    QString m_fileName;
    QDir m_dir;

    QPointF m_org;
    QPointF m_scaleDiv;
    QPointF m_scaleMul;
    double m_scale;

    qint16 m_state;
    qint16 m_brushHandle;
    quint32 m_layers;
    QBitArray m_layerBits;
    qint16 m_defaultFlags;

    //TODO: Extract tool collections and functions to ToolManager class
    HandleCollection<PenTool> m_pens;
    HandleCollection<BrushTool> m_brushes;
    HandleCollection<TextureTool> m_textures;
    HandleCollection<TextureTool> m_maskedTextures;
    HandleCollection<FontTool> m_fonts;
    HandleCollection<StringTool> m_strings;
//    HandleCollection<TextFragmentTool> m_texts;
    HandleCollection<RasterTextTool> m_rasterTexts;

    HandleCollection<SpaceItem> m_items;
    QList<SpaceItem*> m_zOrderList;
    QList<SpaceItem*> m_controlItems;

    SpaceHyperKeyPtr m_hyperKey;
};

}

#endif // SPACE2D_H
