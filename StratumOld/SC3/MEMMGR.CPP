/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            memmgr.cpp
Description:
            Solid-linear memory manager
Author:
            Alexander Shelemekhov
*/
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <windows.h>
#include <float.h>
#include <signal.h>
#include <mem.h>
//#include <math.h>
#include "stream.h"
#include "memmgr.h"
#include "errors.h"
#include "class.h"
#include "project.h"
#include "vmachine.h"
#include <stratum\stratum.h>
#include "dialogs.h"
#include "strings.rh"

HINSTANCE extern HResource;

#ifdef __cplusplus
typedef __cdecl void (*fptr)(int);
#else
typedef __cdecl void (*fptr)();
#endif

class TMemBlockHeader{
public:
    HGLOBAL HANDLE;
    long used;
    long start;
    TMemBlockHeader*next;
};

TMemoryManager::TMemoryManager(long _flags,long _blocksize){
    first=current=0;
    blocksize=_blocksize;
    size=0;lost=0;pos=NULL;
    flags=_flags;
};
TMemoryManager::~TMemoryManager(){
    DoneBlocks();
};
void  TMemoryManager::DoneBlocks(){
    while(first){
        HGLOBAL HANDLE=first->HANDLE;
        first=first->next;
        GlobalUnlock(HANDLE);
        GlobalFree(HANDLE);
    }
    first=current=0;
    size=0;lost=0;pos=NULL;
};

void* TMemoryManager::GetMem(WORD _msize)
{
    //if (size>(blocksize-30))return NULL; //??? DEBUG
    if (_msize > lost)
        AllocNewBlock();

    lost -= _msize;
    size += _msize;
    void* ret = pos;
    pos = pos + _msize;
    return ret;
}

void* TMemoryManager::GetMemZ(WORD _msize)
{
    void* p = GetMem(_msize);
    memset(p, 0, _msize);
    return p;
}

long TMemoryManager::GetPos(){
    return size;
};
BOOL TMemoryManager::Release(long){
    current=first;
    DoneBlocks();
    return 1;
};
BOOL TMemoryManager::FreeMem(void*){
    return 0;
};
char *TMemoryManager::NewStr(char*s){
    if(s==NULL)return NULL;
    int l=lstrlen(s);
    if (l){
        char *_s=(char *)GetMem(l+1);
        lstrcpy(_s,s);
        return _s;
    }
    return NULL;
};
void TMemoryManager::ClearAll(){
    TMemBlockHeader* b=first;
    while(b){
        memset((char*)b+sizeof(TMemBlockHeader),0,blocksize-sizeof(TMemBlockHeader));
        b=b->next;
    }
};

void TMemoryManager::AllocNewBlock()
{
    if (!(current && current->next))
    {
        UINT f=GMEM_MOVEABLE;
        if(flags==ZEROINIT)f|=GMEM_ZEROINIT;
        HGLOBAL HANDLE=GlobalAlloc(f,blocksize);
        TMemBlockHeader*block=(TMemBlockHeader*)GlobalLock(HANDLE);
        if (!block){
            _Error(EM_hglobal);
        }
        block->HANDLE=HANDLE;
        block->next=0;
        block->used=0;

        if (!first)
            first=block;
        if (current)
        {
            current->next = block;
            current->used = size-current->start;
        }
        current = block;
    }
    else
    {
        current = current->next;
    }
    //else{}
    current->start = size;
    pos = ((char*)current) + sizeof(TMemBlockHeader);
    lost = blocksize-sizeof(TMemBlockHeader);
}

#ifdef CHECKHEAP
#define memsinature 798979l
struct TMemInfo{
    TMemInfo*Next;
    TMemInfo*Prev;
    long     num;
    int      size;
    long     signature;
};
TMemInfo*first=NULL;
TMemInfo*last=NULL;
unsigned long refcount = 0;
unsigned long newcount= 0;
unsigned long memcount=0;

