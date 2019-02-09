/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            twindow.cpp
Author:
            Alexander Shelemekhov
*/
#ifdef  WIN32
#undef _WINREG_
#endif
#include <stdlib.h>

#include "stream.h"
#include "twindow.h"
#include <commdlg.h>
#include "treeview.h"
#include <dir.h>
#include <mem.h>
#include "sc3.rh"
#include "class.h"
#include "dialogs.h"
#include "systab.h"
#include "browser.h"
#include "messages.h"
#include "winman.h"
#include "stream_m.h"
#include "compiler.h"
#include "tscheme.h"
#include "tcntbar.h"
#include "sc_ctl3.h"
#ifdef DBE
#include "dbengine.h"
#endif
#include "video\scvideo.h"
#include "tree_itm.h"
#include "matrix.h"
#include "ballon.h"
#include "varinfo.h"
#include <shellapi.h>
#include "statusba.h"
#include <cderr.h>
#include "equ.h"
#include "project.h"
#include "dllman.h"
#include "vmachine.h"
#include "grtools.h"
#include <stratum\stratum.h>
#ifdef CAN_EDIT_BITMAP
#include "bmpwnd.h"
#endif
#include "msgcrk.h"
#include <dos.h>
#include "menu.h"
#include "filetype.h"
#include "watch.h"
#include "tpopup.h"
#include "menu.h"
#include "msgh.h"
#include "idehelp.h"
#include "password.h"
#ifdef CHECKVERSIONPROJECT
#include "scbuild.h"
#endif

#ifdef CHECKUSERKEY
#include "userkey.h"
#endif
#include "dynodata.h"
#include "winuser.h"


#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

#define long_to_point (INT16)LOWORD(lParam),(INT16)HIWORD(lParam)

#define DECLARE_WNDPROC(proc,winclass)\
    LRESULT CALLBACK _export \
    proc( HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam) \
{winclass* win=(winclass*)GetWindowLong(hwnd,0); \
    if (message==WM_CREATE){win=new winclass((LPCREATESTRUCT)lParam,hwnd);\
    return win->DefWindowProc(message,wParam,lParam);\
    }\
    LRESULT result=win?win->_WndProc(message,wParam,lParam):\
    DefWindowProc(hwnd,message,wParam,lParam); \
    if (message==WM_DESTROY){\
    if (win) delete win;SetWindowLong(hwnd,0,0l);}; \
    return result;\
    };



#define MM_WOM_OPEN         0x3BB           /* waveform output */
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD

#ifdef RUNTIME
#define FIRST_RECENTLY 9
#else
#define FIRST_RECENTLY 14
#endif

#ifdef NODLLRESOURCE
#define sc_GLOBALCLASS 0
#else
#define sc_GLOBALCLASS CS_GLOBALCLASS
#endif

#include "\sc3_add\syntax\syntax.h"
#include "scvars.h"
#include <stratum\sccall.h>
#include <stratum\sc_types.h>
#include <stratum\sc_msg.h>
#include "strings.rh"

#ifdef PROTECT
BOOL Register();
#endif

BYTE extern off97;
char extern * ini_file;
extern HINSTANCE hInstance;
char filesavetype[5]; //Тип записываемого файла
extern HWND title;

#ifndef RUNTIME
int __GetTreeItemInfo(HWND tree,DWORD data,int update=0);
int __EditTreeItem(HWND tree,DWORD data,BOOL newitem=FALSE);
int __UpdateClassWindows(HWND tree,UPDATESTRUCT & us);
int __TreeItemMenu(HWND tree,DWORD data);
#endif

int strpos(char*str,char*fragment);

#if !defined(WIN32)
HICON ICON_EDITO2D,ICON_TEXT,ICON_BMP,ICON_TREE,ICON_LIB,ICON_MESSAGE;
#endif

HICON ICON_MAIN,ICON_FOLDER;

class TErrorHandler:public TCollection
{
    void InsertInWind(int i);
public:
    HWND hwnd;
    TErrorHandler();
    void AllRemove();
    void InsertMSG(char*);
    void MakeWindow(HWND hwnd);
    virtual void  FreeItem(pointer );
};


class TMessageMsgServer:public TWindow
{
public:
    TMessageMsgServer(LPCREATESTRUCT,HWND h):TWindow(0,0){HWindow=h;};
    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    LRESULT WndProc(UINT message, WPARAM/* wParam*/,LPARAM /*lParam*/)
    {
        return 0;
    };// WndProc
    virtual int IsCommandDisable(int command);
};


int  TMessageMsgServer::IsCommandDisable(int command)
{
    switch(command)
    {
        case CM_MSGALLREMOVE:
        case CM_FILESAVEAS:return 0;
    }
    return DM_NOHANDLER;
};

#ifndef RUNTIME
//класс для отображения редактора текста в отдельном окне
class TEditMsgServer:public TWindow
{
public:
    HWND edit;
    PClass _class;
    UINT16 mode;
    char  filename[260];
    TEditMsgServer(LPCREATESTRUCT,HWND h):TWindow(0,0){HWindow=h;filename[0]=0;mode=0;};
    void UpdateCrd();
    BOOL Compile();
    virtual BOOL CanClose();
    virtual LRESULT WndProc(UINT message, WPARAM wParam,LPARAM lParam);
    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    virtual int  IsCommandDisable(int command);
    BOOL CmSave();
    BOOL CmSaveAs(BOOL dlg=TRUE);
    BOOL DoFind(WPARAM,int );
};

class TTreeMsgServer:public TWindow //Для библиотек
{
public:
    HWND tree;
    TTreeMsgServer(LPCREATESTRUCT,HWND h):TWindow(0,0){HWindow=h;};
    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    virtual int  IsCommandDisable(int command);
    BOOL         DoFind(BOOL start);
    void         ExpandLibs(PClassListInfo);
    void         Paste(TLibrary*);
    LRESULT WndProc(UINT message, WPARAM/* wParam*/,LPARAM /*lParam*/)
    {
        return 0;
    } // WndProc
};

class THMsgServer:public TTreeMsgServer // Для иерархии
{
public:
    PObject object;
    THMsgServer(LPCREATESTRUCT l,HWND h):TTreeMsgServer(l,h){}
    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    virtual int  IsCommandDisable(int command);
    BOOL         DoFind(BOOL start);
    void         ExpandH(PObject);
};

#endif

class TWatchWindow:public TMDIChildWindow
{
    HWND table,label,vlabel;
public:
    char *titles[4];
    TProject * project;
    TWatchWindow(LPCREATESTRUCT,HWND);
    ~TWatchWindow();
    virtual LRESULT WndProc(UINT message, WPARAM wParam,LPARAM lParam);
    virtual void EvSize(WORD sizeType,int sx,int sy);
    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    virtual int  IsCommandDisable(int command);
};

TWatchWindow::TWatchWindow(LPCREATESTRUCT l,HWND h):TMDIChildWindow(l,h)
{
    MODE=MODE_WATCH;
    flags|=WF_PROJECT;
    label=CreateWindow("SC_BLabel",0,BLS_RESIZE|BLS_USERTEXT|BLS_HORIZONTAL| WS_CHILD | WS_VISIBLE | WS_BORDER,
                       0,0,640,22,HWindow,(HMENU)2,(HINSTANCE)hInstance,NULL);
    vlabel=CreateWindow("SC_BLabel",0,BLS_USERTEXT| WS_CHILD | WS_VISIBLE | WS_BORDER,
                        0,0,640,22,HWindow,(HMENU)3,(HINSTANCE)hInstance,NULL);
    table=CreateWindow("SC_Browser",0,0 | WS_CHILD | WS_VISIBLE  | WS_TABSTOP,
                       0,22,640,22,HWindow,(HMENU)1,(HINSTANCE)hInstance,NULL);

    SendMessage(table,TB_SETLABEL,(WPARAM)label,(LPARAM)vlabel);

    TCollumn c[4]={	TCollumn(0,100,CF_READONLY),
                    TCollumn(0,60,CF_READONLY),
                    TCollumn(0,150,0),
                    TCollumn(0,300,0)};
    for(int i=0;i<4;i++)
    {
        c[i]._min/=2;c[i]._max*=2;
        SendMessage(table,TB_ADDCOLLUMN,-1,LPARAM(&(c[i])));
    }
    prj=project=(TProject*)GetCurrentProject();
    SendMessage(table,TB_SETRANGEY,0,project->watch?project->watch->count:0);
    project->wnd=table;
    winmanager->PostCreateMDIWindow("@Watch@",(TMDIChildWindow*)this,0);
    for(i=0;i<4;i++)
    {
        char s[256];
        int j=i<3?IDS_VARLABEL+i:IDS_VARDESK;
        LoadString(HResource,j,s,sizeof(s));
        titles[i]=NewStr(s);
    }
};

TWatchWindow::~TWatchWindow()
{
    project->wnd=NULL;
    for(int i=0;i<4;i++) delete titles[i];
};

int  TWatchWindow::IsCommandDisable(int command)
{
    switch(command){
        case CM_PROPERTIES:
        case CM_EDITDELETE:
        case CM_DELETEALL :
        {
            if(project->watch)return 0;
            return DM_NOWATCHES;
        }
    }
    return TMDIChildWindow::IsCommandDisable(command);
};

void TWatchWindow::EvCommand(UINT command,HWND,WPARAM )
{
    CELLINFO  ci;
    SendMessage(table,TB_GETACTIVE,0,(LPARAM)&(ci));
    switch(command){
        case CM_PROPERTIES:
        {
            TWatch* w=(TWatch*)project->watch->At(ci.y);
            Variable2dDialog(HWindow,w->object,w->index);
        }break;
#ifndef RUNTIME
        case CM_EDITOBJECT:
        {
            TWatch* w=(TWatch*)project->watch->At(ci.y);
            PClass _parent=w->object?(w->object->parent->_class):NULL;
            ClassDialog(HWindow,NULL,w->object,NULL,NULL,_parent);
        }break;
#endif
        case CM_EDITDELETE:
        {
            project->DeleteWatch((C_TYPE)ci.y);
        }break;
        case CM_DELETEALL :
        {
            while(project->watch)project->DeleteWatch(0);
        }break;
    }
};

void TWatchWindow::EvSize(WORD sizeType,int sx,int sy)
{
    const int sizey=22;
    const int sizex=20;
    project->watchdisable=(sizeType==SIZE_MINIMIZED)||(sizeType==SIZE_MAXHIDE);
    SetWindowPos(label,0,sizex,0,sx-sizex,sizey,SWP_NOZORDER);
    SetWindowPos(vlabel,0,0,sizey-1,sizex,sy-sizey+1,SWP_NOZORDER);
    SetWindowPos(table,0,sizex,sizey-1,sx-sizex,sy-sizey+1,SWP_NOZORDER);
};

LRESULT TWatchWindow::WndProc(UINT message, WPARAM wParam,LPARAM lParam)
{
    switch(message){
        case TBN_RBUTTON:
        {
            //	CELLINFO* ci=(CELLINFO*)lParam;
            POINT pos;
            GetCursorPos(&pos);
            TSTRATUMMENU sm;
            sm.hWindow=HWindow;
            sm.hType=SCMENU_WATCH;
            TrackPopupMenuR(pos,MENU_WATCH,&sm,0);
        }break;
        case WM_PAINT:
        {
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            SelectObject(hdc,Bface);
            PatBlt(hdc,0,0,40,40,PATCOPY);
            EndPaint(HWindow,&ps);
        }return 0;
        case TBN_SETTEXT:
        {
            CELLINFO* ci=(CELLINFO*)lParam;
            return project->SetWatchText((INT16)ci->y,(INT16)ci->x,ci->text);
        }
        case TBN_GETTEXT:
        {
            CELLINFO* ci=(CELLINFO*)lParam;
            return project->GetWatchText(ci->y,ci->x,ci->text);
        };
        case TBL_GETTEXT:
        {
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info)
            {
                if (info->id==2 && info->pos>=0 && info->pos<5)
                    lstrcpy(info->text,titles[(int)info->pos]);
                else
                    lstrcpy(info->text,"•");
            }
        } return TRUE;
    }
    return TMDIChildWindow::WndProc(message,wParam,lParam);
};

#ifndef RUNTIME

void GetNONAME(char *ss){
    static BYTE number=0;
    wsprintf(ss,"NONAME%02d.VDR",(int)number);
    number++;
    if(number>99)number=0;
};


int  TEditMsgServer::IsCommandDisable(int command)
{
    switch(command)
    {
        case CM_SEARCHAGAIN:
        case CM_SEARCH:
        case CM_REPLACE:return 0;
        case CM_SAVE:
        case CM_FILESAVEAS:if(_class)return 200;return 0;
        case CM_EDITUNDO:if(SendMessage(edit,EM_CANUNDO, 0, 0))return 0;return DM_NOUNDO;
        case CM_EDITCUT:
        case CM_EDITCOPY:
        case CM_EDITDELETE:
        {
            DWORD dwResult = SendMessage(edit, EM_GETSEL, 0, 0);
            int wStart = LOWORD(dwResult);
            int wEnd   = HIWORD(dwResult);
            if (wEnd>wStart)
                return 0;
            return DM_NOTEXTSELECTED;
        };
        case CM_LOCALHELP:
        case CM_COMPILE:return 0;
        case CM_EDITPASTE:
            if (IsClipboardFormatAvailable(CF_TEXT))
                return 0;
            return DM_NOTEXTINCLIP;
    }
    return DM_NOHANDLER;
};

BOOL TEditMsgServer::CanClose(){
    char s[256];
    char buf[256];
    if(SendMessage(edit,EM_GETMODIFY,0,0)){
        if(_class){

            LoadString(HResource,IDS_str106,buf,sizeof(buf));
            wsprintf(s,buf,_class->name);
            int id= MSGBOX(HWindow,s,_MSG_QSC,MB_DEFBUTTON1|MB_YESNOCANCEL|MB_ICONQUESTION);
            switch(id){
                case IDYES:{return Compile();}
                case IDCANCEL:return FALSE;
            }
        }else{
            char * fn=filename;
            if(!fn[0])fn="noname.txt";
            LoadString(HResource,IDS_str107,buf,sizeof(buf));

            wsprintf(s,buf,fn);
            int id= MSGBOX(HWindow,s,_MSG_QSC,MB_DEFBUTTON1|MB_YESNOCANCEL|MB_ICONQUESTION);
            switch(id){
                case IDYES:{return CmSave();}
                case IDCANCEL:return FALSE;
            }
        }
    }
    return TRUE;
};
BOOL TEditMsgServer::CmSave(){
    if(!filename[0]){
        return CmSaveAs();
    }
    return CmSaveAs(FALSE);
};
BOOL TEditMsgServer::CmSaveAs(BOOL dlg){
    if(dlg){
        if(!FileDialog(HWindow,filename,IDS_TEXT,FD_SAVE))return FALSE;
        SetWindowText(GetParent(GetParent(HWindow)),filename);
    }
    TDOSStream st(filename,TDOSStream::stCreate);
    if(st.status)return FALSE;
    int len=(int)SendMessage(edit,WM_GETTEXTLENGTH,0,0);
    char * text=new char[len+1];
    GetWindowText(HWindow,text,len+1);
    if(mode==1)AnsiToOem(text,text);
    st.Write(text,len+1);
    delete text;
    return (st.status==0);
};
BOOL TEditMsgServer::DoFind(WPARAM wNotifyCode,int i){
    switch(i){
        case 3:{
            if(!wNotifyCode){
                SendMessage(edit,ESM_SEARCH,6,0);
            }return 0;
        }
        case 1:if(!wNotifyCode && SearchTextDialog(HWindow,0)){
                SendMessage(edit,ESM_SEARCH,0,0);
            }return 0;
        case 2:{
            int r=0;
            if(!wNotifyCode && (r=SearchTextDialog(HWindow,1))){
                int flags=1;
                if(r==2)flags|=16;
                SendMessage(edit,ESM_SEARCH,flags,0);
            }
        }return 0;
    }
    return 1;
};


void TEditMsgServer::EvCommand(UINT command,HWND /*hwndCtl*/,WPARAM wNotifyCode)
{
    switch (command){
        case CM_LOCALHELP:
        {
            SendMessage(edit,ESM_HELP,0,0);
        }break;
        case CM_SEARCHAGAIN:
            DoFind(wNotifyCode,3);
            break;
        case CM_SEARCH:
            DoFind(wNotifyCode,1);
            break;
        case CM_REPLACE:DoFind(wNotifyCode,2);break;
            //if(!wNotifyCode)SendMessage(edit,ESM_SEARCH, 1, 0);return;
        case CM_EDITUNDO:SendMessage(edit,WM_UNDO,0,0);break;
        case CM_EDITCUT:SendMessage(edit,WM_CUT,0,0);break;
        case CM_EDITCOPY:SendMessage(edit,WM_COPY,0,0);break;
        case CM_EDITPASTE:SendMessage(edit,WM_PASTE,0,0);break;
        case CM_EDITDELETE:SendMessage(edit,WM_CLEAR,0,0);break;
        case CM_SAVE:      CmSave();break;
        case CM_FILESAVEAS:CmSaveAs();break;
        case 10:
            switch (wNotifyCode){
                case EN_UPDATE:
                case EN_CHANGE:UpdateCrd();break;
            } break;
        case CM_REPAINT: //если нажали на F5
        case CM_COMPILE:Compile();break;
    }
};

LRESULT TEditMsgServer::WndProc(UINT message, WPARAM wParam,LPARAM lParam)
{
    switch (message)
    {
        case TW_UPDATECLASSWINDOWS:
        {
            UPDATESTRUCT*us=(UPDATESTRUCT*)lParam;
            if(_class && (us->what==UW_CLASS))
            {
                PClassListInfo pcl=(PClassListInfo)us->data;
                if(pcl->GetClass()==_class)
                {
                    char s[256]="";
                    GetTextWindowName(_class,s);
                    SetWindowText(HWindow,s);
                }
            }
        }return 0;
        case EMN_FIND:
        {
            return DoFind(0,wParam);
        }
        case EMN_HELP:
        {
            char *ps=(char *)lParam;
            HELP(ps);
        }return 0;
        case EMN_RDOWN:
        {
            POINT pos;
            pos.x=(INT16)LOWORD(lParam);
            pos.y=(INT16)HIWORD(lParam);
            ClientToScreen(HWindow,&pos);

            TSTRATUMMENU sm;
            sm.hWindow=HWindow;
            sm.hType=SCMENU_MENUSYNTAX;

            mainwindow->TrackPopupMenuR(pos,MENU_SYNTAX,&sm);
        }break;
        case WM_SETFOCUS:
            //SetControlBarRange(RANGE_TEXT);
            break;
    }
    return 0;
};// WndProc

int  TTreeMsgServer::IsCommandDisable(int command){
    switch(command){
        case CM_SEARCHAGAIN:{
            char s[256];
            TreeItemData td;
            if(SCGetScVarS("search_string",s)&&
               SendMessage(tree,TVN_GETACTIVEITEM,0,(long)&td))  return 0;
        }return DM_CANAGAIN;
        case CM_SEARCH:return 0;
        case CM_RECOMPLILE:
        case CM_RENAMEPATHS:
        case CM_DELUNUSED:return 0;
        case CM_EDITPASTE:
        case CM_EDITCOPY:
        case CM_IMAJE_DETAIL_INFO:
        case CM_SCHEME:
        case CM_scTEXTEDIT:
        case CM_IMAGE:
        case CM_NEWCLASS:
        case CM_NEWLIB:
        case CM_EDITDELETE:
        case CM_PROPERTIES:
        case CM_tNEWCLASS:
        case CM_DUPLICATE:{
            TreeItemData td;
            if(!SendMessage(tree,TVN_GETACTIVEITEM,0,(long)&td))return 200;
            switch(td.datatype){
                case TREE_CLASS:{
                    PClass _class=((PClassListInfo)td.data)->GetClass();
                    switch(command){
                        case CM_SCHEME:
                        case CM_scTEXTEDIT:
                        case CM_IMAGE:
                        case CM_NEWCLASS:if(_class->Protected())return DM_PROTECTED;else return 0;

                        case CM_EDITPASTE:if(!IsClipboardFormatAvailable(CFormat_CLASS))return 200;
                        case CM_EDITCOPY:
                        case CM_NEWLIB:
                        case CM_tNEWCLASS:if(_class->listinfo->library->flags&(CLF_READONLY|CLF_PACKFILE))return DM_PROTECTED;else return 0;
                        case CM_EDITCUT:
                        case CM_EDITDELETE:if(_class->Protected())return DM_PROTECTED;else {
                                if(_class->first)return 200;else return 0;
                            }
                        case CM_IMAJE_DETAIL_INFO:
                        case CM_PROPERTIES:
                        case CM_DUPLICATE: return 0;
                    }}break;
                case TREE_LIBRARY:{
                    TLibrary*lib=(TLibrary*)(td.data);
                    switch(command){
                        case CM_PROPERTIES:if (GetParentLibrary()!=lib)return 0;break;
                        case CM_EDITPASTE:if(!IsClipboardFormatAvailable(CFormat_CLASS))return 200;
                        case CM_NEWCLASS:
                        case CM_NEWLIB:
                        case CM_tNEWCLASS:if(lib->flags&(CLF_READONLY|CLF_PACKFILE))return DM_PROTECTED;else return 0;
                        case CM_EDITDELETE:if(lib->flags&(CLF_READONLY|CLF_PACKFILE))return DM_PROTECTED;
                            if (lib->items->count)return DM_LIBNOTEMPTY;
                            return 0;
                    }
                }break;
            }
            return DM_NOHANDLER;
        };
    }
    return 2;
};

pointer  __SearchLib(TLibrary*_lib,char *s,pointer p,BOOL &begin){
    for(C_TYPE i=0;i<_lib->items->count;i++){
        if(GetAsyncKeyState(VK_ESCAPE)<0)return (pointer)1;
        PClassListInfo pcli=(PClassListInfo)_lib->items->At(i);
        if(pcli->flags&CLF_LIBRARY){
            pointer _p= __SearchLib((TLibrary*)pcli,s,p,begin);
            if(_p)return _p;
        }else{
            if(begin)
            {
                if(ClassCompare(pcli->GetClass(),NULL,NULL,s)){
                    return pcli;
                }}}
        if(pcli==p)begin=TRUE;
    }
    return NULL;
};

void TTreeMsgServer::ExpandLibs(PClassListInfo pcli)
{
    TreeItemData td;
    td.data  = (LPARAM)pcli;
    if(::SendMessage(tree,TVN_GETITEMINFO,0,(long)&td))return;
    PClassListInfo _pcli;
    if(pcli->flags&CLF_LIBRARY)_pcli=(PClassListInfo)((TLibrary*)pcli)->parent;
    else _pcli=(PClassListInfo)pcli->library;
    ExpandLibs(_pcli);
    __GetTreeItemInfo(tree,(DWORD)_pcli,0);
};

BOOL TTreeMsgServer::DoFind(BOOL start){
    TLibrary*_lib =GetParentLibrary();
    char s[256];
    if(SCGetScVarS("search_string",s)){
        TWaitCursor wait(HWindow);

        UINT32 vm=SCGetScVar("search_opt1",0);
        BOOL _case=!((vm&1)!=0);
        if(_case)AnsiUpper(s);
        BOOL begin=TRUE;
        pointer p=NULL;
        if(!start){
            begin=FALSE;
            p=(pointer)SendMessage(tree,TVN_GETACTIVEITEM,0,NULL);
        }
        pointer _p= __SearchLib(_lib,s,p,begin);
        if(_p){
            if((UINT32)_p<0xff)return FALSE;
            ExpandLibs((PClassListInfo)_p);
            SendMessage(tree,TV_SETACTIVEITEM,0,(LPARAM)_p);
            return TRUE;
        }
    }
    MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG22),_MSG_INFO,MB_OK|MB_ICONINFORMATION);
    return FALSE;
};

void TTreeMsgServer::Paste(TLibrary*lib)
{
    if(OpenClipboard(HWindow))
    {
        TMemoryStream ms(GetClipboardData(CFormat_CLASS));
        int command=ms.ReadWord();
        char s[256];
        while(ms.ReadStr(s,sizeof(s)))
        {
            PClass _class=GetClassByName(s);
            if(command==CM_EDITCOPY)
            {
                char s2[80];
                lstrcpy(s2,s);
                GetUniqueClassName(s2,TRUE);
                PClass __class=CreateClassByClass(s,s2,lib);
                if(__class)
                {
                    SendMessage(tree,TV_SETACTIVEITEM,0,(LPARAM)(__class->GetClassListInfo()));
                }
            }
            else
            {
            }
        }
        CloseClipboard();
    }
};

void TTreeMsgServer::EvCommand(UINT command,HWND,WPARAM)
{
    TreeItemData td;
    if(SendMessage(tree,TVN_GETACTIVEITEM,0,(long)&td)){
        if(td.datatype==TREE_CLASS){
            TClass*_class=(((TClassListInfo*)td.data)->GetClass());
            switch(command){
                case CM_IMAJE_DETAIL_INFO:{ ClassInfoDialog(HWindow,_class);  };return;
                case CM_EDITPASTE:{Paste(_class->GetClassListInfo()->library); }return;
                case CM_EDITCUT:
                case CM_EDITCOPY:{
                    if (OpenClipboard(HWindow)){
                        EmptyClipboard();
                        TMemoryStream *ms=new TMemoryStream(0);
                        ms->WriteWord(command);
                        ms->WriteStr(_class->name);
                        ms->WriteWord(0);
                        ms->Truncate();
                        SetClipboardData(CFormat_CLASS,ms->GetHANDLE());
                        delete ms;
                        CloseClipboard();
                    }
                }return;
            }}
        if(td.datatype==TREE_LIBRARY){
            TLibrary *lib=(TLibrary *)(td.data);
            switch(command){
                case CM_EDITPASTE:{Paste(lib); }return;
                case CM_SETASDEF:{DefaultLib=lib;};break;
            }
        }
    }

    switch(command){
        case CM_SEARCHAGAIN:DoFind(FALSE);break;
        case CM_SEARCH:{
            if(SearchDialog(HWindow,1|4|8|16)){
                //MSGBOX(HWindow,"Not realised!",_MSG_INFO,MB_OK|MB_ICONINFORMATION);
                DoFind(TRUE);
            }
        };return;
        case CM_NEWLIB:
        case CM_NEWCLASS:
        case CM_HELPONIMAGE:
        case CM_PROPERTIES:
        case CM_SCHEME:
        case CM_scTEXTEDIT:
        case CM_EDITDELETE:
        case CM_tNEWCLASS:
        case CM_IMAGE:
        case CM_DUPLICATE:
        case CM_RENAMEPATHS:
        case CM_DELUNUSED:
        case CM_RECOMPLILE:{
            TreeItemData td;
            if(SendMessage(tree,TVN_GETACTIVEITEM,0,(long)&td)){
                if ((td.datatype==TREE_CLASS)||((td.datatype==TREE_LIBRARY)&&
                                                ((command==CM_NEWCLASS)||(command==CM_NEWLIB)||(command==CM_EDITDELETE)||
                                                 (command==CM_PROPERTIES)||(command==CM_HELPONIMAGE)||(command==CM_SETASDEF)||
                                                 (command==CM_DELUNUSED)||(command==CM_RECOMPLILE) || (command==CM_RENAMEPATHS)
                                                 ))){
                    PClass _class=NULL;
                    if(td.datatype==TREE_CLASS)_class=(((TClassListInfo*)td.data)->GetClass());

                    switch(command){
                        case CM_RENAMEPATHS:
                        case CM_DELUNUSED:
                        case CM_NEWLIB:
                        case CM_NEWCLASS:
                        case CM_RECOMPLILE:  {

                            TLibrary * lib=NULL;
                            switch(td.datatype){
                                case TREE_LIBRARY: lib=(TLibrary *)(td.data);break;
                                case TREE_CLASS:   lib=(TLibrary *)(td.parent);break;
                            }
                            if (lib){
                                switch (command){
                                    case CM_RECOMPLILE:Recompile(lib);break;
                                    case CM_RENAMEPATHS:DoRenamePaths(tree,lib);break;
                                    case CM_DELUNUSED:UnusedClassesDialog(tree,lib);break;
                                    case CM_NEWCLASS:ClassDialog(tree,NULL,NULL,lib);break;
                                    case CM_NEWLIB:{

                                        char s[256]="";
                                        char s1[128],s2[128];
                                        LoadString(HResource,IDS_str165,s1,sizeof(s1));
                                        LoadString(HResource,IDS_str166,s2,sizeof(s2));

                                        if (InputBox(tree,s1,s2,s))	MakeNewLib(lib,s);
                                    }break;
                                }
                            }} break;
                        case CM_HELPONIMAGE:{
                            if (td.datatype==TREE_CLASS){
                                _class->Help();
                            }
                            if (td.datatype==TREE_LIBRARY){
                                TLibrary * lib=(TLibrary *)(td.data);
                                char s[256];
                                wsprintf(s,"Library %s",lib->path);
                                HELP(s);
                            }
                        };break;
                        case CM_PROPERTIES:
                            if (td.datatype==TREE_CLASS) ClassDialog(HWindow,_class,NULL,0);
                            if (td.datatype==TREE_LIBRARY){
                                TLibrary *lib=(TLibrary *)(td.data);
                                /* ADDREF
           if(lib->project && lib->parent &&(lib->parent->project==0)&&
           (((TProject*)lib->project)->lib==lib))
              ProjectDialog(HWindow,(TProject*)lib->project);
           else */ LibraryDialog(HWindow,(TLibrary *)(td.data));


                            };break;
                        case CM_SCHEME:
                        {
                            char s[80];
                            lstrcpy(s,_class->name);
                            mainwindow->CreateSchemeWindow(s,0);
                        }break;
                        case CM_scTEXTEDIT:
                            mainwindow->CreateTextWindow(_class);
                            break;
                        case CM_EDITDELETE:
                            if (td.datatype==TREE_CLASS){
                                if (_class->first){

                                    MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG25),_MSG_ERROR,MB_OK|MB_ICONSTOP);
                                }else{
                                    if(MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG26),_MSG_WARNING,MB_YESNOCANCEL|MB_ICONQUESTION)==IDYES)
                                    {char s[80];
                                        lstrcpy(s,_class->name);
                                        DeleteClass(s);
                                    }}
                            }else{
                                if (td.datatype==TREE_LIBRARY){
                                    TLibrary * lib=(TLibrary *)(td.data);
                                    if (lib){
                                        if(MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG27),_MSG_WARNING,
                                                  MB_YESNOCANCEL|MB_ICONQUESTION)==IDYES)

                                        {
                                            BOOL b=DeleteLib(lib);
                                            if(!b){
                                                MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG28),_MSG_ERROR,MB_OK|MB_ICONSTOP);
                                            }}}
                                }}
                            break;
                        case CM_tNEWCLASS:
                            break;
                        case CM_DUPLICATE:{
                            char s[80];
                            char s2[80];
                            lstrcpy(s,_class->name);
                            lstrcpy(s2,_class->name);
                            GetUniqueClassName(s2,TRUE);
                            CreateClassByClass(s,s2,((TClassListInfo*)td.data)->library);}
                            break;
