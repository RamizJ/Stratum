#include "VarStack.h"

#include <Class.h>
#include <Sc2000DataStream.h>
#include <VmCodes.h>

using namespace StData;

namespace StCompiler {

VarStack::VarStack()
{}

VarStack::~VarStack()
{}

void VarStack::push(Class* cl, qint16 pos)
{
    QStack::push(qMakePair(cl, pos));
}

Class* VarStack::pop(int& pos)
{
    QPair<Class*, qint16> pair = QStack::pop();

    pos = pair.second;
    return pair.first;
}

Class* VarStack::top()
{
    return QStack::top().first;
}

bool VarStack::setPtr(St2000DataStream& stream)
{
    QPair<Class*, qint16> topItem = QStack::top();

    qint16 pos = topItem.second;
    if(pos == -1)
        return false;

    int prevPos = stream.pos();

    stream.seek(pos);
    qint16 vmCode;
    stream >> vmCode;
    stream.seek(pos);

    switch(vmCode)
    {
        case PUSH_FLOAT :
        case PUSH_STRING :
        case vmPUSH_LONG :
            stream << qint16(PUSHPTR);
            break;

        case _PUSH_STRING :
        case _PUSH_FLOAT :
        case vm_PUSH_LONG :
            stream << qint16(PUSHPTRNEW);
            break;

        default :
            return false;
    }
    stream.seek(prevPos);
    return true;
}

}
