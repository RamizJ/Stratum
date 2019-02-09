/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			auto sintes.h     (autosntz.h)
Author:
			Alexander Shelemekhov
*/
#ifndef __AUTO_SNTZ_SC3_H_
#define __AUTO_SNTZ_SC3_H_

HOBJ2D Vm_AddChildren(char * parent,char * name,char * alias,gr_float x,gr_float y); //H
BOOL   Vm_RemoveChildren(char * parent,HOBJ2D id,BOOL noquescion,PObject po);
BOOL   Vm_CreateClass(char * newclass,char * _template,UINT32 flags);
void   Vm_DeleteClass(char * classname);
HSP2D  Vm_OpenClassScheme(char * classname,UINT32 flags); //H
BOOL   Vm_CloseClassScheme(char * classname);
HOBJ2D Vm_CreateLink(char * parent,HOBJ2D id1,HOBJ2D id2); //H
BOOL   Vm_SetLinkVars(char * parent,HOBJ2D id /*H*/,char *text);
BOOL   Vm_RemoveLink(char * parent,HOBJ2D id1); //H
BOOL   GetUniqueClassName(char *n1,char *n2,BOOL copy=FALSE);
HOBJ2D Vm_CreateVarPlace(char * _class,gr_float x,gr_float y);
// Промежуточные
HSP2D __GetClassScheme(PClass _class,BOOL & close);
void __CloseClassScheme(PClass _class,int close);

#define __OPENSCHEME(__c_) int __close;\
HSP2D hsp=__GetClassScheme(__c_,__close);\
if (!hsp)return 0;

#endif //__AUTO_SNTZ_SC3_H_
