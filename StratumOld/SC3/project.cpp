#define FILE_ITERATOR
#include <mem.h>
#include "_class.h"

#include "_message.h"

#include "matrix_h.h"
#ifndef WIN32
#include <dos.h>
#include <dir.h>
#endif
#include <mem.h>
#include "twindow.h"
#include "scvars.h"
#include "winman.h"
#include "stream_m.h"
#ifdef DBE
#include "dbengine.h"
#endif
#include "matrix.h"
#include "dynodata.h"
#include "watch.h"
#include <string.h>
#include "strings.rh"
#include "meta.h"
#include "meta_c.h"

#ifdef PROTECT
#include "password.h"
#endif
#include "sc3.rh"
#include "registry.h"
#include "../BC/bc_include_stratum/STREAM.H"

HSP2D Vm_OpenClassScheme(char * classname,UINT32 /*flags*/);

THyperAdd::THyperAdd(char*s)
{
    char * keywords[]={"readonly","scheme:","state:","open:","stop:","save:","hidewindows"};
    char * params[]={"run","edit","close","stop","prompt","no","return"};
    memset(this,0,sizeof(THyperAdd));
    save=1;
    char *_ps=s;
    while(*_ps)
    {
        if(*_ps != ' ')
        {
            if(*_ps=='/')
            {
                _ps++;
                for(int keyWordIndex=0;keyWordIndex<sizeof(keywords)/4;keyWordIndex++)
                {
                    int keyWordLen=lstrlen(keywords[keyWordIndex]);
                    if(!strncmpi(_ps,keywords[keyWordIndex],keyWordLen))
                    {
                        _ps+=keyWordLen;
                        switch(keyWordIndex)
                        {
                            case 0:readonly=1;break;
                            case 6:hidewindows=1;break;
                            case 1:
                            case 2:{
                                char *st=_ps;
                                char *en=_ps;
                                if(*st=='"')
                                {
                                    while (*_ps && *_ps!='"')
                                        _ps++;
                                    en=_ps-1;
                                }else{
                                    while (*_ps && *_ps!=' ')_ps++;
                                    en=_ps;
                                }
                                char ss[256];
                                lstrcpyn(ss,st,(en-st)+1);
                                if(keyWordIndex==1)defscheme=NewStr(ss);else state=NewStr(ss);
                            }break;

                            default:
                            {
                                BOOL r=FALSE;
                                do
                                {
                                    while (*_ps && *_ps==' ')
                                        _ps++;

                                    for(int argIndex=0;argIndex<sizeof(params)/4;argIndex++)
                                    {
                                        int l=lstrlen(params[argIndex]);
                                        if(!strncmpi(_ps,params[argIndex],l))
                                        {
                                            _ps+=l;
                                            switch(keyWordIndex)
                                            {
                                                case 3:// open
                                                {
                                                    switch(argIndex){
                                                        case 0:autorun=1;break;
                                                        case 1:autoedit=1;break;
                                                        case 3:autorun=2;break;
                                                    }
                                                }break;
                                                case 4:// stop
                                                {
                                                    switch(argIndex){
                                                        case 1:editonclose=1;break;
                                                        case 6:
                                                        case 2:autoclose=1;break;
                                                    }
                                                }break;
                                                case 5:// save
                                                {
                                                    switch(argIndex){
                                                        case 4:save=2;break;
                                                        case 5:save=0;break;
                                                    }
                                                }break;
                                            }
                                            break;
                                        }
                                    }
                                    while (*_ps && *_ps==' ')_ps++;
                                    r=(*_ps==',');
                                    if(r)_ps++;
                                }while(r);
                            }
                        }// endcase
                        break;
                    }
                }
            }else return;
        }else _ps++;
    }
};
THyperAdd::~THyperAdd(){
    if(state)delete state;
    if(defscheme)delete defscheme;
};
void THyperAdd::Generate(char*s){
    s=0;
}


void TProject::OnOpenScheme()
{
    if(hyperadd)
    {
        char *ps=_class->name;
        if(hyperadd->defscheme)
            ps=hyperadd->defscheme;
        Vm_OpenClassScheme(ps,0);
    }
};

void TProject::OnLoad()
{
    if(hyperadd)
    {
        if(hyperadd->hidewindows)
            winmanager->HideProjectWindows();

        if(hyperadd->autoedit)
            OnOpenScheme();

        switch(hyperadd->autorun)
        {
            case 1:mainwindow->EvCommand(CM_RUN,0,0);break;
            case 2:mainwindow->EvCommand(CM_PAUSE,0,0);break;
        }
    }
};

void TProject::OnStop()
{
    if(hyperadd && (nohandleunload==0))
    {
        if(hyperadd->autoclose)
        {
            // mainwindow->EvCommand(CM_PREVPAGE,0,0);
            if(meta)
                meta->DoUndo(ME_UNLOADPROJECT);
            else
                CloseProject(this);
        }else{
            if(hyperadd->editonclose)OnOpenScheme();
        }
    }
};


BOOL CopyDirStruct(char *path,PLibrary lib){
    for(C_TYPE i=0;i<lib->items->count;i++){
        PLibrary _lib=(PLibrary )lib->items->At(i);
        if(_lib->flags&CLF_LIBRARY){
            char s[260];
            lstrcpy(s,path);
            AddSlash(s);
            lstrcat(s,_lib->path);
            if((!MakeDirectory(s))|| (!CopyDirStruct(s,_lib)))return FALSE;
        }
    }
    return TRUE;
}

