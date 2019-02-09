#ifndef TOOLFUNCTIONS_H
#define TOOLFUNCTIONS_H

#include <QtGlobal>

namespace StVirtualMachine {

extern void setupToolFunctions();

extern void deleteTool2d();
extern void getNextTool2d();
extern void getToolRef2d();

//Color
extern void rgb();
extern void rgbEx();
extern void getRValue();
extern void getGValue();
extern void getBValue();

extern int getRGBA(quint8 r, quint8 g, quint8 b, quint8 a = 255);

}

#endif // TOOLFUNCTIONS_H
