#include "ObjectStateLoader.h"

#include <QDebug>

#include <Sc2000DataStream.h>
#include <VarManager.h>
#include <QFile>
#include <QDir>
#include <QFile>
#include <Project.h>
#include <Object.h>
#include <Class.h>
#include <ClassVarInfo.h>

namespace StData {

ObjectVarsLoader::ObjectVarsLoader()
{}

ObjectVarsLoader::~ObjectVarsLoader()
{}

void ObjectVarsLoader::load(Project* project, Object* /*object*/)
{
    if(!project)
        return;

    QList<ClassData*> classDataList;

    QString sttFileName = project->varManager()->getVarS("preload_file");
    if(sttFileName.isEmpty())
        sttFileName = project->fileInfo().absoluteDir().absoluteFilePath("_preload.stt");

    QFile sttFile(sttFileName);
    if(sttFile.exists() && sttFile.open(QFile::ReadOnly))
    {
        St2000DataStream stream(&sttFile);
        QString header = stream.readString(100);
        if(header == "SC Scheme Variables")
        {
            /*QString className = */stream.readString(128);
            int code = stream.readInt16();
            while(code)
            {
                switch(code)
                {
                    case StreamVersion:
                        stream.readFileVersion();
                        break;

                    case Classes:
                        classDataList = loadClassDataList(stream);
                        break;

                    case Vars:
                        qDebug() << "Code_Vars";
                        break;

                    case SetVars:
                        loadVars(stream, project);
                        break;
                }
                code = stream.readInt16();
            }
        }
    }
    qDeleteAll(classDataList);
    classDataList.clear();
}

QList<ClassData*> ObjectVarsLoader::loadClassDataList(St2000DataStream& stream)
{
    QList<ClassData*> result;

    int classesCount = stream.readInt16();
    for(int i = 0; i < classesCount; i++)
    {
        ClassData* classData = new ClassData();
        result.append(classData);

        classData->setName(stream.readString());
        if(stream.version() > 1)
            classData->setClassId(stream.readUint32());

        qint16 varCount = stream.readInt16();
        qint16 childCount = stream.readInt16();
        classData->setVarSize(stream.readInt16());

        if(childCount)
        {
            for(int j = 0; j < childCount; j++)
            {
                ChildData* childData = new ChildData();
                childData->setId(stream.readInt16());
                /*child[i]._lclass = (TLClass*)*/stream.readInt16();
                classData->addChildData(childData);
            }
        }
        if(varCount)
        {
            for(int j = 0; j < varCount; j++)
            {
                VarDataInfo* varData = new VarDataInfo();
                varData->setName(stream.readString());
                /*vars[i]._lclass = (TLClass*)*/stream.readInt16();
                classData->addVarData(varData);
            }
        }
    }
    return result;
}

void ObjectVarsLoader::loadVars(St2000DataStream& stream, Project* project)
{
    qint32 endOfData = stream.readInt32();
    /*qint16 childId = */stream.readInt16();
    QString className = stream.readString(65);
    quint32 classId = (stream.fileVersion() > 1) ? stream.readUint32() : 0;

    Object* object = project->rootObject();
    if(className == object->cls()->originalName() || (classId && classId == object->cls()->classId()))
    {
        if(stream.pos() < endOfData)
            loadObjectVars(stream, object, endOfData);

        stream.device()->seek(endOfData);
    }
}

void ObjectVarsLoader::loadObjectVars(St2000DataStream& stream, Object* object, qint32 endOfData)
{
    qint16 count = stream.readInt16();
    for(int i = 0; i < count; i++)
    {
        QString varName = stream.readString();
        QString varData = stream.readString();
        object->setVar(varName, varData);
    }

    while(stream.pos() < endOfData && stream.status() == QDataStream::Ok)
    {
        qint32 childEndOfData = stream.readInt32();
        qint16 childId = stream.readInt16();
        QString childClassName = stream.readString(65);
        quint32 childClassId = (stream.fileVersion() > 1) ? stream.readInt32() : 0;

        Object* childObject = object->childByHandle(childId);
        if(childObject)
        {
            if(childClassName == childObject->cls()->originalName() ||
               childClassId == childObject->cls()->classId())
            {
                loadObjectVars(stream, childObject, childEndOfData);
            }
        }
        stream.device()->seek(childEndOfData);
    }
}

/*----------------------------------------------------------------------------------------------------------*/
ClassData::~ClassData()
{
    qDeleteAll(m_childDataList);
    qDeleteAll(m_varDataList);

    m_childDataList.clear();
    m_varDataList.clear();
}

void ClassData::addChildData(ChildData* childData)
{
    m_childDataList.append(childData);
}

void ClassData::addVarData(VarDataInfo* varData)
{
    m_varDataList.append(varData);
}

}
