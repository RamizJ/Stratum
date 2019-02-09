/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      stream.cpp
      stream.h
Author:
      Alexander Shelemekhov
*/
#include <windows.h>
#define FILE_ITERATOR
#include "../BC/bc_include_stratum/STREAM.H"
#include <errno.h>
#include <io.h>
#include <mem.h>
#include <dir.h>
#include <string.h>


//создает новый массив символов и копирует туда строку astr

char *NewStr(char*astr)
{
    if(astr)
    {
        int i=lstrlen(astr);
        if(i)
        {
            char * s=new char[i+1];
            lstrcpy(s, astr);
            return s;
        }
    }
    return NULL;
} ;

TStream::TStream()
{
    status=0;
    version=0;
}

TStream::~TStream(){ } ;

void TDOSStream::Error(INT16 h)
{
    _Error(h, 0, filename);
    status=1;
} ;

void TStream::Error(INT16 h)
{
    _Error(h);
    status=1;
} ;

void TStream::Flush(){_abstract();};

long TStream::GetPos(){return _abstract();}

long TStream::GetSize(){return _abstract();}

int TStream::Read(void _BIDSFAR *, int){return _abstract();};

#if !defined(WIN32)
long TStream::Read(void _huge *, long){return _abstract();};
long TStream::Write(const void __huge *, long){return _abstract();}
#endif

char *TStream::ReadStr()
{
    char * str;
    INT16 i=ReadWord();
    if(i>0)
    {
        str=new char[i+1];
        Read(str, i);
        str[i]=0;
    }
    else
        str=NULL;
    return str;
}

INT16 TStream::ReadStr(char* str, INT16 bufsize)
{
    int i=ReadWord();
    if(i > 0)
    {
        int toread=min(i, bufsize-1);
        if(i)
            Read(str, toread);
        str[toread]=0;
        return (INT16)i;
    }
    *str=0;
    return 0;
}

void TStream::Reset(){status=0;};

long TStream::Seek(long, seek_modes){return _abstract();}

void TStream::Truncate(){_abstract();}

int TStream::Write(const void _BIDSFAR *, int)
{
    return _abstract();
}

INT16 TStream::WriteStr(char*str)
{
    INT16 l=str ? (INT16)lstrlen(str) : 0;
    WriteWord(l);
    if(l)
        Write(str, l);
    return 0;
} ;

BOOL TStream::WriteWord(INT16 w)
{
    Write(&w, 2);
    return TRUE;
} ;

INT16 TStream::ReadWord()
{
    INT16 w;
    if(status)
        return 0;
    Read(&w, 2);
    return w;
} ;

BOOL TStream::WriteLong(long l)
{
    Write(&l, 4);
    return 1;
} ;

long TStream::ReadLong()
{
    long l;
    if(status)
        return 0;
    Read(&l, 4);
    return l;
} ;

long TStream::CopyTo(TStream*ps, long start, long len)
{
    if(len==0)
        return 0;
    long oldpos = GetPos();
    long _len=0;
    HGLOBAL hglb;
    void FAR *buffer;
    const bufsize=24576;
    Seek(start);
    hglb=GlobalAlloc(GMEM_FIXED, bufsize);
    buffer=GlobalLock(hglb);
    while((len)&&(ps->status==0)&&(status==0))
    {
        long i=min(len, bufsize);
        len-=i;
        int j=LOWORD(i);
        Read(buffer, j);
        ps->Write(buffer, j);
        if(j>0)
            _len+=(long)j;
    }
    GlobalUnlock(hglb);
    GlobalFree(hglb);
    Seek(oldpos);
    return _len;
} ;
/****************************************************************************
                     TDOSStream
****************************************************************************/

#ifdef WIN32
#define TO_OPENREAD       OF_READ|OF_SHARE_DENY_NONE
#define TO_OPENWRITE      OF_WRITE|OF_SHARE_DENY_WRITE
#define TO_OPENREAD_WRITE OF_READWRITE|OF_SHARE_DENY_WRITE
#define TO_CREATE         OF_SHARE_DENY_WRITE
#else
#define TO_OPENREAD       READ|OF_SHARE_DENY_NONE
#define TO_OPENWRITE      WRITE|OF_SHARE_DENY_WRITE
#define TO_OPENREAD_WRITE READ_WRITE|OF_SHARE_DENY_WRITE
#define TO_CREATE         OF_SHARE_DENY_WRITE
#endif

