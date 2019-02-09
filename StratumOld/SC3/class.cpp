/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            class.cpp
            class.h
Author:
            Alexander Shelemekhov
*/
// Для защиты
#ifdef PROTECT
#undef _WINREG_
#define _SC_WINREG_
#endif


#define FILE_ITERATOR
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#ifndef WIN32

#include <dir.h>
#endif
#include <mem.h>
#include <string.h>

#include "_class.h"
#include "STREAM.H"
#include "sc3_graphics/_object.h"

#include "grtools.h"
#include "twindow.h"
#include "tscheme.h"
#include "equ.h"
#include "stream_m.h"
#include "matrix_h.h"
#include "compiler.h"
#include "winman.h"
#include "dynodata.h"
#include "_message.h"
#include "dialogs.h"
#include "strings.rh"
#ifdef PROTECT
#include "password.h"
#include "registry.h"

#endif
#define WRITE_LABEL(_LB)\
    position[writecount].code=_LB;\
    position[writecount++].pos=st.GetPos()-pos;\
    st.WriteWord(_LB);

//#define version  0x3001
//#define version  0x3002 // были введены POINT2D
#define version  0x3003 // правильно считываеются флаги у связей
//#include "_class.h"

#include "sc3.rh"
#include "autosntz.h"
#include "inet.h"

struct TCallBack
{
    TCallBack *  next;
    TCallBack **  _first;
    void*proc;
};


LPSTR varpoint_dbm = "VARPOINT.DBM";
LPSTR default_dbm  = "default.dbm";

// Функция закрывает все окна проекта

//*************************************************************************//
//                        Global Variables                                 //
//*************************************************************************//

BYTE sysflags=0; // Флаги безопасности

//????
PClass changedclass;
// Указывает на класс который менятся при записи переменных

// Для рисования иконок
BITMAPINFOHEADER _info_16={sizeof(_info_16),32,32,1,4,0,32*32/2,300,300,16,16};
BYTE palette[]=
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x80,0x00,
    0x80,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x80,0x00,0x00,0xC0,0xC0,0xC0,0x00,
    0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
    0xFF,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00
};
// Палитра для 16 цветных иконок
//----------------------- Class List ---------------------------------------//


char scheme_path[128]=""; // Путь к иконкам
TClassCollection * classes=NULL;
TLibrary*librarys=NULL;
TLibrary*DefaultLib=NULL;
BYTE static savestatemode=1;


//TClassListInfo** __classinfo;// Для отладки

//		 char*projectfile=NULL; //PRJ
//		 char*projectpath=NULL; //PRJ
//		 PObject root=NULL;     //PRJ

//static PClass rootclass=NULL;            //PRJ
//		 TMemoryManager*VarsMemory=NULL;   //PRJ
//		 TMemoryManager*VarsPointers=NULL; //PRJ
TClassListInfo*_cliFLOAT;
TClassListInfo*_cliINTEGER;
TClassListInfo*_cliSTRING;
TClassListInfo*_cliWORD;
TClassListInfo*_cliBYTE;
TClassListInfo*_cliHANDLE;
TClassListInfo*_cliRGB;

TProject* project   = NULL;
TProject* projects   = NULL;
TStreamMan * streams = NULL;
DWORD  EXEC_FLAGS=0;

int TLibCollection::Compare(pointer key1,pointer key2)
{
    TLibrary * lib1 = (TLibrary*)key1;
    TLibrary * lib2 = (TLibrary*)key2;

    if( (lib1->flags & CLF_LIBRARY) || (lib2->flags & CLF_LIBRARY))
    {
        if((lib1->flags & CLF_LIBRARY) && (lib2->flags & CLF_LIBRARY))
        {
            return lstrcmpi(lib1->path, lib2->path);
        }
        else
        {
            if(lib1->flags & CLF_LIBRARY)
                return -1;
            else
                return 1;
        }
    }
    else
    {
        return lstrcmpi(((PClassListInfo)key1)->GetClassName(),
                        ((PClassListInfo)key2)->GetClassName());
    }
    return 0;
}

C_TYPE TLibCollection::IndexOf(pointer item){
    return TCollection::IndexOf(item);
};
TClassListInfo::TClassListInfo()
{
    classname=filename=NULL;
    flags=0l;
    posinpack=0;
};

char *TClassListInfo::GetClassName()
{
    return (flags & CLF_LOADED) ? _class->name : classname;
};

PClass TClassListInfo::GetClass()
{
    if (flags & CLF_LOADED)
        return _class;

    char s[80];
    lstrcpy(s, classname);
    return GetClassByName(s);
}

PProject TClassListInfo::GetProject(){
    if(flags&CLF_PROJECT){
        TLibrary *lib=library;
        while(lib){
            if(lib->parent==librarys){
                TProject * prj=projects;
                while(prj){
                    for(C_TYPE i=0;i<prj->libs->count;i++){
                        if(prj->libs->At(i)==lib)return prj;
                    }
                    prj=prj->next;
                }
            }
            lib=lib->parent;
        }
    }
    return NULL;
};
PProject TClass::GetProject(){
    return listinfo->GetProject();
};

void TClassCollection::FreeItem(pointer p){
    PClassListInfo p1=(PClassListInfo)p;
    if (p1->filename)delete(p1->filename);
    if (p1->flags&1)delete p1->_class;else delete p1->classname;
    delete p1;
};
int TClassCollection::Compare(pointer key1,pointer key2)
{
    return lstrcmpi(((PClassListInfo)key1)->GetClassName(),((PClassListInfo)key2)->GetClassName());
};
//http://netserver.hp.com/file:///D:/netserver/support/year2000/year2000_manual_chart.htm

INT16 TLibrary::GetClassCount()
{
    INT16 r=0;
    for(C_TYPE i=0;i<items->count;i++)
    {
        TLibrary*lib=(TLibrary*)items->At(i);
        if (lib->flags&CLF_LIBRARY)
            r+=lib->GetClassCount();
        else
            r++;
    }
    return r;
};

TLibrary::TLibrary(char*_path, TLibrary *_parent)
{
    Ref=1;
    path = NewStr(_path);
    items = new TLibCollection(10,40);
    flags=CLF_LIBRARY;

    if (_parent)
        _parent->Insert(this);
    else
        parent=NULL;
}

INT32 TLibrary::AddRef()
{
    for(C_TYPE i=0;i<items->count;i++)
    {
        TLibrary*lib=(TLibrary*)items->At(i);
        if(lib->flags&CLF_LIBRARY)
            lib->AddRef();
    }
    Ref++;
    return Ref;
};

INT32 TLibrary::DecRef()
{
    C_TYPE i=0;
    while(i<items->count)
    {
        TLibrary*lib=(TLibrary*)items->At(i);
        if(lib->flags&CLF_LIBRARY)
        {
            if(!lib->DecRef())
                continue;
        }
        i++;
    }
    Ref--;
    BOOL rez=(Ref>0); // Обязательно так
    if(Ref<1)
    {
        C_TYPE i=0;
        while(i<items->count)
        {
            PClassListInfo pcli=(PClassListInfo)items->At(i);
            if(!(pcli->flags&CLF_LIBRARY))
            {
                pcli->flags|=CLF_CURRENTPROJECT;
                items->AtDelete(i);
            }
            else
                i++;
        }
        i=0;
        while(i<classes->count)
        {
            PClassListInfo pcli=(PClassListInfo)classes->At(i);
            if (pcli->flags&CLF_CURRENTPROJECT)
            {
                classes->AtFree(i);
            }
            else
                i++;
        }
        PostDeleteChild();
    }
    return rez;
};

void TLibrary::PostDeleteChild()
{
    if(Ref<1 && items->count==0)
    {
        UpdateClassWindows(UW_DELETE,UW_LIBRARY,this,parent);
        parent->items->Delete(this);
        parent->PostDeleteChild();
        delete this;
    }
};

BOOL TLibrary::IsContain(PClassListInfo _pcli)
{
    for(C_TYPE i=0;i<items->count;i++)
    {
        PClassListInfo pcli=(PClassListInfo)items->At(i);
        if(pcli->flags&CLF_LIBRARY)
        {
            if (((PLibrary)pcli)->IsContain(_pcli))
                return TRUE;
        }
        else
        {
            if(pcli==_pcli)return TRUE;
        }
    }
    return FALSE;
};

TLibrary::TLibrary(TLibrary*_parent,TStream*st,char*_name,BYTE f)
{
    Ref=1;
    path=NULL;
    items=new TLibCollection(10,40);
    flags=CLF_LIBRARY|f|CLF_PACKFILE;

    if (_parent)
        _parent->Insert(this);
    else
        parent=NULL;

    long pos=st->GetPos();
    long rs=st->ReadLong();
    st->ReadLong();
    st->ReadLong();
    path=NewStr(_name);

    char s[256];
    st->ReadStr(s,sizeof(s));
    if(path==NULL)path=NewStr(s);

    while( st->GetPos() < (pos+rs-2) )
    {
        UINT16 c=st->ReadWord();
        switch(c)
        {
            case 1:
            {
                PClassListInfo pcli = new TClassListInfo(this,st,NULL);
                if(classes->Insert(pcli)<0)
                {
                    if(pcli->filename)
                        delete pcli->filename;

                    if(pcli->classname)
                        delete pcli->classname;

                    delete pcli;
                }
                else
                {
                    items->Insert(pcli);
                    pcli->flags |= CLF_PACKFILE;
                    if(flags & CLF_PROJECT)
                        pcli->flags |= CLF_PROJECT;
                }
            } break;
            case 2:
                new TLibrary(this,st,NULL,f);
        }
    }
};

TLibrary::~TLibrary()
{
    if (path)
    {
        delete path;
        path=NULL;
    }
    for(C_TYPE i=0;i<items->count;i++)
    {
        TLibrary*lib=(TLibrary*)items->At(i);
        if (lib->flags&CLF_LIBRARY)
            delete lib;
    }
    delete items;
};

void TLibrary::Insert(TLibrary*child)
{
    child->parent = this;
    // if(child->flags&CLF_LIBRARY)items->AtInsert(0,child); else
    items->Insert(child);
};

//рекурсивный
void TLibrary::GetPath(char*_path)
{
    if (parent)
    {
        parent->GetPath(_path);
    }
    else
        _path[0]=0;

    if (flags&CLF_PACKFILE && parent && parent->flags&CLF_PACKFILE)
        return;
    if (path)
    {
        lstrcat(_path,path);
        if (!(flags&CLF_PACKFILE))
            lstrcat(_path,"\\");
    }
    //if (flags&CLF_PACKFILE)return;
};

///----------------------------------------------------
BOOL TClassListInfo::isFLOAT()	{return this==_cliFLOAT;}
BOOL TClassListInfo::isINTEGER(){return this==_cliINTEGER;}
BOOL TClassListInfo::isSTRING()	{return this==_cliSTRING;}
BOOL TClassListInfo::isWORD()		{return this==_cliWORD;}
BOOL TClassListInfo::isBYTE()		{return this==_cliBYTE;}
BOOL TClassListInfo::isHANDLE()	{return this==_cliHANDLE;}
BOOL TClassListInfo::isRGB()		{return this==_cliRGB;}

TLibrary* GetParentLibrary()
{
    return librarys;
};

HOBJ2D GetIconDib(HSP2D hsp,char *_file){
    HOBJ2D hddib=0;
    int notfound=1;
    do{
        hddib=GetNextTool2d(hsp,DOUBLEDIB2D,hddib);
        if (hddib && (GetToolType2d(hsp,DOUBLEDIB2D,hddib)==ttREFTODOUBLEDIB2D)){
            char FileName[129];
            GetRDoubleDib2d(hsp,hddib,FileName,128);
            if (!lstrcmpi(FileName,_file))notfound=0;
        }}
    while (hddib && notfound);
    if (!hddib) hddib=CreateRDoubleDib2d(hsp,_file);
    return hddib;
}

void SetDefaultSrc(HSP2D hsp,HOBJ2D obj,int id,char *file){
    char FileName[129];
    HOBJ2D hd=GetDoubleDibObject2d(hsp,obj);
    if(GetRDoubleDib2d(hsp,hd,FileName,128)){
        if(lstrcmpi(file,FileName)){
            hd=GetIconDib(hsp,file);
            SetDoubleDibObject2d(hsp,obj,hd);
        }
    }
    HGLOBAL xor=GetXorDib2d(hsp,hd);
    if(xor){
        LPBITMAPINFOHEADER info = (LPBITMAPINFOHEADER)GlobalLock(xor);
        if(info){
            int sizex=(int)info->biWidth;
            //  int sizey=info->biHeight;
            int pl=(sizex/32);
            if(pl){
                POINT2D size32,org;
                org.y=id/pl;
                org.x=id-org.y*pl;
                size32.x=size32.y=32;
                org.x*=32;org.y*=32;
                SetBitmapSrcRect(hsp,obj,&org,&size32);
                SetObjectSize2d(hsp,obj,&size32);
            }
        }
        GlobalUnlock(xor);
    }
};

BOOL TObject::IsContainObject(PObject po){
    if(po==this)return TRUE;
    PObject _po=childObjects;
    while(_po){
        if(_po->IsContainObject(po))return TRUE;
        _po=_po->next;
    }
    return FALSE;
};

PObject TObject::GetMainObject(PObject po1,PObject po2){
    if(IsContainObject(po1)&&
       IsContainObject(po2))
    {
        PObject _po=childObjects;
        while(_po){
            PObject _p=_po->GetMainObject(po1,po2);
            if(_p){return _p;}
            _po=_po->next;
        }
        return this;
    }
    return NULL;
};

void TObject::ClearVarPointers()
{
    PObject childObject = childObjects;
    while (childObject)
    {
        childObject->ClearVarPointers();
        childObject = childObject->next;
    }

    if (!_class)
        _Error(EM_NULLCLASSPOINTER);
    else
    {
        if (_class->varcount)
        {
            vars = (TVARARRY)project->VarsPointers->GetMemZ((INT16)(_class->varcount*4));
        }
        else
            vars=NULL;
    }
};

BOOL TClass::AddChild(PClass _child,HOBJ2D h)
{
    if (h)
    {
        PChildInfo ci=new TChildInfo[childcount+1];
        memset(ci,0,sizeof(TChildInfo)*(childcount+1));
        for(INT16 i=0;i<childcount;i++)
            ci[i]=childs[i];
        ci[i].type=_child->GetClassListInfo();
        ci[i].handle=h;
        ci[i].name=NULL;
        if (scheme->Loaded())
        {
            HSP2D hsp=scheme->GetSpace();
            POINT2D org;
            GetObjectOrgSize2d(hsp,ci[i].handle,&org,NULL);
            ci[i].x=org.x;
            ci[i].y=org.y;
            ci[i].flags=0;
        }
        else
        {
            ci[i].x=ci[i].y=0;
            ci[i].flags=CHF_NEEDCREATE;
        }
        if (childs)delete childs;
        childcount++;
        childs=ci;
        PObject po=first;
        {
            TMemoryManager temp;
            while(po)
            {
                PObject _po=CreateObjectByClass(_child);
                if(_po)
                {
                    _po->parent=po;
                    if (po->childObjects)
                    {
                        PObject _p=po->childObjects;
                        while(_p->next)_p=_p->next;
                        _p->next=_po;
                    }
                    else po->childObjects=_po;
                    UpdateClassWindows(UW_CREATE,UW_OBJECT,_po,po);
                    _po->ClearVarPointers();
                    _po->UpdateLinks(temp,*(project->VarsMemory));
                    _po->SetDefault();
                    _po->_CopyState(TObject::OLD_TO_NEW);
                }
                po=po->class_next;
            }
        }
        SetModify();
        return TRUE;
    }
    return FALSE;
};

PClassListInfo GetClassListInfo(char *name)
{
    TClassListInfo ci;
    ci.classname = name;
    C_TYPE i;
    if(classes->Search(&ci,i))
    {
        return (PClassListInfo)classes->At(i);
    };
    return NULL;
};

///----------------------------------------------------
TClassListInfo::TClassListInfo(TLibrary*library, TStream*st, char *fileNameWithoutExt)
{
    filename=NULL;
    classname=NULL;
    packsize=posinpack=flags=0;
    library=library;
    filename=NULL;
    if(fileNameWithoutExt)
    {
        TClass::fileversion = st->ReadLong();
        st->Seek(14);
        filename = NewStr(fileNameWithoutExt);
        classname = st->ReadStr();
    }
    else
    {
        st->ReadLong();
        posinpack=st->ReadLong();
        packsize=st->ReadLong();
        classname=st->ReadStr();
    }
};

TLibrary*MakeNewLib(TLibrary*lib,char*name){
    if(lib){
        char s[256];
        lib->GetPath(s);
        AddSlash(s);
        lstrcat(s,name);
#ifdef WIN32
        if(CreateDirectory(s,NULL))
#else
        if (!mkdir(s))
#endif
        {
            TLibrary *_lib=new TLibrary(name,lib);
            UpdateClassWindows(UW_CREATE,UW_LIBRARY,_lib,lib);
            return _lib;
        }
    }
    return NULL;
};
BOOL DeleteLib(TLibrary*lib){
    if(lib->items->count==0){
        char s[256];
        lib->GetPath(s);
        s[lstrlen(s)-1]=0;
#ifdef WIN32
        if (RemoveDirectory(s))
#else
        if (!rmdir(s))
#endif
        { UpdateClassWindows(UW_DELETE,UW_LIBRARY,lib,lib->parent);
            lib->parent->items->Delete(lib);
            delete lib;
            return TRUE;
        }
    }
    return FALSE;
};
TCollection *_adddir_c=NULL;

void BeginChkAddDir()
{
    _adddir_c=new TCollection(30,30);
}

void EndChkAddDir()
{
    if(_adddir_c->count)
    {
        AddDirErrorDialog(mainwindow->HWindow);
        for(C_TYPE i=0;i<_adddir_c->count;i++)
        {
            delete (char*)_adddir_c->At(i);
        }
    }
    _adddir_c->DeleteAll();
    delete _adddir_c;
    _adddir_c=0;
}

int a_Action(PLibrary pl,void*data)
{
    ((TCollection*)data)->Insert(pl);
}

//Создание новой библиотеки
TLibrary* AddDirPath(char*s,BYTE _flags)
{
    TCollection items(10,10);
    EnumerateLibs(NULL,ELF_NOHIERARCHCY,a_Action,(void*)&items);

    char buf[260];
    char _buf[260];
    lstrcpy(_buf,s);
    AddSlash(_buf);

    for(C_TYPE i=0;i<items.count;i++)
    {
        TLibrary*lib=(TLibrary*)items.At(i);
        lib->GetPath(buf);
        if(!lstrcmpi(buf,_buf))
        {
            lib->AddRef();
            return lib;
        }
    }

    RemoveSlash(_buf);
    TLibrary *lib=new TLibrary(_buf,librarys);
    lib->flags|=_flags;
    AddDir(lib,_flags);
    UpdateClassWindows(UW_CREATE,UW_LIBRARY,lib,lib->parent);
    return lib;
};

void AddDirEx(TLibrary*lib, BOOL chk, BYTE b)
{
    BeginChkAddDir();
    AddDir(lib, b);
    EndChkAddDir();
}

//
void AddDir_Class(TLibrary* library, char* fileName, BYTE flag)
{
    char s[256];
    library->GetPath(s);
    lstrcat(s,"\\");lstrcat(s,fileName);

    TDOSStream st(s,TDOSStream::stOpenRead);
    UINT16 code = st.ReadWord();
    if (st.status)
        return;

    if(code == CLASS_SIGNATURE)
    {
        char fileNameWithoutExt[256];
        lstrcpyn(fileNameWithoutExt, fileName, lstrlen(fileName)-3);

        //Устанавливаем имя файла, читаем имя класса
        PClassListInfo pcli=new TClassListInfo(library, &st, fileNameWithoutExt);
        if ((pcli->filename==NULL)||(pcli->classname==NULL))
            delete pcli;

        else
        {
            if (flag & CLF_PROJECT)
                pcli->flags |= CLF_PROJECT;

            if (flag & CLF_READONLY)
                pcli->flags |= CLF_READONLY;

            //Добавление в коллекцию ошибочных загрузок
            if (classes->Insert(pcli) < 0)
            {
                _adddir_c->Insert(NewStr(pcli->classname));
                PClassListInfo _pcli=GetClassListInfo(pcli->classname);
                _pcli->GetFileName(s,FALSE);
                _adddir_c->Insert(NewStr(s));
                pcli->GetFileName(s,FALSE);
                _adddir_c->Insert(NewStr(s));
                delete pcli->filename;
                delete pcli->classname;
                delete pcli;
            }
            else
            {
                library->Insert((TLibrary*)pcli);
            }
        }
    }
    if(code==PACK_SIGNATURE)
    {
        if(st.ReadWord()==2)
        {
            TLibrary *_lib=new TLibrary(library,&st,fileName,flag);
        }
    }
}

void AddDir(TLibrary* library, BYTE flag)
{
    char sp[280];
    library->GetPath(sp);
    {
        TFileIterator fcls(sp,"*.cls");
        while (fcls.GetNext())
        {
            BYTE _f = flag;
            if(fcls.IsReadOnly())
                _f |= CLF_READONLY;

            AddDir_Class(library, fcls.GetFileName(), _f);
        }
    }
    // Поиск классов завершен

    if(!(flag & CLF_NORECURSE))
    {
        TFileIterator flib(sp,"*.*",TRUE);
        while (flib.GetNext())
        {
            TLibrary *_lib = new TLibrary(flib.GetFileName(),library);
            //	 _lib->project=library->project;
            if (flag & CLF_PROJECT)_lib->flags|=CLF_PROJECT;
            AddDir(_lib,flag);
        }
    }

    // Поиск библиотек завершен
    {
        TFileIterator fdll(sp, "*.tdl");
        while (fdll.GetNext())
        {
            char s[280];
            fdll.GetFullFileName(s);
            if(dllmanager)
                dllmanager->LoadDLL(s,(DWORD)project);
        }
    }
};
C_TYPE InitAddLib()
{
    int i=1;
    BOOL w=TRUE;
    char var[64];
    char s[256];
    do
    {
        // удаляем
        wsprintf(var,"addlib%d",i);
        char s[256],path[256];
        if(SC_GetScVarS(var,s))
        {
            if(w)
            {
                WriteTitleStr("Loading additional classes...");
                w=0;
            }
            GetFullFileNameEx(NULL,s,path);
            RemoveSlash(path);
            PLibrary Lib = new TLibrary(path,librarys);
            Lib->flags|=CLF_ADDLIB;
            AddDirEx(Lib, CLF_ADDLIB);
        }else break;
        i++;
    }while(1);
    return 1;
};

C_TYPE InitClassList(char* path)
{
    classes=new TClassCollection(100,50);
    // __classinfo=(TClassListInfo**)classes->items;
    librarys = new TLibrary("",NULL);
    DefaultLib = new TLibrary(path, librarys);
    dllmanager=new TDLLManager();

    //AddDir(DefaultLib);
    AddDirEx(DefaultLib);

    _cliFLOAT = GetClassListInfo("FLOAT");
    _cliINTEGER = GetClassListInfo("INTEGER");
    _cliSTRING = GetClassListInfo("STRING");
    _cliWORD = GetClassListInfo("WORD");
    _cliBYTE = GetClassListInfo("BYTE");
    _cliHANDLE = GetClassListInfo("HANDLE");
    _cliRGB = GetClassListInfo("COLORREF");
    {
        GetClassListInfo("EMPTY_CLASS")->flags |= CLF_READONLY;
    }
    int ps=GetPrivateProfileInt("access","password",0,ini_file);
    if (ps==42)
        sysflags |= 1;
    /*
  {PClass _class=GetClassByName("HANDLE");
    _class->varsize=4;
    _class->SetModify();
    _class=GetClassByName("COLORREF");
    _class->varsize=4;
    _class->SetModify();
  }
*/
    return classes->count;
};

void DoneClassList()
{
#ifdef LOGON
    LogMessage("Зашел в DoneClassList()");
    TLOGoffset __offset(2);
#endif
    //Stop();
    //while(projects)
    {
        //MessageBox(0,((PProject)projects)->path,((PProject)projects)->file,0);
        //CloseProject(projects);
    }
    //MessageBox(0,"DoneClassList","awef",0);
    SaveAllModifyClasses();
    delete librarys;
    DeleteCollection(classes);
    if(dllmanager){delete dllmanager;dllmanager=0;}
    return;
};

PClassListInfo TClass::GetClassListInfo()
{
    return listinfo;
}

BOOL TClass::GetClassFile(char*s,BOOL forsave)
{
    PClassListInfo _clInfo=GetClassListInfo();
    if (_clInfo)
    {
        return _clInfo->GetFileName(s,forsave);
    }
    return FALSE;
};

PClass GetClassByName(char* name, INT16 flag)
{
    PClassListInfo classInfo = GetClassListInfo(name);
    if (classInfo)
    {
        if (classInfo->flags & CLF_LOADED)
            return classInfo->_class;

        if (flag > -1)
        {
            char s[256];
            classInfo->GetFileName(s, FALSE);

            TDOSStream st(s, TDOSStream::stOpenRead);
            if(classInfo->flags & CLF_PACKFILE)
            {
                st.Seek(classInfo->posinpack);
            }

            if (!st.status && (st.ReadWord() == CLASS_SIGNATURE))
            {
                TClass::fileversion=st.ReadLong();
                st.SetVersion((INT16)TClass::fileversion);

                PClass pc = new TClass(st, flag);

                delete classInfo->classname;
                classInfo->flags|=CLF_LOADED;
                classInfo->_class=pc;
                pc->listinfo=classInfo;
                return pc;
            }
        }
    }
    else
    {
        //_Error(EM_CLASSNOTFOUND,0,name);
        _Message(EM_CLASSNOTFOUND,0,name);
    }
    return NULL;
}

