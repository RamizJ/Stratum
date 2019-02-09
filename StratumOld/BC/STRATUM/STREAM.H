/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      d:\bc\include\stratum\stream.h
			d:\sc3\stream.h
      d:\sc3\stream.cpp
Author:
			Alexander Shelemekhov
*/

#ifndef __sc__stream_h
#define __sc__stream_h
#ifdef WIN32
#define _BIDSFAR
#else
#define _BIDSFAR __far
#endif
#include <stratum\sccall.h>
#include <stratum\sc_type.h>

class _SCEXPCLASS TStream
{
	INT16 version;
public:
 	enum seek_modes {FROM_START=0,FROM_CURRENT=1, FROM_END=2};
	BYTE  status;
	BOOL  eof;

	TStream();
	virtual ~TStream();

	virtual void   Error(INT16);
	virtual void   Flush();
	virtual long   GetPos();
	virtual long   GetSize();
	char*          ReadStr();
	INT16          ReadStr(char*,INT16 bufsize=256);
	void           Reset();
	virtual long   Seek(long,seek_modes mode=FROM_START);
	virtual void   Truncate();
	virtual int 	 Read( void _BIDSFAR *buffer, int numBytes );
	virtual int 	 Write( const void _BIDSFAR *buffer, int );
#if !defined(WIN32)
	virtual long 	 Read( void __huge *buffer, long numBytes );
	virtual long 	 Write( const void __huge *buffer, long );
#endif
	INT16          WriteStr(char*);
	void           WriteDOSSTR(char*);
	BOOL           WriteWord(INT16);
	INT16          ReadWord();
	BOOL           WriteLong(long);
	long           ReadLong();
	long           CopyTo (TStream * ps,long start,long len);
	virtual BOOL   Eof();
	void           get(char&);
	char           peek();
	void inline    GoBack(){Seek(-1,FROM_CURRENT);}
	INT16 inline   GetVersion(){return version;}
	void  inline   SetVersion(INT16 v){version=v;}
};

typedef TStream * PStream;

//*----------------------------------------------------*

class TDOSStream : public TStream
{
public:
	BYTE temp;
	enum open_modes {stOpenRead = 0,stOpenWrite = 1,
					 stOpen = 2,stCreate = 3,stNoWarning=8,stNoEraseTmp=16};
private:
	HFILE  _HANDLE;
	char * filename;
	open_modes mode;
public:

  TDOSStream(char*,open_modes);
  ~TDOSStream();

	virtual void   Flush();
	virtual long   GetPos();
	virtual long   GetSize();
	virtual long   Seek(long,seek_modes mode=FROM_START);
	virtual void   Truncate();
	virtual int    Read( void _BIDSFAR *buffer, int numBytes );
	virtual int    Write( const void _BIDSFAR *buffer, int );
#if !defined(WIN32)
	virtual long   Read( void __huge *buffer, long numBytes );
	virtual long   Write( const void __huge *buffer, long );
#endif
	virtual void   Error(INT16);
	char *         GetFileName(){return filename;};
	BOOL           Overflow();
};

class _SCEXPCLASS TMemoryStream : public TStream
{
	HGLOBAL _HANDLE;
	BYTE flags;
	long size,pos,alloc;

#ifdef WIN32
	BOOL Update(int&,BOOL);
#else
	BOOL Update(long&,BOOL);
#endif
	BOOL ReAlloc(long);
public:
	Hpointer base;

	TMemoryStream(char*,BOOL del=TRUE);
	TMemoryStream(BOOL del,char*); //From File
	TMemoryStream(HGLOBAL);
	TMemoryStream(BOOL del=TRUE);
	TMemoryStream(void* b,int _size);
	~TMemoryStream();

	virtual void  Flush();
	virtual long  GetPos();
	virtual long  GetSize();
	virtual long  Seek(long,seek_modes mode=FROM_START);
	virtual void  Truncate();
	virtual int   Read( void _BIDSFAR *buffer, int numBytes );
	virtual int   Write( const void _BIDSFAR *buffer, int );
#if !defined(WIN32)
	virtual long  Read( void __huge *buffer, long numBytes );
	virtual long  Write( const void __huge *buffer, long );
#endif
	virtual void  Error(INT16);
	HGLOBAL       GetHANDLE();
	virtual BOOL  Eof();
};
#endif