TDOSStream::TDOSStream(char*fname, open_modes _mode)
{
    //mode=_mode&7;
    mode=_mode;
    char szBuf[255];
    if(fname)
        temp=0;
    else
    {

#ifdef WIN32

        {
            char path[256];
            GetTempPath(sizeof(path), path);
            GetTempFileNameA(path, "tstuuuu.$$$", 0, szBuf);
        }
#else

        GetTempFileName(0, "tstuuuu.$$$", 0, szBuf);
#endif

        fname=szBuf;
        temp=1;
    }
    filename=NewStr(fname);
    switch(_mode&7){
        case stOpenRead:
            _HANDLE=_lopen(fname, TO_OPENREAD);
            break;
        case stOpenWrite:
            _HANDLE=_lopen(fname, TO_OPENWRITE);
            break;
        case stOpen:
            _HANDLE=_lopen(fname, TO_OPENREAD_WRITE);
            break;
        case stCreate:
            _HANDLE=_lcreat(fname, TO_CREATE);
            break;
    }
    status=1;
    if(_HANDLE>0)
        status=0;
    else
    {
        if(!(_mode&stNoWarning))_Error(EC_FILEOPENERROR,0,fname);
    }
    //Seek(0);
} ;

TDOSStream::~TDOSStream()
{
    if(_HANDLE)
        _lclose(_HANDLE);
    if(temp&&(!(mode&TDOSStream::stNoEraseTmp)))
    {
        OFSTRUCT ofstruct;
        OpenFile(filename, &ofstruct, OF_DELETE);
    }
    delete filename;
} ;

void TDOSStream::Flush(){ } ;

long TDOSStream::GetPos()
{
    long l=_llseek(_HANDLE, 0, 1);
    if(l<0)
    {
        Error(EC_FILEERROR);
        l=0;
    }
    return l;
} ;

long TDOSStream::GetSize()
{
    //return filelength(_HANDLE);
    long p=GetPos();
    long l=_llseek(_HANDLE, 0, 2);
    Seek(p);
    return l;
}

int TDOSStream::Read(void _BIDSFAR *buf, int len)
{
    if(status==0)
    {
        int l = _lread(_HANDLE, buf, len);
        if(l != len)
        {
            status=-1;
            if(l > -1)
            {
                ((char *)buf)[l]='\0';
                //for (int i=l; i<len; i++)
                //  ((char *)buf)[i]=0;
            }
        } ;
        return l;
    }
    else
    {
        //int i;
        //for (i=0; i<len; i++)
        //  ((char *)buf)[i]=0;
        ((char *)buf)[0]='\0';
    }
    return 0;
} ;

#if !defined  WIN32

long TDOSStream::Read(void __huge *buf, long len)
{
    if(status==0)
    {
        long l=_hread(_HANDLE, buf, len);
        if(l!=len)
        { status=-1;}
    }
    //        else _hmemset(buf,0,len);
    return 0;
} ;
#endif
/*
long    TDOSStream::Seek(long pos,seek_modes mode)
  {    long s=_llseek(_HANDLE,0,2);      //
      long l=_llseek(_HANDLE,pos,mode);
      eof=(l==s)||(l==HFILE_ERROR);    //
      if (mode==0){
      if (l==pos){ return l;}else
      { Error(EC_FILEERROR);

       return -1;
      }}return 1;
  };
*/

long TDOSStream::Seek(long pos, seek_modes mode)
{
    long l = _llseek(_HANDLE, pos, mode);
    if(mode==0)
    {
        if(l == pos)
        {
            status=0;
            return l;
        }
        else
        {
            Error(EC_FILEERROR);
            return -1;
        }
    }
    return 1;
} ;

UINT32

extern EXEC_FLAGS;