BOOL CopyDirEntry(char *path,PLibrary lib)
{
    char s[260];
    lib->GetPath(s);
    TFileIterator fcls(s,"*.*");
    while (fcls.GetNext()){
        char *f=fcls.GetFileName();
        if(*f=='.')continue;
        char *ps=f;
        char *pp=ps;
        while (*ps){if(*ps=='.')pp=ps+1;ps++;}
        if(lstrcmpi(pp,"bak")&&lstrcmpi(pp,"cls")
           &&lstrcmpi(pp,"_cl")&&lstrcmpi(pp,"del")){
            char _old[260],_new[260];
            fcls.GetFullFileName(_old);
            lstrcpy(_new,path);
            AddSlash(_new);lstrcat(_new,f);
            if(!ScCopyFile(_old,_new))return FALSE;
        }
    } // Закончили переброску файлов в текущем каталоге
    for(C_TYPE i=0;i<lib->items->count;i++){
        PLibrary _lib=(PLibrary )lib->items->At(i);
        if(_lib->flags&CLF_LIBRARY){
            char s[260];
            lstrcpy(s,path);
            AddSlash(s);
            lstrcat(s,_lib->path);
            if(!CopyDirEntry(s,_lib))return FALSE;
        }}
    return TRUE;
}

BOOL SaveProjectAsEx(TProject*project,char*path)
{};

BOOL SaveProjectAs(TProject*project,char*path)
{
    TWaitCursor wait(NULL);
    MakeDirectory(path);
    SaveAllModifyClasses(TRUE);
    char s[999];
    TLibrary*lib=(TLibrary*)project->libs->At(0);
    char *oldpath=lib->path;
    //Создаем структуру каталогов
    if(!CopyDirStruct(path,lib))return FALSE;
    // Создали структуру каталогов
    // Теперь копируем имиджи
    for(C_TYPE i=0;i<classes->count;i++)
    {
        PClassListInfo pcli=(PClassListInfo)classes->At(i);
        if(!lib->IsContain(pcli))continue;
        // Только для имиджей текущего проекта
        char _old[999],_new[999];
        pcli->GetFileName(_old,FALSE);// Старое имя файла
        PClass _class=pcli->GetClass();
        lib->path=path;
        if (_class->t_flags&TCF_WASSAVE)
        {
            _class->t_flags&=~TCF_WASSAVE;
            pcli->GetFileName(_new,TRUE);
            _class->t_flags|=TCF_WASSAVE;
        }
        else pcli->GetFileName(_new,TRUE);
        lib->path=oldpath;
        if(!ScCopyFile(_old,_new))return FALSE;
    }
    // Скопировали все имиджи (потом нужно удалить все *._cl файлы)
    // Копируем все лежащие в проекте файлы
    if(!CopyDirEntry(path,lib))return FALSE;
    // Записываем состояние и проект
    char *_ppath=project->path;
    char *_pfile=project->file;
    project->path=path;
    char pfile[260];
    lstrcpy(pfile,path);
    AddSlash(pfile);
    lstrcat(pfile,_pfile+lstrlen(_ppath)+1);
    project->file=pfile;
    BOOL newproject=project->newproject;
    project->newproject=FALSE;
    BOOL vm=project->IsVarModifyed();
    BOOL pm=project->IsModifyed();
    if(SaveSetState(project->object,NULL) && project->Store())
    {
        project->path=NewStr(project->path);
        project->file=NewStr(project->file);
        delete _ppath;delete _pfile;
        AddRecentlyProject(project->file);
        project->SetModify(FALSE);
        project->VarChange(FALSE);
        delete lib->path;
        lib->path=NewStr(path);
        UpdateClassWindows(UW_MODIFY,UW_LIBRARY,lib,librarys);
        SetMainTitle(project->file);
    }else{
        project->path=_ppath;
        project->file=_pfile;
        project->SetModify(pm);
        project->VarChange(vm);
        project->newproject=newproject;
        return FALSE;
    }

    // Удаляем все *._cl файлы
    for(i=0;i<classes->count;i++)
    {
        PClassListInfo pcli=(PClassListInfo)classes->At(i);
        if(!lib->IsContain(pcli))continue;
        PClass _class=pcli->GetClass();
        if (_class->t_flags&TCF_WASSAVE)
        {
            char _old[260];
            pcli->GetFileName(_old,FALSE);//Имя _cl файла
            ScDeleteFile(_old);
            _class->t_flags&=~TCF_WASSAVE;
        }
    }
    return TRUE;
}

BOOL TWatch::Update(TProject*)
{
    object = SCGetObjectByFullName(project,path,count);
    if(object==NULL)
        return FALSE;
    UpdateVarIndex();
    return (index>-1);
};

void TWatch::UpdateVarIndex()
{
    index = object->GetVarIdByName(name);
}

BOOL IsURL(char*file);