#ifndef RUNTIME
                        case CM_IMAGE:{
                            char s[80];lstrcpy(s,_class->name);
                            mainwindow->CreateImageWindow(s);
                        }break;
#endif
                    }}
            }
        }break;
    }
};
int  THMsgServer::IsCommandDisable(int command){
    switch(command){
        case CM_SEARCHAGAIN:{
            char s[256];
            TreeItemData td;
            if(SCGetScVarS("search_string",s)&&
               SendMessage(tree,TVN_GETACTIVEITEM,0,(long)&td)) return 0;
        }return DM_CANAGAIN;
        case CM_SEARCH:return 0;
        case CM_PROPERTIES:
        case CM_SCHEME:
        case CM_scTEXTEDIT:
        case CM_IMAGE:
        case CM_tNEWCLASS:
        case CM_DUPLICATE:{
            TreeItemData td;
            if(SendMessage(tree,TVN_GETACTIVEITEM,0,(long)&td)){
                if (td.datatype==TREE_OBJECT)return 0;
            }
            return DM_NOHANDLER;
        };
    }
    return 2;
};

PObject __SearchObjs(PObject po,char *s,PObject p,TChildInfo *ci,BOOL&begin){
    if(begin){
        if(ClassCompare(po->_class,po,ci,s))return po;
    }
    if(p==po)begin=TRUE;
    PObject _po=po->childObjects;
    INT16 i=0;
    while (_po){
        PObject ok=__SearchObjs(_po,s,p,po->_class->childs+i,begin);
        if(ok)return ok;
        _po=_po->next;
        i++;
    }
    return 0;
};

BOOL THMsgServer::DoFind(BOOL start){
    char s[256];
    TWaitCursor wait(HWindow);
    if(SCGetScVarS("search_string",s)){
        UINT32 vm=SCGetScVar("search_opt1",0);
        BOOL _case=!((vm&1)!=0);
        if(_case)AnsiUpper(s);
        BOOL begin=TRUE;
        PObject bpo=NULL;
        TChildInfo* info=NULL;
        PObject po=object;
        if(!start){
            begin=FALSE;
            bpo=(PObject)SendMessage(tree,TVN_GETACTIVEITEM,0,NULL);
            INT16 num;
            if(bpo && bpo->GetHandle(&num)){
                info=bpo->parent->_class->childs+num;
            }
        }
        PObject _po= __SearchObjs(po,s,bpo,info,begin);
        if(_po){
            if((UINT32)_po>0xff){
                ExpandH(_po);
                SendMessage(tree,TV_SETACTIVEITEM,0,(LPARAM)_po);
                return TRUE;
            }
        }
    }
    MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG22),_MSG_INFO,MB_OK|MB_ICONINFORMATION);
    return FALSE;
};

void THMsgServer::ExpandH(PObject po){
    TreeItemData td;
    td.data  = (LPARAM)po;
    if(::SendMessage(tree,TVN_GETITEMINFO,0,(long)&td))return;
    ExpandH(po->parent);
    __GetTreeItemInfo(tree,(DWORD)(po->parent),0);
};

void THMsgServer::EvCommand(UINT command,HWND,WPARAM){
    switch(command){
        case CM_SEARCHAGAIN:DoFind(FALSE);break;
        case CM_SEARCH:{
            if(SearchDialog(HWindow,4|8|16)){
                DoFind(TRUE);
            }
        };return;
        case CM_PROPERTIES:
        case CM_SCHEME:
        case CM_scTEXTEDIT:
        case CM_EDITDELETE:
        case CM_IMAGE:
        {
            TreeItemData td;
            if(SendMessage(tree,TVN_GETACTIVEITEM,0,(long)&td)){
                if (td.datatype==TREE_OBJECT){
                    PObject po=((PObject)td.data);
                    if (po){
                        PClass _class=po->_class;
                        switch(command){
                            case CM_PROPERTIES:
                                ClassDialog(HWindow,NULL,po,0);
                                break;

                            case CM_SCHEME:
                            {
                                char s[80];
                                lstrcpy(s,_class->name);
                                mainwindow->CreateSchemeWindow(s,po);
                            }break;
                            case CM_scTEXTEDIT:{
                                mainwindow->CreateTextWindow(_class);
                                break;}
                            case CM_IMAGE:{
                                char s[80];lstrcpy(s,_class->name);
                                mainwindow->CreateImageWindow(s);
                                break;
                            }
                        }
                    }
                }
            }}
    }
};


void TEditMsgServer::UpdateCrd(){
    int c1=(int)SendMessage(edit, EM_LINEFROMCHAR,(WPARAM) -1, 0);
    int c2=(int)SendMessage(edit,EM_LINEINDEX,(WPARAM)-1, 0);
    char s[64];
    wsprintf(s,"%d:%d",c1,c2);
    SCSetStatusText(2,s);
};

BOOL TEditMsgServer::Compile()
{
    BOOL rez=TRUE;
    int l=(int)SendMessage(edit,WM_GETTEXTLENGTH,0,0);
    char * txt=NULL;
    if (l)
    {
        txt=new char [l+2];
        GetWindowText(edit,txt,l+1);
    }
    if(!_class)
        return FALSE;

    if (!SetClassText(_class,txt))
    {
        C_TYPE start,end;
        compiler->GetLastErrorPos(&start,&end);
        SendMessage(edit,EM_SETSEL,
            #ifdef WIN32
                    start,end
            #else
                    0,MAKELONG(start,end)
            #endif
                    );
        rez=FALSE;
    }
    else
    {
        SendMessage(edit,EM_SETMODIFY,0,0);
    }
    if(txt)
        delete txt;
    return rez;
};

#endif // RUNTIME
TErrorHandler::TErrorHandler():TCollection(1000,1)
{
    hwnd=NULL;
};
void TErrorHandler::AllRemove()
{
    if(hwnd)
        SendMessage(hwnd,LB_RESETCONTENT,0,0);
    FreeAll();
};

void TErrorHandler::InsertMSG(char*s)
{
    if(s==NULL || (*s)==0)
        s=" ";
    if(count>=limit)
    {
        AtFree(count-1);
        if(hwnd)
            //SendMessage(hwnd,LB_DELETESTRING,0,0);
            SendMessage(hwnd,LB_DELETESTRING,(WPARAM)count,0);
    }
    InsertInWind(Insert(NewStr(s)));
};

void TErrorHandler::MakeWindow(HWND _hwnd)
{
    hwnd=_hwnd;
    for(int i=0;i<count;i++)
        InsertInWind(i);
};

void TErrorHandler::InsertInWind(int i)
{
    //SendMessage(hwnd,LB_ADDSTRING,0,(LPARAM)At(i));
    char str[9999];
    struct time t;
    gettime(&t);
    wsprintf(str,"%d:%d:%d - %s",t.ti_hour,t.ti_min,t.ti_sec,At(i));
    SendMessage(hwnd,LB_INSERTSTRING,0,(LPARAM)str);
};
void TErrorHandler::FreeItem(pointer p)
{
    delete (char*)p;
};

TErrorHandler*errorhandler=NULL;
TCollection * ingnorederr=NULL;

void InitErrorHandler()
{
    errorhandler=new TErrorHandler();
    InitMathErrorHandler();
};

void DoneErrorHandler()
{
    DeleteCollection(errorhandler);
    if(ingnorederr)
    {
        ingnorederr->DeleteAll();
        DeleteCollection(ingnorederr);
    }
    errorhandler=NULL;
};

TStatusBar*_StatusBar=NULL;


class TClassIconWindow:public TWindow{
    TClass * _class;
public:
    TClassIconWindow(LPCREATESTRUCT l,HWND h);
    LRESULT WndProc(UINT message, WPARAM wParam,LPARAM lParam);
};

TClassIconWindow::TClassIconWindow(LPCREATESTRUCT l,HWND h):TWindow(l,h){
    _class=0;
    int x,y;
    x=y=34;
    //  if (l->style&WS_BORDER){
    {
        x+=2*(GetSystemMetrics(SM_CXDLGFRAME)/*+GetSystemMetrics(SM_CXBORDER)*/);
        y+=2*(GetSystemMetrics(SM_CYDLGFRAME)/*+GetSystemMetrics(SM_CYBORDER)*/);
    }
    SetWindowPos(HWindow,0,0,0,x,y,SWP_NOMOVE|SWP_NOZORDER);
};
LRESULT TClassIconWindow::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            if (_class){
                SelectObject(hdc,Bface);
                PatBlt(hdc,0,0,40,40,PATCOPY);
                PaintIcon(_class,hdc,1,1,32,32);
            }
            EndPaint(HWindow,&ps);
            return 0;
        };
        case TW_SETCLASS:{
            _class =(TClass*)lParam;
        }break;
    }
    return DefWindowProc(message,wParam,lParam);
};


//====================================================================

TScroller::TScroller(TWindow*_w,long amin,long amax,long _step,int v)
{
    win=_w;
    singlestep=1;
    pos=0;
    lo=amin;
    hi=amax;
    step=_step;
    page=0;
    mode=(BYTE)v;
    autohide=1;
    flags=0;
    show=TRUE;
    RECT r;
    GetClientRect(_w->HWindow,&r);
    if (v)
    {
        if(r.bottom)page=(amax-amin)/r.bottom;
        else page=0;
    }
    else
    {
        if (r.right)page=(amax-amin)/r.right;
        else page=0;
    }
#ifdef WIN32
    SetPageSize();
#endif
};

void TScroller::Show(BOOL s)
{
    if(s==show)return;
    show=s;
    if(flags&SF_MANUAL)
    {
        ShowScrollBar(win->HWindow,mode,s);
    }
    else
    {
        SetRange(lo,hi);
    }
};

long TScroller::GetPos(){
    return pos;
};

long TScroller::ValidValue(long v)
{
    if(v<_lo)v=v=_lo;
    if(v>hiscroll)v=hiscroll;
    return v;
};

void TScroller::_SetPos(long n)
{
    if (n==pos)return;

    if (mode==SB_VERT)
    {
        ScrollWindow(win->HWindow,0,(int)((pos-n)*step),NULL,NULL);
        win->ScrollTo(NULL,&n);
    }
    if (mode==SB_HORZ)
    {
        ScrollWindow(win->HWindow,(int)((pos-n)*step),0,NULL,NULL);
        win->ScrollTo(&n,NULL);
    }

    pos=n;
    SetScrollPos(win->HWindow,(int)mode,(int)pos,TRUE);
    UpdateWindow(win->HWindow);
    pos=n;
};

void TScroller::SetPos(long _pos,BOOL scroll)
{
    if (scroll)
    {
        _SetPos(_pos);
    }
    else pos=_pos;
    SetScrollPos(win->HWindow,mode,(int)pos,TRUE);
};

inline long LongMulDiv(long mul1, long mul2, long div1)
{return (mul1*mul2) / div1;}

void TScroller::SetRange(long amin,long amax)
{
    lo=amin;
    hi=amax;
    _lo=lo;
    int enable;
    if (flags&SF_SCHEME)
    {
        hiscroll=(hi-lo);
        _lo=lo-page;
        enable=1;
    }else{
        hiscroll=(hi-page);
        enable=((hiscroll-lo))>=0 || !autohide;
    }
    if (enable)
    {
#ifdef WIN32
        SetScrollRange(win->HWindow,mode,(int)_lo,(int)hiscroll+Spage,TRUE);
#else
        SetScrollRange(win->HWindow,mode,(int)_lo,(int)hiscroll,TRUE);
#endif
    }
    if(flags&SF_FIXTOP)
    {
        if((page+pos)>hi)
        {
            //long j=(pos-lo)*i;
            long _pos=(hi-lo)-page;
            _pos=max(_pos,lo);
            if(_pos!=pos) SetPos(_pos);
        }
    }
    if (autohide && show)
        ShowScrollBar(win->HWindow,mode,enable);
};

LRESULT TScroller::Process(WPARAM wParam,LPARAM lParam){

    long n;
#ifdef WIN32
    int nScrollCode = (int) LOWORD(wParam);
    HWND hwndScrollBar=(HWND)lParam;
#else
    int nScrollCode = wParam;
    HWND hwndScrollBar=(HWND)HIWORD(lParam);
#endif

    if(hwndScrollBar)return 0;

    switch (nScrollCode){
        case SB_TOP:n=lo;break;
        case SB_BOTTOM:n=hi;break;
        case SB_LINEUP:n=max(_lo,pos-singlestep);break;
        case SB_LINEDOWN:n=min(pos+singlestep,hiscroll);break;
        case SB_PAGEUP:n=max(_lo,pos-page);break;
        case SB_PAGEDOWN:n=min(pos+page,hiscroll);break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
#ifdef WIN32
            //  n= GetScrollPos(win->HWindow,mode);
            n=(short)HIWORD(wParam);
            //  LongMulDiv(HIWORD(wParam),32767,hiscroll-_lo);
#else
            n=(int)LOWORD(lParam);
#endif
            break;
        default: n=pos;
    }
    _SetPos(n);
    if(nScrollCode!=SB_THUMBTRACK){
        win->PostScrollTo();
    }

    return 0;
};
#ifdef WIN32
void TScroller::SetPageSize(){
    if(flags&SF_MANUAL)return;
    SCROLLINFO scr;
    scr.cbSize=sizeof(SCROLLINFO);
    scr.fMask=SIF_PAGE;
    RECT r;
    GetClientRect(win->HWindow,&r);
    int w=(mode==SB_HORZ)?(r.right):(r.bottom);
    Spage = max(1, (w+1) / step - 1);
    scr.nPage=Spage;
    // scr.nPage=50;
    scr.nMin=lo;
    scr.nMax=hi+scr.nPage;
    scr.nPos=pos;
    scr.nTrackPos=pos;
    SetScrollInfo(win->HWindow,	// handle of window with scroll bar
                  mode,	// scroll bar flag
                  &scr,	// pointer to structure with scroll parameters
                  TRUE// redraw flag
                  );
};
#endif

void TScroller::SetParam(long amin,long amax,long apos, BOOL show,long apage){
    pos=apos;
    lo=amin;hiscroll=hi=amax;
    _lo=lo;
    ShowScrollBar(win->HWindow,mode,show);
    if(!show)return;
    SetScrollPos(win->HWindow,mode,(int)pos,TRUE);
#ifdef WIN32
    SetScrollRange(win->HWindow,mode,(int)lo,(int)hiscroll+apage,TRUE);
#else
    SetScrollRange(win->HWindow,mode,(int)lo,(int)hiscroll,TRUE);
#endif
#ifdef WIN32
    Spage=apage;
    SCROLLINFO scr;
    scr.cbSize=sizeof(SCROLLINFO);
    scr.fMask = SIF_PAGE;
    scr.nPage = Spage;
    SetScrollInfo(win->HWindow,	// handle of window with scroll bar
                  mode,	// scroll bar flag
                  &scr,	// pointer to structure with scroll parameters
                  TRUE// redraw flag
                  );
#endif
};

void TScroller::ChangeSize(long size){
    if (!(flags&1)){
        flags|=1;
        page=size/step;
        if(!(flags&SF_MANUAL)){
#ifdef WIN32
            SetPageSize();
#endif
            SetRange(lo,hi);
            win->PostScrollTo();
        }
        flags&=~1;
    }
};

void _TrackPopupMenu(HWND HWindow,POINT point,TSTRATUMMENU*sm,BOOL chkDisable){
    HWND hf=GetFocus();
    EnableWindow(HWindow,FALSE);
    if(chkDisable){
        TWindow*w=(TWindow*)SendMessage(HWindow,scwnd_message,0,0);
        if(w)CheckMenuForEnable(w,sm->hMenu);
    }
    dllmanager->ForEach(SC_INITMENUPOPUP,(UINT32)sm,0);
    ::TrackPopupMenu(sm->hMenu,TPM_LEFTALIGN , point.x,
                     point.y, 0,HWindow,NULL);
    EnableWindow(HWindow,TRUE);
    FullDeleteMenu(sm->hMenu);
    SetFocus(hf);
};

HMENU   ScLoadPopup(int r)
{
    HMENU menu = LoadMenu(HResource,MAKEINTRESOURCE(r));
    if(menu)
    {
        HMENU p=GetSubMenu(menu,0);
        RemoveMenu(menu,0,MF_BYPOSITION);
        DestroyMenu(menu);
        return p;
    }
    return NULL;
};

void ScAppendMenu(HMENU p,int flag,int cmd,int strid)
{
    char s[256];
    if(!LoadString(HResource,strid,s,sizeof(s)))
        lstrcpy(s,"**");

    AppendMenu(p, flag, cmd, s);
}

void _TrackPopupMenuR(HWND HWindow,POINT point,int r,TSTRATUMMENU*sm,BOOL chkDisable)
{
    HMENU menu= ScLoadPopup(r);
    if(menu)
    {
        sm->hMenu=menu;
        _TrackPopupMenu(HWindow,point,sm,chkDisable);
    }
};

HHOOK KeyHook;

LRESULT CALLBACK 
KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
    if(nCode!=HC_ACTION || nCode<0)
        return CallNextHookEx(KeyHook, nCode, wParam, lParam );

    bool result=false;

    int lang = LOWORD(GetKeyboardLayout(0));
    if(lang==1087)//затычка для казахского языка. Заменяем казахские unicode-символы на ascii-символы
    {
        HWND hFocusedControl = GetFocus();//получаем активный контрол
        int code=0;
        bool shift = (GetAsyncKeyState(VK_SHIFT)&0xffff)!=0;//нажата ли Shift
        bool caps = (GetKeyState(VK_CAPITAL)&0xffff)!=0;//нажата ли CapsLock
        bool isShift=(shift && !caps)||(!shift && caps);

        switch(wParam)//проверяем на какую клавишу нажали (а не какой символ вводим)
        {
            case 50://2
                if(isShift)	code = 170;
                else				code = 186;
                break;
            case 51://3
                if(isShift)	code = 178;
                else				code = 179;
                break;
            case 52://4
                if(isShift)	code = 140;
                else				code = 156;
                break;
            case 53://5
                if(isShift)	code = 129;
                else				code = 131;
                break;
            case 56://8
                if(isShift)	code = 135;
                else				code = 137;
                break;
            case 57://9
                if(isShift)	code = 166;
                else				code = 177;
                break;
            case 48://0
                if(isShift)	code = 141;
                else				code = 157;
                break;
            case 189://-
                if(isShift)	code = 164;
                else				code = 181;
                break;
            case 187://=
                if(isShift)	code = 142;
                else				code = 158;
                break;
        }
        if(code>0)
        {
            if(!(lParam & (1 << 31)))//обязательно нужно (иначе будет добавляться два символа). Значит клавишу нажимаем, а не отпускаем.
                SendMessage(hFocusedControl,WM_CHAR, code, lParam);
            result=true;
        }
    }
    return result | CallNextHookEx(KeyHook, nCode, wParam, lParam );
}



TWindow::TWindow(LPCREATESTRUCT,HWND hwnd)
{
    _scrollFlag=0;
    HWindow=hwnd;
    vscroll = hscroll = 0;
    CanUseDelete=0;
    CursorModule=0;CursorResId=0;HCursor=0;
    DefCursorModule=0;DefCursorResId=IDC_ARROW;HDefCursor=NULL;
    if(hwnd)SetWindowLong(hwnd,0,(long)this);
    tmenu=0;
    messages=0;
#ifdef LOCKEXITBUTTONOFWINDOW
    HMENU hMenuHandle;
    hMenuHandle = GetSystemMenu(HWindow, FALSE);
    if (hMenuHandle)
        DeleteMenu(hMenuHandle, SC_CLOSE, MF_BYCOMMAND);
#endif
};

void __gethelpfile(char*s,int mode=0);
HBITMAP extern list_items;


TWindow::~TWindow()
{
    if (vscroll)delete vscroll;
    if (hscroll)delete hscroll;
    if (HCursor && CursorModule) SetCursor(0,0);
    if (tmenu)delete tmenu;
    if(messages){
        messages->DoneHandler(NULL);
        DeleteCollection(messages);
    }
};



TMainWindow::~TMainWindow()
{
    UnhookWindowsHookEx(KeyHook);
    // if(hinet)FreeLibrary(hinet);
    SetMenu(HWindow,0);
    DestroyMenu(menu);menu=NULL;
    if(list_items)DeleteObject(list_items);
    char s[256];
    __gethelpfile(s,0);
    WinHelp(HWindow,s,HELP_QUIT,0);
    __gethelpfile(s,2);
    WinHelp(HWindow,s,HELP_QUIT,0);
    if (menu_logo)
    {
        DeleteObject(menu_logo);
        menu_logo=0;
    }
    mainwindow=NULL;
    //if (recently_modifyed){WriteRecently();}
    for(int i=0;i<recently->count;i++)
    {
        char *s=(char *)recently->At(i);
        delete s;
    }
    for(i=0;i<4;i++)
        DeleteCollection(docks[i]);
    delete recently;
    DoneControlBars();
    if(bm_home)
        DeleteObject(bm_home);
    // KillTimer(0,hTimer);
    GetSystemMenu(HWindow,TRUE);
};

void TWindow::TrackPopupMenuR(POINT point,int r,TSTRATUMMENU*sm,BOOL chkDisable)
{
    _TrackPopupMenuR(HWindow,point,r,sm,chkDisable);
};

void TWindow::TrackPopupMenu(POINT point,TSTRATUMMENU*sm,BOOL chkDisable)
{
    _TrackPopupMenu(HWindow,point,sm,chkDisable);
};
BOOL TWindow::GetBallonStr(BALLONSTRUCT*)
{
    return 0;
};

void AddRecentlyProject(char *p){
    if (mainwindow)mainwindow->AddRecentlyProject(p);
};
void TMainWindow::RemoveRecently(char*p){
    //C_TYPE _c=recently->count;
    for(C_TYPE i=0;i<recently->count;i++){
        char *s=(char *)recently->At(i);
        if (!lstrcmpi(s,p)){
            recently_modifyed=1;
            recently->AtDelete(i);
            RemoveRecentlyMenu();
            RebuildRecentlyMenu();return;
        }
    }
};
void TMainWindow::RemoveRecentlyMenu(){
    int count=GetMenuItemCount(menu_recently);
    int _id=GetMenuItemID(menu_recently,count-3);
    int sep=GetMenuItemID(menu_recently,count-2);
    if ((sep==0)&&
        (_id>=CM_RECENTLY)&&(_id<=CM_RECENTLY+11)){
        count-=2;
        RemoveMenu(menu_recently,count,MF_BYPOSITION);
        count--;
        while (GetMenuItemID(menu_recently,count)){
            RemoveMenu(menu_recently,count,MF_BYPOSITION);
            count--;
        }
    }
};

void TMainWindow::AddRecentlyProject(char*p){
    if (p && lstrlen(p)){
        const max_inlist=9;
        C_TYPE _c=recently->count;
        for(C_TYPE i=0;i<recently->count;i++){
            char *s=(char *)recently->At(i);
            if (!lstrcmpi(s,p)){
                if (i!=0){
                    recently_modifyed=1;
                    recently->AtDelete(i);
                    recently->AtInsert(0,s);
                    goto m1;
                }else return;
            }
        }

        while(recently->count>=max_inlist){
            char *s=(char *)recently->At((C_TYPE)(recently->count-1));
            recently->AtDelete((C_TYPE)(recently->count-1));
            delete s;
        }
        recently->AtInsert(0,NewStr(p));
        recently_modifyed=1;

m1:
        RemoveRecentlyMenu();
        RebuildRecentlyMenu();
        StoreDesktop();
    }
};
void TWindow::EvSize(WORD sizeType,int sx,int sy){
    switch(sizeType){
        case SIZE_MINIMIZED:
        case SIZE_MAXHIDE  :return;
        default:
            if (vscroll){vscroll->ChangeSize(sy);}
            if (hscroll){hscroll->ChangeSize(sx);}
    }
};


BOOL IsWindowCombo(HWND h){
    char s[20];
    GetClassName(h,s,19);
    return (!lstrcmpi(s,"COMBOBOX"));
};

void TMainWindow::EvRButtonDown(WORD modKeys,int,int){
    POINT point;
    // point.x=x;point.y=y;
    //ClientToScreen(HWindow,&point);
    GetCursorPos(&point);
    HWND hw=WindowFromPoint(point);
    if(hw==HWindow)
    {
        POINT p1=point;
        ScreenToClient(HWindow,&p1);
        if(!PtInRect(&mdirect,p1)){
            HMENU p=CreateControBarMenu();
            TSTRATUMMENU sm;
            sm.hWindow=HWindow;
            sm.hMenu=p;
            sm.hType=SCMENU_CONTROLBAR;

            TrackPopupMenu(point,&sm);
        }
    }
};
static HWND dropcombo=0;
void    TWindow::CloseCombo(){
    if (dropcombo){
        if (IsWindowCombo(dropcombo))
            SendMessage(dropcombo,CB_SHOWDROPDOWN,(WPARAM)FALSE, 0);
        dropcombo=0;
    }
};
LRESULT TWindow::_WndProc(UINT message, WPARAM wParam,LPARAM lParam)
{
    if(messages)
    {
        BOOL process;
        messages->lock++;
        LRESULT ret=messages->WndProc(message,wParam,lParam,process);
        messages->lock--;
        if(process)return ret;
    }
    return WndProc(message,wParam,lParam);
};

LRESULT TWindow::WndProc(UINT message, WPARAM wParam,LPARAM lParam)
{
    if (message==ballon_message)
    {
        return GetBallonStr((BALLONSTRUCT*)lParam);
    }
    if (message==scwnd_message)
    {
        return ((LRESULT)this);
    }

    switch(message)
    {
        case WM_KEYDOWN:
            if(CanUseDelete)
            {
                if(wParam==VK_DELETE)
                {
                    if((GetAsyncKeyState(VK_CONTROL)>=0)&&
                       (GetAsyncKeyState(VK_MENU)>=0)&&
                       (GetAsyncKeyState(VK_SHIFT)>=0))
                    {
                        if(mainwindow->IsCommandDisable(CM_EDITDELETE)==0)
                        {
                            mainwindow->EvCommand(CM_EDITDELETE,0,0);
                        };
                    }
                    return 0;
                };
            };break;
        case WM_QUERYENDSESSION:if(!CanClose())return FALSE;return TRUE;
        case WM_CLOSE:if(!CanClose())return 0;break;
        case WM_SYSCOMMAND: // close comboboxes if dropped down
            CloseCombo(); break;
        case WM_MOUSEMOVE:EvMouseMove((WORD)wParam,long_to_point);return 0;
        case WM_LBUTTONDBLCLK:EvLButtonDblClk((WORD)wParam,long_to_point);return 0;
        case WM_LBUTTONDOWN:
            if(GetFocus()!=HWindow)SetFocus(HWindow);
            EvLButtonDown((WORD)wParam,long_to_point);return 0;
        case WM_LBUTTONUP:EvLButtonUp((WORD)wParam,long_to_point);return 0;

        case WM_RBUTTONDOWN:{
            if(GetFocus()!=HWindow)SetFocus(HWindow);
            EvRButtonDown((WORD)wParam,long_to_point);}return 0;
        case WM_RBUTTONUP:EvRButtonUp((WORD)wParam,long_to_point);return 0;
        case WM_RBUTTONDBLCLK:EvRButtonDblClk((WORD)wParam,long_to_point);return 0;

        case WM_SIZE:EvSize((WORD)wParam,LOWORD(lParam),HIWORD(lParam));return 0;
        case WM_VSCROLL:if (vscroll){vscroll->Process(wParam,lParam);return 0;}else break;
        case WM_HSCROLL:if (hscroll){hscroll->Process(wParam,lParam);return 0;}else break;

        case WM_SETCURSOR:if(EvSetCursor((HWND)wParam,LOWORD(lParam),0))return 0;break;

        case TW_ACCEPTDROP:return AcceptDrop(*(DRAGDROPSTRUCT*)((void*)(lParam)));
        case TW_DROP      :return ProcessDrop(*(DRAGDROPSTRUCT*)((void*)(lParam)));
        case WM_COMMAND:{
            EV_COMMAND
                    if (command>=100 && command<16384){
                if ((wNotifyCode == CBN_DROPDOWN) ||
                    (dropcombo&&(wNotifyCode == CBN_CLOSEUP)))
                { if (IsWindowCombo(hwndCtl))
                    {
                        if (wNotifyCode == CBN_DROPDOWN){
                            dropcombo=hwndCtl;
                        };
                        if (dropcombo && (wNotifyCode == CBN_CLOSEUP)){
                            dropcombo=0;
                        }
                    }
                }
                EvCommand(command,hwndCtl,wNotifyCode);
                return 0;
            }else
            if((command>=16384)&&(command<20000)){
                dllmanager->ForEach(SC_COMMAND,(UINT32)HWindow,command);
                return 0;
            }
        };break;
        case WM_ENTERIDLE:{
            if (!paused){
                PauseAll();paused=1;
            }
            SetBallonData(0,0,0);
        }break;
    }
    return DefWindowProc(message,wParam,lParam);
};

void TWindow::EvCommand(UINT command,HWND /*hwndCtl*/,WPARAM wNotifyCode)
{
    // DefWindowProc(HWindow,command,MAKELONG(hwndCtl,wNotifyCode));
}