void TDOSStream::Truncate()
{

#ifdef WIN32

    if(!SetEndOfFile((HANDLE)_HANDLE))
        status=3;
#else

    int ret=chsize(_HANDLE, GetPos());
    if(ret)
    {
        if(EXEC_FLAGS&0x01/*EF_RUNNING*/)
        {
            char * s=NULL;
            switch(errno){
                case EACCES:
                    s="Permission denied";
                    break;
                case EBADF:
                    s="Bad file number";
                    break;
                case ENOSPC:
                    s="No space left on device";
                    break;
                default:
                    s="Unknow";
            }
            _Message(s);
        }
        status=3;
    }
#endif

} ;

BOOL TDOSStream::Overflow()
{
    _Error(EM_DISKOVERFLOW, 0, filename);
    return TRUE;
} ;

int TDOSStream::Write(const void _BIDSFAR *buf, int len)
{
m1:
    if(status==0)
    {
        int l = _lwrite(_HANDLE, (LPCSTR)buf, len);
        if(l!=len)
        {
            if(mode==stOpenRead)
            {
                _Error(EM_CANTWRITE);
                status=-2;
                return 0;
            } ;
            if(Overflow())
            {
                status=0;
                if(l>0)
                {
                    len-=l;
                    ((char *)buf)+=l;
                }
                goto m1;
            }
            else
                status=-1;
        }
    }
    return 0;
} ;

#if !defined(WIN32)

long TDOSStream::Write(const void __huge *buf, long len)
{
m1:
    if(status==0)
    {
        long l=_hwrite(_HANDLE, buf, len);
        if(l!=len)
        {
            if(mode==stOpenRead)
            {
                _Error(EM_CANTWRITE);
                status=-2;
                return 0;
            } ;
            if(Overflow())
            {
                status=0;
                len-=l;
                ((char __huge *)buf)+=l;
                goto m1;
            }
            else
                status=-1;
        }
    }
    return 0;
} ;
#endif
/****************************************************************************
                  TMemoryStream
****************************************************************************/
//HGLOBAL _HANDLE;
//BYTE flags;
//long size,pos;
//public:

TMemoryStream::TMemoryStream(char*fn, BOOL del)
{
    flags=0x80;
    if(del)
        flags|=1;
    _HANDLE=0;
    pos=0;
    size=lstrlen(fn)+1;
    _HANDLE=GlobalAlloc(GMEM_MOVEABLE, size);
    base=GlobalLock(_HANDLE);
    if(!base)
    {
        _HANDLE=0;
        _Error(EM_hglobal);
    }
    else
    {
#ifdef WIN32
        memcpy(base, fn, size);
#else
        hmemcpy(base, fn, size);
#endif
    } ;
} ;

TMemoryStream::TMemoryStream(BOOL del, char*fn)
{
    flags=0;
    if(del)
        flags|=1;
    _HANDLE=0;
    pos=0;
    status=0;
    //TDOSStream ds(fn, TDOSStream::stOpenRead|TDOSStream::stNoWarning);
    TDOSStream ds(fn, TDOSStream::stOpenRead);
    if(!ds.status)
    {
        size=ds.GetSize();
        alloc=size;
        _HANDLE=GlobalAlloc(GMEM_MOVEABLE, size);
        base=GlobalLock(_HANDLE);
        if(!base)
        {
            _HANDLE=0;
            _Error(EM_hglobal);
        }
        else
        {
            ds.Read(base, size);
            if(ds.status)
                status=1;
        }
    }
} ;

BOOL TMemoryStream::Eof()
{
    if(flags&0x80)
        return (*((char *)base)==0);
    else
        return TStream::Eof();
} ;

TMemoryStream::TMemoryStream(HGLOBAL hg)
{
    flags=2;
    _HANDLE=hg;
    pos=0;
    size=0;
    base=GlobalLock(_HANDLE);
    if(!base)
    {
        _HANDLE=0;
        _Error(EM_hglobal);
    }
    else
    {
        size=GlobalSize(_HANDLE);
        alloc=size;
    }
} ;

TMemoryStream::TMemoryStream(BOOL del)
{
    flags=0;
    if(del)
        flags|=1;
    size=0;
    pos=0;
    _HANDLE=NULL;
    base=0;
    alloc=size;
    _HANDLE=GlobalAlloc(GMEM_MOVEABLE, 65400u);
    base=GlobalLock(_HANDLE);
    if(!base)
    {
        _HANDLE=0;
        _Error(EM_hglobal);
    }
    else
    { alloc=GlobalSize(_HANDLE);}
} ;