void GetFullFileNameEx(TProject*prj,char*src,char*dst)
{
    if(src==NULL || (lstrlen(src)==0))
    {
        *dst=0;
        return;
    }
    if(IsURL(src)) { lstrcpy(dst,src);return; }
    int code=IsAbsolutePath(src);
    if(code==1){lstrcpy(dst,src);return;}
    if(src[0]=='(')
    {
        char * ps=strstr(src,")");
        if(ps)
        {
            char ss[256];
            lstrcpyn(ss,src+1,ps-(src));
            src=ps+1;
            if(!lstrcmpi(ss,"$project"))				{lstrcpy(dst,prj->path);goto m1;}
            if(prj && !lstrcmpi(ss,"$stratum"))	{lstrcpy(dst,sc_path);goto m1;}
            if(!lstrcmpi(ss,"$libs"))						{lstrcpy(dst,sc_path);lstrcat(dst,"\\library");goto m1;}
            if(!lstrcmpi(ss,"$addlibs"))				{lstrcpy(dst,sc_path);lstrcat(dst,"\\add.lib");goto m1;}
            if(!lstrcmpi(ss,"$plugins"))				{lstrcpy(dst,sc_path);lstrcat(dst,"\\plugins");goto m1;}
            if(!lstrcmpi(ss,"$windows"))				{GetWindowsDirectory(dst,256);goto m1;}
            if(!lstrcmpi(ss,"$temp"))						{GetTempPath(256,dst);goto m1;}
            if(!lstrcmpi(ss,"$system"))					{GetSystemDirectory(dst,256);goto m1;}
            if(!lstrcmpi(ss,"$empty"))					{lstrcpy(dst,"");goto m1;}
            if(!lstrcmpi(ss,"$cdrom"))
            {
                lstrcpy(dst,"");
                char buf[4096];
                if(GetLogicalDriveStrings(sizeof(buf),buf))
                {
                    char *_ps=buf;
                    while(*_ps)
                    {
                        int l=lstrlen(_ps);
                        if(GetDriveType(_ps)==DRIVE_CDROM )
                        {
                            lstrcpy(dst,_ps);
                            break;
                        }
                        _ps+=(l+1);
                    }
                }
                goto m1;
            }
            if(!strncmpi(ss,"$reg:",5))
            {
                char s[256];
                lstrcpy(s,ss+5);
                char*_ps=s;
                char*_pps=NULL;
                while(*_ps)
                {
                    if(*_pps=='\\')_pps=_ps;
                    _ps++;
                }
                if(_pps)
                {
                    *_pps=0;
                    _pps++;
                    GetValueData(s,_pps,dst,sizeof(dst));
                }
                else
                    GetValueData("data",s,dst,sizeof(dst));
                goto m1;
            }
        }
    }

    if(prj)
    {
        if(prj->realname)
            GetPath(prj->realname,dst);
        else
            lstrcpy(dst,prj->path);
    }
    else
        lstrcpy(dst,sc_path);
m1:
    while(src[0]=='.' && src[1]=='.' && src[2]=='\\')
    {
        char _path[256];
        GetPath(dst,_path);
        lstrcpy(dst,_path);
        src+=3;
    };

    AddSlash(dst);
    if(*src=='\\')src++;
    lstrcat(dst,src);
};

void TProject::GetFullFileName(char* src, char* dst, BOOL useURL)
{
    if(useURL && realname && IsURL(realname))
    {
        if(IsURL(src))
        {
            lstrcpy(dst,src);
            return;
        }
        char path[MAX_PATH];
        GetPath(src,path);
        GetAbsoluteName(path,src,dst);
    }
    if(useURL && realname)
    {
        char*_realname=realname;
        realname=0;
        GetFullFileNameEx(this,src,dst);
        realname=_realname;
        TDOSStream st(dst,TDOSStream::stOpenRead|TDOSStream::stNoWarning);
        if(st.status==0)return;
        GetFullFileNameEx(this,src,dst);
    }
    else
        GetFullFileNameEx(this,src,dst);
};

TProject::TProject()
{
    memset(this,0,sizeof(TProject));
};
#ifndef RUNTIME
extern char *user_id_names[];

TProject::TProject(char*_file,char* _name)
{
    memset(this,0,sizeof(TProject));
    libs=new TCollection(5,5);
    modifyed = 1;
    char _path[256];
    GetPath(_file,_path);
    file = NewStr(_file);
    path = NewStr(_path);
    newproject=1;
    TLibrary *lib = AddDirPath(path,CLF_PROJECT);
    DefaultLib=lib;
    libs->Insert(lib);

    char s[256];
    lstrcpy(s,_name);
    if(GetClassListInfo(s))
        GetUniqueClassName(s);

    _class=CreateNewClass(s,NULL,NULL,NULL);
    CreateDefClassScheme(s);
    PClassListInfo pcli=_class->GetClassListInfo();
    pcli->flags|=CLF_PROJECT;
    if(SCGetScVar("user_copy",1)){
        for(int i=0;i<6;i++){
            ::SCGetScVarS(user_id_names[i],s);
            if(lstrlen(s))SetVarS(user_id_names[i],s);
        }
    }

    if(Init())_Message(_M_PRJCREATE);
};
#endif // runtime

//получение значения переменной по имени из загруженных ранее переменных
INT32 TProject::GetVarDef(char*n,INT32 d){
    if(vars){
        return vars->GetVarDef(n,d);
    }
    return d;
}

void TProject::GetPrloadFile(char*__s)
{
    lstrcpy(__s, path);
    AddSlash(__s);
    if(!GetVarS("preload_file", __s + lstrlen(__s)))
        lstrcat(__s,"_preload.stt");
}

BOOL  TProject::GetVarS(char*n,char*d)
{
    if(vars){
        return vars->GetVarS(n,d);
    }
    *d=0;
    return FALSE;
}