LRESULT TWindow::DefWindowProc(UINT m, WPARAM w,LPARAM l){
    return ::DefWindowProc(HWindow,m,w,l);
};
int TWindow::IsCommandDisable(int){
    return 2;
};
BOOL TWindow::AcceptDrop(DRAGDROPSTRUCT&){return FALSE;};
BOOL TWindow::ProcessDrop(DRAGDROPSTRUCT&){return FALSE;};

BOOL TWindow::SetCursor(HINSTANCE module,LPCSTR resId)
{
    if (module == CursorModule && resId == CursorResId)
        return FALSE;

    HCURSOR hOldCursor = (HCursor && CursorModule) ? HCursor : 0;
    CursorModule = module;
    CursorResId = resId;
    if (CursorResId)
        if (CursorModule)
            HCursor = LoadCursor(CursorModule,CursorResId);
        else
            HCursor = ::LoadCursor(0, CursorResId);
    else
        HCursor = 0;

    if(HDefCursor && HCursor==0)
        HCursor=HDefCursor;

    ::SetCursor(HCursor);
    if (hOldCursor)
        ::DestroyCursor(hOldCursor);
    return TRUE;
};

BOOL TWindow::SetDefCursor(HINSTANCE module,LPCSTR resId)
{
    if(resId)
    {
        DefCursorResId=resId;
        DefCursorModule=module;
        ::DestroyCursor(HDefCursor);
        HDefCursor=NULL;
    }
    if(HDefCursor!=NULL)
        return SetCursor(0,(LPCSTR)NULL);
    return SetCursor(DefCursorModule,DefCursorResId);
}

BOOL TWindow::LoadDefCursor(char* filename)
{
    char s[260];
    GetFullFileNameEx((TProject*)GetCurrentProject(), filename, s);
    HCURSOR c = LoadCursorFromFile(s);
    if(c)
    {
        HDefCursor = c;
        SetCursor(0, (LPCSTR)1);
        return SetCursor(0, (LPCSTR)0);
    }
    return FALSE;
}

BOOL TWindow::EvSetCursor(HWND hWndCursor, UINT hitTest, UINT /*mouseMsg*/)
{
    if (hWndCursor == HWindow && hitTest == HTCLIENT && HCursor)
    {
        ::SetCursor(HCursor);
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------------------//
TMainWindow*mainwindow=0;
//--------------------------------------------------------------------------//

TMainWindow::TMainWindow(LPCREATESTRUCT lp,HWND hw):TWindow(lp,hw)
{
    hinet=0;
    fullscreen=0;
    viewcontrol=1;
    lastactivemdi=NULL;
    taskactive=TRUE;
    menu_logo=0;
    docks[0]=0;
    CLIENTCREATESTRUCT ClientAttr; // far
    mainwindow=this;
    bm_home=NULL;
    menu = LoadMenu(HResource,MAKEINTRESOURCE(MAIN_MENU));
    SetMenu(HWindow,menu);
    menu_recently=GetSubMenu(menu,0);
    InsertMenu(menu_recently,0, MF_OWNERDRAW|MF_ENABLED|MF_BYPOSITION,CM_MENU_LOGO,"");
    ClientAttr.hWindowMenu =
        #ifdef RUNTIME
            GetSubMenu(menu,1);// Номер popup-меню в главном меню
#else
            GetSubMenu(menu,7);// Номер popup-меню в главном меню
#endif
    ClientAttr.idFirstChild = 32001;
    statusbar=controlbar=0;
    controlHeight=BUTTON_HEIGHT+6;
    statusHeight=22;
    for(int i=0;i<4;i++)docks[i]=new TControlDock(i+1);
    BOOL ic=InitControlBars(docks);
    //  if(ic) ShowControlBar(1,TRUE);
    WriteTitleStr("Init MDI window");
#ifdef WIN32
    mdi=CreateWindowEx(WS_EX_CLIENTEDGE,
                   #else
    mdi=CreateWindow(
        #endif
            "MDICLIENT",0,WS_CLIPCHILDREN|WS_HSCROLL|WS_VSCROLL|WS_CLIPSIBLINGS|
            WS_VISIBLE|WS_CHILD,0,0,640,480,HWindow,(HMENU)1,(HINSTANCE)hInstance,&ClientAttr);
    //устанавливаем фон mdiclient
    //SetClassLong(mdi, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(113,157,250)));

    WriteTitleStr("Init status window....");
    statusbar=CreateWindow("SC_STATUS_BAR",0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE|WS_CHILD,
                           0,480,640,statusHeight,HWindow,(HMENU)2,(HINSTANCE)hInstance,NULL);
    _StatusBar=(TStatusBar*)(GetWindowLong(statusbar,0));
    recently=new TCollection(5,1);
    WriteTitleStr("Init drag & drop...");
    DragAcceptFiles(HWindow,TRUE);
    HMENU hm=GetSystemMenu(HWindow,FALSE);
    ScAppendMenu(hm,MF_ENABLED,CM_CONTROLBARS,             IDS_MENU62);
    ScAppendMenu(hm,MF_ENABLED|MF_CHECKED,CM_VIEWSTATUSBAR,IDS_MENU63);
    ScAppendMenu(hm,MF_ENABLED|MF_CHECKED,CM_VIEWMAINMENU, IDS_MENU64);
    if(!KeyHook)
    {
        KeyHook = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardProc,0,GetCurrentThreadId());
        //KeyHook = SetWindowsHookEx(WH_CALLWNDPROC,(HOOKPROC)KeyboardProc,0,GetCurrentThreadId());
        //KeyHook = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardProc,GetModuleHandle(NULL),0);
    }
};

void TMainWindow::RebuildRecentlyMenu()
{
    int pos=FIRST_RECENTLY;
    char s[256];
    InsertMenu(menu_recently,pos,MF_BYPOSITION|MF_SEPARATOR,0,"");
    if (recently->count)
    {
        for(int i=0;i<recently->count;i++)
        {
            pos++;
            wsprintf(s,"&%d %s",i+1,(char*)recently->At(i));
            AnsiLower(s);;
            InsertMenu(menu_recently,pos,MF_BYPOSITION|MF_ENABLED| MF_STRING,CM_RECENTLY+i,s);
        }
    }
    else
    {
        LoadString(HResource,IDS_str71,s,sizeof(s));
        InsertMenu(menu_recently,pos+1,MF_BYPOSITION|MF_ENABLED| MF_STRING,CM_NORECENTLY,s);
    }
};

/*
void CloseProjectWindows(void * project){
#ifdef LOGON
 LogMessage("Зашел в CloseProjectWindows()");
 TLOGoffset __offset(2);
#endif

 if (mainwindow){
  C_TYPE i=0;
  while (i<winmanager->allmdi->count){
      TMDIChildWindow *w=(TMDIChildWindow *)winmanager->allmdi->At(i);
     if ((w->prj==project)||(project==NULL))
       SendMessage(mainwindow->GetMDI(),WM_MDIDESTROY,(WPARAM)w->HWindow,0);
        else i++;
    }
 }
};
*/

LRESULT TMainWindow::DefWindowProc(UINT m,WPARAM w,LPARAM l){
    return ::DefFrameProc(HWindow,mdi,m,w,l);
};

void TMainWindow::Cm_ClearVar(){
    TProject * p = (TProject*)GetCurrentProject();
    if(p)p->ClearVariables();
};

void TMainWindow::Cm_ClearStream(){
    TProject * p = (TProject*)GetCurrentProject();
    if(p)p->CloseWindows();
};

void TMainWindow::Cm_ClearWindows(){
    TProject * p = (TProject*)GetCurrentProject();
    if(p)p->CloseWindows();
};

void TMainWindow::Cm_ClearAll()
{
    Stop();

    EXEC_FLAGS=0;
    calcedsteps=0;
    SetLastSteps();

    if(project)
        project->ResetProject();

    CloseAllVideos();
};

int  TMainWindow::IsCommandDisable(int command)
{
    if ((command>=CM_CONTROLRANGE)&&
        (command<=CM_ENDCONTROLRANGE))
        return 0;
    switch (command){
        case CM_DBE_EXPLORER:
        {
#ifdef DBE
            return 0;
#endif
        }return 200;
        case CM_OPENURL:
        case CM_ORDER:
        case CM_WEBSUPPORT:
        case CM_TUTORIAL:
        case CM_REMIND:
        case CM_REGISTER:return 0;
        case CM_NEXTPAGE:if(winmanager->CanRedo())return 0;return DM_NONEXTPAGE;//break;
        case CM_PREVPAGE:if(winmanager->CanUndo())return 0;return DM_NOPREVTPAGE;//;break;
        case CM_FULLSCREEN:
        case CM_PRIMITIVE3D:
        case CM_SUPPORT:
        case CM_CONTROLBARS:
        case CM_PRINTERSETUP:
        case CM_PROPERTIESobj2d:return 0;
        case CM_EDITDELETEobj2d:
        case CM_VIEWSTATUSBAR:return SCGetScVar("StatusMode",2)!=0;
        case CM_VIEWTOOLBAR  :return viewcontrol;
        case CM_CLOSEVIDEOS:
            if (CanCloseAllVideos())
                return 0;
            return DM_NOVIDEOSTREAM;
        case CM_STATUSTOP:return DM_NOTREALIZED;
        case CM_SAVEALLEX:
        case CM_SAVEALL:if(project && project->hyperadd && project->hyperadd->save==0)return DM_READONLY;
        case CM_WEBSTRATUM:
        case CM_WEBQSC:
        case CM_HOMEPAGE:
        case CM_PLUGINS:
        case CM_VIEWMAINMENU:
        case CM_HIDESTATUSBAR:
        case CM_STATUSBOTTOM:
        case CM_HELPONIMAGE:
        case CM_DESCKTOP:
        case CM_SYSTEMINFO:
        case CM_COMPILE:
        case CM_VIEWMESSAGES:
        case CM_HELPGRAPI:
        case CM_HELPTHEORY:
        case CM_HELPINDEX1:
        case CM_SETASDEF:
        case CM_LIBHELP:
        case CM_HELPUSING_HELP:
        case CM_HELPKEYBOARD:return 0;
        case CM_CLEARALL:
        {
            PObject object;
            PClass _class; // view main scheme
            if(GetRootObject(&_class,&object))return 0;
            if (streams->count)return 0;
            if (winmanager->count)return 0;
#ifdef DBE
            int p;DBSQLPParam(p);
            if (p&2)return 0;
#endif
        };return DM_NOPROJECT;
#ifdef DBE
        case CM_DOSQL:
        case CM_CLOSEDATABASES:{int p;DBSQLPParam(p);if (p&2)return 0;else return DM_NOBASES;}
#endif
        case CM_CLOSESTREAM:if (streams->count)return 0;else return DM_NOSTREAMS;
        case CM_CLOSEWINDOWS:if (winmanager->count)return 0;else return DM_NOWINDOWS;
        case CM_SETVAR:
        case CM_VARTODEF:
        case CM_VARCLEAR:
        {
            PObject object;
            PClass _class; // view main scheme
            if(GetRootObject(&_class,&object))
                return 0;
        } return DM_NOPROJECT;
        case CM_PAUSE:if (Running())return 0;else return DM_NORUNNING;
        case CM_RUN:
        case CM_ONESTEP:{PObject object;PClass _class; // view main scheme
            if (Running())return DM_RUNNING;
            if(GetRootObject(&_class,&object))return 0;}return DM_NOPROJECT;
        case CM_CLOSEEDITCHILDREN:{
            for(C_TYPE i=0;i<winmanager->allmdi->count;i++){
                TMDIChildWindow *w=(TMDIChildWindow *)winmanager->allmdi->At(i);
                if (!(w->flags&WF_PROGRAM))return 0;
            }
            return DM_NOEDITORS;
        };

        case CM_CASCADECHILDREN:
        case CM_TILEHORZ:
        case CM_TILEVERT:
        case CM_ARRANGEICONS:
        case CM_CLOSECHILDREN:if (GetWindow(mdi,GW_CHILD))return 0;else return DM_NOALLWINDOWS;
            //--------------
        case CM_HELPABOUT:
        case CM_EXIT:
        case CM_LIBRARYS: return 0;
            //--------------
        case CM_CLOSECHILDPROJECT:{
            TProject*prj=(TProject*)GetCurrentProject();
            if(prj){
                if(prj->projecttoactivate){
                    return 0;
                }
                return DM_NOCHILDPROJECT;
            }
            return DM_NOPROJECT;
        };
        case CM_SAVEPROJECTAS:
        case CM_SAVETODEFAULT:;
        case CM_LOADFROMDEFAULT:;
        case CM_LOADSTATE:;
        case CM_STATESAVE:;
        case CM_HIERARCHY:
        case CM_PROJECTCLOSE:
            //  case CM_SAVE_AS:
        case CM_PROJECTOPTIONS:
        case CM_WATCH:
        case CM_MAIN:{PObject object;PClass _class; // view main scheme
            if(GetRootObject(&_class,&object))return 0;}return DM_NOPROJECT;
            //--------------

        case CM_NEWMATRIX:return DM_NOTREALIZED;
        case CM_NEWSPACE:
        case CM_FILEOPEN:return 0;
        case CM_NEWSIMPLYPROJECT:
        case CM_NEWPROJECT:{/*
     PObject object;PClass _class;
     if(GetRootObject(&_class,&object))return DM_PROJECTLOAD;else*/
#ifdef BETA
            return DM_BETA;
#else
            return 0;
#endif
        };
            //--------------
    }
    HWND mdihwnd=(HWND)SendMessage(mdi,WM_MDIGETACTIVE,0,0);
    if (mdihwnd){
        TMDIChildWindow * win=(TMDIChildWindow *)GetWindowLong(mdihwnd,0);
        int h=2;
        if (win->msgserver)h=win->msgserver->IsCommandDisable(command);
        if (h!=2)return h;
        if (win && win->client)return win->client->IsCommandDisable(command);else
            return win->IsCommandDisable(command);
    }
    return DM_NOHANDLER;
};

int  TSpaceWindow::IsCommandDisable(int command)
{
    switch (command)
    {
        case CM_REPAINT:
        case CM_SCALEDIALOG:
        case CM_SCALEBOX:
        case CM_PRINT:
        case CM_FILESAVEAS:
        case CM_SAVESCHEME:return 0;
    }
    return DM_NOHANDLER;
};

BOOL TSpaceWindow::Cm_SaveAs(BOOL settitle)
{
    char s[2560];
    char * filename=GetSpaceFile2d(hsp);
    if(filename)lstrcpy(s,filename);
    else
    {
        if(GetTexturePath2d(hsp,s))  lstrcat(s,"\\noname.vdr");
        else *s=0;
    }
    if(FileDialog(HWindow,s,IDS_EXPORT,FD_SAVE))
    {
#ifdef DEMO
        DemoMSG();
#else
        if(!lstrcmpi(filesavetype,"wmf"))
        {
            SimplyExchange(hsp,s,"wmf",1);
        }
        else
        {
            if(State2dSaveAs(hsp,s))
            {
                AddRecentlyProject(s);
                if(settitle)   	SetWindowText(GetParent(HWindow),s);
                return TRUE;
            }
        }
#endif
    }
    return 0;
};

BOOL TSpaceWindow::Cm_Save(BOOL settitle)
{
    char * filename=GetSpaceFile2d(hsp);
    if (filename==NULL)
    {
        if(Cm_SaveAs(settitle))
        {
            if (settitle)
            {
                filename=GetSpaceFile2d(hsp);
                AddRecentlyProject(filename);
            }
        }return FALSE;
    }
    else
#ifdef DEMO
        DemoMSG();
#else
        return (State2dSave(hsp));
#endif
};

void TSpaceWindow::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode)
{
    switch (command)
    {
        case CM_SCALEDIALOG:
        {
            POINT2D _m,_d;
            GetScaleSpace2d(hsp,&_m,&_d);
            double s=_m.x;s/=((double)_d.x);
            s=ScaleDialog(HWindow,s);
            if(s>0)
            {
                _d=TPoint(100,100);_m=TPoint(s*100,s*100);
                SetScaleSpace2d(hsp,&_m,&_d);
                UpdateScrollers();
                SetScaleBox(s);
            };
        }break;

        case CM_REPAINT:
            InvalidateRect(HWindow,0,0);
            UpdateWindow(HWindow);
            break;
        case CM_PRINT:
            SpacePrintDialog(HWindow,hsp);break;
        case CM_FILESAVEAS:
        case CM_SAVESCHEME:
        {
            Cm_SaveAs();
        }break;
        case CM_SCALEBOX:
            if (scaling)
            {
                SetFocus(HWindow);
                double sc=GetScaleBox();
                POINT2D m,d;
                if (sc==1.0){m.x=m.y=d.x=d.y=1;}
                else
                {
                    d.x=d.y=100;
                    m.x=m.y=sc;
                    RECT rect;
                    GetClientRect(HWindow,&rect);

                    Zoom((rect.right-rect.left)/2,(rect.bottom-rect.top)/2,0,&m,&d);

                    UpdateScrollers();
                }
            }break;
        case CM_SCALETOFIT   :
        case CM_SCALETOCUSTOM:
            if (scaling)
            {
                SetFocus(HWindow);
                POINT2D m;
                m.x=m.y=1;
                SetScaleSpace2d(hsp,&m,&m);
                UpdateScrollers();
                UpdateScaleBox();
            }break;
        default:
            TWindow::EvCommand(command,hwndCtl,wNotifyCode);
    }
};

TSpaceWindow*TMainWindow::GetWindowWithSpace(HSP2D hsp){
    HWND old=GetSpaceWindow2d(hsp);
    if (old){
        HWND child=GetParent(old);
        SendMessage(mdi,WM_MDIACTIVATE,(WPARAM)child,0l);
        return (TSpaceWindow*)GetWindowLong(old,0);
    }// Test for alredy opening this class scheme
    return NULL;
};

BOOL GetModelTextWindow(PClass _class,HWND*mdi,HWND*edit)
{
#ifndef RUNTIME
    for(int i=0;i<winmanager->allmdi->count;i++)
    {
        TMDIChildWindow * c=(TMDIChildWindow*)winmanager->allmdi->At(i);
        if (c && (c->MODE == MODE_EDITORTEXT) && c->msgserver)
        {
            TEditMsgServer* ser=(TEditMsgServer*)c->msgserver;
            if (ser->_class==_class)
            {
                if(mdi)*mdi=c->HWindow;
                if(edit)*edit=ser->edit;
                return TRUE;
            }
        }
    }
#endif
    return FALSE;
}

#ifndef RUNTIME
int TMainWindow::CreateTextWindow(char *file,int mode)
{
    TDOSStream st(file,TDOSStream::stOpenRead);
    if(st.status)
        return 0;
    long size=max(st.GetSize(),32000);
    char *txt=new char [size+1];
    st.Read(txt,(int)size);
    txt[size]=0;
    if(mode)
        OemToAnsi(txt,txt);
    HWND edit=0;
    if(SCGetScVar("use_syntax_edit",0) && InitSyntax())
    {
        edit=CreateWindowEx(
         #ifdef WIN32
                 WS_EX_CLIENTEDGE
         #else
                 0
         #endif
                 ,"SC_SyntaxEdit",0,WS_CLIPCHILDREN|WS_CHILD,
                 0,0,640,480,HWindow,(HMENU)10,(HINSTANCE)hInstance,NULL);
        SetDefaultSyntaxColor(0,edit);
    }
    else
    {
        edit=CreateWindowEx(
         #ifdef WIN32
                 WS_EX_CLIENTEDGE
         #else
                 0
         #endif
                 ,"EDIT",0,ES_AUTOHSCROLL	|ES_AUTOVSCROLL|WS_CLIPCHILDREN|
                 ES_LEFT|ES_MULTILINE|ES_WANTRETURN|WS_CHILD|WS_VSCROLL|WS_HSCROLL,
                 0,0,640,480,HWindow,(HMENU)10,(HINSTANCE)hInstance,NULL);
        SendMessage(edit,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
    }
    SetWindowText(edit,txt);
    if(txt) delete  txt;
    TMDIChildWindow * child=CreateMDIChild(file,0,edit,NULL,MODE_EDITORTEXT);
    child->prj=NULL;
    TEditMsgServer* ser=new TEditMsgServer(NULL,child->HWindow);
    ser->edit=edit;
    ser->_class=0;
    ser->mode=mode;
    lstrcpy(ser->filename,file);
    child->msgserver=ser;
    return 1;
};

void GetTextWindowName(PClass _class,char*s)
{
    char buf[20];
    LoadString(HResource,IDS_str231,buf,sizeof(buf));
    wsprintf(s,buf,_class->name);
};

int TMainWindow::CreateTextWindow(PClass _class)
{
    if (_class)
    {
        HWND _mdi;
        if(GetModelTextWindow(_class,&_mdi,NULL))
        {
            SendMessage(mdi,WM_MDIACTIVATE,(WPARAM)_mdi,0);
            return 1;
        }
        char s[256];
        GetTextWindowName(_class,s);
        HWND edit=0;
        if(SCGetScVar("use_syntax_edit",0) && InitSyntax())
        {
            edit=CreateWindow("SC_SyntaxEdit",0,WS_CLIPCHILDREN|WS_CHILD,
                              0,0,640,480,HWindow,(HMENU)10,(HINSTANCE)hInstance,NULL);
            SetDefaultSyntaxColor(_class,edit);
        }
        else
            edit=CreateWindow("EDIT",0,ES_AUTOHSCROLL	|ES_AUTOVSCROLL|WS_CLIPCHILDREN|
                              ES_LEFT	|ES_MULTILINE|ES_WANTRETURN|WS_CHILD|WS_VSCROLL|WS_HSCROLL,
                              0,0,640,480,HWindow,(HMENU)10,(HINSTANCE)hInstance,NULL);
        if(_class->Protected())
            SendMessage(edit,EM_SETREADONLY,1,0);
        char *txt=GetClassText(_class);
        SetWindowText(edit,txt);
        if (txt) delete  txt;
        SendMessage(edit,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
        TMDIChildWindow * child=CreateMDIChild(s,0,edit,NULL,MODE_EDITORTEXT);
        child->prj=_class->GetProject();
        child->helpctx=Text_edit_Window;
        PClassListInfo pcli=_class->GetClassListInfo();
        if (pcli->flags&CLF_PROJECT)
            child->flags|=WF_PROJECT;
        lstrcpy(s,"@Text@");
        lstrcat(s,_class->name);
        winmanager->PostCreateMDIWindow(s,child,0);
        TEditMsgServer* ser=new TEditMsgServer(NULL,child->HWindow);
        ser->edit=edit;
        ser->_class=_class;
        child->msgserver=ser;
        // SetControlBarRange(RANGE_TEXT);
        return 1;
    }
    return 0;
};
#endif // runtime

#ifdef CAN_EDIT_BITMAP
TBMPChild* TMainWindow::CreateBmpWindow(char *,HGLOBAL _xor,HGLOBAL _and){

    MDICREATESTRUCT md;
    md.szClass="SC_BMPCHILD";
    md.szTitle="Bitmap";
    md.hOwner=hInstance;
    md.style=MDIS_ALLCHILDSTYLES;
    md.y=md.x=CW_USEDEFAULT;
    md.cy=400;md.cx=400;

    HWND active=(HWND)SendMessage(mdi,WM_MDIGETACTIVE,0,0);
    if (active && IsZoomed(active))md.style|=WS_MAXIMIZE;
    md.lParam=NULL;
    ShowControlBar(40,TRUE);
    HWND win=(HWND)SendMessage(mdi,WM_MDICREATE,0,(LPARAM)&md);


    TBMPChild* b= (TBMPChild*)GetWindowLong(win,0);
    b->OpenBMP(_xor,_and);

    return b;
};
#endif


#ifndef RUNTIME
void GetSchemeWindowName(PClass _class,char*s){
    LoadString(HResource,IDS_str86,s,64);
    lstrcat(s,_class->name);
    if(_class->Protected()){
        char ss[64];
        LoadString(HResource,IDS_str169,ss,sizeof(ss));
        lstrcat(s,ss);
    }
}

TSchemeWindow *TMainWindow::CreateSchemeWindow(char* name,PObject object)
{
    PClass _class=name ? GetClassByName(name) : NULL;
    if (object && (!_class))
        _class = object->_class;

    if (!_class)
        return NULL;

#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в TMainWindow::CreateSchemeWindow(%s)",_class->name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    HSP2D hsp = GetClassScheme(name, &_class);
    if (!hsp)
    {
        if( MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG29),_MSG_QSC,MB_DEFBUTTON2|MB_YESNOCANCEL|MB_ICONQUESTION	)==IDYES)
        {
            CreateDefClassScheme(name);
            hsp = GetClassScheme(name,&_class);
            if(!hsp)
                return 0;
        }
        else return 0;
    }
    /////////////
    TSchemeWindow* alredy = (TSchemeWindow*)GetWindowWithSpace(hsp);
    if (alredy)
    {
        alredy->SetClass(_class, object);
        return alredy;
    }
    HWND hsw = CreateWindow("SC_Scheme_WINDOW", 0, WS_CLIPCHILDREN | WS_CHILD,
                            0,0, 640,480,
                            HWindow,(HMENU) 10, (HINSTANCE)hInstance,NULL);
    if (hsw)
    {
        TSchemeWindow* sw = (TSchemeWindow*)GetWindowLong(hsw,0);
        sw->SetSpace(hsp);
        sw->SetClass(_class, object);
        AssignWindowToSpace2d(hsp,hsw);
        sw->scheme = _class->scheme;
        _class->scheme->window = sw;
        _class->scheme->flags|=SF_EDITING;

        sw->hardmode=SCHEME_EDITOR;

        char s[256];
        GetSchemeWindowName(_class,s);

        TMDIChildWindow * child=CreateMDIChild(s,sw,NULL,NULL,MODE_EDITOR2D);
        child->helpctx=Edit_scheme;
        child->prj=_class->GetProject();
        sw->SetReadOnly(_class->Protected());
        lstrcpy(s,"@Scheme@");
        lstrcat(s,_class->name);

        winmanager->PostCreateMDIWindow(s,child,hsp);

        PClassListInfo pcli=_class->GetClassListInfo();
        if (pcli->flags&CLF_PROJECT)
            child->flags|=WF_PROJECT;

        int _FALIED=_class->ChkScheme(hsp);
        if(_FALIED)
        {
            // правим схему
            sw->Repair(_FALIED);
        }
        return sw;
    }
    return 0;
};
#endif
BOOL TSpaceWindow::EvQueryNewPalette(){
    HDC hdc;
    HPALETTE hpalWnd=GetColorPalette2d(hsp) , hpalT;
    if (hpalWnd){
        UINT i;
        hdc = GetDC(HWindow);
        hpalT = SelectPalette (hdc, hpalWnd, FALSE);
        i = RealizePalette(hdc);
        SelectPalette (hdc, hpalT, FALSE);
        ReleaseDC(HWindow, hdc);
        if (i > 0) InvalidateRect(HWindow,NULL,0);
        return i;}return 0;
};

TRestoreCrd::TRestoreCrd(HSP2D _hsp){
    hsp=_hsp;
    PushCrdSystem2d(hsp);
    CRDSYSTEM2D c;
    memset(&c,0,sizeof(c));
    c.type=MMTEXT_COORDINATE2D;
    SetCrdSystem2d(hsp,&c);

};
TRestoreCrd::~TRestoreCrd(){
    PopCrdSystem2d(hsp);
};

void TSpaceWindow::UpdateScrollers(BOOL delta){
    RECT2D r;
    if (GetSpaceExtent2d(hsp,r)){
        TRestoreCrd crd(hsp);
        LPtoDP2d(hsp,(POINT2D*)(&r),2);
        POINT2D org;
        GetOrgSpace2d(hsp,&org);
        r.left+=org.x;r.right+=org.x;
        r.top+=org.y;r.bottom+=org.y;
        int _d_=0;
        if (hscroll){

            hscroll->SetRange(r.left-_d_,r.right+_d_);
        }
        if (vscroll){
            vscroll->SetRange(r.top-_d_,r.bottom+_d_);
        }
    }else{
        if (vscroll){vscroll->SetRange(0,0);vscroll->SetPos(0);}
        if (hscroll){hscroll->SetRange(0,0);hscroll->SetPos(0);}
    }
};
void TSpaceWindow::EvPaletteChanged(HWND hWndPalChg){
    if(hWndPalChg!=HWindow)EvQueryNewPalette();
}
#ifndef RUNTIME
void  GetImageWindowName(PClass _class,char*s)
{
    LoadString(HResource,IDS_str87,s,64);
    lstrcat(s,_class->name);

    if(_class->Protected()){
        char ss[64];
        LoadString(HResource,IDS_str169,ss,sizeof(ss));
        lstrcat(s,ss);
    }
}

TSchemeWindow*TMainWindow::CreateImageWindow(char*name){

#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в TMainWindow::CreateImageWindow(%s)",name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    PClass _class=GetClassByName(name);

    HSP2D hsp=GetClassImage(_class);
    if (!hsp){
        CreateClassImage(name,0,0);
        hsp=GetClassImage(_class);
        if (!hsp)return 0;
    }

    TSchemeWindow * alredy=(TSchemeWindow*)GetWindowWithSpace(hsp);
    if (alredy)return alredy;
    HWND hsw=CreateWindow("SC_Scheme_WINDOW",0,WS_CLIPCHILDREN|
                          WS_CHILD,0,0,640,480,HWindow,(HMENU)10,(HINSTANCE)hInstance,NULL);
    TSchemeWindow*sw=(TSchemeWindow*)GetWindowLong(hsw,0);
    sw->SetReadOnly(_class->Protected());
    sw->_class=_class;
    AssignWindowToSpace2d(hsp,hsw);
    sw->hardmode=IMAGE_EDITOR;
    sw->SetSpace(hsp);
    char s[256];
    GetImageWindowName(_class,s);

    TMDIChildWindow * child=CreateMDIChild(s,sw,NULL,NULL,MODE_IMAGE);
    
    if (_class)
    {
        child->prj=_class->GetProject();
        PClassListInfo pcli=_class->GetClassListInfo();
        if (pcli->flags&CLF_PROJECT)
            child->flags|=WF_PROJECT;
    }
    sw->scheme = _class->image;

    lstrcpy(s,"@Image@");
    lstrcat(s,_class->name);
    winmanager->PostCreateMDIWindow(s,child,hsp);

    return sw;
};
#endif
TMDIChildWindow* TMainWindow::CreateMDIChild(char*name,TWindow*_child,HWND hwnd,RECT *r,MDI_CHILD_MODE _mode){
    MDICREATESTRUCT md;
    md.szClass="SC_CHILD";
#ifdef WIN32
    char classname[64];
    if(_mode){wsprintf(classname,"SC_CHILD%d",_mode);
        md.szClass=classname;
    }
#endif
    md.szTitle=name;
    md.hOwner=hInstance;
    md.style=MDIS_ALLCHILDSTYLES;
    if (r){
        md.cy=r->bottom;
        md.cx=r->right;
        md.y=r->top;
        md.x=r->left;
    }else {md.cy=md.cx=md.y=md.x=CW_USEDEFAULT;
        HWND active=(HWND)SendMessage(mdi,WM_MDIGETACTIVE,0,0);
        if (active && IsZoomed(active))md.style|=WS_MAXIMIZE;
    }

    TMDICreate c;c.window=_child;if (hwnd)c.hwnd=hwnd;
    else c.hwnd=_child?_child->HWindow:NULL;
    c._mode=_mode;
    md.lParam=(LPARAM)&c;
    HWND win=(HWND)SendMessage(mdi,WM_MDICREATE,0,(LPARAM)&md);
    if (_child)_child->WndProc(TW_SETFOCUS,1,0);
    SetFocus(c.hwnd);
    return (TMDIChildWindow*)GetWindowLong(win,0);

};
void TMainWindow::ShowControl(BOOL show){
    if (show!=viewcontrol){
        viewcontrol=show;
        InvalidateRect(HWindow,0,0);
        UpdateMainWindow();
    }
};
void TMainWindow::SetFullScreen(BOOL full){
    if(fullscreen!=full){
        fullscreen=full;
        UINT32 mask=WS_SYSMENU | 	WS_CAPTION |WS_MAXIMIZEBOX	|WS_MINIMIZEBOX;
        if(full){
            ShowWindow(HWindow,SW_MAXIMIZE);
            UINT32 style=GetWindowLong(HWindow,GWL_STYLE);
            style&=~( mask	);
            SetWindowLong(HWindow,GWL_STYLE,style);
            ShowSysMenu(0);

        }else{
            ShowWindow(HWindow,SW_RESTORE);
            UINT32 style=GetWindowLong(HWindow,GWL_STYLE);
            style|=(mask);
            SetWindowLong(HWindow,GWL_STYLE,style);
            ShowSysMenu(1);

        }

        UpdateMainWindow();
    }
};
void TMainWindow::ShowStatus(INT16 mode){
    INT32 _mode=SCGetScVar("StatusMode",2);
    if (mode!=_mode){
        SCSetScVarI("StatusMode",mode);
        UpdateMainWindow();
        InvalidateRect(HWindow,0,0);
    }
};

void TMainWindow::UpdateMainWindow(){
    RECT r;
    GetClientRect(HWindow,&r);
    EvSize(SIZE_RESTORED,r.right,r.bottom);
};

int TMainWindow::IsControlChild(void*_b){
    TControlBar*b=(TControlBar*)_b;
    POINT p=b->trackpoint;
    ClientToScreen(b->HWindow,&p);
    ScreenToClient(HWindow,&p);
    int d=controlHeight;
    RECT r;
    r.left=r.top=0;
    r.right=sizex;r.bottom=sizey;
    d=controlHeight/2;
    InflateRect(&r,d,d);
    r.bottom-=statusHeight;
    if(PtInRect(&r,p)){
        if(!PtInRect(&mdirect,p))
        {
            if(p.y>=mdirect.top && p.y<mdirect.bottom){
                if(p.x<=mdirect.left)return 4;
                if(p.x>=mdirect.right)return 2;
            }else{
                if(p.y<=mdirect.top)return 1;
                if(p.y>=mdirect.bottom)return 3;
            }
        }

        r=mdirect;
        r.left+=d;r.right-=d;
        r.top+=d;r.bottom-=d;

        if(!PtInRect(&r,p)){
            if (p.x<r.left){return 4;}
            if (p.x>r.right){return 2;}
            if (p.y<r.top){ return 1;}
            if (p.y>r.bottom){return 3;}
        }

    }
    return 0;
};



BOOL  TMainWindow::RemoveControl(void*_b){
    TControlBar*bar=(TControlBar*)_b;
    if(bar->dock){
        if(bar->dock)docks[bar->dock-1]->Remove(bar);
        UpdateControlSize();
        SetChildSize();
        InvalidateRect(HWindow,0,0);
    }
    bar->HardRemove();
    DestroyWindow(bar->HWindow);
}; // Удаляет бар

void* TMainWindow::InsertControl(char*s,int dock){
    BOOL popup=dock==0;
    DWORD style=WS_CLIPCHILDREN|WS_CHILD|WS_CLIPSIBLINGS|dock;

    if (popup)style=WS_CLIPCHILDREN|WS_POPUP;

    controlbar=CreateWindow(ControlBarName,s,style,
                            0,0,0,0,HWindow,0,(HINSTANCE)hInstance,NULL);
    TControlBar*
            bar=(TControlBar*)(GetWindowLong(controlbar,0));
    POINT p;


    bar->dock=dock;
    if(popup){
        //		bar->GetRect(0,p);
        //		bar->Adjust();
        bar->GetRect(0,p);
        bar->_Adjust(0,p,1);

        RECT wr;
        HWND desktop=GetDesktopWindow();

        int _x,_y;
        GetWindowRect(desktop,&wr);

        if((bar->poprect.left+p.x)>wr.right){
            _x=wr.right-p.x;
        }else _x=bar->poprect.left;

        if((bar->poprect.top+p.y)>wr.bottom){
            _y=wr.bottom-p.y;
        }else _y=bar->poprect.top;

        if(_x<0)_x=0;
        if(_y<0)_y=0;

        SetWindowPos(bar->HWindow,0,_x,_y,p.x,p.y,
                     SWP_SHOWWINDOW|SWP_NOZORDER);

    }else{
        bar->GetRect(dock,p);
        SetWindowPos(bar->HWindow,0,bar->poprect.left,bar->poprect.top,p.x,p.y,
                     SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOMOVE);
        docks[bar->dock-1]->InsertBar(bar,0);
        UpdateControlSize();
        SetChildSize();
        InvalidateRect(HWindow,0,0);

    }
};

BOOL  TMainWindow::AdjustControl(void*_b){
    TControlBar*bar=(TControlBar*)_b;
    InvalidateRect(bar->HWindow,0,0);
    if(bar->dock!=bar->_dock){
        BOOL popup=(bar->_dock==0);
        if(bar->dock)docks[bar->dock-1]->Remove(bar);

        if(bar->dock==0 || bar->_dock==0){
            char s[128];
            GetWindowText(bar->HWindow,s,sizeof(s));
            RECT r=bar->trackrect;
            int _dock=bar->_dock;

            DestroyWindow(bar->HWindow);
            DWORD style=WS_CLIPCHILDREN|WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS;
            if (popup)style=WS_CLIPCHILDREN|WS_POPUP;
            style|=_dock;
            controlbar=CreateWindow(ControlBarName,s,style,
                                    r.left,r.top,r.right-r.left,r.bottom-r.top,HWindow,0,(HINSTANCE)hInstance,NULL);
            bar=(TControlBar*)(GetWindowLong(controlbar,0));
            bar->_dock=_dock;
            bar->trackrect=r;
            if (popup){bar->dock=_dock;
                //bar->Adjust();
                POINT __p;
                bar->GetRect(0,__p);
                //      bar->_Adjust(0,__p);
                bar->poprect.right=bar->poprect.left+__p.x;
                bar->poprect.bottom=bar->poprect.top+__p.y;
                bar->trackrect=bar->poprect;
                RECT r=bar->poprect;
                SetWindowPos(bar->HWindow,HWND_TOP,r.left,r.top,
                             r.right-r.left,r.bottom-r.top,
                             SWP_SHOWWINDOW);
            }

        }else{
            if(((bar->dock)&1) != ((bar->_dock&1))){
                bar->RemoveCombos();
                bar->UpdateCombos(bar->_dock);
                if(bar->_dock){
                    POINT __p;
                    bar->GetRect(bar->_dock,__p);
                    SetWindowPos(bar->HWindow,HWND_TOP,0,0,__p.x,__p.y,SWP_NOMOVE|SWP_NOZORDER);
                }
            }
            //docks[bar->dock-1]->Adjust(bar);
        }
        if(bar->_dock)docks[bar->_dock-1]->InsertBar(bar,1);
    }else{
        if(bar->dock==0){
            RECT r=bar->trackrect;
            SetWindowPos(bar->HWindow,HWND_TOP,r.left,r.top,
                         r.right-r.left,r.bottom-r.top,
                         SWP_SHOWWINDOW);
            bar->poprect=r;
        }else{
            docks[bar->dock-1]->Adjust(bar);
        }
    }
    UpdateControlSize();
    SetChildSize();
    InvalidateRect(HWindow,0,0);
    InvalidateRect(mdi,0,0);
    InvalidateRect(bar->HWindow,0,0);
    //  if(bar->_dock)InvalidateRect(HWindow,0,0);
};  // После перемещения

void  TMainWindow::UpdateControlSize(){
    mdirect.left=0;
    mdirect.top=0;
    mdirect.bottom=sizey;
    mdirect.right=sizex;
    INT32 mode=SCGetScVar("StatusMode",2);

    if (statusbar && mode && (!fullscreen)){
        if(mode==2)mdirect.bottom-=statusHeight;
        if(mode==1)mdirect.top+=statusHeight;
    }
    if(viewcontrol && (!fullscreen)){
        mdirect.top+=docks[0]->GetSize();
        mdirect.right-=docks[1]->GetSize();
        mdirect.bottom-=docks[2]->GetSize();
        mdirect.left+=docks[3]->GetSize();
        if(off97){
            //if(docks[0]->count)
            mdirect.top+=5;
            if(docks[1]->count)mdirect.right-=7;
            if(docks[2]->count)mdirect.bottom-=5;
            if(docks[3]->count)mdirect.left+=7;
        }
        docks[0]->_size=docks[2]->_size=sizex;
        docks[1]->_size=docks[3]->_size=mdirect.bottom-mdirect.top;
    }
};


void TMainWindow::SetChildSize(){
    INT32 mode=SCGetScVar("StatusMode",2);
    int miny=0;
    if(mode==1){
        miny=statusHeight;
    }
    // if (controlbar) SetWindowPos(controlbar,NULL,0,0,sizex,controlHeight,SWP_NOZORDER|(viewcontrol?SWP_SHOWWINDOW:SWP_HIDEWINDOW));
    if (mdi)
#ifdef WIN32
        SetWindowPos(mdi,NULL,mdirect.left,mdirect.top,(mdirect.right-mdirect.left),
                     (mdirect.bottom-mdirect.top),SWP_NOZORDER);
#else
        SetWindowPos(mdi,NULL,mdirect.left+2,mdirect.top+2,(mdirect.right-mdirect.left)-3,
                     (mdirect.bottom-mdirect.top)-3,SWP_NOZORDER);
#endif

    // |SWP_NOREDRAW
    for(int i=0;i<4;i++){
        for(C_TYPE j=0;j<docks[i]->count;j++){
            TCollection *c=(TCollection *)docks[i]->At(j);
            for(C_TYPE k=0;k<c->count;k++){
                TControlBar*bar=(TControlBar*)c->At(k);
                int y=j*(docks[i]->size+2);
                int x=bar->start;
                int _x,_y;

                POINT p;
                bar->GetRect(i+1,p);
                switch(i){
                    case 0:_x=x;_y=y+2;
                        break;
                    case 1:_y=x+mdirect.top+2;_x=sizex-y;
                        _x-=docks[i]->size;
                        if(off97){_x+=-2;_y+=-5;}
                        break;
                    case 2:_x=x;_y=(sizey)-y;
                        if(mode)_y-=statusHeight;
                        _y-=docks[i]->size;
                        _y-=2;
                        if(off97){_y+=0;}
                        break;
                    case 3:_x=y;_y=x+mdirect.top+2;
                        if(off97){_x+=2;_y+=-5;}
                        break;
                }
                _y+=miny;
                if(i==1 || i==3){
                    if((_y+p.y)>mdirect.bottom-2){
                        p.y=(mdirect.bottom-2)-_y;
                    }
                }
                if(viewcontrol && (!fullscreen)) SetWindowPos(bar->HWindow,NULL,_x,_y,p.x,p.y,SWP_SHOWWINDOW|SWP_NOZORDER);
                else ShowWindow(bar->HWindow,SW_HIDE);
            }
        }
    } //i
    if (statusbar){
        int y=(mode==2)?(sizey-statusHeight):0;
        SetWindowPos(statusbar,NULL,0,y,sizex,statusHeight,SWP_NOZORDER|((mode)?SWP_SHOWWINDOW:SWP_HIDEWINDOW));
    }
};

void TMainWindow::EvSize(WORD sizeType,int sx,int sy){
    if (sizeType==SIZE_MINIMIZED || SIZE_MINIMIZED==SIZE_MAXHIDE)return;
    sizex=sx;sizey=sy;
    if(docks[0]){
        UpdateControlSize();
        SetChildSize();
#ifdef WIN32
        RECT r;
        GetClientRect(HWindow,&r);
        HRGN hr1=CreateRectRgnIndirect(&r);
        HRGN hr2=CreateRectRgnIndirect(&mdirect);
        CombineRgn(hr1,hr1,hr2,RGN_DIFF);
        InvalidateRgn(HWindow,hr1,0);
        DeleteObject(hr1);
        DeleteObject(hr2);
#endif
    }
};

void TMainWindow::Cm_Exit(){
    if(CanClose()){
        DestroyWindow(HWindow);
    }
};
#ifndef RUNTIME
void TMainWindow::Cm_NewProject(){
#ifdef LOGON
    LogMessage("Зашел в Cm_NewProject()");
    TLOGoffset __offset(2);
#endif

    if( CanCloseProject()){
        CloseProject((TProject*)GetCurrentProject());
        PObject object;PClass _class;
        //	 if(GetRootObject(&_class,&object))return;// if alredy open then return
        char file[256],name[256];
        if(!GetNewProjectPath(file))return;
        lstrcat(file,"\\project.spj");
        lstrcpy(name,"New_Project_Class");
        if(NewProjectDialog(HWindow,file,name)){

            if (CreateNewProject(file,name)){
                if(GetRootObject(&_class,&object)){
                    CreateSchemeWindow(_class->name,object);
                }
            }}}
};

void TMainWindow::Cm_NewSimplyProject(){
#ifdef LOGON
    LogMessage("Зашел в Cm_NewSimplyProject()");
    TLOGoffset __offset(2);
#endif
    if( CanCloseProject()){
        CloseProject((TProject*)GetCurrentProject());
        if(CreateSimplyProject()){
            PObject object;PClass _class;
            if(GetRootObject(&_class,&object)){
                CreateSchemeWindow(_class->name,object);
            }
        }
    }
}
#endif // runtime

BOOL noschemeonload=0;

BOOL TMainWindow::Cm_Load(char *_name,BOOL alternate)
{
#ifdef PROTECT
    if(!IsRegistered())
    {
        if(rand()>(RAND_MAX-RAND_MAX/3))
        {
            if(!ReminderDialog(HWindow))
                return FALSE;
        }
    }
#endif
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в Cm_Load(%s)",_name?_name:"");
        LogMessage(s);
    };
    // LOGOFFSET
    TLOGoffset __offset(2);
#endif
    char s[256]="";
    if (_name)
        lstrcpy(s,_name);
    else
    {
        SCGetScVarS("lastdir",s);
    }
    if(_name || FileDialog(HWindow,s,IDS_PROJECT,FD_READONLY))
    {
#ifdef LOGON
        LogMessage(s);
#endif
        TWaitCursor wait(HWindow);
        UINT type=GetFileType(s,0);
        if(!type && alternate)
        {
#ifndef RUNTIME
            GETFILETYPE ft;
            memset(&ft,0,sizeof(ft));
            ft.file=s;
            if(GetFileTypeDialog(HWindow,ft))
            {
                if(!lstrlen(ft.filter))
                {
                    if(!lstrcmpi(ft.group,"TABLE"))
                    {
                        BOOL std= !lstrcmpi(ft.type,"DBASE") || !lstrcmpi(ft.type,"PARADOX") || !lstrcmpi(ft.type,"ASCII");
                        char *name,*type,*password;
                        if(std)
                        {
                            type=password=name=NULL;
                        }
                        else
                        {
                            type     = ft.type;
                            password = NULL;
                            name     = ft.type;
                        };
#ifdef DBE
                        int base =DBOpenDataBase(name,type,password,"DBIAUTOCLOSE");
                        if(base)
                        {
                            char path[260];
                            GetPath(s,path);
                            DBSetDir(base,path);
                            int table=DBOpenTable(base,s,ft.type,"","",0,"DBIAUTOCLOSE");
                            if(table)
                                winmanager->OpenBrowseWindow(s,table,"");
                            else
                            {
                                DBCloseBase(base);
                                return FALSE;
                            }
                        }
#else
                        return FALSE;
#endif
                        return TRUE;
                    } // end tables
                    if(!lstrcmpi(ft.group,"TEXT"))
                    {
                        int mode=-1;
                        if(!lstrcmpi(ft.type,"dostext"))mode = 1;
                        if(!lstrcmpi(ft.type,"wintext"))mode = 0;
                        if(mode>-1)
                        {
                            CreateTextWindow(s,mode);
                            return TRUE;
                        }
                    }
                }
                HSP2D hsp=0;
                HOBJ2D hobject=0;
                if(SpaceExchange(hsp,hobject,ft) && hsp)
                {
                    char s[64];
                    GetNONAME(s);
                    CreateSpaceWindow(s,hsp);
                    return TRUE;
                }
                else
                {
                    MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG36),_MSG_INFO,MB_OK|MB_ICONINFORMATION);
                    return FALSE;
                }
            };
#endif
        }
        if(type)
        {
            switch(type)
            {
                case FILE_MAT:
                {
                    int m=matrixs->LoadMatrix(0,s);
                    if(m)
                    {
                        if(MatrixDialog(HWindow,m))
                        {
                            matrixs->SaveMatrixAs(m,s);
                        }
                        matrixs->MDelete(m);
                    }
                }break;
                case FILE_URL:
                case FILE_RAR:
                case FILE_PRJ:
                {
                    if(CanCloseProject())
                    {
                        if(LoadProject(s))
                        {
                            PObject object;
                            PClass _class;
#ifdef CHECKVERSIONPROJECT
#ifdef LOGON
                            LogMessage("CHECKVERSIONPROJECT begin");
#endif
                            PClassListInfo pcli=GetClassListInfo("STRAT2957");
                            if(!pcli)
                                pcli=GetClassListInfo("STRAT2958");
#ifdef LOGON
                            if(pcli)
                            {
                                LogMessage("Founded class:");
                                LogMessage(pcli->classname);
                            }
                            else
                                LogMessage("Class not founded");
#endif
                            bool Found=false;
                            if(pcli!=NULL)
                            {
                                _class=pcli->GetClass();
                                //char* text=GetClassText(_class);
                                for(int i=0;i<_class->varcount;i++)
                                    if(lstrcmpi(_class->vars[i].name,"name")==0)
                                        if(_class->vars[i].def)
                                            if(atoi(_class->vars[i].def) == __SC_BUILD__)
                                            {	Found=true; break;  }
                            }
                            if(!Found || pcli==NULL)
                            {
#ifdef LOGON
                                LogMessage("Unreg version. Can't found var name with build version");
#endif
                                MessageBox(0,"Незарегистрированная версия","Выход",0);
                                SystemQuit(1);
                                return 0;
                            }
#ifdef LOGON
                            LogMessage("Found var 'name' with build version");
                            LogMessage("CHECKVERSIONPROJECT end");
#endif
#endif
#ifdef CHECKUSERKEY
                            userkey=new UserKey();
                            //						  if(!userkey->IsUserAutorized())
                            //						  {
                            //						  	MessageBox(0,"Вы не авторизованы!","Проверка",0);
                            //						    SystemQuit(1);
                            //						  }
#endif
                            if(GetRootObject(&_class,&object))
                            {
                                if(!_name)
                                {
                                    project->readonly|=openinreadonly;
                                    project->SetTitle();
                                }
                                AddRecentlyProject(project->realname?project->realname:project->file);
#ifndef RUNTIME
                                if(!noschemeonload)
                                    CreateSchemeWindow(_class->name,object);
#endif
                                if(!_name)
                                    SCSetScVarS("lastdir",s);
                                project->OnLoad();
                                return TRUE;
                            }
                        }
                    }return FALSE;
                }
                case vect_VDR:
                {
#ifndef RUNTIME
                    HSP2D hsp=LdSpace2d(0,s);
                    if(hsp)
                    {
                        TMDIChildWindow*w=CreateSpaceWindow(s,hsp);
                        if(w && (!_name))
                        {
                            TSchemeWindow*_w=(TSchemeWindow*)w->client;
                            if(_w && openinreadonly)
                                _w->SetReadOnly(1);
                        }
                        if(!_name)
                            SCSetScVarS("lastdir",s);
                        AddRecentlyProject(s);
                        return TRUE;
                    }
                    else
                        return FALSE;
#endif
                }break;
                case raster_PNG :
                case file3d_3DS :
                case raster_BMP :
                case raster_TGA :
                case raster_JPG :
                case raster_GIF :
                case raster_PCX :
                case raster_LBM :
                case raster_TIFF:
                case raster_RLE :
                case raster_DOUBLE:
                case raster_ICO :
                case vect_WMF   :
                case video_AVI  :
                {
#ifndef RUNTIME
                    char ss[64];
                    GetNONAME(ss);
                    TMDIChildWindow* child=CreateSpaceWindow(ss,CreateSpace2d(0,scheme_path));
                    if (child)
                    {
                        TSchemeWindow *w=(TSchemeWindow *)child->client;
                        w->__ImportObject(s);
                        if(w->object2d)
                        {
                            SetObjectName2d(w->hsp,w->object2d,s);
                            return TRUE;
                        }
                    }
#endif
                }return FALSE;
                default:
                    _Error(EM_UNCNOWTOOPEN);
            }
        }
        else
            return FALSE;
    }
    return TRUE;
};