PClass  CreateClassByClass(char* name1,char* name2,TLibrary*lib)
{
    PClassListInfo _clInfo=GetClassListInfo(name1);
    if (_clInfo)
    {
        /*		PClassListInfo _clInfo2=GetClassListInfo(name2);
        if(_clInfo2)
    {
          _Message(EM_CLASSNOTFOUND,0,name2);
        return 0;
    }
*/    // добавил проверку на CF_MODIFY
        if (!lib)
            //lib=DefaultLib;
            lib=_clInfo->library;
        TLibrary*_lib=lib;
        while(_lib->flags&(CLF_PACKFILE|CLF_READONLY))
            _lib=_lib->parent;
        if(_lib==NULL)
        {
            MSGBOX(mainwindow->HWindow,MAKEINTRESOURCE(IDS_str95),_MSG_INFO,MB_OK);
            return NULL;
        }
        lib=_lib;
        if (_clInfo->flags&CLF_LOADED)
        {
            PClass _base;
            _base=_clInfo->_class;
            if (_base->flags&CF_MODIFY)
                _base->UpdateClassFile();
        }
        char file[255];
        _clInfo->GetFileName(file,FALSE);
        TDOSStream st(file,TDOSStream::stOpenRead);
        if(_clInfo->flags&CLF_PACKFILE)
            st.Seek(_clInfo->posinpack);
        if (!st.status && (st.ReadWord()==CLASS_SIGNATURE))
        {
            TClass::fileversion=st.ReadLong();
            if (TClass::fileversion)
            {
                PClass _class=new TClass(st,1000);
                PClassListInfo ci= new TClassListInfo();
                delete(_class->name);
                _class->name=NewStr(name2);
                lib->Insert((TLibrary*)ci);
                ci->_class=_class;
                _class->listinfo=ci;
                ci->flags=CLF_LOADED;
                if (lib->flags & CLF_PROJECT)
                    ci->flags|=CLF_PROJECT;
                classes->Insert(ci);
                _class->SetClassIdByDate();
                _class->UpdateClassFile();
                UpdateClassWindows(UW_CREATE,UW_CLASS,ci,lib);
                _class->t_flags|=TCF_NEWCLASS;
                _class->Minimize();
                return _class;
            }
        }
    }
    else
    {
        //_Error(EM_CLASSNOTFOUND,0,name1);
        _Message(EM_CLASSNOTFOUND,0,name1);
    }
    return NULL;
};

PClass CreateNewClass(char* name,char *text,char * scheme,TLibrary*lib)
{
#ifdef LOGON
    LogMessage("Зашел в CreateNewClass");
#endif

    if (!GetClassListInfo(name))
    {
        if (!lib)lib=DefaultLib;
        if(!lib)return NULL;
        PClass _class=new TClass(name,text);
        PClassListInfo ci= new TClassListInfo();
        lib->Insert((TLibrary*)ci);
        ci->_class=_class;
        _class->listinfo=ci;
        ci->flags=CLF_LOADED;
        if (lib->flags & CLF_PROJECT)
            ci->flags|=CLF_PROJECT;
        classes->Insert(ci);
        if (scheme)
        {
            _class->scheme=new TScheme(_class,scheme,0);
        }
        _class->SetClassIdByDate();
        return _class;
    }
    else
        //_Error(EM_DUPLCLASS_CREAT,0,name);
        _Message(EM_DUPLCLASS_CREAT,0,name);
    return NULL;
};

char * TClass::GetIconFile()
{
    if(IconName)
        return IconName;
    return default_dbm;
};

BOOL TClass::CanAddChildren(TClass*_child)
{
    if(_child==this)
    {
        _Error(EM_INSERTRECURSE);
        return FALSE;
    }
    PObject obj=first;
    while(obj)
    {
        PObject _obj=obj;
        while(_obj)
        {
            if(_obj->_class==_child)
            {
                _Error(EM_INSERTRECURSE);
                return FALSE;
            }
            _obj=_obj->parent;
        }
        obj=obj->class_next;
    }
    return TRUE;
};

void ShowChildren(PClass _child, HSP2D hsp, HOBJ2D* id, gr_float xpos, gr_float ypos)
{
    HOBJ2D ids[2];
    char *_file=_child->GetIconFile();
    HOBJ2D hddib=GetIconDib(hsp,_file);

    POINT2D SrcOrg;		SrcOrg.x=0;			SrcOrg.y=0;
    POINT2D SrcSize;	SrcSize.x=32;		SrcSize.y=32;
    POINT2D DstOrg;		DstOrg.x=xpos;	DstOrg.y=ypos;

    ids[0]=CreateDoubleBitmap2d(hsp,hddib,&SrcOrg,&SrcSize,&DstOrg,&SrcSize);
    ids[1]=0;
    //POINT2D pp[2];pp[0]=DstOrg;pp[1]=DstOrg;
    SetDefaultSrc(hsp,ids[0],_child->deficon,_child->GetIconFile());
    if (!*id)
    {
        if (!(_child->flags&CF_USEDEFICON))
        {
            int imageloaded = _child->image != 0;
            HSP2D image = GetClassImage(_child);
            if(image)
            {
                BOOL modify=IsSpaceChanged2d(image);
                HOBJ2D group=MakeSingleGroup(image);
                HGLOBAL mem=CopyToMemory2d(image,group,NULL);

                DeleteGroup2d(image,group);
                if (!modify)
                    SetSpaceModify2d(image,0);

                ids[1] = PasteFromMemory2d(hsp,mem,&DstOrg,PFC_MOVEOBJECT);
                SetShowObject2d(hsp,ids[0],0);

                POINT2D size;
                if (_child->flags & CF_SIZEABLE)
                    GetObjectOrgSize2d(hsp, ids[1], NULL, &size);
                else
                    size.x = size.y = 1;

                SetObjectSize2d(hsp,ids[0],&size);
                GlobalFree(mem);
                if (!imageloaded)CloseClassImage(_child);
            }
        }
    }
    else 	ids[1]=*id;
    *id=CreateGroup2d(hsp,ids,ids[1]?(INT16)2:(INT16)1);
    SetObjectLayer2d(hsp,*id,16);
}

PClass AddChildren(PClass _class,char* name,HOBJ2D*id,gr_float xpos,gr_float ypos)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в AddChildren(Parent:%s,Child:%s)",_class->name,name);
        LogMessage(s);
    }
#endif
    PClass _child=GetClassByName(name);
    if (_child && _class && id && _class->CanAddChildren(_child))
    {
        if (!(*id))
        {
            HSP2D hsp=_class->scheme->GetSpace();
            if (hsp)
            {
                ShowChildren(_child,hsp,id,xpos,ypos);
                /*TWinItem* w;HOBJ2D*id1;   //добавляем изображение нового имиджа во всех окнах
                for(C_TYPE i=0;i<winmanager->count;i++)
          {
                    w=(TWinItem*)(winmanager->At(i));
            if(lstrcmpi(w->_classname,_class->name)==0)
          {
            *id1=0;
            ShowChildren(_child,w->hsp,id1,xpos,ypos);
          }
          }*/
            }
        }
        _class->AddChild(_child,*id);
        if(project)	{project->ResetEq();}
        return _child;
    }
    return 0;
};


BOOL RemoveChildren(PClass _class,HOBJ2D id,BOOL notconfirm,BOOL updatememory)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в RemoveChildren(Parent:%s,Child:%d)",_class->name,(int)id);
        LogMessage(s);
    }
#endif

    if (_class && id)// id>0
    {
        int varp=0;
        for(int i=0;i<_class->linkcount;i++)
        {
            if (_class->link[i].varhandle==id)
            {
                varp=1;
                break;
            }
        }
        int dellink=1;
        int res=0;
        HSP2D hsp=0;
        if (_class->scheme)
        {
            hsp=_class->scheme->GetSpace();
        }
        if ((varp==0)&& hsp )
        {
            if (hsp)
            {
                if (GetObjectType2d(hsp,id)==otDOUBLEBITMAP2D)
                {
                    HOBJ2D hdib=GetDoubleDibObject2d(hsp,id);
                    char s[64];
                    if(GetRDoubleDib2d(hsp,hdib,s,63))
                    {
                        if (!lstrcmp(s,varpoint_dbm))
                        {
                            DeleteObject2d(hsp,id);
                            res=1;
                        }
                    }
                }
            }
        }
m1:
        res=(varp==0)?_class->RemoveChild(id):res;
        if (res && hsp)
        {
            DeleteObject2d(hsp,id);
        }
        else
        {//2
            if (dellink && _class->linkcount)
            { //1
                if (varp || _class->GetLinkToObject(id))
                {
                    if(notconfirm==0)
                    {
                        if((MSGBOX(0,MAKEINTRESOURCE(IDS_LNK1),_MSG_QSC,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_TASKMODAL)!=IDYES) )
                        {
                            return 0;
                        }
                    }
                    _class->RemoveLinkToObject(id);
                    dellink=0;
                    res=1;
                    goto m1;
                }
            } //1
        }//2
        if (res && projects)
        {
            if ((dellink==0) || updatememory)
                TProject::UpdateLinks(_class);
            ObjectsChanged();
        }
        return res;
    }// id >0
    return 0;
};


int DeleteClass(char* name){
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в DeleteClass(%s)",name);
        LogMessage(s);
    }
#endif
    PClassListInfo pcli=GetClassListInfo(name);
    PClass _class=NULL;
    if(pcli){
        if (pcli->flags&CLF_LOADED){
            _class=pcli->GetClass();
            if (_class->first){ _Error(EM_DELCLASSWITHOBJECTS,0,name); return 0; }
        }
        char filename[256]="";
        BOOL getname=pcli->GetFileName(filename,FALSE);
        // delete from tree
        UpdateClassWindows(UW_DELETE,UW_CLASS,pcli,pcli->library);

        classes->Delete(pcli);
        pcli->library->items->Delete(pcli);
        if(_class)delete _class;
        if (pcli->filename)delete pcli->filename;
        delete pcli;
        if(getname)
        { // Может быть новый
            // Переименовываем в ????????.del
            if (lstrlen(filename)){
                char s[256];
                lstrcpyn(s,filename,lstrlen(filename)-3);
                lstrcat(s,".del");
                //OFSTRUCT  of;
                //OpenFile(s, &of, OF_DELETE);
                ScDeleteFile(s);
                //rename(filename,s);
                ScRenameFile(filename,s);
            }
        }
        return TRUE;
    }

    return 0;
}

int GetRootObject(PClass*_class, PObject*object)
{
    if (project)
    {
        if(_class)
            *_class = project->_class;

        if (object)
            *object = project->object;

        return 1;
    }
    return 0;
}

BOOL IsURL(char*file)
{
    return (strncmpi(file,"http://",7)==0
            || strncmpi(file,"ftp://",6)==0);
};

BOOL LoadProject(char* prj, BOOL destroy)
{
    char TempFileName[MAX_PATH];
    char _prj[MAX_PATH];
    char key_data[256]="";
    BYTE unloadondeactivate=0;
    char *realname=0;
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,">LoadProject         Begin(%s)",prj);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    TProject* oldProject = project;
    if(GetProject(prj))
        return FALSE;

    if(IsURL(prj))
    {
        char TempPath[MAX_PATH];
        GetTempPath(sizeof(TempPath),TempPath);
        GetTempFileName(TempPath,"spj",0,TempFileName);
        if(InitInet())
        {
            int rez=URLLoadFile(prj,TempFileName,1);
            if(!rez)
            {
                ScDeleteFile(TempFileName);
                return FALSE;
            }
            realname = prj;
            prj = TempFileName;
        }
        else
            return FALSE;
    }

    SetStatusIcon(STATUS_OPERATION,4);
    char TempDir[MAX_PATH]="";
    {
        UINT type = GetFileType(prj,0);
        if(type == FILE_RAR)
        {
            if(InitInet())
            {
                //создаем временную папку
                char TempPath[MAX_PATH];
                GetTempPath(sizeof(TempPath),TempPath);
                GetTempFileName(TempPath,"spj",0,TempDir);
                ScDeleteFile(TempDir);
                MakeDirectory(TempDir);
                //ScDeleteDir(TempDir);// test
                //MakeDirectory(TempDir);

                BOOL rez=URLDecode(prj,TempDir);
                //ScDeleteDir(TempDir);// test
                if(realname)
                    ScDeleteFile(TempFileName); // т.е. URL
                if(rez)
                {
                    //if(!realname)realname=prj; неверное определение пути к проекту
                    lstrcpy(_prj,TempDir);
                    char ini[MAX_PATH];
                    lstrcpy(ini,TempDir);
                    lstrcat(ini,"\\project.ini");
                    char prj_name[256];
                    GetPrivateProfileString("MAIN","project","project.spj",prj_name,256,ini);

                    lstrcat(_prj,"\\");
                    lstrcat(_prj,prj_name);
                    {
                        char s[64];
                        GetPrivateProfileString("MAIN","deactivate","",s,sizeof(s),ini);
                        if(lstrcmpi(s,"unload")==0)
                            unloadondeactivate=1;
                    }

                    prj=_prj;
                    if(!realname)realname=prj;
                    GetPrivateProfileString("MAIN","keydata","",key_data,256,ini);
                }
                else
                    return FALSE;
            }
            else
                return FALSE;
        }
    }

    TDOSStream st(prj, TDOSStream::stOpenRead);
    if(st.status == 0)
    {
        if(st.ReadWord() != PRJ_SIGNATURE)
        {
            SetStatusIcon(STATUS_OPERATION,0);
            return _Error(EM_BADPROJETCTFILE,0,prj);
        }
    }
    else
    {
        SetStatusIcon(STATUS_OPERATION,0);
        return FALSE;
    }

    if(destroy)
    {
        EXEC_FLAGS=0;
        CloseProject(project); // Close previously open project
        calcedsteps=0;
        SetLastSteps();
    }

    if(project)
        project->_exec_flags=EXEC_FLAGS;

    project = new TProject(st);

    if(TempDir[0])
        project->tmpfile=NewStr(TempDir);

    SetStatusIcon(STATUS_OPERATION,0);
    if(!project->_class)
    {
        delete project;
        project=oldProject;
        return FALSE;
    }

#ifdef LOGON
    LogMessage(">LoadProject        END");
#endif
    TProject* old = project;
    if(project->Ok())
    {
        if(realname)project->realname=NewStr(realname);
        SetActiveProject(project);
        if(key_data[0])
            project->hyperadd=new THyperAdd(key_data);
        project->unloadondeactivate=unloadondeactivate;
        return TRUE;
    }
    else
    {
        delete project;
        project=old;
        return FALSE;
    };
};

BOOL CreateClassImage(char*name,HSP2D hsp,char* _name)
{
    PClass _class=GetClassByName(name);
    if (!_class)
        return FALSE;
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в CreateClassImage(%s)",name);
        LogMessage(s);
    }
#endif

    if (_class->image)
        return FALSE;

    if (_name)
        _class->image = new TScheme(_class,_name,0);
    else
    {
        if(!hsp)
        {
            char s[256];
            if(SCGetScVarS("template_image2d",s))
            {
                hsp=LdSpace2d(0,s);
                SetSpaceModify2d(hsp,TRUE);
            }
            if(!hsp)
                hsp=CreateSpace2d(0,scheme_path);
        }
        _class->image = new TScheme(_class,hsp);
    }
    _class->SetModify();
    return TRUE;
};

HSP2D GetClassImage(PClass _class){
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в GetClassImage(%s)",_class->name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    if (!_class->image)
    {
        _class->UpdateItem(CR_IMAGE);
        if (!_class->image)
        {
#ifdef LOGON
            LogMessage("У класса нет изображения");
#endif
            return 0;
        }
    }
    return _class->image->GetSpace();
};


void _ChkClassImage(PClass _base,PClass _class,INT16 i,HSP2D&hsp,HOBJ2D &group)
{
#ifndef RUNTIME
#ifdef LOGON
    {
        char s[256];
        LogMessage("Зашел в _ChkClassImage");
    }
#endif

    HSP2D basehsp=_base->scheme->GetSpace();
    int wasframe=(GetObjectType2d(basehsp,OID_FRAME2d)==otEDITFRAME2D);
    HOBJ2D _id=_base->childs[i].handle;
    //INT16 count=GetGroupItemsNum2d(basehsp,_id);
    ///if (count==2)DeleteObject2d(basehsp,GetGroupItem2d(basehsp,_id,1));
    while(GetGroupItemsNum2d(basehsp,_id)>1)
    {
        FullDeleteObject2d(basehsp,GetGroupItem2d(basehsp,_id,1));
    }

    HOBJ2D objfirst=GetGroupItem2d(basehsp,_id,0);
    ShowObject2d(basehsp,objfirst);
    if (_class->image && (!(_class->flags&CF_USEDEFICON)))
    {
        if (hsp==0 )
        {
            long size;
            HSP2D __hsp=_class->image->GetSpace();
            BOOL changed=IsSpaceChanged2d(__hsp);
            HGLOBAL mem=SaveToMemory2d(__hsp,&size);
            if (changed) SetSpaceModify2d(__hsp,1);
            char cp[260];
            _class->GetClassListInfo()->library->GetPath(cp);
            hsp=LdFromMemory2d(0,mem,cp);
            group=MakeSingleGroup(hsp);
            GlobalFree(mem);
        }
        POINT2D org;
        GetObjectOrgSize2d(basehsp,_id,&org,NULL);
        HGLOBAL mem=CopyToMemory2d(hsp,group,NULL);
        HOBJ2D newobj=PasteFromMemory2d(basehsp,mem,&org,PFC_MOVEOBJECT);
        GlobalFree(mem);
        if (newobj)
        {
            AddGroupItem2d(basehsp, _id, newobj);
            HOBJ2D objfirst = GetGroupItem2d(basehsp,_id,0);
            HideObject2d(basehsp,objfirst);
            if (_class->flags & CF_SIZEABLE)
            {
                POINT2D size;
                GetObjectOrgSize2d(basehsp,newobj,NULL,&size);
                SetObjectSize2d(basehsp,objfirst,&size);
            }
            else
            {
                POINT2D size;
                size.x = size.y = 1;
                SetObjectSize2d(basehsp,objfirst,&size);
            }
        }
    }
    else
    {
        POINT2D size;size.x=size.y=32;
        SetObjectSize2d(basehsp,objfirst,&size);
        SetDefaultSrc(basehsp,objfirst,_class->deficon,_class->GetIconFile());
    }
    int layer=GetObjectLayer2d(hsp,_id);
    if(layer<16 || layer>23)layer=16;
    SetObjectLayer2d(hsp,_id,layer);
    // Фрейм
    if (wasframe){
        HOBJ2D _cur=GetCurrentObject2d(basehsp);
        if (_cur==_id){
            DeleteEditFrame2d(basehsp);
            if(_base->scheme->window){
                if(_base->scheme->window->object==_id)
                    _base->scheme->window->CreateImageFrame();
            }
        }
    }
#endif
};
void ChkClassImage(PClass _class,HSP2D &hsp){
    HOBJ2D group=0;
    for(C_TYPE i=0;i<classes->count;i++){
        PClassListInfo pcli=(PClassListInfo)classes->At(i);
        if (pcli->flags&CLF_LOADED){
            PClass _base=pcli->_class;
            if (_base && _base->scheme && _base->scheme->Loaded()){
                for(C_TYPE j=0;j<_base->childcount;j++){
                    PClassListInfo _pcli=_base->childs[j].type;
                    if (_pcli->flags&CLF_LOADED &&_pcli->_class==_class){
                        _ChkClassImage( _base,_class,j,hsp,group);
                    }
                }	} } }
}
BOOL CloseClassImage(PClass _class,BOOL update){
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в CloseClassImage(%s)",_class->name);
        LogMessage(s);
    }
#endif

    if (_class->image){
        if (_class->image->Loaded()){
            HSP2D hsp=_class->image->GetSpace();
            if (IsSpaceChanged2d(hsp) && update && (!(_class->Protected())) ){
                DeleteAllUnusedTool2d(hsp);
                //*--------------------------------------------------------------
                //********  Обновить все пространства имеющие эти имиджи *******/
                HSP2D image=0;
                //	int group=0;
                if (GetNextObject2d(hsp,0)==0){
                    delete(_class->image);_class->image=NULL;_class->NS.image=1;
                }
                ChkClassImage(_class,image);
                if (image)DeleteSpace2d(image);
                //*-----------------------------------------
                if (_class->image){
                    _class->image->SetScheme();
                }
                _class->UpdateIcon();
                if (!_class->UpdateClassFile())return FALSE;
            }}
        delete(_class->image);_class->image=NULL;
        return TRUE;
    }
    return FALSE;
};
BOOL UpdateClassDefIcon(PClass _class){
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в UpdateClassDefIcon(%s)",_class->name);
        LogMessage(s);
    }
#endif

    BOOL wasimage=(_class->image)!=0;
    if (!wasimage) GetClassImage(_class);

    HSP2D image=0;
    ChkClassImage(_class,image);
    if (image)DeleteSpace2d(image);

    if (!wasimage) CloseClassImage(_class);
    return 0;
};

BOOL CreateDefClassScheme(char*name){
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в CreateDefClassScheme(%s)",name);
        LogMessage(s);
    }
#endif

    PClass _class=GetClassByName(name);
    if (!_class)
        return 0;

    if (_class->scheme || _class->flags & CF_SCHEME)
        return FALSE;

    _class->scheme=new TScheme(_class);
    _class->scheme->CreateSpace();
    _class->flags|=CF_SCHEME;
    _class->SetModify();
    return TRUE;
};

HSP2D __ConvertClassScheme(PClass _class, BOOL fromdisk, BOOL createnew)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в ConvertClassScheme(%s,%d,%d)",_class->name,fromdisk,createnew);
        LogMessage(s);
    }
