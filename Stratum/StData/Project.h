#ifndef PROJECT_H
#define PROJECT_H

#include "stdata_global.h"

#include <QFileInfo>
#include <QObject>
#include <QStringList>
#include <QMap>
#include <memory>

namespace StData
{
class Library;
class ProjectLibrary;
class SystemLibrary;
class ExtraLibrary;
class LibraryCollection;

class ProjectVarCollection;
class Class;
class Object;
class ObjectVarsLinker;
class HyperKeyParams;

typedef std::shared_ptr<HyperKeyParams> HyperKeyParamsPtr;

class STDATASHARED_EXPORT Project : public QObject
{
    Q_OBJECT
public:
    enum FileCodeSignature {ProjectCode = 0x6849, PasswordCode = 101, MainClassCode = 100, ProjectVarsCode = 102, WatchCode = 103};

public:
    explicit Project(const QString& fileName, QObject *parent = 0);
    ~Project();

    QFileInfo fileInfo() const {return m_fileInfo;}
    QDir projectDir() const;

    LibraryCollection* libraryCollection() const { return m_libraryCollection; }

    Class* rootClass() const {return m_rootClass;}
    void setRootClass(Class* rootClass) {m_rootClass = rootClass;}

    Object* rootObject() const {return m_rootObject;}

    HyperKeyParamsPtr hyperKeyParams() const { return m_hyperKeyParams; }
    void setHyperKeyParams(const HyperKeyParamsPtr& hyperKeyParams) { m_hyperKeyParams = hyperKeyParams; }

    ProjectVarCollection* varManager() const {return m_varCollection;}

    Class* getClassByName(const QString& className);

    void toStartupState();
    bool load();

    void setupObjects();
    void setupLinks(bool saveVarData);
    void setupVars();

private:
    QFileInfo m_fileInfo;

    Class* m_rootClass;
    Object* m_rootObject;
    HyperKeyParamsPtr m_hyperKeyParams;

    ProjectLibrary* m_projectLibrary;
//    QList<ExtraLibrary*> m_extraLibraries;
    LibraryCollection* m_libraryCollection;

    ObjectVarsLinker* m_objectVarsLinker;
    ProjectVarCollection* m_varCollection;
};

}

#endif // PROJECT_H
