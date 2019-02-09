#ifndef ARRAYFUNCTIONS_H
#define ARRAYFUNCTIONS_H

namespace StVirtualMachine {

extern void setupArrayFunctions();

extern void dm_new();
extern void dm_delete();
extern void dm_clearall();
extern void dm_vinsert();
extern void dm_vdelete();
extern void dm_vgetcount();
extern void dm_gettype();

extern void dm_vsetf();
extern void dm_vseth();
extern void dm_vsets();

extern void dm_vgetf();
extern void dm_vgeth();
extern void dm_vgets();

extern void dm_save();
extern void dm_load();

extern void dm_sort();

}

#endif // ARRAYFUNCTIONS_H
