/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      stream_m.h
      stream_m.cpp
Author:
      Alexander Shelemekhov
Description:
      Manager for user defined streams
*/

#ifndef ___STREAM_M_h
#define ___STREAM_M_h

#define MAX_STREAM 64
#define MAX_CLASSTEXTSIZE 16384
#define MAX_DOS_STREAM 20

enum STREAM_TYPE
{
    ST_NULL=1,
    ST_FILE,
    ST_MEMORY,
    ST_PIPE,
    ST_NET,
    ST_MIDI,
    ST_SNDFX,
    ST_SERIAL,
    ST_LPT
};

class TStreamItem
{
public:
    TStream 		*stream;
    void 				*project;
    int 				HANDLE;
    int 				Width;
    STREAM_TYPE type;

    TStreamItem();
    TStreamItem(int handle, STREAM_TYPE, TStream *);
    ~TStreamItem();

    virtual long Seek(long);
    virtual void Flush(int);
    virtual long GetPos();
    virtual long Read(void *, int count);
    virtual long Write(void *, int count);
    char*GetTypeStr();
};

class TStreamMan: public TSortedCollection
{
    TStreamItem _item;
public:
    TStreamMan();
    ~TStreamMan();

    BOOL Error(int);
    int Write(int h, void *, int size);

    int FRead(int h, double *);
    int Read(int h, void *, int size);

    long Seek(int h, long);
    long GetPos(int h);
    long GetSize(int h);

    int Eof(int h);

    int CreateStream(char*device, char*name, char*add);
    int CloseStream(int h);
    void CloseAll(void *);

    int GetType(int h, char *);

    int Status(int h);

    int SetWidth(int h, int);

    int FWrite(int h, double);
    int SWrite(int h, char *);
    // int  SRead(int h,char*);

    int GetLine(int h, char *, int = 255, char = '\n');
    int GetAllText(int h, char*);

    TStream* GetStream(int handle);

private:
    TStreamItem*_GetStream(int handle);
    virtual int Compare(pointer key1, pointer key2);
    virtual void FreeItem(pointer);
    BOOL CanCreateDOS();
};
TStreamMan extern* streams;

#endif
