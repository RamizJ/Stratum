#include "EditPolylineDialog.h"
#include "ui_EditPolylineDialog.h"

#include <PolylineGraphicsItem.h>

#include <SpaceItem.h>
#include <Tool2d.h>

using namespace StData;
using namespace StSpace;

EditPolylineDialog::EditPolylineDialog(PolylineGraphicsItem* item, QWidget* parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditPolylineDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Свойства полилинии"));

    setPolylineGraphicsItem(item);
}

EditPolylineDialog::~EditPolylineDialog()
{
    delete ui;
}

void EditPolylineDialog::setPolylineGraphicsItem(PolylineGraphicsItem* polylineGraphicsItem)
{
    if(m_polylineGraphicsItem == polylineGraphicsItem)
        return;

    m_polylineGraphicsItem = polylineGraphicsItem;
    PolylineItem2d* polylineItem = m_polylineGraphicsItem->polylineItem();

    ui->widgetEditLocation->setSpaceItemPosition(m_polylineGraphicsItem->pos());
    ui->widgetEditLocation->setSpaceItemSize(m_polylineGraphicsItem->boundingRect().size());
    ui->widgetEditLocation->setSpaceItemAngle(m_polylineGraphicsItem->rotation());
    ui->widgetEditLocation->setSpaceItemLayer(polylineItem->layer());

    ui->widgetEditPen->setPen(polylineItem->getPen());
    ui->widgetEditBrush->setBrush(polylineItem->getBrush());
}

void EditPolylineDialog::accept()
{
    m_polylineGraphicsItem->setPos(ui->widgetEditLocation->spaceItemPosition());
    m_polylineGraphicsItem->setRotation(ui->widgetEditLocation->spaceItemAngle());

//    PolylineItem2d* polylineItem = m_polylineGraphicsItem->polylineItem();
//    polylineItem->setPenTool(ui->widgetEditPen->pen());
//    polylineItem->setBrushData(ui->widgetEditBrush->brush());

//    m_polylineGraphicsItem->setPolylinePen(ui->widgetEditPen->pen());
//    m_polylineGraphicsItem->setPolylineBrush(ui->widgetEditBrush->brush());

    QDialog::accept();
}
