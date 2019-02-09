/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
		  dbe.h   // data base engine internal header
Author:
			Alexander Shelemekhov
*/

#ifndef __DBE_I_SC3_H_
#define __DBE_I_SC3_H_

#include "stream.h"
#include "stream_m.h"
#include "cursor.h"

class THandlers:public TCollection{
public:
 inline  C_TYPE IbyH(int h){return (C_TYPE)(table?(h-256):(h-16384));};
 inline  int HbyI(C_TYPE i){return table?(256+i):(16384+i);};
 int table;
 THandlers():TCollection(10,10){};
 virtual C_TYPE Insert(pointer item);
 pointer Get(int handle);
 virtual void 	FreeItem(pointer );
 int DBEdel(int h);
};

class TTableWnd{
public:
 TTableWnd * next;
 HWND hwnd;
};

class TBase{
 public:
 hDBIDb  hDb;
 hDBIXact hXact; // дескриптор транзакции
 void * project;
 BOOL autoclose;
 int  _handle;
 TBase(hDBIDb _hDb);
 BeginTran(eXILType);
 EndTran(eXEnd);
 ~TBase();
};
const TF_MODIFY=1;
class TTable{
public:
BOOL autoclose;
BOOL indexactive;
TTableWnd *browsers;
enum UpdateMode{UM_NONE=0,UM_MODIFY,UM_INSERT,UM_APPEND};
hDBICur  hCursor;
pBYTE    pRecBuf;
CURProps curProps;
pFLDDesc pFldArray;
RECProps recProps;
pVOID    ppLdObj;     //Language driver
BOOL     customdriver;// драйвер быз загружен в SetCodePage

UpdateMode um;
UINT32    position;
BOOL      posvalid;
BOOL      empty;
// для browse
pBYTE     mark;
UINT32    last;
BOOL      lastok;
UINT32    posbefore;
BOOL      wasbegin;
//--------------------
 DBIOpenMode openmode;
 DBIShareMode sharemode;

BOOL autoflush;

TBase* base;
TTable(hDBICur  hCur,TBase* _base);
~TTable();
void Flush();
void ReOpen();
void GetText(INT16 id,char *out); // if id ==-2 Deleteflag
BOOL SetText(INT16 id,char *in);
BOOL  _SetText(INT16 id,pBYTE _pRecBuf,char *in);
BOOL SetToKey(char *key);

double GetDouble(INT16 id);
BOOL SetDouble(INT16 id,double d);
void GetNext();
void GetPrior();
void GetRelative(long offset);
void GetTop();
void GetBottom();
int DeleteRecord(BOOL pack);
INT16 GetFId(char * name);

//** BLOB **
BOOL ReadBlob(int id,TStream*st);
BOOL WriteBlob(int id,TStream*st);
BOOL FreeBlob(int id);
//** BLOB **

int AddIndex(char * name, UINT16 IndexId, char * tag, UINT16 Props, char ** Fields,int count, UINT16 keytype, char* keyexpr,char* keycond); // Writed by AK
int DeleteIndex(char * name, UINT16 IndexId, char * tag); // Writed by AK
int OpenIndex(char * name, UINT16 IndexId); // Writed by AK
int SwitchToIndex(char  * name, UINT16 IndexId, char *tag, BOOL bCurrRec);// Writed by AK
int CloseIndex(char * name);// Writed by AK
int RegenIndex(char* name,char* tag, UINT16 IndexId);// Writed by AK
int PackBase();
BOOL InsertWindow(HWND);
BOOL RemoveWindow(HWND);
void NotifyWindows(UPDATE_BROWSE code,long y,long x);//Обновляет окна
BOOL IsDBASE();
BOOL SetDeleteMode(BOOL );
BOOL UnDeleteRecord();
};



hDBIDb _GeBase(int h);
hDBICur _GetCursor(int h);

TBase*  GetBase(int h);
TTable* GetCursor(int h);

THandlers extern * bases;
THandlers extern * tables;
#endif //__DBE_I_SC3_H_
