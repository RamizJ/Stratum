/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			_vmach.h
Description
			Virtual Machine internal header
Author:
			Alexander Shelemekhov
*/


#ifndef _VMACHINE_INT_H_
#define _VMACHINE_INT_H_

#include "stream.h"
#include "class.h"
#include "project.h"
#include "vmachine.h"
#include "vmcodes.h"
#include "../BC/bc_include_stratum/SCCALL.H"

extern WORD * v_code;
extern WORD* codepoint;
extern double * spf;    //стек переменных
extern PObject pobject; // текущий объект
extern BYTE invfunction;

#define spl  ((INT32*)spf)
#define spp  ((char**)spf)
#define spb  ((BYTE*)spf)


#define STRING_SIZE 256
//#define STRING_BUFFER_SIZE 32
#define MAX_DOUBLE   1.7e308
#define MIN_DOUBLE   -1.7e308

#define POP_DOUBLE (*spf++)
#define PUSH_DOUBLE(v) {spf--;*spf=(v);}

#define POP_PTR (*spp++)
#define PUSH_PTR(v) {spp--;*spp=(v);}

#define POP_LONG (*spl++)
#define POP_LONGi ((INT16)(*spl++))
#define POP_HSP   ((HSP2D)(*spl++))
#define POP_HSP3D ((HSP3D)(*spl++))
#define POP_HOBJ  ((HOBJ2D)(*spl++))

#define PUSH_LONG(v) {spl--;*spl=(v);}
#define PUSH_HSP(hsp) {spl--;*spl=((INT16)hsp);}
#define PUSH_HOBJ(obj) {spl--;*spl=((INT16)obj);}

typedef void (*VMPROC)();
extern VMPROC *commands;
extern "C" PObject SCGetObject(char* path);
char* GetTmpString();
#endif
