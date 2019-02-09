#include "IVirtualMachine.h"

namespace StInterfaces {

IVirtualMachine::IVirtualMachine(QObject* parent) :
    QObject(parent)
{}

void IVirtualMachine::stop()
{
    emit stopRequested();
}

}