TMemoryStream::TMemoryStream(void*b, int _size)
{
    _HANDLE=0;
    flags=0;
    pos=0;
    alloc=0;
    size=_size;
    base=b;
} ;

TMemoryStream::~TMemoryStream()
{
    if(_HANDLE)
    {
        GlobalUnlock(_HANDLE);
        if(flags&1)
            GlobalFree(_HANDLE);
        flags=0;
        base=0;
    }
    else
    { delete base;}
} ;

BOOL TMemoryStream::ReAlloc(long s)
{
    if(!status)
    {

#ifdef WIN32

        GlobalUnlock(_HANDLE);
        HGLOBAL h=GlobalReAlloc(_HANDLE, s, GMEM_MOVEABLE);
        if(h)
        { _HANDLE=h;}
        base=GlobalLock(_HANDLE);
        if(base)
            return TRUE;
#else

        HGLOBAL h=GlobalReAlloc(_HANDLE, s, GMEM_MOVEABLE);
        if(h)
        {
            if(h!=_HANDLE)
            {
                GlobalUnlock(_HANDLE);
                GlobalFree(_HANDLE);
                _HANDLE=h;
                base=GlobalLock(_HANDLE);
            }
            alloc=GlobalSize(_HANDLE);
            if(base)
                return TRUE;
            _Error(EM_hglobal);
        }
#endif

    }
    return FALSE;
} ;

#ifdef WIN32

BOOL

TMemoryStream

::Update(int&s,BOOL w)
#else

BOOL TMemoryStream::Update(long&s,BOOL w)
#endif

{
    if(status)
        return 0;
    long np=pos+s;
    //  if (np>=size)eof=1;
    if(np>size)
    {
        if(w)
        {
            size=np;
            if(size>alloc)
            { ReAlloc((1+size/65536ul)*65536ul);}
        }
        else
        {
            s=size-pos;
            pos=size;
            return s>0;
        }
    } //else eof=0;
    pos=np;
    return !status;
} ;

void TMemoryStream::Error(INT16 e)
{
    TStream::Error(e);
    if(_HANDLE)
    {
        GlobalUnlock(_HANDLE);
        if(flags&2)
            GlobalFree(_HANDLE);
        _HANDLE=0;
        size=0;
        alloc=0;
        pos=0;
    }
} ;

void TMemoryStream::Flush(){ } ;

long TMemoryStream::GetPos(){return pos;};

long TMemoryStream::GetSize(){return size;};

long TMemoryStream::Seek(long p, seek_modes mode)
{
    switch(mode)
    {
        case FROM_CURRENT:
            p = pos + p;
            break;

        case FROM_END:
            p = size + p;
    }
    //	eof=(p<size) && (p>=0);
    if(p >= 0 && p <= size)
        pos = p;
    return pos;
}

void TMemoryStream::Truncate()
{
    if(!status)
    {
        size=pos;
        ReAlloc(size);
    }
} ;

int TMemoryStream::Read(void _BIDSFAR *buffer, int numBytes)
{
    long p=pos;
    int nb=numBytes;
    if(Update(numBytes, 0))
    {
#ifdef WIN32
        memcpy(buffer, (char *)base+p, numBytes);
        if(numBytes<nb)
            memset(((char *)buffer)+numBytes, 0, nb-numBytes);
#else
        hmemcpy(buffer, (char __huge *)base+p, numBytes);
        if(numBytes<nb)
            memset(((char *)buffer)+numBytes, 0, nb-numBytes);
#endif
        return numBytes;
    }
    else
    { memset(buffer, 0, nb);}
    return 0;
} ;

int TMemoryStream::Write(const void _BIDSFAR *buffer, int numBytes)
{
    long p=pos;
    if(Update(numBytes, 1))
    {

#ifdef WIN32

        memcpy((char *)base+p, buffer, numBytes);
#else

        hmemcpy((char __huge *)base+p, buffer, numBytes);
#endif

        return numBytes;
    }
    return 0;
} ;