#endif

    if (fromdisk)
    {
        HSP2D hsp;
        _class->UpdateItem(CR_SCHEME);

        if (_class->scheme)
        {
            hsp = _class->scheme->GetSpace();
            BOOL _remove=0;
            for(INT16 i=0;i < _class->linkcount;i++)
            {
                HOBJ2D h = _class->link[i].handle;
                if (GetObjectType2d(hsp,h) != otLINE2D)
                {
#ifdef LOGON
                    {
                        LogMessage("@@@@ Не найденна связь");
                    }
#endif

                    if (_remove ||
                        MSGBOX(0,MAKEINTRESOURCE(IDS_MSG9),_MSG_ERROR,
                               MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2|MB_TASKMODAL)==IDYES)
                    {
                        _remove=1;
                        RemoveLink(_class,h);i=0;
                    }
                }
                else
                {
                    int layer=GetObjectLayer2d(hsp,h);
                    if (layer<24)
                    {
                        SetObjectLayer2d(hsp,h,24);
                    }
                }
            }

            // Преобразовать в отображаемый формат
            for(i = 0; i < _class->childcount; i++)
            {
                HOBJ2D id=_class->childs[i].handle;
                INT16 type=GetObjectType2d(hsp,id);

                if (type==otGROUP2D)
                {
                    INT16 count=GetGroupItemsNum2d(hsp,id);
                    if (count==1)
                    {// Еще не вносили изменений
                        PClass _child = _class->childs[i].type->GetClass();
                        if (_child->flags & CF_USEDEFICON)
                        {
                            SetDefaultSrc(hsp, GetGroupItem2d(hsp,_class->childs[i].handle,0), _child->deficon, _child->GetIconFile());
                        }
                        else
                        {
                            HSP2D image;
                            image=GetClassImage(_child);

                            if (image)
                            {
                                BOOL modify=IsSpaceChanged2d(image);
                                BOOL created=0;
                                HOBJ2D group=MakeSingleGroup(image,&created);
                                if (group)
                                {
                                    HGLOBAL mem=CopyToMemory2d(image,group,NULL);
                                    //  CopyToClipboard2d(image,GetCurrentObject2d(image));

                                    for(INT16 j=i;j<_class->childcount;j++)
                                    {
                                        if (_class->childs[i].type == _class->childs[j].type)
                                        {
                                            HOBJ2D _id = _class->childs[j].handle;
                                            INT16 _type = GetObjectType2d(hsp,id);
                                            if (_type==otGROUP2D)
                                            {
                                                INT16 _count = GetGroupItemsNum2d(hsp,_id);
                                                if (_count==1)
                                                {
                                                    POINT2D org;
                                                    GetObjectOrgSize2d(hsp,_id,&org,NULL);
                                                    if (_class->childs[i].flags&CHF_NEEDUPDATE)
                                                    {
                                                        _class->childs[i].x=org.x;
                                                        _class->childs[i].y=org.y;
                                                        _class->childs[i].flags&=(~CHF_NEEDUPDATE);
                                                    }
                                                    HOBJ2D newobj=PasteFromMemory2d(hsp,mem,&org,PFC_MOVEOBJECT|PFC_DIBS|PFC_DDIBS);
                                                    // int newobj=PasteFromClipboard2d(hsp,org,PFC_MOVEOBJECT|PFC_ALL);
                                                    if (newobj)
                                                    {
                                                        AddGroupItem2d(hsp,_id,newobj);
                                                        HOBJ2D objfirst=GetGroupItem2d(hsp,_id,0);
                                                        HideObject2d(hsp,objfirst);
                                                        if (((_class->childs[i].type->GetClass()->flags)&CF_SIZEABLE)&&
                                                            (_class->childs[i].flags&CHF_WASSIZED)
                                                            )
                                                        {
                                                            POINT2D size;
                                                            GetObjectOrgSize2d(hsp,objfirst,NULL,&size);
                                                            SetObjectSize2d(hsp,newobj,&size);
                                                        }
                                                        else
                                                        {
                                                            POINT2D size;size.x=size.y=1;
                                                            SetObjectSize2d(hsp,objfirst,&size);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    GlobalFree(mem);
                                    if (created)DeleteGroup2d(image,group);
                                }
                                //!!!!
                                if (!modify)
                                    SetSpaceModify2d(image,0);

                                CloseClassImage(_class->childs[i].type->_class);
                            } // Нет образа
                            else
                            {

                                HOBJ2D objfirst = GetGroupItem2d(hsp,id,0);
                                SetDefaultSrc(hsp, objfirst, _child->deficon,_child->GetIconFile());
                                ShowObject2d(hsp, objfirst);

                            }
                        } // нет Иконк по умолчанию

                    } // еще не поменяли
                }// чилд - группа

                else
                {
                    _Message("Children not found on scheme");
#ifdef LOGON
                    LogMessage("@@@@ Чилд не является группой.");
#endif
                }


                int layer=GetObjectLayer2d(hsp,id);
                if ((layer < 16) || (layer > 24))
                    layer = 16;
                SetObjectLayer2d(hsp, id, layer);
            }//for по преобразованию
            SetSpaceModify2d(hsp,0);
            return hsp;
            // End converting from disk
        }// no scheme
    }
    else
    {
        HSP2D hsp=_class->scheme->GetSpace();
        if(createnew)hsp=GetDuplicateSpace(hsp);
        for(INT16 i=0;i<_class->childcount;i++)
        {
            HOBJ2D id=_class->childs[i].handle;
            INT16 type=GetObjectType2d(hsp,id);
            if (type==otGROUP2D)
            {
                INT16 count=GetGroupItemsNum2d(hsp,id);
                if (count==2)
                {
                    HOBJ2D f=GetGroupItem2d(hsp,id,0);
                    HOBJ2D objid=GetGroupItem2d(hsp,id,1);

                    if (((_class->childs[i].type->GetClass()->flags)&CF_SIZEABLE))
                    {
                        POINT2D size;size.x=size.y=32;
                        if(GetObjectOrgSize2d(hsp,id,0,&size))
                        {
                            SetObjectSize2d(hsp,f,&size);
                        }
                    }

                    DeleteObject2d(hsp,objid);
                    //	SetShowObject2d(hsp,GetGroupItem2d(hsp,id,0),1);

                    ShowObject2d(hsp,f);
                    if (!((_class->childs[i].type->GetClass()->flags)&CF_SIZEABLE))
                    {
                        POINT2D size;size.x=size.y=32;
                        SetObjectSize2d(hsp,f,&size);
                    }
                }
            }
        }
        DeleteAllUnusedTool2d(hsp);
        return hsp;
    }
    return 0;
};

// Эта просто вызывает предыдущую, но она меняеет иконки внизу Statusbar
HSP2D ConvertClassScheme(PClass _class,BOOL fromdisk,BOOL createnew)
{
    SetStatusIcon(STATUS_OPERATION,6);
    HSP2D hsp = __ConvertClassScheme(_class,fromdisk,createnew);
    SetStatusIcon(STATUS_OPERATION,0);
    return hsp;
}

HSP2D GetClassScheme(char*name, PClass *_c, BOOL createcopy)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в GetClassScheme(%s)",name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    PClass _class = GetClassByName(name);

    if (!_class || ((_class->flags & CF_SCHEME)==0))
        return 0;
    if(_c)
        *_c = _class;

    HSP2D hsp;
    if (!_class->scheme)
    {
        hsp = ConvertClassScheme(_class, 1);
        if (createcopy)
        {
            _class->scheme->hsp = 0;
            delete(_class->scheme);
            _class->scheme=NULL;
        }
        else
            _class->scheme->flags|=SF_EDITING;
    }
    else    // class have scheme
    {
        hsp = _class->scheme->GetSpace(createcopy);
        //if(_class->scheme->window && _class->scheme->window->)
    }
    return hsp;
};

BOOL CloseClassScheme(PClass _class)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в CloseClassScheme(%s)",_class->name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    if (SaveClassScheme(_class))
    {
        delete(_class->scheme);
        _class->scheme=NULL;
        return TRUE;
    }
    return FALSE;
}

typedef struct tagclassdate
{
    unsigned int secunde: 5;
    unsigned int minute: 6;
    unsigned int hour: 5;
    unsigned int day: 5;
    unsigned int month: 4;
    unsigned int year: 7;
}classdate;

BOOL SaveClassScheme(PClass _class)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в SaveClassScheme(%s)",_class->name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    if (_class && _class->scheme)
    {
        if (_class->scheme->Loaded())
        {
            HSP2D hsp=_class->scheme->GetSpace();//0
            if (IsSpaceChanged2d(hsp) && (!(_class->Protected())  ))
            {
                // Преобразовать схему в короткий формат
                //Т.е. удалить изображения объектов
                ConvertClassScheme(_class,0);
                // DeleteUnusedTools2d();
                _class->scheme->flags&=~SF_EDITING;
                _class->scheme->SetScheme();
                if(!_class->UpdateClassFile())
                    return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
};

void TClass::SetClassIdByDate()
{
    struct time _t;
    date _d;

    union{
        classdate _date;
        UINT32 id;
    };


    getdate(&_d);
    gettime(&_t);

    _date.year=_d.da_year-1996;
    _date.month=_d.da_mon;
    _date.day=_d.da_day;

    _date.hour=_t.ti_hour;
    _date.minute=_t.ti_min;
    _date.secunde=_t.ti_sec/2;

    _classid=id;
};
BOOL TClass::GetCreateTime(char*s,int n){
    if(_classid){
        union{
            classdate _date;
            UINT32 id;
        };
        id=_classid;
        int year=_date.year+1996;
        sprintf(s,"%d.%d.%d   %d:%d",_date.day,_date.month,year,_date.hour,_date.minute);
    }else lstrcpyn(s,"Не известно",n);
    return TRUE;
};


BOOL TClass::UpdateItem(INT16 __c)
{
    char file[256];
    if (GetClassFile(file,FALSE))  // Проверка на случай отсутствия файла
    {
        TDOSStream st(file,TDOSStream::stOpenRead);
        if(listinfo->flags & CLF_PACKFILE)
            st.Seek(listinfo->posinpack);

        if (!st.status && (st.ReadWord()==CLASS_SIGNATURE))
        {
            long ver=st.ReadLong();
            if (ver)
            {
            }
            INT16 _l[]={__c,0};
            Update(st,_l);
        }
        return TRUE;
    }
    return FALSE;
}

BOOL TClass::UpdateClassFile()
{
#ifdef DEMO || RUNTIME
    return TRUE;
#endif
    if(listinfo->flags&CLF_PACKFILE)return FALSE;

#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в class::UpdateClassFile() this==%s",name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    BOOL wasscheme=(scheme!=NULL),wastext=(text!=NULL),wasimage=(image!=0),
            wasinfo=(description!=NULL),wasicon=(wasicon!=NULL);
    //if (icon)f|=4;
    char file[256];
    if (GetClassFile(file,FALSE))  // Проверка на случай отсутствия файла
    {
#ifdef LOGON
        {
            char s[256];
            wsprintf(s,"Чтение из : %s",file);
            LogMessage(s);
        }
#endif

        TDOSStream st(file,TDOSStream::stOpenRead);
        if (!st.status && (st.ReadWord()==CLASS_SIGNATURE))
        {
            long ver=st.ReadLong();
            if (ver)
            {
            }
            Update(st,NULL);
        }
        else
        {
            _Message("Class file open Error (01)");
#ifdef LOGON
            LogMessage("Class file open Error (01)");
#endif

        }
    }
    if (!(t_flags&TCF_WASSAVE))
    {
        t_flags|=TCF_WASSAVE;
        GetClassFile(file,TRUE);
    }
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Запись в : %s",file);
        LogMessage(s);
    }
#endif

    {  TDOSStream st(file,TDOSStream::stCreate);
        st.WriteWord(CLASS_SIGNATURE);
        st.WriteLong(version);
        Store(st);
        if (st.status){
            _Message("Class file create Error (02)");
            return FALSE;}
    }
    if (!wasscheme){if (scheme)delete scheme;scheme=0;}
    if (!wastext){if (text)delete text;text=0;}
    if (!wasimage){if(image)delete image;image=0;}
    if (!wasinfo){if(description)delete description;description=0;}
    if (!wasicon){if(icon)delete icon;icon=0;}
    //if (!(f&4)){delete icon;icon=0;}
    int NSsize=sizeof(NS);
    memset(&NS,0,NSsize);
    return TRUE;
};

BOOL AcceptChanges(BOOL accept,TStringCollection*items)
{
    BOOL error=FALSE;
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в AcceptChanges(%d)",accept);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif
    SetStatusIcon(STATUS_OPERATION,3);

#ifdef PROTECT
    BOOL _g=accept;
    accept=projects->libs->count;
    if(!IsRegistered3()){
        TProject*p=projects;
        if(p){
            if(!p->CanSaveProject())
            {
                accept = FALSE;
                HWND to=mainwindow->HWindow;
                // Посылаем сообщение об ограничениях.
                if(_g){
#ifdef WIN32
                    PostMessage(to,WM_COMMAND, MAKEWPARAM((UINT)(CM_REMIND),(UINT)(0)),0);
#else
                    PostMessage(to,WM_COMMAND,CM_REMIND,0);
#endif
                    error=_g;
                }
                // мало ли че , если поправили то вываливаемся с ошибкой.
                if(to!=mainwindow->HWindow)(void*)classes=projects;
                // только в случае записи вываливаемся.
                if(_g) goto end_accept;
                //   return FALSE;
            }
        }else{
            // на всякий случай генерация GPF
            if(projects!=p){
                (void*)projects=mainwindow; // генерация GPF
            }
        }
    }
    accept=_g;
#endif

    SaveAllModifyClasses();

#if !defined DEMO
    C_TYPE index;
    if(accept){
        TProject*prj=projects;
        while(prj){
            if (prj->IsModifyed()){
                if(items==NULL || items->Search("$1",index))prj->Store();
                else prj->SetModify(FALSE);
            }
            TProject * _p=project;
            project=prj;
            if(SCGetScVar("vars_save",1)){
                if (prj && prj->IsVarModifyed()){
                    if(items==NULL || items->Search("$2",index))SaveSetState(prj->object,NULL);
                    else prj->VarChange(FALSE);
                }
            }
            project=_p;
            prj=prj->next;
        }
    }
#endif

    for(C_TYPE i=0;i<classes->count;i++){
        PClassListInfo pcli=(PClassListInfo)classes->At(i);
        if (pcli->flags&CLF_LOADED){
            PClass _class=pcli->GetClass();
            if (_class->t_flags&TCF_WASSAVE){
#ifdef LOGON
                {
                    char s[256];
                    wsprintf(s,"Запись класса %s",_class->name);
                    LogMessage(s);
                }
                TLOGoffset __offset1(2);
#endif
                char _new[25600];
                char _old[25600];
                _class->GetClassFile(_new,TRUE);
                _class->t_flags&=~TCF_WASSAVE;
                _class->GetClassFile(_old,TRUE);
#if !defined DEMO
                if (accept && (items==NULL || items->Search(_class->name,index)) ){
                    if (!(_class->t_flags&TCF_NEWCLASS))
                    {// делете bak file
                        char s[25600];
                        lstrcpyn(s,_old,lstrlen(_old)-3);
                        lstrcat(s,".bak");
                        //OFSTRUCT  of;
                        //OpenFile(s, &of, OF_DELETE);
                        ScDeleteFile(s);
                        //error|=rename(_old,s);
                        error=error||(!ScRenameFile(_old,s));
#ifdef LOGON
                        {
                            char ss[256];
                            wsprintf(ss,"Удаление %s",s);
                            LogMessage(ss);
                        }
#endif

                    }else {_class->t_flags&=(~TCF_NEWCLASS);
#ifdef LOGON
                        LogMessage("Это новый класс");
#endif
                    }
                    //error|=rename(_new,_old);
                    error=error||(!ScRenameFile(_new,_old));
#ifdef LOGON
                    {
                        char ss[256];
                        wsprintf(ss,"Переименование `%s` в `%s`",_new,_old);
                        LogMessage(ss);
                    }
#endif

                }else
#endif
                {
                    ScDeleteFile(_new);
#ifdef LOGON
                    {
                        char ss[2560];
                        wsprintf(ss,"Удаление : %s",_new);
                        LogMessage(ss);
                    }
#endif

                    //OFSTRUCT  of;
                    //OpenFile(_new, &of, OF_DELETE);
                }
            }}}
    if (error){
        _Error(EM_ACCEPTERROR);

    }
end_accept:
    SetStatusIcon(STATUS_OPERATION,0);
    return (error==FALSE);
};
BOOL IsNeedAcceptChanges(char *s){
    UINT32 f=0;
    SaveAllModifyClasses();
    for(C_TYPE i=0;i<classes->count;i++){
        PClassListInfo pcli=(PClassListInfo)classes->At(i);
        if (pcli->flags&CLF_LOADED){
            if (pcli->_class->t_flags&TCF_WASSAVE){
                if(pcli->flags&CLF_PROJECT)f|=2;else f|=1;
            }
        }}

    TProject *prj=projects;
    while(prj){
        if(prj->IsModifyed())f|=4;
        if( prj->IsVarModifyed() )f|=8;
        prj=prj->next;
    }
    if(s){
        *s=0;
        if(f){
            //  lstrcat(s,"\n");
            for(int i=0;i<4;i++){
                if(f&(1<<i)){
                    char _s[25600];
                    LoadString(HResource,IDS_CHANGES+i,_s,sizeof(_s));
                    lstrcat(s,"  • ");
                    lstrcat(s,_s);
                    lstrcat(s,"\n");
                }
            }
            lstrcat(s,"\n");
        }
    }
    return f!=0;
};

BOOL GetUniqueClassName(char*name,BOOL copy){
    int len=lstrlen(name);
    if(copy){
        char *_name=name;
        char s[128];
        lstrcpy(s,name);
        AnsiUpper(s);
        char *_pos=strstr(s,"_COPY_");

        if(_pos==NULL){
            _pos=s+lstrlen(s);
            _name+=lstrlen(name);
            lstrcat(s,"_copy_1");
        }else{
            _name+=(_pos-s);
        }
        _pos+=6;
        char *_ps=_pos;
        BOOL ok=TRUE;
        while (*_ps){
            if(*_ps<'0' || *_ps>'9'){ok=FALSE;break;}
            _ps++;
        }
        if(ok){
            int c=atol(_pos);
            while(c<100){
                *_name=0;
                wsprintf(name+lstrlen(name),"_copy_%d",c);
                if(GetClassListInfo(name))c++; else return TRUE;
            }
        }

    }
    if (len<3){
        lstrcat(name,"___");
        name[3]=0;
    };
    struct  time t;
    struct date d;
    gettime(&t);
    getdate(&d);
    sprintf(name+lstrlen(name),"_%x%x%x%x_%x%x",t.ti_hour,t.ti_min,t.ti_sec,t.ti_hund,d.da_mon,d.da_day,d.da_year);

    PClassListInfo pcli=(PClassListInfo)name;
    long _d=1;
    char s[256];
    char ss[20];
    ss[0]=0;
    while (pcli){
        lstrcpy(s,name);
        lstrcat(s,ss);
        pcli=GetClassListInfo(s);
        if (pcli){
            _d++;
            wvsprintf(ss,"%d",&_d);
        }};
    lstrcat(name,ss);
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в GetUniqueClassName(%s)",name);
        LogMessage(s);
    }
#endif

    return TRUE;
};

int strpos(char*str,char*fragment);

#ifndef RUNTIME
BOOL CreateNewProject(char * filename,char*name){
    {
#ifdef LOGON
        LogMessage("Зашел в CreateNewProject");
        TLOGoffset __offset(2);
#endif

        TDOSStream st(filename,TDOSStream::stOpenRead|TDOSStream::stNoWarning);
        if(st.status==0){
            if (
                MSGBOX(0,MAKEINTRESOURCE(IDS_MSG10),MAKEINTRESOURCE(IDS_NEWPRJ),
                       MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2|MB_TASKMODAL)!=IDYES)return 0;
        }
    }
    char path[256];
    GetPath(filename,path);
    CloseProject(project);
    calcedsteps=0;
    SetLastSteps();

    EXEC_FLAGS=0;
    TLibrary* _old=DefaultLib;
#ifdef WIN32
    WIN32_FIND_DATA fdata;
    memset(&fdata,0,sizeof(fdata));
    fdata.dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
    HANDLE fhandle=FindFirstFile(path,&fdata);
    if (fhandle==INVALID_HANDLE_VALUE)
#else
    find_t ffblk;
    if (_dos_findfirst(path,_A_SUBDIR,&ffblk))
#endif
    {
        char s[512];
        char ss[128];
        LoadString(HResource,IDS_MSG11_,ss,sizeof(ss));
        sprintf(s,ss,path);
        if (
            MSGBOX(0,s,MAKEINTRESOURCE(IDS_NEWPRJ),
                   MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON1|MB_TASKMODAL)!=IDYES)return 0;

        if (!MakeDirectory(path)){
            _Error(EM_DIRCREATEERROR,0,path);
            return 0;
        }
    }else{

#ifdef WIN32
        FindClose(fhandle);
#endif
        // Такой каталог есть
        char filefind[256];
        lstrcpy(filefind,path);
        lstrcat(filefind,"\\*.prj");
        BOOL alredy=0;
#ifdef WIN32
        memset(&fdata,0,sizeof(fdata));
        fdata.dwFileAttributes=FILE_ATTRIBUTE_NORMAL;
        HANDLE fhandle=FindFirstFile(filefind,&fdata);	// address of returned information
        BOOL ok=(fhandle!=INVALID_HANDLE_VALUE);
        if (ok){
            while(ok){
                char ss[MAX_PATH];
                lstrcpy(ss,fdata.cFileName);
                AnsiUpper(ss);
                if (strpos(ss,".PRJ"))alredy=1;
                ok=FindNextFile(fhandle,&fdata);
            } FindClose(fhandle);
        }
#else
        int done;
        memset(&ffblk,0,sizeof(ffblk));
        done = _dos_findfirst(filefind,_A_NORMAL,&ffblk);
        while (!done) {
            char ss[64];
            lstrcpy(ss,ffblk.name);
            AnsiUpper(ss);
            if (strpos(ffblk.name,".PRJ"))alredy=1;
            done = _dos_findnext(&ffblk);
        };
#endif
        if (alredy){
            char s[512];
            char ss[128];
            LoadString(HResource,IDS_MSG12_,ss,sizeof(ss));

            sprintf(s,ss,path);

            if(MSGBOX(0,s,MAKEINTRESOURCE(IDS_NEWPRJ),
                      MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2|MB_TASKMODAL)!=IDYES)return 0;
        }
    }
    UINT filehandle=_lcreat(filename,OF_SHARE_DENY_WRITE);
    if (filehandle) _lclose(filehandle);else
    {
        _Error(EM_PRGFILEECREATERROR,0,filename);
        return 0;
    };
#ifdef LOGON
    LogMessage("Началось создание проекта");
#endif

    // projectpath=NewStr(path);
    // projectfile=NewStr(filename);

    project=new TProject(filename,name);
    if(project->Ok()){
#ifdef LOGON
        LogMessage("Создание проекта успешно завершенно");
#endif
        return TRUE;
    }else {
        DefaultLib=_old;
        return FALSE;}
    //  DefaultLib=_old;
};
BOOL GetNewProjectPath(char*file){
    lstrcpy(file,sc_path);
    lstrcat(file,"\\projects\\project_");
    int len=lstrlen(file);
    BOOL rez=FALSE;
    int i=0;
    while(!rez){
        rez=FALSE;
        rez=IsDirEmpty(file);
        if(!rez){
            char s[8];
            wsprintf(s,"%d",i);i++;
            int l=lstrlen(s);
            lstrcpy(file+len-l,s);
        };
        if(i>400){return FALSE;}
    }
    return TRUE;
}
BOOL CreateSimplyProject(){
    calcedsteps=0;
    SetLastSteps();
    EXEC_FLAGS=0;
    TLibrary* _old=DefaultLib;
    char file[260];
    //
    if(!GetNewProjectPath(file))return FALSE;
    if (!MakeDirectory(file)){
        _Error(EM_DIRCREATEERROR,0,file);
        return 0;
    }
    lstrcat(file,"\\project.spj");
    char mnclassname[256]="Root";
    GetUniqueClassName(mnclassname);
    project=new TProject(file,mnclassname);
    if(project->Ok()){
#ifdef LOGON
        LogMessage("Создание проекта успешно завершенно");
#endif
        return TRUE;
    }else {
        DefaultLib=_old;
        return FALSE;
    }
};
#endif
BOOL SetActiveProject(TProject*prj)
{
    if(prj){
        project=prj;
        //SetMainTitle(project->file);
        project->SetTitle();
        return TRUE;
    }
    return FALSE;
};

TProject* GetProject(char*s)
{
    if(!s)
        return project;

    TProject * prj = projects;
    while(prj)
    {
        if (!lstrcmpi(prj->file, s) || (prj->realname && (!lstrcmpi(prj->realname,s))))
        {
            return prj;
        }
        prj = prj->next;
    }
    return NULL;
};

BOOL CloseProject(TProject* p)
{
    //if(p)MessageBox(0,p->path,p->file,0);
    TProject * prj=p?p:project;
    if(!prj)
        return TRUE;
#ifdef LOGON
    LogMessage("Зашел в CloseProject");
    LogMessage(prj->path);
    LogMessage(prj->file);
    TLOGoffset __offset(2);
#endif
    //MessageBox(0,prj->path,prj->file,0);
    if(prj==project && (EXEC_FLAGS&EF_STEPACTIVE))
    {
        prj->AddPostprocess(PPF_UNLOADPROJECT);
        return TRUE;
    }
#ifdef LOGON
    LogMessage("prj->projecttoactivate");
#endif
    TProject*_prj = prj->projecttoactivate ? GetProject(prj->projecttoactivate) : NULL;
    if(_prj)
        prj->nohandleunload = 1;
    //    CloseProject(p);
#ifdef LOGON
    LogMessage("Stop");
#endif
    Stop();
#ifdef LOGON
    LogMessage("SaveAllModifyClasses");
#endif
    SaveAllModifyClasses();
#ifdef PROTECT
    // удаляем ._CL
    if(!IsRegistered3())
    {
        AcceptChanges(FALSE);
    }
#endif

    delete prj;
    if(prj == project)
    {
        project=NULL;
        DefaultLib=0;
        EXEC_FLAGS=0;
        SetMainTitle(NULL);
    }

    GetDefaultLib();

    if(_prj)
    {
        SetActiveProject(_prj);
        EXEC_FLAGS = _prj->_exec_flags;
        if(EXEC_FLAGS&EF_RUNNING)
            Run();
    }
    return TRUE;
}

BOOL SaveProject()
{
    if (project)
    {
        TDOSStream st(project->file,TDOSStream::stCreate);
        if (!st.status)
        {
            st.WriteWord(PRJ_SIGNATURE);
            project->Store();
            return TRUE;
        }
    }
    return FALSE;
}

// Переменные
INT16 GetObjectVarId(PClass _class,char* name)
{
    if (_class)
    {
        for(INT16 i=0;i<_class->varcount;i++)
        {
            if (!lstrcmpi(_class->vars[i].name,name))
                return i;
        }
    }
    return -1;
}

char* GetVarType(PClass _class,INT16 id)
{
    return _class->vars[id].classType->GetClassName();
}

pointer GetObjectVar(PObject po, INT16 id)
{
    return po->GetVar(id);
}

BOOL SetObjectVar(PObject po, INT16 id, pointer newvar)
{
    if(po)
    {
        pointer var = po->GetVar(id);
        if (var)
        {
            int size = (int)po->_class->vars[id].classType->_class->varsize;
            if (po->_class->vars[id].classType->isSTRING())
            {
                char* str = (char*)newvar;
                lstrcpyn((char*)var, str, 255);
            }
            else
            {
                memcpy(var, newvar, size);
            }
            memcpy(((char*)po-> vars[id]) + size, var, size); //NEW_TO_OLD

            return TRUE;
        }
    }
    return FALSE;
}

// Непосредственно текстовый вид переменной
BOOL    _GetVarA(void*var,PClassListInfo type,char*text)
{
    if (type->isINTEGER())
    {
        long d=*((long*)var);
        wsprintf(text,"%ld",d);
        return FALSE;
    }
    else
        if (type->isFLOAT())
        {
            double d=*((double*)var);
            sprintf(text,"%g",d);
            return FALSE;
        }
        else
            if (type->isSTRING())
            {
                lstrcpy(text,(char*)var);
                return FALSE;
            }
            else
            {
                if (type->isHANDLE())
                {
                    long d=*((long*)var);
                    wsprintf(text,"#%ld",d);
                    return FALSE;
                }
                else
                {
                    if (type->isRGB())
                    {
                        COLORREF c=*((COLORREF*)var);
                        RGBToStr(c,text);
                        return FALSE;
                    }
                }
            }
    return TRUE;
}

char* GetObjectVarA(PObject po,INT16 id,char* text)
{
    if (po){
        pointer var=po->GetVar(id);
        if (var){
            PClassListInfo type=po->_class->vars[id].classType;
            _GetVarA(var,type,text);
            return NULL;
            ///
        }
        //lstrcpyn(n);
    }
    lstrcpy(text,"");
    return NULL;
};

int InitCompiler();

BOOL _SetObjectVarA(PObject po, INT16 id, char*text)
{
    char s[255];
    if (po)
    {
        PClassListInfo type = po->_class->vars[id].classType;
        if (type)
        {
            if (type->isINTEGER())
            {
                long l = atol(text);
                SetObjectVar(po, id, &l);
            }
            else
            {
                if (type->isFLOAT())
                {
                    double value;
                    BOOL rez = StrToDouble(text, value);
                    if(!rez)
                    {
                        char *ps;
                        if((ps=strstr(text,",")) != NULL)
                        {
                            lstrcpy(s,text);
                            s[ps-text]='.';
                            rez = StrToDouble(s,value);
                        }
                        else
                        {
                            if(!compiler)
                                InitCompiler();

                            rez = compiler->GetConstant(text,value);
                        }
                    }
                    if(rez)
                    {
                        SetObjectVar(po, id, &value);
                        return TRUE;
                    }
                }
                else
                {
                    if (type->isSTRING())
                    {
                        SetObjectVar(po,id,text); return TRUE;
                    }
                    else
                    {
                        if (type->isHANDLE())
                        {
                            if (*text=='#')
                            {
                                long l=atol(text+1);
                                SetObjectVar(po,id,&l);
                            }
                            else
                                return FALSE;
                        }
                        else
                        {
                            if (type->isRGB())
                            {
                                COLORREF rgb;
                                if(StrToRGB(text,rgb,NULL))
                                {
                                    SetObjectVar(po,id,&rgb);
                                }
                                return TRUE;
                            }
                            else
                                return FALSE;
                        }
                    }
                }
            }
            return TRUE;
        }
    }
    return FALSE;
};

BOOL TObject::DeleteSetVar(char*varname)
{
    TSetVar*v=vardata;
    TSetVar*_v=NULL;
    while(v)
    {
        if(!lstrcmpi(v->name,varname))
        {
            if(vardata==v)
                vardata=v->next;
            else
                _v->next=v->next;
            v->next=NULL;
            delete v;
            return TRUE;
        }
        _v=v;
        v=v->next;
    }
    return FALSE;
};

BOOL SetObjectVarA(PObject po,INT16 id,char*text)
{
    if(_SetObjectVarA(po,id,text))
    {
        po->GetProject()->UpdateWatchWindow();
        if(SCGetScVar("vars_logset",1))
        {
            if((calcedsteps==0)|| (SCGetScVar("vars_1step",1)==0))
            {
                char *varname=po->_class->vars[id].name;

                TSetVar*v=po->vardata;
                while(v)
                {
                    if(!lstrcmpi(v->name,varname))
                    {
                        if(lstrlen(text))
                        {
                            if(v->data){delete v->data;v->data=NULL;}
                            v->data=NewStr(text);
                            if(project)project->VarChange();
                        }
                        else
                        {
                            po->DeleteSetVar(varname);
                            if(project)project->VarChange();
                        }
                        return TRUE;
                    }
                    v=v->next;
                }
                if(lstrlen(text))
                {
                    v=new TSetVar(varname,text);
                    v->next=po->vardata;
                    po->vardata=v;
                    if(project)project->VarChange();
                }
            }
        }
        return TRUE;
    }
    return FALSE;
};

BOOL TObject::IsSetExist()
{
    if(vardata)
        return TRUE;
    PObject po = childObjects;
    while (po)
    {
        if(po->IsSetExist())
            return TRUE;
        po=po->next;
    }
    return FALSE;
};

//перенос vardata в var
void TObject::SetSettingsVars()
{
    if(vardata)
    {
        TSetVar* _v = NULL;
        TSetVar* v = vardata;

        while(v)
        {
            INT16 id = GetVarIdByName(v->name);
            if((id > -1) && (v->data))
            {
                _SetObjectVarA(this, id, v->data);
                _v = v;
                v = v->next;
            }
            else
            {
                TSetVar* _v_ = v->next;
                if(_v)
                    _v->next = v->next;
                else
                    vardata = v->next;

                v->next=NULL;
                delete v;
                v = _v_;
            }
        }
    };

    PObject po=childObjects;
    while (po)
    {
        po->SetSettingsVars();
        po=po->next;
    }
}

BOOL SaveAllModifyClasses(BOOL save)
{
    BOOL _u=FALSE;
    for(C_TYPE i=0;i<classes->count;i++)
    {
        PClassListInfo pcli=(PClassListInfo)classes->At(i);
        if (pcli->flags&CLF_LOADED)
        {
            BOOL update=FALSE;
            PClass _class=pcli->GetClass();
            if(!_class->Protected())
            {
                if (_class->flags&CF_MODIFY)
                    update=TRUE;
                else
                {
                    if(_class->scheme && _class->scheme->Loaded()
                       && IsSpaceChanged2d(_class->scheme->GetSpace()))
                        update=TRUE;
                    else
                    {
                        if(_class->image && _class->image->Loaded()
                           && IsSpaceChanged2d(_class->image->GetSpace()))
                            update=TRUE;
                    }
                }
            }
            if (update)
            {
                _u|=TRUE;
                if(save)_class->UpdateClassFile();
            }
        }
    }
    return _u;
};

//----------------------- Class List ---------------------------------------//
BOOL TClassListInfo::GetFileName(char*_name,BOOL forsave)
{
    char * file=filename;
    char path[256];
    library->GetPath(path);
    if(flags&CLF_PACKFILE)
    {
        lstrcpy(_name,path);
        return TRUE;
    }

    if (!file && forsave)
    {
        char file[256];
        //#ifdef WIN32
        //Если WIN32 то создаем длинное имя файла (че то глючит потом)
        /*
        lstrcpyn(file,GetClassName(),64);
        int l=lstrlen(file);

        char _file[MAX_PATH];
        lstrcpy(_file,path);
        lstrcat(_file,file);
        lstrcat(_file,".cls");
        HFILE h=_lopen(_file,OF_READ|OF_SHARE_DENY_WRITE);
        if (h!=HFILE_ERROR)
    {
        _lclose(h);
    }
    else
    {
        lstrcat(file,".cls");
        goto m1;
        }
 */
        //#else
        lstrcpyn(file,GetClassName(),9);
        int l=lstrlen(file);
        //#endif
        {
            while (l<8)
            {
                lstrcat(file,"u");
                l=lstrlen(file);
            }
            char * ps=file;
            while (*ps)
            {
                char c=*ps;
                if (!((c>='0' && c<='9')
                      ||(c>='A' && c<='Z')||(c>='a' && c<='z')))
                    *ps='_';
                ps++;
            }
            lstrcat(file,".cls");
            _GetTempFileName(path,file,3,"_cl");
        }
        //m1:
        _class->t_flags|=TCF_WASSAVE;
        file[lstrlen(file)-4]=0;
        filename=NewStr(file);
        UpdateClassWindows(UW_MODIFY,UW_CLASS,this,library); // Отображаем новое имя
    };
    lstrcpy(_name,path);
    if(filename)
    {
        lstrcat(_name,filename);
        if ((flags&CLF_LOADED)&&(_class->t_flags&TCF_WASSAVE))
        {  //?????
            lstrcat(_name,"._cl");
        }
        else
            lstrcat(_name,".cls");
        return TRUE;
    }
    else
        return FALSE;
};

INT16 _abstract(){return 0;};


int EnumerateLibs(PLibrary parent,DWORD flags,int Action(PLibrary,void*),void* data)
{
    if (flags&ELF_NOHIERARCHCY && parent==NULL)
        return EnumerateLibs(librarys,flags,Action, data);

    if (parent)
    {
        for(int i=0;i<parent->items->count;i++)
        {
            PLibrary lib=(PLibrary)parent->items->At(i);
            if (lib->flags&CLF_LIBRARY)
            {
                if (!Action(lib,data))return 0;
                if (flags & ELF_NOHIERARCHCY)EnumerateLibs(lib,flags,Action,data);
            }
        }
        return 1;
    }
    return 0;
};

BOOL AddLink(PClass _class,PLinkInfo pl)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в AddLink(%s)",_class->name);
        LogMessage(s);
    }
#endif
#ifdef LITE
    if(_class->linkcount>40)
    {
        _Message("Ограничение на число связей в HOME версии");
        return FALSE;
    }
#endif
    if(_class->AddLink(pl))
    {
        if (_class->first && pl->count)
        {
            if(project)project->UpdateVarsLinks(TRUE);
            ObjectsChanged();
        }
        // only if it has exemplars
        return TRUE;
    }
    return FALSE;
};

BOOL RemoveLink(PClass _class,HOBJ2D id,BOOL /*fromscheme*/)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в RemoveLink(%s,%d)",_class->name,int(id));
        LogMessage(s);
    }
