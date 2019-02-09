#ifndef STINTERFACES_H
#define STINTERFACES_H

#include "stinterfaces_global.h"

#include <QObject>

#include <Project.h>
#include <Object.h>

namespace StInterfaces {

class STINTERFACESSHARED_EXPORT IVirtualMachine : public QObject
{
    Q_OBJECT
public:
    IVirtualMachine(QObject* parent = nullptr);

    virtual StData::Project* project() const = 0;
    virtual void setProject(StData::Project* project) = 0;

    virtual void initialize() = 0;
    virtual void resetForProject(StData::Project* project) = 0;
    virtual void reset() = 0;

    virtual void doOneIteration() = 0;
//    virtual void execute(StData::Object* object) = 0;
    virtual void stop();

    virtual void savePlusMatrixes() = 0;
    virtual void deletePlusMatrixes() = 0;
    virtual void deleteAllMatrixes() = 0;
    virtual void loadProjectMatrixes() = 0;

    virtual void showProjectWindows(StData::Project* project) = 0;

    virtual void setJumpingState() = 0;
    virtual void resetJumpingState() = 0;
    virtual bool isJumpingState() = 0;

signals:
    void closeAllRequested();
    void stopRequested();
    void hyperJumpOnProject(StData::Project* project);
};

}

#endif // STINTERFACES_H
