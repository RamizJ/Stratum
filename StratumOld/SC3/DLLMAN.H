/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
                        dllman.h
Author:
                        Alexander Shelemekhov
*/
#ifndef __DLLS__SC3_H_
#define __DLLS__SC3_H_
#if !defined(__CLASS__SC3_H_)
#include "class.h"
#endif
#ifndef MAX_STRING
#define MAX_STRING 256
#endif

typedef UINT32 (CALLBACK* SCDLLPROC)(UINT16 code,UINT32 p1,UINT32 p2);

class TFCollection;
class ID;

class TDLLFunction
{
public:
    TFCollection* pdll;
    char* Name;
    char* RealName;
    INT16 Type;
    INT16 arg;
    INT16 retsize;
    INT16 argsize;
    INT16 retconv;
    INT32 pointers;
    unsigned char argconv[64];
    void far* pproc;
    PClassListInfo ret;
    PClassListInfo* pType;

    TDLLFunction(TFCollection*, ID&, char*,INT16);
    ~TDLLFunction();

    // INT16 Load(ID&,char*,INT16);
    INT16 Init(HINSTANCE);
    BOOL Call(BYTE**stuck);
    INT16 GetCType(ID&);
    INT16 GetTSize(INT16);
};

class TFCollection : public TSortedCollection
{
public:
    SCDLLPROC _proc;

    char dllname[MAX_STRING];
    char *tdl;

    HINSTANCE hdll;

    DWORD data;
    TFCollection(char* name,DWORD dt,WORD asize = 64,WORD adelta = 32) : TSortedCollection(asize,adelta)
    {
        lstrcpy(dllname,name);
        data = dt;
        hdll = 0;
        _proc = 0;
    }
    ~TFCollection();

    INT16 Load(ID&,INT16 tp);
    INT16 Init();
    INT16 Free();
    UINT32 Procedure(UINT16 code,UINT32 p1,UINT32 p2);
    virtual int Compare(pointer key1,pointer key2);
    virtual void FreeItem(pointer );
    C_TYPE SSearch(char*,C_TYPE&);
};

class TDLLManager
{
public:
    TCollection* dlls;
    TCollection * messages;

    TDLLManager();
    ~TDLLManager();

    INT16 LoadDLL(char*,DWORD data = 0);
    INT16 InitDLL(DWORD);

    INT16 FreeDLL(DWORD);
    BOOL  FreeDLL(TFCollection* ptr,BOOL ask=FALSE);

    TFCollection* Search(char*);
    TDLLFunction* SearchFunction(char*);
    BOOL ForEach(UINT16 code,UINT32 p1=0,UINT32 p2=0);
    BOOL FirstThat(UINT16 code,UINT32 p1=0,UINT32 p2=0);
    TFCollection *LoadPlugin(char*,DWORD data = 0);
};
TDLLManager extern* dllmanager;

class TProject;
HINSTANCE ScLoadLibrary(char*, TProject*prj=NULL, char* jaddpath=NULL);
char extern * _plugins;
#endif