#ifndef RUNTIME
TMDIChildWindow*  TMainWindow::CreateSpaceWindow(char* s,HSP2D hsp)
{
    if (hsp){
        HWND hsw=CreateWindow("SC_Scheme_WINDOW",0,WS_CLIPCHILDREN|
                              WS_CHILD,0,0,640,480,HWindow,(HMENU)10,(HINSTANCE)hInstance,NULL);
        if (hsw){
            TSchemeWindow*sw=(TSchemeWindow*)GetWindowLong(hsw,0);
            sw->SetSpace(hsp);
            AssignWindowToSpace2d(hsp,hsw);
            sw->hardmode=EDITOR;
            //TMDIChildWindow * child=
            return CreateMDIChild(s,sw,NULL,NULL,MODE_EDITOR2D);
        }
    }
    return 0;
};
#endif // runtime
void TMainWindow::Cm_VarSave(){
    char s[256]="";
    if(FileDialog(HWindow,s,IDS_STATE,FD_SAVE)){
        SaveState(NULL,s);
    }
};
void TMainWindow::Cm_VarLoad(){
    char s[256]="";
    if(FileDialog(HWindow,s,IDS_STATE,0)){
        //	PObject object;PClass _class;  view main scheme
        LoadState(NULL,s);
    }
};
WNDPROC oldmsglistproc=NULL;
LRESULT scCALLBACK _ListMSGProc( HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam){
    if (message==WM_RBUTTONDOWN)
    {HWND parent=GetParent(hwnd);
#ifdef WIN32
        PostMessage(parent,WM_COMMAND,MAKELONG(MESSAGE_WND,0),(LPARAM)hwnd);
#else
        PostMessage(parent,WM_COMMAND,MESSAGE_WND,MAKELONG(hwnd,0));
#endif
        return 0;
    }
    return oldmsglistproc(hwnd,message,wParam,lParam);
};

void TMessageMsgServer::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode)
{
    switch(command)
    {
        case CM_MSGALLREMOVE:errorhandler->AllRemove();break;
        case CM_FILESAVEAS:
        {
            char filename[256]="";
            if(FileDialog(HWindow,filename,IDS_TEXT,FD_SAVE))
            {
                TDOSStream st(filename,TDOSStream::stCreate);
                for(C_TYPE i=0;i<errorhandler->count;i++)
                {
                    st.WriteDOSSTR((char*)errorhandler->At(i));
                }
            }
        }break;
    }
};