#endif
    INT16 count=0;
    INT16 num;
    PLinkInfo pl=_class->GetLinkById(id,&num);
    if (pl)count=pl->count;
    if(_class->RemoveLink(id))
    {
        if (_class->first && count)
        {
            if(project)project->UpdateVarsLinks(0);
            ObjectsChanged();
        }
        //only if it has exemplars
        return TRUE;
    }
    return FALSE;
};

BOOL ModifyLink(PClass _class,HOBJ2D id,PLinkInfo pl,BOOL onlyconnect){
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в ModifyLink(%s,%d)",_class->name,int(id));
        LogMessage(s);
    }
#endif

    if (onlyconnect || _class->ModifyLink(id,pl)){
        if (_class->first){TProject::UpdateLinks(_class);ObjectsChanged();}
        return TRUE;
    }
    return FALSE;
};
BOOL ChangeClassName(PClass _class,char *name){
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в ChangeClassName(%s,%s)",_class->name,name);
        LogMessage(s);
    }
#endif

    if (_class && name){
        PClassListInfo pcli=_class->GetClassListInfo();
        classes->Delete(pcli);
        delete (_class->name);
        _class->name=NewStr(name);
        classes->Insert(pcli);
        _class->SetModify();
        for(INT16 i=0;i<classes->count;i++){
            PClassListInfo pc=(PClassListInfo)classes->At(i);
            if (pc->flags&CLF_LOADED){
                PClass _c=pc->_class;
                for(INT16 j=0;j<_c->childcount;j++){
                    if (_c->childs[j].type->_class==_class){_c->SetModify();}
                } }
        }

        UpdateClassWindows(UW_MODIFY,UW_CLASS,pcli,pcli->library);
        if(_class->first){
            PObject po=_class->first;
            while(po){
                UpdateClassWindows(UW_MODIFY,UW_OBJECT,po,NULL);
                po=po->class_next;
            }
        }

        TProject*prj=projects;
        while(prj){
            if(prj->_class==_class)prj->SetModify();
            prj=prj->next;
        }
        // if (root && root->_class==_class)SaveProjectAs(projectfile,0);


        return TRUE;
    }
    return FALSE;
};

TCallBack* CBdoonestep = NULL;
BOOL DoOneStep()
{
    BOOL ret = FALSE;
    if(project)
    {
        //if(project->equations)project->equations->Regeneration();
        ret = project->DoOneStep();
    }
    winmanager->PostStep();

    TCallBack* c = CBdoonestep;
    while(c)
    {
        BOOL  (SDKPROC * proc)();
        (void*)proc = c->proc;
        proc();
        c = c->next;
    }
    return ret;
}


/*
    Var data
*/

C_TYPE TVarData::GetRealIndex(C_TYPE i)
{
    C_TYPE index = 0;
    for(C_TYPE j = 0; j < items->count; j++)
    {
        PVarInfo _pv = (PVarInfo)items->At(j);
        if (!(_pv->flags & VF_LOCAL))
        {
            if (index==i)
                return j;
            index++;
        }
    }
    return -1;
}

C_TYPE TVarData::GetNonLocalCount()
{
    C_TYPE index=0;
    for(C_TYPE i=0;i<items->count;i++)
    {
        PVarInfo _pv =(PVarInfo)items->At(i);
        if (!(_pv->flags&VF_LOCAL))
        {
            index++;
        }
    }
    return index;
}

TVarData::TVarData(TVarData&vardata)
{
    flags=0;
    _class=vardata._class;
    items=new TCollection(10,10);
    titles=NULL;

    for(INT16 i=0;i<vardata.items->count;i++)
    {
        PVarInfo pv=new TVarInfo();
        pv->Create((PVarInfo)vardata.items->At(i));
        items->Insert(pv);
    }
}

TVarData::TVarData(PClass __class)
{
    _class = __class;
    if (_class)
    {
        flags = 0;
        items = new TCollection(10,10);

        for(int i = 0; i < _class->varcount; i++)
        {
            PVarInfo pv = new TVarInfo();
            pv->Create(&(_class->vars[i]));
            items->Insert(pv);
        }
    }
    titles=NULL;
}

TVarData::~TVarData(){
    for(C_TYPE i=0;i<items->count;i++){
        PVarInfo _pv =(PVarInfo)items->At(i);
        _pv->Done();
        delete _pv;
    }
    if(titles){
        for(C_TYPE i=0;i<titles->count;i++){
            delete titles->At(i);
        }
        titles->DeleteAll();
        delete titles;
    }

    items->DeleteAll();
    DeleteCollection(items);
};

char* TVarData::GetTitle(int i)
{
    if(!titles)
    {
        titles=new TCollection(10,0);
        char s[256];
        int j=IDS_VARLABEL;
        while(LoadString(HResource,j,s,sizeof(s)))
        {
            titles->Insert(NewStr(s));
            j++;
        };
    }
    return (char*)titles->At(i);
}
PVarInfo TVarData::Search(char*name,INT16 *index)
{
    for(C_TYPE i=0; i < items->count; i++)
    {
        PVarInfo _pv =(PVarInfo)items->At(i);
        if (lstrcmpi(_pv->name,name)==0){if (index)*index=i; return _pv;}
    }
    return NULL;
};

INT16 TVarData::Insert(char*name, char*type, BOOL fix)
{
    C_TYPE index;
    if (lstrlen(name)<1)
    {
        return -1;
    }

    TVarInfo * tv=Search(name,&index);
    if (!tv)
    {
        PClassListInfo  pcli = type ? GetClassListInfo(type) :
                                      GetClassListInfo("FLOAT");
        if (pcli)
        {
            PVarInfo pv=new TVarInfo();

            pv->info = pv->def = NULL;
            pv->classType=pcli;
            pv->flags=VF_BYCOMPILER;
            pv->name = NewStr(name);

            if (!fix)
                pv->flags |= VF_NOTFIXED;

            index = items->Insert(pv);
            flags |= VDF_MODIFY | VDF_SUBMODIFY;
        }
        else
            return -1;
    }
    else
    {
        if (type && lstrcmp(tv->classType->GetClassName(),type))
            return -2;
    }
    return index;
};

BOOL TVarData::Delete(INT16 id)
{
    PVarInfo _pv =(PVarInfo)items->At(id);
    items->AtDelete(id);
    flags|=VDF_MODIFY|VDF_SUBMODIFY;
    _pv->Done();
    delete _pv;
    return TRUE;
}

BOOL _SetStr(char*&target,char*t){
    if ((target || t) && lstrcmp(target,t)){
        if (target){
            delete target;
            target=NULL;};
        if (t)target=NewStr(t);
        return TRUE;
    }
    return FALSE;
}

void TVarData::UpdateVarInfo()
{
    if (items->count==_class->varcount)
    {
        BOOL mdf=FALSE;
        for(int i=0;i<items->count;i++)
        {
            PVarInfo _pv =(PVarInfo)items->At(i);
            PVarInfo _pvc=&(_class->vars[i]);
            if ((_pv->classType==_pvc->classType)
                && (!lstrcmpi(_pv->name,_pvc->name)))
            {
                mdf|=_SetStr(_pvc->def,	_pv->def);
                mdf|=_SetStr(_pvc->info,_pv->info);
            }
            if(_pvc->flags!=_pv->flags)
            {
                _pvc->flags=_pv->flags;
                mdf|=TRUE;
            }
        }
        if (mdf)_class->SetModify();
    }
};

PVarInfo TVarData::CreateInfo(INT16 *count)
{
    *count = items->count;
    if (items->count)
    {
        PVarInfo pv;
        pv=new TVarInfo[items->count];
        for(int i=0;i<items->count;i++)
        {
            PVarInfo _pv =(PVarInfo)items->At(i);
            pv[i]=*_pv;
            pv[i].name=NewStr(_pv->name);
            pv[i].info=NewStr(_pv->info);
            pv[i].def=NewStr(_pv->def);
        }
        // if (count)*count=items->count;
        return pv;
    };
    return 0;
}
int TVarData::Undo(){
    INT16 j=0;
    for(INT16 i=0;i<items->count;i++){
        PVarInfo _pv =(PVarInfo)items->At(i);
        if (_pv->flags&VF_NOTFIXED){Delete(i);j++;}
    }
    return j;
};

int TVarData::DeleteCompilerVars()
{
    INT16 i=0, j=0;
    while (i < items->count)
    {
        // PVarInfo _pv =(PVarInfo)items->At(i);
        // if (_pv->flags&VF_BYCOMPILER){Delete(i);j++;}else i++;
        Delete(i);
        j++;
    }
    return j;
};

int TVarData::Fix()
{
    int j=0;
    for(int i=0;i<items->count;i++)
    {
        PVarInfo _pv =(PVarInfo)items->At(i);
        _pv->flags&=(~VF_NOTFIXED);
        j++;
    }
    return j;
};

// Далее для ModifyVar

INT16 ModifyVar(PClass _class,PVarData data)
{
    changedclass = _class;
    TProject* prj = projects;
    BOOL rez = TRUE;
    while(prj)
    {
        if((_class == NULL) || (prj->object->IsContainClass(_class)))
            rez&=projects->PreModifyVar(_class);
        prj=prj->next;
    }
    INT16 count;
    PVarInfo pv=data->CreateInfo(&count);
    _class->SetVars(pv,data->items->count);

    prj=projects;
    while(prj)
    {
        if((_class==NULL) || (prj->object->IsContainClass(_class)))
            rez &= projects->PostModifyVar(_class);
        prj=prj->next;
    }
    return rez;
    changedclass=NULL;
}
/*
INT16 ModifyVar(PClass _class,PVarData data)
{
  if(projects && (projects->next==NULL)){projects->ModifyVar(_class,data);return 1;}else{
 TProject*prj=projects;
 BOOL rez;
 while(prj){
   if((_class==NULL) || (prj->object->IsContainClass(_class))){projects->ModifyVar(_class,data);rez=TRUE;}
  prj=prj->next;
   }
   return rez;
  }
};
*/

BOOL Running()
{
    return (EXEC_FLAGS & EF_RUNNING)!=0;
}

void   DoneRun()
{};

/*
 Далее для записи и чтения состояний
*/
void __SaveAddC(PClass _class,TCollection&items)
{
    if (items.IndexOf(_class)>-1)return;

    items.Insert(_class);

    for(INT16 i=0;i<_class->childcount;i++)
        __SaveAddC(_class->childs[i].type->_class,items);
    for(i=0;i<_class->varcount;i++)
        __SaveAddC(_class->vars[i].classType->_class,items);
}

BOOL __SaveState(TStream &st,PObject po,BYTE flags){
    if (!po)return 0;
    st.WriteStr("SC Scheme Variables");
    st.WriteStr(po->_class->name);
    st.WriteWord(RR_VERSION);
    st.WriteWord(2);

    if(flags&1){
        TCollection items(100,100);
        __SaveAddC(po->_class,items);

        st.WriteWord(VR_CLASSES);
        st.WriteWord(items.count);

        for(int i=0;i<items.count;i++){
            PClass _class=(PClass)items.At(i);
            st.WriteStr(_class->name);
            st.WriteLong(_class->_classid);
            st.WriteWord(_class->varcount);
            st.WriteWord(_class->childcount);
            st.WriteWord((INT16)_class->varsize);

            for(INT16 j=0;j<_class->childcount;j++){
                st.WriteWord((UINT16)(_class->childs[j].handle));
                st.WriteWord(items.IndexOf(_class->childs[j].type->_class));
            }
            for(j=0;j<_class->varcount;j++){
                st.WriteStr(_class->vars[j].name);
                st.WriteWord(items.IndexOf(_class->vars[j].classType->_class));
            }
        };
        st.WriteWord(VR_VARS);
        po->StoreState(st,STATE_FAST);
    }
    if(flags&2){
        if(po->IsSetExist()){
            st.WriteWord(VR_SETVAR);
            po->SaveSetState(st);
        }
    }
    st.WriteWord(0);
    return 1;
};

BOOL SaveState(PObject po,char*name){
#ifdef LOGON
    {
        char s[256];
        char* ps="root class";
        if(po)ps=po->_class->name;

        wsprintf(s,"Зашел в SaveState(PObject of class:%s,file:%s)",ps,name);
        LogMessage(s);
    }
#endif



    TDOSStream st(name,TDOSStream::stCreate);
    if (!po){
        if(project){
            po=project->object;
        }else return FALSE;
    }
    int m=savestatemode;
    savestatemode=1;
    return __SaveState(st,po,m);
};

class TLClass;

struct TLVar
{
    char* name;
    TLClass* _lclass;
};

struct TLChild
{
    TLClass *_lclass;
    HOBJ2D handle;
};

class TLClass
{
public:
    TLClass(TStream& st);
    ~TLClass();

public:
    char* name;

    UINT32 _classid;
    TClass *_class;
    PClassListInfo classInfo;

    INT16 varcount, childcount, varsize;
    TLVar   * vars;

    TLChild * child;
};

TLClass::TLClass(TStream&st)
{
    classInfo=0;
    _classid=0;

    name = st.ReadStr();

    if(st.GetVersion() > 1)
        _classid = st.ReadLong();

    varcount=st.ReadWord();
    childcount = st.ReadWord();
    varsize = st.ReadWord();

    if(childcount)
    {
        child = new TLChild[childcount];
        for(int i = 0; i < childcount; i++)
        {
            child[i].handle = st.ReadWord();
            child[i]._lclass = (TLClass*)st.ReadWord();
        }
    }
    else
        child = NULL;

    if(varcount)
    {
        vars = new TLVar[varcount];
        for(int i=0;i<varcount;i++)
        {
            vars[i].name = st.ReadStr();
            vars[i]._lclass = (TLClass*)st.ReadWord();
        }
    }
    else
        vars=NULL;
}

TLClass::~TLClass()
{
    delete name;
    for(int i=0;i<varcount;i++)
        delete vars[i].name;

    if (vars)
        delete vars;

    if (child)
        delete child;
};

class TLObject
{
public:
    TLClass *_lclass;
    TLObject * next;
    TLObject * items;

    void** vars;

    TLObject(TLClass*_lc);
    ~TLObject();

    void Load(TStream&st);
    void SetVar(PObject po);
};

TLObject::TLObject(TLClass* _lc)
{
    memset(this,0,sizeof(TLObject));
    _lclass=_lc;

    if (_lc->varcount)
        vars = new pointer[_lc->varcount];

    //Задаем размер значения переменной в зависимости от типа
    for(int i = 0; i < _lc->varcount; i++)
        vars[i] = new char[_lc->vars[i]._lclass->varsize];

    //Создание иерархии объектов по иерархии классов
    TLObject* prev = NULL;
    for(i = 0; i < _lc->childcount; i++)
    {
        TLObject* obj = new TLObject(_lc->child[i]._lclass);
        if (prev)
            prev->next = obj;
        else
            items = obj;

        prev = obj;
    }
}

void TLObject::Load(TStream& st)
{
    for(int i = 0; i < _lclass->varcount; i++)
    {
        TLClass * lc = _lclass->vars[i]._lclass;
        if (lc->classInfo && (lc->classInfo->isSTRING()))
            st.ReadStr((char*)vars[i]);
        else
            st.Read(vars[i],lc->varsize);
    }

    TLObject* pl = items;
    while(pl)
    {
        pl->Load(st);
        pl=pl->next;
    }
}

TLObject::~TLObject()
{
    if(vars)
    {
        for(int i=0;i<_lclass->varcount;i++)
            delete vars[i];
        delete vars;
    }

    TLObject *pl=items;
    while(pl)
    {
        TLObject* pl1=pl->next;
        delete pl;
        pl=pl1;
    }
}

void TLObject::SetVar(PObject po)
{
    //(скорее всего) если нет не сохраненных переменных (все переменные сохранены)
    if(!(po->_class->flags & CF_NOSAVEVAR))
    {
        for(int i = 0; i < _lclass->varcount; i++)
        {
            for(int j = 0; j < po->_class->varcount; j++)
            {
                //(скорее всего) если переменная сохранена
                if(!(po->_class->vars[i].flags & VF_NOSAVE))
                {
                    if (!lstrcmpi(_lclass->vars[i].name, po->_class->vars[j].name))
                    {
                        if (_lclass->vars[i]._lclass->_class == po->_class->vars[j].classType->GetClass())
                        {
                            memcpy(po->vars[j], vars[i], (int)(po->_class->vars[j].classType->_class->varsize));
                        }
                        j = po->_class->varcount;
                    }
                }
            }
        }

        //установка переменных для дочерних TLObject
        int j=0;
        TLObject *pl = items;
        while(pl)
        {
            TObject* _po = po->_GetObject(_lclass->child[j].handle);

            if(_po && _po != po &&
               ((_po->_class==pl->_lclass->_class) ||
                ((pl->_lclass->_classid) && (_po->_class->_classid == pl->_lclass->_classid))))
                pl->SetVar(_po);

            pl = pl->next;
            j++;
        }
    }
}

bool __LoadState(TStream &st, PObject object, BYTE) // Загрузить состояние
{
    if(st.status)
        return false;

    if(!object)
    {
        if(project)
            object = project->object;
        else
            return false;
    }

    // Проверка файла на корректность контента
    char str[128];
    st.ReadStr(str, 100);
    if(lstrcmp("SC Scheme Variables", str))
    {
        _Error(EM_INVALIDSTATEFILE,0,str);
        return 0;
    }

    st.ReadStr(str, sizeof(str));

    //  if (lstrcmp(s,po->_class->name)){_Error(EM_NOTTHISCLASS,0,s);return 0; }
    TLObject* _lobj = NULL;
    TLClass* lClassRoot = NULL;
    TCollection items(100,100);

    int code = st.ReadWord();
    while (code)
    {
        switch(code)
        {
            case RR_VERSION:
                st.SetVersion(st.ReadWord());
                break;

            case VR_CLASSES:
            {
                //Инициализация псевдоклассов
                int count = st.ReadWord();
                for(int i = 0; i < count; i++)
                    items.Insert(new TLClass(st));

                for(i = 0; i < count; i++)
                {
                    TLClass* lClass = (TLClass*)items.At(i);
                    if (!lstrcmp(lClass->name, str))
                        lClassRoot = lClass;

                    lClass->classInfo = GetClassListInfo(lClass->name);
                    //lc->_class=GetClassByName(lc->name); //was
                    if(lClass->classInfo)
                        lClass->_class = lClass->classInfo->GetClass();
                    else
                        lClass->classInfo = NULL; // now

                    for(int j = 0; j < lClass->childcount; j++)
                        lClass->child[j]._lclass = (TLClass*)items.At((int)lClass->child[j]._lclass);

                    //Инициализация типов переменных (бред какой-то)
                    for(j = 0; j < lClass->varcount; j++)
                        lClass->vars[j]._lclass = (TLClass*)items.At((int)lClass->vars[j]._lclass);
                }
                //Создание псевдообъекта: выделение памяти для значений переменных
                _lobj = new TLObject(lClassRoot);

            }break;

            case VR_VARS:
            {
                _lobj->Load(st);
                // str == rootClassName
                if ((lstrcmp(str, object->_class->name) == 0) ||
                    (_lobj->_lclass->_classid && (object->_class->_classid == _lobj->_lclass->_classid)))
                {
                    _lobj->SetVar(object);
                    object->_CopyState(TObject::OLD_TO_NEW);
                }
                else
                {
                    _Error(EM_NOTTHISCLASS,0,str);
                }
                delete _lobj;

                for(int i = 0; i < items.count; i++)
                {
                    TLClass* lc = (TLClass*)items.At(i);
                    delete lc;
                }
            }break;

            case VR_SETVAR:
            {
                _LoadSetState(object, st);
            }break;
        }
        code=st.ReadWord();
    }

    TProject *prj = object->GetProject();
    if(prj)
        prj->UpdateWatchWindow();

    return true;
}

bool _LoadSetState(PObject po, TStream &st)
{
    long pos=st.ReadLong();
    st.ReadWord();

    char s[65];
    st.ReadStr(s, sizeof(s));

    UINT32 _classid=0;
    if(st.GetVersion()>1)
    {
        _classid = st.ReadLong();
    }

    if((!lstrcmpi(s,po->_class->name)) || (_classid && (_classid == po->_class->_classid)))
    {
        if(st.GetPos()<pos)
        {
            po->DoneSetVar();
            po->LoadSetState(st, pos);
        }
        st.Seek(pos);
        return TRUE;
    }
    return FALSE;
}

HOBJ2D GetObjectHandle(PObject po)
{
    if (po->parent)
    {
        int i=0;
        PObject _po=po->parent->childObjects;
        while (_po)
        {
            if (_po==po)
            {
                return po->parent->_class->childs[i].handle;
            }
            i++;
            _po=_po->next;
        }
    }
    return 0;
};

BOOL LoadState(PObject po,char* name)
{
#ifdef LOGON
    {
        char s[256];
        char* ps="root class";
        if(po)ps=po->_class->name;

        wsprintf(s,"Зашел в LoadState(PObject of class:%s,file:%s)",ps,name);
        LogMessage(s);
    }
#endif
    TDOSStream st(name,TDOSStream::stOpenRead);
    return __LoadState(st,po);
};

