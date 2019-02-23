#include "ObjectVar.h"

#include "Class.h"
#include "ClassVarInfo.h"
#include "GraphicsHelper.h"
#include "StandardTypes.h"
#include "StDataGlobal.h"

#include <QDebug>

namespace StData {

ObjectVar::ObjectVar(ClassVarInfo* varInfo) :
    m_varInfo(varInfo)
{
    m_varData = std::make_shared<VarData>();
    setData(m_defaultData, varInfo->defValue());
}

void ObjectVar::setData(const QString& data)
{
    setData(*m_varData, data);
}

void ObjectVar::setDouble(double value)
{
    m_varData->d = value;
}

void ObjectVar::setInt(qint32 value)
{
    m_varData->i = value;
}

void ObjectVar::setString(const QString& str)
{
    m_varData->s = str;
    if(m_varInfo == nullptr)
    {
        bool isNumeric = false;
        double value = str.toDouble(&isNumeric);
        if(isNumeric)
        {
            m_varData->d = value;
            m_varData->i = value;
        }
    }
}

void ObjectVar::setVar(ObjectVar* var)
{
    *m_varData = *(var->m_varData);
    m_savedVarData = var->m_savedVarData;
}

void ObjectVar::incDouble(double value)
{
    m_varData->d += value;
}

void ObjectVar::incInt(int value)
{
    m_varData->i += value;
}

void ObjectVar::setSavedDouble(double value)
{
    m_savedVarData.d = value;
}

void ObjectVar::setSavedInt(qint32 value)
{
    m_savedVarData.i = value;
}

void ObjectVar::setSavedString(const QString& str)
{
    m_savedVarData.s = str;
}

void ObjectVar::save()
{
    m_savedVarData = *m_varData;
}

void ObjectVar::restore()
{
    *m_varData = m_savedVarData;
}

void ObjectVar::saveAsStartup()
{
    m_startupData = *m_varData;
}

Class*ObjectVar::type()
{
    return m_varInfo->varType();
}

QString ObjectVar::typeName()
{
    return m_varInfo->varType() ? m_varInfo->varType()->originalName() : "";
}

void ObjectVar::toDefaultState()
{
//    m_varData->reset();
    if(StandardTypes::isFloat(m_varInfo->varType()))
    {
        if(m_defaultData.d != 0.0)
            m_varData->d = m_defaultData.d;
    }
    else if(StandardTypes::isNumeric(m_varInfo->varType()) || StandardTypes::isColorRef(m_varInfo->varType()))
    {
        if(m_defaultData.i != 0)
            m_varData->i = m_defaultData.i;
    }
    else if(StandardTypes::isString(m_varInfo->varType()))
    {
        if(m_defaultData.s != "")
            m_varData->s = m_defaultData.s;
    }
}

void ObjectVar::clear()
{
    if(StandardTypes::isFloat(m_varInfo->varType()))
    {
        if(m_defaultData.d != 0.0)
            m_varData->d = 0;
    }
    else if(StandardTypes::isNumeric(m_varInfo->varType()) || StandardTypes::isColorRef(m_varInfo->varType()))
    {
        if(m_defaultData.i != 0)
            m_varData->i = 0;
    }
    else if(StandardTypes::isString(m_varInfo->varType()))
    {
        if(m_defaultData.s != "")
            m_varData->s = "";
    }
}

void ObjectVar::toStartupState()
{
//    m_varData->reset();
    *m_varData = m_startupData;
}

QString ObjectVar::toString()
{
    Class* varType = m_varInfo->varType();
    if(StandardTypes::isNumeric(varType))
    {
        return QString().setNum(StandardTypes::isFloat(varType) ? m_varData->d : m_varData->i);
    }
    else if(StandardTypes::isColorRef(varType))
    {
        QColor color = intToColor(m_varData->i);
        if(color.alpha() != 255)
        {
            return QString("rgb(%1,%2,%3,%4)")
                    .arg(color.red())
                    .arg(color.green())
                    .arg(color.blue())
                    .arg(color.alpha());
        }
        return QString("rgb(%1,%2,%3)")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue());
    }

    return m_varData->s;
}

bool ObjectVar::isNumeric() const
{
    return StandardTypes::isNumeric(m_varInfo->varType());
}

bool ObjectVar::isDouble() const
{
    return StandardTypes::isFloat(m_varInfo->varType());
}

bool ObjectVar::isString() const
{
    return StandardTypes::isString(m_varInfo->varType());
}

void ObjectVar::setData(VarData& varData, const QString& data)
{
    Class* varType = m_varInfo->varType();
    if(StandardTypes::isNumeric(varType))
    {
        if(StandardTypes::isFloat(varType))
            varData.d = data.toDouble();

        else if(StandardTypes::isHandle(varType))
        {
            QString cleanData = data;
            varData.i = cleanData.replace("#", "").toInt();
        }
        else
            varData.i = data.toInt();
    }
    else if(StandardTypes::isColorRef(varType))
    {
        QColor color = GraphicsHelper::colorFromTemplate(data);
        varData.i = colorToInt(color);
    }
    else
        varData.s = data;
}

}
