#include "Context.h"
#include "VmGlobal.h"

#include <Class.h>

using namespace StData;

namespace StVirtualMachine {

bool pushContext(bool isInVirtualFunction)
{
    ContextPtr context = std::make_shared<Context>();
    context->object = executedObject;
    context->codePosition = codePointer->getPosition();
    context->isInVirtualFunction = isInVirtualFunction;
    context->variables = variables;
    contextStack.push(context);

    if(executedObject == nullptr)
        return false;

    return (contextStack.count() <= 8);
}

void popContext()
{
    ContextPtr context = contextStack.pop();
    executedObject = context->object;

    if(executedObject != nullptr)
    {
        const QByteArray& vmCode = executedObject->cls()->vmCode();
        codePointer->setCodeBytes(vmCode);
        codePointer->setPosition(context->codePosition);

        variables = context->variables;
        if(!context->isInVirtualFunction)
            updateVars();
    }
}

void updateVars()
{
    variables = &(executedObject->variables());
}

}