void TObject::GetFullFileName(char*src,char*dst)
{
    TProject * p=GetProject();
    p->GetFullFileName(src,dst);
    //lstrcpy(dst,p->path);
};

HOBJ2D CreateObjectFromFile(HSP2D hsp,char * file,gr_float x,gr_float y,UINT16 flags)
{

    HSP2D image = LdSpace2d(0,file);
    HOBJ2D group = MakeSingleGroup(image);

    long i;
    HGLOBAL h=CopyToMemory2d(image,group,&i);

    DeleteSpace2d(image);
    if (h)
    {
        POINT2D p;p.x=x;p.y=y;
        return PasteFromMemory2d(hsp,h,&p,flags);
    }
    return 0;
};

PObject TObject::GetReceiver(char*name, BOOL* all)
{
    if (!name)
        return this;

    PObject po;
    if (*name=='\\')
    {
        if(project)
            po = project->object;
        else
            return NULL;
        name++;
    }
    else
        po=this;

    if (all)
        *all = FALSE;

    while (*name && po)
    {
        if (*name=='*' && name[1]==0)
        {
            if (all)
                *all = TRUE;
            return po->childObjects;
        }

        if (*name == '.' && name[1] == '.')
        {
            po = po->parent;
            name += 2;
            if (*name == '\\')
                name++;
        }
        else
        {
            char *s = name;
            while(*name && *name!='\\')
                name++;

            //char c=*name;
            char findname[256];
            lstrcpyn(findname, s, (int)((name-s)+1));
            if(*name=='\\')
                name++;
            //*name=0;
            PObject _po;
            BOOL found=0;

            if(findname[0] == '#')
            {
                INT16 id = 0;
                char * np = findname+1;
                while(*np && (*np>='0') && (*np<='9'))
                {
                    id = id * 10 + ((*np)-'0');
                    np++;
                }
                INT16 num = po->_class->GetChildById(id);

                if(num > -1)
                {
                    found = TRUE;
                    _po = po->childObjects;
                    for(INT16 i = 0; i < num; i++)
                        _po = _po->next;

                    po = _po;
                }
            }
            else
            {
                _po = po->childObjects;

                for(INT16 i = 0; i < po->_class->childcount; i++)
                {
                    char * objname = po->_class->childs[i].name;
                    if (objname && (!lstrcmpi(objname, findname)))
                    {
                        po = _po;
                        found = TRUE;
                        break;
                    }
                    _po = _po->next;
                }
            }
            //*name=c;
            if (!found)
            {
                char mes[256];

                LoadString(HResource,IDS_str134,mes,sizeof(mes));
                lstrcat(mes,findname);

                if (po)
                {
                    LoadString(HResource,IDS_str135,mes+lstrlen(mes),100);
                    lstrcat(mes,po->_class->name);
                }
                //_Message(mes);
                return NULL;
            }
        }
    }
    return po;
}
// ----   AUTO SINTEZ --------------------//
//*----------------------------------------*//
HSP2D __GetClassScheme(PClass _class,BOOL & close){
    PClass _c;
    char * name=_class->name;
    close=0;
    close=(_class->scheme==NULL);
    HSP2D hsp=GetClassScheme(name,&_c);
    if (!hsp){	CreateDefClassScheme(name);
        hsp=GetClassScheme(name,&_class);
    }
    return hsp;
};

void __CloseClassScheme(PClass _class,int close)
{
    if (close)CloseClassScheme(_class);
};

#define __OPENSCHEME(__c_) int __close;\
    HSP2D hsp=__GetClassScheme(__c_,__close);\
    if (!hsp)return 0;

HOBJ2D Vm_AddChildren(char * parent,char * name,char * alias,gr_float x,gr_float y)
{
    PClass _parent=GetClassByName(parent);
    if (_parent)
    {
        PClass _class=GetClassByName(name);
        if (_class)
        {
            __OPENSCHEME(_parent);
            HOBJ2D id=0;
            AddChildren(_parent,name,&id,x,y);
            if (alias && lstrlen(alias))
            {
                INT16 num=_parent->GetChildById(id);
                if (num>-1)
                {
                    _parent->childs[num].name=NewStr(alias);
                    if(alias && alias[0])SetObjectName2d(hsp,id,alias);
                }
            }
            __CloseClassScheme(_parent,__close);
            return id;
        }
    }
    return 0;
};

BOOL Vm_RemoveChildren(char * parent,HOBJ2D id,BOOL noquescion,PObject po)
{
    PClass _parent=GetClassByName(parent);
    if (_parent && ((_parent->childcount)!=0))
    {
        if (po)
        {// Проверка на удаление самого себя
            PObject par=po->parent;
            while (par)
            {
                PObject _po=par->_GetObject(id);
                if (_po ==po)
                    return 0;
                par=par->parent;
            }
        }
        __OPENSCHEME(_parent);
        //int r=0;
        if (id==(HOBJ2D)-1)
        {
            while (_parent->childcount)
            {
                RemoveChildren(_parent,_parent->childs[0].handle,noquescion,FALSE);
                //r++;
            }
            TProject::UpdateLinks(_parent);
        }
        {
            RemoveChildren(_parent,id,noquescion);
        }
        __CloseClassScheme(_parent,__close);
        return TRUE;
    }
    return 0;
};

BOOL Vm_CreateClass(char *newclass,char *_template,UINT32 /*flags*/)
{
    if (IsNameValid(newclass))
    {
        if (_template==NULL || lstrlen(_template)==0)
            return CreateNewClass(newclass,NULL,NULL,DefaultLib)!=0;
        else
            //return CreateClassByClass(newclass,_template)!=0;
            return CreateClassByClass(_template,newclass)!=0;

    }
    return 0;
};

void Vm_DeleteClass(char * classname)
{
    DeleteClass(classname);
}

HOBJ2D Vm_CreateLink(char * parent,HOBJ2D obj2d,HOBJ2D object)
{
    PClass _class=GetClassByName(parent);
    if (_class)
    {
        HOBJ2D rez = 0;
        __OPENSCHEME(_class);
        //  TLinkInfo * li=new TLinkInfo();
        //  memset(li,0,sizeof(TLinkInfo));
        BOOL okey=0;
        if (obj2d && object && (obj2d!=object))
        {
            INT16 t1=_WhatType(_class,hsp,obj2d);
            INT16 t2=_WhatType(_class,hsp,object);
            if ((t1==1 || t1==2)&&(t2==1 || t2==2))
                okey=1;
        }
        if (okey)
        {
            PLinkInfo pl=new TLinkInfo;
            memset(pl,0,sizeof(TLinkInfo)); // добавил эту строку
            pl->varhandle=0;
            if (_class->GetChildById(object)>-1)
                pl->obj1Id=object;
            else
            {
                pl->obj1Id=0;
                pl->varhandle=object;
            }
            if (_class->GetChildById(obj2d)>-1)
                pl->obj2Id=obj2d;
            else
            {
                pl->obj2Id=0;
                pl->varhandle=obj2d;
            }
            POINT2D pp[2];
            POINT2D _pp,_pp1;
            GetObjectOrgSize2d(hsp,object,&(pp[0]),&_pp);
            GetObjectOrgSize2d(hsp,obj2d,&(pp[1]),&_pp1);
            pp[0].x+=_pp.x/2;
            pp[0].y+=_pp.y/2;
            pp[1].x+=_pp1.x/2;
            pp[1].y+=_pp1.y/2;
            HOBJ2D hpen=CreatePen2d(hsp,PS_SOLID,0 ,RGB(255,0,0), R2_COPYPEN);
            pl->handle=CreatePolyline2d(hsp,hpen,0 ,pp,2);

            AddLink(_class,pl);
            rez=pl->handle;
            delete pl;
        }
        __CloseClassScheme(_class,__close);
        return rez;
    }
    return 0;
};


BOOL Vm_SetLinkVars(char * parent,HOBJ2D id /*H*/,char *text){
    PClass _class=GetClassByName(parent);
    INT16 _n_=0;
    PLinkInfo _pl;
    if (_class && ((_pl=_class->GetLinkById(id,&_n_))!=0)){
        PLinkInfo pl=new TLinkInfo;
        TCollection items(10,10);
        char s[64];s[0]=0;
        INT16 i;
        while (*text){
            s[0]=0;
            i=0;
            while (*text && *text!='-'){
                s[i]=*text;i++;
                text++;
            }s[i]=0;
            if (*text)text++;
            items.Insert(NewStr(s));
            if (*text){
                s[0]=0;i=0;
                while (*text && *text!=','){
                    s[i]=*text;i++;
                    text++;
                }s[i]=0;
                if (*text)text++;
                items.Insert(NewStr(s));
            }
        }
        if (items.count>1 && ((items.count&1)==0)){
            *pl=*_pl;
            pl->count=(INT16)(items.count/2);
            pl->items=new TLinkEntryInfo[pl->count];
            memset(pl->items,0,sizeof(TLinkEntryInfo)*pl->count);
            for(INT16 i=0;i<items.count/2;i++){
                pl->items[i].name1=NewStr((char*)items.At(i*2));
                pl->items[i].name2=NewStr((char*)items.At(i*2+1));
            }
            ModifyLink(_class,id,pl);
        }else{
            pl->count=0;
            pl->items=0;
        }
        for(i=0;i<items.count;i++){
            delete (char*)(items.At(i));
        }
        items.DeleteAll();
        pl->Done();
        delete pl;
        return TRUE;
    }
    return FALSE;
};

BOOL Vm_RemoveLink(char * parent,HOBJ2D id1){
    PClass _class=GetClassByName(parent);
    if (_class){
        __OPENSCHEME(_class);
        BOOL r=RemoveLink(_class,id1);
        __CloseClassScheme(_class,__close);
        return r;
    }return 0;
};

HOBJ2D Vm_CreateVarPlace(char * parent,gr_float x,gr_float y){
    PClass _class=GetClassByName(parent);
    if (_class){
        __OPENSCHEME(_class);
        HOBJ2D r= CreateVarPlace(_class,hsp,x,y);
        __CloseClassScheme(_class,__close);
        return r;
    }return 0;
};

INT16 _WhatType(PClass _class, HSP2D hsp, HOBJ2D obj)
{
    //1 - object
    //2 - dib
    //3 - link
    //0 - polyline
    if (_class)
    {
        if(_class->GetChildById(obj) >= 0)
            return 1;

        if (GetObjectType2d(hsp, obj) == otDOUBLEBITMAP2D)
        {
            HOBJ2D hdib = GetDoubleDibObject2d(hsp,obj);

            char s[64];
            if(GetRDoubleDib2d(hsp,hdib,s,63))
                if (!lstrcmp(s,varpoint_dbm))
                    return 2;
        }

        INT16 n;
        if (_class->GetLinkById(obj,&n))
            return 3;
    }
    return 0;
}

BOOL GetUniqueClassName(char *n1,char *n2,BOOL copy){
    lstrcpy(n2,n1);
    GetUniqueClassName(n2,copy);
    return 0;
};

void ObjectsChanged()
{
    StratumProc(SC_OBJECTSCHANGED,0,0);
};

BOOL TClass::Protected()
{
    if(listinfo->flags&(CLF_READONLY|CLF_PACKFILE))
        return TRUE;
    if(!(listinfo->flags&CLF_PROJECT)&&(!(sysflags&2)))
    {
        return TRUE;
    }
    if (project && project->readonly)
        return TRUE;

    if (flags&CF_PROTECTED)
    {
        return ((sysflags&1)==0);
    }
    return 0;
};

BOOL SetClassText(PClass _class, char *txt, bool showErrorDialog)
{
    if(!_class)
        return FALSE;

    if(_class->Protected())
        return FALSE;

    // int t=_class->text==NULL;
    TVarData vd(_class);
    BOOL r= SetClassText(_class, &vd, txt);
    if (r)
    {
        _class->UpdateClassFile();
    }
    else if(showErrorDialog)
    {
        char *result = compiler->GetLastError();
        char s[50];
        LoadString(HResource,IDS_str236,s,sizeof(s));
        MessageBox(0, result, s, MB_OK | MB_TASKMODAL);
    }
    return r;
};

char* GetClassText(PClass _class)
{
    char * rez = NULL;
    BOOL t=_class->text==NULL;
    if (_class->text==NULL)
    {
        _class->UpdateItem(CR_TEXT);
    }
    if (_class->text)
        rez=NewStr(_class->text);
    if (t && _class->text)
    {
        delete _class->text;
        _class->text=0;
    }
    return rez;
};

char* GetClassInfo(PClass _class)
{
    char * rez = NULL;
    BOOL t=_class->description==NULL;
    if (_class->description==NULL)
    {
        _class->UpdateItem(CR_INFO);
    }
    if (_class->description)
        rez=NewStr(_class->description);
    if (t && _class->description)
    {
        delete _class->description;
        _class->description=0;
    }
    return rez;
};


HOBJ2D CreateVarPlace(PClass _class,HSP2D hsp,gr_float x,gr_float y){
    HOBJ2D hddib=0;
    int notfound=1;
    if (!_class)return 0;
    do{
        hddib=GetNextTool2d(hsp,DOUBLEDIB2D,hddib);
        if (hddib && (GetToolType2d(hsp,DOUBLEDIB2D,hddib)==ttREFTODOUBLEDIB2D)){
            char FileName[129];
            GetRDoubleDib2d(hsp,hddib,FileName,128);
            if (!lstrcmpi(FileName,varpoint_dbm))notfound=0;
        }}  while (hddib && notfound);
    if (!hddib) hddib=CreateRDoubleDib2d(hsp,varpoint_dbm);
    POINT2D SrcOrg;SrcOrg.x=0;SrcOrg.y=0;
    POINT2D SrcSize;SrcSize.x=32;SrcSize.y=32;
    POINT2D DstOrg;DstOrg.x=x;DstOrg.y=y;
    HOBJ2D place=CreateDoubleBitmap2d(hsp,hddib,&SrcOrg,&SrcSize,&DstOrg,&SrcSize);
    _class->SetModify();
    return place;
};


int GetEQTreeInfo(HWND tree,DWORD data,int DataType)
{
    if (project && project->equations) project->equations->GetEQTreeInfo(tree,data,DataType);
    return 0;
};


PObject CreateObjectByName(char*name)
{
    PClass _class=GetClassByName(name);
    if (_class)
        return CreateObjectByClass(_class);
    else
    {
        _Error(EM_CLASSNOTFOUND,0,name);
        return NULL;
    }
}

PObject CreateObjectByClass(PClass cls)
{
    if (cls==NULL)
        return NULL;

    PObject object = new TObject();

    object->_class = cls; // устанвливаем ссылку на класс объекта
    PObject firstObject = cls->first;

    cls->first = object ;   // устанвливаем ссылку на первый объект(т.е. на PO)
    object->class_next = firstObject; // устанвливаем ссылку на следующий объект

    // Создаем чилдренов
    if (cls->childcount)
    {
        firstObject = NULL;
        for(int i=0; i < cls->childcount; i++)
        {
            PObject childObject;
            // char s[65];
            // lstrcpy(s,_class->child[i].type->GetClassName());
            PClass childClass = cls->childs[i].type->GetClass();

            //проверка на рекурсию в парентах
            {
                PObject __p = object;
                while(__p)
                {
                    if(__p->_class == childClass)
                    {
                        _Error(EM_RECURSE,0,cls->name);
                        cls->childs[i].type = GetClassListInfo("Empty_Class");
                        childClass=cls->childs[i].type->GetClass();
                        __p = NULL;
                    }
                    else
                        __p = __p->parent;
                }
            }

            childObject = CreateObjectByClass(childClass);
            childObject->parent = object;

            if (!childObject)
            {
                object->flags |= OF_ERROR;
                return object;
            }
            //	 p->id=_class->child[i].handle;
            if (firstObject)
            {
                firstObject->next=childObject;
                firstObject=childObject;
            }
            else
            {
                object->childObjects = childObject;
                firstObject = childObject;
            }
        }
    }
    if (cls->varcount)
    {
        for(int i=0; i < cls->varcount; i++)
        {
            //Загрузка классов переменных (float, handle, ...), если еще не были загружены
            if (!(cls->vars[i].classType->flags & CLF_LOADED))
            {
                char s[65];
                lstrcpy(s, cls->vars[i].classType->GetClassName());
                GetClassByName(s, 10);
            }
        }
    }
    return object;
}

double TObject::GetVarF(char*VarName,double def)
{
    int id= GetVarIdByName(VarName,TRUE);
    if (id>-1)
    {
        if(_class->vars[id].classType->isFLOAT())
            return ((double*)(vars[id]))[1];

        else
            _Message("var type mismatch");
    }
    return def;
}

UINT32 TObject::GetVarH(char*VarName,UINT32 def)
{
    int id = GetVarIdByName(VarName,TRUE);
    if (id>-1)
    {
        if(_class->vars[id].classType->GetClass()->varsize==4)
            return ((long*)(vars[id]))[1];
        else
            _Message("var type mismatch");
    }
    return  def;
}

char *  TObject::GetVarS(char*VarName,char* def)
{
    int id= GetVarIdByName(VarName,TRUE);
    if (id>-1)
    {
        if( _class->vars[id].classType->isSTRING())
            def = ((char*)(vars[id]))+256;
        else
            _Message("var type mismatch");
    }
    return def;
}

void TObject::SetVarF(char*VarName, double d)
{
    int id = GetVarIdByName(VarName,TRUE);
    if(id>-1)
    {
        if(_class->vars[id].classType->isFLOAT())
        {
            ((double*)(vars[id]))[1]=d;
            if(EXEC_FLAGS&EF_RUNNING)((double*)(vars[id]))[0]=d;
        }
        else
        {
            char s[256];
            wsprintf(s,"Вызов функции SerVar: Несовпадение типа устанавливаемой переменной в классе - %c, имя переменной - %c", _class->name, _class->vars[id].name);
            _Message(s);
        }
    }
}

void TObject::SetVarH(char*VarName,UINT32 h)
{
    int id= GetVarIdByName(VarName,TRUE);
    if (id>-1)
    {
        if( _class->vars[id].classType->GetClass()->varsize==4)
        {
            ((long*)(vars[id]))[1]=h;
            if(EXEC_FLAGS&EF_RUNNING)
                ((long*)(vars[id]))[0]=h;
        }
        else
        {
            char s[256];
            wsprintf(s,"Вызов функции SerVar: Несовпадение типа устанавливаемой переменной в классе - %c, имя переменной - %c", _class->name, _class->vars[id].name);	    _Message(s);
        }
    }
}

void TObject::SetVarS(char*VarName,char* s)
{
    int id= GetVarIdByName(VarName,TRUE);
    if (id>-1)
    {
        if(_class->vars[id].classType->isSTRING())
        {
            lstrcpy(((char*)(vars[id]))+256,s);
            if(EXEC_FLAGS&EF_RUNNING)
                lstrcpy(((char*)(vars[id])),s);
        }
        else
        {
            char s[256];
            wsprintf(s,"Вызов функции SerVar: Несовпадение типа устанавливаемой переменной в классе - %c, имя переменной - %c", _class->name, _class->vars[id].name);	    _Message(s);
            _Message(s);
        }
    }
};

TObject::TObject()
{
    memset(this,0,sizeof(TObject));
}

char* default_nameH[] = {"_HObject"};
char* default_nameS[] = {"_CLASSNAME", "_OBJNAME"};
char* default_nameF[] = {"OrgX", "OrgY"};

void TObject::ClearAndDef()
{
    ClearVars();
    SetDefault();
    _CopyState(TObject::OLD_TO_NEW);
}

void TObject::ClearVars()
{
    //hSpace=0;
    for(INT16 i=0;i<_class->varcount;i++)
    {
        INT16 size=_class->vars[i].classType->_class->varsize;
        memset(vars[i], 0, size*2);
    }
    PObject po=childObjects;
    while(po)
    {
        po->ClearVars();
        po=po->next;
    }
}

//
void TObject::SetDefault()
{
    PObject po = childObjects;
    while(po)
    {
        po->SetDefault();
        po=po->next;
    }

    for(INT16 i = 0; i < _class->varcount; i++)
    {
        char* varName = _class->vars[i].name;
        if (_class->vars[i].classType->isFLOAT())
        {
            for(INT16 j = 0; j < sizeof(default_nameF) / sizeof(char*); j++)
            {
                if (!lstrcmpi(varName, default_nameF[j]))
                {
                    switch(j)
                    {
                        case 0:
                            if (parent)
                            {
                                INT16 num=-1;
                                if (GetHandle(&num))
                                {
                                    *((double*)(vars[i])) = parent->_class->childs[num].x;
                                }
                            }
                            break;

                        case 1:
                        {
                            if (parent)
                            {
                                INT16 num=-1;
                                if (GetHandle(&num))
                                    *((double*)(vars[i]))=parent->_class->childs[num].y;
                            }
                        }
                            break;
                    }
                    break; // for break LOOP
                }
            }// FLOAT variables
        }
        else
        {//1
            if (_class->vars[i].classType->isSTRING())
            {
                //for(INT16 j=0;j<sizeof(default_nameF)/sizeof(char*);j++)
                for(INT16 j=0;j<sizeof(default_nameS)/sizeof(char*);j++)
                {
                    if (!lstrcmpi(varName,default_nameS[j]))
                    {
                        switch(j)
                        {
                            case 0:
                            {
                                lstrcpyn((char*)(vars[i]),_class->name,255);
                            }break;

                            case 1:
                            {
                                if (parent)
                                {
                                    INT16 num=-1;
                                    if (GetHandle(&num))
                                        lstrcpyn((char*)(vars[i]),parent->_class->childs[num].name,255);
                                }
                            }break;
                        }
                        break;// for break LOOP
                    }
                }
            } // STRING variables
            else
            { // 2
                if (_class->vars[i].classType->isHANDLE())
                {
                    for(INT16 j=0;j<sizeof(default_nameH)/sizeof(char*);j++)
                    {
                        if (!lstrcmpi(varName,default_nameH[j]))
                        {
                            switch(j)
                            {
                                case 0:
                                {
                                    HOBJ2D _h;
                                    INT16 num=-1;
                                    if  ((_h=GetHandle(&num))!=0)
                                        *((long*)(vars[i]))=_h;
                                }break;
                            }break;
                        }
                    }
                }  // loop
            }// 2
        } // 1

        if (_class->vars[i].def)
            _SetObjectVarA(this, i, _class->vars[i].def);
    }
}

PObject TObject::_GetObject(HOBJ2D id)
{
    if (id)
    {
        PObject object = childObjects;
        if (_class->childs)
        {
            int j = -1;
            for(int i = 0; i < _class->childcount; i++)
                if (_class->childs[i].handle == id)
                {
                    j = i;
                    break;
                }
            i=0;

            if (object)
                    while (object)
                {
                    if (i==j)
                        return object;

                    object = object->next;
                    i++;
                }
        }
        return NULL;
    }
    return this;
}

void TObject::AllEnable()
{
    flags&=(~OF_DISABLED);
    PObject po=childObjects;
    while (po){
        po->AllEnable();
        po=po->next;
    }
}

BOOL TObject::IsContainClass(PClass _c)
{
    if(_class==_c)return TRUE;
    PObject po=childObjects;
    while (po)
    {
        if(po->IsContainClass(_c))return TRUE;
        po=po->next;
    }
    return FALSE;
};

void TObject::CopyState(COPY_MODE mode)
{
    if (flags & OF_DISABLED && _class->flags & CF_DISABLED)
        return;

    PObject po = childObjects;
    while (po)
    {
        po->CopyState(mode);
        po = po->next;
    }

    for(int i = 0; i < _class->varcount; i++)
    {
        int size = (int)_class->vars[i].classType->_class->varsize;

        if(mode == NEW_TO_OLD)
            memcpy(vars[i], ((char*)vars[i]) + size, size); //NEW_TO_OLD
        else
            memcpy(((char*)vars[i])+size, vars[i], size); //OLD_TO_NEW
    }
}

//PObject lastcopy;
void TObject::_CopyState(COPY_MODE mode)
{
    // lastcopy=this;
    PObject po=childObjects;
    while (po)
    {
        po->_CopyState(mode);
        po = po->next;
    }

    for(int i = 0; i < _class->varcount; i++)
    {
        int size = (int)_class->vars[i].classType->_class->varsize;
        if(mode == NEW_TO_OLD)
            memcpy(vars[i], ((char*)vars[i])+size, size); //NEW_TO_OLD
        else
            memcpy(((char*)vars[i]) + size, vars[i], size); //OLD_TO_NEW
    }
}

TObject::~TObject()
{
    // Считается, что он уже вынут из списка объектов
    if(vardata)
        delete vardata;

    _class->RemoveObjectFromList(this);
    while (childObjects)
    {
        PObject po=childObjects;
        childObjects=childObjects->next;
        delete po;
    }// Грохнули детей
}
/*
pointer NewVar(PVarInfo pv){
// return new char[pv->type->_class->size];
 //new char[pv->name->size];
} */

void TObject::LoadState(TStream& st, INT16 mode)
{
    int i;
    switch(mode)
    {
        case STATE_VARS:
        {
            if (_class==changedclass)
            {
                while( st.ReadWord()==10)
                {
                    char varname[128];
                    st.ReadStr(varname);

                    PClassListInfo type = (PClassListInfo)st.ReadLong();
                    INT16 id = GetObjectVarId(_class, varname);

                    if (id>-1&&_class->vars[id].classType == type)
                    {
                        pointer var=GetVar(id);
                        st.Read(var, (int)_class->vars[id].classType->_class->varsize);
                    }
                    else
                    {
                        st.Seek(st.GetPos()+type->_class->varsize);
                    }
                }break;
            }//иначе чтение по STATE_FAST
        }
        case STATE_FAST:
        {
            if (EXEC_FLAGS & EF_STEPACTIVE)
            {
                for(i = 0; i < _class->varcount; i++)
                {
                    st.Read(vars[i], 2*((int)_class->vars[i].classType->_class->varsize));
                }
            }
            else
            {
                for(i = 0; i < _class->varcount; i++)
                {
                    if(_class->vars[i].classType->isSTRING())
                    {
                        st.ReadStr((char*)(vars[i]));
                    }
                    else
                        st.Read(vars[i],(int)_class->vars[i].classType->_class->varsize);
                }
            }break;
        }
    }
    PObject po=childObjects;
    //int j=0;
    while(po)
    {
        po->LoadState(st,mode);
        po=po->next;
    };
};

