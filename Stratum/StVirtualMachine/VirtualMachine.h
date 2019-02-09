#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include "stvirtualmachine_global.h"
#include "KeyStateObserver.h"

#include <IVirtualMachine.h>

#include <QFontDatabase>


namespace StData {
class Project;
}

namespace StVirtualMachine {

class STVIRTUALMACHINESHARED_EXPORT VirtualMachine : public StInterfaces::IVirtualMachine
{
    Q_OBJECT
public:
    VirtualMachine(QObject* parent = nullptr);

    StData::Project* project() const { return m_project; }
    void setProject(StData::Project* project);

    // IVirtualMachine interface
public:
    void initialize();
    void resetForProject(StData::Project* project);
    void reset();
    void doOneIteration();


    void savePlusMatrixes();
    void deletePlusMatrixes();
    void deleteAllMatrixes();
    void loadProjectMatrixes();

    void showProjectWindows(StData::Project* project);

    void setJumpingState();
    void resetJumpingState();
    bool isJumpingState();

private:
    KeyStateObserver* m_keyStateObserver;
    QFontDatabase m_fontDatabase;
    StData::Project* m_project;
};

}

#endif // VIRTUALMACHINE_H
