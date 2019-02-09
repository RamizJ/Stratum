#ifndef CONTEXT_H
#define CONTEXT_H

#include "stcore_global.h"

#include <QDir>

namespace StCore {

class Project;
class Log;

class STCORESHARED_EXPORT Context
{
public:
    ~Context();

    Log* log() const {return m_log;}

    QDir stratumDir() const;
    QDir stratumLibraryDir() const;

    Project* currentProject() const {return m_currentProject;}
    void setCurrentProject(Project* currentProject) {m_currentProject = currentProject;}

public:
    static Context& instance();

private:
    Context();

    Context(const Context&);
    Context& operator=(const Context&);

private:
    Log* m_log;
    QDir m_stratumDir;
    Project* m_currentProject;
};

}

#endif // CONTEXT_H
