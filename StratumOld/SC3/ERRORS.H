/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			errors.h
Author:
			Alexander Shelemekhov
*/
#ifndef __ERROR_HANDLER__
#define __ERROR_HANDLER__
#include "errors.rh"
#ifndef __GRAPH_DLL_TYPES
#include <gwin2d3d\gwintype.h>
#endif
int  _Error(int code,int=0,void * data=NULL);
int  GroupFuncError(int code,int group,char*add=NULL);
void _Message(char*);
void _Message(int,int=0,void * data=NULL);
INT16  _abstract();
INT16   GetLastScError();
void  SetLastScError(INT16);
void InitErrorHandler();
void DoneErrorHandler();
void InitMathErrorHandler();
void _SelfMathCatcher(int c2);
#endif
