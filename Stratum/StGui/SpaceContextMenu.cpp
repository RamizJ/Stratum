#include "SpaceContextMenu.h"
#include "EditGroupDialog.h"
#include "EditPixmapDialog.h"
#include "EditPolylineDialog.h"
#include "EditRasterTextDialog.h"
#include "EditSchemeDialog.h"
#include "SpaceScene.h"
#include "EditObjectDialog.h"
#include "GuiContext.h"
#include "TabManager.h"
#include "EditLinkDialog.h"

#include <PixmapGraphicsItem.h>
#include <PolylineGraphicsItem.h>
#include <ObjectGraphicsItem.h>
#include <GroupGraphicsItem.h>
#include <RasterTextGraphicsItem.h>
#include <LinkGraphicsItem.h>

#include <Class.h>

#include <Object.h>
#include <QMenu>

using namespace StData;
using namespace StSpace;

SpaceContextMenu::SpaceContextMenu(Object* spaceOwnerObject, QObject* parent) :
    QObject(parent),
    m_spaceOwnerObject(spaceOwnerObject)
{
    m_zOrderMenu = new QMenu(tr("Z Порядок"));
    m_actionZOrderToFront = new QAction(tr("На верх"), m_zOrderMenu);
    m_actionZOrderToBack = new QAction(tr("Поместить назад"), m_zOrderMenu);
    m_actionZOrderStepFront = new QAction(tr("Приблизить на одну"), m_zOrderMenu);
    m_actionZOrderStepBack = new QAction(tr("Отодвинуть на одну"), m_zOrderMenu);

    m_zOrderMenu->addAction(m_actionZOrderToFront);
    m_zOrderMenu->addAction(m_actionZOrderToBack);
    m_zOrderMenu->addAction(m_actionZOrderStepFront);
    m_zOrderMenu->addAction(m_actionZOrderStepBack);

    m_actionCut = new QAction(tr("Вырезать"), this);
    m_actionCopy = new QAction(tr("Копировать"), this);
    m_actionPaste = new QAction(tr("Вставить"), this);

    m_clipboardActions << m_actionCut << m_actionCopy << m_actionPaste;
}

SpaceContextMenu::~SpaceContextMenu()
{
    delete m_zOrderMenu;
}

void SpaceContextMenu::openContextMenu(const QPoint& pos, QGraphicsItem* graphicsItem)
{
    SpaceScene* spaceScene = qobject_cast<SpaceScene*>(sender());
    if(spaceScene)
    {
        if(graphicsItem == nullptr)
            openSpaceContextMenu(pos, spaceScene);

        else if(auto linkItem = dynamic_cast<LinkGraphicsItem*>(graphicsItem))
            openLinkContextMenu(pos, linkItem);

        else if(auto objectItem = dynamic_cast<ObjectGraphicsItem*>(graphicsItem))
            openObjectContextMenu(pos, objectItem);

        else if(auto polylineItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
            openPolylineContextMenu(pos, polylineItem);

        else if(auto pixmapItem = dynamic_cast<PixmapGraphicsItem*>(graphicsItem))
            openPixmapContextMenu(pos, pixmapItem);

        else if(auto groupItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem))
            openGroupContextMenu(pos, groupItem);

        else if(auto textItem = dynamic_cast<RasterTextGraphicsItem*>(graphicsItem))
            openTextContextMenu(pos, textItem);
    }
}

void SpaceContextMenu::openSpaceContextMenu(const QPoint& pos, SpaceScene* spaceScene)
{
    QMenu menu;

    QAction* actionSchemaProperties = new QAction(tr("Параметры листа..."), &menu);
    QAction* actionObjectProperties = new QAction(tr("Свойства этого имиджа..."), &menu);
    QAction* actionPrint = new QAction(tr("Печать..."), &menu);
    QAction* actionInsertObjectFromLibrary = new QAction(tr("Вставить имидж из библиотеки..."), &menu);
    QAction* actionCreateObject = new QAction(tr("Создать новый имидж"), &menu);
    QAction* actionCreateConnectionArea = new QAction(tr("Создать контактную площадку"), &menu);
    QAction* actionPasteFromClipboard = new QAction(tr("Вставить"), &menu);
    QAction* actionPasteFromFile = new QAction(tr("Вставить из файла"), &menu);

    menu.addAction(actionSchemaProperties);
    menu.addAction(actionObjectProperties);
    menu.addAction(actionPrint);
    menu.addSeparator();
    menu.addAction(actionInsertObjectFromLibrary);
    menu.addAction(actionCreateObject);
    menu.addAction(actionCreateConnectionArea);
    menu.addSeparator();
    menu.addAction(actionPasteFromClipboard);
    menu.addAction(actionPasteFromFile);

    QAction* selectedAction = menu.exec(pos);

    if(selectedAction == actionSchemaProperties)
        EditSchemeDialog(spaceScene->space()).exec();
}

