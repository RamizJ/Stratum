/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      stream_m.cpp
      stream_m.h
Author:
      Alexander Shelemekhov
Description:
      Manager for user defined streams
*/

#include <stdlib.h>
#include "collect.h"
#include "stream.h"
#include "stream_m.h"
#include "../BC/bc_include_stratum/STREAM.H"
#define MAX_STREAM 64
#include "class.h"
#include "serrors.rh"
#include "_message.h"
#include "project.h"
//ftp://ftp.installshield.com/InstallShield/SDK_Edition/MasterDisks/Intel/Disk1ni.exe

class TSerialStream: public TStream
{
#ifdef WIN32
    HANDLE idComDev;
    OVERLAPPED overlapped;
#else
    int idComDev;
#endif
public:
    TSerialStream(char *, char *);
    ~TSerialStream();
    virtual void Flush();
    virtual long GetPos();
    virtual long GetSize();
    virtual long Seek(long, seek_modes mode=FROM_START);
    virtual void Truncate();
    virtual int Read(void _BIDSFAR *buffer, int numBytes);
    virtual int Write(const void _BIDSFAR *buffer, int);
#if !defined(WIN32)
    virtual long Read(void __huge *buffer, long numBytes);
    virtual long Write(const void __huge *buffer, long);
#endif
    virtual BOOL Eof();
};

TSerialStream::TSerialStream(char*lpszDevControl, char*text)
{
    idComDev=0;
    char * endptr=(char *)text;
    int value=(int)strtol(text, &endptr, 10);
    int cbInQueue, cbOutQueue;
    cbInQueue=cbOutQueue=1024;
    if((endptr==NULL)||((*endptr)==0))
    { cbInQueue=cbOutQueue=value;}
    char devname[200];
    char * ps=lpszDevControl;
    while(*ps&&*ps!=':')
        ps++;
    lstrcpyn(devname, lpszDevControl, (ps-lpszDevControl)+1);

#ifdef WIN32

    idComDev=CreateFile(devname, 0, 0, 0, 0, 0, 0);
#else

    idComDev=OpenComm(devname, cbInQueue, cbOutQueue);
    ps=NULL;
    switch(idComDev){
        case IE_BADID:
            ps="The device identifier is invalid or unsupported.";
            break;
        case IE_BAUDRATE:
            ps="The device's baud rate is unsupported.";
            break;
        case IE_BYTESIZE:
            ps="The specified byte size is invalid.";
            break;
        case IE_DEFAULT:
            ps="The default parameters are in error.";
            break;
        case IE_HARDWARE:
            ps="The hardware is not available (is locked by another device).";
            break;
        case IE_MEMORY:
            ps="The function cannot allocate the queues.";
            break;
        case IE_NOPEN:
            ps="The device is not open.";
            break;
        case IE_OPEN:
            ps="The device is already open.";
            break;
    }
#endif

    if(ps)
    {
        idComDev=0;
        status=1;
        char ss[256];
        lstrcpy(ss, ps);
        AddObjectName(ss);
        return;
    }
    DCB dcb;

    int err=BuildCommDCB(lpszDevControl, &dcb);
    if(err<0)
    {
        _Message("BuildCommDCB");
        status=1;
        return;
    }

#ifdef WIN32

    err=SetCommState(idComDev, &dcb);
#else

    err=SetCommState(&dcb);
#endif

    if(err<0)
    {
        _Message("SetCommState");
        return;
    }
} ;

TSerialStream::~TSerialStream()
{

#ifdef WIN32

    if(idComDev)
        CloseHandle(idComDev);
#else

    if(idComDev)
        CloseComm(idComDev);
#endif

} ;

void TSerialStream::Flush(){ } ;

long TSerialStream::GetPos(){return 0;};

long TSerialStream::GetSize(){return 0;};

long TSerialStream::Seek(long, seek_modes mode){_Message(_M_SEEKCOM);};

void TSerialStream::Truncate(){_Message(_M_TRUNCATECOM);};

int TSerialStream::Read(void _BIDSFAR *buffer, int numBytes)
{

#ifdef WIN32

    DWORD rez;
    ReadFile(idComDev, buffer, numBytes, &rez, &overlapped);
    return rez;
#else

    return ReadComm(idComDev, buffer, numBytes);
#endif

} ;

