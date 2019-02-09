#ifndef STCORE_H
#define STCORE_H

#include "stcore_global.h"

#include <ErrorProvider.h>
#include <QObject>
#include <memory>

namespace StData {
class Project;
class ProjectPool;
class SystemLibrary;
class Error;

typedef std::shared_ptr<Error> ErrorPtr;
}

namespace StCompiler{
class Compiler;
typedef std::shared_ptr<Compiler> CompilerPtr;
}

namespace StInterfaces {
class IVirtualMachine;
}

namespace StCore
{

class Run;
//class Settings;

class STCORESHARED_EXPORT Core : public QObject
{
    Q_OBJECT
public:
    ~Core();

    StData::Project* currentProject() const { return m_currentProject; }
    static Core& instance();

public:
    void initialize();

    void openProject(const QString& projectFileName);
    void saveProject();
    void closeCurrentProject();

    StCompiler::Compiler* compiler() const { return m_compiler; }
    StInterfaces::IVirtualMachine* virtualMachine() const { return m_virtualMachine; }

    Run* run() const { return m_run; }

    //Settings* settings() const { return m_settings; }

private:
    explicit Core();

private:
    StCompiler::Compiler* m_compiler;
    StInterfaces::IVirtualMachine* m_virtualMachine;

    StData::Project* m_currentProject;

    Run* m_run;
//    Settings* m_settings;
};

}
#endif // STCORE_H