INT32 TObject::GetChildCount()
{
    INT32 count=1;
    PObject po=childObjects;
    // int j=0;
    while(po)
    {
        count+=po->GetChildCount();
        po=po->next;
    }
    return count;
};

void TObject::StoreState(TStream&st,INT16 mode)
{
    int i;
    switch(mode)
    {
        case STATE_VARS:
            if (_class==changedclass)
            {
                for(i=0;i<_class->varcount;i++)
                {
                    st.WriteWord(10);
                    st.WriteStr(_class->vars[i].name);
                    st.WriteLong((long)_class->vars[i].classType);
                    st.Write(vars[i],(int)_class->vars[i].classType->_class->varsize);
                }
                st.WriteWord(0);
                break;
            }//иначе запись по STATE_FAST
        case STATE_FAST:
            if (EXEC_FLAGS&EF_STEPACTIVE)
            {
                for(i=0;i<_class->varcount;i++)
                {
                    st.Write(vars[i],2*((int)_class->vars[i].classType->_class->varsize));
                }
            }
            else
            {
                for(i=0;i<_class->varcount;i++)
                {
                    if (_class->vars[i].classType->isSTRING())
                    {
                        st.WriteStr((char*)(vars[i]));
                    }
                    else
                        st.Write(vars[i],(int)_class->vars[i].classType->_class->varsize);
                }
            }break;
    }
    PObject po=childObjects;
    // int j=0;
    while(po)
    {
        po->StoreState(st,mode);
        po=po->next;
    }
};

void TObject::DoneSetVar()
{
    if(vardata)
    {
        delete vardata;
        vardata=NULL;
    }

    PObject po = childObjects;
    while(po)
    {
        po->DoneSetVar();
        po = po->next;
    }
}

void TObject::SaveSetState(TStream &st)
{
    if (IsSetExist()){
        long pos=st.GetPos();
        st.WriteLong(0);
        HOBJ2D handle=GetHandle(NULL);
        st.WriteWord(handle);
        st.WriteStr(_class->name);
        st.WriteLong(_class->_classid);
        if(vardata){
            TSetVar*v=vardata;
            INT16 i=0;
            while(v){v=v->next;i++;}
            st.WriteWord(i);
            v=vardata;
            while(v){
                st.WriteStr(v->name);
                st.WriteStr(v->data);
                v=v->next;
            }

        }else st.WriteWord(0);
        PObject po=childObjects;
        // int j=0;
        while(po){
            po->SaveSetState(st);
            po=po->next;
        }
        long p1=st.GetPos();
        st.Seek(pos);
        st.WriteLong(p1);
        st.Seek(p1);
    }
}

bool TObject::LoadSetState(TStream &st,long pos)
{
    INT16 count = st.ReadWord();
    if(count)
    {
        for(INT16 i = 0; i < count; i++)
        {
            TSetVar* var = new TSetVar(st);
            var->next = varData;
            varData = var;
        }
    }

    while((st.GetPos() < pos) && (st.status == 0))
    {
        long _pos = st.ReadLong();
        HOBJ2D handle = st.ReadWord();

        char type[65];
        st.ReadStr(type, sizeof(type));

        UINT32 _classid = 0;
        if(st.GetVersion() > 1)
            _classid = st.ReadLong();

        PObject po = _GetObject(handle);
        if(po && lstrcmpi(po->_class->name, type) == 0 ||
           (_classid && (_classid == po->_class->_classid)))
        {
            po->LoadSetState(st, _pos);
        }
        st.Seek(_pos);
    }
    return TRUE;
}

void SetNextSaveStateMode(int m)
{
    savestatemode = m;
}

BOOL SaveSetState(PObject po,char*name)
{
    if(!po)
    {
        GetRootObject(NULL,&po);
    }

    char __s[256];
    if(!name)
    {
        PProject prj = po->GetProject();
        prj->GetPrloadFile(__s);
        name = __s;
        prj->VarChange(FALSE);
    }
    if(po)
    {
        TDOSStream st(name,TDOSStream::stCreate);
        if(st.status==0){
            __SaveState(st,po,2);
            return TRUE;}
    }
    return FALSE;
};

bool LoadSetState(PObject po, char* sttFileName)
{
    if(!po)
        GetRootObject(NULL, &po);

    char __s[256];
    if(!sttFileName)
    {
        po->GetProject()->GetPrloadFile(__s);
        sttFileName = __s;
    }

    if(po)
    {
        TDOSStream st(sttFileName, TDOSStream::stOpenRead|TDOSStream::stNoWarning);
        if(st.status==0)
        {
            __LoadState(st, po);
            return TRUE;
        }
    }

    return FALSE;
}

#define DEBUG_LINKS

void TObject::UpdateLinks(TMemoryManager& m, TMemoryManager& mv)
{
    if (_class->linkcount)
    {
        struct _LIU
        {
            pointer var;
#ifdef DEBUG_LINKS
            PVarInfo pv;
#endif
            _LIU* next;
        };

        struct _LIF
        {
            _LIU* childObjects;
            _LIF* next;
            long size;
        };

        _LIF* first=NULL;
        long size;
        for(int i=0; i < _class->linkcount; i++)
        {
            PLinkInfo link = &(_class->link[i]);

            if(!(link->flags&LF_DISABLED))// in future
            {
                PObject po1 = _GetObject(link->obj1Id);
                PObject po2 = _GetObject(link->obj2Id);

                if (po1 && po2)
                    for(int j = 0; j < link->count; j++)
                    {
                        PVarInfo _pv1,_pv2;
                        pointer v1 = po1->_GetVar(link->items[j].name1, &size,&_pv1);
                        pointer v2 = po2->_GetVar(link->items[j].name2, 0, &_pv2);

                        if (v1 && v2)
                        {
                            if (_pv1->classType != _pv2->classType)
                            {
                                char *s= new char[256];
                                lstrcpy(s,_pv1->classType->GetClassName());
                                lstrcat(s," and ");
                                lstrcat(s,_pv2->classType->GetClassName());
                                _Error(EM_LINKTYPEMISMATCH,0,s);
                                ulinkerror|=2;
                                delete s;
                            }
                            _LIF*c;
                            _LIF**_c=&first;

                            while(*_c)
                            {
                                c=*_c;
                                if(c->size==size)
                                {
                                    _LIU*r=c->items;
                                    BYTE f=0;
                                    while(r)
                                    {
                                        if (r->var==v1){f=1;break;}
                                        if (r->var==v2){f=2;break;}
                                        r=r->next;
                                    }
                                    if (f)
                                    {
                                        while(r->next)r=r->next;
                                        // Глюки первой версии
                                        _LIF*_z=c;
                                        _LIF*z=c->next;
                                        while(z)
                                        {
                                            BYTE rm=0;
                                            if(z->size==size)
                                            {
                                                _LIU*a=z->items;
                                                _LIU*_r_=NULL;
                                                while(a)
                                                {
                                                    if (a->var==v1){_r_=a;break;}
                                                    if (a->var==v2){_r_=a;break;}
                                                    a=a->next;
                                                }
                                                if(_r_)
                                                {
                                                    rm=1;
                                                    r->next=z->items;
                                                    a=r;
                                                    r=r->next;
                                                    while(r){
                                                        if(r==_r_)a->next=r->next;else a=r;
                                                        r=r->next;
                                                    }
                                                    r=a;
                                                }
                                            }
                                            if(rm)_z->next=z->next;else  _z=z;
                                            z=z->next;
                                        }
                                        // Глюки первой версии
                                        r->next=(_LIU*)m.GetMem(sizeof(_LIU));
                                        r=r->next;
                                        r->next=NULL;
                                        r->var=(f==2)?v1:v2;
#ifdef DEBUG_LINKS
                                        r->pv=(f==2)?_pv1:_pv2;
#endif

                                        break;
                                    }
                                }		  //
                                _c = &(c->next);
                            }
                            if (!*_c)
                            {
                                *_c=(_LIF*)m.GetMemZ(sizeof(_LIF));
                                c=*_c;
                                c->size=size;
                                c->items=(_LIU*)m.GetMem(sizeof(_LIU));
                                c->items->var=v1;
#ifdef DEBUG_LINKS
                                c->items->pv=_pv1;
#endif

                                //			c->items->next=c->items+sizeof(_LIU);
                                c->items->next=(_LIU*)m.GetMem(sizeof(_LIU));

                                _LIU*r;
                                r=c->items->next;
                                r->next=0;
                                r->var=v2;
#ifdef DEBUG_LINKS
                                r->pv=_pv2;
#endif
                            }
                        }
                        else
                        {
                            char *s   = new char[256];
                            char *cmd = new char[256];
                            LoadString(HResource,IDS_str136,cmd,256);
                            wsprintf(s,cmd,_class->name);
                            LoadString(HResource,IDS_str137,cmd,256);
                            if (!v1){wsprintf(s+lstrlen(s),cmd,po1->_class->name,link->items[j].name1);}
                            if (!v2){wsprintf(s+lstrlen(s),cmd,po2->_class->name,link->items[j].name2);}
                            ulinkerror|=4;
                            _Message(s);
                            delete s;
                            delete cmd;
                        }
                    }
                else
                {
                    ulinkerror|=1;
                    _Error(EM_LINKNULLOBJECT);
                }
            }
        }

        _LIF* lf = first;
        _LIU* lu;
        while(lf)
        {
            pointer var=NULL;
            lu=lf->items;
            while(lu)
            {
                if (*((void**)lu->var))
                {
                    var = (*((void**)lu->var));//Ищем если уже выделенна существовала
                    break;
                }
                lu = lu->next;
            }

            if (!var)
                var = mv.GetMemZ((INT16)(lf->size*2));

            lu = lf->items;
            while(lu)
            {
                if (!(*((void**)lu->var)))
                {
                    *((long*)lu->var) = (long)var;
                }
                lu=lu->next;
            }
            lf=lf->next;
        }
        m.Release(0);
    }

    for(INT16 i = 0; i < _class->varcount; i++)
    {
        if(!vars[i])
            vars[i] = mv.GetMemZ((INT16)(2*GetVarSize(i)));
    }

    PObject po = childObjects;
    while(po)
    {
        po->UpdateLinks(m,mv);
        po = po->next;
    }
    //
}

BOOL TObject::IsDisable(BOOL chkparent)
{
    if (_class->flags&CF_DISABLED)
    {
        double *dv = (double *)vars[_class->disablevar];
        BOOL d;
        if(_class->flags&CF_UNDISABLED)d=!(dv[1]>0); else d=dv[1]>0;
        if(d)
            return TRUE;
    }
    if(chkparent && parent)return parent->IsDisable(chkparent);
    return FALSE;
};

void TObject::Compute()
{
    if (_class->flags & CF_HAS_DISABLE_VAR)
    {
        double *dv = (double *)vars[_class->disablevar];

        BOOL disabled;
        if(_class->flags & CF_HAS_ENABLE_VAR)
            disabled = !(dv[1] > 0);
        else
            disabled = dv[1] > 0;

        if(disabled)
        {
            flags |= OF_DISABLED;
            return;
        }
        else
            flags &= (~OF_DISABLED);
    }

    // Вычисления детей
    _Compute();
}

void TObject::_Compute()
{
    PObject object = childObjects;
    while (object)
    {
#ifdef  TESTING
        if (object->parent!=this){
            _Error(EM_INTERNAL);
            object->parent=this;
        }
#endif
        object->Compute();
        object=object->next;
    }
    _class->Compute(this);
}

pointer TObject::GetVar(INT16 id)
{
    if (_class->varcount > id && id > -1)
    {
        return vars[id];
    }
    return NULL;
}

long TObject::GetVarSize(INT16 id)
{
    if (_class->varcount > id && id > -1)
    {
        return _class->vars[id].classType->_class->varsize;
    }
    return NULL;
}

INT16 TClass::GetVarIdByName(char*n, BOOL warning)
{
    for(INT16 i = 0; i < varcount; i++)
    {
        if(!lstrcmpi(vars[i].name,n))
            return i;
    }
    if(warning)
    {
        {
            char s[256],cmd[128];
            LoadString(HResource,IDS_str138,cmd,sizeof(cmd));
            wsprintf(s,cmd,n);
            _Message(s);
        }
    }
    return (INT16)-1;
}

INT16 TObject::GetVarIdByName(char*n,BOOL warning)
{
    return _class->GetVarIdByName(n,warning);
};

pointer TObject::_GetVar(char*n,long*size,PVarInfo * pv){
    for(INT16 i=0;i<_class->varcount;i++){
        if(!lstrcmpi(_class->vars[i].name,n)){
            if (size)*size=_class->vars[i].classType->_class->varsize;
            if(pv)(*pv)=&(_class->vars[i]);
            return &(vars[i]);
        }
    }
    return NULL;
};
/*
  Class Class Class Class Class Class Class Class Class Class Class Class
*/
void TClass::Minimize()
{
    if (scheme && (scheme->window==0))
    {
        delete scheme;
        scheme=NULL;
    }
    if (icon)
    {
        delete icon;
        icon=NULL;
    }
    if (image && (image->window==0))
    {
        delete image;
        image=NULL;
    }
    if (text)
    {
        delete text;
        text = NULL;
    }
    if (description)
    {
        delete description;
        description=NULL;
    }
}
TClass::~TClass(){
    if (scheme)delete scheme;
    // if (icon)delete icon;
    if (text)delete text;
    if (image)delete image;
    if (IconName)delete IconName;
    if (icon)delete icon;
    if (VM_code)delete VM_code;
    if (description)delete description;
    if (equ)delete equ;
    if (moveinfo)delete moveinfo;
    if(helpfile)delete helpfile;
    scheme=image=0;

    // if (link)delete link; //
    // if (staticvars)delete staticvars;

    DeleteVars();
    DeleteChilds();
    DeleteLinks();
    if (name)delete name;
}

void TClass::DeleteVars()
{
    if (varcount){
        for(int i=0;i<varcount;i++)DeleteVar(&(vars[i]));
        delete vars;
        vars=NULL;
        varcount=0;}
};
PLinkInfo TClass::GetLinkById(HOBJ2D id,INT16*number){
    for(INT16 i=0;i<linkcount;i++){
        if (link[i].handle==id){
            if (number)*number=i;
            return &(link[i]);
        }
    }
    //  _Error(EM_BADLINKHANDLE,id);
    return NULL;
};

BOOL  TClass::AddLink(PLinkInfo _link)
{
    PLinkInfo _links=new TLinkInfo[linkcount+1];
    for(INT16 i=0;i<linkcount;i++)
        _links[i]=link[i];
    _links[linkcount].Create(_link);
    if (link) delete link;
    link=_links;
    linkcount++;
    SetModify();
    return TRUE;
};

BOOL TClass::ModifyLink(HOBJ2D id,PLinkInfo _link)
{
    INT16 num;
    PLinkInfo pl=GetLinkById(id,&num);
    if (pl && _link)
    {
        DeleteLink(pl);
        link[num].Create(_link);
        SetModify();
        return TRUE;
    }
    return FALSE;
};

BOOL  TClass::RemoveLink(HOBJ2D id,BOOL fromscheme)
{
    INT16 num;
    PLinkInfo pl=GetLinkById(id,&num);
    if (pl)
    {
        if(linkcount==1)DeleteLinks();
        else
        {
            DeleteLink(pl);
            PLinkInfo _links=new TLinkInfo[linkcount-1];
            INT16 j=0;
            for(INT16 i=0;i<linkcount;i++)
            {
                if (i!=num){_links[j]=link[i];j++;}
            }
            delete link;
            link=_links;
            linkcount--;
        }
        if (scheme && fromscheme)
        {
            HSP2D hsp=scheme->GetSpace();
            HOBJ2D pen=GetPenObject2d(hsp,id);
            DeleteObject2d(hsp,id);
            DeleteTool2d(hsp,PEN2D,pen);
        }
        SetModify();
        return TRUE;
    }
    return FALSE;
};// По дескриптору полилинии

void TVarInfo::Done(){
    if (name){delete name;name=NULL;}
    if (info){delete info;info=NULL;}
    if (def){delete def;def=0;}
};

void TVarInfo::Create(TVarInfo*_pv)
{
    classType= _pv->classType;
    flags=_pv->flags;
    info=NewStr(_pv->info);
    def=NewStr(_pv->def);
    name=NewStr(_pv->name);
};

void TClass::DeleteVar(PVarInfo pv){
    pv->Done();
};
void TClass::DeleteChilds(){
    if (childcount){
        for(int i=0;i<childcount;i++)DeleteChild(&(childs[i]));
        delete childs;
        childs=NULL;
        childcount=0;
    }
};
void TClass::DeleteChild(PChildInfo pc){
    //if (pc->classname)delete pc->classname;
    pc->type=NULL;
    if (pc->name){delete(pc->name);pc->name=NULL;}
    if (pc->absentclass){delete (pc->absentclass);pc->absentclass=NULL;}
};
void TClass::DeleteLinks(){
    if (linkcount){
        for(int i=0;i<linkcount;i++){
            DeleteLink(&(link[i]));
        }
        delete link;
        link=0;linkcount=0;
    }
};
void TClass::DeleteLink(PLinkInfo pl){
    pl->Done();
};

void TClass::Update(TStream&st, INT16*codes)
{
    long base=st.GetPos();
    long pos=st.ReadLong();
    long _size=st.ReadLong();
    int count=((int)_size)/sizeof(TClassItem);
    TClassItem*_items=new TClassItem[count];
    st.Seek(base+pos);
    st.Read(_items,(int)_size);
    for(INT16 i=0;i<count;i++)
    {
        BOOL load=FALSE;
        if (!codes)
            load=TRUE;
        else
        {
            int j=0;
            while (codes[j]!=0 && codes[j]!=_items[i].code)
                j++;

            if (codes[j]==_items[i].code)
                load=TRUE;
        }
        if (load)
        {
            st.Seek(base+_items[i].pos);
            INT16 code=st.ReadWord();
            if (code==_items[i].code)
                switch(code)
                {
                    case CR_SCHEME:
                        if (!scheme)
                            Update(st,code,10000);
                        break;

                    case CR_IMAGE:
                        if (!image)
                            Update(st,code,(INT16)10000);
                        break;

                    case CR_TEXT:
                        if (!text)
                            Update(st,code,(INT16)10000);
                        break;

                    case CR_INFO:
                        if (!description)
                            Update(st,code,(INT16)10000);
                        break;

                    case CR_ICON:
                        if (!icon)
                            Update(st,code,(INT16)10000);
                        break;
                }
        }
    }
    delete _items;
};

void TClass::Update(TStream&st,INT16 code,UINT16 InitFlags)
{
    switch (code)
    {
        case CR_HELPFILE:
            helpfile=st.ReadStr();
            break;

        case CR_CLASSTIME:
            _classid=st.ReadLong();
            break;

        case CR_ICONFILE:
            IconName=st.ReadStr();
            break;

        case CR_DEFICON:
            deficon=st.ReadWord();
            break;

        case CR_EQU:
            equ=new TEqu(st);
            break;

        case CR_CODE:
            codesize=st.ReadLong();
            VM_code=new WORD[(INT16)codesize];
            st.Read(VM_code,(INT16)(codesize*2));
            break;

        case CR_VARS1:
        case CR_VARS:
        {
            varcount=st.ReadWord();
            vars=new TVarInfo[varcount];
            int i;
            for(i=0;i<varcount;i++)
            {
                vars[i].name  = st.ReadStr();
                vars[i].info  = st.ReadStr();

                if (code==CR_VARS1)
                    vars[i].def=st.ReadStr();
                else
                    vars[i].def=NULL;

                char type[256];
                st.ReadStr(type);
                vars[i].flags = st.ReadLong();
                if (vars[i].flags & VF_ONSCHEME)
                {
                    //vars[i].handle=
                    st.ReadWord();
                };//else vars[i].handle=0;

                vars[i].classType = ::GetClassListInfo(type);
                if (!vars[i].classType)
                    _Error(EM_VARCLASSNOTFOUND,0,type);
                //varsize++;
            }
        }break;

        case CR_VARSIZE:
            varsize = st.ReadWord();
            break;

        case CR_LINKS:
            linkcount=st.ReadWord();
            link=new TLinkInfo[linkcount];
            for(int i=0;i<linkcount;i++)
            {
                PLinkInfo l=&link[i];
                ReadLink(st,l);
            }
            break;

        case CR_CHILDSnameXY:
        case CR_CHILDSname:
        case CR_CHILDS:
        {
            childcount=st.ReadWord();
            childs=new TChildInfo[childcount];
            memset(childs,0,childcount*sizeof(TChildInfo));

            for(INT16 i=0;i<childcount;i++)
            {
                char type[256];
                st.ReadStr(type);
                childs[i].type=::GetClassListInfo(type);
                if(!(childs[i].type))
                {
                    _Error(EM_CHILDCLASSNOTFOUND,0,type);
                    childs[i].type=::GetClassListInfo("EMPTY_CLASS");
                    childs[i].absentclass=NewStr(type);
                }

                childs[i].handle   = st.ReadWord();
                if(code==CR_CHILDSname || code==CR_CHILDSnameXY)
                    childs[i].name=st.ReadStr();
                else
                    childs[i].name=NULL;
                if(code==CR_CHILDSnameXY)
                {
                    //st.Read(&(child[i].x),5);
                    if (fileversion<0x3002)
                    {
                        childs[i].x=st.ReadWord();
                        childs[i].y=st.ReadWord();
                        st.Read(&(childs[i].flags),1);
                    }
                    else
                    {
                        st.Read(&(childs[i].x),17);
                    }
                }
                else
                {
                    childs[i].x=childs[i].y=0;
                    childs[i].flags=CHF_NEEDUPDATE;
                }
            }
            /*-------------------------*/
            // была проверка на дублирование дескрипторов
            /*-------------------------*/
        }break;
        case CR_FLAGS:
            flags=st.ReadLong();
            break;
            /*  case CR_ICON:if (InitFlags >0)
              icon=new TIcon(st);
              else  st.Seek(st.GetPos()+st.ReadLong()+4);
              break;*/
        case CR_TEXT:
        {
            if ((InitFlags >1)&& (!NS.text))
                text=st.ReadStr();
            else
                st.Seek(st.GetPos()+st.ReadWord()+2);
        }break;
        case CR_INFO:
        {
            if ((InitFlags >2)&& (!NS.description))
                description=st.ReadStr();
            else
                st.Seek(st.GetPos()+st.ReadWord()+2);
        }break;
        case CR_SCHEME:
        {
            if (InitFlags > 2)
                scheme = new TScheme(this, st);
            else
                st.Seek(st.GetPos()+st.ReadLong());
        }break;
        case CR_IMAGE:
        {
            if ((InitFlags >1)&& (!NS.image))
                image=new TScheme(this,st);
            else
                st.Seek(st.GetPos()+st.ReadLong());
        }break;
        case CR_ICON:
        {
            if (InitFlags >1)
                icon=new TIcon(this,st);
            else
                st.Seek(st.GetPos()+st.ReadLong());
        }break;
        default:
        {
            // Обработка незнакомого
            st.Seek(st.GetPos()+st.ReadLong()-2);
            _Error(EC_UNCNOUCLASSENTRY,code);
        }
    }
};

int TClass::ChkScheme(HSP2D hsp)
{
    int _FAILED=0;
    for(INT16 i=0;i<childcount;i++)
    {
        HOBJ2D h=childs[i].handle;
        for(INT16 j=i+1;j<childcount;j++)
        {
            if(h==childs[j].handle){_FAILED|=1; goto m1;}
        }
    }
m1:
    for(i=0;i<linkcount;i++){
        HOBJ2D h=link[i].handle;
        for(INT16 j=i+1;j<linkcount;j++){
            if(h==link[j].handle){_FAILED|=2;goto m2;}
        }}
m2:
    if(_FAILED){
        char s[512]="";
        if(_FAILED&1)lstrcat(s,"* Invalid childs handle (duplicated images handle found!\n");
        if(_FAILED&2)lstrcat(s,"* Invalid links  handle (duplicated links handle found!\n");
        lstrcat(s,"Are YOU want to repair this scheme?");

        if(MessageBox(mainwindow->HWindow,s
                      ,name,MB_YESNO|MB_ICONSTOP)!=IDYES)_FAILED=0;
    }
    return _FAILED;
};
void TClass::ReadLink(TStream&st,PLinkInfo l){
    l->obj1Id=st.ReadWord();
    l->obj2Id=st.ReadWord();
    l->handle=st.ReadWord();
    l->flags=st.ReadLong();
    if(fileversion<0x3003) l->flags=0;
    l->count=st.ReadWord();
    l->varhandle=st.ReadWord();
    l->items=NULL;
    if (l->count>0)
    {
        l->items=new TLinkEntryInfo[l->count];
        for(INT16 j=0;j<l->count;j++){
            l->items[j].name1=st.ReadStr();
            l->items[j].name2=st.ReadStr();
        }
    }
};
void TClass::WriteLink(TStream&st,PLinkInfo l){
    st.WriteWord(l->obj1Id);
    st.WriteWord(l->obj2Id);
    st.WriteWord(l->handle);
    st.WriteLong(l->flags);
    st.WriteWord(l->count);
    st.WriteWord(l->varhandle);
    for(INT16 j=0;j<l->count;j++){
        st.WriteStr(l->items[j].name1);
        st.WriteStr(l->items[j].name2);
    }
}

TClass::TClass(TStream&st,UINT16 InitFlags)
{
    long _pos=st.ReadLong();
    long _size=st.ReadLong();
    memset(this,0,sizeof(TClass));
    name=st.ReadStr();
    WORD code;

    while((code=st.ReadWord())!=0)
        Update(st,code,InitFlags);

    st.Seek(_pos+_size);
    //if (flags&CF_DISABLED)
    UpdateDisableVar();
};

