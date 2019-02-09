#ifndef VARFUNCTIONS_H
#define VARFUNCTIONS_H

namespace StVirtualMachine {

extern void setupVarFunctions();

extern void pushFloat();
extern void _pushFloat();
extern void pushFloatConst();
extern void pushFloatPtr();

extern void _popFloat();
extern void _popFloatOld();
extern void popFloatPtr();

//Long
extern void pushLong();
extern void _pushLong();
extern void pushLongConst();

extern void _popLongOld();
extern void _popLong();

//String
extern void pushString();
extern void _pushString();
extern void pushStringConst();

extern void _popString();
extern void _popStringOld();

//Conversion
extern void floatToLong();
extern void longToFloat();

extern void pushPtr();
extern void pushPtrNew();

}

#endif // VARFUNCTIONS_H
