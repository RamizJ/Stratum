#include "VarManager.h"

#include "Sc2000DataStream.h"
#include "Log.h"

namespace StData {

ProjectVarCollection::ProjectVarCollection()
{}

ProjectVarCollection::~ProjectVarCollection()
{
    qDeleteAll(m_varList);
    m_varList.clear();
    m_varForName.clear();
}

//void VarManager::load(Sc2000DataStream& stream)
//{
//    qint16 count = stream.readInt16();

//    for (qint16 i=0; i < count; i++)
//    {
//        QByteArray varData = stream.readByteArray();
//        if(varData.size() > 0)
//        {
//            ProjectVariable* varInfo = new ProjectVariable;
//            varInfo->setType(ProjectVariable::VarType((int)varData[0]));

//            qint8 nameLength = (qint8)varData[1];
//            QString name;
//            for(int j = 2; j <= nameLength; j++)
//                name.push_back(varData.at(j));
//            varInfo->setName(name);

//            QByteArray value;
//            for(int j = nameLength+2; j < varData.length(); ++j)
//                value.push_back(varData.at(j));

//            Sc2000DataStream readValueStream(value);
//            switch (varInfo->type())
//            {
//                case ProjectVariable::Int:
//                {
//                    qint32 intValue(0);
//                    readValueStream >> intValue;
//                    varInfo->setValue(intValue);
//                }break;

//                case ProjectVariable::Double:
//                {
//                    double doubleValue(0.0);
//                    readValueStream >> doubleValue;
//                    varInfo->setValue(doubleValue);
//                }break;

//                case ProjectVariable::String:
//                    varInfo->setValue(QString::fromLocal8Bit(value));
//                    break;
//            }
//            m_varList << varInfo;
//            m_varForName[name] = varInfo;
//        }
//    }
//}

QString ProjectVarCollection::getVarS(QString varName)
{
    QString result;
    if(isVarTypeEqual(varName, ProjectVariable::String))
        result = m_varForName[varName]->value().toString();
    return result;
}

int ProjectVarCollection::getVarI(QString varName)
{
    int result = 0;
    if(isVarTypeEqual(varName, ProjectVariable::Int))
        result = m_varForName[varName]->value().toInt();
    return result;
}

double ProjectVarCollection::getVarD(QString varName)
{
    double result = 0.0;
    if(isVarTypeEqual(varName, ProjectVariable::Double))
        result = m_varForName[varName]->value().toDouble();
    return result;
}

bool ProjectVarCollection::contains(QString varName)
{
    return m_varForName.keys().contains(varName);
}

bool ProjectVarCollection::isVarTypeEqual(QString varName, ProjectVariable::VarType varType)
{
    return (contains(varName) && m_varForName[varName]->type() == varType);
}

void ProjectVarCollection::addVariable(ProjectVariable* variable)
{
    if(variable)
    {
        m_varList << variable;
        m_varForName[variable->name()] = variable;
    }
}

}
