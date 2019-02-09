InitRead(/*
                           Copyright (c) 1995  Virtual World
                           Module Name:
                                _object.cpp
                           Author:
                                Alexander Shelemekhov
                           */

         //#define NOKERNEL         // KERNEL APIs and definitions
         //#define NOGDI            // GDI APIs and definitions
         //#define NOUSER           // USER APIs and definitions
         #define NOSOUND          // Sound APIs and definitions
         #define NOCOMM           // Comm driver APIs and definitions
         #define NODRIVERS        // Installable driver APIs and definitions
         #define OEMRESOURCE      // OEM Resource values
         #define NONLS            // All NLS defines and routines
         #define NOSERVICE        // All Service Controller routines, SERVICE_ equates, etc.
         #define NOKANJI          // Kanji support stuff.
         #define NOMINMAX         // min() and max() macros
         #define NOLOGERROR       // LogError() and related definitions
         #define NOPROFILER       // Profiler APIs
         //#define NOMEMMGR         // Local and global memory management
         //#define NOLFILEIO        //_l* file I/O routines
         //#define NOOPENFILE       // OpenFile and related definitions
         //#define NORESOURCE       // Resource management
         #define NOATOM           // Atom management
         #define NOLANGUAGE       // Character test routines
         //#define NOLSTRING        // lstr* string management routines
         #define NODBCS           // Double-byte character set routines
         #define NOKEYBOARDINFO   // Keyboard driver routines
         #define NOGDICAPMASKS    // GDI device capability constants
         #define NOCOLOR          // COLOR_* color values
         //#define NOGDIOBJ         // GDI pens, brushes, fonts
         #define NODRAWTEXT       // DrawText() and related definitions
         #define NOTEXTMETRIC     // TEXTMETRIC and related APIs
         #define NOSCALABLEFONT   // Truetype scalable font support
         //#define NOBITMAP         // Bitmap support
         #define NORASTEROPS      // GDI Raster operation definitions
         #define NOMETAFILE       // Metafile support
         #define NOSYSMETRICS     // GetSystemMetrics() and related SM_* definitions
         #define NOSYSTEMPARAMSINFO //SystemParametersInfo() and SPI_* definitions
         #define NOMSG            // APIs and definitions that use MSG structure
         #define NOWINSTYLES      // Window style definitions
         #define NOWINOFFSETS     // Get/SetWindowWord/Long offset definitions
         #define NOSHOWWINDOW     // ShowWindow and related definitions
         #define NODEFERWINDOWPOS  // DeferWindowPos and related definitions
         #define NOVIRTUALKEYCODES // VK_* virtual key codes
         #define NOKEYSTATES       // MK_* message key state flags
         #define NOWH              // SetWindowsHook and related WH_* definitions
         #define NOMENUS           // Menu APIs
         #define NOSCROLL          // Scrolling APIs and scroll bar control
         #define NOCLIPBOARD       // Clipboard APIs and definitions
         #define NOICONS           // IDI_* icon IDs
         #define NOMB              // MessageBox and related definitions
         #define NOSYSCOMMANDS     // WM_SYSCOMMAND SC_* definitions
         #define NOMDI             // MDI support
         #define NOCTLMGR          // Control management and controls
         #define NOWINMESSAGES     // WM_* window messages
         #define NOHELP            // Help support
         #include <windows.h>
         #include "_object.h"

         #include <io.h>

         char * NewStr(char * astr)
{
             int i=lstrlen(astr);
             if (i)
             {
                 char * s=new char[i+1];
                 lstrcpy(s,astr);
                 return s;
             }
             return NULL;
         };

TStream::TStream(){
    status=0;fileversion=currentfileversion;
    //_cur_wait=LoadCursor((HINSTANCE)GetInstance(),MAKEINTRESOURCE(CURSOR_WAIT1));
    //_old_cur=SetCursor(_cur_wait);
}
TStream::~TStream()
{//SetCursor(_old_cur);
    //DestroyCursor(_cur_wait);
};

void    TStream::Error(int h){_Error(h);status=1;};

void    TStream::Flush(){_abstract();};

pointer TStream::Get(INT16 id)
{
    if (status)
        return NULL;

    _OBJECT* obj=NULL;
    CHUNK chunk;

    chunk.InitRead(this);

    if (chunk.code)
    {
        if (chunk.code==-1)
            return NULL;

        if ((id!=0) && (id!=chunk.code))
        {
            _Error(EM_invalidobjecttype);
        }
        else
        {
            obj = AllocateObject(chunk);
            if(!obj)
            {
                _Error(EM_invalidobjecttype);
            }
        }
    }
    else
    {
        _Error(EM_nullobjid);return NULL;
    }
    chunk.Check();
    return obj;
};

long    TStream::GetPos(){return _abstract();}
long    TStream::GetSize(){return _abstract();}

void    TStream::Put(pointer obj)
{
    CHUNK chunk;
    chunk.InitWrite(this,obj?((_OBJECT*)obj)->WhoIsIt():-1);
    if(obj)
    {
        ((_OBJECT*)obj)->Store(this);
        ((_OBJECT*)obj)->PostStore(this);
    }
    chunk.DoneWrite();

    /*
  INT16 i=-1;if (obj){
    WriteWord(((_OBJECT*)obj)->WhoIsIt());
    ((_OBJECT*)obj)->Store(this);}else WriteWord(i);
  */
};


int     TStream::Read( void _BIDSFAR *, int ){return _abstract();};

#if !defined(WIN32)
long    TStream::Read(void _huge *, long){return _abstract();};
long    TStream::Write(const void __huge *, long){return _abstract();}
#endif // !WIN32

char*     TStream::ReadStr()
{
    char* str;
    int i=ReadWord();
    if (i)
    {
        str= new char[i+1];
        Read(str,i);
        str[i]=0;
    }
    else
        str=NULL;

    return str;
};


void    TStream::Reset(){status=0;};
long    TStream::Seek(long){return _abstract();}
void    TStream::Truncate(){_abstract();}
int     TStream::Write(const void _BIDSFAR *,int){return _abstract();}

int     TStream::WriteStr(char* str)
{if (str){
        INT16 l=(INT16)lstrlen(str);
        WriteWord(l);
        Write(str,l);}else WriteWord(0);
    return 0;
};
INT16     TStream::WriteWord(INT16 w)
{
    Write(&w,2);
    return w;
};
UINT32    TStream::WriteLong(UINT32 u){
    Write(&u,4);
    return u;
};
UINT32    TStream::ReadLong(){
    UINT32  u;
    if (status) return 0;
    Read(&u,4);return u;

};

INT16     TStream::ReadWord()
{
    INT16 w;
    if (status) return 0;
    Read(&w,2);return w;
}

long  TStream::CopyTo(TStream * ps,long start,long len)
{long oldpos=GetPos();
    HGLOBAL hglb;
    void FAR* buffer;
    const bufsize=24576;
    Seek(start);
    hglb = GlobalAlloc(GMEM_FIXED,bufsize);
    buffer = GlobalLock(hglb);
    while ((len)&&(ps->status==0)&&(status==0))
    {long i=min(len,bufsize);len-=i;
        int j=LOWORD(i);
        Read(buffer,j);
        ps->Write(buffer,j);
    }
    GlobalUnlock(hglb);
    GlobalFree(hglb);
    Seek(oldpos);
    return len;
};
/****************************************************************************
                                     TDOSStream
****************************************************************************/
#ifdef WIN32
#define TO_OPENREAD       OF_READ|OF_SHARE_DENY_WRITE
#define TO_OPENWRITE      OF_WRITE|OF_SHARE_DENY_WRITE
#define TO_OPENREAD_WRITE OF_READWRITE|OF_SHARE_DENY_WRITE
#define TO_CREATE         OF_SHARE_DENY_WRITE
#else
#define TO_OPENREAD       READ|OF_SHARE_DENY_WRITE
#define TO_OPENWRITE      WRITE|OF_SHARE_DENY_WRITE
#define TO_OPENREAD_WRITE READ_WRITE|OF_SHARE_DENY_WRITE
#define TO_CREATE         OF_SHARE_DENY_WRITE
#endif

TDOSStream::TDOSStream(char* fname,int mode){
    char szBuf[144];
    if (fname)temp=0;else
    {
        GetTempFileName(0, "tstuuuu.$$$", 0, szBuf);
        fname=szBuf;
        temp=1;
    }
    filename=NewStr(fname);

    switch (mode){
        case 0:_HANDLE = _lopen(fname,TO_OPENREAD);break;
        case 1:_HANDLE = _lopen(fname,TO_OPENWRITE);break;
        case 2:_HANDLE = _lopen(fname,TO_OPENREAD_WRITE);break;
        case 3:_HANDLE = _lcreat(fname,TO_CREATE);break;
    }
    status=1;
#ifdef WIN32
    if (_HANDLE>0)status=0;
#else
    if (_HANDLE!=HFILE_ERROR)status=0;
#endif

};
TDOSStream::~TDOSStream(){
    if (_HANDLE) _lclose(_HANDLE);
    if (temp)
    {
        OFSTRUCT ofstruct;
        OpenFile(filename,&ofstruct,OF_DELETE);
    }
    delete filename;
};
void      TDOSStream::Flush(){};
long      TDOSStream::GetPos()
{ long l=_llseek(_HANDLE,0,1);
    if (l<0){Error(EM_fileerror);l=0;}
    return l;
};
long      TDOSStream::GetSize(){
    //return filelength(HANDLE);
    long p=GetPos();
    long l=_llseek(_HANDLE,0,2);
    Seek(p);
    return l;
}

int TDOSStream::Read( void _BIDSFAR *buf, int len)
{
    if (status==0)
    {
        int l = _lread(_HANDLE, buf, len);
        if (l!=len) status=-1;
    }
    else
    {
        int i;
        for(i=0;i<len;i++) ((char*) buf)[i]=0;
    }
    return 0;
}

#if !defined(WIN32)
long TDOSStream::Read( void __huge *buf, long len )
{ if (status==0) {
        long l=_hread(_HANDLE,buf,len);
        if (l!=len) status=-1;
    }
    //	  else _hmemset(buf,0,len);
    return 0;
};
#endif
long TDOSStream::Seek(long pos)
{
    long l=_llseek(_HANDLE,pos,0);
    if (l==pos)
        return l;
    else
    {
        //Error(EM_fileerror);
        return -1;
    }
};

void      TDOSStream::Truncate()
{
    if (chsize(_HANDLE,GetPos())) status=3;
    /*
//extern void FAR PASCAL DOS3Call(void);
    WORD h=HANDLE;
     asm {
//	 extrn DOS3Call : far
     mov bx,h
     mov cx,0
     mov ah,0x40
     call DOS3Call
     }
  */
};

int TDOSStream::Write( const void _BIDSFAR *buf, int len)
{
    if (status==0)
    {
        int l = _lwrite(_HANDLE, (char*)buf, len);
        if (l != len)
            Error(EM_writeerror);
    }
    return 0;
}

#if !defined(WIN32)
long TDOSStream::Write( const void __huge *buf, long len)
{
    if (status==0) {long l=_hwrite(_HANDLE,buf,len);
        if (l!=len) status=-1;
    }
    return 0;
};
#endif
/****************************************************************************
                                     TMemoryStream
****************************************************************************/
//HGLOBAL _HANDLE;
//BYTE flags;
//long size,pos;
//public:
TMemoryStream::TMemoryStream(char* fn,BOOL del)
{flags=0;if (del)flags|=1;_HANDLE=0;
    pos=0;
    TDOSStream ds(fn,TDOSStream::stOpenRead);
    if (!ds.status)
    {size=ds.GetSize();alloc=size;
        _HANDLE=GlobalAlloc(GMEM_MOVEABLE,size);
        base=GlobalLock(_HANDLE);
        if(!base){_HANDLE=0;_Error(EM_hglobal);}else
        {ds.Read(base,size);
            if (ds.status)status=1;
        }
    }
};
TMemoryStream::TMemoryStream(HGLOBAL hg)
{flags=2;
    _HANDLE=hg;pos=0;size=0;
    base=GlobalLock(_HANDLE);
    if(!base){_HANDLE=0;_Error(EM_hglobal);}else
    {size=GlobalSize(_HANDLE);alloc=size;}
};
TMemoryStream::TMemoryStream(BOOL del)
{flags=0;
    if (del)flags|=1;
    size=0;pos=0;
    _HANDLE=NULL;base=0;alloc=size;
    _HANDLE=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,65400u);
    base=GlobalLock(_HANDLE);
    if(!base){_HANDLE=0;_Error(EM_hglobal);}else
    {alloc=GlobalSize(_HANDLE);}
};
TMemoryStream::~TMemoryStream()
{if (_HANDLE){
        GlobalUnlock(_HANDLE);
        if (flags & 1)GlobalFree(_HANDLE);
    }
};
BOOL TMemoryStream::ReAlloc(long s)
{if (!status){
#ifdef WIN32
        GlobalUnlock(_HANDLE);
        HGLOBAL h=GlobalReAlloc(_HANDLE,s,GMEM_MOVEABLE);
        if(h){
            _HANDLE =h;
        }
        base=GlobalLock(_HANDLE);
        if (base) return TRUE;
#else
        HGLOBAL h=GlobalReAlloc(_HANDLE,s,GMEM_MOVEABLE);
        if (h){
            if (h!=_HANDLE)
            {GlobalUnlock(_HANDLE);GlobalFree(_HANDLE);
                _HANDLE=h;base=GlobalLock(_HANDLE);
            }
            alloc=GlobalSize(_HANDLE);
            if (base) return TRUE;_Error(EM_hglobal);
        }
#endif
    }
    return FALSE;
};
BOOL TMemoryStream::Update(long s,BOOL w)
{if (status)return 0;
    long np=pos+s;
    if (np>size)
    {if (w){size=np;
            if(size>alloc){ReAlloc((1+size/65536ul)*65536ul);
            }
        }else return FALSE;
    }
    pos=np;return !status;
};

