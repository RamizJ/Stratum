#ifndef LOGICBINARYFUNCTIONS_H
#define LOGICBINARYFUNCTIONS_H

namespace StVirtualMachine {

extern void setupLogicBinaryFunctions();

extern void v_and();
extern void v_or();
extern void v_xor();
extern void v_not();
extern void v_andi();
extern void v_ori();
extern void v_noti();

extern void v_jmp();
extern void v_jz();
extern void v_jnz();
extern void v_jz_handle();
extern void v_jnz_handle();
// -------------------------------- LOGICAL FLOAT
extern void v_equal();
extern void v_notequal();
extern void v_more();
extern void v_moreorequal();
extern void v_low();
extern void v_loworequal();
extern void v_equali();
extern void v_notequali();
// -------------------------------- LOGICAL STRING
extern void s_equal();
extern void s_notequal();
extern void s_more();
extern void s_moreorequal();
extern void s_low();
extern void s_loworequal();

extern void v_and_binary();
extern void v_or_binary();
extern void v_shl();
extern void v_shr();
extern void v_notbin();
extern void v_xorbin();

}

#endif // LOGICBINARYFUNCTIONS_H
