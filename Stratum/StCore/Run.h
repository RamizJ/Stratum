#ifndef RUNMANAGER_H
#define RUNMANAGER_H

#include <stcore_global.h>

#include <ErrorProvider.h>
#include <QFileInfo>
#include <QObject>
#include <memory>

namespace StData {
class Project;
}

//namespace StVMachine {
//class VirtualMachine;
//}

namespace StInterfaces {
class IVirtualMachine;
}

namespace StCore {

class STCORESHARED_EXPORT Run : public QObject
{
    Q_OBJECT
public:
    explicit Run(StInterfaces::IVirtualMachine* virtualMachine, QObject* parent = nullptr);
    ~Run();

    int timerInterval() const { return m_timerInterval; }
    void setTimerInterval(int timerInterval) { m_timerInterval = timerInterval; }

    StData::Project* currentProject() const { return m_currentProject; }
    void setCurrentProject(StData::Project* currentProject);

    bool inProgress() const { return m_inProgress; }

    void doOneIteration();

signals:
   void vmPause();
   void vmStop();
   void stopped();

public slots:
    void start();
    void pause();
    void stop();
    void oneStep();

protected:
    void timerEvent(QTimerEvent*);

private slots:
    void onCloseAll();
    void hyperJumpOnProject(StData::Project* project);

private:
    void stopTimer();

private:
    int m_timerId;
    int m_timerInterval;
    bool m_inProgress;
    bool m_iterationInProcess;
    bool m_mustStop;

    StData::Project* m_currentProject;
    StData::Project* m_jumpingProject;
    StInterfaces::IVirtualMachine* m_virtualMachine;

    QList<StData::Project*> m_executedProjects;
};

}

#endif // RUNMANAGER_H