void  TProject::SetVarS(char*n,char*d)
{
    if(vars){
        if(d==NULL || *d==0){
            vars->DeleteVar(n);
            VarChange();
            return;
        }
    }else{
        vars=new TScVarManager();
    }
    vars->SetVarS(n,d);
    SetModify();
}

void TProject::SetVarDef(char*n,INT32 data,INT32 def){
    if(vars){
        if(data == def){
            vars->DeleteVar(n);
            VarChange();
            return;
        }
    }else{
        if(data ==def) return;
        vars=new TScVarManager();
    }
    vars->SetVarI(n,data);
    SetModify();
};
void TProject::SetVarI(char*n,INT32 data){
    if(!vars){
        vars=new TScVarManager();
    }
    vars->SetVarI(n,data);
    SetModify();
};


double TProject::GetVarD(char*n,double def){
    double d=def;
    if(vars){
        if(vars->GetVarD(n,d));
    }
    return d;
};
void TProject::SetVarD(char*n,double d,double def){
    if(!vars){
        if(d==def) return;
        vars=new TScVarManager();
    }
    if(d==def){vars->DeleteVar(n);SetModify();return;}
    vars->SetVarD(n,d);
    SetModify();return;

};

void TProject::DeleteVar(char*n){
    if(vars){
        vars->DeleteVar(n);
        SetModify();
    }
};
void GetSecondCode();

TProject::TProject(TDOSStream& st)
{
    TWaitCursor wait(NULL);
    memset(this,0,sizeof(TProject));
#ifdef PROTECT
    GetSecondCode();
    //if(!test3(secondcode))return;
    secondcode[2]='Y';
#endif
    libs = new TCollection(5,5);
    file = NewStr(st.GetFileName());
    char _path[256];
    GetPath(file,_path);
    path = NewStr(_path);

    INT16 code;
    while((code=st.ReadWord())!=0)
    {
        switch(code)
        {
            case PR_PASSWORD:
            {
                char ps[256];
                st.ReadStr(ps);
                char ac[256];
                MessageBox(0,"Пароль","Пароль",MB_OK);
                /*  if (InputBox(0,"Пароль","Введите пароль...",ac,sizeof(ac))) */
                if (lstrcmp(ac,ps))
                {
                    status=-2;
                    return;
                };
            }break;
            case PR_MAINCLASS:
            {
                char obj[256];
                st.ReadStr(obj);

                project = this;

                //создается коллекция в _adddir_c
                BeginChkAddDir();

                BYTE _flags=CLF_PROJECT;
                if(GetVarDef("lib_norecurse",0))
                    _flags|=CLF_NORECURSE;

                //Библиотека проекта
                TLibrary *lib = AddDirPath(path,_flags);
                DefaultLib=lib;
                libs->Insert(lib);

                //Инициализация библиотек добавленных пользователем
                char addlib[256];
                if(GetVarS("addlib",addlib))
                {
                    char *_a=addlib;
                    char *ps;
                    do
                    {
                        ps=strstr(_a,";");
                        char s[256];
                        if(ps)
                        {
                            lstrcpyn(s,_a,(ps-_a)+1);
                            _a=ps+1;
                        }
                        else
                            lstrcpy(s,_a);
                        char temp[256];
                        if(lstrlen(s))
                        {
                            GetFullFileName(s,temp);
                            TLibrary *_lib = AddDirPath(temp,CLF_PROJECT);
                            libs->Insert(_lib);
                            //UpdateClassWindows(UW_CREATE,UW_LIBRARY,_lib,librarys);
                        }
                    }while(ps);
                }

                //удаляется коллекция _adddir_c
                /**/EndChkAddDir();
                _class = GetClassByName(obj);
                if(Init())
                    _Message(_M_PRJLOAD);
                else
                    return;
            }break;

            case PR_VARS:
            {
                vars=new TScVarManager();
                vars->Load(st);
            }break;

            case PR_WATCH:
            {
                watch=new TCollection(10,10);
                C_TYPE count=st.ReadWord();
                for(C_TYPE i=0;i<count;i++)
                {
                    INT16 pc=st.ReadWord();
                    int size=sizeof(TWatch)-2+2*pc; // было pc
                    TWatch *w = (TWatch*)new BYTE [size];
                    memset(w,0,size);
                    st.Read(w->path,pc*2);
                    w->count=pc;
                    w->name=st.ReadStr();
                    w->info=st.ReadStr();
                    watch->Insert(w);
                }
                UpdateWatches();
            }break;
        }
    }
    // <Пытаемся определить возможность работы с проектом в режиме редактирования.>
    if(readonly==0)
    {
        {
            TFileIterator fcls(file,NULL);
            if(fcls.GetNext())
            {
                if(fcls.IsReadOnly())
                    readonly=1;
            }
        }
        if(readonly==0)
        {
            // пытаемя создать файл и если не удается то переходим в режим READONLY.
            int d=1;
            BOOL found;
            char s[9999];
            do
            {
                wsprintf(s,"%s\\sctmp%03d.~sp",path,d);
                {
                    TDOSStream _st(s,TDOSStream::stOpenRead|TDOSStream::stNoWarning);
                    found=_st.status==0;
                    d++;
                }
            }while(found && d<100);
            if(!found)
            {
                {
                    TDOSStream _st(s,TDOSStream::stCreate|TDOSStream::stNoWarning);
                    if(_st.status)
                        readonly=1;
                }
                if(!readonly)
                    ScDeleteFile(s);
            }
        }
    }
    // <\Пытаемся определить возможность работы с проектом в режиме редактирования.>
};