void TMainWindow::Cm_Message(){
    if (errorhandler->hwnd)SendMessage(mdi,WM_MDIACTIVATE,(WPARAM)GetParent(errorhandler->hwnd),0);
    else{
        errorhandler->MakeWindow(
                    CreateWindowEx(
                #ifdef WIN32
                        WS_EX_CLIENTEDGE
                #else
                        0
                #endif
                        ,"LISTBOX",0,LBS_NOINTEGRALHEIGHT|WS_CLIPCHILDREN|
                        WS_CHILD|WS_VSCROLL|WS_HSCROLL,0,0,640,480,HWindow,(HMENU)0,(HINSTANCE)hInstance,NULL));

        oldmsglistproc=(WNDPROC)GetWindowLong(errorhandler->hwnd,GWL_WNDPROC);

        SetWindowLong(errorhandler->hwnd,GWL_WNDPROC,(LPARAM)_ListMSGProc);

        //TMDIChildWindow * child=
        char s[256];
        LoadString(HResource,IDS_str88,s,sizeof(s));
        TMDIChildWindow * child=
                CreateMDIChild(s,0,errorhandler->hwnd,NULL,MODE_MESSAGES);
        SendMessage(errorhandler->hwnd,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
        child->prj=NULL;
        child->helpctx=Message_Window;
        TMessageMsgServer* ser=new TMessageMsgServer(NULL,child->HWindow);
        child->msgserver=ser;

        winmanager->PostCreateMDIWindow("@Messages@",child,0);

    };
};

void TMainWindow::Cm_VarToDef(BOOL setset){
    TProject * p = (TProject * )GetCurrentProject();
    if(p)p->VarToDef(setset);
};

void FillObjectTreeInfo(TreeItemData&td,DWORD parent,PObject po){
    td.data =(DWORD) po;
    td.text=po->_class->name;
    td.parent = parent;
    td.IsLeaf  = !(po->_class->childcount);
    td.IsImage = USER_IMAGE;
    td.datatype=TREE_OBJECT;
};

int __InsertObject(HWND tree,DWORD parent,PObject po){
    TreeItemData td;
    FillObjectTreeInfo(td,parent,po);
    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
    return 1;
};

int extern peridle;
BOOL TMainWindow::Cm_CloseChildren(BOOL ask){
m1:
    for(C_TYPE i=0;i<winmanager->allmdi->count;i++){
        TMDIChildWindow *w=(TMDIChildWindow *)winmanager->allmdi->At(i);
        if (!(w->flags&WF_PROGRAM))
        {
            if (w->client  && ask && (!w->client->CanClose()))return FALSE;
            SendMessage(mdi,WM_MDIDESTROY,(WPARAM)w->HWindow,0);
            goto m1;
        }
    }
    return TRUE;
};
BOOL TMainWindow::ShowSysMenu(int i){
    HMENU m=GetMenu(HWindow);
    HMENU _m=tmenu?TWindow::tmenu->menu:menu;
    switch(i){
        case 0:if(m)SetMenu(HWindow,0);break;
        case 1:if(!m)SetMenu(HWindow,_m);break;
        case 2:if(m)SetMenu(HWindow,0);else SetMenu(HWindow,_m);
    }
    return (GetMenu(HWindow)!=NULL);
};

int (WINAPI*URLLoadFile)(char *remote,char *local,LONG flags)=0;
int (WINAPI*URLDecode)(char *file,char *dir)=0;

BOOL InitInet()
{
    if(!mainwindow->hinet)
    {
        mainwindow->hinet=LoadLibrary("inet32.dll");
        TFCollection*f=dllmanager->LoadPlugin("inet32.dll");
        if(f)
        {
            mainwindow->hinet=f->hdll;
            if(mainwindow->hinet)
            {
                (FARPROC)URLLoadFile=GetProcAddress(mainwindow->hinet,"URLLoadFile");
                (FARPROC)URLDecode=GetProcAddress(mainwindow->hinet,"URLDecode");
            }
        }
    }
    if(mainwindow->hinet && URLLoadFile && URLDecode) return TRUE;
    return FALSE;
};

BOOL TMainWindow::OpenURL()
{
    char s[256];
    SCGetScVarS("LastURL",s);
    if(InputBox(HWindow,"Open URL","URL location",s) && lstrlen(s)){
        SCSetScVarS("LastURL",s);
        // http:// beigin test
        Cm_Load(s);
    };
};

#ifndef RUNTIME
void TMainWindow::Cm_Watch(){
    for(INT16 i=0;i<winmanager->allmdi->count;i++){
        TMDIChildWindow*w=(TMDIChildWindow*)winmanager->allmdi->At(i);
        if(w->MODE==MODE_WATCH){
            SendMessage(mdi,WM_MDIACTIVATE,(WPARAM)w->HWindow,0l);
            return;
        }
    };
    MDICREATESTRUCT md;
    md.szClass="SC_WatchWindow";
    md.szTitle="Watch";
    md.hOwner=hInstance;
    md.style=MDIS_ALLCHILDSTYLES;
    md.y=md.x=CW_USEDEFAULT;
    md.cy=200;md.cx=400;

    HWND active=(HWND)SendMessage(mdi,WM_MDIGETACTIVE,0,0);
    if (active && IsZoomed(active))md.style|=WS_MAXIMIZE;
    md.lParam=NULL;
    HWND win=(HWND)SendMessage(mdi,WM_MDICREATE,0,(LPARAM)&md);
    TMDIChildWindow*w=(TMDIChildWindow*)GetWindowLong(win,0);
    w->helpctx=Watch_window;

};
void TMainWindow::Cm_Libs(){
    TMDIChildWindow *m=winmanager->GetChild(ID_LIBRARYS);
    if (m){
        SendMessage(mdi,WM_MDIACTIVATE,(WPARAM)m->HWindow,0l);
        return;
    };
    HWND tsw=
        #ifdef WIN32
            CreateWindowEx(WS_EX_CLIENTEDGE,
                       #else
            CreateWindow(
            #endif


                "SC_Tree_WINDOW",0,WS_CLIPCHILDREN|
                WS_CHILD,0,0,200,300,HWindow,(HMENU)ID_LIBRARYS,(HINSTANCE)hInstance,0);
    if (tsw){
        VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
        sw->CanUseDelete=1;
        sw->CanDrag=TRUE;
        sw->NotifyWnd=HWindow;
        char s[256];
        LoadString(HResource,IDS_str74,s,sizeof(s));

        TreeItemData td;
        td.text = s;
        td.data = (DWORD)GetParentLibrary();
        td.datatype=TREE_LIBRARY;
        td.parent = NULL;
        td.IsLeaf = 0;
        td.IsImage = 0;
        ::SendMessage(tsw,TVN_INSERTITEM,0,(long)&td);
        ::SendMessage(tsw,TV_EXPAND,1,td.data);

        LoadString(HResource,IDS_str89,s,sizeof(s));
        TMDIChildWindow*child=CreateMDIChild(s,(TWindow*)sw,NULL,NULL,MODE_LIBRARY);
        child->prj=NULL;
        child->helpctx=Library_Window;
        winmanager->PostCreateMDIWindow("@Libraries@",child,0);
        TTreeMsgServer* ser=new TTreeMsgServer(NULL,child->HWindow);
        ser->tree=tsw;
        child->msgserver=ser;

    }
}

void TMainWindow::Cm_Hier(){
    {
        TMDIChildWindow *m=winmanager->GetChild(ID_HIERARCHY);
        if (m){
            SendMessage(mdi,WM_MDIACTIVATE,(WPARAM)m->HWindow,0l);
            return;
        };
        HWND tsw=
        #ifdef WIN32
                CreateWindowEx(WS_EX_CLIENTEDGE,
                       #else
                CreateWindow(
            #endif
                    "SC_Tree_WINDOW",0,WS_CLIPCHILDREN|
                    WS_CHILD,0,0,200,300,HWindow,(HMENU)ID_HIERARCHY,
                    (HINSTANCE)hInstance,0);
        if (tsw){
            VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
            sw->NotifyWnd=HWindow;
            PObject object;PClass _class; // view main scheme
            GetRootObject(&_class,&object);

            __InsertObject(tsw,0,object);
            ::SendMessage(tsw,TV_EXPAND,1,(DWORD)object);

            char s[256];
            LoadString(HResource,IDS_str90,s,sizeof(s));
            TMDIChildWindow*child=CreateMDIChild(s,(TWindow*)sw,NULL,NULL,MODE_HIERARCHY);
            child->helpctx=Project_hierarhy_Window;
            child->flags|=WF_PROJECT;

            child->prj=project=(TProject*)GetCurrentProject();

            winmanager->PostCreateMDIWindow("@Hierarchy@",child,0);
            THMsgServer* ser=new THMsgServer(NULL,child->HWindow);
            ser->object=object;
            ser->tree=tsw;
            child->msgserver=ser;
        }}
}
#endif

void TMainWindow::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode)
{
    if ((command >= CM_CONTROLRANGE)&&
        (command <= CM_ENDCONTROLRANGE))
    {
        SwapControlBar(command-CM_CONTROLRANGE);
        return;
    }
    if (command >=CM_RECENTLY && command <CM_RECENTLY+10)
    {
        char s[256];
        char *_s=s;
        if (GetMenuString (menu_recently,command,s,256,MF_BYCOMMAND))
        {
            if ((s[0]=='&') && (s[2]==' '))_s+=3;
            if(!Cm_Load(_s))RemoveRecently(_s);
        }
        return ;
    }
    switch (command)
    {
        case CM_DBE_EXPLORER:{
            WinExec("dbe\\dbe.exe",SW_SHOWNORMAL);
        }break;
        case CM_OPENURL:
            OpenURL();break;
        case CM_FULLSCREEN:SetFullScreen(!fullscreen);break;
        case CM_NEXTPAGE:winmanager->DoRedo();break;
        case CM_PREVPAGE:
            winmanager->DoUndo();
            break;
            //  case CM_NEXTMDICHILD:SendMessage(mdi,WM_MDINEXT,0,1);break;
        case CM_PLUGINS:PluginsDialog(HWindow);break;
        case CM_PRINTERSETUP:PrinterSetupDialog(HWindow);break;
        case CM_ORDER:LaunchURL("http://www.stratum.ac.ru/scorder.htm");break;
        case CM_WEBSUPPORT:LaunchURL("http://www.stratum.ac.ru/scsupport.htm");break;
        case CM_TUTORIAL:LaunchURL("http://www.stratum.ac.ru/sctutor.htm");break;
        case CM_HOMEPAGE:{
            LaunchURL(MAKEINTRESOURCE(IDS_str108));
        }break;
        case CM_VIEWMAINMENU:{ShowSysMenu(2);}break;
        case CM_CLOSECHILDREN:Cm_ClearWindows();
        case CM_CLOSEEDITCHILDREN:{Cm_CloseChildren(TRUE);

        };break;
        case CM_HIDESTATUSBAR:ShowStatus(0);break;
        case CM_STATUSTOP:    ShowStatus(1);break;
        case CM_STATUSBOTTOM: ShowStatus(2);break;
        case CM_VIEWSTATUSBAR:ShowStatus((SCGetScVar("StatusMode",2)==0)?2:0);break;
            //case CM_VIEWTOOLBAR  :ShowControl(!viewcontrol);break;
        case CM_CONTROLBARS:ControlBarsDialog(HWindow);break;
        case CM_CLOSEVIDEOS:CloseAllVideos();break;
        case CM_DESCKTOP:if(EnvironmentDialod(HWindow))
            {
                _StatusBar->Rebuild();
                InvalidateRect(_StatusBar->HWindow,0,0);
                SetLastSteps();
            }break;
#ifndef RUNTIME
        case CM_SAVEPROJECTAS:{
#ifdef DEMO
            DemoMSG();
#else
            TProject * prj=(TProject *)GetCurrentProject();
            if(prj)
            {
                static char s[260]="";
                char ss[128];
                LoadString(HResource,IDS_str203,ss,sizeof(ss));
                if (ChoseDirDialog(HWindow,s,ss,CHDIR_FORSAVE)){
                    // Если старый и новый каталоги одинаковы то просто записываем изменения
                    if(!lstrcmpi(s,project->path)) { if(!AcceptChanges(TRUE))return;  }
                    if(!IsDirEmpty(s)){
                        if(MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG33),_MSG_WARNING,
                                  MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2)!=IDYES)return;
                    }
                    SetStatusIcon(STATUS_OPERATION,3);
                    int rez=SaveProjectAs(project,s);
                    SetStatusIcon(STATUS_OPERATION,0);
                    if(!rez){
                        MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG34),_MSG_ERROR,MB_OK|MB_ICONSTOP);
                    }}
            }
#endif
        }break;
        case CM_PROJECTOPTIONS:ProjectDialog(HWindow,project);break;
        case CM_SYSTEMINFO:SysInfoDialog(HWindow);break;
#endif
        case CM_SAVEALL:{
#ifdef DEMO
            DemoMSG();
#else
            AcceptChanges(TRUE);
#ifndef RUNTIME
            C_TYPE i=0;
            while (i<winmanager->allmdi->count)
            {
                TMDIChildWindow *w=(TMDIChildWindow *)winmanager->allmdi->At(i);
                if(w->MODE==MODE_EDITOR2D && ((w->flags&WF_PROGRAM)==0))
                {
                    TSchemeWindow*sw=(TSchemeWindow*)w->client;
                    if(sw->hardmode==EDITOR)
                    {
                        if(!sw->Cm_Save(TRUE))break;
                    }
                }
                i++;
            }
#endif
#endif
        }break;
        case CM_SAVEALLEX:
        {
            SaveAllEx();
        }break;

        case CM_VIEWMESSAGES:Cm_Message();break;

        case CM_LIBHELP:
        case CM_SUPPORT:
        case CM_HELPGRAPI:
        case CM_HELPTHEORY:
        case CM_HELPINDEX1:
        case CM_HELPUSING_HELP:
        case CM_HELPKEYBOARD:HELP(command);break;

        case CM_LOADFROMDEFAULT:
        case CM_SAVETODEFAULT:{
            char s[256];
            if(GetProjectPath(s)){
                AddSlash(s);
                lstrcat(s,"default.stt");
                if (command==CM_SAVETODEFAULT) SaveState(NULL,s);else
                    LoadState(NULL,s);
            }
        }break;
        case CM_SETVAR:{
            PObject object;PClass _class; // view main scheme
            if(!GetRootObject(&_class,&object))return;
            TCollection _items(1,0);
            _items.Insert(object);
            SetVarDialog(HWindow,&_items);
        }break;
        case CM_LOADSTATE:Cm_VarLoad();break;
        case CM_STATESAVE:Cm_VarSave();break;
        case CM_VARTODEF:Cm_VarToDef();break;
        case CM_CLOSESTREAM:Cm_ClearStream();break;
#ifdef DBE
        case CM_DOSQL:{
            DoSqlDialog(HWindow);
        }break;
        case CM_CLOSEDATABASES:{
            TProject *_project=(TProject *)GetCurrentProject();
            if(_project)_project->CloseBases();else DBCloseAll(0);
        }
#endif DBE
        case CM_VARCLEAR:
            Cm_ClearVar();
            break;

        case CM_CLEARALL:
            Cm_ClearAll();
#ifdef LOGON
            LogMessage("Cm_ClearAll()");
#endif
            break;

        case CM_CLOSEWINDOWS:
            Cm_ClearWindows();
            break;

        case CM_PAUSE:
            Stop();
#ifdef LOGON
            LogMessage("Cm_Pause()");
#endif
            break;

        case CM_RUN:
#ifdef LOGON
            LogMessage("CM_RUN");
#endif
#ifdef PROTECT
            if(!IsRegistered3())
            {
                int i=rand();
                if(i>100 && i<120){if(!ReminderDialog(HWindow))return;}
            }
#endif
            PressControlButton(CM_RUN);
            peridle=SCGetScVar("PerIdle",1);
            Run();
            break;

        case CM_ONESTEP:
        {
            //TWaitCursor wait(HWindow);
            HCURSOR hold=::SetCursor(LoadCursor(0,IDC_WAIT));

            PressControlButton(CM_ONESTEP, 1);
            UpdateWindow(HWindow);
            DWORD _e = EXEC_FLAGS;
            EXEC_FLAGS |= EF_RUNNING;
            DoOneStep();
            EXEC_FLAGS=_e;
            PressControlButton(CM_PAUSE,1);
            SetLastSteps();
            ::SetCursor(hold);
#ifdef LOGON
            LogMessage("One step()");
#endif
        }break;

        case CM_CASCADECHILDREN:Cm_Cascade();break;
        case CM_TILEHORZ:Cm_TileH();break;
        case CM_TILEVERT:Cm_TileV();break;
        case CM_ARRANGEICONS:Cm_Arrange();break;
#ifdef PROTECT
        case CM_REMIND:ReminderDialog(HWindow);break;
        case CM_REGISTER:if(!Register()){/*sleep(1000);*/}break;
#endif
        case CM_EXIT:Cm_Exit();break;
        case CM_HELPABOUT:AboutDialog(HWindow);break;

#ifndef RUNTIME
        case CM_WATCH:
            Cm_Watch();
            break;

        case CM_MAIN:{
            PObject object;PClass _class;
            if(GetRootObject(&_class,&object)){
                CreateSchemeWindow(_class->name,object);
            }
        }break;
#endif
        case CM_CLOSECHILDPROJECT:
        {
            TProject*prj=(TProject*)GetCurrentProject();
            if(prj)
            {
                if(prj->projecttoactivate)
                {
                    TProject* _prj = GetProject(prj->projecttoactivate);
                    project = NULL;
                    DWORD _EXEC_FLAGS = EXEC_FLAGS;
                    CloseProject(prj);
                    SetActiveProject(_prj);
                    //EXEC_FLAGS = _EXEC_FLAGS;
                    EXEC_FLAGS = _prj->_exec_flags;
                    if(EXEC_FLAGS&EF_RUNNING)
                        Run();
                }
            }
        };break;
        case CM_PROJECTCLOSE:{
            if( CanCloseProject()){
                //		SetControlBarRange(0);
                CloseProject((TProject*)GetCurrentProject());
            }
        }break;
#ifndef RUNTIME
        case CM_NEWSPACE :
        {
            char s[256];char ss[64];
            GetNONAME(ss);

            HSP2D hsp=0;
            if(SCGetScVarS("template_vdr2d",s)){
                hsp=LdSpace2d(0,s);
                SetSpaceFile2d(hsp,NULL);
            }
            if(!hsp)hsp=CreateSpace2d(0,scheme_path);
            CreateSpaceWindow(ss,hsp);
            break;
        }
#endif //runtime
        case CM_NEWMATRIX:break;
        case CM_FILEOPEN:Cm_Load(NULL,TRUE);break;
#ifndef RUNTIME
        case CM_NEWSIMPLYPROJECT:Cm_NewSimplyProject();break;
        case CM_NEWPROJECT:Cm_NewProject();break;
        case CM_LIBRARYS:Cm_Libs();break;
        case CM_HIERARCHY:Cm_Hier();break;
#endif // runtime
        default:
        {

            HWND mdihwnd=(HWND)SendMessage(mdi,WM_MDIGETACTIVE,0,0);
            if (mdihwnd){
                TMDIChildWindow * win=(TMDIChildWindow *)GetWindowLong(mdihwnd,0);
                if (win){
                    if (win->msgserver)win->msgserver->EvCommand(
                                command,hwndCtl,wNotifyCode);
                    if(win->client) win->client->EvCommand(
                                command,hwndCtl,wNotifyCode);else
                        win->EvCommand(command,hwndCtl,wNotifyCode);

                }
                if((command==CM_HELP)&&(win->helpctx)) HELP(win->helpctx);

            }else{
                if(command==CM_HELP)HELP(CM_HELPINDEX1);
            }
        }
            //	else TWindow::EvCommand(command,hwndCtl,wNotifyCode);
    }
};
LRESULT TMainWindow::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    //SetBkColor(GetDC(mdi),RGB(200,0,0));

    switch(message){
#ifdef WIN32
        case WM_CTLCOLORMSGBOX:{
            if(title)ShowWindow(title,SW_HIDE);
        }break;
#endif
        case WM_SETMENU:{
            HMENU m=GetMenu(HWindow);
            BOOL rez= SetMenu(HWindow,(HMENU)wParam);
            if(lParam){*((BOOL*)lParam)=rez;}
            return (LPARAM)m;
        };
        case WM_GETSCINFO:{
            switch(wParam){
                case 1:return SCGetScVar("AllowMultiple",0);
            }
        }return 0;
            //	 case WM_INITMENU:CheckMenuForEnable(this,(HMENU) wParam);break;
            /*
  case WM_PARENTNOTIFY:{
POINT point;
 int fwEvent = wParam;
  point.x = LOWORD(lParam);
  point.y = HIWORD(lParam);
  if(fwEvent==WM_RBUTTONDOWN){
    ClientToScreen(HWindow,&point);
    TrackPopupMenuR(point,MENU_MDI,TRUE);
   }
  }return 0;
*/
        case WM_MENUSELECT:{
#ifdef WIN32
            int item= (UINT) LOWORD(wParam);
            WORD fwMenu = (UINT) HIWORD(wParam); // menu flags
            // hmenu = (HMENU) HIWORD(lParam); /* handle of the menu  */
#else
            int item= wParam;
            WORD fwMenu = LOWORD(lParam);        /* menu flags  */
#endif
            if (fwMenu&(MF_POPUP|MF_SEPARATOR)){SetStatusMode(TRUE);return 0;}
            if (item>=CM_RECENTLY && (item<CM_RECENTLY+10)){

                char s[256];
                LoadString(HResource,IDS_str91,s,sizeof(s));
                SetSimpleText(s);
                return 0;
            };

            int id=IsCommandDisable(item);
            if(id>99){
                SetDisableText(id);
            }else{
                char s[256];
                if(GetCommandInfo(item,s)){
                    SetSimpleText(s);
                }else SetStatusMode(TRUE);
            }
        }break;
        case WM_SYSCOMMAND:{
            switch(wParam){
                case SC_SCREENSAVE:{
                    return Running() && SCGetScVar("DisableScrSaver",1);
                }break;
                case CM_CONTROLBARS:
                case CM_VIEWSTATUSBAR:
                case CM_VIEWMAINMENU:{EvCommand(wParam,HWindow,0); }return 0;
            }
        }break;
        case WM_ACTIVATE:{
#ifdef WIN32
            int fActive = LOWORD(wParam);           // activation flag
            int fMinimized = (BOOL) HIWORD(wParam); // minimized flag
            HWND hwnd = (HWND) lParam;
#else
            int fActive = wParam;
            HWND hwnd = (HWND) LOWORD(lParam);
#endif
            if (fActive==0){
                if(hwnd){
                    char s[64]="";
                    GetClassName(hwnd,s,sizeof(s));
                    if (!lstrcmp(s,ControlBarName)){
                        return 0;
                    }
                }
            }}break;
        case WM_DROPFILES:{
            HDROP handle=(HDROP)wParam;
            int count=DragQueryFile(handle,-1,NULL,0);

#ifdef WIN32
            char s[MAX_PATH];
#else
            char s[256];
#endif

            POINT p;
            POINT pp;
            BOOL getpoint= DragQueryPoint(handle,&p);

            HWND hwnd=NULL;
            if(getpoint){
                ClientToScreen(HWindow,&p);
                hwnd=WindowFromPoint(p);
            }

            POINT _p;
            _p=pp;
            int dx=0,dy=0,_dy=0;

            for(int i=0;i<count;i++){
                DragQueryFile(handle,i,s,sizeof(s));

                TSchemeWindow *scheme=NULL;
                if(getpoint){
                    if(!hwnd)hwnd=WindowFromPoint(p);
                    if(hwnd){
                        pp=p;
                        ScreenToClient(hwnd,&pp);
                        char classname[64]="";
                        GetClassName(hwnd,classname,sizeof(classname));
                        if(!lstrcmpi("SC_Scheme_WINDOW",classname))
                            scheme=(TSchemeWindow*)SendMessage(hwnd,scwnd_message,0,0);
                    }
                }

                if(scheme){
                    UINT type=GetFileType(s,0);
                    if(type){
                        if((type>=raster_START && type<raster_END )||
                           (type>=vector_START && type<vector_END )||
                           (type==video_AVI)||
                           (type==raster_DOUBLE)||
                           (type==raster_ICO)||(type==file3d_3DS)||(type==file_SPACE3D)
                           )
                        {
#ifndef RUNTIME
                            _p.x=pp.x+dx;
                            _p.y=pp.y+dy;
                            TRestoreCrd crd(scheme->hsp);
                            scheme->__ImportObject(s,type,&_p);
                            if (scheme->object2d){
                                POINT2D _s;
                                GetObjectOrgSize2d(scheme->hsp,scheme->object2d,NULL,&_s);
                                dx+=_s.x;
                                if(_s.y>_dy)_dy=_s.y;
                                if(dx>640){
                                    dy+=_dy;
                                    dx=0;_dy=0;
                                }

                                SetObjectName2d(scheme->hsp,scheme->object2d,s);
                            }
#endif // runtime
                        }
                    }
                }else   Cm_Load(s,count==1);

            };

        }break;
        case WM_MEASUREITEM:{
            MEASUREITEMSTRUCT FAR* lm = (MEASUREITEMSTRUCT FAR*) lParam;
            menu_logo=LoadBitmap(HResource,"menu_logo");
            BITMAP bm;
            GetObject(menu_logo,sizeof(bm),&bm);
            menu_logoCX=bm.bmWidth;
            menu_logoCY=bm.bmHeight;
            DWORD cm=GetMenuCheckMarkDimensions();
            lm->itemWidth=bm.bmWidth-LOWORD(cm);
            lm->itemHeight=0;
        }return TRUE;
        case WM_DRAWITEM:{
            DRAWITEMSTRUCT FAR* pdis = (DRAWITEMSTRUCT FAR*) lParam;
            HDC dc=CreateCompatibleDC(pdis->hDC);
            SelectObject(dc,menu_logo);
            RECT rect;
            GetClipBox(pdis->hDC,&rect);
            HGDIOBJ b=CreateSolidBrush(RGB(0,0,128));
            HGDIOBJ old=SelectObject(pdis->hDC,b);
            PatBlt(pdis->hDC,pdis->rcItem.left,pdis->rcItem.top,menu_logoCX,rect.bottom-menu_logoCY,PATCOPY);
            SelectObject(pdis->hDC,old);
            DeleteObject(b);
            BitBlt(pdis->hDC,pdis->rcItem.left,pdis->rcItem.top+(rect.bottom-menu_logoCY),menu_logoCX,menu_logoCY,
                   dc,0,0,SRCCOPY );


            DeleteDC(dc);
        }return TRUE;
        case  WM_ACTIVATEAPP:
            taskactive = (BOOL) wParam;
            if (!taskactive) SetBallonData(0,0,0);
            PaintCaptions();
            break;
        case WM_SYSCOLORCHANGE:{
            DoneBrushes();
            CreateBrushes();
            if(statusbar){
                TStatusBar*s=(TStatusBar*)GetWindowLong(statusbar,0);
                if(s)s->DoneIcons();
            }
            if (IsCtl3d()){Ctl3dColorChange();}
            ControlColorChanged(); //Смена буттонов
        };break;

        case WM_GETMINMAXINFO:{
            MINMAXINFO FAR* lpmmi;
            lpmmi = (MINMAXINFO FAR*) lParam;
            lpmmi->ptMinTrackSize.y = 200;
            lpmmi->ptMinTrackSize.x = 500;
        }return 0;
        case WM_INITMENUPOPUP:{
            HMENU hmenuPopup = (HMENU) wParam;         // handle of pop-up menu
            //nIndex = (int) LOWORD(lParam);       // index of pop-up menu
            BOOL fSystemMenu = (BOOL) HIWORD(lParam); // System-menu flag
            //		  if (!fSystemMenu)
            CheckMenuForEnable(this,hmenuPopup);//else break;
        }return 0;
#ifndef RUNTIME
        case TVN_NEEDPAINT:{
            TREEPAINTSTRUCT *tps=(TREEPAINTSTRUCT *)lParam;
            PClass _class=NULL;
            switch(tps->datatype){
                case TREE_OBJECT:{PObject object=(TObject*)(tps->data);
                    _class=object->_class;
                }break;
                case TREE_CLASS:{
                    PClassListInfo pcli=( PClassListInfo)(tps->data);
                    _class=pcli->GetClass();
                }break;
            }
            if (_class)PaintIcon(_class,tps->hDC,tps->x,tps->y,tps->sizex,tps->sizey);

        };return 0;
        case TVN_ITEMMENU:{
            lasttreeitem=lParam;
            HWND tree=(HWND)wParam;
            __TreeItemMenu(tree,lParam);
        }return 0;
        case TVN_DBLCLK:{
            HWND tree=(HWND)wParam;
            __EditTreeItem(tree,lParam);
        }return 0;
        case TW_UPDATECLASSWINDOWS:{
            HWND tree=(HWND)wParam;
            UPDATESTRUCT* us=(UPDATESTRUCT*)lParam;
            __UpdateClassWindows(tree,*us);
        }return 0;
        case TVN_GETITEMINFO:{
            HWND tree=(HWND)wParam;
            __GetTreeItemInfo(tree,lParam);
        };return 0;
        case TVN_INSERTITEM:{
            HWND tree=(HWND)wParam;
            __EditTreeItem(tree,lParam,1);
            //	 ClassDialog(HWindow,NULL,NULL,NULL);
        }return 0;
#endif // runtime
        case MM_WOM_OPEN:
        case MM_WOM_DONE:
        case MM_WOM_CLOSE:
            aviaudioMessage(HWindow,message, wParam, lParam);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_ERASEBKGND:
            SetBkColor(GetDC(mdi), RGB(222,0,0));
        case WM_PAINT:{
            INT32 statusmode=SCGetScVar("StatusMode",2);
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            SelectObject(hdc,Bface);
            PatBlt(hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right-ps.rcPaint.left,
                   ps.rcPaint.bottom-ps.rcPaint.top,PATCOPY);

            //  DrawShadowFrame(hdc,mdirect.left,mdirect.top,(mdirect.right-mdirect.left),(mdirect.bottom-mdirect.top),1);
            // Рисуем фреймы
            {
                HRGN hrgn = CreateRectRgn(mdirect.left,mdirect.top,mdirect.left,mdirect.bottom);
                //    SelectClipRgn(hdc, hrgn);


                POINT pm;pm.x=pm.y=0;
                ClientToScreen(HWindow,&pm);
                if((viewcontrol && docks[0]->count)||(off97)){
                    DrawShadowLine(hdc,0,0,sizex,0);
                }
                if(viewcontrol &&  (docks[2]->count) && (statusmode==2)){
                    DrawShadowLine(hdc,0,sizey-statusHeight-2,sizex,0);
                }
                if(viewcontrol){
                    if(off97){

                        if(docks[0]->count) DrawShadowLine(hdc,0,mdirect.top-5,sizex,0);
                        if(docks[1]->count){ DrawShadowLine(hdc,mdirect.right+3,mdirect.top-3,0,mdirect.bottom-mdirect.top+5);
                            DrawShadowLine(hdc,sizex-2,mdirect.top-3,0,mdirect.bottom-mdirect.top+5);
                        }
                        if(docks[2]->count) DrawShadowLine(hdc,0,mdirect.bottom+3,sizex,0);
                        if(docks[3]->count){ DrawShadowLine(hdc,mdirect.left-5,mdirect.top-3,0,mdirect.bottom-mdirect.top+5);
                            DrawShadowLine(hdc,0,mdirect.top-3,0,mdirect.bottom-mdirect.top+5);
                        }
                    }

                    for(int i=0;i<4;i++){
                        for(C_TYPE j=0;j<docks[i]->count;j++){
                            TCollection* _c=(TCollection*)docks[i]->At(j);
                            {

                                for(C_TYPE k=0;k<_c->count;k++){
                                    TControlBar* bar=(TControlBar*)_c->At(k);
                                    RECT r;

                                    int sizex=bar->sizex+4;
                                    int sizey=bar->sizey+4;
                                    GetWindowRect(bar->HWindow,&r);
                                    int x=r.left-pm.x-2;
                                    int y=r.top-pm.y-2;
                                    int dx=0,ox=0;
                                    if(off97 && (i&1)){dx=2;ox=2;}
                                    DrawShadowLine(hdc,x+ox,y,sizex-3-dx,0);
                                    DrawShadowLine(hdc,x,y+2,0,sizey-2);
                                    DrawShadowLine(hdc,x,y+sizey-2,sizex-3,0);
                                    DrawShadowLine(hdc,x+ox+sizex-2-dx,y+2,0,sizey-4);
                                }
                            }
                        }
                    }}
                //    SelectClipRgn(hdc,NULL);
                DeleteObject(hrgn);
            }

#ifndef WIN32
            HGDIOBJ oldbrush=SelectObject(hdc,Bdark);

            PatBlt(hdc,mdirect.left,mdirect.top,mdirect.right-mdirect.left,1,PATCOPY);//-----------
            PatBlt(hdc,mdirect.left,mdirect.top+1,0,mdirect.bottom-mdirect.top,PATCOPY);//|

            PatBlt(hdc,mdirect.left+1,mdirect.top+1,(mdirect.right-mdirect.left)-2,1,BLACKNESS);//-----------
            PatBlt(hdc,mdirect.left+1,mdirect.top+1,1,(mdirect.bottom-mdirect.top)-2,BLACKNESS);//|

            SelectObject(hdc,Blight);

            PatBlt(hdc,mdirect.left+2,mdirect.bottom-1,(mdirect.right-mdirect.left)-2,1,PATCOPY);//-----------
            PatBlt(hdc,mdirect.right-1,mdirect.top+2,1,(mdirect.bottom-mdirect.top)-1,PATCOPY);//|

            SelectObject(hdc,oldbrush);
#endif

            if(viewcontrol){
                if(docks[3]->count&& !off97){
                    int x=docks[3]->GetSize();
                    DrawShadowLine(hdc,0,mdirect.top,x,0);
                    DrawShadowLine(hdc,0,mdirect.bottom-2,x,0);
                }
                if(docks[1]->count && !off97){
                    int x=docks[1]->GetSize();
                    DrawShadowLine(hdc,mdirect.right,mdirect.top,x,0);
                    DrawShadowLine(hdc,mdirect.right,mdirect.bottom-2,x,0);
                }
            }

            EndPaint(HWindow,&ps);
        }break;
    }

    return TWindow::WndProc(message,wParam,lParam);
};

void TMainWindow::Cm_Cascade(){
    SendMessage(mdi,WM_MDICASCADE,0,0);
}
void TMainWindow::Cm_TileH(){
    SendMessage(mdi,WM_MDITILE,MDITILE_HORIZONTAL,0);
}
void TMainWindow::Cm_TileV(){
    SendMessage(mdi,WM_MDITILE,MDITILE_VERTICAL,0);
}
void TMainWindow::Cm_Arrange(){
    SendMessage(mdi,WM_MDIICONARRANGE,0,0);
}
//--------------------------------------------------------------------------//

//--------------------------------------------------------------------------//

TMDIChildWindow::~TMDIChildWindow(){
    if(mainwindow && mainwindow->lastactivemdi==this)mainwindow->lastactivemdi=NULL;
    int i=0;
    while(i<winmanager->allmdi->count){
        TMDIChildWindow * wc=(TMDIChildWindow *)winmanager->allmdi->At(i);
        if (wc && wc->mdiparent==this){
            SendMessage(mainwindow->GetMDI(),WM_MDIDESTROY,(WPARAM)wc->HWindow,0);
            continue;
        }
        i++;
    }

    if(msgserver){delete msgserver;msgserver=NULL;};
    winmanager->PostDelete(HWindow);

    if (_client==errorhandler->hwnd)errorhandler->hwnd=NULL;
    winmanager->allmdi->Delete(this);
    SetStatusBeginText();
};
void  SetStatusBeginText(){
    // Устанавливает начальную строку
    char s[256];
    LoadString(HResource,IDS_str213,s,sizeof(s));
    SCSetStatusText(1,s);
}