#if !defined (WIN32)

long TMemoryStream::Read(void __huge *buffer, long numBytes)
{
    long p=pos;
    int nb=numBytes;
    if(Update(numBytes, 0))
    {
        hmemcpy(buffer, (char __huge *)base+p, numBytes);
        if(numBytes<nb)
            memset(((char *)buffer)+numBytes, 0, nb-numBytes);
        return numBytes;
    }
    else
    { memset(buffer, 0, nb);}
    //			 else {_hmemset}
    return 0;
} ;

long TMemoryStream::Write(const void __huge *buffer, long numBytes)
{
    long p=pos;
    if(Update(numBytes, 1))
    {
        hmemcpy((char __huge *)base+p, buffer, numBytes);
        return numBytes;
    }
    return 0;
} ;
#endif

HGLOBAL TMemoryStream::GetHANDLE(){return _HANDLE;};

INT16 _GetTempFileName(char*path, char*s, INT16 extsize, char*ext)
{
    static long _d=69898l;
    BOOL ok=1;
    int pos=lstrlen(s)-(5+extsize);
    do
    {
        _d+=GetTickCount()>>4;
        char ss[10];
        wvsprintf(ss, "%d", &_d);
        int len=lstrlen(ss);
        if(len>4)
            len-=4;
        else
            len=0;
        for (int i=0; i<4; i++)
        { s[pos+i]=ss[len+i];}
        char test[256];
        lstrcpy(test, path);
        lstrcat(test, s);
        HFILE h=_lopen(test, TO_OPENREAD); // trying to open file
        if(h!=HFILE_ERROR)
        { _lclose(h);}
        else
        {
            lstrcpy(test+(lstrlen(test)-extsize), ext);
            h=_lopen(test, TO_OPENREAD);
            if(h!=HFILE_ERROR)
            { _lclose(h);}
            else
                ok=0;
        }
    }
    while(ok);
    return TRUE;
} ;

BOOL GetPath(char*file, char*path)
{
    int l=lstrlen(file);
    if(l)
    {
        while(file[l]!='\\'&&file[l]!='/'&&l>0)
            l--;
        if(l)
        {
            for (int i=0; i<l; i++)
                path[i]=file[i];
            path[l]=0;
            return TRUE;
        }
    }
    lstrcpy(path, "");
    return FALSE;
} ;

BOOL TStream::Eof()
{
    long _s=GetSize();
    long _p=GetPos();
    if(_s)
        return (_p>=_s||_p<0||_s<0);
    else
        return 1;
} ;

void TStream::get(char&_char){Read(&_char, 1);};

char TStream::peek()
{
    char c;
    Read(&c, 1);
    Seek(-1, FROM_CURRENT);
    return c;
} ;

void AddSlash(char*p)
{
    int l=lstrlen(p);
    if(l)
    {
        if(p[l-1]!='\\')
        { lstrcat(p, "\\");}
    }
} ;

BOOL MakeDirectory(char*dir)
{
    if(IsDirExist(dir))  return TRUE;
    if(mkdir(dir)==0)    return TRUE;

    char path[25600];
    GetPath(dir, path);
    if(lstrlen(path))
    {
        if(MakeDirectory(path))
        {
            if (mkdir(dir)==0)return TRUE;
        }
    }
    return FALSE;
} ;

BOOL DeleteDirectory(char*dir)
{
    if(!IsDirExist(dir))    return FALSE;

    WIN32_FIND_DATA fdata,fdata1;
    memset(&fdata, 0, sizeof(fdata));
    memset(&fdata1, 0, sizeof(fdata1));
    fdata.dwFileAttributes=fdata1.dwFileAttributes=FILE_ATTRIBUTE_NORMAL;
    AddSlash(dir);
    char *temp=new char[10240];
    lstrcpy(temp,dir);
    AddSlash(temp);
    HANDLE fhandle=FindFirstFile(strcat(temp,"*.*"), &fdata);
    if(fhandle!=INVALID_HANDLE_VALUE)
    {
        int t=0;
        HANDLE fhandle1;
        do
        {
            if(t==0 || t==1){t++; continue;}
            lstrcpy(temp,dir);
            lstrcat(temp,fdata.cFileName);
            AddSlash(temp);
            fhandle1=FindFirstFile(lstrcat(temp,"*.*"), &fdata1);
            lstrcpy(temp,dir);
            lstrcat(temp,fdata.cFileName);
            if(fhandle1!=INVALID_HANDLE_VALUE)
            {
                FindClose(fhandle1);
                DeleteDirectory(temp);
            }
            else
                ScDeleteFile(temp);
            FindClose(fhandle1);
        }
        while(FindNextFile(fhandle, &fdata));
    }
    FindClose(fhandle);
    delete temp;
    if(rmdir(dir)==0)
        return TRUE;
    return FALSE;
} ;