void TClass::Store(TStream&st)
{
    flags&=(~CF_MODIFY);
    int writecount=0;
    TClassItem position[10];
    long pos=st.GetPos();

    st.WriteLong(0);
    st.WriteLong(0);
    st.WriteStr(name);
#if !defined DEMO
    if (varcount)
    {
        int i;
        st.WriteWord(CR_VARS1);
        st.WriteWord(varcount);
        for(i=0;i<varcount;i++){
            st.WriteStr(vars[i].name);
            st.WriteStr(vars[i].info);
            st.WriteStr(vars[i].def);
            st.WriteStr(vars[i].classType->GetClassName());
            st.WriteLong(vars[i].flags);

            /*	if (vars[i].flags&VF_ONSCHEME){
            st.WriteWord(vars[i].handle);
     } */
        }
    }
    if (linkcount){
        st.WriteWord(CR_LINKS);
        st.WriteWord(linkcount);
        for(int i=0;i<linkcount;i++){
            PLinkInfo l=&link[i];
            WriteLink(st,l);
        }
    }
    if (childcount){
        st.WriteWord(CR_CHILDSnameXY);
        st.WriteWord(childcount);
        for(int i=0;i<childcount;i++){
            char *_type=childs[i].type->GetClassName();
            if(lstrcmp(_type,"EMPTY_CLASS")==0 && childs[i].absentclass)_type=childs[i].absentclass;
            st.WriteStr(_type);

            st.WriteWord(childs[i].handle);
            st.WriteStr(childs[i].name);
            // st.Write(&(child[i].x),5);
            st.Write(&(childs[i].x),17);
        }
    }
#endif
    if (icon){
        WRITE_LABEL(CR_ICON)
                icon->Store(st);
    };

    if (scheme){
        WRITE_LABEL(CR_SCHEME)
                scheme->Store(st);
    };
    if (image){
        WRITE_LABEL(CR_IMAGE)
                image->Store(st);
    };

    if (text){
        WRITE_LABEL(CR_TEXT)
                st.WriteStr(text);
    };
    if (description){
        WRITE_LABEL(CR_INFO);
        st.WriteStr(description);
    }
    if (varsize){
        st.WriteWord(CR_VARSIZE);
        st.WriteWord((INT16)varsize);
    }

    if (flags){
        st.WriteWord(CR_FLAGS);
        st.WriteLong(flags);
    }
    if (deficon){
        st.WriteWord(CR_DEFICON);
        st.WriteWord(deficon);
    }
    if(IconName){
        st.WriteWord(CR_ICONFILE);
        st.WriteStr(IconName);
    }

#if !defined DEMO
    if (VM_code){
        st.WriteWord(CR_CODE);
        st.WriteLong(codesize);
        st.Write(VM_code,(int)(codesize*2));
    }
    if (equ){
        st.WriteWord(CR_EQU);
        equ->Store(st);
    };
    if(!_classid)SetClassIdByDate();
    st.WriteWord(CR_CLASSTIME);
    st.WriteLong(_classid);
#endif
    if(helpfile){
        st.WriteWord(CR_HELPFILE);
        st.WriteStr(helpfile);
    }
    st.WriteWord(0);

    long _pos=st.GetPos();
    int _size=sizeof(TClassItem)*writecount;
    st.Write(&position,_size);
    st.Seek(pos);
    st.WriteLong(_pos-pos);
    st.WriteLong(_size);
    st.Seek(_pos+_size);
};
void TClass::Help(){
    char s[256];
    if(helpfile){

        if(!strncmpi(helpfile,"($class)",8)){
            listinfo->library->GetPath(s);
            AddSlash(s);
            lstrcpy(s,helpfile+8);
        }else lstrcpy(s,helpfile);

        char *ps= strstr(s,":");
        if(ps){*ps=0;ps++;}else ps="";

        WinHelp(mainwindow->HWindow,s,HELP_PARTIALKEY,(DWORD)ps);

    }else{

        wsprintf(s,"Class %s",name);
        HELP(s);
    }
};
void TClass::UpdateDisableVar()
{
    flags &= ~(CF_DISABLED | CF_UNDISABLED);
    for(INT16 i = 0; i < varcount; i++)
    {
        if(vars[i].classType->isFLOAT())
        {
            int isDisableVarExist = lstrcmpi(vars[i].name, DISABLE_VAR_NAME) == 0;
            int isEnableVarExist = lstrcmpi(vars[i].name, ENABLE_VAR_NAME) == 0;

            if(isDisableVarExist || isEnableVarExist)
            {
                flags |= CF_HAS_DISABLE_VAR;
                if(isEnableVarExist)
                    flags |= CF_HAS_ENABLE_VAR;

                disablevar=i;
                return;
            }
        }
    }
    disablevar=0;
}

TClass::TClass(char*_name, char*_text)
{
    memset(this,0,sizeof(TClass));
    if (_name)
        name=NewStr(_name);

    t_flags|=TCF_NEWCLASS;
    IconName=NewStr("system.dbm");
    if (_text)
        text=NewStr(_text);
    flags|=CF_MODIFY;
};

BOOL TClass::RemoveObjectFromList(PObject po){
    if (po==NULL || first==NULL)return FALSE;
    if (po==first)
    {first=po->class_next;return TRUE;}
    else {
        PObject _po=first;
        while(_po->class_next){
            if (_po->class_next==po){
                _po->class_next=_po->class_next->class_next;
                return TRUE;
            }
            _po=_po->class_next;
        }}
    return FALSE;
};

BOOL TClass::RemoveLinkToObject(HOBJ2D obj)
{
    if (obj<1)return 0;
    int i=0;
    while (i<linkcount)
    {
        if (link[i].obj1Id==obj ||
            link[i].obj2Id==obj ||
            (link[i].varhandle==obj &&(link[i].obj1Id==0 ||link[i].obj2Id==0)))
        {
            RemoveLink(link[i].handle,1);
        }
        else
            i++;
    }
    return 0;
};

BOOL TClass::RemoveChild(HOBJ2D id){
    if (GetLinkToObject(id))return 0;
    INT16 i=GetChildById(id);
    if (i>-1){
        PObject po=first; //-- сам не меняется (только списки детей)
        while (po){  // Просмотр объектов этого класса
            PObject _po=po->childObjects;
            PObject _pp=NULL;
            int j=0;
            while 	(_po){ // Собственно достаем объект c таким-id  из списка
                if (j==i){     // childof объекта, этого класса
                    if (_pp)_pp->next=_po->next;
                    else po->childObjects=_po->next;
                    UpdateClassWindows(UW_DELETE,UW_OBJECT,_po,NULL);
                    delete _po;
                    break;
                }
                _pp=_po;
                _po=_po->next;
                j++;
            }
            po=po->class_next;
        }            // Просмотр объектов этого класса
        // delete child[i].classname;
        DeleteChild(&(childs[i]));
        if (childcount>1){
            PChildInfo ci=new TChildInfo[childcount-1];
            int j=0;
            HOBJ2D _id=id;
            for(int i=0;i<childcount;i++)
            {
                if(childs[i].handle!=_id) ci[j++]=childs[i];else _id=(HOBJ2D)-2;

            }
            delete childs;
            childs=ci;
        }else {delete childs;childs=0;}
        childcount--;
        SetModify();
        return id;
    }return 0;
};

INT16 TClass::GetChildById(HOBJ2D id)
{
    for(INT16 i = 0; i < childcount; i++)
        if (childs[i].handle == id)
            return i;

    // _Error(EM_BADCHILDHANDLE,id);
    return -1;
};

/*void TClass::SetCode(int*_code,long _size){
 if (VM_code)delete VM_code;VM_code=NULL;
  if (_size){
    VM_code=new WORD [(WORD)_size];
    memcpy(VM_code,_code,(int)(_size/2));
  }
};*/

void TClass::SetCode(TStream* st)
{
    if (VM_code)
        delete VM_code;
    VM_code=NULL;
    // if (st) delete VM_code; VM_code=NULL;codesize=0;

    //  TDOSStream st(filename,TDOSStream::stOpenRead);
    codesize = st->GetSize()/2;
    if (codesize)
    {
        VM_code = new WORD[(int)codesize];
        st->Seek(0);
        st->Read(VM_code, (int)codesize*2);
    }
}

PLinkInfo TClass::GetLinkToObject(HOBJ2D id){
    for(INT16 i=0;i<linkcount;i++){
        if (link[i].obj1Id==id || link[i].obj2Id==id)return &(link[i]);
    }
    return NULL;
};

void TClass::Compute(PObject po)
{
    if (VM_code)
        VM_Execute(VM_code, po->vars, po);
}

void TClass::ClassProc(TClassMSG*){

};
PObject TClass::GetPrevObject(PObject){
    return NULL;
};


/*
    TScheme - class for storing scheme.vdr if image file
*/

TGrid::TGrid(HSP2D _hsp){
    hsp=_hsp;
    INT16 size=sizeof(TGrid)-sizeof(hsp);
    if (GetObjectData2d(hsp,0,UD_GRID,((BYTE*)this)+sizeof(hsp),0,size)!=size){
        OffsetX=OffsetY=0;
        StepX=StepY=8;
        use=1;
        visible=0;
    }
};
void TGrid::SetGrid(){
    visible=1;
    Update();
};
void TGrid::Remove(){
    visible=0;
    Update();
};

void TGrid::Update(){
    DWORD flags=SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
    if (visible)flags|=SF_GRID;else flags&=(~SF_GRID);
    SetSpaceParam2d(hsp,SP_SETSPACEFLAG,flags);
    SetObjectData2d(hsp,0,UD_GRID,((BYTE*)this)+sizeof(hsp),0,sizeof(TGrid)-sizeof(hsp));
};

TScheme::TScheme(PClass _c){
    memset(this,0,sizeof(TScheme));
    _class=_c;
};
TScheme::TScheme(PClass _c,HSP2D _hsp){
    memset(this,0,sizeof(TScheme));
    _class=_c;
    hsp=_hsp;
};

TScheme::TScheme(PClass _c, TStream&st)
{
    memset(this,0,sizeof(TScheme));
    _class = _c;
    long _pos = st.GetPos();
    long _size = st.ReadLong();
    flags=st.ReadLong();

    if (flags & SF_EXTERNAL)
    {
        filename = st.ReadStr();
    }
    else
    {
        size = st.ReadLong();
        block = GlobalAlloc(GMEM_MOVEABLE,size);
        scheme = GlobalLock(block);
        st.Read(scheme, size);
    }
    long newpos = st.GetPos();
    if((newpos-_size) != _pos)
    {
        /*
  int h=st.ReadWord();
  if (h==0x4242){
    int code=st.ReadWord();
    while(code){
     if (code==1)grid=new TGrid(st);
     code=st.ReadWord();
    }
  }
  */
        _Error(EM_BADSHEMESIZE);
        st.Seek(_pos);
    }
}

TScheme::TScheme(PClass _c,char*_filename,int)
{
    memset(this,0,sizeof(TScheme));
    _class=_c;
    filename=NewStr(_filename);
    flags|=SF_EXTERNAL;
};

void TScheme::Store(TStream& stream)
{
    BOOL wasscheme= scheme != NULL;
    long _pos=stream.GetPos();

    stream.WriteLong(0);
    stream.WriteLong(flags&(~SF_EDITING));

    HSP2D _hsp=hsp;
#ifndef RUNTIME
    if (window)
        window->DeleteAllEditItems(_hsp);
#endif 
    Hpointer _scheme = scheme;
    long _size=size;
    HGLOBAL mem=0;

    /*
    Если редактируется то копируем и сохраняем копию
 */

    if (flags&SF_EDITING)
    {
        if (_class->scheme==this)
            _hsp=ConvertClassScheme(_class,0,1);

        if (!(flags&SF_EXTERNAL))
        {
            HGLOBAL mem=SaveToMemory2d(_hsp,&_size);
            _scheme=GlobalLock(mem);
        }
    }

    if (flags&SF_EXTERNAL)
    {
        stream.WriteStr(filename);
        if (flags&SF_EDITING)
        {
            char s[256];
            GetFilename(s);
            State2dSaveAs(_hsp,s);
        };
    }
    else
    {
        if (_scheme==0 && _hsp)
        {
            SetScheme();
            _scheme=scheme;
            _size=size;
        }
        stream.WriteLong(_size);
        stream.Write(_scheme,_size);
        SetSpaceModify2d(hsp,0);
    };
    /*
  На случай редактирования удаляем копию
*/

    if (flags&SF_EDITING)
    {
        if (_class->scheme==this)
            DeleteSpace2d(_hsp);

        if (!(flags&SF_EXTERNAL))
        {
            GlobalUnlock(mem);
            GlobalFree(mem);
        }
    };

    long _pos1=stream.GetPos();
    stream.Seek(_pos);
    stream.WriteLong(_pos1-_pos);
    stream.Seek(_pos1);
    if((wasscheme==0) && scheme)
    {
        if (block)
        {
            GlobalUnlock(block); GlobalFree(block);scheme=0;
        }
    }
};

HSP2D TScheme::CreateSpace()
{
    if (hsp)return hsp;
    //hsp=CreateSpace2d(0,scheme_path);
    if(!hsp)
    {
        char s[256];
        if(SCGetScVarS("template_scheme2d",s))
            hsp = LdSpace2d(0, s);

        if(!hsp)
            hsp = CreateSpace2d(0, scheme_path);
    }
    //DeleteTool2d(hsp,STRING2D,CreateString2d(hsp,"m"));
    SetSpaceModify2d(hsp, TRUE);
    return hsp;
}

HSP2D TScheme::GetSpace(BOOL getcopy)
{
    if (hsp)
    {
        if (getcopy)
        {
            long size;
            BOOL m = IsSpaceChanged2d(hsp);
            HGLOBAL bl = SaveToMemory2d(hsp, &size);
            char cp[260];
            _class->GetClassListInfo()->library->GetPath(cp);
            HSP2D __hsp = LdFromMemory2d(0, bl, cp);
            GlobalFree(bl);
            SetSpaceModify2d(hsp,m);
#ifndef RUNTIME
            if(window)
                window->DeleteFrames(__hsp);
#endif  
            return __hsp;
        }
        return hsp;
    }
    if (flags & SF_EXTERNAL)
    {
        char s[256];
        GetFilename(s);
        hsp = LdSpace2d(0,s);
    }
    else
    {
        char cp[260];
        _class->GetClassListInfo()->library->GetPath(cp);

        hsp = LdFromMemory2d(0,block,cp);
        if (hsp)
        {
            GlobalUnlock(block);
            GlobalFree(block);
            scheme=0;
            block=0;
        }
    }
    return hsp;
};

BOOL TScheme::GetFilename(char*s){
    PClassListInfo pcli=_class->GetClassListInfo();
    pcli->library->GetPath(s);lstrcat(s,filename);
    return 1;
};

BOOL TScheme::SetScheme(){
    if (flags&SF_EXTERNAL){
        char s[256];
        GetFilename(s);
        State2dSaveAs(hsp,s);
    }
    else{if(block){
            GlobalUnlock(block);
            GlobalFree(block);
            scheme=0;
        }
        //  State2dSaveAs(hsp,"c:\\1__.vdr");
        block=SaveToMemory2d(hsp,&size);
        scheme=GlobalLock(block);
    }
    return TRUE;
}

TScheme::~TScheme()
{

    if (flags&SF_EXTERNAL){
    }
    else{if (block){GlobalUnlock(block); GlobalFree(block);scheme=0;}
    }

    if (filename)
    {
        delete filename;
        filename=0;
    }

    if (hsp)
    {
        DeleteSpace2d(hsp);
        hsp=0;
    }
    // if (grid)delete grid;
    // int h=1;

}

void TLinkInfo::Create(TLinkInfo*_link)
{
    *this = *_link;
    if (count)
    {
        items=new TLinkEntryInfo[count];
        for(int i=0;i<count;i++)
        {
            items[i].name1=NewStr(_link->items[i].name1);
            items[i].name2=NewStr(_link->items[i].name2);
        }
    }
    else
        items=NULL;
}

void TLinkInfo::Done()
{
    for(int i=0;i<count;i++)
    {
        if(items[i].name1)
        {
            delete items[i].name1;
            items[i].name1=NULL;
        }
        if(items[i].name2)
        {
            delete items[i].name2;
            items[i].name2=NULL;
        }
    }
    if (items)
    {
        delete items;
        items=NULL;
    }
    count=0;
}

void TClass::SetVars(PVarInfo pv,INT16 count)
{
    DeleteVars();
    vars=pv;
    varcount=count;
    for(INT16 i=0;i<varcount;i++)
    {
        if (!(vars[i].classType->flags&CLF_LOADED))
        {
            char s[65];
            lstrcpy(s,vars[i].classType->GetClassName());
            GetClassByName(s,10);
        }
    }
    UpdateDisableVar();
    SetModify();
}

BOOL StrToDouble(char*text,double&value)
{
    if(text)
    {
        char * endptr=(char *)text;
        value = strtod(text,&endptr);
        if (endptr==NULL || *endptr==0)
            return TRUE;
    }
    else
    {
        value=0;
    }

    return 0;
}
BOOL TClass::IsBaseClass()
{
    if(varcount || childcount)return FALSE;
    return  (
                listinfo->isFLOAT()||listinfo->isHANDLE()||
                listinfo->isRGB()  ||listinfo->isSTRING()||
                listinfo->isWORD()||listinfo->isBYTE()
                );
}

BOOL TClass::SetObjectOrder(HOBJ2D id,INT16 order)
{
    INT16 j=GetChildById(id);
    if (order>=childcount || order<0)
        return FALSE;

    if (j==order)
        return order;

    TChildInfo*ci=new TChildInfo[childcount];
    memcpy(ci,childs,sizeof(TChildInfo)*childcount);

    TCollection items(childcount,0);
    for(C_TYPE i=0;i<childcount;i++)
    {
        items.Insert(ci+i);
    }
    pointer _pc=items.At(j);
    items.AtDelete(j);
    items.AtInsert(order,_pc);

    for(i=0;i<childcount;i++)
    {
        childs[i]= *((TChildInfo*)(items.At(i)));
    }
    delete ci;
    items.DeleteAll();
    // Идет изменение объектов
    PObject po=first;
    while(po)
    {
        PObject _po=po->childObjects;
        while (_po)
        {
            items.Insert(_po);
            _po=_po->next;
        }
        //	items.Swap(j,order);
        pointer _pobject=items.At(j);
        items.AtDelete(j);
        items.AtInsert(order,_pobject);

        po->childObjects=(PObject)items.At(0);;
        for(i=1;i<items.count;i++)
        {
            PObject _pp=(PObject)items.At(i);
            ((PObject)items.At(i-1))->next=_pp;
            _pp->next=NULL;
        }
        items.DeleteAll();
        po=po->class_next;
    }
    SetModify();
    return TRUE;
};

BOOL TObject::SendMessage2(char *typeName, char *path, char**send_vars, int count, char* filter)
{
    UINT32 _changecount;
    if (!typeName || (!(*typeName)))
        typeName=NULL;

    if (!path || (!(*path)))
        path=NULL;

    PClass type=NULL;
    PObject targetObject=NULL;
    BOOL    all=FALSE;
    if (typeName)
    {
        type = GetClassByName(typeName);
        if (!type)
            return FALSE;
    }

    if (path)
        targetObject = GetReceiver(path, &all);

    if ((targetObject==NULL)&&(type==NULL))
        return FALSE;

    struct namestruct
    {
        char *out;
        char *in;
    };
    namestruct * names=(namestruct *)send_vars;
    struct msgstruct
    {
        INT16 v1;
        INT16 v2;
        INT16 L;
        char * _v1;
        char * _v2;
    };
    msgstruct * _vars;
    if (count)
    {
        _vars=new msgstruct[count];
    }
    else _vars=NULL;

    for(INT16 i=0;i<count;i++)
    {
        INT16 index;
        if ((index=_vars[i].v1=GetVarIdByName(names[i].in))>-1)
        {
            _vars[i].L=(INT16)_class->vars[index].classType->_class->varsize;
            _vars[i]._v1=(char*)vars[index];
        }
        else
        {
            _vars[i].L=0;
            _vars[i]._v1=0;
        }
    }

    PClass _oldtype=NULL;
    PClass _curtype=NULL;
    PObject _targetObject = targetObject;

    if ((_targetObject==NULL) && (type))
        _targetObject = type->first;

    while (_targetObject)
    {
        if (type==0 || (_targetObject->_class == type) )
        {
            // Проверка типа
            _curtype=_targetObject->_class;

            //чтобы не послать себе
            if (!((all && _targetObject==this) || (type && _targetObject==this)))
            {
                // Обработка фильтра
                if(filter)
                {
                    INT16 num;
                    if(_targetObject->GetHandle(&num))
                    {
                        char *name=_targetObject->parent->_class->childs[num].name;
                        if(name)
                        {
                            char *_pf=filter;
                            while (*name && *_pf)
                            {
                                if(*name!=*_pf)
                                {
                                    if(*_pf!='?')
                                    {
                                        if(*_pf!='*')goto m1;
                                    }
                                }
                                name++;_pf++;
                            }
                            if(*name)goto m1;
                        }else {
                            if(*filter)goto m1;
                        }
                    }else {if(*filter)goto m1;}
                }
m2:

                for(i=0;i<count;i++)
                {
                    if (_curtype!=_oldtype)
                    {
                        INT16 index;
                        if((index=_vars[i].v2=_targetObject->GetVarIdByName(names[i].out))>-1)
                        {
                            if (_curtype->vars[index].classType!=_class->vars[_vars[i].v1].classType)
                            {
                                _vars[i].v2=-1;
                            }
                        }
                    }
                    if(_vars[i].v2>-1){ _vars[i]._v2=(char*)_targetObject->vars[_vars[i].v2];}else _vars[i]._v2=NULL;
                }

                _oldtype=_curtype;

                //*******************************************************
                //     Собственно сообщение B E G I N
                //*******************************************************
                _changecount=changecount;
                for(i=0;i<count;i++)
                {
                    if((_vars[i]._v1) && (_vars[i]._v2))
                    {
                        INT16 l=_vars[i].L;
                        memcpy(_vars[i]._v2,_vars[i]._v1+l,l);
                        memcpy(_vars[i]._v2+l,_vars[i]._v1+l,l);
                    }
                }
                // po->_CopyState(OLD_TO_NEW);
                _targetObject->_Compute();
                _targetObject->_CopyState(NEW_TO_OLD);

                /*
 if(!lstrcmpi(_tobject->_class->name,"SearchVar_01"))
 {
  _Message("Failed");
 }
*/
                if(_changecount!=changecount)
                {
                    // Произошло изменение модели
                    for(i=0;i<count;i++)
                    {
                        INT16 index;
                        if( (index = _vars[i].v1 = GetVarIdByName(names[i].in)) > -1)
                        {
                            _vars[i]._v1=(char*)vars[index];
                        }

                        if ((index=_vars[i].v2=_targetObject->GetVarIdByName(names[i].out))>-1)
                        {
                            _vars[i]._v2=(char*)vars[index];
                        }
                        else
                            _vars[i]._v2=NULL;
                    }

                }
                for(i=0;i<count;i++){
                    if((_vars[i]._v1) && (_vars[i]._v2)){
                        INT16 l=_vars[i].L;
                        memcpy(_vars[i]._v1+l,_vars[i]._v2+l,l);
                    }};

                //*******************************************************
                //     Собственно сообщение E N D
                //*******************************************************
m1:
            }// Чтоб не вызвать себя
        }// Проверка типа

        // Ищем следующий
        if (all)
        {
            _targetObject = _targetObject->next;
        }
        else
        {
            if (type && targetObject == NULL)
                _targetObject = _targetObject->class_next;
            else
                _targetObject=NULL;
        }

    }
    if (_vars)delete _vars;
    return TRUE;
}

HOBJ2D TObject::GetHandle(INT16 *num)
{
    if (parent)
    {
        PObject po = parent->childObjects;
        INT16 i=0;
        while (po)
        {
            if (po == this)
            {
                if (num)
                    *num = i;

                return (parent->_class->childs[i].handle);
            }
            i++;
            po = po->next;
        }
    }
    return 0;
}

TClassMSG* CreateMSG(PObject po, HOBJ2D obj, INT16 code, UINT16 flags, char* data)
{
    INT16 size = sizeof(TClassMSG);
    char* s = data;
    INT16 count = 0;
    INT16 ssize = 0;

    if (s)
    {
        int f = 1;
        while(*s)
        {
            if (*s==',')
            {
                count++;
                f = 1;
            }
            if (*s==':')
                f=0;

            if (f && (*s != ':'))
                ssize++;
            s++;
        }

        if (data)
        {
            ssize++;
            count += (INT16)1;
        }
    }
    INT16 pcount = SCGetFullObjectName(po,0,0);
    size += (INT16)(sizeof(WORD) * pcount + sizeof(TClassMSG::TVAR) * count + ssize);
    TClassMSG * msg = (TClassMSG *) new char[size];
    memset(msg,0,size);

    msg->pobject = po;
    msg->code = code;
    msg->object = obj;
    msg->flags = flags;
    msg->varcount = count;
    msg->pathcount = pcount;

    msg->path = (INT16*)((char*)(msg) + sizeof(TClassMSG));
    msg->vars = (TClassMSG::TVAR*)( (char*)msg->path + 2 * pcount );

    char * names = ((char*)(msg->vars))+sizeof(TClassMSG::TVAR) * count;

    SCGetFullObjectName(po, msg->path, pcount);

    //Инициализация переменных сообщения (непонятно почему сначала переменные кодируются в строку, а затем здесь парсятся обратно)
    s=data;
    {
        char * var = s;
        char * type = NULL;
        //char * names1=names;
        INT16 i = 0;
        s = data;
        while(count)
        {
            if (*s == ',' || *s == 0)
            {
                if (type)
                {
                    char ss[65];
                    INT16 j = 0;
                    while (*type && *type != ',')
                    {
                        ss[j] = *type;
                        type++;
                        j++;
                    }
                    ss[j] = 0;
                    msg->vars[i].type = GetClassListInfo(ss);
                }
                else
                    msg->vars[i].type = GetClassListInfo("FLOAT");

                msg->vars[i].name = names;
                while(*var && *var != ':' && *var != ',')
                {
                    *names = *var;
                    names++;
                    var++;
                }
                *names = 0;
                names++;
                var = s + 1;
                type = 0;
                i++;
            }
            if (!(*s))
                return msg;

            if (*s==':')
                type=s+1;
            s++;
        }
        return msg;
    }
}

void TClassMSG::InvalidType(char *s,PClassListInfo pcli)
{
    char ss[128];
    lstrcpy(ss,"Var (");
    lstrcat(ss,s);
    lstrcat(ss," in message must be ");
    lstrcat(ss,pcli->GetClassName());
    lstrcat(ss,"type");
    _Message(ss);
}

void TClassMSG::NotFound(char *s)
{
    if(flags&16)return;
    char ss[128];
    lstrcpy(ss,"Var (");
    lstrcat(ss,s);
    lstrcat(ss,")in message not found");
    _Message(ss);
}

