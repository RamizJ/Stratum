/*
Copyright (c) 1995  Virtual World
Module Name:
	 error_h.cpp
Author:
	 Alexander Shelemekhov
*/

#include "windows.h"
#include "error_h.h"
#include "_g_def.h"

static WORD lasterror=0;
static char log[80];

int _abstract(){return _Error(EM_abstract);}

int _notrealized(){return _Error(EM_notrealized);};

int _Error(int code)
{
	lasterror=(WORD)code;
	char info[255];
	if (LoadString(hInstance,code, info, sizeof(info)-1)==0)
  	lstrcpy(info,"Unknow Error!");
	code=MessageBox(0,info,
#ifdef WIN32
	"Gwin2d32.dll :Error !"
#else
	"Gwin2d16.dll :Error !"
#endif
	,MB_TASKMODAL|MB_ICONHAND);
	return 0;
};

int SetErrorLogFile(char* f)
 {  lstrcpy(log,f);
 return 0;};

_EXTERNC WORD _EXPORT GetLastGraphicError()
 {WORD i=lasterror;
  lasterror=0;
  return i;
 };
