#ifndef MATRIXFUNCTIONS_H
#define MATRIXFUNCTIONS_H

namespace StVirtualMachine {

extern void setupMatrixFunctions();

extern void mcreate();
extern void mdelete();
extern void v_mfill();
extern void v_mdiag();
extern void v_maddx();
extern void v_msubx();
extern void v_mmulx();
extern void v_mdivx();
extern void v_med();
extern void v_mdelta();
extern void v_mdet();
extern void v_transp();
extern void v_maddc();
extern void v_mnot();
extern void v_msum();
extern void v_msubc();
extern void v_mmulc();
extern void v_mdivc();
extern void v_mmul();
extern void v_mglue();
extern void v_mcut();
extern void v_mmove();
extern void v_mobr();
extern void v_mput();
extern void v_mget();
extern void v_meditor();
extern void v_msaveas();
extern void v_mload();
extern void v_msort();
extern void v_mdim();
}

#endif // MATRIXFUNCTIONS_H