#ifdef WIN32
void * cdecl operator  new( size_t size){
    return new char[size];
}
void * cdecl operator  new[]( size_t size)
#else
void * cdecl operator  new( size_t size)
#endif
{
    refcount++;
    newcount++;
    if (size>32768){
        MessageBox(0,"Попытка захватить блок памяти  больше 32768 кБайт!\n Хотя ничего страшного но скорее всего ошибка.",
                   "MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);
    }
    memcount+=(long)size;
    TMemInfo*m=(TMemInfo*)malloc(size+sizeof(TMemInfo)+1);
    memset(m,0xff,size+sizeof(TMemInfo));
    if (last)last->Next=m;
    m->Prev=last;
    m->Next=NULL;
    m->num=newcount;
    m->signature=memsinature;
    m->size=size;
    last=m;
    if (!first)first=m;
    void *r=(((char*)m)+sizeof(TMemInfo));
    char * cb=(char*)r;
    cb[size]=char(newcount);
    /*
                  if (newcount==3952){
  MessageBox(0,"**********************************************\nOk!",
        "MEMORY CHECK NOTIFY!",MB_OK|MB_TASKMODAL);

                    int i=6;
                    i++;
                  }
*/
    return r;
}
void cdecl operator  delete( void* addr ) {
    TMemInfo*m;
    refcount--;
    //   memcount-=size;
    if (addr){
        if ((DWORD)(addr)==0xfffffffful){
            MessageBox(0,"Attempt to delete FFFF:FFFF pointer",
                       "MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);
        }else{
            m=(TMemInfo*)((long)(addr)-sizeof(TMemInfo));
            if (m->signature==memsinature){
                memcount-=m->size;
                if (m->Prev)m->Prev->Next=m->Next;
                if (m->Next)m->Next->Prev=m->Prev;
                if (((char*)addr)[m->size]!=char(m->num)){
                    MessageBox(0,"Memory was writen in place that not allocated!\n(Used more then allocated)!",
                               "MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);
                }
                if (last==m){
                    last=m->Prev;
                }
                if (first==m)first=m->Next;
                memset(m,0,m->size+sizeof(TMemInfo));
                free(m);
                return ;
            }// signature
            else{
                MessageBox(0,"Попытка удалить невыделенный блок памяти!\n(Вызов delete без предшествующего new)\n Или фатально повреждена память!",
                           "MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);
            }}
    }else{
        MessageBox(0,"Попытка удалить нулевой указатель (NULL pointer)\nХотя это и не приведет к ошибке, но все равно не порядок!",
                   "MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);
    }
    int i=5;
    i++;
}
void CheckFinalMemoryState(){
    if (refcount){
        if (!first){
            MessageBox(0,"Memory refernce count dont ZERO but first memory block iz NULL !",
                       "MEMORY CHECK ERROR!",MB_OK|MB_ICONSTOP|MB_TASKMODAL);
        }
    };
    if (first){
        char s[256];
        wsprintf(s,
                 "Некоторые блоки памяти(общим числом %ld ) не освобожденны.\nОбщий объем потерянной памяти составил %ld байт\n\nХотите ли вы записать информацию о памяти в файл\n( c:\\sc_mem.log & c:\\sc_mem.bin )\nдля последующего анализа "
                 ,refcount,memcount);

        if(MessageBox(0,s,
                      "MEMORY CHECK ERROR!",MB_YESNO|MB_ICONSTOP|MB_TASKMODAL)==IDYES){


            HFILE log=_lcreat("c:\\sc_mem.log",OF_SHARE_DENY_WRITE);
            HFILE bin=_lcreat("c:\\sc_mem.bin",OF_SHARE_DENY_WRITE);

            wsprintf(s,"Total new called : %ld \r\n",newcount);
            _lwrite(log,s,lstrlen(s));
            wsprintf(s,"******* E R R O R ********\r\nRefcount: %ld\r\n\r\n",refcount);
            _lwrite(log,s,lstrlen(s));
            TMemInfo*m=first;
            long offset=0;
            while (m){
                wsprintf(s,"Num: %ld size: %d In bin: %ld\r\n",m->num,m->size,offset);
                _lwrite(log,s,lstrlen(s));
                _lwrite(bin,((char*)m)+sizeof(TMemInfo),m->size);
                offset+=m->size;
                m=m->Next;
            }
            _lclose(log);
            _lclose(bin);

        }
    }
};
#endif
long totalcatch=0;

void _SelfMathCatcher(int c2){
    char s[256];
    int mmode=SCGetScVar("MathMode",0);
    if(mmode!=3){
        int cause=c2;

        char _descr[64];
        char *descr=_descr;

        switch(cause){
            case FPE_INTOVFLOW   : descr="80x86 Interrupt on overflow";break;
            case FPE_INTDIV0     : descr="80x86 Integer divide by zero";break;
            case FPE_INVALID     : descr="80x87 invalid floating point operation";break;
            case FPE_ZERODIVIDE  : descr="80x87 divide by zero";break;
            case FPE_OVERFLOW    : descr="80x87 arithmetic overflow";break;
            case FPE_UNDERFLOW   : descr="80x87 arithmetic underflow";break;
            case FPE_INEXACT     : descr="80x87 precision loss";break;
            case FPE_STACKFAULT  : descr="80x87 stack overflow";break;
            case FPE_EXPLICITGEN : descr="When SIGFPE is raise()'d";break;
            default:
                LoadString(HResource,IDS_str158,_descr,sizeof(_descr));
        };

        if (EXEC_FLAGS&EF_STEPACTIVE){
            LoadString(HResource,IDS_str159,s,sizeof(s));

            if (EXEC_FLAGS&EF_MSGACTIVE){
                LoadString(HResource,IDS_str160,s+lstrlen(s),80);
            }

            if (EXEC_FLAGS&EF_EQUACTIVE){
                LoadString(HResource,IDS_str161,s+lstrlen(s),80);
            }else {
                LoadString(HResource,IDS_str162,s+lstrlen(s),80);
                AddObjectName(s);
            }
        }else {

            char cmd[128];
            LoadString(HResource,IDS_str163,cmd,sizeof(cmd));

            wsprintf(s,cmd,cause);

        }
        LoadString(HResource,IDS_str164,s+lstrlen(s),80);
        lstrcat(s,descr);
        int rez=0;
        if(mmode<2){
            int rez=MathErrorDialog(s);

            if(rez==2){
                TProject*prj=GetProject(NULL);
                if(prj)prj->SetVarI("MathMode",3);
                else SCSetScVarI("MathMode",3);
            }
            if(mmode==0 && rez!=1){
                Vm_Halt();
                SetLastScError(EM_MATHERROR);

            }
        }

        if(mmode<3){
            _Message(s);
        }
    }else{
        wsprintf(s,"%d",0);
    }
};

void _USERENTRY MathCatcher(int c1,int c2)
{// int cause=*((&sig)+1);
    totalcatch++;
    signal(SIGFPE,(fptr)MathCatcher);  //  ******reinstall signal handler
    _SelfMathCatcher(c2);
}

void InitMathErrorHandler(){
    signal(SIGFPE,(fptr) MathCatcher);  /* cast Catcher to appropriate type */
};

#ifdef LOGON

TDOSStream* logfile=NULL;
int logdelta=0;

BYTE logenable=1;

void  LogStep(int delta){
    logdelta+=delta;
    if(logdelta<0)logdelta=0;
    if(logdelta>40)logdelta=40;
};

void LogMessage(char*msg)
{
    if(!logenable)return;
    if(logfile==0)
    {
        char file[9999];
        GetPath(_argv[0],file);
        AddSlash(file);
        lstrcat(file,"protocol.log");
        logfile=new TDOSStream(file,TDOSStream::stOpen|TDOSStream::stNoWarning);
        if(logfile->status==0)
        {
            logfile->Seek(logfile->GetSize());
        }
        else
        {
            delete logfile;
            logfile=new TDOSStream(file,TDOSStream::stCreate);
        }
        struct date d;
        getdate(&d);
        logfile->WriteDOSSTR("Создается файл протокола");
        logfile->WriteDOSSTR(file);
#ifdef WIN32
        logfile->WriteDOSSTR("32 bit version of SC 2000 started ");
#else
        logfile->WriteDOSSTR("16 bit version of SC 2000 started");
#endif
        char s[256];
        sprintf(s,"Дата %02d:%02d:%d",d.da_day,d.da_mon,d.da_year);
        logfile->WriteDOSSTR(s);
        struct  time t;
        gettime(&t);
        sprintf(s,"Время : %2d:%02d:%02d.%02d", t.ti_hour, t.ti_min, t.ti_sec, t.ti_hund);
        logfile->WriteDOSSTR(s);
    }
    if(logdelta)
    {
        char s[80];
        memset(s,' ',logdelta);
        logfile->Write(s,logdelta);
    }
    logfile->WriteDOSSTR(msg);
};

void  DoneLog(){
    if(logfile){
        logfile->WriteDOSSTR("*********************************");

        delete logfile;logfile=NULL;}
};
#endif