int TSerialStream::Write(const void _BIDSFAR *buffer, int numBytes)
{
#ifdef WIN32
    DWORD rez;
    WriteFile(idComDev, buffer, numBytes, &rez, &overlapped);
    return rez;
#else
    return WriteComm(idComDev, buffer, numBytes);
#endif
} ;

#if !defined(WIN32)

long TSerialStream::Read(void __huge *buffer, long numBytes){Read((void *)buffer, (int)numBytes);};

long TSerialStream::Write(const void __huge *buffer, long numBytes){Write((void *)buffer, (int)numBytes);};
#endif

BOOL TSerialStream::Eof(){return 0;};

/**************************************************************************/
/**************************************************************************/
int strpos(char*str, char*fr)
{
    int i=0;
    while(*str)
    {
        if(*str==*fr)
        {
            char * p1=str;
            char * p2=fr;
            while(*p1&&*p1==*p2)
            {
                p1++;
                p2++;
                if(!(*p2))
                    return i;
            }
        }
        i++;
        str++;
    }
    return -1;
} ;

TStreamItem::TStreamItem(int handle, STREAM_TYPE _type, TStream*_stream)
{
    HANDLE=handle;
    type=_type;
    stream=_stream;
    Width=8;
    project=GetCurrentProject();
}

TStreamItem::~TStreamItem(){delete stream;};

long TStreamItem::Seek(long pos){return stream->Seek(pos);};

void TStreamItem::Flush(int){stream->Flush();};

long TStreamItem::GetPos(){return stream->GetPos();};

long TStreamItem::Read(void*b, int count){return stream->Read(b, count);};

long TStreamItem::Write(void*b, int count){return stream->Write(b, count);};

char * streamtypes []= { "NULL", "FILE", "MEMORY", "PIPE", "NET", "MIDI", "SNDFX", "SERIAL", "LPT"};

char *TStreamItem::GetTypeStr(){return streamtypes[type-1];};

int TStreamMan::Write(int h, void*buf, int size)
{
    TStreamItem * item=_GetStream(h);
    if(item)
        return (int)item->Write(buf, size);
    return 0;
} ;

int TStreamMan::Read(int h, void*buf, int size)
{
    TStreamItem * item=_GetStream(h);
    if(item)
        return (int)item->Read(buf, size);
    return 0;
} ;

long TStreamMan::Seek(int h, long pos)
{
    TStreamItem * item=_GetStream(h);
    if(item)
        return item->Seek(pos);
    return 0;
} ;

long TStreamMan::GetPos(int h)
{
    TStreamItem * item=_GetStream(h);
    if(item)
        return item->GetPos();

    return 0;
}

long TStreamMan::GetSize(int h)
{
    TStream * stream=GetStream(h);
    if(stream)
        return stream->GetSize();
    return 0;
} ;

int TStreamMan::CreateStream(char* device, char* name, char*_add)
{
    for (int i=0; i<(sizeof(streamtypes)/sizeof(char *)); i++)
    {
        if(lstrcmpi(streamtypes[i], device)==0)
        {
            char add[999];
            lstrcpy(add, _add);
            AnsiUpper(add);
            i += ST_NULL;
            TStream * stream=NULL;
            switch(i)
            {
                case ST_NULL:
                    break;

                case ST_FILE:
                {
                    if(CanCreateDOS())
                    {
                        TDOSStream::open_modes mode = TDOSStream::stOpenRead;
                        if(strpos(add, "READONLY")>=0)
                            mode=TDOSStream::stOpenRead;

                        if(strpos(add, "READWRITE")>=0)
                            mode=TDOSStream::stOpen;

                        if(strpos(add, "WRITEONLY")>=0)
                            mode=TDOSStream::stOpenWrite;

                        if(strpos(add, "CREATE")>=0)
                            mode=TDOSStream::stCreate;

                        if(strpos(add, "NOWARNING")>=0)
                            mode|=TDOSStream::stNoWarning;

                        char dst[260];
                        GetFullFileNameEx((TProject *)GetCurrentProject(), name, dst);
                        stream = new TDOSStream(dst, mode);
                    }
                    else
                    {
                        Error(S_EC_TOMOREDOS);
                    }
                    break;
                }
                case ST_MEMORY:
                    stream=new TMemoryStream();
                    break;

                case ST_PIPE:
                    break;

                case ST_NET:
                    break;

                case ST_MIDI:
                    break;

                case ST_SNDFX:
                    break;

                case ST_SERIAL:
                {
                    stream=new TSerialStream(name, add);
                }
                    break;
                case ST_LPT:
                    break;
            }
            if(stream)
            {
                if(stream->status)
                {
                    delete stream;
                    return 0;
                }
                int handle = 1;
                for (int j = 0; j < count; j++)
                {
                    TStreamItem * item = (TStreamItem *)At(j);
                    if(item->HANDLE != handle)
                        break;
                    handle++;
                } ;
                TStreamItem * item=new TStreamItem(handle, (STREAM_TYPE)i, stream);
                Insert(item);
                return handle;
            }
        }
    }
    Error(S_EC_BADTYPE);
    return 0;
} ;

