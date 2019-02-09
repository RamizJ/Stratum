#ifndef CONTEXT_H
#define CONTEXT_H

#include <Object.h>
#include <QStack>

namespace StVirtualMachine {

struct Context
{
    StData::Object* object;
    int codePosition;
    bool isInVirtualFunction;
    QList<StData::ObjectVar*>* variables;
};

bool pushContext(bool isInVirtualFunction = false);
void popContext();
void updateVars();

}


#endif // CONTEXT_H
