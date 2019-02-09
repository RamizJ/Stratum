/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
                        memmgr.h
Description:
                        Solid-linear memory manager
Author:
                        Alexander Shelemekhov
*/
#ifndef _SOLID_MEM_MGR_H_
#define _SOLID_MEM_MGR_H_

class TMemBlockHeader;

class TMemoryManager
{
    long flags;
    long blocksize;
    WORD lostinblock;
    char* pos;
    long size;
    long lost;

    TMemBlockHeader* first;
    TMemBlockHeader* current;

    void AllocNewBlock();
    void DoneBlocks();

public:
    enum {NO_ZEROINIT, ZEROINIT};
    TMemoryManager(long flags = NO_ZEROINIT, long _blocksize = 64000l);
    ~TMemoryManager();

    void* GetMem(WORD size);
    char* NewStr(char*);
    void* GetMemZ(WORD size);
    bool FreeMem(void*);
    long GetPos();
    BOOL Release(long pos=0);
    void ClearAll();
    DWORD GetSize(){return size;}
};

#endif