BOOL TProject::UpdateWatches()
{
    if(watch)
    {
        C_TYPE i=0;
        while(watch && i<watch->count)
        {
            TWatch* w = (TWatch*)watch->At(i);
            if(w->Update(this))
                i++;
            else
                DeleteWatch(i);
        }
    }
    return TRUE;
}

bool TProject::Init()
{
    if(!_class)
        return false;

    if(projects)
    {
        TProject *_prj=projects;

        while(_prj->next)
            _prj=_prj->next;

        _prj->next = this;
        prev=_prj;
    }
    else
        projects=this;
    project=this;

    object = CreateObjectByClass(_class);
    if (!object)
    {
        status=-1;
        return false;
    }
    else
    {
        UpdateVarsLinks();
        if(object->childObjects && SCGetScVar("vars_preload",1))
        {
            LoadSetState(NULL, NULL);
        };
        object->SetDefault();
        object->SetSettingsVars();
        InitProjectMatrix(path);
#ifdef WIN32
        SetCurrentDirectory(path);
#else
        chdir(path);
#endif
        /*
  for(C_TYPE i=0;i<libs->count;i++){
  TLibrary*lib=(TLibrary*)libs->At(i);
//  UpdateClassWindows(UW_CREATE,UW_LIBRARY,lib,lib->parent);
 }
*/
        if(!(EXEC_FLAGS&EF_STEPACTIVE))
        {
            SetTitle();
        }

        return true;
    }
}

void TProject::SetTitle()
{
    if(realname)
    {
        SetMainTitle(realname);
    }
    else
    {
        if(readonly)
        {
            char s[256];
            lstrcpy(s,file);
            lstrcat(s," R");
            SetMainTitle(s);
        }
        else
            SetMainTitle(file);
    }
}

TProject::~TProject()
{
    if(post)
        delete post;

    nohandleunload=1;
    SetStatusIcon(STATUS_ERROR,0);
    StratumProc(SC_PROJECTCLOSE,(UINT32)this,0);

    if(_class)
        ResetProject();

    if(!readonly)
        SaveMatrix(path);

    matrixs->DeleteAllMatrix(this);
    winmanager->CloseAll(this,TRUE);

    if(next || prev){
        if(next)next->prev=prev;
        if(prev)prev->next=next;
    }else {projects=NULL;
        SCSetStatusText(0,"");
    }
    if (equations) { delete equations; equations=0; }
    if(object)delete object;
    if(file)delete file;
    if(path)delete path;
    if(projecttoactivate)delete projecttoactivate;

    // DoneLib(librarys);
    DefaultLib=(TLibrary*)librarys->items->At(0);
    /*
 INT16 i=0;
 while(i<classes->count){
    PClassListInfo pcli=(PClassListInfo)classes->At(i);
    if (pcli->flags&CLF_CURRENTPROJECT){classes->AtFree(i);}else i++;
 }
*/
    if(libs){
        for(C_TYPE i=0;i<libs->count;i++){
            TLibrary*lib=(TLibrary*)libs->At(i);
            lib->DecRef();
        }
        delete libs;
    }
    if(VarsMemory)delete VarsMemory;
    if(VarsPointers)delete VarsPointers;
    if(vars)delete vars;

    if(dllmanager)dllmanager->FreeDLL((DWORD)this);

    while (watch){
        DeleteWatch(0);
    }

    if(hyperadd)
    {
        if(hyperadd->hidewindows)
            winmanager->ShowProjectWindows(prev);
        delete hyperadd;
    }
    if(tmpfile)
    {
        ScDeleteDir(tmpfile);
        delete tmpfile;
    }
}
/*
 void TProject::_DoneLib(PLibrary pl){
   INT16 i=0;
    while(i<pl->items->count){
         TLibrary*_lib=(TLibrary*)pl->items->At(i);
       if(_lib->flags&CLF_LIBRARY){
          _DoneLib(_lib);

    }else{
     PClassListInfo pcli=(PClassListInfo)_lib;
     pcli->flags|=CLF_CURRENTPROJECT;
    }
     i++;
   }
 };

 void TProject::DoneLib(PLibrary pl){
 INT16 i=0;
 while(i<pl->items->count){
  TLibrary*_lib=(TLibrary*)pl->items->At(i);
     if((_lib->flags&CLF_LIBRARY) && (_lib->project==this)){
      UpdateClassWindows(UW_DELETE,UW_LIBRARY,_lib,_lib->parent);
      _DoneLib(_lib);
      delete _lib;
      pl->items->AtDelete(i);
     }else{
      i++;
     }
  }
 };
*/
double TProject::SetProjectProp(char*,double){

    return 0;
};
double TProject::SetProjectProp(char*v,char*s){
    if(lstrcmpi(v,"activateproject")==0){
        if(projecttoactivate){delete projecttoactivate;projecttoactivate=NULL;}
        if(s && lstrlen(s))projecttoactivate=NewStr(s);
        return 1;
    }
    return 0;
};

double TProject::GetProjectProp(char*v)
{ return 0;	}

BOOL GetProjectPath(char*path)
{
    if(project){lstrcpy(path,project->path);return TRUE;}
    *path=0;
    return FALSE;
};