BOOL TWindow::CanClose(){
    return TRUE;
};

TMDIChildWindow::TMDIChildWindow(LPCREATESTRUCT l,HWND h):TWindow(l,h)
{
    TMDICreate * c=(TMDICreate*)( ((MDICREATESTRUCT*)(l->lpCreateParams))->lParam);
    flags=0;
    msgserver=0;
    prj=0;
    mdiparent=0;
    helpctx=0;
    //menuwnd=NULL;
    if (c)
    {
        MODE=c->_mode;
        client=c->window;
        _client=c->hwnd;
        SetParent(_client,HWindow);
        ShowWindow(_client,SW_SHOW);
    }
    else
    {
        client=0;
        _client=0;
    }
    winmanager->allmdi->Insert(this);
    //SetMenu(h,mainwindow->menu);
};

void TMDIChildWindow::EvCommand(UINT command,HWND/* hwndCtl*/,WPARAM /*wNotifyCode*/)
{
    switch(command)
    {
        case MESSAGE_WND:
        {
            POINT pos;
            GetCursorPos(&pos);
            TSTRATUMMENU sm;
            sm.hWindow=mainwindow->HWindow;
            sm.hType=SCMENU_MESSAGE;
            mainwindow->TrackPopupMenuR(pos,MENU_MESSAGE,&sm);
        }break;
    }
}

LRESULT TMDIChildWindow::WndProc(UINT message,WPARAM wParam,LPARAM lParam)
{
    if (msgserver)msgserver->WndProc(message,wParam,lParam);
    switch(message)
    {
        case WM_PARENTNOTIFY:
        {
            if(wParam==WM_RBUTTONDOWN)
            {
                SendMessage(mainwindow->GetMDI(),WM_MDIACTIVATE,(WPARAM)HWindow,0l);
            }
        }break;
#if !defined(WIN32)
        case WM_QUERYDRAGICON:
        {
            HICON i=ICON_EDITO2D;
            switch(MODE){
                //	  case MODE_NOTHING:
                case MODE_EDITOR2D:i=ICON_EDITO2D;break;
                case MODE_EDITORTEXT:i=ICON_TEXT;break;
                case MODE_MESSAGES:i=ICON_MESSAGE;break;
                case MODE_HIERARCHY:i=ICON_TREE;break;
                case MODE_LIBRARY:i=ICON_LIB;break;
                case MODE_BMPEDIT:i=ICON_BMP;break;
            }
            return (LPARAM)i;
        };
#endif
        case WM_SYSCOMMAND:
            if(MODE==MODE_MODEL && flags&WF_HSP){
                TMSGWindow*msg=(TMSGWindow*)client;
                if(msg && msg->item && msg->item->nochangesize){
                    UINT uCmdType = wParam;
                    if(uCmdType == SC_MAXIMIZE || uCmdType == SC_SIZE )return 0;
                }
            }break;
        case WM_INITMENUPOPUP:{
            HMENU hmenuPopup = (HMENU) wParam;         // handle of submenu
            UINT uPos = (UINT) LOWORD(lParam);        // submenu item position
            BOOL fSystemMenu = (BOOL) HIWORD(lParam); // window menu flag
            if(fSystemMenu && MODE==MODE_MODEL && flags&WF_HSP){
                TMSGWindow*msg=(TMSGWindow*)client;
                if(msg->item && msg->item->nochangesize){
                    EnableMenuItem(hmenuPopup,SC_MAXIMIZE,MF_DISABLED|MF_BYCOMMAND|MF_GRAYED);
                    EnableMenuItem(hmenuPopup,SC_SIZE,MF_DISABLED|MF_BYCOMMAND|MF_GRAYED);
                }
            }
        }break;
        case WM_GETMINMAXINFO:{
            MINMAXINFO FAR* lpmmi;
            lpmmi = (MINMAXINFO FAR*) lParam;
            lpmmi->ptMinTrackSize.y = 50;
            lpmmi->ptMinTrackSize.x = 100;
            if(client)client->WndProc(message,wParam,lParam);
        }break;
        case WM_RBUTTONDOWN:{

        }return 0;
        case WM_SETFOCUS:if(mainwindow->lastactivemdi && mainwindow->lastactivemdi!=this){
                if(mainwindow->lastactivemdi->client){
                    mainwindow->lastactivemdi->client->WndProc(TW_SETFOCUS,0,0);
                }
            };
            mainwindow->lastactivemdi=this;
            if (_client){SetFocus(_client);
                if (client)if(client->WndProc(TW_SETFOCUS,1,0)!=5){
                }
            }return 0;
        case WM_SIZE:EvSize((WORD)wParam,LOWORD(lParam),HIWORD(lParam));
            return DefWindowProc(message,wParam,lParam);
    }
    return TWindow::WndProc(message,wParam,lParam);
};


BOOL TMDIChildWindow::CanClose(){
    for(C_TYPE i=0;i<winmanager->allmdi->count;i++){
        TMDIChildWindow* w=(TMDIChildWindow*)winmanager->allmdi->At(i);
        if(w->mdiparent==this){
            if(!w->CanClose())return FALSE;
        }
    }
    if (client ){

        if (flags&WF_PROGRAM)
        { if(client->_WndProc(WM_CANCLOSE,0,0)!=256)return TRUE;
            else return  FALSE;
        }else{
            return client->CanClose();
        }
    }else{
        if (msgserver)return msgserver->CanClose();

    }
    return TRUE;
};


LRESULT TMDIChildWindow::DefWindowProc(UINT m, WPARAM w,LPARAM l){
    return DefMDIChildProc(HWindow,m,w,l);
};
void TMDIChildWindow::EvSize(WORD sizeType,int,int){

    if(mainwindow->fullscreen && SIZE_MAXIMIZED==sizeType){
        ShowWindow(HWindow,SW_MAXIMIZE);
    }

    if(flags&WF_HSP && client && MODE==MODE_MODEL  && sizeType==SIZE_MAXIMIZED){
        TMSGWindow*msg=(TMSGWindow*)client;
        if(msg->item && msg->item->nochangesize && ((EXEC_FLAGS&EF_STEPACTIVE)==0))
        {
            PostMessage(mainwindow->mdi,WM_MDIRESTORE,(WPARAM)HWindow,0);
            return;
        }
    }
    if (_client && (sizeType!=SIZE_MINIMIZED)){
        RECT r;
        GetClientRect(HWindow,&r);
        int y=0;
        SetWindowPos(_client,0,0,y,r.right,r.bottom,SWP_NOZORDER);
    }
};
//--------------------------------------------------------------------------//

//--------------------------------------------------------------------------//

//DECLARE_FRAMEPROC(TMainWindowProc,TMainWindow);
//DECLARE_MDICHILDPROC(SCMDIChildProc,TMDIChildWindow);

DECLARE_WNDPROC(TMainWindowProc,TMainWindow);
DECLARE_WNDPROC(SCMDIChildProc,TMDIChildWindow);
DECLARE_WNDPROC(StatusBarProc,TStatusBar);
DECLARE_WNDPROC(TreeProc,VTreeWindow);
DECLARE_WNDPROC(ControlBarProc,TControlBar);
DECLARE_WNDPROC(SysTabControlProc,TPropertySheet);
DECLARE_WNDPROC(Browser_proc,TBrowser);
DECLARE_WNDPROC(BLabel_proc,TBrowserLabel);
DECLARE_WNDPROC(MSGProc,TMSGWindow);
//DECLARE_WNDPROC(MIconProc,TIconWindow);
#ifdef DBE
DECLARE_WNDPROC(MCursorProc,TCursorWindow);
#endif
DECLARE_WNDPROC(WClassIconProc,TClassIconWindow);
DECLARE_WNDPROC(TBallonProc,TBallonWindow);
DECLARE_WNDPROC(ColorButtonProc,TColorButton);
DECLARE_WNDPROC(BMP_View_proc,TMBPView);
DECLARE_WNDPROC(ToolPopup_proc,TPopupTool);
DECLARE_WNDPROC(SpaceProc,TSpaceWindow);
//DECLARE_WNDPROC(MenuWndProc,TMenuWindow);

#ifndef RUNTIME
DECLARE_WNDPROC(SchemeProc,TSchemeWindow);
DECLARE_WNDPROC(ChoseRectProc,TViewDIB);
DECLARE_WNDPROC(TWatchWindowProc,TWatchWindow);
#endif

#ifdef CAN_EDIT_BITMAP
DECLARE_WNDPROC(TBmpWindowProc,TBMPChild);
#endif

UINT  CFormat_OBJ=0;
UINT  CFormat_CLASS=0;

