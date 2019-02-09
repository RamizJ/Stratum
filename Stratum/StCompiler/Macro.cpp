#include "Macro.h"
#include "TplCursor.h"

#include <QDebug>

namespace StCompiler {

Macro::Macro(const QString& name, double value) :
    m_name(name),
    m_value(value)
{}

Macro::~Macro()
{}

void Macro::load(TplCursor& cursor)
{
    if(cursor.type() != OperatorType::opnd)
        qCritical() << "error while reading const from constant.tpl";

    m_name = cursor.string();
//    qDebug() << "load constant: " << m_name;

    cursor.read();
    switch (cursor.type())
    {
        case OperatorType::dconst:
        case OperatorType::fconst:
            m_value = cursor.string().toDouble();
            break;

        case OperatorType::hconst:
            m_value = cursor.string().toInt(0, 16);
            break;

        default:
            qCritical() << "error type for constant:" << m_name;
            break;
    }
}

}