int TStreamMan::GetType(int h, char *)
{
    TStreamItem * item=_GetStream(h);
    if(item)
        return item->type;
    return 0;
} ;

int TStreamMan::Eof(int h)
{
    TStreamItem * item=_GetStream(h);
    if(item)
        return item->stream->Eof();
    return 0;
} ;

int TStreamMan::Status(int h)
{
    TStream * stream=GetStream(h);
    if(stream)
    { return stream->status;}
    return 0;
} ;

int TStreamMan::SetWidth(int h, int w)
{
    TStreamItem * stream=_GetStream(h);
    if(stream)
    {
        stream->Width=w;
        return 1;
    }
    return 0;
} ;

int TStreamMan::FWrite(int h, double d)
{
    TStream * stream = GetStream(h);
    TStreamItem * _stream=_GetStream(h);
    if(stream)
    {
        switch(_stream->Width)
        {
            case 0:
            {
                unsigned char n=(unsigned char)d;
                stream->Write(&n, 1);
                return 1;
            }
            case 1:
            {
                char n=(char)d;
                stream->Write(&n, 1);
                return 1;
            }
            case 2:
            {
                UINT16 n=(unsigned int)d;
                stream->Write(&n, 2);
                return 1;
            }
            case 3:
            {
                INT16 n=(int)d;
                stream->Write(&n, 2);
                return 1;
            }
            case 4:
            {
                UINT32 n=(unsigned long)d;
                stream->Write(&n, 4);
                return 1;
            }
            case 5:
            {
                INT32 n=(long)d;
                stream->Write(&n, 4);
                return 1;
            }
            case 6:
            {
                float n=(float)d;
                stream->Write(&n, 4);
                return 1;
            }
            case 7:
            {
                // not released !
                return 0;
            }
            case 8:
            {
                stream->Write(&d, 8);
                return 1;
            }
            case 9:
            {
                long double n=d;
                stream->Write(&n, 10);
                return 1;
            }
        }
    }
    return 0;
}

int TStreamMan::SWrite(int h, char*s)
{
    TStream * stream = GetStream(h);
    if(stream)
    {
        stream->Write(s, lstrlen(s));
        stream->WriteWord(0x0a0d);
        return 1;
    }
    return 0;
}

int TStreamMan::FRead(int h, double*d)
{
    TStream * stream=GetStream(h);
    TStreamItem * _stream=_GetStream(h);

    if(stream)
    {
        switch(_stream->Width){
            case 0:
            {
                unsigned char n;
                stream->Read(&n, 1);
                * d=n;
                return 1;
            }
            case 1:
            {
                char n;
                stream->Read(&n, 1);
                * d=n;
                return 1;
            }
            case 2:
            {
                UINT16 n;
                stream->Read(&n, 2);
                * d=n;
                return 1;
            }
            case 3:
            {
                INT16 n;
                stream->Read(&n, 2);
                * d=n;
                return 1;
            }
            case 4:
            {
                UINT32 n;
                stream->Read(&n, 4);
                * d=n;
                return 1;
            }
            case 5:
            {
                INT32 n;
                stream->Read(&n, 4);
                * d=n;
                return 1;
            }
            case 6:
            {
                float n;
                stream->Read(&n, 4);
                * d=n;
                return 1;
            }
            case 7:
            {
                // not released !
                return 0;
            }
            case 8:
            {
                stream->Read(d, 8);
                return 1;
            }
            case 9:
            {
                long double n;
                stream->Read(&n, 10);
                * d=(double)n;
                return 1;
            }
        }
    }
    return 0;
}