void  TMemoryStream::Error(int e)
{
    TStream::Error(e);
    if (_HANDLE)
    {
        GlobalUnlock(_HANDLE);
        if (flags & 2)
            GlobalFree(_HANDLE);
        _HANDLE=0;
        size=0;
        alloc=0;
        pos=0;
    }
};

void  TMemoryStream::Flush(){};
long  TMemoryStream::GetPos(){return pos;};
long  TMemoryStream::GetSize() {return size; };
long  TMemoryStream::Seek(long p)
{if (p>=0 && p<=size)pos=p;return pos;
};
void  TMemoryStream::Truncate()
{if (!status)
    {size=pos;
        ReAlloc(size);
    }
};

int   TMemoryStream::Read( void _BIDSFAR *buffer, int numBytes )
{long p=pos;
    if (Update(numBytes,0))
    {
#ifdef WIN32
        memcpy(buffer,(char*)base+p,numBytes);return numBytes;
#else
        hmemcpy(buffer,(char __huge*)base+p,numBytes);return numBytes;
#endif
    }
    return 0;
};
#if !defined(WIN32)
long  TMemoryStream::Read( void __huge *buffer, long numBytes )
{long p=pos;
    if (Update(numBytes,0))
    { hmemcpy(buffer,(char __huge*)base+p,numBytes);return numBytes;}
    return 0;
};
#endif

