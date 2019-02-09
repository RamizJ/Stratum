#ifndef STRINGFUNCTIONS_H
#define STRINGFUNCTIONS_H

namespace StVirtualMachine {

extern void setupStringFunctions();

extern void lengthString();
extern void plusString();
extern void leftString();
extern void rightString();
extern void substrString();
extern void substr1String();

extern void posString();
extern void replicateString();
extern void schange();

extern void lowerString();
extern void upperString();

extern void ansiToOem();
extern void oemToAnsi();

extern void compareString();
extern void compareiString();

extern void trimLeftString();
extern void trimRightString();
extern void trimAllString();

extern void asciiString();
extern void chrString();

extern void floatToString();
extern void stringToFloat();

extern void plusStringFloat();
extern void plusFloatString();

}

#endif // STRINGFUNCTIONS_H