//Копирование данных переменных сообщения из объекта
BOOL  TClassMSG::Update(INT16 uo)
{
    if (uo)
        pobject = SCGetObjectByFullName(project, path, pathcount);

    if (!pobject)
    {
        _Message("WARNING Object to message not found!");
        return 0;
    }

    int id = pobject->GetVarIdByName("msg");

    if (id > -1 && pobject->_class->vars[id].classType->isFLOAT())
    {
        message = (double*)pobject->vars[id];

        for(INT16 i = 0; i < varcount; i++)
        {
            PVarInfo pv;
            id = pobject->GetVarIdByName(vars[i].name);
            if (id > -1)
            {
                pv = &(pobject->_class->vars[id]);
                vars[i].data = pobject->vars[id];
                if(vars[i].type!=pv->classType)
                {
                    vars[i].data=0;
                    InvalidType(vars[i].name,vars[i].type);
                }
            }
            else
            {
                vars[i].data = 0;
                NotFound(vars[i].name);
            }
        }
    }
    else
    {
        _Message("Var (msg) of type FLOAT not found or type mismatch");
    }
    return TRUE;
}

BOOL Push_Context();
void Pop_Context();

//----------------------------------------//
void  TClassMSG::Send(INT16 code, void *data, BOOL ret)
{
    if (pobject == 0 || message == NULL)
        return;

    if (((EXEC_FLAGS & EF_RUNNING)==0) && ((flags & 256)==0))
        return;

    if(lflags & CMSG_INPROCESS)
        return;

    lflags |= CMSG_INPROCESS;

    *message = code;        //set msg
    *(message + 1) = code;  //set ~msg судя по всему

    char* _d = (char*)data;
    for(INT16 i = 0; i < varcount; i++)
    {
        INT16 l = (INT16)(vars[i].type->_class->varsize);
        if (vars[i].data)
        {
            memcpy(vars[i].data, _d, l);
            memcpy(((char*)(vars[i].data))+l,_d,l);
        }
        _d += l;
    }

    DWORD _EXEC_FLAGS = ::EXEC_FLAGS;
    EXEC_FLAGS |= (EF_STEPACTIVE|EF_MSGACTIVE);
    
    PClass __c=pobject->_class;
    if(Push_Context())
        pobject->_Compute();
    Pop_Context();
    
    /*
 if(IsBadReadPtr(pobject,sizeof(TObject)) || __c!=pobject->_class ){
  MessageBox(mainwindow->HWindow,"  ","  ",MB_OK);
 }
*/
    pobject->_CopyState(TObject::NEW_TO_OLD);
    ::EXEC_FLAGS=_EXEC_FLAGS;
    if (ret)
    {
        for(int i = 0; i < varcount; i++)
        {
            if(vars[i].data)
            {
                int l = (int)vars[i].type->_class->varsize;
                memcpy(_d, vars[i].data, l);
            }
        }
    }

    lflags &= ~CMSG_INPROCESS;
    if(lflags & CMSG_MUSTREMOVE)
    {
        delete this;
    }
}

//----------------------------------------//
HSP2D GetDuplicateSpace(HSP2D hsp){
    long size=0;
    HGLOBAL mem=SaveToMemory2d(hsp,&size);
    char cp[260];GetTexturePath2d(hsp,cp);
    HSP2D _hsp=LdFromMemory2d(0,mem,cp);
    GlobalFree(mem);
    return _hsp;
};
TIcon::TIcon(PClass _class){
    flags=0;bits=new char [32*32/2];
    _class->flags|=CF_ICON;
}

TIcon::TIcon(PClass,TStream&st)
{
    /*	long _size=*/st.ReadLong();
    flags=st.ReadWord();
    int size=32*32/2;
    bits=new char [size];
    st.Read(bits,size);
}

void TIcon::Store(TStream&st){
    long _pos=st.GetPos();
    st.WriteLong(0);
    st.WriteWord(flags);
    st.Write(bits,32*32/2);
    long _pos1=st.GetPos();
    st.Seek(_pos);
    st.WriteLong(_pos1-_pos);
    st.Seek(_pos1);
};

TIcon::~TIcon(){
    if (bits)delete bits;
    bits=NULL;
};


void TIcon::Update(PClass _class)
{
    _class->flags|=CF_ICON;
    RECT2D r;
    HSP2D hsp=_class->image->GetSpace();

    if (GetSpaceExtent2d(hsp,r))
    {
        int sizex,sizey;
        sizey= r.bottom-r.top;
        sizex=r.right-r.left;

        if (sizex<32)
            sizex=32;
        if (sizey<32)
            sizey=32;

        POINT2D org;
        GetOrgSpace2d(hsp,&org);

        POINT2D _mul,_div;
        GetScaleSpace2d(hsp,&_mul,&_div);

        BOOL m=IsSpaceChanged2d(hsp);
        HDC dc;
        HDC hdc = GetDC(0);
        dc=CreateCompatibleDC(hdc);
        HBITMAP hbm=CreateCompatibleBitmap(hdc,sizex,sizey);
        if (_mul.x != _div.x || _mul.y != _div.y)
        {
            POINT2D p;
            p.x = p.y = 1;
            SetScaleSpace2d(hsp, &p, &p);
        }
        POINT2D p;
        p.x = r.left;
        p.y = r.top;

        SetOrgSpace2d(hsp,&p);
        SelectObject(dc,hbm);
        PatBlt(dc, 0,0, sizex,sizey, WHITENESS);

        RECT r1;
        r1.left = palette[0]; // Чтобы была использованна
        r1.left = 0;
        r1.top = 0;
        r1.bottom = sizey;
        r1.right = sizex;
        PaintSpace2d(hsp,dc,&r1,0);

        if (sizex>32 || sizey>32)
        {
            HBITMAP hbm1 = CreateCompatibleBitmap(hdc, 32,32);
            HDC dc1 = CreateCompatibleDC(hdc);
            SelectObject(dc1, hbm1);
            PatBlt(dc1,0,0,32,32,WHITENESS);
#ifdef WIN32
            SetStretchBltMode(dc1, COLORONCOLOR);
#else
            SetStretchBltMode(dc1,STRETCH_DELETESCANS);
#endif
            StretchBlt(dc1, 0,0,32,32, dc, 0,0, sizex,sizey, SRCCOPY);
            DeleteDC(dc);
            DeleteObject(hbm);

            dc=dc1;
            hbm=hbm1;
        }

        /*int scl=*/GetDIBits(dc, hbm, 0,32, bits, (BITMAPINFO*)&_info_16, DIB_RGB_COLORS);

        DeleteDC(dc);
        DeleteObject(hbm);
        if (_mul.x != _div.x || _mul.y != _div.y)
            SetScaleSpace2d(hsp,&_mul,&_div);

        SetOrgSpace2d(hsp,&org);
        SetSpaceModify2d(hsp,m);

        ReleaseDC(0, hdc);
    }
    else
    {
        delete bits;
        bits=0;
    }
}

void TClass::UpdateIcon()
{
    if (image)
    {
        if (!icon)
            icon=new TIcon(this);

        if (icon->flags & IF_CORRECTED)
            return;
        icon->Update(this);
        if (!icon->bits)
        {
            delete icon;
            icon=0;
            flags &= ~CF_ICON;
        }
    }
    else
    {
        if (icon)
            delete icon;
        icon=0;
        flags &= ~CF_ICON;
    }
    UpdateClassWindows(UW_MODIFY, UW_CLASS, listinfo, listinfo->library);
}

void PaintIcon(PClass _class, HDC dc,int x,int y,int sx,int sy)
{
    if ((_class->flags & CF_ICON) && (_class->flags & CF_USEDEFICON) == 0)
    {
        if(!(_class->icon))
            _class->UpdateItem(CR_ICON);

        if(_class->icon)
        {
            StretchDIBits(dc,x,y,sx,sy,0,0,32,32, _class->icon->bits,(BITMAPINFO*)&_info_16,DIB_RGB_COLORS,SRCCOPY);
        }
    }
    else
    {
        HOBJ2D static bmp;
        if (!spec_space)
        {
            spec_space=   CreateSpace2d(0,scheme_path);
            HOBJ2D o=CreateRDoubleDib2d(spec_space,default_dbm);
            POINT2D SrcOrg;SrcOrg.x=0;SrcOrg.y=0;
            POINT2D SrcSize;SrcSize.x=32;SrcSize.y=32;
            POINT2D DstOrg;DstOrg.x=0;DstOrg.y=0;
            bmp=CreateDoubleBitmap2d(spec_space,o,&SrcOrg,&SrcSize,&DstOrg,&SrcSize);
        };
        SetDefaultSrc(spec_space,bmp,_class->deficon,_class->GetIconFile());
        POINT2D p;
        p.x=x;p.y=y;
        SetObjectOrg2d(spec_space,bmp,&p);
        p.x=sx;p.y=sy;
        SetObjectSize2d(spec_space,bmp,&p);
        RECT r;
        r.left=x;r.top=y;
        r.right=r.left+sx;r.bottom=r.top+sy;
        PaintSpace2d(spec_space,dc,&r,1);
    }
}

void TClass::UpdateCoordinates(){
    HSP2D hsp=0;
    if (scheme && scheme->Loaded())
    {
        hsp=scheme->GetSpace();
    }
    if (hsp)
    {
        for(INT16 i=0;i<childcount;i++)
        {
            HOBJ2D h=childs[i].handle;
            POINT2D p;
            GetObjectOrgSize2d(hsp,h,&p,NULL);
            if (childs[i].x!=p.x || childs[i].y!=p.y)
            {
                SetModify();
                childs[i].x=p.x;
                childs[i].y=p.y;
            }
        }
    }
}

BOOL IsNameValid(char *name){
    int l=lstrlen(name);
    if (l>2){
        BOOL cyr=SCGetScVar("CanUseCyr",0);
        char *ps=name;
        char c=*ps;
        if ((c>='A'&& c<='Z')  ||
            (c>='a' && c<='z') ||
            ((cyr)&&(c&0x80))   ||
            (c=='_')
            )
        {ps++;
            while (*ps){
                c=*ps;
                if (    (c>='0' && c<='9')
                        || (c>='A'&&  c<='Z')
                        || (c>='a' && c<='z')
                        ||	(c=='_')
                        || ((cyr)&&(c&0x80))
                        )ps++;else return 0;
            }
            return TRUE;
        } }
    return FALSE;
};

void TClass::SetEqu(TEqu*_equ)
{
    if (equ){delete equ;equ=0;}
    equ=_equ;
};

BOOL AddObjectName(char*ss)
{
    PClass _class;
    PObject po;
    INT16 operation;
    if((operation=GetRunningObject(&_class,&po))!=0)
    {
        if (_class)
        {
            char cmd[128];
            LoadString(HResource,IDS_str139,cmd,sizeof(cmd));
            wsprintf(ss+lstrlen(ss),cmd ,_class->name);
        }
        if(compiler)
        {
            char s[256]="";
            if (compiler->GetFunctionName(operation,s))
            {
                LoadString(HResource,IDS_str140,ss+lstrlen(ss),50);
                lstrcat(ss,s);
            }
        }
        else
        {
            LoadString(HResource,IDS_str141,ss+lstrlen(ss),80);
        }
    }
    return TRUE;
};


UINT32  TClass::fileversion=0;
HSP2D spec_space=0;
UINT32 calcedsteps=0;
#ifdef LOGON
TLOGoffset::TLOGoffset(BYTE _offset){
    offset=_offset;
    LogStep(offset);
};
TLOGoffset::~TLOGoffset(){
    LogStep(-offset);
};
#endif


void GetClassesInfo(TStream&stream)
{
    char s[256];
    C_TYPE j=0;
    C_TYPE objs=0;
    for(C_TYPE i=0;i<classes->count;i++){
        PClassListInfo pcli=(PClassListInfo)classes->At(i);
        if(pcli->flags&CLF_LOADED){
            j++;
            if(pcli->GetClass()->first)objs++;
        }
    }

    char cmd[128];
    LoadString(HResource,IDS_str142,cmd,sizeof(cmd));

    wsprintf(s,cmd,classes->count,j);
    stream.WriteDOSSTR(s);
    if(project)
    {
        LoadString(HResource,IDS_str143,cmd,sizeof(cmd));
        stream.WriteDOSSTR(cmd);
        LoadString(HResource,IDS_str144,cmd,sizeof(cmd));

        INT32 cc=project->object->GetChildCount();
        LoadString(HResource,IDS_str145,cmd,sizeof(cmd));
        wsprintf(s,cmd,cc);

        stream.WriteDOSSTR(s);
        LoadString(HResource,IDS_str146,cmd,sizeof(cmd));
        wsprintf(s,cmd,project->VarsPointers->GetSize()/4L);
        stream.WriteDOSSTR(s);
        LoadString(HResource,IDS_str147,cmd,sizeof(cmd));
        wsprintf(s,cmd,project->VarsMemory->GetSize());
        stream.WriteDOSSTR(s);
    }
}

TSetVar::TSetVar(TStream&st)
{
    next = NULL;
    name = st.ReadStr();
    data = st.ReadStr();
}

TSetVar::TSetVar(char*v,char*d)
{
    next = NULL;
    name = NewStr(v);
    data = NewStr(d);
}

TSetVar::~TSetVar()
{
    if(name)delete name;
    if(data)delete data;
    if(next)delete next;
}
/* SDK */

extern "C" void SCMessage(char*s)
{
    _Message(s);
}

extern "C" INT16 SCGetFullObjectName(PObject po, INT16* p, INT16 size)
{
    PObject _po = po;
    if(size)
    {
        for(INT16 i = 0; i < size; i++)
        {
            INT16 num = 0;
            p[size - (i + 1)] = _po->GetHandle(&num);
            _po = _po->parent;
        }
        return size;
    }
    else
    {
        while (_po)
        {
            _po = _po->parent;
            if(_po)
                size++;
        }
        return size;
    }
}

extern "C" PObject  _SDKPROC SCGetObjectByFullName(PProject p,INT16*path,INT16 count)
{
    PObject object=p->object;
    for(INT16 i=0;i<count;i++)
    {
        object=object->_GetObject(path[i]);
        if(object==NULL)return NULL;
    }
    return object;
};

extern "C" PClassListInfo _SDKPROC SCGetClassListInfo(char *s)
{
    return GetClassListInfo(s);
};

extern "C" PClass _SDKPROC SCGetClass(char *s)
{
    return GetClassByName(s);
};

extern "C" PProject _SDKPROC SCGetObjectProject(PObject po)
{
    if(po)
    {
        return po->GetProject();
    }
    return NULL;
};

TCollection *  callbacks=NULL;

DWORD __InstallCallBack(TCallBack** _c, void* p)
{
    if(!callbacks)
        callbacks=new TCollection(10,10);

//    TCallBack*  next;
//    TCallBack*  _first;
//    void* proc;

    TCallBack* c = new TCallBack;
    callbacks->Insert(c);

    c->proc = p;
    c->_first = _c;
    c->next = NULL;

    if(*_c)
        c->next = (*_c);
    *_c = c;

    return (DWORD)c;
}

#ifdef PROTECT
void GetSecondCode2(char*out){
    char *ps="\xa6\xa9\xb6\xac\xa1\xb9\x9e\xa6\xdd\xa0\xdd\xd0\xd2\xd2\xd5\xc8\xd0\xd0\xd7\xdd\xc8\xd4\xd4\xa1\xd7\xc8\xdc\xd2\xd5\xd7\xc8\xd5\xd5\xa6\xd5\xd3\xa6\xd0\xdd\xd4\xd1\xdc\xd2\x98\xb9\xb3\x80\x97\x96\x8c\x8a\x8b\xe5";
    char *_ps=ps;
    char s1[256];
    char*__ps=s1;
    while(1){
        *__ps=((BYTE)(*_ps))^(0xe5);
        if(((BYTE)(*__ps))==0xe5)goto m2;
        _ps++;__ps++;
    }
m2:
    __ps=out;
    GetValueData(HKEY_CLASSES_ROOT,s1,"",out,sizeof(64));
    for(int i=0;i<12;i++){
        __ps[i]='Z'-__ps[i*3]+'0';
    }
    __ps[12]=0;

};

BOOL IsRegistered3(){
    char ss[256];
    GetValueData("Register","Name",ss,sizeof(ss));
    lstrcat(ss," ");
    GetValueData("Register","EMail",ss+lstrlen(ss),sizeof(ss)-lstrlen(ss));
    char s[64];
    GetSecondCode2(s);
    return test5(s,ss);
};

BOOL IsRegistered4(){
    char ss[256];
    GetValueData("Register","Name",ss,sizeof(ss));
    GetValueData("Register","EMail",ss+lstrlen(ss),sizeof(ss));
    return test3(secondcode,ss);
};
void _CharLower(char*s);
int _test5(char *password, char *key_string)
{
    _CharLower(key_string);
    char buffer[6];
    unsigned long keyval;
    int i,element,code,j;
    unsigned char ch,*byte_array;
    byte_array =(unsigned char*) &keyval;
    j = 0;
    code = 0;
    for ( i = 0; i < strlen(key_string); i++)
    {
        element = key_string[i];
        if ((element > 96 && element < 123)||(element > 47 && element < 58))
        {
            code += element;
            if (j < 6)
            { buffer[j] = element; j++; }
        }
    }
    ch = password[10];
    password[10] = 0;
    keyval = atol(password);
    password[10] = ch;
    for (i = 2; i >= 0; i--) byte_array[i+1] ^= byte_array[i];
    keyval -= code;
    for (i = 0; keyval > 0; i++)
    {
        if (i == strlen(key_string)) return 0;
        element = key_string[i];
        code = keyval%36;
        keyval /= 36;
        if (code < 10) code += 48;
        else code += 87;
        if (code != buffer[5-i]) return 0;
    }
    return 1;
}
BOOL IsRegistered5(){
    char ss[256];
    GetValueData("Register","Name",ss,sizeof(ss));
    GetValueData("Register","EMail",ss+lstrlen(ss),sizeof(ss)-lstrlen(ss));
    char s[64];
    GetSecondCode2(s);
    if(_test5(s,ss)){
        GetSecondCode();
        return test3(secondcode,ss);
    }
};

#endif
extern "C" DWORD _SDKPROC SCInstallCallBack(UINT16 type, void*, void* p)
{
    switch(type)
    {
        case SC_CB_ONESTEP : // Do One Step
        {
            return __InstallCallBack(&CBdoonestep, p);
        }
    }
    return FALSE;
};

extern "C" BOOL _SDKPROC SCUnInstallCallBack(DWORD d){
    if(callbacks && d){
        C_TYPE i=callbacks->IndexOf((pointer)d);
        if(i>-1){
            callbacks->AtDelete(i);
            TCallBack*c=(TCallBack*)d;
            if(c==*(c->_first))
            { *(c->_first)=c->next;
            }else{
                TCallBack*_c=*(c->_first);
                while(_c){
                    if(_c->next==c){
                        _c->next=_c->next;
                        break;
                    }
                    _c=_c->next;
                }
            }
            if(!callbacks->count){delete callbacks;callbacks=NULL;}
            delete c;
        }
    }
    return FALSE;
};

TSortedCollection *GetClasses(){
    return classes;
};

UINT32 StratumProc(UINT16 code,UINT32 p1,UINT32 p2)
{
    switch(code)
    {
        case SC_PROJECTCLOSE:
        {
#ifndef RUNTIME
            if(winmanager!=NULL)
            {
                for(C_TYPE i=0;i<winmanager->allmdi->count;i++)
                {
                    TMDIChildWindow*w=(TMDIChildWindow*)winmanager->allmdi->At(i);
                    if(w->MODE==MODE_EDITOR2D)
                    {
                        TSchemeWindow*_w=(TSchemeWindow*)w->client;
                        if(_w)
                        {
                            _w->ProjectClose((TProject*)p1);
                        }
                    }
                }
            }
#endif
        };break;
        case SC_PROJECTRESET:
            break;

        case SC_OBJECTSCHANGED:
        {
            winmanager->UpdateMSG();
#ifndef RUNTIME
            for(C_TYPE i = 0; i < classes->count; i++)
            {
                PClassListInfo pcli = (PClassListInfo)classes->At(i);
                if (pcli->flags & CLF_LOADED)
                {
                    PClass _class = pcli->_class;
                    if (_class->scheme && _class->scheme->window)
                    {
                        _class->scheme->window->UpdateObject();
                    }
                }
            }
#endif
            changecount++;
            TProject* prj = projects;
            while(prj)
            {
                prj->ResetEq();
                prj->UpdateWatches();
                prj = prj->next;
            }
        }break; // SC_OBJECTSCHANGED
    }
    dllmanager->ForEach(SC_PROJECTCLOSE,p1,p2);
    return 0;
}

void Decode(char*);
BOOL UpdateSystemVars(){
    char s[256]="";
    sysflags&=~2;
    if(SCGetScVarS("library_password",s)){
        Decode(s);
        if (!lstrcmp(s,"SC-3.097acess")) sysflags|=2;
    }
    return TRUE;
};
BOOL RGBToStr(COLORREF c,char*text){
    int a=c>>24;
    int r=GetRValue(c),g=GetGValue(c),b=GetBValue(c);
    if(a){
        switch(a){
            case 1:lstrcpy(text,"Transparent");break;
            case 2:wsprintf(text,"SysColor(%d)",r);break;
            default: wsprintf(text,"RgbEx(%d,%d,%d,%d)",a,r,g,b);
        }
    }else{
        wsprintf(text,"rgb(%d,%d,%d)",r,g,b);
    }
    return TRUE;
};
BOOL StrToRGB(char*s, COLORREF& color, COLORREF* realcolor)
{
    char text[256];
    lstrcpyn(text,s,sizeof(text));
    AnsiUpper(text);
    char *ps=text;
    while (*ps && *ps==' ')ps++;
    color=RGB(0,0,0);
    int a=0,r=0,g=0,b=0;

    if(*ps=='R')
    {
        if (strstr(ps,"RGB(")==ps)
        {
            if(sscanf(ps,"RGB(%d,%d,%d)",&r,&g,&b))
            {
                color=RGB(r,g,b);
                if(realcolor) *realcolor=color;
                return TRUE;
            }
        }
        else
        {
            if (strstr(ps,"RGBEX(")==ps){
                if(sscanf(ps,"RGBEX(%d,%d,%d,%d)",&a,&r,&g,&b)){
                    COLORREF rgb=RGB(r,g,b)|(a<<24);
                    color=rgb;
                    if(realcolor){
                        switch(a){
                            case 0:*realcolor=color;break;
                            case 1:*realcolor=RGB(255,255,255);break;
                            case 2:*realcolor=GetSysColor((BYTE)rgb);break;
                        }
                    }

                    return TRUE;
                }}
        }}else{
        if(!lstrcmpi(ps,"TRANSPARENT")){if(realcolor)*realcolor=RGB(255,255,255);color= 0x01000000L;return TRUE;}
        if (strstr(ps,"SYSCOLOR")){
            sscanf(ps,"SYSCOLOR(%d)",&r);
            if(realcolor) *realcolor=GetSysColor(r);
            color= r|0x02000000L;
            return TRUE;
        }
    }
    if(realcolor)*realcolor=RGB(255,255,255);
    return FALSE;
};
BOOL ScGetKeyState(int key);
PLinkInfo TClass::GetLinkByHH(HOBJ2D id1,HOBJ2D id2,INT16*num){
    for(INT16 i=0;i<linkcount;i++){
        PLinkInfo pl=link+i;
        if ((pl->obj1Id==id1 && pl->obj2Id==id2)||
            (pl->obj1Id==id2 && pl->obj2Id==id1)){
            if(num)*num=i;
            return pl;
        } }
    return NULL;
}

void Recompile(PLibrary lib)
{
    SetStatusIcon(STATUS_OPERATION,5);
    C_TYPE count=0;

    for(C_TYPE i = 0; i < classes->count; i++)
    {
        if(lib->IsContain((PClassListInfo)classes->At(i)))
            count++;
    }
    if(count)
    {
        C_TYPE j=0;
        for(i=0;i<classes->count;i++)
        {
            PClassListInfo pcli=(PClassListInfo)classes->At(i);
            if(lib->IsContain(pcli))
            {
                BOOL rez=TRUE;
                PClass _class=(PClass)pcli->GetClass();

                if(!_class->Protected())
                {
                    char*txt= GetClassText(_class);
                    if(txt)
                    {
                        SCSetStatusText(1,_class->name);
                        rez=SetClassText(_class,txt);
                        delete txt;
                    }
                }
                j++;
                long p=1000.0*((float)j)/(float)count;
                SCSetStatusProgressBar(0,p);
                if(ScGetKeyState(VK_ESCAPE))
                    break;
                if(!rez)
                {
                    MSGBOX(mainwindow->HWindow,_class->name,_MSG_ERROR, MB_ICONWARNING|MB_OK);
                    break;
                }
            }
        }
        SCSetStatusProgressBar(0,1001);
    }
    SetStatusIcon(STATUS_OPERATION,0);
}

TLibrary *GetDefaultLib()
{
    if(DefaultLib)return DefaultLib;
    if(project && project->libs->count){
        DefaultLib=(TLibrary*)project->libs->At(0);
    }
    if(!DefaultLib){
        DefaultLib=(TLibrary*)librarys->items->At(0);
    }
    return DefaultLib;
};

BOOL _DoRenamePaths(PLibrary lib,char*from,char*to)
{
    if((lib->items==NULL)||(lib->items->count==0))return FALSE;
    for(C_TYPE i=0;i<lib->items->count;i++)
    {
        PLibrary _lib=(PLibrary)lib->items->At(i);
        if(_lib->flags&CLF_LIBRARY)
        {
            _DoRenamePaths(_lib,from,to);
        }else{
            PClassListInfo pcli=(PClassListInfo)_lib;
            PClass _class=pcli->GetClass();
            if (_class){
                char s[256];
                __OPENSCHEME(_class);
                HOBJ2D tool=0;
                while ((tool=GetNextTool2d(hsp,DIB2D,tool))!=0)
                {
                    INT16 t=GetToolType2d(hsp,DIB2D,tool);
                    if(t==ttREFTODIB2D)
                    {
                        int fl=lstrlen(from);
                        if(GetRDib2d(hsp,tool,s,sizeof(s)) && lstrlen(s)>=fl)
                        {
                            if(!strncmpi(s,from,fl))
                            {
                                char s1[256];
                                lstrcpy(s1,to);
                                lstrcat(s1,s+fl);
                                SetRDib2d(hsp,tool,s1);
                            }
                        }
                    }
                }
                __CloseClassScheme(_class,__close);
            }
        }
    }
    return TRUE;
}
void DoRenamePaths(HWND hwnd,PLibrary lib)
{
    char from[256]="",to[256]="";
    if (InputBox(hwnd,"1","_1",from)
        && InputBox(hwnd,"2","_2",to)
        )
    {
        _DoRenamePaths(lib,from,to);
    }
};
UINT32 changecount=0;
PClass currentclass=0;