int   TMemoryStream::Write( const void _BIDSFAR *buffer, int numBytes)
{
    long p=pos;
    if(numBytes==249928)
    {
        p=pos;
    }
    if (Update(numBytes,1))
    {
#ifdef WIN32
        memcpy((char*)base+p,buffer,numBytes);return numBytes;
#else
        hmemcpy((char __huge*)base+p,buffer,numBytes);return numBytes;
#endif
    }
    return 0;
};
#if !defined(WIN32)
long  TMemoryStream::Write( const void __huge *buffer, long numBytes)
{long p=pos;
    if (Update(numBytes,1))
    {hmemcpy((char __huge*)base+p,buffer,numBytes);return numBytes;}
    return 0;
};
#endif
HGLOBAL TMemoryStream::GetHANDLE() {return _HANDLE; };
_OBJECT::_OBJECT(){
#ifdef _check_for__object
    _ref__object++;
#endif
};

_OBJECT::~_OBJECT()
{
#ifdef _check_for__object
    _ref__object--;
#endif
}

#ifdef _check_for__object
int  _OBJECT::_ref__object = 0;
#endif

void   TStream::WritePOINT(LPPOINT2D p){
    Write(p,sizeof(POINT2D));
};
void  TStream::ReadPOINT(LPPOINT2D p)
{
    if (HIBYTE(fileversion)<2)
    {
        p->x=ReadWord();
        p->y=ReadWord();
    }else Read(p,sizeof(POINT2D));
};
void  TStream::WritePOINTs(LPPOINT2D p,INT16 count){
    Write(p,sizeof(POINT2D)*count);
};

