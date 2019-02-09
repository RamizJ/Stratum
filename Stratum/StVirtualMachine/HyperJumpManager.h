#ifndef HYPERJUMPMANAGER_H
#define HYPERJUMPMANAGER_H

#include <QMap>
#include <QObject>
#include <memory>

#include "stvirtualmachine_global.h"

namespace StData {
class Project;
class SpaceItem;
class SpaceItemHyperKey;

typedef std::shared_ptr<SpaceItemHyperKey> SpaceItemHyperKeyPtr;
}

namespace StSpace {
class SpaceWidget;
class SpaceGraphicsItem;
}

namespace StVirtualMachine {

class VirtualMachine;

class STVIRTUALMACHINESHARED_EXPORT HyperJumpManager : public QObject
{
    Q_OBJECT
public:
    explicit HyperJumpManager(VirtualMachine* virtualMachine);

    void registerWindow(StSpace::SpaceWidget* widget);
    void registerItem(StSpace::SpaceWidget* widget, StSpace::SpaceGraphicsItem* item);
    void reset();

signals:
    void hyperJumpOnProject(StData::Project* project);

public slots:
    void hyperJumpInvoked();

private:
    void runApp(const QString& fileName);
    void runProject(const StData::SpaceItemHyperKeyPtr& key);
    void openSpaceWindow();

private:
    QList<StSpace::SpaceWidget*> m_registeredWindows;
    QList<StSpace::SpaceGraphicsItem*> m_registeredItems;
    QMap<StData::SpaceItem*, StSpace::SpaceWidget*> m_spaceWidgetForItem;
    VirtualMachine* m_virtualMachine;
};

}

#endif // HYPERJUMPMANAGER_H
