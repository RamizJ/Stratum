/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			_class.h
Author:
			Alexander Shelemekhov
Description:
         Internal class definition
*/
#if !defined(_CLASS_I_H)
#define _CLASS_I_H
#include <gwin2d3d\gwinfunc.h>
#include "memmgr.h"
#include "stream.h"
#include "class.h"
#include "equ.h"
#include "project.h"
#include "records.h"
#include "vardata.h"
#include "vmachine.h"
#include "filetype.h"
#include "dllman.h"
#include <stratum\stratum.h>
#include <stratum\sc_msg.h>

class TClassCollection:public TSortedCollection
{
public:
  TClassCollection(WORD asize,WORD adelta):TSortedCollection(asize,adelta){};
 	virtual int   Compare(pointer key1,pointer key2);
 	virtual void 	FreeItem(pointer );
};

class TLibCollection : public TSortedCollection
{
public:
  TLibCollection(WORD asize,WORD adelta):TSortedCollection(asize,adelta){};
  virtual int       Compare(pointer key1,pointer key2);
  virtual C_TYPE  	IndexOf(pointer item);
};

void CloseProjectWindows(void * project);
TLibrary*AddDirPath(char*,BYTE=0);
void AddDir(TLibrary*,BYTE=0);
void AddDirEx(TLibrary*, BOOL chk = TRUE, BYTE = 0);
void BeginChkAddDir();
void EndChkAddDir();

BYTE extern sysflags;
PClass extern changedclass;
TLibrary extern *librarys;
TLibrary extern *DefaultLib;
char extern scheme_path[128]; // ѕуть к иконкам
TClassCollection extern * classes;

#endif