void TStream::WriteDOSSTR(char*s)
{
    int l=lstrlen(s);
    Write(s, l);
    WriteWord(0xa0d);
} ;

BOOL ScRenameFile(char*oldname, char*newname)
{
    if(!MoveFile(oldname, newname))
    {
        char s[51200];
        wsprintf(s, "ќшибка переименовани¤ %s в %s", oldname, newname);
        _Message(s);
        return FALSE;
    }
    /*
  int rez=rename(oldname, newname);
  if(rez==-1)
  {
    char s[512];
    char * type=0;
    switch(errno){
    case EACCES:
      type=":Permission denied";
      break;
    case ENOENT:
      type=":No such file or directory";
      break;
    case ENOTSAM:
      type=":Not same device";
      break;
    } ;
    wsprintf(s, "ќшибка переименовани¤ %s в %s %s", oldname, newname, type);
    _Message(s);
    return FALSE;
  } */
    return TRUE;
} ;

BOOL ScFileExist(char* name)
{
    TDOSStream st(name, TDOSStream::stOpenRead|TDOSStream::stNoWarning);
    return st.status==0;
}

BOOL ScDeleteFile(char*name)
{
    //OFSTRUCT of;
    //OpenFile(name, &of, OF_DELETE);
    if(ScFileExist(name))
        DeleteFile(name);
    return TRUE;
} ;

int IsAbsolutePath(char*file)
{
    if(!file||((*file)==0))
        return 0;
    while((*file==' ')&&*file)
        file++;
    int l=lstrlen(file);
    if(l>2)
    {
        char * Lps=strstr(file, ":");
        if(Lps)
        {
            if(Lps[1]=='\\')
                return 1;
            return 0;
        }
        if((file[0]=='\\')&&(file[1]=='\\'))
            return 1;
    }
    return 2;
} ;

TFileIterator::TFileIterator(char*_path, char*_mask, BOOL _dir)
{
    memset(this, 0, sizeof(TFileIterator));
    path=_path;
    dir=_dir;
    mask=_mask;
} ;

#ifdef WIN32

TFileIterator::~TFileIterator()
{
    if(fhandle)FindClose(fhandle);
} ;
#endif

