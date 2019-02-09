#include "Class.h"
#include "Sc2000DataStream.h"
#include "Equation.h"
#include "ClassVarInfo.h"
#include "LinkInfo.h"
#include "Log.h"
#include "Project.h"
#include "Scheme.h"
#include "ObjectInfo.h"
#include "Icon.h"
#include "ObjectInfoLoader.h"
#include "Object.h"
#include "StandardTypes.h"
#include "Library.h"

#include <QDebug>

namespace StData
{

Class::Class(const QFileInfo& fileInfo, Library* library) :
    m_fileInfo(fileInfo),
    m_library(library),
    m_fileVersion(0),
    m_classId(0),
    m_codeSize(0),
    m_varSize(0),
    m_equation(nullptr),
    m_scheme(nullptr),
    m_image(nullptr),
    m_icon(nullptr),
    m_classDataPos(0),
    m_classDataSize(0),
    m_isLoaded(false),
    m_isNameLoaded(false),
    m_flags(0)
{}

Class::~Class()
{
    qDeleteAll(m_varInfoList);
    qDeleteAll(m_childInfoList);
    qDeleteAll(m_linkForHandle);

    if(m_equation)
        delete m_equation;

    if(m_scheme)
        delete m_scheme;

    if(m_image)
        delete m_image;

    if(m_icon)
        delete m_icon;
}

QString Class::originalName()
{
    return m_originalName;
}

void Class::setOriginalName(const QString& name)
{
    m_originalName = name;
    m_cleanName = m_originalName.toLower();
}

Equation* Class::equation()
{
    return m_equation;
}

QString Class::helpFile()
{
    return m_helpFile;
}

QString Class::iconFileName()
{
    return m_iconFileName;
}

qint16 Class::defIcon()
{
    return m_defIcon;
}

qint32 Class::flags()
{
    return m_flags;
}

QString Class::text()
{
    return m_text;
}

QString Class::description()
{
    return m_description;
}

Scheme* Class::image()
{
    return m_image;
}

Scheme* Class::scheme()
{
    return m_scheme;
}

QMap<qint16, LinkInfo*> Class::linkDictionary()
{
    return m_linkForHandle;
}

QList<LinkInfo*> Class::links()
{
    return m_linkForHandle.values();
}

bool Class::isLinkExist(qint16 linkId)
{
    return m_linkForHandle.contains(linkId);
}

LinkInfo* Class::linkById(qint16 linkId)
{
    return m_linkForHandle.value(linkId, nullptr);
}

void Class::addLink(LinkInfo* linkInfo)
{
    m_linkForHandle[linkInfo->handle()] = linkInfo;
}

bool Class::isObjectInfoExist(qint16 objectInfoId) const
{
    return m_objectInfoForHandle.contains(objectInfoId);
}

ObjectInfo* Class::objectInfoByHandle(qint16 handle) const
{
    return m_objectInfoForHandle.value(handle, nullptr);
}

void Class::addObjectInfo(ObjectInfo* objectInfo)
{
    m_childInfoList << objectInfo;
    m_objectInfoForHandle[objectInfo->handle()] = objectInfo;
}

int Class::childsCount() const
{
    return m_objectInfoForHandle.count();
}

int Class::childHandleByIndex(int index)
{
    int handle = -1;
    if(index >=0 && index < m_childInfoList.count())
        handle = m_childInfoList.at(index)->handle();

    return handle;
}

int Class::childIndexByHandle(int handle)
{
    for(int i = 0; i < m_childInfoList.count(); i++)
        if(m_childInfoList.at(i)->handle() == handle)
            return i;
    return -1;
}

bool Class::setChildCalcOrder(int handle, int order)
{
    if(order < 0 || order >= m_childInfoList.count())
        return false;

    int index = childIndexByHandle(handle);
    if(index == order)
        return true;

    ObjectInfo* objInfo = m_childInfoList.takeAt(index);
    m_childInfoList.insert(order, objInfo);

    for(Object* object : m_objects)
        object->setChildCalcOrder(index, order);
    return true;
}

QList<ClassVarInfo*> Class::varInfoList()
{
    return m_varInfoList;
}

bool Class::addVarInfo(ClassVarInfo* var)
{
    if(!m_varInfoForName.contains(var->varName()))
    {
        m_varInfoList << var;
        m_varInfoForName[var->varName()] = var;
        return true;
    }
    return false;
}

void Class::deleteAllVariables()
{
    qDeleteAll(m_varInfoList);
    m_varInfoList.clear();
    m_varInfoForName.clear();
}

ClassVarInfo* Class::varInfoByName(const QString& varName)
{
    return m_varInfoForName.value(varName, nullptr);
}

ClassVarInfo*Class::varInfoByIndex(int index)
{
    return m_varInfoList.at(index);
}

void Class::load()
{
    if(m_isLoaded)
        return;

    QFile file(m_fileInfo.absoluteFilePath());
    if(file.open(QFile::ReadOnly))
    {
        St2000DataStream stream(&file);
        DataCode code = readDataCode(stream);
        if(code == DataCode::ClassSignature)
        {
            m_fileVersion = stream.readUint32();
            m_classDataPos = stream.readUint32();
            m_classDataSize = stream.readUint32();
            setOriginalName(stream.readString());

            code = readDataCode(stream);
            while(code != DataCode::Undefined)
            {
                switch(code)
                {
                    case DataCode::HelpFile:
                        m_helpFile = stream.readString();
                        break;

                    case DataCode::ClassTime:
                        m_classId = stream.readInt32();
                        break;

                    case DataCode::IconFile:
                        m_iconFileName = stream.readString();
                        break;

                    case DataCode::DefIcon:
                        m_defIcon = stream.readInt16();
                        break;

                    case DataCode::Equ:
                        m_equation = new Equation();
                        m_equation->load(stream);
                        break;

                    case DataCode::Code:
                    {
                        qint32 codeSize = stream.readInt32() * 2;
                        m_codeBytes = stream.readBytes(codeSize);
                    }break;

                    case DataCode::Vars:
                    case DataCode::VarsWithDefValue:
                    {
                        qint16 varCount = stream.readInt16();
                        for(int i = 0; i < varCount; i++)
                        {
                            ClassVarInfo* varInfo = new ClassVarInfo();
                            varInfo->load(stream, code == DataCode::VarsWithDefValue);
                            addVarInfo(varInfo);
                        }

                    }break;

                    case DataCode::VarSize:
                        m_varSize = stream.readInt16();
//                        stream.skipRawData(2);
                        break;

                    case DataCode::Links:
                    {
                        qint16 linkCount = stream.readInt16();
                        for(int i = 0; i < linkCount; i++)
                        {
                            LinkInfo* linkInfo = new LinkInfo();
                            linkInfo->load(stream);
                            addLink(linkInfo);
                        }
                    }break;

                    case DataCode::ChildsNameXY:
                    case DataCode::ChildsName:
                    case DataCode::Childs:
                    {
                        qint16 childCount = stream.readInt16();
                        bool loadName = (code == DataCode::ChildsName || code == DataCode::ChildsNameXY);
                        bool loadPos = (code == DataCode::ChildsNameXY);
                        for(int i = 0; i < childCount; i++)
                        {
                            ObjectInfo* objectInfo = new ObjectInfo();
                            objectInfo->load(stream, m_fileVersion, loadName, loadPos);
                            addObjectInfo(objectInfo);
                        }
                    }break;

                    case DataCode::Flags:
                        m_flags = stream.readInt32();
                        break;

                    case DataCode::Text:
                        m_text = stream.readString();
                        break;

                    case DataCode::Info:
                        m_description = stream.readString();
                        break;

                    case DataCode::Schm:
                        m_scheme = new Scheme(this);
                        m_scheme->load(stream);
                        break;

                    case DataCode::Image:
                        m_image = new Scheme(this);
                        m_image->load(stream);
                        break;

                    case DataCode::Ico:
                        m_icon = new Icon();
                        m_icon->load(stream);
                        break;

                    default:
                        SystemLog::instance().error("undef class file code");
                        break;
                }
                code = readDataCode(stream);
            }
            stream.device()->seek(m_classDataPos + m_classDataSize);
            m_isLoaded = true;
        }
    }
}

void Class::loadName()
{
    if(m_isNameLoaded)
        return;

    QFile file(m_fileInfo.absoluteFilePath());
    if(file.open(QFile::ReadOnly))
    {
        St2000DataStream stream(&file);
        DataCode code = readDataCode(stream);
        if(code == DataCode::ClassSignature)
        {
            stream.skipRawData(12);
            setOriginalName(stream.readString());
        }
        m_isNameLoaded = true;
    }
}


bool Class::isProtected()
{
    if(dynamic_cast<SystemLibrary*>(m_library))
       return true;

    return (m_flags & (qint32)Flag::Protected) != 0;
}


void Class::addObject(Object* object)
{
    if(object && object->cls() == this)
        m_objects << object;
}

void Class::recreateObjectVars()
{
    Object* obj;
    foreach (obj, m_objects)
    {
        obj->recreateVariables();
    }
}

void Class::updateDisableVars()
{
    m_flags &= (Flag::Disabled | Flag::UnDisabled);
    for (int i = 0; i < m_varInfoList.count(); ++i)
    {
        ClassVarInfo* var = m_varInfoList[i];
        if(StandardTypes::isFloat(var->varType()))
        {
            if(var->isEnableVar() || var->isDisableVar())
            {
                m_flags |= (qint32)Flag::Disabled;
                if(var->isEnableVar())
                    m_flags |= (qint32)Flag::UnDisabled;

                return;
            }
        }
    }
}


Class::DataCode Class::readDataCode(St2000DataStream& stream)
{
    return static_cast<DataCode>(stream.readInt16());
}

}
