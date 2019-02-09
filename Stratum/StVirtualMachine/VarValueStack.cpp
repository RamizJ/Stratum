#include "VarValueStack.h"

#include <ObjectVar.h>

using namespace StData;

namespace StVirtualMachine {

VarValueStack::VarValueStack(QObject* parent) :
    QObject(parent),
    m_values(1000),
    m_valuePointer(nullptr),
    m_i(0)
{
    reset();
}

VarValueStack::~VarValueStack()
{}

void VarValueStack::reset()
{
    m_valuePointer = &m_values.last();
#ifdef QT_DEBUG
    m_i = getIndex();
#endif

}

void VarValueStack::pushDouble(double value)
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    (--m_valuePointer)->d = value;
}

void VarValueStack::pushInt32(qint32 value)
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    (--m_valuePointer)->i = value;
}

void VarValueStack::pushInt16(qint16 value)
{
    (--m_valuePointer)->w = value;
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
}

void VarValueStack::pushByte(quint8 value)
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    (--m_valuePointer)->b = value;
}

void VarValueStack::pushString(const QString& value)
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    (--m_valuePointer)->s = value;
}

void VarValueStack::pushVarData(VarData* value)
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    (--m_valuePointer)->v = value;
}

double VarValueStack::popDouble()
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    return (m_valuePointer++)->d;
}

qint32 VarValueStack::popInt32()
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    return (m_valuePointer++)->i;
}

qint16 VarValueStack::popInt16()
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    return (m_valuePointer++)->w;
}

quint8 VarValueStack::popByte()
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    return (m_valuePointer++)->b;
}

QString VarValueStack::popString()
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    return (m_valuePointer++)->s;
}

VarData* VarValueStack::popVarData()
{
#ifdef QT_DEBUG
    m_i = getIndex();
#endif
    return static_cast<VarData*>((m_valuePointer++)->v);
}

double VarValueStack::getDouble(int offset) const
{
    return (m_valuePointer + offset)->d;
}

qint32 VarValueStack::getInt32(int offset) const
{
    return (m_valuePointer + offset)->i;
}

qint16 VarValueStack::getInt16(int offset) const
{
    return (m_valuePointer + offset)->w;
}

quint8 VarValueStack::getByte(int offset) const
{
    return (m_valuePointer + offset)->b;
}

QString VarValueStack::getString(int offset) const
{
    return (m_valuePointer + offset)->s;
}

VarData* VarValueStack::getVarData(int offset) const
{
    return m_valuePointer + offset;
}

void VarValueStack::setDouble(double value, int offset)
{
    (m_valuePointer + offset)->d = value;
}

void VarValueStack::setInt32(qint32 value, int offset)
{
    (m_valuePointer + offset)->i = value;
}

void VarValueStack::setInt16(qint16 value, int offset)
{
    (m_valuePointer + offset)->w = value;
}

void VarValueStack::setByte(quint8 value, int offset)
{
    (m_valuePointer + offset)->b = value;
}

void VarValueStack::setString(const QString& value, int offset)
{
    (m_valuePointer + offset)->s = value;
}

//void VarValueStack::setVarData(VarData* value, int offset)
//{
//    (m_valuePointer + offset)->v = value;
//}

void VarValueStack::incDouble(double value, int offset)
{
    (m_valuePointer + offset)->d += value;
}

void VarValueStack::incInt(int value, int offset)
{
    (m_valuePointer + offset)->i += value;
}

void VarValueStack::incPosition(int offset)
{
    m_valuePointer += offset;
#ifdef QT_DEBUG
    m_i = getIndex();
#endif

}

int VarValueStack::getIndex() const
{
    return m_valuePointer - &m_values.first();
}

}
