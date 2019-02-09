#ifndef SPACEEDITWIDGET_H
#define SPACEEDITWIDGET_H

#include <QDir>
#include <QGraphicsItem>
#include <QWidget>

namespace StData {
class Space;
class Object;
class Project;
}

namespace StSpace {
class SpaceScene;
class SpaceView;
}

class SpaceContextMenu;

class SpaceEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpaceEditWidget(StData::Space* space, StData::Object* object, QWidget *parent = 0);

    StSpace::SpaceScene* spaceScene() const { return m_spaceScene; }
    StSpace::SpaceView* spaceView() const { return m_spaceView; }

private slots:
    void spaceDoubleClick(QGraphicsItem* graphicsItem);

private:
    StData::Space* m_space;
    StData::Object* m_spaceOwnerObject;

    StSpace::SpaceScene* m_spaceScene;
    StSpace::SpaceView* m_spaceView;

    SpaceContextMenu* m_spaceContextMenu;
};

#endif // SPACEEDITWIDGET_H