/*
  int TStreamMan::SRead(int h,char *s)
  {
   int position;
   TStreamItem * item=_GetStream(h);
   if (item)
   {
    long pos=item->GetPos();
    int rb=(int)item->Read(s,255);
    int i=0;
    char * ps=s;
   while(i<rb-1)
    {
 //	if (*ps=='\r' && *(ps+1)=='\n')
   if (*ps=='\n')
   {
    *(ps-1)=0;
    position = pos+(ps-s)+1;
    item->Seek(position);
    return 1;
   }ps++;
    }
   }
   return 0;
  };
 */

int TStreamMan::GetLine(int h, char *s, int sz, char dl)
{
    sz = min(sz, 255);
    long position;
    TStreamItem * item = _GetStream(h);
    if (item)
    {
        long pos = item->GetPos();
        int rb = (int)item->Read(s, sz);
        if (!rb)
            return 0;

        int i=0;
        char * ps = s;
        while(i++ < rb-1)
        {
            if (*ps++==dl)
            {
                *ps=0;
                position = pos+(ps-s);
                item->Seek(position);
                return 1;
            }
        }

        *(s + rb)=0;
        position = pos + rb;
        item->Seek(position);
        return 1;
    }
    return 0;
}

int TStreamMan::GetAllText(int h,char *s)
{
    int position;
    TStreamItem * item=_GetStream(h);
    if (item)
    {
        long pos = item->GetPos();
        item->Seek(0);
        int rb=(int)item->Read(s,MAX_CLASSTEXTSIZE);
        if(!rb) return 0;
        *(s + rb)=0;
        item->Seek(pos);
        return 1;
    }
    return 0;
}

int TStreamMan::CloseStream(int h)
{
    TStreamItem *item=_GetStream(h);
    if (item) {Free(item); return h;}
    return 0;
}

TStreamMan::TStreamMan():TSortedCollection(10,10){};
TStreamMan::~TStreamMan(){};

BOOL TStreamMan::CanCreateDOS(){
    int c=0;
    for(C_TYPE i=0;i<count;i++){
        TStreamItem* item=(TStreamItem*)At(i);
        if (item && (item->type==ST_FILE))c++;
    }
    return MAX_DOS_STREAM>c;
}

void TStreamMan::CloseAll(void * p)
{
    C_TYPE i=0;
    while(i<count)
    {
        TStreamItem* item=(TStreamItem*)At(i);
        if (item && ((item->project == p) || (p == NULL)) )
        {
            Free(item);
        }
        else
            i++;
    }
}

BOOL TStreamMan::Error(int c)
{
    return  GroupFuncError(c,S_SPRINTF);
}

int TStreamMan::Compare(pointer key1,pointer key2)
{
    int h1=((TStreamItem*)key1)->HANDLE;
    int h2=((TStreamItem*)key2)->HANDLE;
    if (h1==h2)return 0;
    if (h1<h2) return -1;
    return 1;
}

void TStreamMan::FreeItem(pointer item)
{
    //char s[20];
    //wsprintf(s,"%d",((TStreamItem*)item)->HANDLE);
    //MessageBox(0,s,"awef",0);
    delete ((TStreamItem*)item);
}

TStream* TStreamMan::GetStream(int handle)
{
    _item.HANDLE=handle;
    C_TYPE index;

    if (Search(&_item,index))
        return ((TStreamItem*)At(index))->stream;

    Error(S_EC_BADHANDLE);
    return NULL;
}

TStreamItem* TStreamMan::_GetStream(int handle)
{
    _item.HANDLE=handle;
    C_TYPE index;
    if (Search(&_item,index))return (TStreamItem*)At(index);
    Error(S_EC_BADHANDLE);
    return NULL;
}

TStreamItem::TStreamItem()
{
    HANDLE=0;
    stream=NULL;
}
