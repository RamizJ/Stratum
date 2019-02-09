#include "VmGlobal.h"
#include "VmCodes.h"
#include "VirtualMachine.h"
#include "VmLog.h"
#include "VmContext.h"

#include <Class.h>
#include <Object.h>
#include <QTime>

using namespace StData;
using namespace StCompiler;

namespace StVirtualMachine {

std::shared_ptr<StCompiler::Compiler> compiler = nullptr;

QVector<vm_operation> operations;
QList<StData::ObjectVar*>* variables = nullptr;
QStack<ContextPtr> contextStack;

Project* executedProject = nullptr;
Object* executedObject = nullptr;
CodePointer* codePointer = nullptr;
VarValueStack* valueStack = nullptr;

ArrayManager* arrayManager = nullptr;
MatrixManager* matrixManager = nullptr;
SpaceWindowManager* windowManager = nullptr;
StreamManager* streamManager = nullptr;
MessageManager* messageManager = nullptr;
HyperJumpManager* hyperJumpManager = nullptr;

VirtualMachine* virtualMachine = nullptr;

QFontDatabase* fontDataBase;

QSet<int> pressedKeys;

QTime environmentTime;
quint32 execFlags = 0;

int lastPrimaryHandle = 0;
quint64 currentIterationNumber = 0;

/*----------------------------------------------------------------------*/

void execute(StData::Object* object, bool executeDisabledObject)
{
    if(VmContext::isMessageBoxRaised())
        return;

    for(int i = 0; i < object->childsCount(); i++)
    {
        Object* childObject = object->childAt(i);
        try
        {
            execute(childObject);
        }
        catch(const std::exception& e)
        {
            throw std::runtime_error(QString("Имидж: '%1'. ")
                                     .arg(childObject->cls()->originalName()).toStdString() + e.what());
        }
    }
    if(executeDisabledObject)
        executeObjectCode(object);

    else if(object->isExecutionEnabled())
        executeObjectCode(object);
}

void executeObjectCode(Object* object)
{
    if(executedObject != object)
    {
        executedObject = object;

        const QByteArray& vmCode = object->cls()->vmCode();
        if(vmCode.isEmpty())
            return;

        codePointer->setCodeBytes(vmCode);
        variables = &(object->variables());
    }
    else
        codePointer->reset();

    if(codePointer->getCodesCount() == 0)
        return;

    vm_operation operation;
    for(qint16 operationId; (operationId = codePointer->getCode()) != 0;)
    {
        codePointer->incPosition();
        if(operationId > 0 && operationId < operations.count())
        {
            operation = operations.at(operationId);
            if(operation)
                operation();
            else
                throw std::runtime_error(QString("VMOperation[%1] not implemented")
                                         .arg(operationId).toStdString());

            if(VmContext::isMessageBoxRaised())
                return;
        }
        else
            throw std::runtime_error("invalid VMOperation index");
    }
}

Object* getRootObject()
{
    if(executedProject)
        return executedProject->rootObject();

    return nullptr;
}

void funcNotImplemented(const QString& funcName)
{
    VmLog::instance().error(QString("'%1' - function not implemented. Call from image type: '%2'")
                            .arg(funcName).arg(executedObject->cls()->originalName()));
}

//void vmStop()
//{
//    virtualMachine->stop();
//}

}