int RegisterWindows()
{
    CFormat_OBJ=RegisterClipboardFormat(CLIP_SCOBJ);
    CFormat_CLASS=RegisterClipboardFormat(CLIP_SCCLS);

#ifdef WIN32
    WNDCLASSEX _wndclass;
    _wndclass.cbSize=sizeof(_wndclass);
    _wndclass.hIconSm=0;
    WNDCLASS &wndclass=*(WNDCLASS*)(&(_wndclass.style));
#else
    WNDCLASS wndclass;
#endif
    wndclass.style         =
        #ifdef WIN32
            0;
#else
            CS_VREDRAW|CS_HREDRAW;
#endif
    wndclass.lpfnWndProc   = TMainWindowProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 4;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = ICON_MAIN=LoadIcon(HResource,"ICON");
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = MainClassName;
    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = SCMDIChildProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor( 0,IDC_ARROW );;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = 0;//MAKEINTRESOURCE(MAIN_MENU);
    wndclass.lpszClassName = "SC_CHILD";

#ifdef WIN32
    for(int i=MODE_EDITOR2D-1;i<MODE_IMAGE+1;i++)
    {
        char * lpszName="ICON_SCHEME";
        switch(i){
            case MODE_EDITORTEXT:lpszName="ICON_TEXT";break;
            case MODE_MESSAGES: lpszName="ICON_MESSAGE";break;
            case MODE_HIERARCHY:lpszName="ICON_TREE";break;
            case MODE_LIBRARY:  lpszName="ICON_TREE";break;
                //case MODE_BMPEDIT:  lpszName="ICON_BMP";break;
            case MODE_MODEL:
            case MODE_IMAGE:break;
        }
        _wndclass.hIconSm=(HICON)LoadImage(
                              HResource, lpszName,
                              IMAGE_ICON,
                              16,
                              16, 0   );
        char name[64];
        if(i>=MODE_EDITOR2D){
            wsprintf(name,"SC_CHILD%d",i);
            wndclass.lpszClassName = name;
        }
        if (!RegisterClassEx(&_wndclass))return 0;
    }
    _wndclass.hIconSm=0;
#else
    if (!RegisterClass(&wndclass))return 0;
#endif


    wndclass.style         = 0;
    wndclass.lpfnWndProc   = TBmpWindowProc;
    wndclass.hIcon         = LoadIcon(HResource,"ICON_BMP");
    wndclass.hCursor       = LoadCursor( 0,IDC_ARROW );;
    wndclass.hbrBackground = 0;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_BMPCHILD";
    if (!RegisterClass(&wndclass))return 0;

#ifndef RUNTIME
    wndclass.style         = CS_HREDRAW |CS_VREDRAW ;
    wndclass.lpfnWndProc   = TWatchWindowProc;
    wndclass.hIcon         = LoadIcon(HResource,"ICON_WATCH");
    wndclass.hCursor       = LoadCursor( 0,IDC_ARROW );;
    wndclass.hbrBackground = 0;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_WatchWindow";
    if (!RegisterClass(&wndclass))return 0;
#endif

    wndclass.style         = CS_DBLCLKS|CS_HREDRAW |CS_VREDRAW ;
    wndclass.lpfnWndProc   = StatusBarProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor( 0,IDC_ARROW );;
    wndclass.hbrBackground = 0;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_STATUS_BAR";
    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = SpaceProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = 0;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_Space_WINDOW";

    if (!RegisterClass(&wndclass))return 0;
    wndclass.style         = CS_DBLCLKS|sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = MSGProc;
    wndclass.lpszClassName = "SC_MSG_WINDOW";
    if (!RegisterClass(&wndclass))return 0;

#ifndef RUNTIME
    wndclass.style         = CS_DBLCLKS|sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = SchemeProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = 0;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_Scheme_WINDOW";
    if (!RegisterClass(&wndclass))return 0;
#endif


    wndclass.style         = CS_HREDRAW |CS_VREDRAW |sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = ControlBarProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor( 0,IDC_ARROW );
    wndclass.hbrBackground = 0;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = ControlBarName;

    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = CS_DBLCLKS|sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = TreeProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_Tree_WINDOW";
    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = SysTabControlProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = PropertyButtons;

    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = CS_HREDRAW|CS_VREDRAW|sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = ColorButtonProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_ColorButton";
    if (!RegisterClass(&wndclass))return 0;

#ifndef RUNTIME
    wndclass.style         = sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = ChoseRectProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "ChoseRect_wnd";
    if (!RegisterClass(&wndclass))return 0;
#endif

    wndclass.style         = sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = BMP_View_proc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_BMP_view_wnd";
    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = sc_GLOBALCLASS|CS_DBLCLKS;
    wndclass.lpfnWndProc   = Browser_proc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = 0;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_Browser";
    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = BLabel_proc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_BLabel";
    if (!RegisterClass(&wndclass))return 0;
    /*
  wndclass.style         = CS_DBLCLKS|sc_GLOBALCLASS;
  wndclass.lpfnWndProc   = MIconProc;
  wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
  wndclass.hCursor       = LoadCursor(0,IDC_ARROW);;
  wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName  = 0;
  wndclass.lpszClassName = "SC_Icon_View";
  if (!RegisterClass(&wndclass))return 0;
*/
    wndclass.style         = sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = WClassIconProc;
    wndclass.hIcon         = 0;
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_ClassIcon";
    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW;
    wndclass.lpfnWndProc   = TBallonProc;
    wndclass.hIcon         = 0;
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = 0;
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_BALLON_WINDOW";
    if (!RegisterClass(&wndclass))return 0;

    wndclass.style         = CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW;
    wndclass.lpfnWndProc   = ToolPopup_proc;
    wndclass.hIcon         = 0;
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = "SC_TOOL_POPUP";
    if (!RegisterClass(&wndclass))return 0;
    /*
  wndclass.style         = 0;
  wndclass.lpfnWndProc   = MenuWndProc;
  wndclass.hIcon         = 0;
  wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
  wndclass.hbrBackground = 0;
  wndclass.lpszMenuName  = 0;
  wndclass.lpszClassName = "SC_MENU_WINDOW";
  if (!RegisterClass(&wndclass))return 0;
*/
#ifdef DBE

    wndclass.style         = CS_DBLCLKS|sc_GLOBALCLASS;
    wndclass.lpfnWndProc   = MCursorProc;
    wndclass.hIcon         = 0;//LoadIcon(hInstance,MAKEINTRESOURCE(ICON_1));
    wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
    wndclass.hbrBackground =(HBRUSH)GetStockObject(GRAY_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = "CURSOR_WINDOW";
    if (!RegisterClass(&wndclass))return 0;


#endif

    return 1;
};

void __SaveTypeProc(HWND hwnd){
    HWND combo=GetDlgItem(hwnd,1136);
    if(combo){
        char s[256];
        int i=SendMessage(combo,CB_GETCURSEL,0,0);
        //GetWindowText(combo,s,sizeof(s));
        SendMessage(combo,CB_GETLBTEXT,i,(LPARAM)s);
        int l=lstrlen(s);
        if(s[l-1]==')'){
            lstrcpyn(filesavetype,s+(l-4),4);
        }
    }else filesavetype[0]=0;
};
const STT_TYPE = 200;
void __SaveSttProc(HWND hwnd){
    HWND combo=GetDlgItem(hwnd,STT_TYPE);
    if(combo){
        int selmode =  SendDlgItemMessage(hwnd,STT_TYPE,CB_GETCURSEL,0,0);
        BYTE m=1;
        switch(selmode){
            case 1:m=2;break;
            case 2:m=3;break;
        }
        SetNextSaveStateMode(m);
    }
}

UINT  scCALLBACK FileHookRead(HWND hwnd, UINT msg,WPARAM wParam, LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            char s[256];
            LoadString(HResource,IDS_str220,s,sizeof(s));
#ifdef WIN32
            HWND hWindow=GetParent(hwnd);
            HWND hl=GetDlgItem(hWindow,1040);
            RECT rok,r;
            GetWindowRect(GetDlgItem(hWindow,IDOK),&rok);
            GetWindowRect(hl,&r);
            SetWindowPos(hl,0,0,0,(rok.left-r.left)-1,r.bottom-r.top,SWP_NOMOVE|SWP_NOZORDER);
            SetDlgItemText(hWindow,1040,s);
#else
            SetDlgItemText(hwnd,1040,s);
#endif
        }return TRUE;
    }
    return FALSE;
}
UINT  scCALLBACK FileHook(HWND hwnd, UINT msg,WPARAM wParam, LPARAM lParam){

    switch(msg){
        case WM_INITDIALOG:{
            HWND combo=GetDlgItem(hwnd,STT_TYPE);
            if(combo){
                int i=IDS_STATEmode;
                char s[256];
                while(LoadString(HResource,i,s,sizeof(s))){
                    SendMessage(combo,CB_ADDSTRING,0,(LONG)s);
                    i++;
                }
                SendMessage(combo,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
                SendMessage(combo,CB_SETCURSEL,0,0);
            }	return TRUE;
        }
#ifdef WIN32
        case WM_NOTIFY:{
            NMHDR * pnmh = (LPNMHDR) lParam;
            if(pnmh->code==CDN_FILEOK){
                __SaveTypeProc(GetParent(hwnd));
                __SaveSttProc(hwnd);
            }
        }break;
#else
        case WM_COMMAND:{
            EV_COMMANDsimply
                    if(command==IDOK){
                __SaveTypeProc(hwnd);
                __SaveSttProc(hwnd);
            }
        }break;
#endif
    }
    return FALSE;
};
BYTE openinreadonly=0;

extern "C" BOOL _SDKPROC SCFileDialog(HWND hwnd,char * s,char * szFilter,BYTE _flags){
    return _FileDialog(hwnd,s,szFilter,_flags);
}
int _FileDialog(HWND hwnd,char * s,char * szFilter,BYTE _flags){

    BOOL save=_flags&FD_SAVE;
    BOOL chkreadonly=_flags&FD_READONLY;
    int cbString=lstrlen(szFilter);
    if(cbString==0)return 0;
    OPENFILENAME ofn;
    char szDirName[256];
    char szFile[256], szFileTitle[256];
    UINT  i;
    char  de[15]; //default extension
    char  chReplace;    /* string separator for szFilter */

    /* Get the current directory name, and store in szDirName */
    //getcurdir(0,szDirName);

    if(lstrlen(s)){
        GetPath(s,szDirName);
        int d=lstrlen(szDirName);
        int d1=lstrlen(s);
        if(d<d1){
            lstrcpy(szFile,s+d+1);
        }else lstrcpy(szFile,"");

    }else
    {
        lstrcpy(szDirName, "X:\\");
        szDirName[0] =(char)('A' + getdisk());
        getcurdir(0, szDirName+3);
        lstrcpy(szFile,"");
    }
    char *ps=szFilter,*ps1=de;
    i=0;
    chReplace = szFilter[cbString - 1]; /* retrieve wildcard */
    while (*ps!=0 && *ps!=chReplace && i<10){*ps1=*ps;ps++;ps1++;i++;}
    if (*ps!=0){ps++;}*ps1=0;

    for (i = (int)(ps-szFilter); szFilter[i] != '\0'; i++) {
        if (szFilter[i] == chReplace)
            szFilter[i] = '\0';
    }


    /* Set all structure members to zero. */
    memset(&ofn, 0, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrDefExt=de;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = ps;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szDirName;
    ofn.lpfnHook = FileHook;

    if (save)ofn.Flags = OFN_SHOWHELP|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;else{
        ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if(!(_flags&(FD_READONLY|FD_ASLINK)))ofn.Flags|=OFN_HIDEREADONLY;else{
            if(openinreadonly)ofn.Flags|=OFN_READONLY;
            if(_flags&FD_ASLINK && openaslink)ofn.Flags|=OFN_READONLY;
        }
    }

#ifdef WIN32
    ofn.Flags|=OFN_EXPLORER;
#endif
    if(save)ofn.Flags |= OFN_ENABLEHOOK;
    if(save && !lstrcmpi(de,"STT")){ ofn.Flags |= OFN_ENABLETEMPLATE;
        ofn.hInstance=HResource;
        ofn.lpTemplateName="FILEOPEN";
    }
    if(_flags&FD_ASLINK){
        ofn.Flags |= OFN_ENABLEHOOK;
        ofn.lpfnHook = FileHookRead;
    }


    BOOL _try=TRUE;
    /* Perform file operations. */
reopen:
    int rez=save?GetSaveFileName(&ofn):GetOpenFileName(&ofn);
    // OFN_READONLY
    if (rez) {
        lstrcpy(s,ofn.lpstrFile);
        if(_flags&FD_ASLINK)openaslink=((ofn.Flags&OFN_READONLY)!=0);

        if(!save && (_flags&FD_READONLY)){
            openinreadonly = ((ofn.Flags&OFN_READONLY)!=0);
        }
        return 1;
    }else{

        DWORD error=CommDlgExtendedError();
        //int j=error;
        if(error){_Error(EM_FILEDLG,error);}
        lstrcpy(s,"");
        if(_try && (error==FNERR_INVALIDFILENAME)){
            lstrcpy(ofn.lpstrFile,"");
            _try=FALSE;
            goto reopen;
        }
    }
    return 0;
};

int FileDialog(HWND hwnd,char * s,int str,int flags){
    char  szFilter[256];

    if (LoadString(HResource,str,
                   szFilter, sizeof(szFilter)) == 0) {
        return 0;
    };
    char  *ps=NULL;
    char *_p=strstr(szFilter,"@");
    if(_p){
        ps=new char[1024];
        lstrcpyn(ps,szFilter,(_p-szFilter)+1);
        char *__p=ps+lstrlen(ps);
        switch(str){
            case IDS_PROJECT:LoadString(HResource,IDS_ALLSUPPORTEDprj,__p,256);break;
            case IDS_impOBJECTS:LoadString(HResource,IDS_ALLSUPPORTED,__p,256);break;

        }
        lstrcat(ps,_p+1);
    }
    //BYTE flags=0;
    //if(save)flags|=FD_SAVE;
    if(str==IDS_PROJECT)flags|=FD_READONLY;
    int rez=_FileDialog(hwnd,s,ps?ps:szFilter,flags);
    if(ps)delete ps;
    return rez;
}
HSP2D LoadSpaceFromResource(HWND HWindow,char * text)
{
    HRSRC resource = FindResource(HResource,text,"space");
    HGLOBAL res = LoadResource(HResource,resource);
#ifdef WIN32
    if(res)
#else
    if((int)res>32)
#endif
    {
#ifdef WIN32
        void *adr1 = LockResource(res);
        long size = SizeofResource(HResource,resource);
        HGLOBAL dib = GlobalAlloc(GMEM_MOVEABLE,size);
        void * adr2 = GlobalLock(dib);
        memcpy(adr2, adr1, size);
        UnlockResource(res);
        FreeResource(res);
        GlobalUnlock(dib);
        res=dib;
#endif
        HSP2D hsp = LdFromMemory2d(HWindow,res,NULL);
#ifdef WIN32
        GlobalFree(res);
#else
        FreeResource(res);
#endif
        return hsp;
    }
    return 0;
}
TSpaceWindow::TSpaceWindow(LPCREATESTRUCT l,HWND h):TWindow(l,h)
{
    hsp=0;
    scaling=0;
    if(l->style&1)
    {
        char text[64];
        GetWindowText(HWindow,text,64);
        hsp = LoadSpaceFromResource(HWindow, text);
    }
}

void TSpaceWindow::ScrollTo(long*x,long*y)
{
    POINT2D org;
    if (!(x && y))GetOrgSpace2d(hsp,&org);
    if (x)org.x=(long)*x;
    if (y)org.y=(long)*y;
    SetOrgSpace2d(hsp,&org);
};

TSpaceWindow::~TSpaceWindow(){
    DeleteSpace2d(hsp);
}
void TSpaceWindow::GetSpaceInfo(char *s){
    *s=0;
}

LRESULT TSpaceWindow::WndProc(UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message){
        case TW_GETSPACEINFO:
            GetSpaceInfo((char*)lParam);
            return 0;

        case WM_QUERYNEWPALETTE:EvQueryNewPalette();return 0;
        case WM_PALETTECHANGED:EvPaletteChanged((HWND)wParam);return 0;
        case WM_ERASEBKGND:return 0;
        case WM_PAINT:
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            PaintSpace2d(hsp,hdc,&(ps.rcPaint),0);
            EndPaint(HWindow,&ps);
            return 0;
    }

    if(DefSpaceWindowProc(hsp,HWindow,message,wParam,lParam))
        return TWindow::WndProc(message,wParam,lParam);
    return 0;
};

void CheckMenuForEnable(TWindow*window,HMENU m){
    int cItems = GetMenuItemCount(m);
    for (int pos = 0; pos < cItems; pos++) {
        int id = GetMenuItemID(m,pos);
        if (id==-1){
            HMENU submenu=GetSubMenu(m,pos);
            if (submenu)CheckMenuForEnable(window,submenu);
        }else {

            if (id){
                if (id >=CM_RECENTLY && id <CM_RECENTLY+10)return;
                int res=(id<10000 && id>99)?window->IsCommandDisable(id):0;
                switch (id){

                    case CM_VIEWSTATUSBAR:
                    case CM_VIEWTOOLBAR  :
                    case CM_HOOKCONTROLS:
                    case CM_LAYERGRID:
                    case CM_LAYERCLASSES:
                    case CM_LAYERLINKS:
                    case CM_LAYEROTHER:if (res==1)CheckMenuItem(m,pos,MF_BYPOSITION|MF_CHECKED);
                        if (res==0)CheckMenuItem(m,pos,MF_BYPOSITION|MF_UNCHECKED); //без break
                    default:if((id>99) && (id<10000))EnableMenuItem(m,pos,MF_BYPOSITION|(res>1?MF_GRAYED:MF_ENABLED));
                }

                if (id==CM_CLOSECHILDREN)break;// To enable windows child list
            }
        }
    }
};
void FullDeleteMenu(HMENU m){
    int c = GetMenuItemCount(m);
    for (int i= 0;i < c;i++) {
        HMENU pm = GetSubMenu(m,i);
        if (pm){
            RemoveMenu(m,i,MF_BYPOSITION);i--;c--;
            FullDeleteMenu(pm);
        }}
    DestroyMenu(m);
};

// Обработка ошибок
static int _lastError;

int  GroupFuncError(int code,int group,char *add){
    char s[256];
    char ss[256];
    char command[64];

    if(!LoadString(HResource,code,s, sizeof(s)))
        LoadString(HResource,EM_UNCNOWERROR,s, sizeof(s));
    if(LoadString(HResource,group,command,sizeof(command))){
        wsprintf(ss,command,s);
        if(add){lstrcat(ss," ");lstrcat(ss,add);lstrcat(ss," ");}
        AddObjectName(ss);
        _Message(ss);
        return TRUE;
    }
    return FALSE;
};

int _Error(int code, int info, void* data)
{
    BOOL message=1;
    char _msgf[256];
    char * msgf=_msgf;

    if(LoadString(HResource, code, _msgf, sizeof(_msgf)))
    {
        if (*msgf == '#') {msgf++; message=0;}

        char msg[256];
        switch(msgf[0])
        {
            case '1':
                wsprintf(msg, msgf+1, info);
                break;

            case '2':
                wsprintf(msg, msgf+1, info, data);
                break;

            case '3':
                wsprintf(msg,msgf+1,data);break;
            case '5':wsprintf(msg,msgf+1,data,info);break;
            default:lstrcpy(msg,msgf);
        }
        char ss[128]="";
        char *ps="";
        DWORD dwerror=GetLastError();
        switch(code)
        {
            case EC_FILEOPENERROR:
            {
                switch(dwerror)
                {
                    case 1L  :ps="ERROR_INVALID_FUNCTION";break;
                    case 2L  :ps="EERROR_FILE_NOT_FOUND";break;
                    case 3L  :ps="EERROR_PATH_NOT_FOUND";break;
                    case 4L  :ps="EERROR_TOO_MANY_OPEN_FILES";break;
                    case 5L  :ps="EERROR_ACCESS_DENIED";break;
                    case 6L  :ps="EERROR_INVALID_HANDLE";break;
                    case 7L  :ps="EERROR_ARENA_TRASHED";break;
                    case 8L  :ps="EERROR_NOT_ENOUGH_MEMORY";break;
                    case 9L  :ps="EERROR_INVALID_BLOCK";break;
                    case 10L :ps="EERROR_BAD_ENVIRONMENT";break;
                    case 11L :ps="EERROR_BAD_FORMAT";break;
                    case 12L :ps="EERROR_INVALID_ACCESS";break;
                    case 13L :ps="EERROR_INVALID_DATA";break;
                    case 14L :ps="EERROR_OUTOFMEMORY";break;
                    case 15L :ps="EERROR_INVALID_DRIVE";break;
                    case 16L :ps="ERROR_CURRENT_DIRECTORY";break;
                    case 17L :ps="ERROR_NOT_SAME_DEVICE";break;
                    case 18L :ps="ERROR_NO_MORE_FILES";break;
                    case 19L :ps="ERROR_WRITE_PROTECT";break;
                    case 20L :ps="ERROR_BAD_UNIT";break;
                    case 21L :ps="ERROR_NOT_READY";break;
                    case 22L :ps="ERROR_BAD_COMMAND";break;
                    case 23L :ps="ERROR_CRC";break;
                    case 24L :ps="ERROR_BAD_LENGTH";break;
                    case 25L :ps="ERROR_SEEK";break;
                    case 26L :ps="ERROR_NOT_DOS_DISK";break;
                    case 27L :ps="ERROR_SECTOR_NOT_FOUND";break;
                    case 28L :ps="ERROR_OUT_OF_PAPER";break;
                    case 29L :ps="ERROR_WRITE_FAULT";break;
                    case 30L :ps="ERROR_READ_FAULT";break;
                    case 31L :ps="ERROR_GEN_FAILURE";break;
                    case 32L :ps="ERROR_SHARING_VIOLATION";break;
                    case 33L :ps="ERROR_LOCK_VIOLATION";break;
                    case 34L :ps="ERROR_WRONG_DISK";break;
                    case 36L :ps="ERROR_SHARING_BUFFER_EXCEEDED";break;
                    case 38L :ps="ERROR_HANDLE_EOF";break;
                    case 39L :ps="ERROR_HANDLE_DISK_FULL";break;
                    case 50L :ps="ERROR_NOT_SUPPORTED";break;
                    case 51L :ps="ERROR_REM_NOT_LIST";break;
                    case 52L :ps="ERROR_DUP_NAME";break;
                    case 53L :ps="ERROR_BAD_NETPATH";break;
                    case 54L :ps="ERROR_NETWORK_BUSY";break;
                    case 55L :ps="ERROR_DEV_NOT_EXIST";break;
                }
                wsprintf(ss,"GetLastError return :%lu\n%s",dwerror,ps);
            }
                if(ss[0]){lstrcat(msg,"\n");lstrcat(msg,ss);}
        }
        wvsprintf(msgf,"Error ! code: %d",&code);
        HWND wnd=0;
        if(mainwindow)
            wnd=mainwindow->HWindow;
        if(wnd)
        {
            HWND popup=GetLastActivePopup(wnd);
            if(popup)
                wnd=popup;
        }
        AddObjectName(msg);
        char s1[287];
        lstrcpy(s1,"Error :");
        lstrcat(s1,msg);
        _Message(s1);
#ifdef LOGON
        {
            char ss[1024];
            lstrcpy(ss,"# ERROR # > ");
            lstrcat(ss,s1);
            LogMessage(ss);
        }
#endif
        //if (message)MessageBox(wnd,msg,msgf,MB_OK|MB_ICONSTOP|MB_TASKMODAL);
        //проверяем есть ли текущая ошибка в списке игнорируемых ошибок
        if(message && ingnorederr)
        {
            for(C_TYPE i=0;i<ingnorederr->count;i++)
            {
                if((int)(ingnorederr->At(i))==code)
                {
                    message=0;
                    break;
                }
            }
        }
        if(message)
        {
            if(ErrorDialog(wnd,msg,msgf))
            {
                if(ingnorederr==NULL)
                    ingnorederr=new TCollection(10,10);
                ingnorederr->Insert((void*)code);
            }
        }
        //&Don't show me this message again.
    }
    _lastError=code;
    return 0;
};

void _Message(int id, int info,void * data)
{
    char s[256];
    char msg[256];
    if(!LoadString(HResource,id,s, sizeof(s)))
        lstrcpy(msg,"Resource string not found");
    else
    {
        char* _s = s;
        if (s[0] == '#')
            _s++;

        switch(s[0])
        {
            case '1':
                wsprintf(msg,_s+1,info);
                break;
            case '2':
                wsprintf(msg,_s+1,info,data);
                break;
            case '3':
                wsprintf(msg,_s+1,data);
                break;
            case '5':
                wsprintf(msg,_s+1,data,info);
                break;

            default:
                lstrcpy(msg,s);
        }
    }
    _Message(msg);
};

void _Message(char*s)
{
    if(HIWORD(s) && !LOWORD(s))
        _Message((int)LOWORD(s));
    else
    {
        errorhandler->InsertMSG(s);
#ifdef LOGON
        char ss[300];
        lstrcpy(ss,"# MESSAGE # > ");
        lstrcat(ss,s);
        LogMessage(ss);
#endif
    }
};

void  SetLastScError(INT16 e)
{
    _lastError=e;
};

INT16 GetLastScError()
{
    INT16 error=(INT16)_lastError;
    _lastError=0;
    return error;
};

//int AddLibrary(HWND tree)
int __InsertLib(HWND tree,DWORD parent,TLibrary*lib,BOOL insert=TRUE){
    char s[256];
    TreeItemData td;
    td.IsLeaf = 0;
    td.text=lib->path;
    if(lib->flags&CLF_PACKFILE&&lib->parent&&((lib->parent->flags&CLF_PACKFILE)==0))
    {
        wsprintf(s,"%s (PACK file)",lib->path);
        td.text=s;
    }

    td.IsImage = 0;
    td.datatype=TREE_LIBRARY;
    td.data =(DWORD)lib;
    td.parent=parent;
    if(insert)
        ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
    else {
        ::SendMessage(tree,TVN_UPDATEITEM,0,(long)&td);

    }
    return 1;
};

int __InsertClass(HWND tree, DWORD parent,TClassListInfo* pcli){
    char text[256];
    TreeItemData td;
    td.parent=parent;
    td.IsLeaf = 1;
    td.datatype=TREE_CLASS;
    td.data =(DWORD)pcli;
    lstrcpy(text,pcli->GetClassName());
    char *fn=pcli->filename;
    if (fn){
        lstrcat(text,"  file name:");
        lstrcat(text,fn);
    }else{
        if(pcli->flags&CLF_PACKFILE)
            lstrcat(text," (class in pack file)");
        else
            lstrcat(text," class no file");
    }
    td.IsImage = USER_IMAGE;//CLASS_IMG;
    td.text=text;
    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
    return 1;
};

int __InsertObject2d(HWND tree,DWORD parent,HSP2D hsp,HOBJ2D obj){
    char text[256];
    INT16 type=GetObjectType2d(hsp,obj);
    // char *ps=GetObjectName2d(hsp,obj,type);

    char pps[256];
    GetObjectTypeName2d(type,pps,sizeof(pps));

    char obj_name[200]="";
    GetObjectName2d(hsp,obj,obj_name,sizeof(obj_name));
    wsprintf(text,"%s #%d %s",pps,(int)obj,obj_name);
    TreeItemData td;
    td.parent=parent;
    td.IsLeaf = 1;
    td.datatype=TREE_OBJECT2D;
    td.IsImage = 0;
    if ((type==otGROUP2D)||(type==otRGROUP2D)||(type==otGROUP3D))td.IsLeaf = 0;

    switch (type){
        case otBITMAP2D:
        case otDOUBLEBITMAP2D:td.IsImage = BMP_IMG ;break;
        case otTEXT2D:td.IsImage = TEXT_IMG;break;
        case otLINE2D:td.IsImage = POLYLINE_IMG;break;
        case otGROUP2D:
        case otRGROUP2D:td.IsLeaf = 0;break;
        case otGROUP3D:td.IsLeaf = 0;
        case otOBJECT3D:td.datatype=TREE_OBJECT3D;break;
        case otLIGHT3D:td.IsImage = LAMPON_IMG;td.datatype=TREE_OBJECT3D;break;
        case otCAMERA3D:td.IsImage = CAMERA_IMG;td.datatype=TREE_OBJECT3D;break;
    };

    UINT16 state=GetAttribute2d(hsp,obj);
    if(state&1){
        lstrcat(text," (hidden)");
    }
    td.data = MAKELONG(hsp,obj);
    td.text=text;
    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
    return 1;
};

int __InsertSpace3d2d(HWND tree,DWORD parent,HSP3D hsp3d){
    char text[256];
    wsprintf(text,"Space3d #%d",hsp3d);
    TreeItemData td;
    td.parent=parent;
    td.IsLeaf = (GetNextObject3d(hsp3d,0)==0);
    td.datatype=TREE_SPACE3D;
    td.data = MAKELONG(hsp3d,0);;
    td.IsImage = WORLD_IMG;
    td.text=text;
    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
    return 1;
};
int __InsertMatrix(HWND tree, DWORD parent,TMatrix*m){
    if(m){
        TreeItemData td;
        td.parent=parent;
        td.IsLeaf = 1;
        td.datatype=TREE_MATRIX;
        td.data =(DWORD)m;
        td.IsImage = MATRIX_IMG;
        char text[256];
        wsprintf(text,"#%d from (%ld,%ld) to (%ld,%ld) size:[%ld,%ld]",
                 m->handle,m->minX,m->minY,m->minX+m->dimX-1,m->minY+m->dimY-1,m->dimX,m->dimY);
        td.text=text;
        ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
        return 1;
    }
    return 0;
};

#ifndef RUNTIME
int __UpdateClassWindows(HWND tree,UPDATESTRUCT & us)
{
    switch(us.what)
    {
        case UW_LIBRARY:
        {
            if (us.mode==UW_CREATE)
            {
                __InsertLib(tree,(DWORD)us.parent,(TLibrary*)us.data);
            }
            if (us.mode==UW_MODIFY)
            {
                __InsertLib(tree,(DWORD)us.parent,(TLibrary*)us.data,FALSE);
            }
        }break;
        case UW_OBJECT:
        {
            if (us.mode==UW_CREATE)
            {
                __InsertObject(tree,(DWORD)us.parent,(TObject*)us.data);
            }
            if(us.mode==UW_MODIFY)
            {
                TreeItemData td;
                FillObjectTreeInfo(td,(DWORD)us.parent,(TObject*)us.data);
                ::SendMessage(tree,TVN_UPDATEITEM,0,(long)&td);
            }
        }break;
        case UW_CLASS:{
            if (us.mode==UW_CREATE)
                __InsertClass(tree,(DWORD)us.parent,(TClassListInfo*)us.data);

            if (us.mode==UW_MODIFY){
                PClassListInfo pcli=(TClassListInfo*)us.data;
                char text[256];
                TreeItemData td;
                td.parent=(DWORD)us.parent;
                td.IsLeaf = 1;
                td.datatype=TREE_CLASS;
                td.data =(DWORD)pcli;
                td.IsImage = WORLD_IMG;
                //--------
                lstrcpy(text,pcli->GetClassName());
                char *fn=pcli->filename;
                if (fn){
                    lstrcat(text,"  file name:");
                    lstrcat(text,fn);
                }else{
                    lstrcat(text," class no file");
                }
                //-------
                td.text=text;
                ::SendMessage(tree,TVN_UPDATEITEM,0,(long)&td);
            }
        }break;
    }
    return 1;
};
#endif
int GetEQTreeInfo(HWND tree,DWORD data,int DataType);

void GetBmpString(LPBITMAPINFOHEADER info,char*ps){
    wsprintf(ps,"%d x %d ",(int)info->biWidth,(int)info->biHeight);
    UINT mode=IDS_BMPCOLORS;
    switch(info->biBitCount){
        //	 case 1: lstrcat(ps," монохромная")break;
        case 4:mode=IDS_BMPCOLORS+1;break;
        case 8:mode=IDS_BMPCOLORS+2;break;
        case 24:mode=IDS_BMPCOLORS+3;break;
    }
    LoadString(HResource,mode,ps+lstrlen(ps),64);
};
#ifndef RUNTIME
int __GetTreeItemInfo(HWND tree,DWORD data,int){
    TreeItemData td;
    td.data=data;
    if(!::SendMessage(tree,TVN_GETITEMINFO,0,(long)&td))return 0;
    switch(td.datatype){
        case TREE_DYNOITEM:{
            td.datatype=TREE_DYNOSUBITEM;
            td.parent=data;
            td.IsImage = 0;
            td.IsLeaf = 1;
            char s[256];
            TDynItem*item=(TDynItem*)data;
            int delta=0;
            for(C_TYPE i=0;i<item->_class->varcount;i++){
                td.data = (DWORD)item;
                lstrcpy(s,item->_class->vars[i].classType->GetClassName());
                lstrcat(s," ");
                lstrcat(s,item->_class->vars[i].name);
                lstrcat(s," ");

                _GetVarA(&(item->_data[delta]),item->_class->vars[i].classType,s+lstrlen(s));
                td.text = s;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                delta+=item->_class->vars[i].classType->GetClass()->varsize;
            }
        }break;
        case TREE_DYNOVAR:{
            td.datatype=TREE_DYNOITEM;
            td.parent=data;

            td.IsImage = 0;
            TDynVar *v=(TDynVar*)data;

            for(C_TYPE i=0;i<v->count;i++){

                TDynItem*item=(TDynItem*)v->At(i);
                td.data = (DWORD)item;

                char s[256];
                lstrcpy(s,item->_class->name);
                lstrcat(s," ");
                _GetVarA(item->_data,item->_class->GetClassListInfo(),s+lstrlen(s));
                td.text = s;
                td.IsLeaf = item->_class->varcount==0;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }
        }break;
        case TREE_DYNOMEMORY:{
            char s[64];
            td.datatype=TREE_DYNOVAR;
            td.parent=data;
            td.IsImage = 0;
            for(C_TYPE i=0;i<dyno_mem->count;i++){
                TDynVar*v=(TDynVar*)dyno_mem->At(i);
                if(v){
                    dyno_t h=dyno_mem->H_by_index(i);
                    wsprintf(s,"#%ld",h);
                    td.data = (DWORD)v;
                    td.text = s;
                    td.IsLeaf = (v->count==0);
                    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                }}
        }break;
        case TREE_VARinfoOBJ:{
            _TObj*o=(_TObj*)data;
            td.IsImage = 0;
            td.IsLeaf = 0;
            td.parent = data;
            _TObj*_o=o->items;
            while(_o){
                char s[256];
                wsprintf(s,"%s",_o->object->_class->name);
                td.text=s;
                td.data=(DWORD)_o;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                _o=_o->next;
            };

            td.IsLeaf = 1;
            _TVar* v=o->vars;
            while(v){
                TVarInfo *vi=&(o->object->_class->vars[v->index]);
                char s[256];

                wsprintf(s,"%s %s",vi->name,vi->info?vi->info:"");
                if(vi->flags&VF_LEFT){
                    lstrcat(s," WRITE !!");
                };

                td.IsImage = VAR_IMG;
                if (o->object==o->data->object && o->data->index==v->index)
                    td.IsImage = CROSS_IMG;
                td.text=s;
                td.data=(DWORD)v;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                v=v->next;
            };

        }break;

        case TREE_SPACE3D:
        case TREE_OBJECTS2D:{
            HSP2D hsp=(HSP2D)data;
            HOBJ2D obj=0;
            while  ((obj=GetNextObject2d(hsp,obj))!=0){
                if(GetObjectParent2d(hsp,obj)==0){
                    __InsertObject2d(tree,data,hsp,obj);
                }
            };
        }break;
        case TREE_OBJECT3D:
        case TREE_OBJECT2D:{
            HSP2D hsp=(HSP2D)data;
            HOBJ2D obj=(HOBJ2D)HIWORD(data);
            INT16 count=GetGroupItemsNum2d(hsp,obj);
            if (count>0){
                HOBJ2D *items=new HOBJ2D[count];
                GetGroupItems2d(hsp,obj,items,count);
                for(int i=0;i<count;i++){
                    __InsertObject2d(tree,data,hsp,items[i]);
                }
                delete items;
            }
        };break;
        case TREE_TOOLS:{
            td.IsImage = 0;
            td.IsLeaf = 0;
            td.parent = data;
            DWORD parent=td.data;

            HSP2D hsp=(HSP2D)data;
            for(int i=0;i<7;i++){
                TOOL_TYPE t=(TOOL_TYPE)(i+1);
                HOBJ2D tool=GetNextTool2d(hsp,t,0);
                char s[64];
                if (tool){
                    LoadString(HResource,IDS_TOOLTYPE+i,s,sizeof(s));
                    td.parent=parent;
                    td.datatype = (INT16)(TREE_PENS+i);
                    td.data=MAKELONG(hsp,t);
                    td.text=s;

                    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                }   }
        }break;
        case TREE_PENS:
        case TREE_BRUSHS:
        case TREE_DIBS:
        case TREE_DDIBS:
        case TREE_STRINGS:
        case TREE_FONTS:
        case TREE_TEXTS:
        case TREE_TEXTURE3D:{
            HOBJ2D tool=0;
            TOOL_TYPE t=(TOOL_TYPE)((td.datatype-TREE_PENS)+1);
            INT16 oldtype=td.datatype;
            TreeItemData td;
            td.parent = data;
            td.IsLeaf = 1;
            td.IsImage = 0;
            td.datatype = TREE_TOOL1;

            HSP2D hsp=(HSP2D)LOWORD(data);
            while ((tool=GetNextTool2d(hsp,t,tool))!=0){
                td.data=MAKELONG(hsp,(WORD)(65535lu-(INT32)tool));
                char s[256];
                wsprintf(s,"#%d ",(WORD)tool);
                char* ps=s+lstrlen(s);
                switch(oldtype){
                    case TREE_PENS:{
                        INT16 width=GetPenWidth2d(hsp,tool);
                        INT16 style=GetPenStyle2d(hsp,tool);
                        COLORREF color=GetPenColor2d(hsp,tool);
                        INT16 rop=GetPenRop2d(hsp,tool);
                        char ss[20];
                        char srop[20];
                        char cmd[64];
                        GetPenStyleString(style,ss);
                        GetRopString(rop,srop);
                        LoadString(HResource,IDS_MSG24,cmd,sizeof(cmd));
                        wsprintf(ps,cmd,ss,width,GetRValue(color),
                                 GetGValue(color),GetBValue(color),srop);
                    };break;
                    case TREE_BRUSHS:{
                        INT16 style=GetBrushStyle2d(hsp,tool);
                        COLORREF color=GetBrushColor2d(hsp,tool);
                        INT16 rop=GetBrushRop2d(hsp,tool);

                        char ss[20];
                        char srop[20];
                        GetBrushStyleString(style,ss);
                        GetRopString(rop,srop);
                        wsprintf(ps,"%s RGB(%d,%d,%d)  %s",ss,GetRValue(color),
                                 GetGValue(color),GetBValue(color),srop);

                    };break;
                    case TREE_DIBS:{
                        HGLOBAL dib=GetDib2d(hsp,tool);
                        if(dib){
                            LPBITMAPINFOHEADER info = (LPBITMAPINFOHEADER)GlobalLock(dib);
                            GetBmpString(info,ps);
                            if (GetToolType2d(hsp,t,tool)==ttREFTODIB2D){
                                char file[255];
                                GetRDib2d(hsp,tool,file,sizeof(file));
                                wsprintf(ps+lstrlen(ps)," File: \"%s\"",file);
                            }
                            GlobalUnlock(dib);
                        }else{
                            wsprintf(ps+lstrlen(ps)," No image available");
                        }
                    };break;
                    case TREE_DDIBS:
                    {
                        HGLOBAL xor=GetXorDib2d(hsp,tool);
                        LPBITMAPINFOHEADER info = (LPBITMAPINFOHEADER)GlobalLock(xor);
                        GetBmpString(info,ps);
                        if (GetToolType2d(hsp,t,tool)==ttREFTODOUBLEDIB2D){
                            char file[255];
                            GetRDoubleDib2d(hsp,tool,file,sizeof(file));
                            wsprintf(ps+lstrlen(ps)," File: \"%s\"",file);
                        }
                        GlobalUnlock(xor);
                    }break;

                    case TREE_STRINGS:
                    {
                        GetLogString2d(hsp,tool,ps,100);
                    }break;

                    case TREE_FONTS:
                    {
                        LOGFONT2D lf;
                        GetLogFont2d(hsp,tool,&lf);
                        wsprintf(ps,"%s %d",lf.lfFaceName,lf.lfHeight);
                        if(lf.lfWeight>=700)lstrcat(ps," bold");
                        if(lf.lfItalic)lstrcat(ps," italic");
                        if(lf.lfUnderline)lstrcat(ps," underline");
                        if(lf.lfStrikeOut)lstrcat(ps," strikeout");

                    }break;

                    case TREE_TEXTS:
                    {

                        LOGTEXT lt;
                        GetLogText2d(hsp,tool,&lt,1);
                        char ss[256];
                        GetLogString2d(hsp,tool,ss,100);
                        wsprintf(ps,"#F:%d,#S:%d (%s)",lt.ltStringIndex,lt.ltFontIndex,ss);

                    }break;
                    case TREE_TEXTURE3D:{
                        GetMaterialText3d((HSP3D)hsp,tool,ps);
                    }break;
                }
                AddUseToolText2d(hsp,t,tool,s);
                td.text=s;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }}break;
        case TREE_EQS :
        case TREE_EQSinfo :
        case TREE_EQSmx :
        case TREE_EQSeq :
        case TREE_EQSvar:return GetEQTreeInfo(tree,data,td.datatype);

        case TREE_LIBRARY  :{
            TLibrary*lib=(TLibrary*)data;
            if (lib && lib->flags&CLF_LIBRARY){
                td.parent =(DWORD) lib;
                if (lib->items->count)
                    for(int i=0;i<lib->items->count;i++){
                        TLibrary*sublib=(TLibrary*)lib->items->At(i);
                        if (sublib){
                            if (sublib->flags&CLF_LIBRARY){
                                __InsertLib(tree,(DWORD)lib,sublib);
                            }else{
                                __InsertClass(tree,(DWORD)lib,(TClassListInfo*)sublib);
                            }
                        }
                    }else{
                    td.data=td.parent+1;
                    td.IsImage = 0;
                    td.IsLeaf = 1;
                    td.text="Empty library";
                    td.datatype=TREE_EMPTYCLASS;
                    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                }
            }
        }break;
        case TREE_CLASS    :{
        }break;
        case TREE_OBJECT :{
            PObject object=(TObject*)(data);
            PObject _po=object->childObjects;
            while(_po){
                __InsertObject(tree,data,_po);
                _po=_po->next;
            };
        }break;
        case TREE_VARSclass :{
        }break;
        case TREE_VARSobj   :{

        }break;
        case TREE_CHILDSclass  :{

        }break;

        case TREE_MAIN   :{
        }break;
        case TREE_STRATUM:{
            td.IsImage = 0;
            td.IsLeaf = 0;
            td.parent = data;
            char s[256];
            if (GetNextSpace2d(0)){
                LoadString(HResource,IDS_str75,s,sizeof(s));
                td.text=s;
                td.data = (DWORD)(td.text);
                td.datatype=TREE_SPACES;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            };

            LoadString(HResource,IDS_str74,s,sizeof(s));
            td.text = s;
            td.data = (DWORD)GetParentLibrary();
            td.datatype=TREE_LIBRARY;
            td.IsLeaf = 0;
            td.IsImage = 0;
            ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);

            PObject object=0;PClass _class=0; // view main scheme
            if (GetRootObject(&_class,&object)){
                __InsertObject(tree,data,object);
            };

            if (streams && streams->count){
                for(int i=0;i<streams->count;i++){
                    if (streams->At(i)){
                        LoadString(HResource,IDS_str76,s,sizeof(s));
                        td.text =s;


                        td.data = (DWORD)streams;
                        td.datatype=TREE_SCFS;
                        td.IsLeaf = 0;
                        td.IsImage = 0;
                        ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                        break;
                    }
                }
            };
            if (matrixs){
                BOOL exist=FALSE;
                for(int i=0;i<matrixs->plusItems->count;i++){
                    TMatrix*m=(TMatrix*)matrixs->plusItems->At(i);
                    if (m){exist=TRUE;break;}
                }
                if (!exist){
                    for(i=0;i<matrixs->minusItems->count;i++){
                        TMatrix*m=(TMatrix*)matrixs->minusItems->At(i);
                        if (m){exist=TRUE;break;}
                    }
                }
                if (exist){
                    LoadString(HResource,IDS_str77,s,sizeof(s));
                    td.text =s;
                    td.data = (DWORD)matrixs;
                    td.datatype=TREE_MATMAN;
                    td.IsLeaf = 0;
                    td.IsImage = 0;
                    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                }
            };

            if(GetNextVideo(0)){
                LoadString(HResource,IDS_str78,s,sizeof(s));
                td.text =s;
                td.data = (DWORD)LoadString;
                td.datatype=TREE_VIDEOS;
                td.IsLeaf = 0;
                td.IsImage = VIDEO_IMG;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }
            if (winmanager->count){
                LoadString(HResource,IDS_str79,s,sizeof(s));
                td.text =s;
                td.data = (DWORD)(winmanager);
                td.datatype=TREE_WINMAN;
                td.IsLeaf = 0;
                td.IsImage = 0;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }


#ifdef DBE
            int sql;
            DBSQLPParam(sql);
            if (sql){
                td.datatype=TREE_DBE;
                td.IsImage = 0;
                td.IsLeaf = ((sql&2)==0);
                td.text =s;
                LoadString(HResource,IDS_str80,s,sizeof(s));
                td.data = (DWORD)DBSQLPParam;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }
#endif
            for(C_TYPE i=0;i<dyno_mem->count;i++){
                TDynVar*v=(TDynVar*)dyno_mem->At(i);
                if(v){
                    td.parent=data;
                    td.datatype=TREE_DYNOMEMORY;
                    td.IsImage = 0;
                    td.IsLeaf = 0;
                    td.text =s;
                    LoadString(HResource,IDS_str81,s,sizeof(s));
                    td.data = (DWORD)dyno_mem;
                    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                    break;
                }
            }
            if(dllmanager && dllmanager->dlls->count){
                td.parent=data;
                td.datatype=TREE_PLUGINS;
                td.IsImage = DLL_IMG;
                td.IsLeaf = 0;
                td.text =s;
                LoadString(HResource,IDS_str82,s,sizeof(s));
                td.data = (DWORD)dllmanager;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }
        }break;
        case TREE_PLUGINS:{
            td.parent=data;
            td.datatype=TREE_PLUGIN;
            td.IsImage = DLL_IMG;
            td.IsLeaf = 0;

            for(C_TYPE i=0;i<dllmanager->dlls->count;i++){
                TFCollection*d=(TFCollection*)dllmanager->dlls->At(i);
                char s[256];
                wsprintf(s,"External module: %s",d->dllname);
                if(d->hdll){
                    lstrcat(s," Loaded");
                }else lstrcat(s," Not loaded");
                td.text = s;
                td.data = (DWORD)d;
                td.IsLeaf = (d->count==0);
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }}break;
        case TREE_PLUGIN:{
            td.parent=data;
            td.datatype=TREE_DLLFUNC;
            td.IsImage = 0;
            td.IsLeaf = TRUE;
            TFCollection*d=(TFCollection*)data;
            for(C_TYPE i=0;i<d->count;i++){
                TDLLFunction*f=(TDLLFunction*)d->At(i);
                char s[256];
                if(f->Type)lstrcpy(s," _pascal ");else lstrcpy(s," _cdecl ");
                lstrcat(s,f->Name);
                if(f->RealName){
                    wsprintf(s+lstrlen(s)," ( %s ) ",f->RealName);
                }
                td.text = s;
                td.data = (DWORD)f;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);

            }
        }break;
#ifdef DBE
        case TREE_DBE:
        case TREE_BASE:
        case TREE_TABLE:{
            __InsertBDEObj(tree,td.datatype,data);
        }break;
#endif
        case TREE_WINMAN:{
            td.IsLeaf = 1;
            td.parent = data;
            td.IsImage = WINDOW_IMG;
            td.datatype=TREE_WINITEM;
            for(C_TYPE i=0;i<winmanager->count;i++){
                TWinItem *w=(TWinItem*)winmanager->At(i);
                char s[256];
                wsprintf(s,"%s #%d",w->name,(int)w->hsp);
                td.text=s;
                td.data = (DWORD)w;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }
        }break;
        case TREE_SCFS:{
            td.IsLeaf = 1;
            td.parent = data;
            td.IsImage = 0;
            td.datatype=TREE_FILE;
            for(int i=0;i<streams->count;i++){
                TStreamItem*item=(TStreamItem*)streams->At(i);
                if (item){
                    char s[256];
                    wsprintf(s,"%s stream #%d ",item->GetTypeStr(),item->HANDLE);
                    if (item->type==ST_FILE){
                        lstrcat(s,((TDOSStream*)(item->stream))->GetFileName());
                    }
                    wsprintf(s+lstrlen(s)," size: %ld pos:%ld",item->stream->GetSize(),item->stream->GetPos());
                    td.text = s;
                    td.data = (DWORD)item;
                    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                }
            }
        }break;
        case TREE_MATMAN:{
            for(int i=0;i<matrixs->plusItems->count;i++){
                TMatrix*m=(TMatrix*)matrixs->plusItems->At(i);
                __InsertMatrix(tree,data,m);
            }
            for(i=0;i<matrixs->minusItems->count;i++){
                TMatrix*m=(TMatrix*)matrixs->minusItems->At(i);
                __InsertMatrix(tree,data,m);
            }
        }break;
        case TREE_SPACES:{

            td.IsLeaf = 1;
            td.parent = data;
            td.datatype=TREE_SPACE;
            HSP2D hsp=0;
            while((hsp=GetNextSpace2d(hsp))!=0){
                td.IsImage = 0;
                char s[256];
                char ss[256];
                HWND hwnd=GetSpaceWindow2d(hsp);
                int str=0;
                if (hwnd){
#ifdef WIN32
                    //	   if(GetWindowTask(hwnd)==GetCurrentProcess())
                    if(TRUE)
#else
                    if(GetWindowTask(hwnd)==GetCurrentTask())
#endif
                    {
                        SendMessage(hwnd,TW_GETSPACEINFO,256,(LPARAM)ss);
                    }else str=IDS_SPACE1;//lstrcpy(ss,"Пространство принадлежит другой программе");

                }else{
                    if (hsp==spec_space){
                        str=IDS_SPACE1+1;//		lstrcpy(ss,"служебное пространство");
                        td.IsImage = 0;
                    }else{
                        str=IDS_SPACE1+2; //lstrcpy(ss,"пространство без окна");
                        td.IsImage = WARNING_IMG;
                    }
                }

                if(str)LoadString(HResource,str,ss,sizeof(ss));

                if(IsSpaceChanged2d(hsp))lstrcat(ss," Modifyed");
                wsprintf(s,"#%d %s",hsp,ss);
                td.text=s;
                td.data = ((INT16)hsp)+1;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            };
        }break;
        case TREE_SPACE:{
        }break;
        case TREE_VIDEOS:{
            HV2D hv=0;
            td.parent = data;
            td.datatype=TREE_VIDEO;
            td.IsLeaf = 1;
            td.IsImage = VIDEO_IMG;

            while((hv=GetNextVideo(hv))!=0){
                VIDEOPROP vp;
                GetVideoProp(hv,&vp);
                AnsiLower(vp.file);
                char s[256],sf[256];
                long time=(vp.timeend-vp.timestart)/1000l;
                long d=((vp.timeend-vp.timestart)%1000l)/100l;
                LoadString(HResource,IDS_str83,sf,sizeof(sf));
                wsprintf(s,sf,(int)hv,vp.file,(int)vp.sizex,(int)vp.sizey,(long)time,(long)d);
                if (vp.audioChannels){
                    char * mono=" Mono";
                    if(vp.audioChannels==2)mono=" Stereo";
                    wsprintf(s+lstrlen(s),"%s %ld",mono,vp.audioSamplesPerSec);

                }else {
                    LoadString(HResource,IDS_str84,s+lstrlen(s),24);
                }
                td.text = s;
                td.data = hv;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }
        }break;

    };
    return 0;
};
#endif // runtime
int TMainWindow::__TreeItemMenu(HWND tree,DWORD data){
#ifndef RUNTIME
    TSTRATUMMENU sm;
    memset(&sm,0,sizeof(sm));
    sm.hWindow=HWindow;
    sm.hType=SCMENU_TREEITEM;

    TreeItemData td;
    td.data=data;
    HMENU  p=0;
    if(!::SendMessage(tree,TVN_GETITEMINFO,0,(long)&td))return 0;
    switch(td.datatype){
        /*n
  case TREE_OBJECT3D:
  case TREE_OBJECT2D:{
     p=CreatePopupMenu();
     AppendMenu(p,MF_ENABLED,CM_PROPERTIESobj2d,"&Свойства...");
    AppendMenu(p,MF_ENABLED,CM_EDITDELETEobj2d,"&Удалить");
  }break;
  */

        case TREE_LIBRARY:{
            p= ScLoadPopup(MENU_LIB);

            PLibrary lib=(PLibrary)td.data;
            sm.hType=SCMENU_LIBRARY;
            sm.data1=td.data;

            if(lib->flags&CLF_PROJECT){
                ScAppendMenu(p,MF_ENABLED,CM_DELUNUSED,IDS_MENU17);
                ScAppendMenu(p,MF_ENABLED,CM_RENAMEPATHS,IDS_MENU74);
            }
            ScAppendMenu(p,MF_ENABLED,CM_RECOMPLILE,IDS_MENU73);

            if (((lib->flags&CLF_PROJECT)==0) && (lib->parent) && (lib->parent->parent)){
                AppendMenu(p,MF_SEPARATOR,0,"");
                ScAppendMenu(p,MF_ENABLED,CM_SETASDEF,IDS_MENU18);
                ScAppendMenu(p,MF_ENABLED,CM_HELPONIMAGE,IDS_MENU19);

            }
            AddCopyCutMenu(p);
        }break;
        case TREE_OBJECT:
        case TREE_CLASS:{
            PClass _class=NULL;

            switch(td.datatype){
                case TREE_OBJECT:_class=((PObject)data)->_class;p= ScLoadPopup(MENU_TREEIMAJE);break;
                case TREE_CLASS:_class=((PClassListInfo)data)->GetClass();p= ScLoadPopup(MENU_TREEIMAJECLASS);break;
            }
            CorrectClassMenu(p,_class);
            if (td.datatype==TREE_CLASS){

                ScAppendMenu(p,MF_ENABLED,CM_EDITDELETE,IDS_MENU20);
                ScAppendMenu(p,MF_ENABLED,CM_DUPLICATE,IDS_MENU21);
                HMENU p1=CreatePopupMenu();
                AppendMenu(p,MF_SEPARATOR,0,"");
                ScAppendMenu(p,MF_POPUP,(UINT)p1,IDS_MENU22);
                ScAppendMenu(p1,MF_ENABLED,CM_NEWCLASS,IDS_MENU23);
                ScAppendMenu(p1,MF_ENABLED,CM_NEWLIB,IDS_MENU24);
            }

            if(((((TClassListInfo*)td.data)->flags&CLF_PROJECT)==0) || (((TClassListInfo*)td.data)->GetClass()->helpfile)){
                AppendMenu(p,MF_SEPARATOR,0,"");
                ScAppendMenu(p,MF_ENABLED,CM_HELPONIMAGE,IDS_MENU25);
            }
            if(td.datatype==TREE_CLASS)AddCopyCutMenu(p);
        }break;
    };
    sm.hMenu=p;
    if (p){
        SetDefaultMenuItem(p,CM_PROPERTIES);
        TrackPopupMenu(lastmousepos,&sm);}
    return 0;
#endif
};


