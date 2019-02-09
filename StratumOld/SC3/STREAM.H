/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      stream.h
      d:\bc\include\stratum\stream.h
      d:\sc3\stream.cpp
Author:
      Alexander Shelemekhov
*/

#ifndef ___STREAM_h
#define ___STREAM_h
#ifndef __GRAPH_DLL_TYPES
#include <gwin2d3d\gwintype.h>
#endif
#include "errors.h"
#ifdef WIN32
#define MK_FP(seg,ofs) (DWORD)(((seg)<<16)|(ofs))
#else
#define MK_FP(seg,ofs) ((void __seg *)(seg) + (void __near *)(ofs))
#endif
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))
//#define _check_for__object

#include <stratum\stream.h>

char *NewStr(char*astr);
//*----------------------------------------------------*

INT16 _GetTempFileName(char*path, char*s, INT16 extsize, char*ext);
BOOL GetPath(char*file, char*path);
int IsAbsolutePath(char*file);
BOOL GetRelativePath(char*base, char*file, char*out);
BOOL GetAbsoluteName(char*base, char*relative, char*full);
// 0 - ќшибка
// 1 - Absolute
// 2 - relative
void AddSlash(char*p);
void RemoveSlash(char*path);
BOOL MakeDirectory(char*dir);
BOOL DeleteDirectory(char*dir);
BOOL ScRenameFile(char*oldname, char*newname);
BOOL ScDeleteFile(char*name);
BOOL ScFileExist(char*name);
BOOL ScDeleteDir(char*name);
BOOL ScCopyFile(char*from, char*to);
BOOL IsDirEmpty(char*dir);
BOOL IsDirExist(char*dir);
#ifdef FILE_ITERATOR
#include <dos.h>

class TFileIterator
{
#ifdef WIN32
  WIN32_FIND_DATA fdata;
  HANDLE fhandle;
#else
  find_t ffblk;
#endif
  BOOL started;
  char *path;
  BOOL ok;
  BOOL dir;
  char *mask;

public:
  TFileIterator(char*_path, char*_mask, BOOL dir=FALSE);
#ifdef WIN32
  ~TFileIterator();
#endif

  BOOL 	GetNext();
  char*	GetFileName();
  void 	GetFullFileName(char *);
  BOOL 	GetFileTime();
  BOOL 	IsReadOnly();
  long 	GetFileSize();
};
#endif
#endif
