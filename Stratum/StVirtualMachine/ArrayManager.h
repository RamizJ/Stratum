#ifndef ARRAYMANAGER_H
#define ARRAYMANAGER_H

#include "StreamItem.h"

#include <QObject>
#include <HandleCollection.h>
#include <HandleItem.h>
#include <ErrorProvider.h>

namespace StData {
class Project;
}

namespace StVirtualMachine {

class Array;
class ArrayItem;
class StreamManager;

class ArrayManager : public QObject, public StData::ErrorHolder
{
    Q_OBJECT
public:
    explicit ArrayManager(QObject* parent = 0);
    ~ArrayManager();

//    StreamManager* streamManager() const { return m_streamManager; }
//    void setStreamManager(StreamManager* streamManager) { m_streamManager = streamManager; }

//    StData::Project* currentProject() const {return m_currentProject; }
//    void setCurrentProject(StData::Project* project) { m_currentProject = project; }

public:
    int createArray();
    int deleteArray(int handle);

    int insertItem(int handle, const QString& typeName);
    bool deleteItem(int handle, int index);
    int getCount(int handle);

    QString getItemTypeName(int handle, int index);
    int getVarTypeIndex(int handle, int index, const QString& varName);

    bool getVarF(int handle, int index, const QString varName, double& value);
    bool getVarH(int handle, int index, const QString varName, int& value);
    bool getVarS(int handle, int index, const QString varName, QString& value);

    bool setVarF(int handle, int index, const QString& varName, double data);
    bool setVarH(int handle, int index, const QString& varName, int data);
    bool setVarS(int handle, int index, const QString& varName, const QString& data);

    Array* getArray(int handle);
    ArrayItem* getItem(int handle, int index);
    int insertArray(Array* array);
    inline bool isIndexValid(Array* array, int index);

    bool save(int handle, StreamItem* streamItem);
    int load(StreamItem* streamItem);

    void reset();

private:
    StData::HandleCollection<Array> m_arrays;
//    StreamManager* m_streamManager;
//    StData::Project* m_currentProject;
};

}

#endif // ARRAYMANAGER_H