BOOL TProject::Store(){
#if !defined DEMO
    if(newproject){
        newproject=0;
        AddRecentlyProject(file);
    }

    TDOSStream st(file,TDOSStream::stCreate);
    if (!st.status){
        st.WriteWord(PRJ_SIGNATURE);
        if(vars && vars->count){
            st.WriteWord(PR_VARS);
            vars->Store(st);
        }
        st.WriteWord(PR_MAINCLASS);
        st.WriteStr(_class->name);

        if(watch){
            st.WriteWord(PR_WATCH);
            st.WriteWord(watch->count);
            for(C_TYPE i=0;i<watch->count;i++){
                TWatch*w=(TWatch*)watch->At(i);
                st.WriteWord(w->count);
                st.Write(w->path,w->count*2);
                st.WriteStr(w->name);
                st.WriteStr(w->info);
            }
        }
        st.WriteWord(0);
        //   VarChange(FALSE);
        SetModify(FALSE);
        return (st.status==0);}
#endif
    return FALSE;
};


BOOL TProject::PreModifyVar(PClass _class)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в PreModifyVar(%s)",_class->name);
        LogMessage(s);
    }
#endif

    TMemoryStream *stream=NULL;
    changedclass=_class;
    if (_class->first)
    {
        if (VarsMemory)
            stream = new TMemoryStream(TRUE);//delete memory blocks after done

        if (stream)
        {
            object->StoreState(*stream,STATE_VARS);
            stream->Seek(0);
        }

        if (VarsMemory)
            VarsMemory->Release(0);
        else
            VarsMemory = new TMemoryManager(TMemoryManager::ZEROINIT);

        if (VarsPointers)
            VarsPointers->Release(0);
        else
            VarsPointers = new TMemoryManager(TMemoryManager::ZEROINIT);
    }
    temp = (void*)stream;
    return  TRUE;
}

BOOL TProject::PostModifyVar(PClass _class)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в PostModifyVar(%s)",_class->name);
        LogMessage(s);
    }
#endif
    TMemoryStream *stream=(TMemoryStream *)temp;
    if (object)
    {
        object->ClearVarPointers();
        //  Расставляем связи по новой
        if (VarsMemory){
            TMemoryManager temp;
            //  object->UpdateLinks(temp,*VarsMemory);
            UpdateLinks(object ,temp,*VarsMemory);
        }
        // Читаем старое значение переменных
        if (stream)
        {
            object->LoadState(*stream,STATE_VARS);
            delete stream;
        }
        // root->_CopyState(TObject::OLD_TO_NEW);
        if (!(EXEC_FLAGS&EF_STEPACTIVE))
            object->_CopyState(TObject::OLD_TO_NEW);

        ObjectsChanged();
    }
    temp = NULL;
    return TRUE;
}

BOOL  EQExist(){
    if(project)return project->EQExist();
    return FALSE;
};

void TProject::CloseWindows(){
    winmanager->CloseAll(this,FALSE);
};

void TProject::CloseStreams(){
    streams->CloseAll(this);
}

void TProject::CloseBases(){
#ifdef DBE
    DBCloseAll(this);
#endif
};

void TProject::ClearVariables()
{
    VarsMemory->ClearAll();
    dyno_mem->Done(this);
};

void TProject::VarToDef(BOOL setset)
{
    ClearVariables();

    object->SetDefault();

    if(setset || SCGetScVar("vars_setondef",1))
        object->SetSettingsVars();

    object->_CopyState(TObject::OLD_TO_NEW);
    object->AllEnable();
    UpdateWatchWindow();
}

void TProject::ResetProject()
{
    dllmanager->ForEach(SC_PROJECTRESET, (UINT32)this);

    CloseStreams();
    CloseWindows();
    CloseBases();

    VarToDef(TRUE);

    matrixs->DeleteMinusMatrix(this);

    ResetEq();

    OnStop();
}

void TProject::UpdateWatchWindow()
{
    if(wnd && !watchdisable)
    {
        RECT r;
        SendMessage(wnd,TB_GETCOLLUMN,2,(LPARAM)&r);
        InvalidateRect(wnd,&r,0);
    }
};