void  TStream::ReadPOINTs(LPPOINT2D p,INT16 count)
{
    if (HIBYTE(fileversion)<2)
    {
        for(int i=0;i<count;i++)
        {
            p[i].x=ReadWord();
            p[i].y=ReadWord();
        }
    }
    else
        Read(p, sizeof(POINT2D) * count);
};

void CHUNK::InitRead(TStream* _st)
{
    st=_st;
    if(_st->fileversion>=0x300)
    {
        _pos=st->GetPos();
        st->Read(&code, 6);
        _pos += size;
    }
    else
    {
        code = st->ReadWord();
        _pos=0;
    }
}
void CHUNK::InitWrite(TStream* _st,INT16 _code)
{
    st=_st;
    _pos=st->GetPos();
    code=_code;
    size=0;
    st->Write(&code,6);
};
void CHUNK::DoneWrite()
{
    LONG p=st->GetPos();
    st->Seek(_pos+2);
    size=p-_pos;
    st->WriteLong(size);
    st->Seek(p);
};

BOOL CHUNK::Check()
{
    if(st->fileversion>=0x300)
    {
        LONG p=st->GetPos();
        if(p==_pos)
            return TRUE;

        st->Seek(_pos);
        return FALSE;
    }
    else
        return TRUE;
};

void CHUNK::WriteStr(TStream* st,INT16 _code,char*s){
    if(s)
    {
        int l=lstrlen(s);
        if(l)
        {
            code=_code;size=l+6;
            st->Write(&code,6);
            st->Write(s,l);
        };
    }
}

char* CHUNK::ReadStr()
{
    if(size>6)
    {
        char* s=new char[size-5];
        s[size-6]=0;
        st->Read(s,(int)(size-6));
        return s;
    }
    return NULL;
}

