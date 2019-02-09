#include "Class.h"
#include "PolylineArrow.h"
#include "Space.h"
#include "SpaceItem.h"
#include "Tool2d.h"
#include "SpaceItemVisitorBase.h"

#include <StDataGlobal.h>

namespace StData {

SpaceItem::SpaceItem(Space* space) :
    QObject(),
    HandleItem(0),
    m_space(space),
    m_options(0),
    m_layer(0),
    m_visible(true),
    m_spaceType(Type_Udefined),
    m_ownerItem(nullptr),
    //    m_zOrder(-2),./
    m_opacity(1.0),
    m_loadingType(0)
{}

SpaceItem::SpaceItem(const SpaceItem& other) :
    QObject(),
    HandleItem(other),
    m_space(other.space()),
    m_options(other.options()),
    m_layer(other.layer()),
    m_visible(other.m_visible),
    m_spaceType(other.m_spaceType),
    m_ownerItem(other.ownerItem()),
    m_opacity(other.opacity()),
    m_loadingType(other.loadingType()),
    m_name(other.name())
{}

void SpaceItem::setOptions(qint16 options)
{
    m_options = options;

    m_layer = ((m_options & 0x1f00) >> 8);
    m_visible = ((m_options & 1) == 0);
}

void SpaceItem::setOptions(SpaceItem::OptionsMode mode, qint16 options)
{
    switch(mode)
    {
        case Put:
            m_options=options;
            break;

        case Set:
            m_options |= options;
            break;

        case Reset:
            m_options &= (qint16)(~options);
            break;
    }

    bool isSel = isSelectable();
    m_layer = ((m_options & 0x1f00) >> 8);
    m_visible = ((m_options & 1) == 0);
}

bool SpaceItem::isItemVisible()
{
    return m_visible;
}

bool SpaceItem::isLayerVisible()
{
    return m_space->isLayerVisible(m_layer);
}

bool SpaceItem::isVisible()
{
    return m_visible && m_space->isLayerVisible(m_layer);
}

bool SpaceItem::isSelectable() const
{
    bool isHardDontSelect = (m_options & SpaceItem::HardDontSelect);
    bool isDontSelect = (m_options & SpaceItem::DontSelect);
    bool isSchemaAllowUnselected = (m_space->state() & 8) != 0;

    if(isHardDontSelect)
        return false;

    if(isSchemaAllowUnselected)
        return true;

    return !isDontSelect;
}

int SpaceItem::zOrder()
{
    return m_space->zOrderList().indexOf(this) + 1;
}

SpaceItem::Type SpaceItem::typeOnSpace()
{
    if(m_spaceType == Type_Udefined)
    {
        m_spaceType = Type_Graphics;
        if(Class* spaceClass = m_space->ownerClass())
        {
            if(spaceClass->isLinkExist(handle()))
                m_spaceType = Type_Link;

            else if(spaceClass->isObjectInfoExist(handle()))
                m_spaceType = Type_Object;
        }
    }
    return m_spaceType;
}

void SpaceItem::setTypeOnSpace(SpaceItem::Type typeOnSpace)
{
    m_spaceType = typeOnSpace;
}

SpaceItemHyperKeyPtr SpaceItem::actualHyperKey() const
{
    SpaceItemHyperKeyPtr result = m_hyperKey;

    SpaceItem* parentItem = ownerItem();
    while(parentItem)
    {
        if(parentItem->hyperKey())
            result = parentItem->hyperKey();

        parentItem = parentItem->ownerItem();
    }

    return result;
}

void SpaceItem::invokeHyperJump()
{
    if(actualHyperKey())
        emit hyperJumpInvoked();
}

GroupItem* SpaceItem::ownerItem() const
{
    return m_ownerItem;
}

SpaceItem* SpaceItem::rootOwnerItem()
{
    SpaceItem* result = nullptr;
    SpaceItem* item = this;

    while(item->ownerItem())
    {
        result = item->ownerItem();
        item = item->ownerItem();
    }
    return result;
}

void SpaceItem::setOwnerItem(GroupItem* ownerItem)
{
    m_ownerItem = ownerItem;
}

bool SpaceItem::isChildFor(SpaceItem* parentItem)
{
    if(!parentItem)
        return false;

    SpaceItem* owner = ownerItem();
    while(owner)
    {
        if(owner == parentItem)
            return true;

        owner = owner->ownerItem();
    }
    return false;
}

SpaceItem2d::SpaceItem2d(Space* space2d) :
    SpaceItem(space2d),
    m_angle(0.0)
{}

SpaceItem2d::SpaceItem2d(const SpaceItem2d& other) :
    SpaceItem(other),
    m_angle(other.angle()),
    m_size(other.size()),
    m_origin(other.origin())
{}

QRectF SpaceItem2d::rect() const
{
    return QRectF(origin(), size());
}

bool SpaceItem2d::containsPoint(double x, double y) const
{
    return (x >= m_origin.x() && x <= (m_origin.x() + m_size.width())) &&
            (y >= m_origin.y() && y <= (m_origin.y() + m_size.height()));
}

//bool SpaceItem2d::getDistanceToPoint(const QPointF& point)
//{
//    if(m_angle == 0.0)
//    {
//        if(m_size.width() == 0.0 || m_size.height() == 0.0)
//            return MAX_DISTANCE;

//        if(containsPoint(point.x(), point.y()))
//            return 0;

//        int x1 = point.x() - (m_origin.x() + m_size.width());
//        int y1 = point.y() - (m_origin.y() + m_size.height());
//        int x2 = point.x() - origin.x();
//        int y2 = point.y() - origin.y();

//        x1 = (x1 < 0) ? MAX_DISTANCE : x1;
//        x2 = (x2 < 0) ? MAX_DISTANCE : x2;

//        y1 = (y1 < 0) ? MAX_DISTANCE : y1;
//        y2 = (y2 < 0) ? MAX_DISTANCE : y2;

//        x1 = qMin(x1, x2);
//        y1 = qMin(y1, y2);
//        x1 = qMax(x1, y1);
//        return x1;
//    }

//    QTransform transform = QTransform().rotateRadians(m_angle);
//    QPolygonF polygon = transform.mapToPolygon(QRectF(m_origin, m_size));
//    return polygon.contains(point) ? 0 : MAX_DISTANCE;
//}

/*----------------------------------------------------------------------------------------------------------*/

ShapeItem2d::ShapeItem2d(Space* space2d) :
    SpaceItem2d(space2d),
    m_penTool(nullptr),
    m_brushTool(nullptr),
    m_penHandle(0),
    m_brushHandle(0)
{}

ShapeItem2d::ShapeItem2d(const ShapeItem2d& other) :
    SpaceItem2d(other),
    m_penTool(nullptr),
    m_brushTool(nullptr),
    m_penHandle(0),
    m_brushHandle(0)
{
    if(other.penTool())
    {
        m_penTool = new PenTool(*other.penTool());
        m_penHandle = m_penTool->handle();
    }

    if(other.brushTool())
    {
        m_brushTool = new BrushTool(*other.brushTool());
        m_brushHandle = m_brushTool->handle();
    }
}

ShapeItem2d::~ShapeItem2d()
{
    if(m_penTool)
        m_penTool->decRef();

    if(m_brushTool)
        m_brushTool->decRef();
}

void ShapeItem2d::setPenTool(PenTool* penTool)
{
    if(penTool != m_penTool)
    {
        if(m_penTool)
            m_penTool->decRef();

        m_penTool = penTool;
        m_penHandle = 0;

        if(m_penTool)
        {
            m_penHandle = m_penTool->handle();
            m_penTool->incRef();
        }
    }
}

void ShapeItem2d::setBrushTool(BrushTool* brushTool)
{
    if(brushTool != m_brushTool)
    {
        if(m_brushTool)
            m_brushTool->decRef();

        m_brushTool = brushTool;
        m_brushHandle = 0;

        if(m_brushTool)
        {
            m_brushHandle = brushTool->handle();
            m_brushTool->incRef();
        }
    }
}

QPen ShapeItem2d::getPen()
{
    if(m_penTool)
        return m_penTool->pen();

    return QPen(Qt::NoPen);
}

//void ShapeItem2d::setPenTool(const QPen& pen)
//{
//    int rgba = colorToInt(pen.color());
//    double width = pen.widthF();
//    int style = PenTool::toStPenStyle(pen.style());

//    m_penTool->setData(rgba, width, style, m_penTool->rop());
//}

QBrush ShapeItem2d::getBrush()
{
    if(m_brushTool)
        return m_brushTool->brush();

    return QBrush(Qt::NoBrush);
}

void ShapeItem2d::setBrushData(const QBrush& brush, TextureTool* texture)
{
    if(m_brushTool)
    {
        int rgba = colorToInt(brush.color());
        int style = BrushTool::toStBrushStyle(brush.style());
        int hatch = BrushTool::toStHatchPattern(brush.style());
        int rop = m_brushTool->rop();
        m_brushTool->setData(rgba, style, hatch, rop, texture);
    }
}


/*----------------------------------------------------------------------------------------------------------*/
PolylineItem2d::PolylineItem2d(Space* space2d) :
    ShapeItem2d(space2d),
    m_fillRule(1)
{}

PolylineItem2d::PolylineItem2d(const PolylineItem2d& other) :
    ShapeItem2d(other),
    m_polyline(other.polyline())
{
    m_boundingRect = m_polyline.boundingRect();
}

void PolylineItem2d::setPolyline(const QPolygonF& polyline)
{
    m_polyline = polyline;
    m_boundingRect = m_polyline.boundingRect();
}

int PolylineItem2d::pointsCount()
{
    return m_polyline.count();
}

void PolylineItem2d::addPoint(const QPointF& point)
{
    m_polyline << point;
    correctBoundingRect(point);
}

void PolylineItem2d::insertPoint(int index, const QPointF& point)
{
    m_polyline.insert(index, point);
    correctBoundingRect(point);
}

QPointF PolylineItem2d::getPoint(int index) const
{
    return m_polyline.at(index);
}

void PolylineItem2d::setPoint(int index, const QPointF& point)
{
    m_polyline.replace(index, point);
    m_boundingRect = m_polyline.boundingRect();
}

void PolylineItem2d::removePoint(int index)
{
    m_polyline.removeAt(index);
    m_boundingRect = m_polyline.boundingRect();
}

void PolylineItem2d::removePoint(const QPointF& point)
{
    m_polyline.removeOne(point);
    m_boundingRect = m_polyline.boundingRect();
}

void PolylineItem2d::correctBoundingRect(const QPointF& point)
{
    if(point.x() < m_boundingRect.left())
        m_boundingRect.setLeft(point.x());
    else if(point.x() > m_boundingRect.right())
        m_boundingRect.setRight(point.x());

    if(point.y() < m_boundingRect.top())
        m_boundingRect.setTop(point.y());
    else if(point.y() > m_boundingRect.bottom())
        m_boundingRect.setBottom(point.y());

}

bool PolylineItem2d::containsPoint(double x, double y) const
{
    double penWidth = penTool() ? penTool()->width() : 1.0;

    QRectF rectWidthPen = m_boundingRect.adjusted(-penWidth-5, -penWidth-5, penWidth+5, penWidth+5);
    if(pointInRect(QPointF(x, y), rectWidthPen))
    {
        double penWidth = penTool() ? penTool()->width() : 0.0;
        QRectF rect = QRectF(x - penWidth/2 - 2, y - penWidth/2 - 2, 2*penWidth + 4, 2*penWidth + 4);

        if(brushTool() || isPoint())
        {
            QPainterPath path;
            path.addPolygon(m_polyline);
            path.setFillRule(m_fillRule == 1 ? Qt::OddEvenFill : Qt::WindingFill);
            return path.intersects(rect);
        }
        else
        {
            for(int i = 0; i < m_polyline.count() - 1; i++)
            {
                QPolygonF line;
                line << m_polyline.at(i);
                line << m_polyline.at(i+1);

                QPainterPath path;
                path.addPolygon(line);
                if(path.intersects(rect))
                    return true;
            }
        }
    }
    return false;
}

//int PolylineItem2d::getDistanceToPoint(const QPointF& p) const
//{
//    double d = 5;
//    if(penTool())
//        d += penTool()->width() / 2;

//    if( ((origin().x() - d) <= p.x()) &&
//        ((origin().y() - d) <= p.y()) &&
//        ((origin().x() + size().width() + d) >= p.x) &&
//        ((origin().y() + size().y() + d) >= p.y))
//    {
//    }
//}

SpaceItem* PolylineItem2d::deepCopy()
{
    return new PolylineItem2d(*this);
}

void PolylineItem2d::accept(SpaceItemVisitorBase* visitor)
{
    visitor->visit(this);
}

QPointF PolylineItem2d::origin() const
{
    return m_polyline.boundingRect().topLeft();
}

void PolylineItem2d::setOrigin(const QPointF& org)
{
    QPointF delta = org - origin();
    m_polyline.translate(delta);
    m_boundingRect.translate(delta);
}

QSizeF PolylineItem2d::size() const
{
    return m_polyline.boundingRect().size();
}

void PolylineItem2d::setSize(const QSizeF& newSize)
{
    if(size() == newSize)
        return;

    QPointF org = origin();
    double kX = size().width() == 0 ? 0 : newSize.width() / size().width();
    double kY = size().height() == 0 ? 0 : newSize.height() / size().height();

    for(int i = 0; i < m_polyline.count(); i++)
    {
        double x = org.x() + kX*(m_polyline[i].x() - org.x());
        double y = org.y() + kY*(m_polyline[i].y() - org.y());

        m_polyline[i].setX(x);
        m_polyline[i].setY(y);
    }
    m_boundingRect.setSize(newSize);
}

QRectF PolylineItem2d::rect() const
{
    QRectF rect = SpaceItem2d::rect();
    if(rect.width() > 0 && rect.height() > 0)
       return rect;

    if(PenTool* pTool = penTool())
    {
        double penWidth = pTool->pen().widthF();
        rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
        return rect;
    }
    else if(isPoint())
    {
        rect.adjust(0, 0, 1, 1);
        return rect;
    }
    return rect;
}

bool PolylineItem2d::pointInRect(const QPointF& p, const QRectF& rect) const
{
    return (p.x() >= rect.left() && p.x() <= rect.right()) &&
            (p.y() >= rect.top() && p.y() <= rect.bottom());
}

bool PolylineItem2d::isPoint() const
{
    if(m_polyline.isEmpty())
        return false;

    QPointF firstPoint = m_polyline.first();
    for(int i = 1; i < m_polyline.count(); i++)
        if(firstPoint != m_polyline.at(i))
            return false;

    return true;
}

bool PolylineItem2d::isClosed() const
{
    return m_polyline.isClosed();
}

void PolylineItem2d::setLineArrows(double aa, double al, double as, double ba, double bl, double bs)
{
    m_arrow = std::make_shared<PolylineArrow>();

    m_arrow->setEndAngle(aa);
    m_arrow->setEndLength(al);

    m_arrow->setStartAngle(ba);
    m_arrow->setStartLength(bl);

    int flag = 0;
    if(as)
        flag |= 2;

    if(bs)
        flag |= 1;

    m_arrow->setFlags(flag);
}

/*----------------------------------------------------------------------------------------------------------*/
GroupItem::GroupItem(Space* space) :
    SpaceItem2d(space)
{}

GroupItem::GroupItem(const GroupItem& other) :
    SpaceItem2d(other)
{}

void GroupItem::addItemHandle(int itemHandle)
{
    if(SpaceItem* item = space()->getSpaceItem(itemHandle))
        if(!m_items.contains(item))
        {
            item->setOwnerItem(this);
            m_items << item;
        }
}

void GroupItem::addItemHandles(QList<int> itemHandles)
{
    for(int itemHandle : itemHandles)
        addItemHandle(itemHandle);
}

void GroupItem::removeItemHandle(int itemHandle)
{
    if(SpaceItem* item = space()->getSpaceItem(itemHandle))
        m_items.removeOne(item);
}

int GroupItem::itemsCount()
{
    return m_items.count();
}

SpaceItem*GroupItem::itemAt(int index)
{
    return m_items.at(index);
}

int GroupItem::indexOf(SpaceItem* item)
{
    return m_items.indexOf(item);
}

void GroupItem::append(SpaceItem* item)
{
    if(item)
    {
        item->setOwnerItem(this);
        m_items << item;
    }
}

void GroupItem::append(QList<SpaceItem*> items)
{
    for(SpaceItem* item : items)
        append(item);
}

void GroupItem::insertAt(int index, SpaceItem* item)
{
    if(index < 0 || index > m_items.count())
        return;

    if(item)
    {
        item->setOwnerItem(this);
        m_items.insert(index, item);
    }
}

void GroupItem::remove(SpaceItem* item)
{
    item->setOwnerItem(ownerItem());
    m_items.removeOne(item);
}

void GroupItem::removeAt(int index)
{
    auto item = m_items.takeAt(index);
    item->setOwnerItem(ownerItem());
}

SpaceItem* GroupItem::takeAt(int index)
{
    auto item = m_items.takeAt(index);
    item->setOwnerItem(ownerItem());

    return item;
}

void GroupItem::clear()
{
    for(SpaceItem* item : m_items)
        remove(item);
}

void GroupItem::prepareToRemoving()
{
    for(SpaceItem* item : m_items)
        item->setOwnerItem(nullptr);

    if(GroupItem* parentGroup = ownerItem())
        parentGroup->remove(this);
}

bool GroupItem::contains(SpaceItem* item)
{
    return m_items.contains(item);
}

bool GroupItem::containsHandle(int itemHandle)
{
    for(SpaceItem* item : m_items)
        if(item->handle() == itemHandle)
            return true;

    return false;
}

bool GroupItem::isSelfOrAncestor(SpaceItem* ancestorItem)
{
    if(!ancestorItem)
        return false;

    SpaceItem* item = this;
    while(item)
    {
        if(item == ancestorItem)
            return true;

        item = item->ownerItem();
    }

    return false;
}

QRectF GroupItem::calculateGroupRect() const
{
    QRectF groupRect;
    for(SpaceItem* spaceItem : m_items)
    {
        QRectF itemRect;

        if(PolylineItem2d* item2d = dynamic_cast<PolylineItem2d*>(spaceItem))
            itemRect = item2d->polyline().boundingRect();

        else if(SpaceItem2d* item2d = dynamic_cast<SpaceItem2d*>(spaceItem))
            itemRect = item2d->rect();

        else if(GroupItem* groupItem = dynamic_cast<GroupItem*>(spaceItem))
            itemRect = groupItem->calculateGroupRect();

        if(itemRect.top() || itemRect.left() || itemRect.bottom() || itemRect.right())
            groupRect |= itemRect;
    }
    return groupRect;
}

SpaceItem* GroupItem::deepCopy()
{
    GroupItem* groupItem = new GroupItem(*this);

    for(SpaceItem* item : m_items)
        groupItem->append(item->deepCopy());

    return groupItem;
}

void GroupItem::setSpace(Space* space)
{
    SpaceItem::setSpace(space);

    for(SpaceItem* item : m_items)
        item->setSpace(space);
}

void GroupItem::accept(SpaceItemVisitorBase* visitor)
{
    visitor->visit(this);
}

QPointF GroupItem::origin() const
{
    return calculateGroupRect().topLeft();
}

void GroupItem::setOrigin(const QPointF& newOrg)
{
    QPointF oldOrg = origin();
    for(SpaceItem* item : m_items)
    {
        SpaceItem2d* item2d = static_cast<SpaceItem2d*>(item);
        QPointF itemOrg = item2d->origin();
        QPointF org = newOrg + (itemOrg - oldOrg);
        item2d->setOrigin(org);
    }
}

QSizeF GroupItem::size() const
{
    return calculateGroupRect().size();
}

void GroupItem::setSize(const QSizeF& newSize)
{
    QPointF oldGroupOrg = origin();
    QSizeF oldGroupSize = size();

    if(oldGroupSize == newSize)
        return;

    double kX = oldGroupSize.width() == 0 ? 0 : (newSize.width() / oldGroupSize.width());
    double kY = oldGroupSize.height() == 0 ? 0 : (newSize.height() / oldGroupSize.height());

    for(SpaceItem* item : m_items)
    {
        SpaceItem2d* item2d = static_cast<SpaceItem2d*>(item);

        double x = oldGroupOrg.x() + kX*(item2d->origin().x() - oldGroupOrg.x());
        double y = oldGroupOrg.y() + kY*(item2d->origin().y() - oldGroupOrg.y());

        double w = item2d->size().width()*kX;
        double h = item2d->size().height()*kY;

        item2d->setOrigin(QPointF(x, y));
        item2d->setSize(QSizeF(w, h));
    }
}

/*----------------------------------------------------------------------------------------------------------*/
TextItem2d::TextItem2d(Space* space2d) :
    SpaceItem2d(space2d),
    m_rasterText(nullptr),
    m_textAngle(0.0),
    m_rasterTextHandle(0)
{}

TextItem2d::TextItem2d(const TextItem2d& other) :
    SpaceItem2d(other),
    m_rasterText(new RasterTextTool(*other.rasterText())),
    m_textAngle(other.textAngle()),
    m_rasterTextHandle(0)
{
    if(m_rasterText)
        m_rasterTextHandle = m_rasterText->handle();
}

TextItem2d::~TextItem2d()
{
    if(m_rasterText)
        m_rasterText->decRef();
}

void TextItem2d::setRasterText(RasterTextTool* rasterText)
{
    if(rasterText != m_rasterText)
    {
        if(m_rasterText)
            m_rasterText->decRef();

        m_rasterText = rasterText;
        m_rasterTextHandle = 0;

        if(m_rasterText)
        {
            m_rasterTextHandle = m_rasterText->handle();
            m_rasterText->incRef();
        }
    }
}

SpaceItem* TextItem2d::deepCopy()
{
    return new TextItem2d(*this);
}

void TextItem2d::accept(SpaceItemVisitorBase* visitor)
{
    visitor->visit(this);
}


/*----------------------------------------------------------------------------------------------------------*/
TextureItem2d::TextureItem2d(Space* space2d) :
    SpaceItem2d(space2d),
    m_textureTool(nullptr),
    m_textureHandle(0)
{}

TextureItem2d::TextureItem2d(const TextureItem2d& other) :
    SpaceItem2d(other),
    m_textureTool(new TextureTool(*other.texture())),
    m_textureOrigin(other.textureOrigin()),
    m_textureSize(other.textureSize()),
    m_textureHandle(0)
{
    if(m_textureTool)
        m_textureHandle = m_textureTool->handle();
}

TextureItem2d::~TextureItem2d()
{
    if(m_textureTool)
        m_textureTool->decRef();
}

void TextureItem2d::setTexture(TextureTool* textureTool)
{
    if(textureTool != m_textureTool)
    {
        if(m_textureTool)
            m_textureTool->decRef();

        m_textureTool = textureTool;
        m_textureHandle = 0;

        if(m_textureTool)
        {
            m_textureHandle = m_textureTool->handle();
            m_textureTool->incRef();
        }
    }
}

QRectF TextureItem2d::offsetRect() const
{
    return QRectF(m_textureOrigin, m_textureSize);
}

SpaceItem* TextureItem2d::deepCopy()
{
    return new TextureItem2d(*this);
}

void TextureItem2d::accept(SpaceItemVisitorBase* visitor)
{
    visitor->visit(this);
}

MaskedTextureItem2d::MaskedTextureItem2d(const MaskedTextureItem2d& other) :
    TextureItem2d(other)
{}

SpaceItem* MaskedTextureItem2d::deepCopy()
{
    return new MaskedTextureItem2d(*this);
}

/*------------------------------------------------------------------------------------------------------------*/
ControlItem2d* ControlItem2d::create(Space* space, const QString& className, int style,
                                     const QString& text, const QSizeF& size, int id)
{
    ClassType classType = Undefined;

    if(className.compare("Button", Qt::CaseInsensitive) == 0)
    {
        if((style & BS_GROUPBOX) == BS_GROUPBOX)
            classType = GroupBox;

        else if(((style & BS_RADIOBUTTON) == BS_RADIOBUTTON) || ((style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON))
            classType = RadioButton;

        else if(((style & BS_CHECKBOX) == BS_CHECKBOX) || ((style & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX))
            classType = CheckBox;

        else
            classType = Button;
    }

    else if(className.compare("Edit", Qt::CaseInsensitive) == 0)
    {
        //Костыль: баг qt QTextEdit(QTextPlainEdit) не могуть быть меньше 70 по высоте
        if(((style & ES_MULTILINE) == ES_MULTILINE) && size.height() >= 70)
            classType = TextEdit;
        else
            classType = Edit;
    }

    else if(className.compare("Combobox", Qt::CaseInsensitive) == 0)
        classType = Combobox;

    else if(className.compare("Listbox", Qt::CaseInsensitive) == 0)
        classType = Listbox;

    else if(className.compare("Scrollbar", Qt::CaseInsensitive) == 0)
        classType = Scrollbar;


    if(classType != Undefined)
        return new ControlItem2d(space, classType, style, text, size, id);

    return nullptr;
}

ControlItem2d::ControlItem2d(const ControlItem2d& other) :
    SpaceItem2d(other),
    m_classType(other.classType()),
    m_style(other.style()),
    m_text(other.text()),
    m_enabled(other.isEnabled()),
    m_id(other.id())
{}

SpaceItem* ControlItem2d::deepCopy()
{
    return new ControlItem2d(*this);
}

void ControlItem2d::accept(SpaceItemVisitorBase* visitor)
{
    visitor->visit(this);
}

ControlItem2d::ControlItem2d(Space* space, ClassType classType, int style, const QString& text,
                             const QSizeF& size, int id) :
    SpaceItem2d(space),
    m_classType(classType),
    m_style(style),
    m_text(text),
    m_enabled(true),
    m_id(id)
{
    setSize(size);
}


SpaceItem* UserObject::deepCopy()
{
    return new UserObject(*this);
}

void UserObject::accept(SpaceItemVisitorBase* visitor)
{
    visitor->visit(this);
}

}
