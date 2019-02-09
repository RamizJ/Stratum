/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			auto dbengine.h   // data base engine
Author:
			Alexander Shelemekhov
*/
#ifdef DBE
#ifndef __DBE_SC3_H_
#define __DBE_SC3_H_
#include "cursor.h"

struct SEARCH_in_CURSOR{
char scKey[256];
BYTE scCase;
BYTE scCurrent;
BYTE scFromStart;
BYTE scDirection;
BYTE scCanAgain;
BYTE scFragment;
};

int DBDoneSQL();
int DBInitSQL();
int DBSys(int i,int j);
// 1 - lang drivers list

int DBOpenDataBase(char *name,char*type,char * password,char * flags);
int DBSetDir(int base,char*dir);

int DBOpenTable(int base,char*name,char*type,char *index,char*indextag,
				 UINT indexId,char * flags);
int DBGetLastError();
int DBGetTextError(int,char *);
int DBCloseTable(int );
int DBCloseBase(int );
void DBCloseAll(void *project);

int DBGoTop(int table);
int DBGoBottom(int table);
int DBSkip(int table,long delta);
int DBSkipTo(int table,long delta);
long DBRecno(int table);

int DBGetFId(int table,char*field);
int DBGetField(int table,INT16 id,char*field);
int DBGetField(int table,INT16 id,double*rez);
int DBGetField(int table,char *name,char*field);
int DBGetField(int table,char *name,double*rez);

int    DBSQL(int base,char * query,int type);
double DBGetCount(int h);
void   DBFieldName(int h,INT16 id,char*);
int    DBFieldType(int h,INT16 id);
UINT   DBFieldCount(int h);

int DBInsertRecord(int table);
int DBAppendRecord(int table);
int DBDeleteRecord(int table,BOOL pack=0);
int DBSetField(int table,INT16 id,char*field);
int DBSetField(int table,INT16 id,double field);
int DBSetField(int table,char *name,char*field);
int DBSetField(int table,char *name,double field);

int DBZap(int table);
BOOL DbPackTable(int table);
BOOL DBSort(int table,UINT16 flags,int count,char **names);
int DBCreateTable(int base,char *name,char * driver,char *desc,int Overwrite);
BOOL IsTableExist(int table);

int DBWriteBlob(int table,int id,int stream);
int DBReadBlob(int table,int id,int stream);
int DBFreeBlob(int,int id);
//*********************** Index functions **********************//
int DBAddIndex(int table,char * name, UINT16 IndexId, char * tag, UINT16 Props, char ** Fields,int count, UINT16 keytype, char* keyexpr,char* keycond);// Writed by AK
int DBDeleteIndex(int table,char  * name, UINT16 IndexId, char *tag); // Writed by AK
int DBCloseIndex(int table,char  * name);
int DBOpenIndex(int table, char * name, UINT16 IndexId);// Writed by AK
int DBSwitchToIndex(int table, char  * name, UINT16 IndexId, char *tag, BOOL bCurrRec);// Writed by AK
int DBRegenIndex(int table,char  * name, char *tag, UINT16 IndexId);// Writed by AK
int DBSetToKey(int table,char*);


/*
int DBAddIndex(int table,char * ??????);
int DBDeleteIndex(int table,char  * name,char *tag);
int DBCloseIndex(int table,char  * name);
int DBOpenIndex(int table,char  * name);
int DBRegenIndex(int table,char  * name,char *tag);
int DBSwitchToIndex(int table,char  * name,char *tag,BOOL bCurrRec);
DBIResult DBIFN DbiSwitchToIndex (phCursor, pszIndexName, pszTagName, iIndexId, bCurrRec);


// Поиск
DBIResult DBIFN DbiSetToKey (hCursor, eSearchCond, bDirectKey, [iFields], [iLen], pBuf);

// Фильтр

 DBIResult DBIFN DbiActivateFilter (hCursor, [hFilter]);
 DBIResult DBIFN DbiAddFilter (hCursor, [iClientData], [iPriority], [bCanAbort], pcanExpr, [pfFilter], phFilter);
 DBIResult DBIFN DbiDeactivateFilter (hCursor, [hFilter]);
 DBIResult DBIFN DbiDropFilter (hCursor, [hFilter]);

*/
//**************************************************************//
int DBGetFLen(int h,INT16 id); // S
int DBBeginDraw(int table); // S
int DBDraw(int table,long y,long x,char *,COLORREF&); // S
int DBEndDraw(int table); // S


int DBAttachWindow  (int,HWND);
int DBDeAttachWindow(int,HWND);
int DBSetFlushMode(int ,int fm);//fm -flush auto
int __InsertBDEObj(HWND tree,int type,DWORD parent);

void DBSQLPParam(int&);   // &1 - loaded  **** &2 - bases not empty

void DBFillInfoDialog(HWND,int table,int mode);
int  DBhandleByPointer(UINT32);

BOOL    DBGetDeleteMode(int tTable);
BOOL    DBSetDeleteMode(int tTable,BOOL mode);
UINT16  DBGetCodePage(int tTable);
UINT16  DBSetCodePage(int tTable,UINT16 page);

BOOL    DBLock(int table,BYTE m);
BOOL    DBUnLock(int table,BYTE m);
BOOL    DBUndeleteRecord(int table);
BOOL    DBCopyTo(int table,char *file,char *type);
BYTE    DBIsFieldReadOnly(int table,int id);

int     DBGetNextBase(int );
int     DBGetNextTable(int ,int);
BOOL    DBGetTableDesc(int h,char*s);
BOOL    DBGetBaseDesc(int h,char*s);

BOOL DbiGetSysVersion(char* SysVer);
#ifndef fldUNKNOWN

#define fldUNKNOWN      0
#define fldZSTRING      1                 // Null terminated string
#define fldDATE         2                 // Date     (32 bit)
#define fldBLOB         3                 // Blob
#define fldBOOL         4                 // Boolean  (16 bit)
#define fldINT16        5                 // 16 bit signed number
#define fldINT32        6                 // 32 bit signed number
#define fldFLOAT        7                 // 64 bit floating point
#define fldBCD          8                 // BCD
#define fldBYTES        9                 // Fixed number of bytes
#define fldTIME         10                // Time        (32 bit)
#define fldTIMESTAMP    11                // Time-stamp  (64 bit)
#define fldUINT16       12                // Unsigned 16 bit integer
#define fldUINT32       13                // Unsigned 32 bit integer
#define fldFLOATIEEE    14                // 80-bit IEEE float
#define fldVARBYTES     15                // Length prefixed var bytes
#define fldLOCKINFO     16                // Look for LOCKINFO typedef

#endif

#endif
#endif