BOOL TProject::DoOneStep(PObject _object)
{
    SetLastScError(0);

    if(!_object)
        _object = object;

    if (!_object)
        return 0;

    //было здесь
    EXEC_FLAGS |= EF_STEPACTIVE;

    // No Root object
    // GetAsyncKeyState(VK_ESCAPE);

    _object->CopyState(TObject::OLD_TO_NEW); //Зачем ? там и так осталось от CopyState(TObject::NEW_TO_OLD);
    if(_object == object)
    {
        _object->Compute();

        ChkEqu();
        CalcEQUs();

        _object->CopyState(TObject::NEW_TO_OLD);
    }
    else
    {
        _object->_Compute();
        _object->_CopyState(TObject::NEW_TO_OLD);
    }

    if (EXEC_FLAGS & EF_MUSTSTOP)
    {
        mainwindow->Cm_ClearAll();
        /*
        if(projecttoactivate)
    {
            TProject * _project=GetProject(projecttoactivate);
            if(_project)SetActiveProject(_project);
        }
    */
        return TRUE;
    };

    INT16 error=GetLastScError();
    if (error)
    {
        MSGBOX(0,MAKEINTRESOURCE(IDS_MSG1),_MSG_INFO,MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
        Stop();
        VM_RESET();
    };

    if(VM_Error())
    {
        VM_RESET();
        Stop();
    }

    calcedsteps++;
    EXEC_FLAGS &= ~EF_STEPACTIVE;
    UpdateWatchWindow();
    PostProcess();

    return TRUE;
}

BOOL TProject::ChkEqu()
{
    if (!eqvalid)
    {
        RebuidEQUs();
        eqvalid=1;
    }
    return TRUE;
};

void TProject::EQBuild(PObject _po)
{
    if (_po->_class->equ)
    {
        if (!equations) equations = new Equations(object);
        equations->EQAdd(_po);
    };
    PObject po=_po->childObjects;
    while (po)
    {
        EQBuild(po);
        po=po->next;
    }
};

BOOL TProject::RebuidEQUs()
{
    if (equations) { delete equations; equations = 0; }
    if (object) EQBuild(object);
    if (equations) equations->Initialize();
    return 0;
};

BOOL TProject::CalcEQUs()
{
    if (equations){
        EXEC_FLAGS|=EF_EQUACTIVE;
        equations->Calculate();
        EXEC_FLAGS&=(~EF_EQUACTIVE);
        return TRUE;
    }
    return FALSE;
};

int TProject::UpdateLinks(PClass _c)
{
    if(projects && (projects->next==NULL))
    {
        projects->UpdateVarsLinks(1);
        return 1;
    }
    else
    {
        TProject*prj=projects;
        int i=0;
        while(prj)
        {
            if((_c==NULL) || (prj->object->IsContainClass(_c)))
            {
                prj->UpdateVarsLinks(1);
                i++;
            }
            prj=prj->next;
        }
        return i;
    }
}
UINT32 ulinkerror;

// Выполняет перетасовку переменных, только при изменении переменных при
bool TProject::UpdateLinks(PObject po, TMemoryManager& m, TMemoryManager& mv)
{
    ulinkerror=0;
    po->UpdateLinks(m, mv);

    if(ulinkerror)
        SetStatusIcon(STATUS_ERROR,1);
    else
        SetStatusIcon(STATUS_ERROR,0);
}

bool TProject::UpdateVarsLinks(bool olddone)
{
    if (object)
    {
        SetStatusIcon(STATUS_OPERATION,5);

        TMemoryStream *stream = NULL;
        if (VarsMemory)
        {
            stream = new TMemoryStream(TRUE); //delete memory blocks after done
            object->StoreState(*stream, STATE_FAST);
            stream->Seek(0);
        }

        if(VarsMemory && olddone)
        {
            delete VarsMemory;
            VarsMemory=NULL;
        }

        if(VarsPointers && olddone)
        {
            delete VarsPointers;
            VarsPointers = NULL;
        }

        if (VarsMemory)
            VarsMemory->Release(0);
        else
            VarsMemory = new TMemoryManager(TMemoryManager::ZEROINIT);

        if (VarsPointers)
            VarsPointers->Release(0);
        else
            VarsPointers = new TMemoryManager(TMemoryManager::ZEROINIT);

        TMemoryManager temp;
        object->ClearVarPointers();
        UpdateLinks(object, temp, *VarsMemory);

        if (stream)
        {
            object->LoadState(*stream, STATE_FAST);
            delete stream;
        }

        if (!(EXEC_FLAGS & EF_STEPACTIVE))
            object->_CopyState(TObject::OLD_TO_NEW);

        SetStatusIcon(STATUS_ERROR+1,0);
        SetStatusIcon(STATUS_OPERATION,0);
        return TRUE;
    }
    return FALSE;
};

void  * GetCurrentProject()
{
    return project;
};

BOOL TProject::IsWatched(PObject po,char *var,C_TYPE*index){
    if(watch)
    {
        for(C_TYPE i=0;i<watch->count;i++)
        {
            TWatch *w=(TWatch*)watch->At(i);
            if ((w->object==po)&&(!lstrcmpi(w->name,var)))
            {
                if(index)*index=i;return TRUE;
            }
        }
    }
    if(index)*index=(C_TYPE)-1;
    return FALSE;
};

BOOL TProject::RemoveWatch(PObject po,char *var)
{
    C_TYPE index;
    if(IsWatched(po,var,&index))
    {
        DeleteWatch(index);
        if(wnd)
        {
            SendMessage(wnd,TB_SETRANGEY,0,watch?watch->count:0);
        }
    }
    return FALSE;
};
BOOL _SetObjectVarA(PObject po,INT16 id,char*text);

BOOL TProject::SetWatchText(C_TYPE y,C_TYPE x,char*text)
{
    if(watch && y>=0 && watch->count>y)
    {
        TWatch *w=(TWatch*)watch->At(y);
        PVarInfo  pv=&(w->object->_class->vars[w->index]);
        switch(x)
        {
            case 2:_SetObjectVarA(w->object,w->index,text);
                break;

            case 3:
                if(w->info)
                {
                    delete w->info;w->info=NULL;
                }
                w->info=NewStr(text);
                SetModify();
                break;

            default:
                return FALSE;

        }
    }
    return TRUE;
}

BOOL TProject::GetWatchText(C_TYPE y,C_TYPE x,char*text)
{
    if(watch && y>=0 && watch->count>y)
    {
        TWatch *w=(TWatch*)watch->At(y);
        PVarInfo  pv=&(w->object->_class->vars[w->index]);
        switch(x)
        {
            case 0:lstrcpy(text,pv->name);
                break;

            case 1:
                lstrcpy(text,pv->classType->GetClassName());
                break;

            case 2:
                GetObjectVarA(w->object,w->index,text);
                break;

            case 3:
                if(w->info)
                    lstrcpy(text,w->info);
                else
                    *text=0;
                break;
            default:
                *text=0;
                return FALSE;

        }
        return TRUE;
    }
    return FALSE;
}

BOOL TProject::AddWatch(PObject po,char *var)
{
    // PObject _po=po;
    if(IsWatched(po,var))
        return FALSE;

    INT16 pcount = SCGetFullObjectName(po,0,0);

    int size = (sizeof(TWatch)-2) + pcount*2;
    TWatch *w = (TWatch*)new BYTE[size];
    memset(w,0,size);

    w->object = po;
    w->name=NewStr(var);
    w->count=pcount;
    w->UpdateVarIndex();
    //_po=po;

    SCGetFullObjectName(po,w->path,pcount);

    w->info=NewStr(po->_class->vars[w->index].info);
    if(!watch)
    {
        watch=new TCollection(10,10);
    }
    watch->Insert(w);
    if(wnd)
        SendMessage(wnd,TB_SETRANGEY,0,watch->count);
    SetModify();
    return TRUE;
};

#ifdef PROTECT
BOOL _CanSave(TCollection *lib,int& c)
{
    for(C_TYPE i=0;i<lib->count;i++)
    {
        TLibrary*_lib=(TLibrary*)lib->At(i);
        if(_lib->flags&CLF_LIBRARY)_CanSave(_lib->items,c);
        else c++;
    }
};

BOOL TProject::CanSaveProject()
{
    if(realname)return FALSE;
    int c=0;
    _CanSave(libs,c);
    c*=2;
    c+=5;
    return c<16;
};
#endif

void TProject::DeleteWatch(C_TYPE i){
    if(watch && watch->count){
        SetModify();
        TWatch *w=(TWatch*)watch->At(i);
        if(w->name)delete w->name;
        if(w->info)delete w->info;
        delete w;
        watch->AtDelete(i);
        if(watch->count==0){delete watch;watch=NULL;}
        if(wnd){
            SendMessage(wnd,TB_SETRANGEY,0,watch?watch->count:0);
        }
    }
};

TProject* TObject::GetProject()
{
    PObject po=this;
    while(po->parent)po=po->parent;
    TProject * prj=projects;
    while(prj)
    {
        if(prj->object==po)return prj;
        prj=prj->next;
    }
    return NULL;
};

HOBJ2D ReplaceChild(PClass _class,HOBJ2D id,char * name){ // Заменяет чилда на нового
    if(!_class)return 0;
    INT16 num = _class->GetChildById(id);
    if (num>-1){
        PClass _c=GetClassByName(name);
        if (_c){
            for(INT16 i=0;i<_class->linkcount;i++){
                if(_class->link[i].obj1Id==id)_class->link[i].obj1Id=(HOBJ2D)0xffffu;
                if(_class->link[i].obj2Id==id)_class->link[i].obj2Id=(HOBJ2D)0xffffu;
            }
            gr_float x,y;
            x=_class->childs[num].x;
            y=_class->childs[num].y;
            TCollection items(10,10);
            PObject _po=_class->first;
            while (_po){
                PObject so=_po->_GetObject(id);
                items.Insert(so->varData);
                TProject *_prj=so->GetProject();
                if(_prj)_prj->VarChange();
                so->varData=NULL;
                _po=_po->class_next;
            }
            RemoveChildren(_class,id,1,0);
            //PClass _class,char* name,HOBJ2D*id,gr_float xpos,gr_float ypos
            HOBJ2D _id=0;
            AddChildren(_class,name,&_id,x,y);
            for(i=0;i<_class->linkcount;i++){
                if(_class->link[i].obj1Id==(HOBJ2D)0xffffu)_class->link[i].obj1Id=_id;
                if(_class->link[i].obj2Id==(HOBJ2D)0xffffu)_class->link[i].obj2Id=_id;
            }
            if (_class->first){
                PObject _po=_class->first;
                i=0;
                while (_po){
                    PObject so=_po->_GetObject(_id);
                    so->varData=(TSetVar*)items.At(i);
                    items.AtPut(i,0);
                    //      so->SetDefault();
                    _po=_po->class_next;
                    i++;
                }
                TProject::UpdateLinks(_class);
                ObjectsChanged();
            }
            //??
            _class->SetObjectOrder(_id,num);
            return _id;
        } } return 0;
};

void TProject::AddPostprocess(int code,PClass _cl,void*data)
{
    TPostProcess* p = new TPostProcess();
    p->code=code;
    p->_class=_cl;
    p->data=data;
    p->next=post;
    post=p;
};

void TProject::PostProcess()
{
    if(post && (!(EXEC_FLAGS & EF_STEPACTIVE)))
    {
        TPostProcess* _p = post;
        while(_p)
        {
            switch(_p->code)
            {
                case PPF_UNLOADPROJECT:
                {
                    if(CloseProject(this))
                        return;
                }break;
            }
            _p=_p->next;
        }
        delete post;
        post = NULL;
    }
}

int code;
PClass *_class;
void * data;
TPostProcess*next;

TPostProcess::TPostProcess()
{
    memset(this,0,sizeof(TPostProcess));
};
TPostProcess::~TPostProcess()
{
    if(data)delete data;
    if(next)delete next;
};

