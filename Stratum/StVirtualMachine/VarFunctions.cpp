#include "VarFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <ObjectVar.h>

using namespace StData;

namespace StVirtualMachine {

void setupVarFunctions()
{
    //Float
    operations[PUSH_FLOAT] = pushFloat;
    operations[_PUSH_FLOAT] = _pushFloat;
    operations[PUSH_FLOAT_CONST] = pushFloatConst;
    operations[PUSH_FLOAT_PTR] = pushFloatPtr;

    operations[_POP_FLOAT] = _popFloat;
    operations[_POP_FLOAT_OLD] = _popFloatOld;
    operations[POP_FLOAT_PTR] = popFloatPtr;

    //Long
    operations[vmPUSH_LONG] = pushLong;
    operations[vmPUSH_LONG_const] = pushLongConst;
    operations[vm_PUSH_LONG] = _pushLong;
    operations[vm_POP_LONG] = _popLong;
    operations[vm_POP_LONG_OLD] = _popLongOld;

    //String
    operations[PUSH_STRING] = pushString;
    operations[_PUSH_STRING] = _pushString;
    operations[PUSH_STRING_CONST] = pushStringConst;
    operations[_POP_STRING] = _popString;
    operations[_POP_STRING_OLD] = _popStringOld;

    //Convert
    operations[FLOAT_TO_LONG] = floatToLong;
    operations[LONG_TO_FLOAT] = longToFloat;

    operations[PUSHPTR] = pushPtr;
    operations[PUSHPTRNEW] = pushPtrNew;
}

void pushFloat()
{
    int varIndex = codePointer->toNextCode();
    ObjectVar* var = variables->at(varIndex);
    valueStack->pushDouble(var->savedDoubleValue());
}

void _pushFloat()
{
    ObjectVar* var = variables->at(codePointer->toNextCode());
    valueStack->pushDouble(var->doubleValue());
}

void pushFloatConst()
{
//    double value = *((double *)codePointer->getPtr());
    double value = codePointer->getDouble();
    valueStack->pushDouble(value);
    codePointer->incPosition(4);
}

//Когда срабатывает?
void pushFloatPtr()
{
//    double* value = (double *)(*((void**)codePointer->getPtr()));
//    valueStack->pushDouble(*value);
    codePointer->incPosition(2);
}

void _popFloat()
{
    ObjectVar* var = variables->at(codePointer->toNextCode());
    double d = valueStack->popDouble();
    var->setDouble(d);
}

void _popFloatOld()
{
    ObjectVar* var = variables->at(codePointer->getCode());
    var->setSavedDouble(valueStack->popDouble());
    codePointer->incPosition();
}

//Когда срабатывает?
void popFloatPtr()
{
//    double* value = (double*)(*((void**)codePointer->getPtr()));
//    *value = valueStack->popDouble();
    codePointer->incPosition(2);
}

void pushLong()
{
    ObjectVar* var = variables->at(codePointer->toNextCode());
    valueStack->pushInt32(var->savedIntValue());
}

void _pushLong()
{
    ObjectVar* var = variables->at(codePointer->toNextCode());
    valueStack->pushInt32(var->intValue());
}

void pushLongConst()
{
//    qint32 i = *((qint32*)(codePointer->getPtr()));
    qint32 i = codePointer->getInt32();
    valueStack->pushInt32(i);
    codePointer->incPosition(2);
}

void _popLongOld()
{
    ObjectVar* var = variables->at(codePointer->getCode());
    var->setSavedInt(valueStack->popInt32());
    codePointer->incPosition();
}

void _popLong()
{
    ObjectVar* var = variables->at(codePointer->getCode());
    var->setInt(valueStack->popInt32());
    codePointer->incPosition();
}

void pushString()
{
    ObjectVar* var = variables->at(codePointer->getCode());
//    int index = valueStack->getIndex();
    valueStack->pushString(var->savedStringValue());
    codePointer->incPosition();
}

void _pushString()
{
    ObjectVar* var = variables->at(codePointer->getCode());
//    int index = valueStack->getIndex();
    valueStack->pushString(var->stringValue());
    codePointer->incPosition();
}

void pushStringConst()
{
    QString str = codePointer->getString();
    valueStack->pushString(str);

//    char* charPtr = (char*)(codePointer->getPtr(1));
//    valueStack->pushString(QString::fromLocal8Bit(charPtr));

    codePointer->incPosition(codePointer->getCode() + 1);
}

void _popString()
{
    ObjectVar* var = variables->at(codePointer->getCode());
    QString str = valueStack->popString();
    var->setString(str);
    codePointer->incPosition();
}

void _popStringOld()
{
    ObjectVar* var = variables->at(codePointer->getCode());
    var->setSavedString(valueStack->popString());
    codePointer->incPosition();
}

void floatToLong()
{
    qint32 value = (qint32)valueStack->popDouble();
    valueStack->pushInt32(value);
}

void longToFloat()
{
    double value = valueStack->popInt32();
    valueStack->pushDouble(value);
}

void pushPtr()
{
    VarData* varData = variables->at(codePointer->getCode())->savedVarData();
    valueStack->pushVarData(varData);
    codePointer->incPosition();

//    QString str = variables->at(codePointer->getCode())->stringValue();
//    valueStack->pushString(str);
//    codePointer->incPosition();
}

void pushPtrNew()
{
    qint16 i = codePointer->getCode();
    VarData* var = variables->at(i)->varData().get();
    valueStack->pushVarData(var);
    codePointer->incPosition();
}


}