#ifndef RUNTIME
int __EditTreeItem(HWND tree,DWORD data,BOOL newitem){
    TreeItemData td;
    td.data=data;
    if(!::SendMessage(tree,TVN_GETITEMINFO,0,(long)&td))return 0;
    if (newitem){
        if (td.datatype!=TREE_EMPTYCLASS)return 0;
    }
    switch(td.datatype){
        case TREE_EQSmx :{
            MessageBox(tree,"Matrix editor here","Matrix editor here",MB_OK|MB_ICONINFORMATION);
        }break;
        case TREE_TOOL1:{
            TreeItemData tdp;
            tdp.data=td.parent;
            if(!::SendMessage(tree,TVN_GETITEMINFO,0,(long)&tdp))return 0;
            switch(tdp.datatype){
                case TREE_TEXTURE3D:{
                    HSP3D  hsp3d =(HSP3D)LOWORD(td.data);
                    HOBJ2D hobj  =(HOBJ2D)(65535ul-(INT32)HIWORD(td.data));
                    TextureDialog(tree,hsp3d,hobj);
                }break;
            }
        }break;
        case TREE_STRATUM:{
            AboutDialog(tree,1);break;
        }break;
        case TREE_PLUGIN:{
            TFCollection*f=(TFCollection*)data;
            f->Procedure(SC_DLLDIALOG,(UINT32)tree,0);
        }break;
        case TREE_DBE:{
#ifdef DBE
            char s[256];
            DbiGetSysVersion(s);
            MessageBox(tree,s,"Database module",MB_OK|MB_ICONINFORMATION);
#endif
            break;
        };
        case TREE_TABLE:{
#ifdef DBE
            TableDialog(tree,DBhandleByPointer(td.data));
#endif
        }break;
        case TREE_ANIMATE:break;//AnimateDiloag(tree,(TAnime*)td.data);
        case TREE_SPACE:{
            HSP2D hsp = (HSP2D )(td.data-1);
            SpaceDialog(tree,hsp,0,0,0,0);
        }break;
        case TREE_MATRIX:{
            TMatrix*m=(TMatrix*)data;
            MatrixDialog(tree,m->handle);
        }break;
        case TREE_EMPTYCLASS:
        case TREE_CLASS:{
            TClass *_class=0;
            TClassListInfo*pcli=(TClassListInfo*)data;
            PLibrary lib=(PLibrary)td.parent;
            if (td.datatype==TREE_EMPTYCLASS){
                if (newitem){
                    pcli=NULL;
                }else return 0;
            }
            if (!newitem){
                _class=GetClassByName(pcli->GetClassName());
                //	 TClassListInfo*pcli=(TClassListInfo*)data;
            }

            int rez=ClassDialog(tree,_class,NULL,lib);
            if (rez && (td.datatype==TREE_EMPTYCLASS)){
                SendMessage(tree,TVN_DELETEITEM,0,data);
            }
        }return 0;
        case TREE_OBJECT: ClassDialog(tree,NULL,(TObject*)data,NULL);return 0;
        case TREE_VIDEO:  VideoInfoDialog(tree,(HV2D)data);return 0;
        case TREE_OBJECT2D:{
            HSP2D hsp=(HSP2D)data;
            HOBJ2D obj=(HOBJ2D)HIWORD(data);
            Object2dDialog(tree,hsp,obj,0);
        }break;
        case TREE_OBJECT3D:{
            HSP3D hsp=(HSP3D)data;
            HOBJ2D obj=(HOBJ2D)HIWORD(data);
            Object3dDialog(tree,hsp,obj);
        }break;
    }
    return 0;
};
#endif
void SetControlBarRange(int range){
    // if (mainwindow)SendMessage(mainwindow->controlbar,CB_SETRANGE,range,0);
};
void SetStatusMode(BOOL normal){
    if (mainwindow) _StatusBar->SetStatusMode(normal);
};
void SetSimpleText(char*txt){
    if (mainwindow) _StatusBar->SetSimpleText(txt);
};
void SetDisableText(int id){
    char txt[256];
    if(!LoadString(HResource,id,txt,255))lstrcpy(txt,"");
    SetSimpleText(txt);
};

extern "C" void _SDKPROC SCSetStatusProgressBar(int id,int percent){
    if (mainwindow) _StatusBar->SetText(id,(char*)(percent));
};

extern "C" void _SDKPROC SCSetStatusText(int id,char*txt)
{
    if (mainwindow)
    {
        if(id>-1)_StatusBar->SetText(id,txt);
        else _StatusBar->SetSimpleText(txt);
    }
};

void SetStatusIcon(int id,int _id)
{
    _StatusBar->SetIcon(id,_id);
};

BOOL scCALLBACK EnUpdateWindows(HWND hwnd,LPARAM lParam)
{
    SendMessage(hwnd,TW_UPDATECLASSWINDOWS,0,lParam);
    return TRUE;
};

int _UpdateClassWindows(UPDATE_MODE mode,UPDATE_WHAT what,void *data,void *parent)
{
    if (mainwindow)
    {
        UPDATESTRUCT us;
        us.mode=mode;
        us.what=what;
        us.data=data;
        us.parent=parent;
        return EnumChildWindows(mainwindow->GetMDI(),EnUpdateWindows,(LPARAM)&us);
    }
    return 0;
};

int UpdateClassWindows(UPDATE_MODE mode,UPDATE_WHAT what,void *data,void *parent)
{
    if (mainwindow)
    {
        if((what==UW_CLASS) && (mode==UW_MODIFY) &&winmanager->IsWindowExist("@Hierarchy@"))
        {
            PClassListInfo pcli=(PClassListInfo)data;
            if(pcli && (pcli->flags&CLF_LOADED))
            {
                PClass _class=pcli->GetClass();
                PObject po=_class->first;
                while(po)
                {
                    _UpdateClassWindows(UW_MODIFY,UW_OBJECT,po,po->parent);
                    po=po->class_next;
                }
            }
        }
        return _UpdateClassWindows(mode,what,data,parent);
    }
    return 0;
};
POINT TWindow::lastmousepos;

int MatEditor(int m)
{
    MatrixDialog(mainwindow->HWindow, m);
    return 0;
};


int SCMessageBox(LPCSTR lpszText,LPCSTR lpszTitle,UINT fuStyle)
{
    return MessageBox(mainwindow->HWindow,lpszText,lpszTitle,fuStyle);
};

void SystemQuit(int)
{
#ifdef WIN32
    PostMessage(mainwindow->HWindow, WM_COMMAND, MAKELONG(CM_EXIT, 0), (LPARAM)mainwindow->HWindow);
#else
    PostMessage(mainwindow->HWindow,WM_COMMAND,CM_EXIT,0);
#endif
};

HSP2D Vm_OpenClassScheme(char * classname,UINT32 /*flags*/)
{
#ifndef RUNTIME
    PClass _class = GetClassByName(classname);
    if(_class)
    {
        TSchemeWindow *sw = mainwindow->CreateSchemeWindow(classname,_class->first);
        if (sw)
            return sw->hsp;
    }
#endif // runtime
    return 0;
}

int Vm_CloseClassScheme(char * classname)
{
#ifndef RUNTIME
    PClass _class = GetClassByName(classname);
    if (_class->scheme && _class->scheme->Loaded())
    {
        TSchemeWindow * alredy = (TSchemeWindow*)mainwindow->GetWindowWithSpace(_class->scheme->GetSpace());
        if (alredy)
        {
            HWND wnd = GetParent(alredy->HWindow);
            SendMessage(mainwindow->GetMDI(),WM_MDIDESTROY,(WPARAM)wnd,0);
        }
    }
#endif // runtime
    return 0;
}

BOOL TMainWindow::SaveAllEx()
{
    BOOL rez=FALSE;
    TStringCollection*items=SaveChangesDialog(HWindow);
    if(items)
    {
        rez=AcceptChanges(TRUE,items);
        if(rez)
        {
            if(items->count)
            {
                for(C_TYPE i=0;i<items->count;i++)
                {
                    char * s=(char *)items->At(i);
                    delete s;
                }
                items->DeleteAll();
            }
            delete items;
        }
    }
    return rez;
};

BOOL TMainWindow::CanCloseProject(){
    char tmp[256];
    char s[256];
    //char cmd[128];
    if (IsNeedAcceptChanges(tmp)){
        // LoadString(HResource,IDS_MSG23,cmd,sizeof(cmd));
        // wsprintf(s,cmd ,tmp);
        // int ret=MSGBOX(HWindow,s,_MSG_QSC,MB_DEFBUTTON2|MB_YESNOCANCEL|MB_ICONQUESTION);
        int ret=IsNeedSaveChanges(HWindow,tmp);
        if (ret!=IDCANCEL){
#ifdef DEMO
            if(ret==IDYES || ret==IDRETRY){
                DemoMSG();return FALSE;
            }
#else
#ifndef RUNTIME
            if(ret==IDRETRY){
                ret=SaveAllEx();
            }else{
                ret=AcceptChanges(ret==IDYES);
            }
#endif // runtime
#endif
            if (ret)return TRUE;
        }
        return FALSE;
    }
    return TRUE;
};

BOOL TMainWindow::CanClose(){
    for(int i=0;i<winmanager->allmdi->count;i++)
    {
        TMDIChildWindow * wc=(TMDIChildWindow *)winmanager->allmdi->At(i);
        if ((wc->MODE!=MODE_BMPEDIT) &&
            (!wc->CanClose()) )return FALSE;
    }

    BOOL rez=CanCloseProject();
    if(rez){
        //   EXEC_FLAGS=0;
        //   Vm_Halt();
        Cm_ClearAll();

        Cm_ClearWindows();
        Cm_CloseChildren(FALSE);

        CloseProject((TProject*)GetCurrentProject());
        StoreDesktop();
    }
    return rez;
};

void PushPos(TStream&st,long& pos){
    pos=st.GetPos();
    st.WriteLong(0);
};

void PopPos(TStream&st,long& pos){
    long _pos=st.GetPos();
    st.Seek(pos);
    st.WriteLong(_pos);
    st.Seek(_pos);
};
char * cnt_tpl=NULL;
void TMainWindow::StoreDesktop(){
    TDOSStream st(cnt_tpl,TDOSStream::stCreate|TDOSStream::stNoWarning);

    if(!st.status){
        st.WriteStr("DSK");
        long pos;
        // ControlBar
        st.WriteWord(1);
        PushPos(st,pos);
        StoreControlBars(st);
        PopPos(st,pos);


        // Window Pos // must be last
        st.WriteWord(2);
        PushPos(st,pos);

        WINDOWPLACEMENT wp;
        wp.length=sizeof(wp);
        GetWindowPlacement(HWindow,&wp);
        RECT rect=wp.rcNormalPosition;

        INT16 code=0;
        if(IsZoomed(HWindow))code|=1;
        if(IsIconic(HWindow))code|=2;
        st.WriteWord(code);

        st.WriteWord((INT16)rect.left);
        st.WriteWord((INT16)rect.right);
        st.WriteWord((INT16)rect.top);
        st.WriteWord((INT16)rect.bottom);

        PopPos(st,pos);
        // Messages
        // End

        // recently
        if(recently->count){
            st.WriteWord(3);
            PushPos(st,pos);
            st.WriteWord(recently->count);
            for(C_TYPE i=0;i<recently->count;i++){
                st.WriteStr((char*)recently->At(i));
            }
            PopPos(st,pos);
        }
        // end recently
        // sc vars
        st.WriteWord(4);
        PushPos(st,pos);
        StoreScVars(st);
        PopPos(st,pos);

        st.WriteWord(5); // custom palette
        COLORREF extern aclrCust[];
        PushPos(st,pos);
        st.Write(aclrCust,4*16);
        PopPos(st,pos);

        // Full end
        st.WriteWord(0);
    }
};

void TMainWindow::LoadDesktop()
{
    TDOSStream st(cnt_tpl,TDOSStream::stOpenRead|TDOSStream::stNoWarning);
    char s[64];
    if(!st.status)
    {
        st.ReadStr(s,sizeof(s));
        if(!lstrcmp(s,"DSK"))
        {
            INT16 code=0;
            while((code=st.ReadWord())!=0){
                long pos=st.ReadLong();
                switch(code){
                    case 1:{
                        RestoreControlBars(st);
                    }break;
                    case 2:{
                        INT16 code=st.ReadWord();
                        RECT rect;

                        rect.left   =st.ReadWord();
                        rect.right  =st.ReadWord();
                        rect.top    =st.ReadWord();
                        rect.bottom =st.ReadWord();
                        int mode=SWP_NOZORDER;
                        if(code){
                            if( (code&1)&&(!IsZoomed(HWindow)))mode|=SWP_NOREDRAW;
                            if( (code&2)&&(!IsIconic(HWindow)))mode|=SWP_NOREDRAW;
                        }
                        SetWindowPos(HWindow,title,rect.left,rect.top,rect.right-rect.left,
                                     rect.bottom-rect.top,mode);
                        if(code&2)ShowWindow(HWindow,SW_SHOWMINIMIZED);
                        if(code&1)ShowWindow(HWindow,SW_SHOWMAXIMIZED);

                    }break;
                    case 3:{
                        C_TYPE count=st.ReadWord();
                        for(C_TYPE i=0;i<count;i++){
                            recently->Insert(st.ReadStr());
                        }
                    }break;
                    case 4:{
                        LoadScVars(st);
                    }break;
                    case 5:{
                        COLORREF extern aclrCust[];
                        st.Read(aclrCust,4*16);
                    }break;
                }
                st.Seek(pos);
            } }

    }else{
        // Create new dsk
#ifndef RUNTIME
        ShowControlBar(2,TRUE);
        ShowControlBar(20,TRUE);
        if(docks[0]->count>1){
            TControlBar*bar1=(TControlBar*)((TCollection*)docks[0]->At(0))->At(0);
            TControlBar*bar2=(TControlBar*)((TCollection*)docks[0]->At(1))->At(0);
            RECT r1,r2;
            GetWindowRect(bar1->HWindow,&r1);
            GetWindowRect(bar2->HWindow,&r2);
            r2.top=r1.top;r2.bottom=r1.bottom;
            r1.right-=5;
            r2.left+=r1.right;
            r2.right+=r1.right;
            bar2->trackrect=r2;
            bar2->_dock=bar2->dock;
            AdjustControl(bar2);
        }
#endif
        ShowControlBar(1,TRUE);

#ifndef RUNTIME
        SCSetScVarI("preload_compiler",1);
        SCSetScVarI("editor2d_opt",2);
        SCSetScVarI("CanUseCyr",1);
        SCSetScVarI("use_syntax_edit",1);
        SCSetScVarS("syntax_scheme","Auto");
        SCSetScVarI("syntax_edit_fontsize",11);
        // SCSetScVarS("syntax_edit_font","Courier New Cyr");
        SCSetScVarI("OnStartup",2);
#endif
        SCSetScVarI("status_perfomance",1);
        SCSetScVarI("cntbar_grayed",1);

        SCSetScVarS("equ_diff","Euler's");
        SCSetScVarS("equ_linear","Gauss");
        SCSetScVarS("equ_nonlinear","Newton-Raphson");

    }

    RebuildRecentlyMenu();
    _StatusBar->Rebuild();
    SetLastSteps();
    SetCntBarStyle(SC_GetScVar("cntbar_style",0));
};

BOOL _SendCommand(HWND to,UINT command){
#ifdef WIN32
    return SendMessage(to,WM_COMMAND, MAKEWPARAM((UINT)(command),(UINT)(0)),0);
#else
    return SendMessage(to,WM_COMMAND,command,0);
#endif
}

LRESULT SendCommand(HWND to,HWND from,UINT codeNotify){
#ifdef WIN32
    return SendMessage(to,WM_COMMAND, MAKEWPARAM((UINT)(GetWindowLong(from,GWL_ID)),(UINT)(codeNotify)), (LPARAM)(HWND)(from));
#else
    return SendMessage(to,WM_COMMAND,GetWindowWord(from,GWW_ID),MAKELONG(from,codeNotify));
#endif
};

BOOL LoadResources(){
#if !defined(WIN32)
    ICON_EDITO2D=LoadIcon(HResource,"ICON_SCHEME");
    ICON_TEXT=LoadIcon(HResource,"ICON_TEXT");
    ICON_BMP=LoadIcon(HResource,"ICON_BMP");
    ICON_TREE=LoadIcon(HResource,"ICON_TREE");
    ICON_LIB=LoadIcon(HResource,"ICON_LIB");
    ICON_MESSAGE=LoadIcon(HResource,"ICON_MESSAGE");
#endif
    ICON_FOLDER=LoadIcon(HResource,"ICON_FOLDER");
    return TRUE;
};
BOOL FreeResources(){
#if !defined(WIN32)
    DeleteObject(ICON_EDITO2D);
    DeleteObject(ICON_TEXT);
    DeleteObject(ICON_BMP);
    DeleteObject(ICON_TREE);
    DeleteObject(ICON_LIB);
    DeleteObject(ICON_MESSAGE);
#endif
    DeleteObject(ICON_FOLDER);
    DeleteObject(ICON_MAIN);
    return TRUE;
};

void SetMainTitle(char*t)
{
    if (mainwindow)
    {
        char s[999]="";
#ifdef SHOWPATHATTITLE
        if (t)
        {
            lstrcat(s,t);
            lstrcat(s," - ");
        }
#endif
        lstrcat(s,"Stratum");
#ifdef DEMO
        lstrcat(s," (Demo)");
        //#elif defined LITE
        //  lstrcat(s,"Stratum (HOME version)");
#elif defined RUNTIME
        lstrcat(s," (Runtime)");
#endif
#ifdef DBE
        lstrcat(s," (Professional)");
#endif
        SetWindowText(mainwindow->HWindow,s);
    }
};

void DemoMSG()
{
    MSGBOX(mainwindow->HWindow,MAKEINTRESOURCE(IDS_str167)
           ,_MSG_INFO, MB_OK|MB_ICONINFORMATION);
};


void __gethelpfile(char*s,int mode)
{
    lstrcpy(s,sc_path);
    lstrcat(s,"\\help\\");
    switch(mode)
    {
        //case 1:lstrcat(s,"graphic.HLP");break;
        //case 2:lstrcat(s,"library.HLP");break;
        case 3:lstrcat(s,"dialogs.HLP");break;
            //case 4:lstrcat(s,"ide.HLP");break;
        default:lstrcat(s,"Sc3.HLP");
    }
};

void HELP(char *string)
{
    char s[256];
    int mode=0;
    if(*string=='#')
    {
        mode=2;
        string++;
    }
    else
    {
        PClassListInfo pcli=GetClassListInfo(string);
        if(pcli && ((pcli->flags&CLF_PROJECT)==0))
            mode=2;
    }
    if(!lstrcmpi(string,"order form"))mode=3;
    __gethelpfile(s,mode);
    WinHelp(mainwindow->HWindow,s,HELP_PARTIALKEY,(DWORD)string);
};

void HELP(int context,int x,int y,HWND hwnd)
{
    char s[256];
    __gethelpfile(s,3);
    WinHelp(hwnd,s,HELP_CONTEXTPOPUP,context);
};

#include "idehelp.h"

void HELP(int context)
{
    UINT type;
    char s[256];
    int mode=0;
    type=HELP_CONTEXT;
    switch(context){
        case CM_SUPPORT:   {mode=4;context=Technical_Support;}break;
        case CM_HELPTHEORY:{mode=4;context=Modeling_theoretic;}break;
        case CM_HELPKEYBOARD:{mode=4;context=Keyboards;}break;
        case CM_HELPUSING_HELP:{mode=4;context=Using_help;}break;
        case CM_LIBHELP:HELP("Standard libraries");return;
            //case CM_LIBHELP:{mode=2;context=0;type=HELP_CONTENTS;}break;
        case CM_HELPINDEX1:
        {
            context=0;
#ifdef WIN32
            type=HELP_FINDER
     #else
            type=HELP_CONTENTS
     #endif
                 ;
        }break;
        default:{mode=4;}break; // Для диалогов
    }
    __gethelpfile(s,mode);
    WinHelp(mainwindow->HWindow,s,type,context);
};

TMDIChildWindow*GetMDIParent(HWND hwnd)
{
    HWND p=GetParent(hwnd);
    TMDIChildWindow*m=(TMDIChildWindow*)GetWindowLong(p,0);
};

BOOL InitSyntax()
{
    TFCollection*f=dllmanager->LoadPlugin(
                   #ifdef WIN32
                       "Syntax32.dll"
                   #else
                       "Syntax16.dll"
                   #endif
                       ,2);
    return (f!=0);
};

HSP2D LdSpace2d(HWND hw, char *file)
{
    char s[256];
    GetPath(file,s);
    //SetTexturePath2d(hsp,s);
    HSP2D hsp=LoadSpace2d(hw, file, scheme_path, s);
    return hsp;
};

HSP2D LdFromMemory2d(HWND hw,HGLOBAL bl,char *path)
{
    HSP2D hsp = LoadFromMemory2d(hw, bl, scheme_path, path);
    return hsp;
};


BOOL IsNeedScroller(TScroller*v,long x2,long vrmin,long vrmax){
    long x1 = v->GetPos();
    x2 = x1+x2;
    if(x1>vrmin)return TRUE;
    if(x2<vrmax)return TRUE;
    return FALSE;
};
BOOL SetScrollData(TScroller*v,long x2,long vrmin,long vrmax,BOOL show){
    if(!v)return FALSE;
    long pos=v->GetPos();
    long _x1=0,_x2=0;
    long page=x2;
    if(show){
        long x1 = pos;
        if(vrmin<x1){
            //if(vrmax>x2){
            _x1=vrmin;
            _x2=max(vrmax-(x2+0),pos-1);
            // }else{ _x1=vrmin; _x2=pos;}
        }else{
            {_x1=pos;
                _x2=vrmax-(x2);
            }
        }
    }
    v->SetParam(_x1,_x2,pos,show,page);
    return TRUE;
};

BOOL  TWindow::SetScrollRange(RECT&maxrect){
    RECT wr;
    if(hscroll || vscroll){
        if(_scrollFlag)return FALSE;
        _scrollFlag=TRUE;

        GetWindowRect(HWindow,&wr);
        wr.right-=wr.left;wr.left=0;
        wr.bottom-=wr.top;wr.top=0;
        POINT scrollsize;
        scrollsize.x=GetSystemMetrics(SM_CXVSCROLL);
        scrollsize.y=GetSystemMetrics(SM_CYHSCROLL);
        BOOL nv=0,nh=0;
        if(hscroll) nh=IsNeedScroller(hscroll,wr.right,maxrect.left,maxrect.right);
        if(nh)wr.bottom-=scrollsize.y;
        if(vscroll) nv=IsNeedScroller(vscroll,wr.bottom,maxrect.top,maxrect.bottom);
        if(nv)wr.right-=scrollsize.x;
        if(hscroll && (nh==0)){ nh=IsNeedScroller(hscroll,wr.right,maxrect.left,maxrect.right);
            if(nh)wr.bottom-=scrollsize.y;
        }
        SetScrollData(hscroll,wr.right ,maxrect.left,maxrect.right ,nh);
        SetScrollData(vscroll,wr.bottom,maxrect.top ,maxrect.bottom,nv);
        _scrollFlag=FALSE;
        return TRUE;
    }
    return FALSE;
};

extern "C" BOOL _SDKPROC SCGetWindowInfo(HWND hwnd,TSTRATUMWINDOW*p){
    TWindow*w=(TWindow*)SendMessage(hwnd,scwnd_message,0,0);
    p->flags = 0;
    if(w){
        p->hWindow=w->HWindow;

        if(hwnd==mainwindow->HWindow){
            p->type  = SCWT_MAIN;
            return TRUE;
        }
        char s[64];
        if(GetClassName(hwnd,s,sizeof(s)))

            if(lstrcmpi(s,"SC_Scheme_WINDOW")==0){
#ifndef RUNTIME
                p->type=SCWT_SCHEME;
                p->flags=SCWF_SPACE2D|SCWF_EDITOR;
                TSchemeWindow*sw=(TSchemeWindow*)w;
                if(sw->_class)p->flags|=SCWF_CLASS;
                if(sw->_object)p->flags|=SCWF_OBJECT;
                if(sw->hardmode==SCHEME_EDITOR)p->flags|=SCWF_SCHEME;else
                    if(sw->hardmode==IMAGE_EDITOR)p->flags|=SCWF_IMAGE;
                p->hSpace=sw->hsp;
                return TRUE;
#else
                return FALSE;
#endif // runtime
            } else
                if(lstrcmpi(s,"SC_MSG_WINDOW")==0){
                    p->type=SCWT_SPACE;
                    p->flags=SCWF_SPACE2D;
                    return TRUE;
                }
    }else {
        p->type  =  SCWT_ERROR;
    }
    return FALSE;
};

TWaitCursor::TWaitCursor(HWND){
    HCURSOR old=::SetCursor(LoadCursor(0,IDC_WAIT));
};
TWaitCursor::~TWaitCursor(){
    ::SetCursor(old);
};

void LaunchURL(char *url)
{
    char buf[256];
    if(HIWORD(url)==0)
    {
        LoadString(HResource,LOWORD(url),buf,sizeof(buf));
        url=buf;
    }
    char s[256];
    wsprintf(s,"%s\\data\\custom.url",sc_path);
    WritePrivateProfileString("InternetShortcut","URL",url,s);
    if(mainwindow)ShellExecute(mainwindow->HWindow,"open",s,NULL,sc_path,SW_SHOW);
}
BYTE openaslink;
