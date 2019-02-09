#ifndef STREAMFUNCTIONS_H
#define STREAMFUNCTIONS_H

namespace StVirtualMachine {

extern void setupStreamFunctions();

extern void v_createstream();
extern void v_closestream();

extern void v_seek();
extern void v_streamstatus();
extern void v_eof();
extern void v_getpos();
extern void v_getsize();
extern void v_setwidth();

extern void v_fread();
extern void v_sread();
extern void v_getline();

extern void v_fwrite();
extern void v_copyblock();
extern void v_truncate();
extern void v_swrite();

}


#endif // STREAMFUNCTIONS_H
