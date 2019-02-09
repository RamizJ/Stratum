// Meta file support 
#ifndef _H_META_DEF_
#define _H_META_DEF_
#define EM_internal 1
#define currentmetaversion 1

typedef INT16 (CALLBACK* PROCESSMETAPROC)(PStream stream,INT16 code,UINT32);
class TMetaWriter;
class TMetaFile
{
public:
    void * project;
    PStream meta;
    META_RECORD mrecord;
    PROCESSMETAPROC proc;

    TMetaFile(PROCESSMETAPROC p, UINT32 data);
    TMetaFile(PStream ps,PROCESSMETAPROC p,UINT32 data);
    ~TMetaFile();

    void   NewMetaRecord(INT16);
    void   NewUndoRecord(INT16);
    void   CloseMetaRecord();
    INT16  DoRedo();
    INT16  DoUndo(UINT );
    BOOL   CanUndo();
    BOOL   CanRedo();

private:
    void   WriteLastRecord(long);
    INT16  ProcessMetaRecord(INT16);
    void   Init();
    void   _Error(int){};

    friend TMetaWriter;
    BYTE record_flag;
    UINT32 data;
};

class TMetaWriter{
public:
    TMetaFile* meta;
    TMetaWriter(TMetaFile*_meta);
    ~TMetaWriter();
};
extern TMetaFile * meta;
#endif

