#ifndef CONTEXTMENUMANAGER_H
#define CONTEXTMENUMANAGER_H

#include <QAction>
#include <QGraphicsItem>
#include <QObject>

namespace StData {
class Object;
}

namespace StSpace {
class SpaceScene;

class PolylineGraphicsItem;
class PixmapGraphicsItem;
class RasterTextGraphicsItem;
class GroupGraphicsItem;
class ObjectGraphicsItem;
class LinkGraphicsItem;
}

class SpaceContextMenu : public QObject
{
    Q_OBJECT
public:
    explicit SpaceContextMenu(StData::Object* spaceOwnerObject, QObject* parent = nullptr);
    ~SpaceContextMenu();

public slots:
    void openContextMenu(const QPoint& pos, QGraphicsItem* graphicsItem = nullptr);

private:
    void openSpaceContextMenu(const QPoint& pos, StSpace::SpaceScene* spaceScene);
    void openPolylineContextMenu(const QPoint& pos, StSpace::PolylineGraphicsItem* item);
    void openPixmapContextMenu(const QPoint& pos, StSpace::PixmapGraphicsItem* item);
    void openTextContextMenu(const QPoint& pos, StSpace::RasterTextGraphicsItem* item);
    void openGroupContextMenu(const QPoint& pos, StSpace::GroupGraphicsItem* item);
    void openObjectContextMenu(const QPoint& pos, StSpace::ObjectGraphicsItem* item);
    void openLinkContextMenu(const QPoint& pos, StSpace::LinkGraphicsItem* item);

private:
    StData::Object* m_spaceOwnerObject;

    QMenu* m_zOrderMenu;
    QList<QAction*> m_clipboardActions;

    //Z-Order
    QAction* m_actionZOrderToFront;
    QAction* m_actionZOrderToBack;
    QAction* m_actionZOrderStepFront;
    QAction* m_actionZOrderStepBack;

    //Clipboard
    QAction* m_actionCut;
    QAction* m_actionCopy;
    QAction* m_actionPaste;
};

#endif // CONTEXTMENUMANAGER_H