BOOL TFileIterator::GetNext()
{
    BOOL _dir;
    do
    {
        if(started)
        {
            if(ok)
            {

#ifdef WIN32

                ok=FindNextFile(fhandle, &fdata);
#else

                ok=(_dos_findnext(&ffblk)==0);
#endif

            }
        }
        else
        {
            // »нициализаци¤
            char sp[280];
            lstrcpy(sp, path);
            if(mask)
            {
                AddSlash(sp);
                lstrcat(sp, mask);
            }

#ifdef WIN32

            fdata.dwFileAttributes=dir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
            fhandle=FindFirstFile(sp, &fdata); // address of returned information
            ok=(fhandle!=INVALID_HANDLE_VALUE);
            if(!ok)
            { fhandle=0;}
#else

            ok=(_dos_findfirst(sp, dir ? _A_SUBDIR : _A_NORMAL, &ffblk)==0);
#endif

            started=TRUE;
        }

#ifdef WIN32

        _dir=(fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
#else

        _dir=(ffblk.attrib&_A_SUBDIR)!=0;
#endif

    }
    while(ok&&((dir!=_dir)||(*GetFileName()=='.')));
    /*
  while (dir && ok && ((!(
 #ifdef WIN32
  fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY
 #else
  ffblk.attrib&_A_SUBDIR
 #endif
  )) || (*GetFileName()=='.')));
 */

    return ok;
};

char *  TFileIterator::GetFileName(){
    if(ok){

#ifdef WIN32
        return fdata.cFileName;
#else

        return ffblk.name;
#endif

    }
};
void    TFileIterator::GetFullFileName(char*sp){
    lstrcpy(sp,path);
    AddSlash(sp);
    lstrcat(sp,GetFileName());
};
BOOL    TFileIterator::GetFileTime(){};
BOOL    TFileIterator::IsReadOnly(){

#ifdef WIN32
    return (fdata.dwFileAttributes&(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY));
#else

    return (ffblk.attrib&(_A_SYSTEM|_A_HIDDEN|_A_RDONLY));
#endif

} ;
long    TFileIterator::GetFileSize(){

#ifdef WIN32
    return fdata.nFileSizeLow;
#else

    return ffblk.size;
#endif

} ;

BOOL   ScCopyFile(char *from,char* to){
    BOOL rez=FALSE;
    TDOSStream in(from,TDOSStream::stOpenRead);
    if(in.status==0){
        TDOSStream out(to,TDOSStream::stCreate);
        if(out.status==0){
            long _len=in.GetSize();
            long len=in.CopyTo(&out,0,_len);
            rez=_len==len;
        }
    }
    return rez;
};

BOOL  IsDirEmpty(char *dir){
    {
        TFileIterator f(dir,"*.*");
        if(f.GetNext())return FALSE;
    }
    {
        TFileIterator f(dir,"*.*",TRUE);
        if(f.GetNext())return FALSE;
    }
    return TRUE;
};

void RemoveSlash(char *path){
    while(1){
        int j=lstrlen(path);
        if(j){
            if(path[j-1]=='\\')path[j-1]=0;else return;
        }else return;
    }
}

BOOL IsDirExist(char *dir){
    char sp[260];
    lstrcpy(sp,dir);
    AddSlash(sp);
    lstrcat(sp,"*.*");
    BOOL ok=FALSE;

#ifdef WIN32

    WIN32_FIND_DATA fdata;
    HANDLE fhandle;
    memset(&fdata,0,sizeof(fdata));
    fdata.dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_NORMAL;
    fhandle=FindFirstFile(sp,&fdata); // address of returned information
    ok=(fhandle!=INVALID_HANDLE_VALUE);
    if(!ok){fhandle=0;}
    if(fhandle)FindClose(fhandle);
#else

    find_t ffblk;
    memset(&ffblk,0,sizeof(ffblk));
    ok = (_dos_findfirst(sp,_A_SUBDIR|_A_NORMAL,&ffblk)==0);
#endif

    return ok;
}

BOOL  GetRelativePath(char*base,char*file,char*out){
    if(base==NULL || base[0]==0){
        lstrcpy(out,file);
        return FALSE;
    }
    char s[260];
    char _buf[260];
    lstrcpy(s,base);
    lstrcpy(_buf,file);

    AddSlash(s);
    AnsiUpper(s);AnsiUpper(_buf);
    int i = 0;
    char * pf=_buf;
    char * pp=s;
    while (*pf && *pp && ((*pp)==(*pf))){i++;pf++;pp++;}
    if(i && s[i-1]=='\\'){
        lstrcpy(out,"");
        char * pf=file+i;
        char * pp=s+i;
        while(pp && *pp){
            lstrcat(out,"..\\");
            char *_pp=strstr(pp,"\\");
            if(_pp)pp=_pp+1;
        };
        lstrcat(out,pf);
        return TRUE;
    }
    return TRUE;
};
BOOL   GetAbsoluteName(char*base,char*relative,char*full){
    char _buf[260];
    lstrcat(full,base);
    AddSlash(full);
    char *_a=relative+1;
    while (*_a=='.' && _a[1]=='.'&& (_a[2]=='\\' || _a[2]=='/')){
        _a+=3;
        char *ps=(full+lstrlen(full))-2;
        while ((ps-full)>0 && (*ps!='\\' && *ps!='/'))ps--;
        if(*ps=='\\' || *ps=='/')ps[1]=0;
    }
    lstrcat(full,_a);
    return TRUE;
};
