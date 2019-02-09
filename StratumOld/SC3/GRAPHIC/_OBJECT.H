/*
Copyright (c) 1995  Virtual World
Module Name:
         _object.h
Author:
         Alexander Shelemekhov
*/

#ifndef ___object_h
#define ___object_h
#ifndef __GRAPH_DLL_TYPES
#include "gwin2d3d\gwintype.h"
#endif
#ifndef __WINDOWS_H
#include "windows.h"
#endif
#include "error_h.h"
//#include "win_type.h"

#ifndef  MK_FP
#if !defined(WIN32)
#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
#else
#define MK_FP( seg,ofs )((void*)(((UINT16)seg)<<16)|(((UINT16)ofs)))
#endif
#endif

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))
//#define _check_for__object


#define currentfileversion 0x0300
// from 0x0201  local 3d coordinates
// from 0x0203  размеры шритов для RTF
// from 0x0300  появились CHUNK (размеры на каждую запись)

#if !defined(WIN32)
#define _BIDSFAR __far
#else
#define _BIDSFAR
#endif


typedef void * pointer;
class _OBJECT;
char * NewStr(char * astr);
//*----------------------------------------------------*
class TStream;

struct CHUNK
{
    INT16 code;
    INT32 size;
    long  _pos;// для записи или чтения
    TStream* st;

    void InitRead(TStream* st);
    void InitWrite(TStream* st, INT16 _code);
    void DoneWrite();
    BOOL Check();
    void WriteStr(TStream* st,INT16 _code,char*); // сразу пишет CHUNK, или непишет если строка пустая
    char*ReadStr();   // чиатет строку но после InitRead
};
//*----------------------------------------------------*
class TStream
{
public:
    INT16 fileversion;
    int status;
    //HCURSOR _old_cur;
    //HCURSOR _cur_wait;
    TStream();
    virtual ~TStream();
    virtual void        Error(int);
    virtual void        Flush();
    pointer             Get(INT16 id=0);
    virtual long        GetPos();
    virtual long        GetSize();
    void                Put(pointer);
    char*               ReadStr();
    void                Reset();
    virtual long        Seek(long);
    virtual void        Truncate();
    virtual int Read( void _BIDSFAR *buffer, int numBytes );
    virtual int Write( const void _BIDSFAR *buffer, int );

#if !defined(WIN32)
    virtual long Read( void __huge *buffer, long numBytes );
    virtual long Write( const void __huge *buffer, long );
#endif

    int                 WriteStr(char*);

    INT16              WriteWord(INT16);
    INT16              ReadWord();
    UINT32             ReadLong();
    UINT32             WriteLong(UINT32);

    void               WritePOINT(LPPOINT2D);
    void               ReadPOINT(LPPOINT2D);

    void               WritePOINTs(LPPOINT2D,INT16 count);
    void               ReadPOINTs(LPPOINT2D,INT16 count);

    _OBJECT*            AllocateObject(CHUNK&);
    long        CopyTo (TStream * ps,long start,long len);
};
typedef TStream * PStream;
//*----------------------------------------------------*

class TDOSStream : public TStream{
#ifdef WIN32
    HFILE _HANDLE;
#else
    UINT _HANDLE;
#endif

    BOOL temp;
    char * filename;
public:
    enum open_modes {stOpenRead = 0,stOpenWrite = 1,
                     stOpen = 2,stCreate = 3};
    TDOSStream(char*,int);
    ~TDOSStream();
    virtual void        Flush();
    virtual long        GetPos();
    virtual long        GetSize();
    virtual long        Seek(long);
    virtual void        Truncate();
    virtual int Read( void _BIDSFAR *buffer, int numBytes );
    virtual int Write( const void _BIDSFAR *buffer, int );

#if !defined(WIN32)
    virtual long Read( void __huge *buffer, long numBytes );
    virtual long Write( const void __huge *buffer, long );
#endif
};

class TMemoryStream : public TStream{
    HGLOBAL _HANDLE;
#ifdef WIN32
    void  * base;
#else
    void __huge * base;
#endif
    BYTE flags;
    long size,pos,alloc;
    BOOL Update(long,BOOL);
    BOOL ReAlloc(long);
public:
    TMemoryStream(char*,BOOL del=TRUE);
    TMemoryStream(HGLOBAL);
    TMemoryStream(BOOL del=TRUE);
    ~TMemoryStream();

    virtual void        Flush();
    virtual long        GetPos();
    virtual long        GetSize();
    virtual long        Seek(long);
    virtual void        Truncate();
    virtual int Read( void _BIDSFAR *buffer, int numBytes );
    virtual int Write( const void _BIDSFAR *buffer, int );

#if !defined(WIN32)
    virtual long Read( void __huge *buffer, long numBytes );
    virtual long Write( const void __huge *buffer, long );
#endif

    virtual void        Error(int);
    HGLOBAL GetHANDLE();
};

//*----------------------------------------------------*
class _OBJECT
{

public:
#ifdef _check_for__object
    static int _ref__object;
#endif

    _OBJECT();
    virtual ~_OBJECT();
    virtual WORD 		  WhoIsIt(){return (WORD)_abstract();}
    virtual void 		  Store(PStream){};
    virtual void    PostStore(PStream){}; // осуществляет запсь необязательных данных общих для детей
    virtual BOOL    ReadChunk(CHUNK&){};  // осуществляет чтение необязательных данных общих для детей
};

//*----------------------------------------------------*
#endif