void SpaceContextMenu::openPolylineContextMenu(const QPoint& pos, PolylineGraphicsItem* item)
{
    QMenu menu;

    QAction* actionPolylineProperties = new QAction(QObject::tr("Свойства..."), &menu);
    QAction* actionEditPolyline = new QAction(QObject::tr("Редактировать точки"), &menu);

    menu.addAction(actionPolylineProperties);
    menu.addAction(actionEditPolyline);
    menu.addSeparator();
    menu.addMenu(m_zOrderMenu);
    menu.addSeparator();
    menu.addActions(m_clipboardActions);

    QAction* selectedAction = menu.exec(pos);
    if(selectedAction == actionPolylineProperties)
        EditPolylineDialog(item).exec();
}

void SpaceContextMenu::openPixmapContextMenu(const QPoint& pos, PixmapGraphicsItem* item)
{
    QMenu menu;

    QAction* actionProperties = new QAction(QObject::tr("Свойства изображения..."), &menu);
    QAction* actionSaveToFile = new QAction(QObject::tr("Записать в файл..."), &menu);

    menu.addAction(actionProperties);
    menu.addAction(actionSaveToFile);
    menu.addSeparator();
    menu.addMenu(m_zOrderMenu);
    menu.addSeparator();
    menu.addActions(m_clipboardActions);

    QAction* selectedAction = menu.exec(pos);

    if(selectedAction == actionProperties)
        EditPixmapDialog(item).exec();
}

void SpaceContextMenu::openTextContextMenu(const QPoint& pos, RasterTextGraphicsItem* item)
{
    QMenu menu;

    QAction* actionProperties = new QAction(QObject::tr("Свойства..."), &menu);

    menu.addAction(actionProperties);
    menu.addSeparator();
    menu.addMenu(m_zOrderMenu);
    menu.addSeparator();
    menu.addActions(m_clipboardActions);

    QAction* selectedAction = menu.exec(pos);

    if(selectedAction == actionProperties)
        EditRasterTextDialog(item).exec();
}

void SpaceContextMenu::openGroupContextMenu(const QPoint& pos, GroupGraphicsItem* item)
{
    QMenu menu;

    QAction* actionProperties = new QAction(QObject::tr("Свойства группы..."), &menu);
    QAction* actionUngroup = new QAction(QObject::tr("Разгруппировать"), &menu);

    menu.addAction(actionProperties);
    menu.addAction(actionUngroup);
    menu.addSeparator();
    menu.addActions(m_clipboardActions);

    QAction* selectedAction = menu.exec(pos);

    if(selectedAction == actionProperties)
        EditGroupDialog(item).exec();
}

void SpaceContextMenu::openObjectContextMenu(const QPoint& pos, ObjectGraphicsItem* item)
{
    QMenu menu;

    QAction* actionProperties = new QAction(QObject::tr("Свойства имиджа..."), &menu);
    QAction* actionEditText = new QAction(QObject::tr("Редактировать текст..."), &menu);
    QAction* actionEditSchema = new QAction(QObject::tr("Редактировать схему..."), &menu);
    QAction* actionEditImage = new QAction(QObject::tr("Редактировать изображение..."), &menu);

    Object* object = m_spaceOwnerObject->childByHandle(item->objectHandle());

    actionEditSchema->setEnabled(object->cls()->scheme() != nullptr);

    menu.addAction(actionProperties);
    menu.addAction(actionEditText);
    menu.addAction(actionEditSchema);
    menu.addAction(actionEditImage);
    menu.addSeparator();
    menu.addActions(m_clipboardActions);

    QAction* selectedAction = menu.exec(pos);

    if(selectedAction == actionProperties)
    {
        EditObjectDialog(object).exec();
    }

    else if(selectedAction == actionEditSchema)
    {
        if(object->cls()->scheme())
            GuiContext::instance().tabManager()->openSpace(object);
    }

    else if(selectedAction == actionEditText)
        GuiContext::instance().tabManager()->openSourceCode(object->cls());

    else if(selectedAction == actionEditImage)
    {}
}

void SpaceContextMenu::openLinkContextMenu(const QPoint& pos, LinkGraphicsItem* item)
{
    QMenu contextMenu;

    QAction* actionProperties = new QAction(QObject::tr("Свойства связи..."), &contextMenu);
    QAction* actionRemoveLink = new QAction(QObject::tr("Удалить эту связь"), &contextMenu);

    contextMenu.addAction(actionProperties);
    contextMenu.addAction(actionRemoveLink);
    contextMenu.addSeparator();
    contextMenu.addActions(m_clipboardActions);

    QAction* selectedAction = contextMenu.exec(pos);
    if(selectedAction == actionProperties)
        EditLinkDialog(item).exec();
}
