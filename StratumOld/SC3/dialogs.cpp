/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            dialogs.cpp
Author:
            Alexander Shelemekhov
*/

#undef NOSERVICE
#undef NOIME
#undef NOMCX
#undef _WINNETWK_
#undef _WINREG_
#undef _WINCON_
#undef NONLS

#ifdef WIN32
#include "shlobj.h"
#endif

#define FILE_ITERATOR
#include "_dialogs.h"
#include "dbengine.h"
#include <string.h>
//#include <stratum\sc_msg.h>
#include <scbuild.h>
#include "winman.h"
#include "vmachine.h"

#ifdef WIN32
#include "winspool.h"
#include <shellapi.h>
#endif

BOOL ScDeleteDir(char*dir)
{
    {
        TFileIterator fcls(dir,"*.*");
        while (fcls.GetNext())
        {
            char s[MAX_PATH];
            fcls.GetFullFileName(s);
            ScDeleteFile(s);
        }
    }
    TFileIterator fcls(dir,"*.*",TRUE);
    while (fcls.GetNext())
    {
        if((*(fcls.GetFileName()))!='.')
        {
            char s[MAX_PATH];
            fcls.GetFullFileName(s);
            RemoveDirectory(s);
        }
    }
    char DirBuf[MAX_PATH];
    GetCurrentDirectory(MAX_PATH,DirBuf);
    if(!lstrcmpi(DirBuf,	dir))
    {
        GetPath(dir,DirBuf);
        SetCurrentDirectory(DirBuf);
        //  dir=DirBuf;
    }
    if(!RemoveDirectory(dir))
        sprintf(DirBuf,"RemoveDirectory(%s) failed: %X",dir,GetLastError());
};

BOOL ScGetKeyState(int key);

#ifndef RUNTIME
TSColor*syntaxcolor=NULL;
char* user_id_names[]={"user_name","user_org","user_email","user_addr","user_phone","user_add"};
#endif

#ifdef WIN32

BOOL scCALLBACK ErrorDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    _STDHELP(0)
            switch(msg){
        case WM_INITDIALOG:
        {
            char s[512];
            SetWindowLong(hwnd,DWL_USER,lParam);
            char**pp=(char**)lParam;
            lstrcpy(s,pp[1]);
            if(EXEC_FLAGS&EF_STEPACTIVE)
                AddObjectName(s);
            SetDlgItemText(hwnd,IDC_ERRORTEXT,pp[0]);
            SetDlgItemText(hwnd,IDC_ERRORDETAIL,s);
        }return TRUE;
        case WM_COMMAND:
        {
            EV_COMMAND
                    switch (command){
        case IDC_DETAILS:
                {
                    HWND hw=GetDlgItem(hwnd,IDC_ERRORDETAIL);
                    RECT rr,r;
                    GetWindowRect(hw,&r);GetWindowRect(hwnd,&rr);
                    int dy=((rr.right-rr.left)-(r.right-r.left))/2;
                    MapWindowPoints(NULL,hwnd,(POINT*)&r,2);
                    SetWindowPos(hwnd,0,0,0,rr.right-rr.left,
                                 r.bottom+dy+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYDLGFRAME),
                                 SWP_NOMOVE|SWP_NOZORDER);
                    EnableWindow(hwndCtl,0);
                }break;
                case IDCANCEL:
                case IDOK:
                    EndDialog(hwnd,IsDlgButtonChecked(hwnd,IDC_DONTSHOWERROR));
                    return TRUE;
            }return TRUE;
        }return TRUE;
    }
    return FALSE;
}

BOOL ErrorDialog(HWND hwnd,char*p1,char*p2)
{
    char *pp[2]={p1,p2};
    return DialogBoxParam(HResource,MAKEINTRESOURCE(ERROR_DIALOG),hwnd,ErrorDialogProc,(LPARAM)&pp);
};

int scCALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData ){
    switch(uMsg){
        case BFFM_INITIALIZED:{
            SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
        }break;
    }
    return 0;
};
struct _CHDIR{
    UINT32 flags;
    char *path;
    char *title;
};

BOOL scCALLBACK ChoseDirDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            switch(msg){
        case WM_INITDIALOG:{
            _CHDIR *ch=(_CHDIR*)lParam;
            SetDlgItemText(hwnd,IDC_CHOSEDIR,ch->path);
            SetDlgItemText(hwnd,IDC_DIRPROMPT,ch->title);
            SetWindowLong(hwnd,DWL_USER,lParam);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch (command){
        case IDCANCEL:EndDialog(hwnd,0);return TRUE;
                case IDOK:{
                    _CHDIR *ch=(_CHDIR*)GetWindowLong(hwnd,DWL_USER);
                    GetDlgItemText(hwnd,IDC_CHOSEDIR,ch->path,255);
                }    EndDialog(hwnd,1);return TRUE;
                case IDC_BROWSE:{
                    char s[256];
                    GetDlgItemText(hwnd,IDC_CHOSEDIR,s,sizeof(s));
                    _CHDIR *ch=(_CHDIR*)GetWindowLong(hwnd,DWL_USER);
                    if(ChoseDirDialog(hwnd,s,ch->title,0)){
                        SetDlgItemText(hwnd,IDC_CHOSEDIR,s);
                    }
                }return TRUE;
            }
        }return TRUE;
    }
    return FALSE;
}

BOOL ChoseDirDialog(HWND hwnd,char *dir,char *title,UINT32 flags)
{
    char _title[256];
    if(!HIWORD(title)){
        LoadString(HResource,LOWORD(title),_title,sizeof(_title));
        title=_title;
    }
    if(flags&CHDIR_FORSAVE){
        _CHDIR ch;
        ch.flags=flags;
        ch.path=dir;
        ch.title=title;
        return DialogBoxParam(HResource,MAKEINTRESOURCE(CHOSE_DIR),hwnd,ChoseDirDialogProc,(LPARAM)&ch);
    }

    BROWSEINFO bi;
    memset(&bi,0,sizeof(bi));
    bi.hwndOwner=hwnd;
    bi.pszDisplayName=dir;
    bi.lParam=(LPARAM)dir;
    // bi.ulFlags=BIF_STATUSTEXT;
    bi.lpszTitle=title;
    bi.lpfn=BrowseCallbackProc;
    LPCITEMIDLIST id=SHBrowseForFolder(&bi);
    if(id){
        return SHGetPathFromIDList(id,dir);
    }
    return FALSE;
}
#else
BOOL ChoseDirDialog(HWND hwnd,char *dir,char *title,UINT32 flags)
{
    return InputBox(hwnd,"Chose Directory",title,dir);
}
#endif
BOOL ListBoxProc(HWND hwnd,UINT msg,WPARAM&wParam,LPARAM&lParam,int _id);

BOOL scCALLBACK IsSaveChangesDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            switch(msg){
        case WM_INITDIALOG:{
            SetDlgItemText(hwnd,IDC_SAVEITEMS,(char*)lParam);
            CheckDlgButton(hwnd,IDC_ALWAYSCHECK,1);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch (command){
        case IDNO:
                case IDYES:
                case IDCANCEL:
                case IDRETRY:EndDialog(hwnd,command);return TRUE;
            }}
    }
    return FALSE;
}
int  IsNeedSaveChanges(HWND hwnd,char*s){
    return DialogBoxParam(HResource,MAKEINTRESOURCE(SAVE_CHANGES),hwnd,IsSaveChangesDialogProc,(LPARAM)s);

};
void ClsLstProc(HWND hcb,HWND  list){
    BOOL f=SendMessage(hcb,BM_GETCHECK,0,0)==1;
    SendMessage(hcb,BM_SETCHECK,f,0);
    int count=(int)SendMessage(list,LB_GETCOUNT,0,0);
    char s[256];
    for(int i=0;i<count;i++){
        SendMessage(list,LB_GETTEXT,i,(LPARAM)s);
        if((s[0]=='c')!=f){
            s[0]=f?'c':'u';
            SendMessage(list,LB_DELETESTRING,i,0);
            SendMessage(list,LB_INSERTSTRING,i,(LPARAM)s);
        }}
};
void ClsLstProc2(HWND hwnd,int id){
    HWND list=GetDlgItem(hwnd,id);
    BYTE f=0;
    int count=(int)SendMessage(list,LB_GETCOUNT,0,0);
    char s[256];
    for(int i=0;i<count;i++){
        SendMessage(list,LB_GETTEXT,i,(LPARAM)s);
        if(s[0]=='c')f|=1;
        if(s[0]=='u')f|=2;
    }
    int _id=(id==IDC_LIBCLASSES)?IDC_LIBCLASSES_C:IDC_PRJCLASSES_C;
    int _f=0;
    switch(f){
        case 1:_f=1;break;
        case 3:_f=3;break;
    }
    CheckDlgButton(hwnd,_id,_f);
};

BOOL scCALLBACK SaveChangesDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(Save_changes_dialog)
            int r=0;
    r=ListBoxProc(hwnd,msg,wParam,lParam,IDC_LIBCLASSES);
    if(r){if(r==2)ClsLstProc2(hwnd,IDC_LIBCLASSES);return TRUE;}
    r=ListBoxProc(hwnd,msg,wParam,lParam,IDC_PRJCLASSES);
    if(r){if(r==2)ClsLstProc2(hwnd,IDC_PRJCLASSES);return TRUE;}

    switch(msg){
        case WM_INITDIALOG:{
            SetWindowLong(hwnd,DWL_USER,lParam);
            SaveAllModifyClasses();
            TProject*prj=(TProject*)GetCurrentProject();
            if(prj && prj->IsModifyed())CheckDlgButton(hwnd,IDC_PRJFILE,1);
            else EnableWindow(GetDlgItem(hwnd,IDC_PRJFILE),0);
            if(prj && prj->IsVarModifyed())CheckDlgButton(hwnd,IDC_PRJSTATE,1);
            else EnableWindow(GetDlgItem(hwnd,IDC_PRJSTATE),0);
            BOOL b1=FALSE,b2=FALSE;
            TSortedCollection*classes= GetClasses();
            for(C_TYPE i=0;i<classes->count;i++){
                PClassListInfo pcli=(PClassListInfo)classes->At(i);
                if (pcli->flags&CLF_LOADED){
                    if (pcli->_class->t_flags&TCF_WASSAVE && !pcli->_class->Protected()){
                        char s[256];
                        wsprintf(s,"c%s",pcli->GetClassName());
                        UINT id=IDC_LIBCLASSES;
                        if(pcli->flags&CLF_PROJECT){
                            b2=TRUE;
                            id=IDC_PRJCLASSES;
                        }else {
                            b1=TRUE;
                        }
                        SendDlgItemMessage(hwnd,id,LB_ADDSTRING,0,(LPARAM)s);
                    }
                }}
            if(b1)CheckDlgButton(hwnd,IDC_LIBCLASSES_C,1);else EnableWindow(GetDlgItem(hwnd,IDC_LIBCLASSES_C),0);
            if(b2)CheckDlgButton(hwnd,IDC_PRJCLASSES_C,1);else EnableWindow(GetDlgItem(hwnd,IDC_PRJCLASSES_C),0);
        } return TRUE;
        case WM_COMMAND:{
            EV_COMMAND
                    switch (command){
        case IDC_LIBCLASSES_C:
                case IDC_PRJCLASSES_C:if(wNotifyCode==BN_CLICKED){
                        ClsLstProc(hwndCtl,GetDlgItem(hwnd,command==IDC_LIBCLASSES_C?IDC_LIBCLASSES:IDC_PRJCLASSES));
                    }break;
                case IDOK:{
                    TStringCollection* items=(TStringCollection*)GetWindowLong(hwnd,DWL_USER);
                    char s[256];
                    HWND list=GetDlgItem(hwnd,IDC_LIBCLASSES);
                    int count=(int)SendMessage(list,LB_GETCOUNT,0,0);
                    for(int i=0;i<count;i++){
                        SendMessage(list,LB_GETTEXT,i,(LPARAM)s);
                        if(s[0]=='c'){ items->Insert(NewStr(s+1)); }
                    }
                    list=GetDlgItem(hwnd,IDC_PRJCLASSES);
                    count=(int)SendMessage(list,LB_GETCOUNT,0,0);
                    for(i=0;i<count;i++){
                        SendMessage(list,LB_GETTEXT,i,(LPARAM)s);
                        if(s[0]=='c'){ items->Insert(NewStr(s+1)); }
                    }
                    if(IsDlgButtonChecked(hwnd,IDC_PRJFILE)){items->Insert(NewStr("$1"));}
                    if(IsDlgButtonChecked(hwnd,IDC_PRJSTATE)){items->Insert(NewStr("$2"));}
                }EndDialog(hwnd,TRUE);return TRUE;

                case IDCANCEL:EndDialog(hwnd,FALSE);return TRUE;
            }

        }break;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

TStringCollection*SaveChangesDialog(HWND hwnd){
    TStringCollection * items=new TStringCollection(10,10);
    BOOL rez=DialogBoxParam(HResource,MAKEINTRESOURCE(SAVE_CLASS_DLG),hwnd,SaveChangesDialogProc,(LPARAM)items);
    if(!rez){delete items;items=NULL;}
    return items;
}



void Decode(char *s)
{
    char *_ps=s;
    char _s[256];
    char *ps=_s;
    while(*s){
        *ps=(*s)^31;
        s++;ps++;
    }
    *ps=0;
    lstrcpy(_ps,_s);
}

void Encode(char *s)
{
    Decode(s);
}

void AddLayer(HWND combo,int start,int end,int current)
{
    if (current<start)
        current=start;

    if (current>=end)
        current=start;

    for(int i=start;i<end;i++)
    {
        char s[10];
        wsprintf(s,"%d",i);
        SendMessage(combo,CB_ADDSTRING,0,(LPARAM)s);
    }
    SendMessage(combo,CB_SETCURSEL,current-start,0);

};

BOOL StdHelpProc(UINT msg,WPARAM wParam,LPARAM lParam,int ctx){
    switch(msg){
        case WM_COMMAND:{
            EV_COMMANDsimply
                    if(command==IDHELP){HELP(ctx);return TRUE;}
        }break;
#ifdef WIN32
        case WM_HELP:{
            HELPINFO * lphi = (LPHELPINFO) lParam;
            HELP(lphi->iCtrlId,(int)lphi->MousePos.x,(int)lphi->MousePos.y,(HWND)lphi->hItemHandle);
        }return TRUE;
#endif
    }
    return FALSE;
};


BOOL scCALLBACK ControlBarsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
//***********************************************************************//
//                                                                       //
//                Далее для PropertySheet() Закладки                     //
//                                                                       //
//***********************************************************************//

struct
{
    TPropertyButton*buttons;
    char* title;
    int active;
}PRPSTRUCT;

TPropertyButton*_lastactive=NULL;

BOOL scCALLBACK DefPropertyItemProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg){
        case WM_KEYDOWN:
        {
            if ((wParam==VK_ESCAPE)||(wParam==VK_RETURN))
            {
                SendMessage(GetParent(hwnd),msg,wParam,lParam);
                return TRUE;
            }
        }break;
#ifdef WIN32
        case WM_HELP:
        {
            HELPINFO * lphi = (LPHELPINFO) lParam;
            HELP(lphi->iCtrlId,(int)lphi->MousePos.x,(int)lphi->MousePos.y,(HWND)lphi->hItemHandle);
        }return TRUE;
#endif
        case PSP_VALID:return TRUE;
        case WM_INITDIALOG:return TRUE;
        case WM_COMMAND:
            EV_COMMANDsimply
                    switch (command){
        case IDOK:
                case IDCANCEL:
                {
                    HWND parent=GetParent(hwnd);
#ifdef WIN32
                    return SendMessage(parent,WM_COMMAND, MAKEWPARAM(command,0),0);
#else
                    return (int)SendMessage(parent,WM_COMMAND,command,MAKELONG(0,0));
#endif
                }
            }
    }
    return FALSE;
};

void MoveButton(HWND hwnd,int id,int dx,int dy)
{
    HWND b=GetDlgItem(hwnd,id);
    RECT r;
    GetWindowRect(b,&r);
    ScreenToClient(hwnd,(POINT*)&r);
    SetWindowPos(b,0,r.left+dx,r.top+dy,0,0,SWP_NOZORDER|SWP_NOSIZE);
}

TPropertyButton*GetActivePropertyButton(HWND hwnd)
{
    return (TPropertyButton*)SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_GETACTIVEPAGE,0,0);
}

TPropertyButton*GetDlgPropertyButton(HWND hwnd)
{
    HWND parent=GetParent(hwnd);
    TPropertyButton*b=(TPropertyButton*)SendDlgItemMessage(parent,ID_SCSYSTAB,PST_GETALLPAGES,0,0);
    while (b)
    {
        if (b->hwnd==hwnd)
            return b;
        b=b->next;
    }
    return NULL;
};

BOOL  ChosePage(HWND hwnd,TPropertyButton*active,TPropertyButton*b){
    TPropertyButton*_b=b;
    while (_b->prev)_b=_b->prev;
    _b->activehwnd=b->hwnd;
    active->flags|=PSP_DATAVALID;
    SendMessage(active->hwnd,PSP_VALID,0,(LPARAM)active);
    if (active->flags&PSP_DATAVALID)	{
        SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_SETACTIVEPAGE,0,(LPARAM)b);
        ShowWindow(active->hwnd,SW_HIDE);
        if(!(b->flags&PSP_INITIALIZE)){
            SendMessage(b->hwnd,PSP_INITPAGE,0,(LPARAM)b);
            b->flags|=PSP_INITIALIZE;}
        SendMessage(b->hwnd,PSP_ACTIVATE,0,(LPARAM)b);
        ShowWindow(b->hwnd,SW_SHOW);
        SetWindowPos(b->hwnd,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
        return TRUE;
    }
    return FALSE;
}

BOOL scCALLBACK PropertySheetProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg){
        //	case WM_SIZE:{	}return TRUE;
        case WM_SETTOOLVAR:
        case WM_GETTOOLVAR:
        {
            TPropertyButton*bf=(TPropertyButton*)
                               SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_GETALLPAGES,0,0);
            while(bf)
            {
                SendMessage(bf->hwnd,msg,wParam,lParam);
                bf=bf->next;
            }
        }return TRUE;
#ifdef WIN32
        case WM_HELP:
        {
            HELPINFO * lphi = (LPHELPINFO) lParam;
            HELP(lphi->iCtrlId,(int)lphi->MousePos.x,(int)lphi->MousePos.y,(HWND)lphi->hItemHandle);
        }return TRUE;
#endif
        case PSN_PAGESELECT:
        {
            TPropertyButton*active=GetActivePropertyButton(hwnd);
            TPropertyButton*b=(TPropertyButton*)lParam;
            return ChosePage(hwnd,active,b);
        }
        case WM_INITDIALOG:  //формируем содержимое окна свойств объекта
        {
            SetWindowText(hwnd,PRPSTRUCT.title);
            HWND prw=GetDlgItem(hwnd,ID_SCSYSTAB);
            TPropertySheet*pr=(TPropertySheet*)GetWindowLong(prw,0);
            RECT r;
            GetWindowRect(prw,&r);
            ScreenToClient(hwnd,(POINT*)&r);
            ScreenToClient(hwnd,((POINT*)&r)+1);
            RECT dlgrect;
            if(pr)
            {
                TPropertyButton*factive=PRPSTRUCT.buttons;
                TPropertyButton*b=PRPSTRUCT.buttons;
                if(PRPSTRUCT.active>-1)
                {
                    int i=0;
                    while(b)
                    {
                        if (i==PRPSTRUCT.active)
                        {
                            factive=b;
                            break;
                        }
                        i++;
                        b=b->next;
                    }
                }
                b=PRPSTRUCT.buttons;
                while (b)
                {
                    if (!b->Procedure)
                        b->Procedure=DefPropertyItemProc;
                    if((b->hInstance)==(HINSTANCE)1)
                    {
                        PObject po=(PObject)b->pdata;
                        PObject _po=po?po->GetReceiver("DialogHandler"):NULL;
                        b->hsp  = winmanager->_CreateDialog(hwnd,NULL,b->templ,TRUE,_po);
                        b->hwnd = GetSpaceWindow2d(b->hsp);
                        // !!! послать сообщение о открытии
                    }
                    else
                        b->hwnd=CreateDialogParam(b->hInstance,b->templ,hwnd,b->Procedure,(LPARAM)b);
                    if(!b->hwnd)
                    {
                        b->Procedure=DefPropertyItemProc;
                        b->hwnd=CreateDialogParam(HResource,"Empty_Dialog",hwnd,b->Procedure,(LPARAM)b);
#ifdef WIN32
                        DWORD d=GetLastError();
                        char s[128];
                        wsprintf(s,"Error in creating dialog. Error code:%lu",d);
                        SetDlgItemText(b->hwnd,IDC_ERRORTEXT,s);
#endif
                    }
                    RECT rr;
                    GetWindowRect(b->hwnd,&rr);
                    rr.right-=rr.left; rr.left = 0;
                    rr.bottom-=rr.top; rr.top  = 0;
                    if (b==factive)
                    {
                        SendMessage(b->hwnd,PSP_INITPAGE,0,(LPARAM)b);
                        SendMessage(b->hwnd,PSP_ACTIVATE,0,(LPARAM)b);
                        b->flags|=PSP_INITIALIZE;
                    }
                    else
                        ShowWindow(b->hwnd,SW_HIDE);
                    if (b==PRPSTRUCT.buttons)
                        dlgrect=rr;
                    else
                    {
                        if(dlgrect.right<rr.right)
                            dlgrect.right=rr.right;
                        if(dlgrect.bottom<rr.bottom)
                            dlgrect.bottom=rr.bottom;
                    }
                    b=b->next;
                }
                int newx=dlgrect.right+5;
                pr->SetButtons(PRPSTRUCT.buttons,newx);
                b=PRPSTRUCT.buttons;
                while (b)
                {
                    SetWindowPos(b->hwnd,HWND_TOP,r.left+3,r.top+pr->height+3,0,0,SWP_NOSIZE);
                    b=b->next;
                }
                BOOL OKbuttons=(GetDlgItem(hwnd,IDOK)!=NULL)|
                               (GetDlgItem(hwnd,IDCANCEL)!=NULL)|
                               (GetDlgItem(hwnd,IDHELP)!=NULL);
                int newy=dlgrect.bottom+pr->height+6;
                SetWindowPos(prw,NULL,0,0,newx,newy,SWP_NOZORDER|SWP_NOMOVE);
                RECT thisrect;
                GetClientRect(hwnd,&thisrect);
                int dx=(newx-(r.right-r.left));
                int dy=(newy-(r.bottom-r.top));
                HWND hdesktop=GetDesktopWindow();
                RECT dskrect,dlgrect;
                GetClientRect(hdesktop,&dskrect);
                GetWindowRect(hwnd,&dlgrect);
                int _dy;
                if (OKbuttons)
                    _dy=16;
                else
                    _dy=0;
                int x=(dskrect.right-(thisrect.right+dx))/2,y=(dskrect.bottom-(thisrect.bottom+dy+_dy))/2;
                SetWindowPos(hwnd,NULL,x,y,thisrect.right+dx,thisrect.bottom+dy+_dy,SWP_NOZORDER);//??
                MoveButton(hwnd,IDOK,dx,dy);
                MoveButton(hwnd,IDCANCEL,dx,dy);
                MoveButton(hwnd,IDHELP,dx,dy);
                PRPSTRUCT.buttons->activehwnd=factive->hwnd;
                SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_SETACTIVEPAGE,0,(LPARAM)factive);
            }
        }return TRUE;
        case WM_COMMAND:
        {
            TPropertyButton*bf=(TPropertyButton*)
                               SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_GETALLPAGES,0,0);
            if(bf)
            {
                EV_COMMAND
                        switch (command){
        case IDHELP:
                    {
                        TPropertyButton*active=GetActivePropertyButton(hwnd);
                        HELP(active->helpctx);
                    }return 0;
                    case IDC_COMBOPAGES:
                        if(wNotifyCode==CBN_SELCHANGE)
                        {
                            TPropertyButton*active=GetActivePropertyButton(hwnd);
                            TPropertyButton*_b=active;
                            while (_b->prev)
                                _b=_b->prev;
                            int i=SendMessage(hwndCtl,CB_GETCURSEL,0,0);
                            for(int j=0;j<i;j++)_b=_b?_b->next:NULL;
                            if(_b && active)return ChosePage(hwnd,active,_b);
                        }
                        break;
                    case IDOK:
                    {
                        TPropertyButton*b=bf;
                        while(b)
                        {
                            SendMessage(b->hwnd,PSP_VALID,1,(LPARAM)b);
                            if((b->flags&PSP_DATAVALID)==0)
                                return TRUE;
                            b=b->next;
                        }
                        b=bf;
                        while(b)
                        {
                            SendMessage(b->hwnd,PSP_CLOSE,(b->flags&PSP_INITIALIZE)?1:2,(LPARAM)b);
                            b=b->next;
                        }
                        bf->result=1;
                        _lastactive=(TPropertyButton*)SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_GETACTIVEPAGE,0,0);
                        EndDialog(hwnd,1);
                        return TRUE;
                    }
                    case IDCANCEL:
                    {
                        bf->result=0;
                        while(bf)
                        {
                            SendMessage(bf->hwnd,PSP_CLOSE,0,(LPARAM)bf);
                            bf=bf->next;
                        }
                        _lastactive=(TPropertyButton*)SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_GETACTIVEPAGE,0,0);
                        EndDialog(hwnd,0);return TRUE;
                    };
                    default:
                        SendMessage(bf->hwnd,msg,wParam,lParam);
                }
            }
        }
        case WM_KEYDOWN:
        {
            if(wParam==VK_ESCAPE)
            {
#ifdef WIN32
                PostMessage(hwnd,WM_COMMAND, MAKEWPARAM(IDCANCEL,0),0);
#else
                PostMessage(hwnd,WM_COMMAND,IDCANCEL,MAKELONG(0,0));
#endif
                return  0;
            }
        }break;
    }
    return FALSE;
};


BOOL IsAllPagesValid(HWND hwnd){
    TPropertyButton*bf=(TPropertyButton*)SendDlgItemMessage(hwnd,ID_SCSYSTAB,PST_GETALLPAGES,0,0);
    TPropertyButton*b=bf;
    while(b){
        SendMessage(b->hwnd,PSP_VALID,1,(LPARAM)b);
        if ((b->flags&PSP_DATAVALID)==0) return FALSE;
        b=b->next;
    }
    return TRUE;
};
HWND GetDisableWindow(HWND ){
    HWND popup=GetLastActivePopup(mainwindow->HWindow);
    if (popup)return popup;
    return mainwindow->HWindow;

    /*
int i=0;
 HWND prev;

while(hwnd){
if(hwnd==mainwindow->HWindow)break;
  prev=hwnd;
  hwnd=GetParent(hwnd);
  char s[32]="";
  GetClassName(hwnd,s,sizeof(s));
  BOOL dlg=!lstrcmpi(s,"Dialog");
  if (dlg)i++;
  if (!dlg && i)return prev;
 return hwnd;
 }*/

};

POINT colororg;
BOOL scCALLBACK ModelDlgProc(HWND,UINT,WPARAM,LPARAM){
    return FALSE;
};
BOOL scCALLBACK ColorSysProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{
            EV_COMMAND
                    if(command==IDC_SYSCOLORS && wNotifyCode==LBN_SELCHANGE){
                TPropertyButton*b=GetDlgPropertyButton(hwnd);
                if(b){
                    long i=SendDlgItemMessage(hwnd,IDC_SYSCOLORS,LB_GETCURSEL,0,0);
                    COLORREF color=((long)i)|0x2000000L;
                    if(b->prev)b=b->prev;
                    b->data=color;
                    return  _SendCommand(GetParent(hwnd),IDOK);
                }
            }
        }break;
        case WM_MEASUREITEM:{
            int nIDCtl = (int) wParam;
            MEASUREITEMSTRUCT FAR* lm = (MEASUREITEMSTRUCT FAR*) lParam;
            HDC dc=GetDC(0);
            SelectObject(dc,GetStockObject(ANSI_VAR_FONT));

            TEXTMETRIC tm;
            GetTextMetrics(dc,&tm);
            lm->itemHeight = tm.tmHeight + tm.tmExternalLeading+1;
            ReleaseDC(0,dc);
            RECT r;
            GetClientRect(GetDlgItem(hwnd,lm->CtlID),&r);
            lm->itemWidth=r.right-4;
            return TRUE;
        };
        case WM_DRAWITEM:{
            DRAWITEMSTRUCT FAR* pdis = (DRAWITEMSTRUCT FAR*) lParam;
            char s[256]="";
            if(SendMessage(pdis->hwndItem,LB_GETTEXT,pdis->itemID,(LPARAM)s)!=LB_ERR)
            {

                COLORREF rgb=GetSysColor(((pdis->itemState&ODS_SELECTED)!=0)?COLOR_HIGHLIGHT:COLOR_WINDOW);
                //  COLORREF rgb=((pdis->itemState&ODS_SELECTED)!=0)?RGB(128,0,128):RGB(255,255,255);
                HGDIOBJ b=CreateSolidBrush(rgb);
                HGDIOBJ old=SelectObject(pdis->hDC,b);
                PatBlt(pdis->hDC,pdis->rcItem.left,pdis->rcItem.top,pdis->rcItem.right-pdis->rcItem.left,
                       pdis->rcItem.bottom-pdis->rcItem.top,PATCOPY);

                HGDIOBJ b1=CreateSolidBrush(GetSysColor(pdis->itemID));
                SelectObject(pdis->hDC,b1);
                SelectObject(pdis->hDC,GetStockObject(BLACK_PEN));
                int h=(pdis->rcItem.bottom-pdis->rcItem.top)-2;

                Rectangle(pdis->hDC,pdis->rcItem.left+1, pdis->rcItem.top+1,
                          pdis->rcItem.left+h,pdis->rcItem.top+h);

                SelectObject(pdis->hDC,old);
                DeleteObject(b);
                DeleteObject(b1);

                SetBkMode(pdis->hDC,TRANSPARENT);
                COLORREF oldc=SetTextColor(pdis->hDC,GetSysColor((pdis->itemState&ODS_SELECTED)?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT));
                TextOut(pdis->hDC,pdis->rcItem.left+h+2,pdis->rcItem.top,s,lstrlen(s));

                SetTextColor(pdis->hDC,oldc);
                if(pdis->itemState&ODS_FOCUS){
                    RECT r=pdis->rcItem;
                    r.top++;r.bottom--;r.right--;
                    r.left+=h+2;
                    DrawFocusRect(pdis->hDC,&r);
                }
            }
            return TRUE;
        };
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            char s[256];
            int i=IDS_SYSCOLORS;
            HWND list=GetDlgItem(hwnd,IDC_SYSCOLORS);
            while(LoadString(HResource,i,s,sizeof(s))){
                SendMessage(list,LB_ADDSTRING,0,(LONG)s);
                i++;
            };
            if(b->data&0x2000000l){
                SendMessage(list,LB_SETCURSEL,(int)(b->data&0xfful),0);
            };
        }return TRUE;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
struct TPalEntry{
    BYTE r,g,b;
};
TPalEntry palette_entrys[48]={
    {255,255,255},{255,191,191},{255,223,191},{255,255,191},{191,255,191},{191,255,255},{191,191,255},{255,191,255},
    {223,223,223},{255,127,127},{255,191,127},{255,255,127},{127,255,127},{127,255,255},{127,127,255},{255,127,255},
    {191,191,191},{255,0,0},{255,127,0},{255,255,0},{0,255,0},{0,255,255},{0,0,255},{255,0,255},
    {127,127,127},{191,0,0},{191,63,0},{191,191,0},{0,191,0},{0,191,191},{0,0,191},{191,0,191},
    {63,63,63},{127,0,0},{127,63,0},{127,127,0},{0,127,0},{0,127,127},{0,0,127},{127,0,127},
    {0,0,0},{63,0,0},{127,63,63},{63,63,0},{0,63,0},{0,63,63},{0,0,63},{63,0,63}};
extern COLORREF aclrCust[];

class TPalettePropertyButton:public TPropertyButton{
public:
    COLORREF *cust_color; // если установленн то требуется запросить цвет
    BYTE state;
    int xx,yy;
    TPalettePropertyButton():TPropertyButton(IDS_str216,HResource,"Get_Color2"){
        state=0;
        xx=yy=-1;
        cust_color=NULL;
    };
    void SetNumbers(COLORREF color){
        SetDlgItemInt(hwnd,IDC_COLORR,GetRValue(color),0);
        SetDlgItemInt(hwnd,IDC_COLORG,GetGValue(color),0);
        SetDlgItemInt(hwnd,IDC_COLORB,GetBValue(color),0);
    };
    COLORREF*  _GetColorButton(LPARAM lParam,COLORREF&color,BOOL*ext);
    void Invalidate();
    void Paint(HDC dc);
};
void TPalettePropertyButton::Invalidate(){
    HDC dc=GetDC(hwnd);
    HDC hdc=CreateCompatibleDC(dc);
    RECT r;
    GetClientRect(hwnd,&r);
    int size=r.right/9;
    int sx=r.right;
    int sy=size*9;
    HBITMAP b=CreateCompatibleBitmap(dc,sx,sy);
    SelectObject(hdc,b);
    Paint(hdc);
    BitBlt(dc,0,0,sx,sy,hdc,0,0,SRCCOPY);
    DeleteDC(hdc);
    DeleteObject(b);
    ReleaseDC(hwnd,dc);
};
void TPalettePropertyButton::Paint(HDC hdc){
    COLORREF color;
    BOOL drawrect=1;
    color=data;
    SelectObject(hdc,Bface);
    RECT r;
    GetClientRect(hwnd,&r);
    PatBlt(hdc,0,0,r.right,r.bottom,PATCOPY);

    int size=r.right/9;
    int sx=(r.right-size*8)/2;
    int x;
    for(int y=0;y<8;y++)
        for(x=0;x<8;x++){

            int _x=x*size+sx;
            int _y=y*size+4;

            COLORREF _rgb=0;
            if(y<6){
                int i=x+y*8;
                _rgb=RGB(palette_entrys[i].r,palette_entrys[i].g,palette_entrys[i].b);
            }else{
                _rgb=aclrCust[(y-6)*8+x];
            }

            HGDIOBJ b=CreateSolidBrush(_rgb);
            HGDIOBJ old=SelectObject(hdc,b);
            if(((xx==-1)&&(color==_rgb && drawrect))||
               (xx==x && yy==y)){
                SelectObject(hdc,GetStockObject(BLACK_PEN));
                Rectangle(hdc, _x-1, _y-1,(_x-1)+ size+1,(_y-1)+size+1);
                drawrect=0;
            }
            PatBlt(hdc, _x+1, _y+1, size-3, size-3,PATCOPY);
            SelectObject(hdc, old);
            DeleteObject(b);
            DrawShadowFrame(hdc,_x,_y,size-1,size-1,5);
        }
    {
        COLORREF rgb=data;
        if(rgb &0x02000000L)rgb=GetSysColor(rgb&0xff);

        HGDIOBJ b=CreateSolidBrush(rgb);
        HGDIOBJ old=SelectObject(hdc,b);
        int _x=x*size+sx;
        int _y=8*size+6;
        int sizey=size/2;
        PatBlt(hdc, sx,_y,size*8,sizey,PATCOPY);
        DrawShadowFrame(hdc,sx,_y,size*8,sizey,5);
        SelectObject(hdc, old);
        DeleteObject(b);
    }

};

COLORREF* TPalettePropertyButton::_GetColorButton(LPARAM lParam,COLORREF&color,BOOL*ext){
    int xp= (INT16)LOWORD(lParam);
    int yp=(INT16)HIWORD(lParam);
    RECT r;
    GetClientRect(hwnd,&r);
    int size=r.right/9;
    int sx=(r.right-size*8)/2;
    if(xp>sx && xp<8*size+sx && yp>4 && yp<8*size+4){
        int x=(xp-sx)/size;
        int y=(yp-4)/size;
        xx=x;yy=y;
        if(y<6){
            int i=x+y*8;
            color=RGB(palette_entrys[i].r,palette_entrys[i].g,palette_entrys[i].b);
            if(ext)*ext=FALSE;
            return &color;
        }else{
            if(ext)*ext=TRUE;
            return aclrCust+((y-6)*8+x);
        }
    }

    return NULL;
};


BOOL scCALLBACK ColorPaletteProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){

    switch(msg){
        case WM_COMMAND:{
            TPalettePropertyButton*b=(TPalettePropertyButton*)GetDlgPropertyButton(hwnd);
            EV_COMMAND
                    if(b){
                switch(command){
                    case IDC_COLORR:
                    case IDC_COLORG:
                    case IDC_COLORB:if((wNotifyCode==EN_CHANGE)&& (GetFocus()==hwndCtl)){
                            INT16 R,G,B;
                            if(IsIntValid(hwnd,IDC_COLORR,R,0,255,FALSE)&&
                               IsIntValid(hwnd,IDC_COLORG,G,0,255,FALSE)&&
                               IsIntValid(hwnd,IDC_COLORB,B,0,255,FALSE)){
                                b->data=RGB(R,G,B);
                                b->xx=b->yy=-1;
                                b->Invalidate();
                            }
                        }break;
                    case IDC_TRANSPARENT:if(wNotifyCode==BN_CLICKED ){
                            if(IsDlgButtonChecked(hwnd,IDC_TRANSPARENT))
                            {b->data|=0x01000000L;
                                _SendCommand(GetParent(hwnd),IDOK);
                            }
                        }break;;
                }}
        }break;
        case WM_RBUTTONDOWN:{
            TPalettePropertyButton*b=(TPalettePropertyButton*)GetDlgPropertyButton(hwnd);
            COLORREF color;
            BOOL ext;
            COLORREF* clr= b->_GetColorButton(lParam,color,&ext);
            int id=IDCANCEL;
            if(clr && ext){
                b->cust_color=clr;
                id=IDOK;
            }
            _SendCommand(GetParent(hwnd),id);
        }return TRUE;
        case WM_MOUSEMOVE:{
            TPalettePropertyButton*b=(TPalettePropertyButton*)GetDlgPropertyButton(hwnd);
            if(b->state==1){
                COLORREF color;
                int x=b->xx;int y=b->yy;
                COLORREF* clr= b->_GetColorButton(lParam,color,NULL);
                if(clr && (x!=b->xx || y!=b->yy)){
                    b->data=*clr;
                    b->Invalidate();
                    b->SetNumbers(*clr);
                }
            }}return TRUE;
        case WM_LBUTTONDOWN:{
            TPalettePropertyButton*b=(TPalettePropertyButton*)GetDlgPropertyButton(hwnd);
            COLORREF color;
            COLORREF* clr =b->_GetColorButton(lParam,color,NULL);
            if(clr){
                b->state=1;
                b->data=*clr;
                b->Invalidate();
                b->SetNumbers(*clr);
                SetCapture(hwnd);
                CheckDlgButton(hwnd,IDC_TRANSPARENT,0);
            }else{
                _SendCommand(GetParent(hwnd),IDCANCEL);
            }
        }return TRUE;
        case WM_LBUTTONUP:{
            TPalettePropertyButton*b=(TPalettePropertyButton*)GetDlgPropertyButton(hwnd);
            if(b->state==1){
                ReleaseCapture();
                b->state=0;
                COLORREF color;
                COLORREF* clr =b->_GetColorButton(lParam,color,NULL);
                int id=IDCANCEL;
                if(clr){
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    b->data=*clr;
                    id=IDOK;
                };
                _SendCommand(GetParent(hwnd),id);

            }}return TRUE;
        case WM_KEYDOWN:{
            if (wParam==VK_ESCAPE){
                TPropertyButton*b=GetDlgPropertyButton(hwnd);
                HWND w=(HWND)b->user;
                SetFocus(w);
            }
        };return FALSE;
        case PSP_INITPAGE:{
            TPalettePropertyButton*b=(TPalettePropertyButton*)lParam;
            EditSubClass(hwnd,IDC_COLORR,0,255,1,UDF_MINMAX);
            EditSubClass(hwnd,IDC_COLORG,0,255,1,UDF_MINMAX);
            EditSubClass(hwnd,IDC_COLORB,0,255,1,UDF_MINMAX);
            b->SetNumbers(b->data);
            if(b->dwuser1)CheckDlgButton(hwnd,IDC_TRANSPARENT,(b->data&0x01000000ul)!=0);else
                ShowWindow(GetDlgItem(hwnd,IDC_TRANSPARENT),SW_HIDE);
        }break;
        case WM_PAINT:{
            TPalettePropertyButton*b=(TPalettePropertyButton*)GetDlgPropertyButton(hwnd);
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(hwnd,&ps);
            b->Paint(hdc);
            EndPaint(hwnd,&ps);
        }break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK ColorSheetProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    BOOL r = PropertySheetProc(hwnd,msg,wParam,lParam);
    switch(msg){
        case WM_ACTIVATE:{
            if(!wParam){
#ifdef WIN32
                return PostMessage(hwnd,WM_COMMAND, MAKEWPARAM((UINT)(IDOK),(UINT)(0)),0);
#else
                return PostMessage(hwnd,WM_COMMAND,IDOK,0);
#endif
            }
            break;
        }return TRUE;
        case WM_INITDIALOG:{
            RECT r,dr;
            GetWindowRect(hwnd,&r);
#ifdef WIN32
            SystemParametersInfo(SPI_GETWORKAREA,0,&dr,0);
#else
            GetWindowRect(GetDesktopWindow(),&dr);
#endif
            int x=colororg.x,y=colororg.y;
            int w=(r.right-r.left);
            int h=(r.bottom-r.top);
            if((x+w)>dr.right)x=dr.right-w;
            if((y+h)>dr.bottom)y=dr.bottom-h;
            SetWindowPos(hwnd,0,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
        }break;
    }
    return r;
};

BOOL ChoseColor(HWND parent,POINT&org,COLORREF&color,long data,int flags){
    colororg=org;
    HWND dialog=0;
    TPalettePropertyButton*_b=NULL;
    TPropertyButton *b=NULL,*b1=NULL;
    if(flags&1){  b=_b = new TPalettePropertyButton();
        b->Procedure=ColorPaletteProc;
        b->data=color;
        b->dwuser=data;
        b->dwuser1=flags&4;
    }
    if(flags&2){  b1= new TPropertyButton(IDS_str217,HResource,"Get_Color1",0);
        b1->Procedure=ColorSysProc;
        (HWND)b1->user=parent;
        b1->data=color;
        b1->dwuser=data;
    }

    if(b && b1)b->Insert(b1);
    if(!b)b=b1;
    if(b){
        PRPSTRUCT.title="";
        PRPSTRUCT.buttons=b;
        PRPSTRUCT.active=(color&0x02000000L)?1:0;
        b->result=-1;
        dialog=CreateDialogParam(HResource,"Get_Color",parent,ColorSheetProc,(LPARAM)b);
        MSG msg;
        do{
            GetMessage(&msg,NULL,0,0);
            if(msg.message==WM_KEYDOWN && msg.wParam==VK_RETURN)
            {
                msg.hwnd=dialog;
            }
            if(!IsDialogMessage(b->activehwnd,&msg))
            {
                if(!IsDialogMessage(dialog,&msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }while(b->result==-1);
    }
    if(b->result==1){
        if(_b && _b->cust_color){
            CHOOSECOLOR cc;
            memset(&cc, 0, sizeof(CHOOSECOLOR));
            cc.lStructSize = sizeof(CHOOSECOLOR);
            cc.hwndOwner = parent;
            cc.rgbResult = *_b->cust_color;
            cc.lpCustColors = aclrCust;
            cc.Flags = CC_RGBINIT|CC_FULLOPEN;
            if (ChooseColor(&cc)){
                *_b->cust_color=cc.rgbResult;
                b->data= *_b->cust_color;
            }else dialog=0;
        }
        color=b->data;
    }else dialog=0;
    delete b;
    return dialog!=0;
};


extern "C" HWND _SDKPROC SCCreatePropertySheet(HWND parent,char*title,TPropertyButton*b,int active,HINSTANCE hr,LPCSTR _template)
{
    if(!_template)
        _template="Property";
    if(!hr)
        hr=HResource;
    char _title[256];
    if(!HIWORD(title))
    {
        if(!LoadString(hr,LOWORD(title),_title,sizeof(_title)))
            title="**";
        else
            title=_title;
    }
    PRPSTRUCT.title=title;
    PRPSTRUCT.buttons=b;
    PRPSTRUCT.active=active;
    b->result=-1;
    HWND dialog=CreateDialogParam(hr,_template,parent,PropertySheetProc,(LPARAM)b);
    return dialog;
};

int CreatePropertySheetEx(HWND hwnd,const char*title,TPropertyButton*b,int active,LPCSTR _template)
{
    HWND wndtodisable=GetDisableWindow(hwnd);
    char _title[256];
    if(!HIWORD(title))
    {
        if(!LoadString(HResource,LOWORD(title),_title,sizeof(_title)))
            title="**";
        else
            title=_title;
    }
    PRPSTRUCT.title=(char*)title;
    PRPSTRUCT.buttons=b;
    PRPSTRUCT.active=active;
    b->result=-1;
    HWND dialog=CreateDialogParam(HResource,_template,hwnd,PropertySheetProc,(LPARAM)b);
    if (dialog)
    {
        MSG msg;
        SendMessage(hwnd,WM_ENTERIDLE,MSGF_DIALOGBOX,(LPARAM)dialog);
        BOOL alredydisabled=EnableWindow(wndtodisable,0);
        TPropertyButton*_b=b;
        BOOL model=FALSE;
        while (_b)
        {
            if((_b->hInstance)==(HINSTANCE)1)model=TRUE;
            _b=_b->next;
        }
        do
        {
            if(model && (EXEC_FLAGS&EF_RUNNING))
            {
                if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    // !!! idle action
                    TPropertyButton*b1=GetDlgPropertyButton(b->activehwnd);
                    if(b1)
                    {
                        PObject pobject=(PObject)b1->pdata;
                        if(pobject)
                        {
                            TProject *_project=pobject->GetProject();
                            DWORD _EXEC_FLAGS=::EXEC_FLAGS;
                            EXEC_FLAGS|=(EF_STEPACTIVE);
                            if(Push_Context())
                                _project->DoOneStep(pobject);
                            Pop_Context();
                            ::EXEC_FLAGS=_EXEC_FLAGS;
                        }
                    }
                    // !!! idle action
                    continue;
                }
            }
            else
                GetMessage(&msg,0,0,0);
            if (msg.message==WM_KEYDOWN && msg.wParam==VK_TAB)
            {
                int shift=ScGetKeyState(VK_SHIFT);
                if(ScGetKeyState(VK_CONTROL))
                {
                    HWND pages=GetDlgItem(dialog,ID_SCSYSTAB);
                    SendMessage(pages,WM_KEYDOWN,shift?VK_LEFT:VK_RIGHT,0);
                    goto m1;
                }
                HWND focus=GetFocus();
                HWND parent=GetParent(focus);
                if (parent==dialog ||parent==b->activehwnd)
                {
                    UINT uFlag=shift?GW_HWNDPREV:GW_HWNDNEXT;
                    if (parent==b->activehwnd)
                    {
                        while ((focus=GetNextWindow(focus, uFlag))!=0)
                        {
                            LONG style=GetWindowLong(focus,GWL_STYLE);
                            if ((style&WS_TABSTOP)&& IsWindowEnabled(focus)&&IsWindowVisible(focus))
                            {
                                SetFocus(focus);
                                goto m1;
                            }
                        }
                        if (!focus)
                        {
                            SetFocus(GetDlgItem(dialog,shift?ID_SCSYSTAB:IDOK));
                            goto m1;
                        }
                    }
                    else
                    {
                        {
#ifdef WIN32
                            int id=GetWindowLong(focus,GWL_ID);
#else
                            int id=GetWindowWord(focus,GWW_ID);
#endif
                            if (id==ID_SCSYSTAB)
                                if(!shift)
                                {
                                    SetFocus(b->activehwnd);
                                    goto m1;
                                }
                                else
                                    // Na Ok Cancle Help
                                    if (shift)
                                    {
                                        SetFocus(GetNextDlgTabItem(b->activehwnd,GetWindow(b->activehwnd,GW_CHILD),uFlag));
                                        goto m1;
                                    }
                            goto m2;
                        }
                    }
                }
            };
            if(!IsDialogMessage(b->activehwnd,&msg))
            {
m2:
                if(!IsDialogMessage(dialog,&msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
m1:
        }
        while(b->result==-1);
        DestroyWindow(dialog);
        EnableWindow(wndtodisable,!alredydisabled);
    }
    return b->result;
};
//*************************************************

TObjVarData::TObjVarData(PObject po,INT16 _index){
    if (_index>-1 && (_index<po->_class->varcount)){
        index=_index;
        object=po;
    }else{object=0;_index=index;}
};

_TObj::_TObj(){memset(this,0,sizeof(_TObj));}
_TObj::~_TObj(){
    while (items){
        _TObj *po=items;
        items=items->next;
        delete po;
    }
    while (vars){
        _TVar* v=vars;
        vars=vars->next;
        delete v;
    }
};

void _TObj::Insert(_TObj*o){
    if (items){
        _TObj*i=items;
        while (i->next)i=i->next;
        i->next=o;
    }else items=o;
};

void _TObj::InsertVar(INT16 index){
    _TVar*v=new _TVar();
    v->o=this;
    v->next=0;
    v->index=index;
    if (vars){
        _TVar*i=vars;
        while (i->next)i=i->next;
        i->next=v;
    }else vars=v;
};



_TObj * BuildVarsData(TObjVarData&d,PObject po){
    _TObj*o=new _TObj();
    o->data=&d;
    o->object=po;

    PObject _po=po->childObjects;

    while (_po){
        _TObj*_o=BuildVarsData(d,_po);
        if (_o)o->Insert(_o);
        _po=_po->next;
    }
    for(INT16 i=0;i<po->_class->varcount;i++){
        if (po->vars[i] == d.var){
            o->InsertVar(i);
        }
    }
    if (o->items==NULL && o->vars==NULL)
    { delete o;o=NULL; }
    return o;
};

//void InsertVarInfo();
BOOL scCALLBACK VarMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TObjVarData*data=(TObjVarData*)b->data;
            TVarInfo * vi= &(data->object->_class->vars[data->index]);
            SetDlgItemText(hwnd,IDC_VARNAME,vi->name);
            char * s=vi->classType->GetClassName();
            SetDlgItemText(hwnd,IDC_VARTYPE,s);
            SetDlgItemText(hwnd,IDC_VARINFO,vi->info);
        }return TRUE;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK VarInfoProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HWND tsw=GetDlgItem(hwnd,IDC_TREEINFO);
            VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
            sw->NotifyWnd=mainwindow->HWindow;

            TObjVarData*data=(TObjVarData*)b->data;
            TreeItemData td;

            td.parent = NULL;
            td.IsLeaf = 0;
            td.IsImage = 0;
            td.datatype=TREE_VARinfoOBJ;
            td.data = (DWORD)data->root;
            td.text=data->root->object->_class->name;
            sw->NotifyWnd=mainwindow->HWindow;
            ::SendMessage(tsw,TVN_INSERTITEM,0,(long)&td);
            ::SendMessage(tsw,TV_EXPAND,0,td.data);

        }return TRUE;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL Variable2dDialog(HWND hwnd,PObject po,INT16 index){
    int rez=0;
    TObjVarData data(po,index);
    if (data.object){
        PObject _po;
        if (GetRootObject(NULL,&_po)){

            data.var=po->vars[index];
            data.root=BuildVarsData(data,_po);
            if(data.root){

                TPropertyButton*b=new TPropertyButton(IDS_str1,HResource,"Var_Info",Var_info___Info);
                TPropertyButton*b1=b;
                b1->data=(DWORD)&data;
                b1->Procedure=VarMainProc;

                b1=new TPropertyButton(IDS_str2,HResource,"System_Info",Var_info___Using);
                b1->data=(DWORD)&data;
                b1->Procedure=VarInfoProc;

                b->Insert(b1);
                rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str3),b,0); // Информация о переменной
                delete b;

            }
            // V - максимальная закладка
            delete data.root;
        }
    }
    return rez;
};

int ModelDialogBox(char *name,int size,char **p)
{
    if((size%3) && (size>2))return 0;
    TPropertyButton*b=NULL;
    for(int i=0;i<size/3;i++){
        TPropertyButton*b1=new TPropertyButton(p[i*3],(HINSTANCE)1,p[i*3+1]);
        b1->pdata=SCGetObject(p[i*3+2]);
        if(b)b->Insert(b1);else b=b1;
    }
    DWORD _e=EXEC_FLAGS;
    EXEC_FLAGS=EXEC_FLAGS&(~EF_STEPACTIVE);
    int rez=CreatePropertySheet(mainwindow->HWindow,name,b,0);
    EXEC_FLAGS=_e;
    delete b;
    return rez;
}

#ifdef DBE
BOOL scCALLBACK TableMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL scCALLBACK TableIdxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL scCALLBACK TableRecProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
#endif
BOOL TableDialog(HWND hwnd,int table){
#ifdef DBE
    TPropertyButton*b=new TPropertyButton(IDS_str5,HResource,"Base_Main");  // Основное
    b->data=table;
    b->Procedure=TableMainProc;

    TPropertyButton*b1=new TPropertyButton(IDS_str63,HResource,"Base_Rec");  // Основное
    b1->data=table;
    b1->Procedure=TableRecProc;
    b->Insert(b1);

    b1=new TPropertyButton("Indexes",HResource,"Base_Idx");  // Основное
    b1->data=table;
    b1->Procedure=TableIdxProc;
    b->Insert(b1);

    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str62),b,0);
    delete b;
    return rez;
#else
    return 0;
#endif
};

void __AddClassItems(TCollection*items,PLibrary lib){
    for(C_TYPE i=0;i<lib->items->count;i++){
        TClassListInfo*clf=(TClassListInfo*)lib->items->At(i);
        if(clf->flags&CLF_LIBRARY)
        {
            __AddClassItems(items,(PLibrary)clf);
        }else{
            //  if (!(clf->flags&CLF_LOADED && clf->_class->first)){
            if (((clf->flags&CLF_LOADED)==0) ||
                (clf->_class->first==NULL)){
                char s[256];
                s[0]='C';
                lstrcpy(s+1,clf->GetClassName());
                items->Insert(NewStr(s));
            }}}
}

BOOL scCALLBACK LibTools1Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(Unused_class_delete)
            if(ListBoxProc(hwnd,msg,wParam,lParam,IDC_CLASSES))return TRUE;
    switch(msg){
        case WM_INITDIALOG:{
            TCollection * items=(TCollection *)lParam;
            HWND list=GetDlgItem(hwnd,IDC_CLASSES);
            for(C_TYPE i=0;i<items->count;i++){
                SendMessage(list,LB_ADDSTRING,0,(LPARAM)items->At(i));
            }} return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch (command){
        case IDOK:{
                    HWND list=GetDlgItem(hwnd,IDC_CLASSES);
                    int count=(int)SendMessage(list,LB_GETCOUNT,0,0);
                    for(int i=0;i<count;i++){
                        char s[256];
                        SendMessage(list,LB_GETTEXT,i,(LPARAM)s);
                        if(s[0]=='C'){
                            DeleteClass(s+1);
                        }
                    }
                }EndDialog(hwnd,TRUE);return TRUE;
                case IDCANCEL:EndDialog(hwnd,FALSE);return TRUE;
            }}break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL UnusedClassesDialog(HWND hwnd,PLibrary lib){
    TStringCollection items(10,10);
    __AddClassItems(&items,lib);
    BOOL rez=0;
    if(items.count){
        rez=DialogBoxParam(HResource,MAKEINTRESOURCE(LIB_TOOLS1),hwnd,LibTools1Proc,(LPARAM)&items);
    }else{
        MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG20),_MSG_INFO,MB_OK);
    }
    for(C_TYPE i=0;i<items.count;i++){
        delete items.At(i);
        items.AtPut(i,0);
    }
    return rez;
};


//***********************************************************************//
//                                                                       //
//                Далее для NewChildDialog                               //
//                                                                       //
//***********************************************************************//


// Это Структура для передачи параметров

char lastlibname[128];
extern HICON ICON_FOLDER;
int NewChildLib(PLibrary lib,void*data){
    int i=(int)SendMessage((HWND)(data),CB_ADDSTRING,0,(LPARAM)lib->path);
    SendMessage((HWND)(data),CB_SETITEMDATA,i,(LPARAM)lib);
    return 1;
};

int NewChildClass(char* _class,void*data){
    SendMessage((HWND)(data),CB_ADDSTRING,0,(LPARAM)_class);
    return 1;
};
char*GetPathName(TLibrary*lib){
    if(lib->path)return lib->path;
    return "All Librarys";
}
extern TLibrary*librarys;

void NewChildSubProc_01(HWND hwnd)
{
    HWND _hwnd=GetDlgItem(hwnd,IDC_LIB);
    INT16 libid=(INT16)SendMessage(_hwnd,CB_GETCURSEL,0,0);
    if(libid<0){
        PLibrary lib=(PLibrary)librarys->items->At(0);
        libid=SendMessage(_hwnd,CB_FINDSTRINGEXACT,-1,(LPARAM)lib->path);
        SendMessage(_hwnd,CB_SETCURSEL,libid,0);
    }
    if(libid<0)return;
    PLibrary lib=(PLibrary)SendMessage(_hwnd,CB_GETITEMDATA,libid,0);
    if(lib==0)return;
    lstrcpyn(lastlibname,lib->path,sizeof(lastlibname));
    _hwnd=GetDlgItem(hwnd,IDC_ICONLIST);
    SendMessage(_hwnd,LB_RESETCONTENT,0,0);
    for(C_TYPE i=0;i<lib->items->count;i++){
        PLibrary _lib=(PLibrary)lib->items->At(i);
        char *s=NULL;
        if(_lib->flags&CLF_LIBRARY)s=_lib->path ;else s=((PClassListInfo)_lib)->GetClassName();
        SendMessage(_hwnd,LB_ADDSTRING,0,(LPARAM)s);
        SendMessage(_hwnd,LB_SETITEMDATA,i,(LPARAM)_lib);
    }
};
void NewChildSubProc_02(HWND hwnd,HWND hwndCtl)
{
    int i=SendMessage(hwndCtl,LB_GETCURSEL,0,0);
    if(i==LB_ERR)return;
    PLibrary lib=(PLibrary)SendMessage(hwndCtl,LB_GETITEMDATA,i,0);
    if(lib->flags&CLF_LIBRARY){
        HWND libwin= GetDlgItem(hwnd,IDC_LIB);
        i=SendMessage(libwin,CB_FINDSTRINGEXACT,-1,(LPARAM)lib->path);
        if(i!=CB_ERR){SendMessage(libwin,CB_SETCURSEL,i,0);}
        NewChildSubProc_01(hwnd);
    }else{
        PClassListInfo pcli=(PClassListInfo)lib;
        char *s=(char *)GetWindowLong(hwnd,DWL_USER);
        lstrcpy(s,pcli->GetClassName());
        EndDialog(hwnd,1);
    }

}
void NewChildSubProc_03(HWND hwnd)
{
    HWND _hwnd=GetDlgItem(hwnd,IDC_LIB);
    INT16 libid=(INT16)SendMessage(_hwnd,CB_GETCURSEL,0,0);
    if(libid<0)return;
    PLibrary lib = (PLibrary)SendMessage(_hwnd,CB_GETITEMDATA,libid,0);
    if(lib->parent){
        libid=SendMessage(_hwnd,CB_FINDSTRINGEXACT,-1,(LPARAM)GetPathName(lib->parent));
        if(libid!=CB_ERR){SendMessage(_hwnd,CB_SETCURSEL,libid,0);
            NewChildSubProc_01(hwnd);
        }
    }
}
BOOL scCALLBACK NewChildProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    _STDHELP(Class_chose_dialog)
            HWND  hwndCtl;
    INT16 wNotifyCode;
    INT16 command;
    switch(msg)
    {
        case WM_MEASUREITEM:
        {
            RECT r;
            GetClientRect(GetDlgItem(hwnd,IDC_ICONLIST),&r);
            MEASUREITEMSTRUCT*lpmisCtl = (MEASUREITEMSTRUCT FAR*) lParam;
            lpmisCtl->itemWidth=r.right/3-1;
            lpmisCtl->itemHeight=36;
        }return TRUE;
        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT*lpdis = (DRAWITEMSTRUCT *) lParam;
            if(lpdis->itemID<0)
                return 0;
            HBRUSH b=CreateSolidBrush(GetSysColor(COLOR_WINDOW));
            HGDIOBJ _b=SelectObject(lpdis->hDC,b);
            PatBlt(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,
                   (lpdis->rcItem.right-lpdis->rcItem.left),
                   (lpdis->rcItem.bottom-lpdis->rcItem.top),PATCOPY);
            SelectObject(lpdis->hDC,_b);DeleteObject(_b);

            PLibrary lib=(PLibrary)lpdis->itemData;
            if(!lib)
                return 0;
            char * s=NULL;
            int x=lpdis->rcItem.left+2,
                    y=lpdis->rcItem.top+2;
            if(lib->flags&CLF_LIBRARY)
            {
                s=lib->path;
                DrawIcon( lpdis->hDC,x,y,ICON_FOLDER);
            }
            else
            {
                PClassListInfo pcli=(PClassListInfo)lib;
                PaintIcon(pcli->GetClass(),lpdis->hDC,x,y,32,32);
                s=pcli->GetClassName();
            }
            x+=36;
            y=lpdis->rcItem.top+10;
            int lx=lpdis->rcItem.right-x;
            char temp[128];
            lstrcpy(temp," ");
            lstrcat(temp,s);
            lstrcat(temp," ");
            s=temp;
            int textlen=lstrlen(s);
            SIZE p1;
            int pointslen=0;
            GetTextExtentPoint(lpdis->hDC,s,textlen,&p1);
            if (p1.cx>lx)
            {
                textlen--;
                {
                    SIZE p2;
                    GetTextExtentPoint(lpdis->hDC,"... ",4,&p2);
                    pointslen=p2.cx;
                }
                while ((p1.cx+pointslen)>lx)
                {
                    textlen--;
                    s[textlen]=0;
                    GetTextExtentPoint(lpdis->hDC,s,textlen,&p1);
                }
                lstrcat(s,"... ");
                textlen=lstrlen(s);
                GetTextExtentPoint(lpdis->hDC,s,textlen,&p1);
            }
            SetBkMode(lpdis->hDC,OPAQUE);
            COLORREF oldf=SetBkColor(lpdis->hDC,GetSysColor(((lpdis->itemState&ODS_SELECTED)!=0)?COLOR_HIGHLIGHT:COLOR_WINDOW));
            COLORREF oldc=SetTextColor(lpdis->hDC,GetSysColor((lpdis->itemState&ODS_SELECTED)?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT));
            SetTextAlign(lpdis->hDC,TA_TOP |TA_LEFT);
            TextOut(lpdis->hDC,x,y,s,lstrlen(s));
            SetTextColor(lpdis->hDC,oldc);
            SetBkColor(lpdis->hDC,oldf);
            SetBkMode(lpdis->hDC,TRANSPARENT);
            if(lpdis->itemState&ODS_FOCUS)
            {
                RECT r=lpdis->rcItem;
                r.top=y;r.bottom=y+p1.cy;
                r.left=x;r.right=x+p1.cx;
                InflateRect(&r, 1, 1 );
                DrawFocusRect(lpdis->hDC,&r);
            }
        } return TRUE;
        case WM_INITDIALOG:{
            SetWindowLong(hwnd,DWL_USER,lParam);
            HWND libwin= GetDlgItem(hwnd,IDC_LIB);
            EnumerateLibs(NULL,ELF_NOHIERARCHCY,NewChildLib,(void*)libwin);
            int i=(int)SendMessage(libwin,CB_ADDSTRING,0,(LPARAM)GetPathName(librarys));
            SendMessage(libwin,CB_SETITEMDATA,i,(LPARAM)librarys);
            i=SendMessage(libwin,CB_FINDSTRINGEXACT,-1,(LPARAM)lastlibname);

            if(i!=CB_ERR){
                SendMessage(libwin,CB_SETCURSEL,i,0);
            }
            NewChildSubProc_01(hwnd);
        };
        case WM_KEYDOWN:{
            int wVkey = wParam;
            if(wVkey==VK_BACK)NewChildSubProc_03(hwnd);
        }break;
        case WM_COMMAND:
            EV_COMMANDnoVar
                    switch (command){
        case IDC_BACKLIB:{
                    NewChildSubProc_03(hwnd);
                }return TRUE;
                case IDC_LIB:
                    if (wNotifyCode==CBN_SELCHANGE){
                        NewChildSubProc_01(hwnd);
                    }return TRUE;
                case IDC_ICONLIST:{
                    if(wNotifyCode==LBN_DBLCLK){
                        NewChildSubProc_02(hwnd,hwndCtl);
                    }};return TRUE;
                case IDOK:{
                    NewChildSubProc_02(hwnd,GetDlgItem(hwnd,IDC_ICONLIST));
                    return TRUE;
                }
                case IDCANCEL: EndDialog(hwnd,0);return TRUE;
            }
    }
    return FALSE;
};

BOOL NewChildDialog(HWND hwnd,PClass _scheme,char*name)
{
    int rez=DialogBoxParam(HResource,MAKEINTRESOURCE(NEW_CHILDDIALOG),hwnd,NewChildProc,(LPARAM)name);
    return rez;
};

char extern * ini_file;

void GetTemplateDir(char*s)
{
    //char tmp[256];
    //GetModuleFileName(hInstance,tmp,sizeof(tmp));
    //GetPath(tmp,s);
    GetPath(_argv[0],s);
    AddSlash(s);
    lstrcat(s,"template");
}

void AddMsgStr(char *s,long t);
MCompiler * compiler=0;

int InitCompiler()
{
    if(compiler)
        return TRUE;

    DWORD tic,tic1;
    tic=GetTickCount();

    char templateDirPath[256];
    //GetPrivateProfileString("COMPILER","template","compiler.tpl",s,255,ini_file);
    GetTemplateDir(templateDirPath);

    lstrcat(templateDirPath,"\\compiler.tpl");
    compiler=new MCompiler(templateDirPath);

    tic1=GetTickCount();
    AddMsgStr((char*)MAKEINTRESOURCE(IDS_str96), tic1-tic);
    return 1;
};

int DoneCompiler()
{
    if (compiler)
        delete compiler;
#ifndef RUNTIME
    if(syntaxcolor)
    {
        delete syntaxcolor;
        syntaxcolor=NULL;
    }
#endif
    return 1;
};

void IfNeedModifyVar(PClass _class, TVarData* vardata,TVarData* old)
{
    BOOL need = FALSE;
    if (vardata->items->count == old->items->count)
    {
        for(int i = 0; i < vardata->items->count; i++)
        {
            PVarInfo pv = (PVarInfo)vardata->items->At(i);
            PVarInfo pv1 = (PVarInfo)old->items->At(i);
            if (lstrcmp(pv->name,pv1->name) || pv->classType!=pv1->classType)
            {
                need=TRUE;
                break;
            }
        }
    }
    else
        need=TRUE;

    if (need)
    {
        ModifyVar(_class, vardata);
    }
    else
    {
        for(int i=0;i<vardata->items->count;i++)
        {
            PVarInfo pv=(PVarInfo)vardata->items->At(i);
            _class->vars[i].flags=pv->flags;
        }
        ObjectsChanged();
    }
    _class->UpdateDisableVar();
}

int SetClassText(PClass _class, TVarData* vardata, char* txt)
{
#ifdef LOGON
    {
        char s[256];
        wsprintf(s,"Зашел в SetClassText(%s) (dialogs.cpp)",_class->name);
        LogMessage(s);
    }
    TLOGoffset __offset(2);
#endif

    TVarData old(*vardata);
    vardata->DeleteCompilerVars();

    //Text not null or empty
    int len = 0;
    if (txt)
    {
        char* p = txt;
        while(*p)
        {
            if (*p > 32)
            {
                len = 1;
                break;
            }
            p++;
        }
    }
    if(txt == NULL || len == 0)     //Удаление текста из класса
    {
        if (_class->text)
        {
            delete _class->text;
            _class->text=NULL;

            _class->SetModify();
            _class->NS.text=1;
        }
        if (_class->VM_code)
        {
            delete _class->VM_code;

            _class->VM_code=NULL;
            _class->SetModify();
        }
        IfNeedModifyVar(_class,vardata,&old);
        return 1;
    }
    else  //если текст корректен
    {
        if (!compiler)
            InitCompiler();

        if(compiler->Compile(_class, vardata, txt))
        {
            if(_class->text)
            {
                delete _class->text;
                _class->text=NULL;
            }
            _class->text = NewStr(txt);
            _class->SetModify();

            //Для каждой переменной копирование имеющегося описания и значения по умолчанию
            for(int i = 0; i < vardata->items->count; i++)
            {
                PVarInfo pv = (PVarInfo)vardata->items->At(i);
                for(int j = 0; j < old.items->count; j++)
                {
                    PVarInfo pv1 = (PVarInfo)old.items->At(j);
                    if (!lstrcmpi(pv->name, pv1->name) && pv->classType == pv1->classType)
                    {
                        if (pv->def == NULL && pv1->def != NULL)
                            pv->def = NewStr(pv1->def);

                        if (pv->info == NULL && pv1->info != NULL)
                            pv->info = NewStr(pv1->info);
                    }
                }
            }
            if (_class->flags & CF_PROCEDURE || _class->flags & CF_STRUCTURE)
            {
                int size = 0;
                for(int i = 0;i < vardata->items->count; i++)
                {
                    PVarInfo pv = (PVarInfo)vardata->items->At(i);
                    PClass _c = pv->classType->GetClass();
                    size += (int)_c->varsize;
                }
                _class->varsize=size;
            }
            else
                _class->varsize=0;

            IfNeedModifyVar(_class, vardata, &old);
        }
        else
        {
            TCollection* c = old.items;
            old.items = vardata->items;
            vardata->items = c;
            return 0;
        }
        return 2;
    }
}

BOOL scCALLBACK LibMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            PLibrary lib=(PLibrary)(b->pdata);
            SetDlgItemText(hwnd,IDC_LIBNAME,lib->path);
            char s[260];
            lib->GetPath(s);
            SetDlgItemText(hwnd,IDC_PATH,s);
            int c=lib->GetClassCount();
            if(b->dwuser1){
                EnableWindow(GetDlgItem(hwnd,IDC_RECURSE),1);

                TProject*prj=(TProject*)b->dwuser1;

                UINT32 i=prj->GetVarDef("lib_norecurse",0);
                CheckDlgButton(hwnd,IDC_RECURSE,!i);

                if(prj->GetVarS("addlib",s)){
                    SetDlgItemText(hwnd,IDC_ADDLIBprj,s);
                };
            }else{
                ShowWindow(GetDlgItem(hwnd,IDC_ADDLIBBOX),SW_HIDE);
                ShowWindow(GetDlgItem(hwnd,IDC_ADDLIBprj),SW_HIDE);
            }
            char buf[256];
            LoadString(HResource,IDS_str97,buf,sizeof(buf));

            wsprintf(s,buf,c);
            SetDlgItemText(hwnd,IDC_TEXT,s);
            return TRUE;
        }
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                if (b->flags&PSP_INITIALIZE){
                    TProject*prj=(TProject*)b->dwuser1;
                    if(prj){
                        int i=IsDlgButtonChecked(hwnd,IDC_RECURSE);
                        prj->SetVarDef("lib_norecurse",!i,0);
                        if(SendDlgItemMessage(hwnd,IDC_ADDLIBprj,EM_GETMODIFY,0,0)){
                            char s[256];
                            SendDlgItemMessage(hwnd,IDC_ADDLIBprj,WM_GETTEXT,sizeof(s),(LPARAM)s);
                            prj->SetVarS("addlib",s);
                        }
                    }}}break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL LibraryDialog(HWND hwnd,PLibrary lib){
    TPropertyButton*b=new TPropertyButton(IDS_str5,HResource,"Lib_Main",Library_options); // Основное
    b->Procedure=LibMainProc;
    b->pdata=lib;
    BOOL rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str13),b); //"Библиотека"
    delete b;
    return rez;
};
//***********************************************************************//
//                                                                       //
//                Далее для FileType Диалог.                             //
//                                                                       //
//***********************************************************************//

char * GetFileExt(char * ft){
    char *_ps;
    char *_pps=ft;
    do{
        _ps=strstr(_pps,".");
        if(_ps)_pps=_ps+1;
    }while (_ps);
    if(_pps==ft)_pps=0;
    return _pps;
}
BOOL scCALLBACK FileTypeProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(File_type_dialog)
            HWND hwndCtl;
    int wNotifyCode;
    int command;

    switch(msg){
        case WM_INITDIALOG:{
            GETFILETYPE *ft=(GETFILETYPE *)lParam;
            SetDlgItemText(hwnd,IDC_FILENAME,ft->file);
            char s[512];
            GetPrivateProfileString("filters",NULL,"",s,sizeof(s),ini_file);
            char *_ps=s;
            while (_ps && *_ps){
                char ss[256];
                GetPrivateProfileString("filters",_ps,_ps,ss,sizeof(ss),ini_file);
                SendDlgItemMessage(hwnd,IDC_FLTGROUP,CB_ADDSTRING,0,(LPARAM)ss);
                SendDlgItemMessage(hwnd,IDC_FLTGROUP1,LB_ADDSTRING,0,(LPARAM)_ps);
                _ps+=lstrlen(_ps)+1;
            };
            SendDlgItemMessage(hwnd,IDC_FLTGROUP,CB_SETCURSEL,0,0);
            SetWindowLong(hwnd,DWL_USER,lParam);
            goto m1;
        }return 1;
        case WM_COMMAND:{
            EV_COMMANDnoVar
                    switch (command){
        case IDC_FLTGROUP:if(wNotifyCode==CBN_SELENDOK){
m1:
                        int i=SendDlgItemMessage(hwnd,IDC_FLTGROUP,CB_GETCURSEL,0,0);
                        char group[64];
                        char s[1024];
                        lstrcpy(group,"filter_");
                        SendDlgItemMessage(hwnd,IDC_FLTGROUP1 , LB_GETTEXT,i,(LPARAM)(group+lstrlen(group)));
                        GetPrivateProfileString(group,NULL,"",s,sizeof(s),ini_file);
                        SendDlgItemMessage(hwnd,IDC_FLTITEM,CB_RESETCONTENT,0,0);
                        SendDlgItemMessage(hwnd,IDC_FLTITEM1,LB_RESETCONTENT,0,0);
                        int count=0;
                        char *_ps=s;
                        while (_ps && *_ps){
                            char ss[256];
                            GetPrivateProfileString(group,_ps,"",ss,sizeof(ss),ini_file);
                            char * _pp=strstr(ss,";");
                            if(_pp)*_pp=0;
                            SendDlgItemMessage(hwnd,IDC_FLTITEM,CB_ADDSTRING,0,(LPARAM)ss);
                            SendDlgItemMessage(hwnd,IDC_FLTITEM1,LB_ADDSTRING,0,(LPARAM)_ps);
                            _ps+=lstrlen(_ps)+1;
                            count++;
                        };
                        EnableWindow(GetDlgItem(hwnd,IDC_FLTITEM),count>0);
                        SendDlgItemMessage(hwnd,IDC_FLTITEM,CB_SETCURSEL,0,0);
                        goto m2;
                    }return 1;
                case IDC_FLTITEM:if(wNotifyCode==CBN_SELENDOK){
m2:    SetDlgItemText(hwnd,IDC_ADVLIST1,"");
                        int i=SendDlgItemMessage(hwnd,IDC_FLTITEM,CB_GETCURSEL,0,0);
                        char item[64];
                        SendDlgItemMessage(hwnd,IDC_FLTITEM1 , LB_GETTEXT,i,(LPARAM)item);
                        char group[64];
                        i=SendDlgItemMessage(hwnd,IDC_FLTGROUP,CB_GETCURSEL,0,0);
                        lstrcpy(group,"filter_");
                        SendDlgItemMessage(hwnd,IDC_FLTGROUP1 , LB_GETTEXT,i,(LPARAM)(group+lstrlen(group)));

                        char ss[1024];
                        GetPrivateProfileString(group,item,"",ss,sizeof(ss),ini_file);
                        char * _ps=strstr(ss,";");
                        EnableWindow(GetDlgItem(hwnd,IDOK),_ps!=NULL);
                        EnableWindow(GetDlgItem(hwnd,IDC_ADVLIST),_ps!=NULL);
                        EnableWindow(GetDlgItem(hwnd,IDC_ALWAYSEXT),_ps!=NULL);
                        SendDlgItemMessage(hwnd,IDC_ADVLIST,CB_RESETCONTENT,0,0);
                        if(_ps){
                            char *_a=_ps+1;
                            do{
                                _ps=strstr(_a,";");
                                char flt[256];
                                if(_ps){lstrcpyn(flt,_a,(_ps-_a)+1);_a=_ps+1;}else lstrcpy(flt,_a);
                                if(!lstrcmpi(flt,"internal")){
                                    SendDlgItemMessage(hwnd,IDC_ADVLIST,CB_ADDSTRING,0,(LPARAM)"Open with Stratum computer");
                                }else{
                                    if(flt[0]=='/'){
#ifdef WIN32
                                        char * _import_="import32=";
#else
                                        char * _import_="import16=";
#endif
                                        if(strstr(flt+1,_import_)==flt+1){
                                            SetDlgItemText(hwnd,IDC_ADVLIST1,flt+1+lstrlen(_import_));
                                        }
                                    }}
                            }while(_ps);
                        }
                        EnableWindow(GetDlgItem(hwnd,IDC_ADVLIST),SendDlgItemMessage(hwnd,IDC_ADVLIST,CB_GETCOUNT,0,0)>0);
                        SendDlgItemMessage(hwnd,IDC_ADVLIST,CB_SETCURSEL,0,0);
                    }return 1;
                case IDOK:{
                    GETFILETYPE *ft=(GETFILETYPE *)GetWindowLong(hwnd,DWL_USER);
                    int i=SendDlgItemMessage(hwnd,IDC_FLTITEM,CB_GETCURSEL,0,0);
                    SendDlgItemMessage(hwnd,IDC_FLTITEM1 , LB_GETTEXT,i,(LPARAM)(ft->type));
                    i=SendDlgItemMessage(hwnd,IDC_FLTGROUP,CB_GETCURSEL,0,0);
                    SendDlgItemMessage(hwnd,IDC_FLTGROUP1 , LB_GETTEXT,i,(LPARAM)((LPARAM)(ft->group)));
                    GetDlgItemText(hwnd,IDC_ADVLIST1,ft->filter,sizeof(ft->filter));
                    ft->flags&(~3);
                    ft->flags |= (IsDlgButtonChecked(hwnd,IDC_FILTER1)?1:0);
                    ft->flags |= (IsDlgButtonChecked(hwnd,IDC_FILTER2)?2:0);
                    if(IsDlgButtonChecked(hwnd,IDC_ALWAYSEXT))
                    {
                        char * _pps=GetFileExt(ft->file);
                        if(_pps){
                            char s[256];
                            wsprintf(s,"%s,%s",ft->group,ft->type);
                            WritePrivateProfileString("filter_ext",_pps,s,ini_file);
                        }}
                }EndDialog(hwnd,1);return 1;
                case IDCANCEL:EndDialog(hwnd,0);return 1;
            };
        }break;
    }
    return 0;
};
BOOL GetFileTypeDialog(HWND hwnd,GETFILETYPE&ft){

    char * _pps=GetFileExt(ft.file);
    if(_pps){
        char s[256];
        GetPrivateProfileString("filter_ext",_pps,"",s,sizeof(s),ini_file);
        if(lstrlen(s)){
            char *_ps=strstr(s,",");
            if(_ps){
                lstrcpyn(ft.group,s,(_ps-s)+1);
                lstrcpy(ft.type,_ps+1);
                return 1;
            }
        }}

    return DialogBoxParam(HResource,MAKEINTRESOURCE(FTYPE_DIALOG),hwnd,FileTypeProc,(LPARAM)&ft);

};



BOOL IsIntValid(HWND hwnd,int id,INT16&r,int _min,int _max,BOOL _msg){

    HWND h=GetDlgItem(hwnd,id);
    int len=(int)SendMessage(h,WM_GETTEXTLENGTH,0,0);
    if (len){
        char text[256];
        GetWindowText(h,text,256);

        char * endptr=(char *)text;
        INT16 value = (INT16)strtol(text,&endptr,10);

        if ((endptr==NULL) || ((*endptr)==0)){
            if (value >= _min && value <= _max){r=value;  return TRUE;}}

    }
    if(_msg){
        char s[256];
        char buf[256];
        LoadString(HResource,IDS_str98,buf,sizeof(buf));
        sprintf(s,buf,_min,_max);
        MSGBOX(hwnd,s,MAKEINTRESOURCE(IDS_str98),MB_OK|MB_ICONSTOP);
        SetFocus(h);
    }
    return FALSE;

};

BOOL IsDoubleValid(HWND hwnd,int id,double&ret,double _min,double _max,BOOL _msg)
{
    BOOL msg=TRUE;
    if(_min >_max)
    {
        double _d=_max;
        _max=_min;_min=_d;
        msg=FALSE;
    }
    char *text;
    HWND h=GetDlgItem(hwnd,id);
    int len=(int) SendMessage(h,WM_GETTEXTLENGTH,0,0);
    if (len)
    {
        text=new char[len+1];
        GetWindowText(h,text,len+1);
        double value;
        int rz=StrToDouble(text,value);
        delete text;
        if (rz)
        {
            if(value>=_min && value<=_max)
            {
                ret=value;
                return TRUE;
            }
        }
    }
    if(msg && _msg)
    {
        char s[256];
        char buf[256];
        LoadString(HResource,IDS_str100,buf,sizeof(buf));
        sprintf(s,buf,_min,_max);
        MSGBOX(hwnd,s,MAKEINTRESOURCE(IDS_str98),MB_OK|MB_ICONSTOP);
        SetFocus(h);
    }
    return FALSE;
};

//
// Project dialog
//
struct NEWPROJECTstruct{
    char file[256];
    char name[256];
};
NEWPROJECTstruct *np;


BOOL scCALLBACK NewProjectProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(Project_creating_Dialog)
            switch(msg){
        case WM_INITDIALOG:{
            SetDlgItemText(hwnd,IDC_PATH,np->file);
            SetDlgItemText(hwnd,IDC_CLASSNAME,np->name);
            SendDlgItemMessage(hwnd,IDC_TEMPLATE,CB_ADDSTRING,0,(LPARAM)"None");
        }return TRUE;
        case WM_COMMAND:
            EV_COMMANDsimply
                    switch (command){
        case IDC_BROWSE:{
                    if(FileDialog(hwnd,np->file,IDS_PROJECT,FD_SAVE)){
                        SetDlgItemText(hwnd,IDC_PATH,np->file);
                    }return TRUE;
                }
                case IDOK:
                    GetDlgItemText(hwnd,IDC_PATH,np->file,255);
                    GetDlgItemText(hwnd,IDC_CLASSNAME,np->name,255);
                    if(!IsNameValid(np->name)){
                        _Error(EM_CLASSNAMEERROR);
                        return TRUE;
                    };
                    EndDialog(hwnd,1);return TRUE;
                case IDCANCEL: EndDialog(hwnd,0);return TRUE;
            }
    }
    return FALSE;
};

BOOL NewProjectDialog(HWND hwnd,char*path,char*name){
    NEWPROJECTstruct tnp;
    lstrcpy(tnp.file,path);
    lstrcpy(tnp.name,name);
    np=&tnp;
    int rez=DialogBox(HResource,MAKEINTRESOURCE(PROJECT_DIALOG),hwnd,
                      NewProjectProc);
    if (rez){
        lstrcpy(path,tnp.file);
        lstrcpy(name,tnp.name);
    }
    return rez;
};


BOOL scCALLBACK MathDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            switch(msg){
        case WM_INITDIALOG:{
            SetDlgItemText(hwnd,IDC_TEXT,(char*)lParam);

        }return TRUE;
        case WM_COMMAND:
            EV_COMMANDsimply
                    switch(command){
        case IDC_SKIP:EndDialog(hwnd,1);return TRUE;
                case IDC_NOSEE:EndDialog(hwnd,2);return TRUE;
                case IDCANCEL:
                case IDC_STOP:EndDialog(hwnd,0);return TRUE;
            }break;
    }
    return FALSE;
};



int  MathErrorDialog(char*s){
    if(winmanager->captured)winmanager->releasecapture();
    return DialogBoxParam(HResource,MAKEINTRESOURCE(MATH_ERROR),mainwindow->HWindow,
                          MathDlgProc,(DWORD)s);
};

int GetSearchRange(HWND hwnd){
    char s[256],s2[256];
    int k= SendMessage(GetDlgItem(hwnd,IDC_WHATFIND),CB_GETCURSEL,0,0);
    SendMessage(GetDlgItem(hwnd,IDC_WHATFIND),CB_GETLBTEXT,k,(LPARAM)s2);
    int i=IDS_SEARCHWHAT;
    int j=0;
    while(LoadString(HResource,i,s,sizeof(s))){
        if(!lstrcmp(s,s2))return j;
        i++;
        j++;
    };
    return 0;
};

void SearchRange(HWND hwnd,LPARAM lParam){
    long i=GetSearchRange(hwnd);
    EnableWindow(GetDlgItem(hwnd,IDC_NAMES      ),(i==0)||(i==2));
    EnableWindow(GetDlgItem(hwnd,IDC_INTERNAL   ),(i==0)||(i==2));
    EnableWindow(GetDlgItem(hwnd,IDC_CLASSNAMEsr),i==0);
    EnableWindow(GetDlgItem(hwnd,IDC_SRCTEXT    ),i==0);
    EnableWindow(GetDlgItem(hwnd,IDC_VARNAME    ),(i==0)||(i==1));
    EnableWindow(GetDlgItem(hwnd,IDC_VARDATA    ),(i==0)&&((lParam&3)==0));
    EnableWindow(GetDlgItem(hwnd,IDC_REMARKS    ),i==0);
    EnableWindow(GetDlgItem(hwnd,IDC_SEARCHGROUP),((i==0)/*||(i==2)*/)&&((lParam&16)==0));
};


void CheckSearch(HWND hwnd,UINT32 b,BYTE flag){
    if(flag&1){
        int f=3;
        if ((b&0xff)==0xff)f=1;
        if ((b&0xff)==0)f=0;
        CheckDlgButton(hwnd,IDC_ALLsr,f);
        int l=SendMessage(GetDlgItem(hwnd,IDC_SEARCHKEY),WM_GETTEXTLENGTH,0,0);
        BOOL d=(l==0)||(f==0);
        EnableWindow(GetDlgItem(hwnd,IDOK),!d);
    }
    if(flag&2){
        CheckDlgButton(hwnd,IDC_NAMES       ,(b&0x01)!=0);
        CheckDlgButton(hwnd,IDC_INTERNAL    ,(b&0x02)!=0);
        CheckDlgButton(hwnd,IDC_CLASSNAMEsr ,(b&0x04)!=0);
        CheckDlgButton(hwnd,IDC_SRCTEXT     ,(b&0x08)!=0);
        CheckDlgButton(hwnd,IDC_VARNAME     ,(b&0x10)!=0);
        CheckDlgButton(hwnd,IDC_VARDATA     ,(b&0x20)!=0);
        CheckDlgButton(hwnd,IDC_REMARKS     ,(b&0x40)!=0);
        CheckDlgButton(hwnd,IDC_REMARKS     ,(b&0x40)!=0);
        CheckDlgButton(hwnd,IDC_SEARCHHANDLE,(b&0x80)!=0);

    }
};

UINT32 GetKSearch(HWND hwnd){
    UINT32 b=0;
    if(IsDlgButtonChecked(hwnd,IDC_NAMES    ))b|=0x01;
    if(IsDlgButtonChecked(hwnd,IDC_INTERNAL ))b|=0x02;
    if(IsDlgButtonChecked(hwnd,IDC_CLASSNAMEsr))b|=0x04;
    if(IsDlgButtonChecked(hwnd,IDC_SRCTEXT  ))b|=0x08;
    if(IsDlgButtonChecked(hwnd,IDC_VARNAME  ))b|=0x10;
    if(IsDlgButtonChecked(hwnd,IDC_VARDATA  ))b|=0x20;
    if(IsDlgButtonChecked(hwnd,IDC_REMARKS  ))b|=0x40;
    if(IsDlgButtonChecked(hwnd,IDC_SEARCHHANDLE ))b|=0x80;

    return b;
};


BOOL scCALLBACK SearchProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            HWND  hwndCtl;
    INT16 wNotifyCode;
    INT16 command;
    switch(msg){
        case WM_INITDIALOG:{
            SetWindowLong(hwnd,DWL_USER,lParam);
            char s[256];
            if(SCGetScVarS("search_string",s)) SetDlgItemText(hwnd,IDC_SEARCHKEY,s);
            //EnableWindow(GetDlgItem(hwnd,IDC_WHATFIND),0);
            int i=IDS_SEARCHWHAT;
            INT32 m=SCGetScVar("search_what_tofind",0);
            int _k=1,j=0,z=0;
            while(LoadString(HResource,i,s,sizeof(s))){
                if (!(lParam & (1<<_k))){
                    SendMessage(GetDlgItem(hwnd,IDC_WHATFIND),CB_ADDSTRING,0,(LONG)s);
                    if (lParam & (1 << (m+1)))m=z;
                    z++;
                }
                _k++;
                i++;
            };

            SendMessage(GetDlgItem(hwnd,IDC_WHATFIND),CB_SETCURSEL,m,0);
            EnableWindow(GetDlgItem(hwnd,IDC_WHATFIND),z>1);

            SetWindowLong(hwnd,DWL_USER,lParam);

            UINT32 b=SCGetScVar("search_opt1",0);
            CheckDlgButton(hwnd,IDC_CASESENSITIVE,(b&SR_CASE)!=0);
            CheckDlgButton(hwnd,IDC_WHOLEWORDS,(b&SR_WORDS)!=0);
            CheckDlgButton(hwnd,IDC_SEARCHGROUP,(b&SR_MAKEGROUP)!=0);


            b=SCGetScVar("search_opt2",0);
            CheckSearch(hwnd,b,3);
            //	EnableWindow(GetDlgItem(hwnd,IDC_VARDATA),(lParam&1)==0);
            SearchRange(hwnd,lParam);
        }return TRUE;
        case WM_COMMAND:
            EV_COMMANDnoVar
                    switch(command){
        case IDC_WHATFIND:if(wNotifyCode==CBN_SELCHANGE){
                        long lP=GetWindowLong(hwnd,DWL_USER);
                        SearchRange(hwnd,lP);
                    };return TRUE;
                case IDC_SEARCHKEY:if(wNotifyCode==EN_UPDATE){
                        int b=IsDlgButtonChecked(hwnd,IDC_ALLsr);
                        int l=SendMessage(hwndCtl,WM_GETTEXTLENGTH,0,0);
                        BOOL d=(l==0)||(b==0);
                        EnableWindow(GetDlgItem(hwnd,IDOK),!d);
                    }return TRUE;
                case IDC_ALLsr:      {
                    int i=IsDlgButtonChecked(hwnd,IDC_ALLsr);
                    UINT32 b=0;
                    if(i==1)b=0xff;
                    CheckSearch(hwnd,b,3);
                } return TRUE;
                case IDC_SEARCHHANDLE:
                case IDC_NAMES    :
                case IDC_INTERNAL :
                case IDC_CLASSNAMEsr:
                case IDC_SRCTEXT  :
                case IDC_VARNAME  :
                case IDC_VARDATA  :
                case IDC_REMARKS  :{
                    UINT32 b=GetKSearch(hwnd);
                    CheckSearch(hwnd,b,1);
                }return TRUE;
                case IDCANCEL     :EndDialog(hwnd,0);return TRUE;
                case IDOK:{
                    long lP=GetWindowLong(hwnd,DWL_USER);


                    SCSetScVarI("search_what_tofind",GetSearchRange(hwnd));

                    UINT32 b=GetKSearch(hwnd);
                    SCSetScVarI("search_opt2",b);
                    char s[256];
                    GetDlgItemText(hwnd,IDC_SEARCHKEY,s,sizeof(s));
                    SCSetScVarS("search_string",s);
                    b=IsDlgButtonChecked(hwnd,IDC_CASESENSITIVE)?SR_CASE:0;
                    if(IsDlgButtonChecked(hwnd,IDC_WHOLEWORDS))b|=SR_WORDS;
                    if(IsDlgButtonChecked(hwnd,IDC_SEARCHGROUP ))b|=SR_MAKEGROUP;

                    SCSetScVarI("search_opt1",b);
                    EndDialog(hwnd,1);return TRUE;}
            }break;
    }
    return FALSE;
};

BOOL SearchDialog(HWND hwnd,UINT16 flags){
    return DialogBoxParam(HResource,MAKEINTRESOURCE(SEARCH_DIALOG),hwnd,SearchProc,flags);
};

#ifdef DBE
BOOL scCALLBACK TsrcMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;

            SEARCH_in_CURSOR*sc=(SEARCH_in_CURSOR*)b->data;
            SetDlgItemText(hwnd,IDC_SEARCHKEY,sc->scKey);
            CheckDlgButton(hwnd,IDC_CASESENSITIVE,sc->scCase);
            if(sc->scCurrent)CheckDlgButton(hwnd,IDC_ACTIVEFIELDS,1);else
                CheckDlgButton(hwnd,IDC_ALLFIELDS,1);
            if(sc->scDirection)CheckDlgButton(hwnd,IDC_LOOKFORWARD,1);else
                CheckDlgButton(hwnd,IDC_LOOKBUCK,1);
            CheckDlgButton(hwnd,IDC_LOOKALWAYS,sc->scFromStart);
            HWND combo=GetDlgItem(hwnd,IDC_SEARCHCMP);
            int i=IDS_SEARCHCMP;
            char s[256];
            while(LoadString(HResource,i,s,sizeof(s))){
                SendMessage(combo,CB_ADDSTRING,0,(LONG)s);
                i++;
            }
            SendMessage(combo,CB_SETCURSEL,sc->scFragment,0);

        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                if (b->flags&PSP_INITIALIZE){
                    SEARCH_in_CURSOR*sc=(SEARCH_in_CURSOR*)b->data;
                    GetDlgItemText(hwnd,IDC_SEARCHKEY,sc->scKey,sizeof(sc->scKey));
                    sc->scCase=     (BYTE)IsDlgButtonChecked(hwnd,IDC_CASESENSITIVE);
                    sc->scCurrent=  (BYTE)IsDlgButtonChecked(hwnd,IDC_ACTIVEFIELDS);
                    sc->scDirection=(BYTE)IsDlgButtonChecked(hwnd,IDC_LOOKFORWARD);
                    sc->scFromStart=(BYTE)IsDlgButtonChecked(hwnd,IDC_LOOKALWAYS);
                    sc->scFragment= (BYTE)SendDlgItemMessage(hwnd,IDC_SEARCHCMP,CB_GETCURSEL,0,0);
                }}return TRUE;
        case PSP_VALID:if (wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                char s[256];
                GetDlgItemText(hwnd,IDC_SEARCHKEY,s,sizeof(s));
                if(lstrlen(s))b->Valid();else b->Invalid();
            }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
#endif
BOOL TableSearchDialog(HWND hwnd,void*d){
#ifdef DBE
    int rez=0;
    TPropertyButton*b=new TPropertyButton("Main",HResource,"Table_Search",0);
    b->Procedure=TsrcMainProc;
    b->data=(DWORD)d;
    rez=CreatePropertySheet(hwnd,"Search in table",b,0); // Информация о переменной
    delete b;
    return rez;
#else
    return FALSE;
#endif
}

// Matrix dialog
TMatrix * __em;
BOOL scCALLBACK MatrixProc(HWND hwnd,UINT msg,WPARAM wParam, LPARAM lParam){
    switch(msg){
        case TBN_SETTEXT:{
            double value;
            CELLINFO* ci=(CELLINFO*)lParam;
            if(StrToDouble(ci->text,value)){
                __em->PUT(ci->y,ci->x,value);
                ci->ok=TRUE;
            }else ci->ok=FALSE;
        }return 0;
        case TBL_GETTEXT:{
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info->id==IDC_HLABEL){
                long p=__em->minX+info->pos;
                wsprintf(info->text,"%ld",p);
            }else{
                long p=__em->minY+info->pos;
                wsprintf(info->text,"%ld",p);
            }
        };break;
        case TBN_GETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            double v=__em->GET(ci->y,ci->x);
            sprintf(ci->text,"%G",v);
        };return 0;
        case WM_INITDIALOG:{
            char s[256];
            if (__em->minX || __em->minY)
                wsprintf(s,"Matrix :(%d) [(%ld - %ld),(%ld - %ld)]",__em->handle,
                         __em->minY,__em->dimY,__em->minX,__em->dimX);
            else
                wsprintf(s,"Matrix :(%d) [%ld,%ld]",__em->handle,__em->dimY,__em->dimX);
            SetWindowText(hwnd,s);
            TCollumn c(0,40,0);
            c._min/=2;c._max*=3;
            for(int i=0;i<__em->dimX;i++)
                SendDlgItemMessage(hwnd,	IDC_MATRIX,TB_ADDCOLLUMN,-1,LPARAM(&(c)));
            SendDlgItemMessage(hwnd,	IDC_MATRIX,TB_SETRANGEY,0,__em->dimY);
            SendDlgItemMessage(hwnd,	IDC_MATRIX,TB_SETLABEL,
                               (WPARAM)GetDlgItem(hwnd,IDC_HLABEL),
                               (LPARAM)GetDlgItem(hwnd,IDC_VLABEL));
            RECT r,_r;
            GetWindowRect(hwnd,&r);
            GetClientRect(hwnd,&_r);
            long xs=40*__em->dimX;
            long ys=CELL_H*__em->dimY;
            xs+=5*2+50+((r.right-r.left)-(_r.right-_r.left))+28;
            ys+=30+22+10+((r.bottom-r.top)-(_r.bottom-_r.top))+20;
            xs=min(xs,(r.right-r.left));
            ys=min(ys,(r.bottom-r.top));
            SetWindowPos(hwnd,0,0,0,xs,ys,SWP_NOZORDER|SWP_NOMOVE);

        }
        case WM_SIZE:{
            int delta=5;
            int h=22;
            int w=50;
            RECT r;
            GetClientRect(hwnd,&r);
            r.bottom-=30;
            SetWindowPos(GetDlgItem(hwnd,IDC_HLABEL),0,delta+w,delta,
                         r.right-(w+2*delta),h,SWP_NOZORDER);

            SetWindowPos(GetDlgItem(hwnd,IDC_VLABEL),0,delta,delta+h,
                         w,r.bottom-(h+2*delta),SWP_NOZORDER);
            SetWindowPos(GetDlgItem(hwnd,IDC_MATRIX),0,w+delta,h+delta,
                         r.right-(w+2*delta),r.bottom-(h+2*delta),SWP_NOZORDER);
            SetWindowPos(GetDlgItem(hwnd,IDOK),0,r.right-80,r.bottom+5,
                         0,0,SWP_NOZORDER|SWP_NOSIZE);
            SetWindowPos(GetDlgItem(hwnd,IDCANCEL),0,r.right-160,r.bottom+5,
                         0,0,SWP_NOZORDER|SWP_NOSIZE);

        }return 0;

        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch (command){
        case IDCANCEL:EndDialog(hwnd,0);return TRUE;
                case IDOK:EndDialog(hwnd,1);return TRUE;
            }}break;

    }
    return FALSE;
};

BOOL MatrixDialog(HWND hwnd,int matrix)
{
    __em = matrixs->GetMatrix(matrix);
    int rez = 0;
    if (__em)
    {
        rez = DialogBox(HResource, MAKEINTRESOURCE(MATRIX_DIALOG), hwnd, MatrixProc);
    }
    return rez;
}

extern TCollection *_adddir_c;
BOOL scCALLBACK AddDirProc(HWND hwnd,UINT msg,WPARAM wParam, LPARAM lParam){
    switch(msg){
        case TBL_GETTEXT:{
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info->id==IDC_HLABEL){
                switch(info->pos){
                    case 0:lstrcpy(info->text,"Class");break;
                    case 1:lstrcpy(info->text,"Used file name");break;
                    case 2:lstrcpy(info->text,"Duplicated file name");break;
                }
            }
        };break;
        case TBN_GETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            C_TYPE y=ci->y*3;
            lstrcpy(ci->text,(char*)_adddir_c->At(y+ci->x));
        };return 0;
        case WM_INITDIALOG:{
            TCollumn c[3]={TCollumn(0,100,CF_READONLY),TCollumn(0,300,CF_READONLY)
                           ,TCollumn(0,300,CF_READONLY)};
            for(int i=0;i<3;i++){
                c[i]._min/=2;c[i]._max*=2;
                SendDlgItemMessage(hwnd,	IDC_MATRIX,TB_ADDCOLLUMN,-1,LPARAM(&(c[i])));
            }
            SendDlgItemMessage(hwnd,	IDC_MATRIX,TB_SETRANGEY,0,_adddir_c->count/3);
            SendDlgItemMessage(hwnd,	IDC_MATRIX,TB_SETLABEL,
                               (WPARAM)GetDlgItem(hwnd,IDC_HLABEL),
                               (LPARAM)GetDlgItem(hwnd,IDC_VLABEL));
        }
        case WM_SIZE:{
            int delta=5;
            int h=22;
            int w=50;
            RECT r;
            GetClientRect(hwnd,&r);
            r.bottom-=50;
            SetWindowPos(GetDlgItem(hwnd,IDC_HLABEL),0,delta+w,delta,
                         r.right-(w+2*delta),h,SWP_NOZORDER);

            SetWindowPos(GetDlgItem(hwnd,IDC_VLABEL),0,delta,delta+h,
                         w,r.bottom-(h+2*delta),SWP_NOZORDER);
            SetWindowPos(GetDlgItem(hwnd,IDC_MATRIX),0,w+delta,h+delta,
                         r.right-(w+2*delta),r.bottom-(h+2*delta),SWP_NOZORDER);

            SetWindowPos(GetDlgItem(hwnd,IDOK),0,r.right-100,r.bottom+5,
                         0,0,SWP_NOZORDER|SWP_NOSIZE);

            SetWindowPos(GetDlgItem(hwnd,IDC_SAVE),0,r.right-200,r.bottom+5,
                         0,0,SWP_NOZORDER|SWP_NOSIZE);

        }return 0;

        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch (command){
        case IDC_SAVE:{
                    char file[256]="errors.txt";
                    if(FileDialog(hwnd,file,IDS_TEXT,FD_SAVE)){
                        TDOSStream st(file,TDOSStream::stCreate);
                        if(st.status==0){
                            char ss[1024];
                            sprintf(ss,"%-20s | %-40s | %s","Class name","File name of used class","File name of ignored class");
                            st.WriteDOSSTR(ss);
                            st.WriteDOSSTR("");

                            for(C_TYPE i=0;i<_adddir_c->count/3;i++){
                                char * c =(char *)_adddir_c->At(i*3);
                                char * f1=(char *)_adddir_c->At(i*3+1);
                                char * f2=(char *)_adddir_c->At(i*3+2);
                                sprintf(ss,"%-20s | %-40s | %s",c,f1,f2);
                                st.WriteDOSSTR(ss);
                            }
                        }
                    }
                };return TRUE;
                case IDCANCEL:
                case IDOK:EndDialog(hwnd,1);return TRUE;
            }}break;

    }
    return FALSE;
};

HWND extern title;
BOOL AddDirErrorDialog(HWND hwnd)
{
    if(title)ShowWindow(title,SW_HIDE);
    return DialogBox(HResource,"AddDirError",
                     hwnd,AddDirProc);

}

///--------------------------------------------------
/*
BOOL scCALLBACK LinkMiskProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
switch(msg){
    case PSP_INITPAGE:{
      TPropertyButton*b=(TPropertyButton*)lParam;
         }return TRUE;
    case PSP_CLOSE:if(wParam==1){
      TPropertyButton*b=(TPropertyButton*)lParam;
     };return TRUE;
    case PSP_VALID:{
    TPropertyButton*b=(TPropertyButton*)lParam;
    if (b->flags&PSP_INITIALIZE){
      }else {b->Valid();}
    }return TRUE;
    }
return FALSE;
};
  */

char * inputline;
char * inputtitle;
char * inputmsg;

void SetDlgItemDouble(HWND h,int id,double d){
    char text[20];
    sprintf(text,"%g",d);
    SetDlgItemText(h,id,text);
};

BOOL scCALLBACK SpaceGridProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{

            EV_COMMAND
                    switch (command){
        case IDC_CRDICON:{
                    HWND h=GetNextWindow(hwndCtl,GW_HWNDNEXT);
                    for(int i=IDC_CRD1;i<=IDC_CRD6;i++){
                        HWND _h=GetDlgItem(hwnd,i);
                        CheckDlgButton(hwnd,i,_h==h);
                    }
                }break;
                case IDC_SETMATRIX:{
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    if(b){
                        CRDSYSTEM2D&c=*(CRDSYSTEM2D*)b->pdata;
                        int m=matrixs->MCreate(0,0,2,0,2,0);
                        for(int i=0;i<3;i++)
                            for(int j=0;j<3;j++){
                                matrixs->MPut(m,i,j,c.matrix[i][j]);
                            }
                        if(MatrixDialog(hwnd,m)){
                            for(i=0;i<3;i++){
                                for(int j=0;j<3;j++){
                                    c.matrix[i][j]=matrixs->MGet(m,i,j);
                                }}
                            for(i=IDC_CRD1;i<=IDC_CRD6;i++)
                                CheckDlgButton(hwnd,i,i==IDC_CRD6);
                        }
                        matrixs->MDelete(m);
                    }
                }break;
            }
        }break;
        case WM_INITDIALOG:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            if(SetSpaceParam2d(hsp,SP_GETREADONLY,0)){
                EnableWindow(GetDlgItem(GetParent(hwnd),IDOK),0);
            }
        }break;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TGrid*grid=(TGrid*)b->dwuser1;
            HSP2D hsp=b->hsp;
            EditSubClass(hwnd,IDC_OFFSETX,0,64,1,UDF_DOUBLE|UDF_MINMAX);
            EditSubClass(hwnd,IDC_OFFSETY,0,64,1,UDF_DOUBLE|UDF_MINMAX);
            EditSubClass(hwnd,IDC_STEPX,1,64,1,UDF_DOUBLE|UDF_MINMAX);
            EditSubClass(hwnd,IDC_STEPY,1,64,1,UDF_DOUBLE|UDF_MINMAX);
            EditSubClass(hwnd,IDC_Ycrd,-32767,32767,1,UDF_DOUBLE|UDF_MINMAX);
            EditSubClass(hwnd,IDC_Xcrd,-32767,32767,1,UDF_DOUBLE|UDF_MINMAX);


            SetDlgItemDouble(hwnd,IDC_OFFSETX,grid->OffsetX);
            SetDlgItemDouble(hwnd,IDC_OFFSETY,grid->OffsetY);
            SetDlgItemDouble(hwnd,IDC_STEPX  ,grid->StepX);
            SetDlgItemDouble(hwnd,IDC_STEPY  ,grid->StepY);
            CheckDlgButton(hwnd,IDC_VISIBLE  ,grid->visible!=0);
            CheckDlgButton(hwnd,IDC_USEGRID  ,grid->use!=0);
            CRDSYSTEM2D&c=*(CRDSYSTEM2D*)b->pdata;
            if(GetCrdSystem2d(hsp,&c)){
                int crd=IDC_CRD6;
                switch(c.type){
                    case MMTEXT_COORDINATE2D:crd=IDC_CRD1;break;
                    case XRYU_COORDINATE2D  :crd=IDC_CRD2;break;
                    case XRYD_COORDINATE2D  :crd=IDC_CRD4;break;
                    case XLYU_COORDINATE2D  :crd=IDC_CRD3;break;
                    case XLYD_COORDINATE2D  :crd=IDC_CRD5;break;
                }
                CheckDlgButton(hwnd,crd,1);
                SetDlgItemDouble(hwnd,IDC_Xcrd,c.center.x);
                SetDlgItemDouble(hwnd,IDC_Ycrd,c.center.y);
            };
        }return TRUE;
        case PSP_CLOSE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TGrid*grid=(TGrid*)b->dwuser1;

            if(b->flags&PSP_INITIALIZE){
                if (wParam){// Ok
                    CRDSYSTEM2D&c=*(CRDSYSTEM2D*)b->pdata;
                    HSP2D hsp=b->hsp;
                    //	GetCrdSystem2d(hsp,&c);
                    double x1,x2,x3,x4;
                    IsDoubleValid(hwnd,IDC_OFFSETX,x1,0,64);
                    IsDoubleValid(hwnd,IDC_OFFSETY,x2,0,64);
                    IsDoubleValid(hwnd,IDC_STEPX,x3,1,64);
                    IsDoubleValid(hwnd,IDC_STEPY,x4,1,64);
                    IsDoubleValid(hwnd,IDC_Xcrd,c.center.x,-32767,32767);
                    IsDoubleValid(hwnd,IDC_Ycrd,c.center.y,-32767,32767);

                    c.type=MMTEXT_COORDINATE2D;
                    if(IsDlgButtonChecked(hwnd,IDC_CRD1))c.type=MMTEXT_COORDINATE2D;
                    if(IsDlgButtonChecked(hwnd,IDC_CRD2))c.type=XRYU_COORDINATE2D;
                    if(IsDlgButtonChecked(hwnd,IDC_CRD3))c.type=XLYU_COORDINATE2D;
                    if(IsDlgButtonChecked(hwnd,IDC_CRD4))c.type=XRYD_COORDINATE2D;
                    if(IsDlgButtonChecked(hwnd,IDC_CRD5))c.type=XLYD_COORDINATE2D;
                    if(IsDlgButtonChecked(hwnd,IDC_CRD6))c.type=MATRIX_COORDINATE2D;

                    SetCrdSystem2d(hsp,&c);
                    SetSpaceModify2d(hsp,TRUE);
                    grid->OffsetX=x1;
                    grid->OffsetY=x2;
                    grid->StepX=x3;
                    grid->StepY=x4;
                    grid->visible=(INT16)IsDlgButtonChecked(hwnd,IDC_VISIBLE);
                    grid->use=(INT16)IsDlgButtonChecked(hwnd,IDC_USEGRID);
                    grid->Update();

                }else{
                    // Cancel
                }

            };}
            return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            //	TScheme*_scheme=(TScheme*)b->dwuser1;

            if (b->flags&PSP_INITIALIZE){
                double x1,x2,x3,x4,x,y;
                if (
                    IsDoubleValid(hwnd,IDC_OFFSETX,x1,0,8)&&
                    IsDoubleValid(hwnd,IDC_OFFSETY,x2,0,8)&&
                    IsDoubleValid(hwnd,IDC_STEPX,x3,1,8)&&
                    IsDoubleValid(hwnd,IDC_STEPY,x4,1,8)&&
                    IsDoubleValid(hwnd,IDC_Ycrd,x,-32767,32767)&&
                    IsDoubleValid(hwnd,IDC_Xcrd,x,-32767,32767)
                    ){
                    b->Valid();
                }else b->Invalid();

            }else {b->Valid();}
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK SpaceOpenProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TSpaceData*sd=new TSpaceData(b->hsp);
            int style=IDC_DEFAULT;
            if (sd->style&SWF_MDI)style=IDC_STYLE1;
            if (sd->style&SWF_DIALOG)style=IDC_STYLE2;
            if (sd->style&SWF_POPUP)style=IDC_STYLE3;
            CheckDlgButton(hwnd,style,1);
            if (sd->style&SWF_MODAL)CheckDlgButton(hwnd,IDC_MODAL,1);
            // Закончили стиль

            if (sd->style&SWF_AUTOSCROLL)CheckDlgButton(hwnd,IDC_AUTOSCROLL,1);
            if (sd->style&SWF_HSCROLL)CheckDlgButton(hwnd,IDC_HSCROLL,1);
            if (sd->style&SWF_VSCROLL)CheckDlgButton(hwnd,IDC_VSCROLL,1);
            if (sd->style&SWF_NORESIZE)CheckDlgButton(hwnd,IDC_NOCHANGE,1);
            if (sd->style&SWF_AUTOORG)CheckDlgButton(hwnd,IDC_AUTOORG,1);

            if(sd->style&SWF_MAXIMIZE)style=IDC_WINMAX;
            else{
                if(sd->style&SWF_MINIMIZE)style=IDC_WINMIN;else{
                    if (sd->style&SWF_SPACESIZE)style=IDC_SPACEEXTENT;
                    else{
                        if (sd->sizex>0 && sd->sizey>0){
                            style=IDC_CUSTOMSIZE;
                            SetDlgItemInt(hwnd,IDC_SIZEX,sd->sizex,0);
                            SetDlgItemInt(hwnd,IDC_SIZEY,sd->sizey,0);
                        }else style=IDC_DEFAULTSIZE;
                    }}
            }
            EditSubClass(hwnd,IDC_SIZEX,1,32767,1,UDF_MINMAX);
            EditSubClass(hwnd,IDC_SIZEY,1,32767,1,UDF_MINMAX);

            CheckDlgButton(hwnd,style,1);
            b->dwuser1=(DWORD)sd;
        }return TRUE;
        case PSP_CLOSE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            //	  TScheme*_scheme=(TScheme*)b->dwuser1;
            if(b->flags&PSP_INITIALIZE){
                TSpaceData*sd=(TSpaceData*)b->dwuser1;
                if (wParam){// Ok
                    ChkStyle(hwnd,IDC_AUTOSCROLL,sd->style,SWF_AUTOSCROLL);
                    ChkStyle(hwnd,IDC_HSCROLL,sd->style,SWF_HSCROLL);
                    ChkStyle(hwnd,IDC_VSCROLL,sd->style,SWF_VSCROLL);

                    ChkStyle(hwnd,IDC_NOCHANGE,sd->style,SWF_NORESIZE);
                    ChkStyle(hwnd,IDC_AUTOORG,sd->style,SWF_AUTOORG);
                    ChkStyle(hwnd,IDC_MODAL,sd->style,SWF_MODAL);

                    ChkStyle(hwnd,IDC_WINMAX,sd->style,SWF_MAXIMIZE);
                    ChkStyle(hwnd,IDC_WINMIN,sd->style,SWF_MINIMIZE);

                    DWORD type=0;
                    if(IsDlgButtonChecked(hwnd,IDC_STYLE1))type=SWF_MDI;
                    if(IsDlgButtonChecked(hwnd,IDC_STYLE2))type=SWF_DIALOG;
                    if(IsDlgButtonChecked(hwnd,IDC_STYLE3))type=SWF_POPUP;
                    if(IsDlgButtonChecked(hwnd,IDC_DEFAULT))type=0;
                    sd->style&=~SWF_WINTYPE;
                    sd->style|=type;

                    ChkStyle(hwnd,IDC_SPACEEXTENT,sd->style,SWF_SPACESIZE);
                    if (sd->style&(SWF_SPACESIZE|SWF_MINIMIZE|SWF_MAXIMIZE)){
                        sd->sizex=0;sd->sizey=0;
                    }else{
                        BOOL err,err1;
                        sd->sizex=GetDlgItemInt(hwnd,IDC_SIZEX,&err,0);
                        sd->sizey=GetDlgItemInt(hwnd,IDC_SIZEY,&err1,0);
                        if (err && err1 && sd->sizex>0 && sd->sizey>0){

                        }else {sd->sizex=0;sd->sizey=0;}
                    }
                    sd->Update();
                }else{
                    // Cancel
                }
                delete sd;
            };}
            return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            //TSpaceData*sd=(TSpaceData*)b->dwuser1;
            if (b->flags&PSP_INITIALIZE){


                // else b->Invalid();
            }else {b->Valid();}
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDC_DEFAULTSIZE:
                case IDC_SPACEEXTENT:SetDlgItemText(hwnd,IDC_SIZEX,"");
                    SetDlgItemText(hwnd,IDC_SIZEY,"");
                    CheckDlgButton(hwnd,IDC_CUSTOMSIZE,0);
                    CheckDlgButton(hwnd,IDC_DEFAULTSIZE,wParam==IDC_DEFAULTSIZE);
                    CheckDlgButton(hwnd,IDC_SPACEEXTENT,wParam==IDC_SPACEEXTENT);
                    return FALSE;
                case IDC_SIZEX:
                case IDC_SIZEY:CheckDlgButton(hwnd,IDC_CUSTOMSIZE,1);
                    CheckDlgButton(hwnd,IDC_SPACEEXTENT,0);
                    CheckDlgButton(hwnd,IDC_DEFAULTSIZE,0); break;
            }
        }return 0;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}

DWORD __GetLayers(HWND hwnd)
{
    DWORD layers=0;
    for(int i=0;i<32;i++)
    {
        layers |= (((DWORD)(IsDlgButtonChecked(hwnd,100+i)==0)) << i);
    }
    return layers;
}

void __CheckMains(HWND hwnd,DWORD layers){
    int mode=2;
    if ((layers&0xff0000ul)==0xff0000ul)mode=0;
    if ((layers&0xff0000ul)==0l)mode=1;
    CheckDlgButton(hwnd,IDC_OBJECTS,mode);
    mode=2;
    if ((layers&0xff000000ul)==0xff000000ul)mode=0;
    if ((layers&0xff000000ul)==0l)mode=1;
    CheckDlgButton(hwnd,IDC_LINKS,mode);
    mode=2;
    if ((layers&0xfffful)==0xfffful)mode=0;
    if ((layers&0xfffful)==0l)mode=1;
    CheckDlgButton(hwnd,IDC_GRAPHICS,mode);
}

void __CheckAll(HWND hwnd,DWORD layers){
    for(int i=0;i<32;i++)
        CheckDlgButton(hwnd,100+i,(layers& (1l<<i))==0);
}

BOOL scCALLBACK SpaceLayersProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
#ifdef WIN32
        case WM_HELP:{
            HELPINFO * lphi = (LPHELPINFO) lParam;
            if(lphi->iCtrlId>=IDC_LAYERS && lphi->iCtrlId<=IDC_LAYERS+32){
                HELP(IDC_LAYERS,(int)lphi->MousePos.x,(int)lphi->MousePos.y,(HWND)lphi->hItemHandle);
                return TRUE;
            }
        }break;
#endif
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            DWORD layers=SetSpaceParam2d(hsp,SP_GETLAYERS,0);
            __CheckMains(hwnd,layers);
            __CheckAll(hwnd,layers);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    if ((command>=100 && command<132) ||
                        ((IDC_GRAPHICS==command) ||(IDC_LINKS==command)|| (IDC_OBJECTS==command)
                         )){
                int bc=IsDlgButtonChecked(hwnd,command);
                int c=(bc==1);
                if (command>=100 && command<132){
                    DWORD layers=__GetLayers(hwnd);
                    if (!c)layers|=(1l<<(command-100));
                    else layers&=~(1l<<(command-100));
                    __CheckMains(hwnd,layers);
                }else{
                    DWORD layers=__GetLayers(hwnd);
                    switch(command){
                        case IDC_GRAPHICS:if (c)layers&=~0xfffful;else layers|=0xfffful;break;
                        case IDC_LINKS:   if (c)layers&=~0xff000000ul;else layers|=0xff000000ul;break;
                        case IDC_OBJECTS: if (c)layers&=~0xff0000ul;else layers|=0xff0000ul;break;
                        default:return 0;
                    }
                    __CheckMains(hwnd,layers);
                    __CheckAll(hwnd,layers);
                }
                break;
            }
        }return 0;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                HSP2D hsp=b->hsp;
                DWORD layers=__GetLayers(hwnd);
                SetSpaceParam2d(hsp,SP_SETLAYERS,layers);
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            {b->Valid();}
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK SpaceMiskProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            DWORD flags=SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
            if ((flags&SF_DONTSELECT))CheckDlgButton(hwnd,IDC_NOSELECTSP,1);
            if ((flags&SF_HOOKMOUSE))CheckDlgButton(hwnd,IDC_HOOKCONTROLS,1);


        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                HSP2D hsp=b->hsp;
                DWORD flags=SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
                DWORD _flags=flags;

                if (IsDlgButtonChecked(hwnd,IDC_HOOKCONTROLS))flags|=SF_HOOKMOUSE;
                else flags&=~SF_HOOKMOUSE;

                if (IsDlgButtonChecked(hwnd,IDC_NOSELECTSP))flags|=SF_DONTSELECT;
                else flags&=~SF_DONTSELECT;
                if (flags!=_flags)
                    SetSpaceParam2d(hsp,SP_SETSPACEFLAG,flags);
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            {b->Valid();}
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL scCALLBACK SpaceToolsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){

    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HWND tsw=GetDlgItem(hwnd,IDC_TREEINFO);
            char s[256];
            LoadStr(IDS_str29,s);

            TreeItemData td;
            td.parent = NULL;
            td.IsLeaf = 0;
            td.IsImage = 0;
            td.datatype = TREE_TOOLS;
            td.data=(DWORD)hsp;
            td.text=s; //"Инструменты"

            SendMessage(tsw,TVN_INSERTITEM,0,(long)&td);
            VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
            sw->NotifyWnd=mainwindow->HWindow;
            ::SendMessage(tsw,TV_SETSHIFT,1,0);
            ::SendMessage(tsw,TV_EXPAND,1,td.data);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                //TPropertyButton*b=(TPropertyButton*)lParam;
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            {b->Valid();}
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL scCALLBACK SpaceObjectsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case CM_PROPERTIESobj2d:return 0;
                case CM_EDITDELETEobj2d:return 0;
            }
        }
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(!b)return 0;
            HSP2D hsp=b->hsp;
            HWND tsw=GetDlgItem(hwnd,IDC_TREEINFO);
            VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
            TreeItemData td;
            td.parent = NULL;
            td.IsLeaf = 0;
            td.IsImage = 0;
            td.datatype = TREE_OBJECTS2D;
            td.data=(DWORD)hsp;
            char buf[256];
            LoadString(HResource,IDS_str34,buf,sizeof(buf));
            td.text=buf;
            ::SendMessage(tsw,TVN_INSERTITEM,0,(long)&td);
            sw->NotifyWnd=mainwindow->HWindow;
            ::SendMessage(sw->NotifyWnd,TVN_GETITEMINFO,(WPARAM)tsw,MAKELONG(hsp,0));
            ::SendMessage(tsw,TV_SETSHIFT,1,0);
            ::SendMessage(tsw,TV_EXPAND,1,td.data);
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL SpaceDialog(HWND hwnd,HSP2D hsp,int mode,TGrid *grid,PClass _class,PObject /*_pobject*/){
    //  TPropertyButton*b=new TPropertyButton(IDS_str101 ,HResource,"Obj_Brush",Page_options___Background);
    TPropertyButton*b=new TBrushTProperty(IDS_str101,Page_options___Background);
    TPropertyButton*b1=b;
    (HOBJ2D)b1->di.d1=GetBkBrush2d(hsp);
    //  b1->Procedure=ObjBrushProc;

    if (_class){
        TScheme*sc=NULL;
        if (_class->scheme && _class->scheme->Loaded() && _class->scheme->GetSpace()==hsp)sc=_class->scheme;
        if (_class->image && _class->image->Loaded() && _class->image->GetSpace()==hsp)sc=_class->image;

        if (sc){
        }
    }
    CRDSYSTEM crd;
    if (grid){
        b1=new TPropertyButton(IDS_str30,HResource,"Space_Grid",Page_options___Coordinates); //"Сетка"
        b1->Procedure=SpaceGridProc;
        b1->dwuser1=(DWORD)grid;
        b1->pdata=&crd;
        b->Insert(b1);
    }
    b1=new TPropertyButton(IDS_str31,HResource,"Space_Hyper",Page_options___Window);//"Окно"
    b1->Procedure=SpaceOpenProc;
    b->Insert(b1);
    b1=new TPropertyButton(IDS_str32,HResource,"Space_Misc",Page_options___Misc); //"Разное"
    b1->Procedure=SpaceMiskProc;
    b->Insert(b1);
    b1=new TPropertyButton(IDS_str33,HResource,"Space_Layers",Page_options___Layers);
    //  b1=new TPropertyButton("Layers",HResource,"Space_Misc");
    b1->Procedure=SpaceLayersProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str29,HResource,"Space_Tools",Page_options___Tools);//"Инструменты"
    b1->Procedure=SpaceToolsProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str34,HResource,"Space_Tools",Page_options___Objects);//"Объекты"
    b1->Procedure=SpaceObjectsProc;
    b->Insert(b1);

    b1=b;
    while (b1){
        b1->hsp=hsp;
        b1=b1->next;
    };

    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str35),b,mode);
    SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
    delete b;
    return rez;
};

void AddMethod(HWND hwnd,int id,char *str,TProject * prj){
    HWND list=GetDlgItem(hwnd,id);
    if(prj)SendMessage(list,CB_ADDSTRING,0,(LPARAM)"");
    char s[1024];
    GetPrivateProfileString("METHOD",str,"NONE",s,sizeof(s),ini_file);
    char *ps=s;
    char *_ps=ps;
    BOOL notquit=TRUE;
    do{
        if (!(*ps) || (*ps=='|')){
            notquit=(*ps);
            *ps=0;
            SendMessage(list,CB_ADDSTRING,0,(LPARAM)_ps);
            _ps=ps+1;

        }
        ps++;
    }while (notquit);
    char equ[64]="equ_";lstrcat(equ,str);

    if(prj){
        prj->GetVarS(equ,s);
    }else{
        sc_vars->GetVarS(equ,s);
    }
    INT16 index=(INT16)SendMessage(list,CB_FINDSTRINGEXACT,0,(LPARAM)s);
    SendMessage(list,CB_SETCURSEL,index,0);
}
void GetMethod(HWND hwnd,int id,char *str,TProject * prj){
    HWND list=GetDlgItem(hwnd,id);
    INT16 index=SendMessage(list,CB_GETCURSEL,index,0);
    char s[256]="";
    SendMessage(list,CB_GETLBTEXT,index,(LPARAM)s);

    char equ[64]="equ_";lstrcat(equ,str);
    if(prj){
        if(lstrlen(s)==0){prj->DeleteVar(equ);
            if(id==IDC_NONLINEMETOD){
                prj->DeleteVar("equ_nl_maxstep");
                prj->DeleteVar("equ_nl_sense");
                prj->DeleteVar("equ_nl_error");
            }
        }else prj->SetVarS(equ,s);
    }else{
        SCSetScVarS(equ,s);
    }
};

BOOL scCALLBACK EnvMethodProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TProject* prj=(TProject*)b->pdata;
            AddMethod(hwnd,IDC_LINEMETOD,"linear",prj);
            AddMethod(hwnd,IDC_NONLINEMETOD,"nonlinear",prj);
            AddMethod(hwnd,IDC_DIFFMETOD,"diff",prj);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                TProject* prj=(TProject*)b->pdata;
                GetMethod(hwnd,IDC_LINEMETOD,"linear",prj);
                GetMethod(hwnd,IDC_NONLINEMETOD,"nonlinear",prj);
                GetMethod(hwnd,IDC_DIFFMETOD,"diff",prj);

            }return TRUE;
        case WM_COMMAND:{
            TPropertyButton*b1=GetDlgPropertyButton(hwnd);
            if(b1){
                EV_COMMANDsimply
                        switch (command){
        case IDC_EQU1SETUP:LinearDialog(hwnd,(TProject*)b1->pdata);return 0;;
                    case IDC_EQU2SETUP:NonLinearDialog(hwnd,(TProject*)b1->pdata);return 0;
                }
            }
        }break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
int extern peridle;
BOOL scCALLBACK EnvCalcProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_TIMERSTEP:if(wNotifyCode==EN_SETFOCUS){
                        CheckDlgButton(hwnd,IDC_BYIDLE,0);
                        CheckDlgButton(hwnd,IDC_BYTIMER,1);
                        CheckDlgButton(hwnd,IDC_HARDMODE,0);
                        CheckDlgButton(hwnd,IDC_ASENV,0);
                    };break;
            }
        }break;
        case PSP_INITPAGE:{
            EditSubClass(hwnd,IDC_TIMERSTEP,1,0,1,UDF_MIN);

            TPropertyButton*b=(TPropertyButton*)lParam;
            TProject* prj=(TProject*)b->pdata;
            int per,mmode,dtimer=20,runmode=-1;
            if(prj){
                per = prj->GetVarDef("PerIdle",-1);
                runmode= prj->GetVarDef("run_mode",-1);
                mmode = prj->GetVarDef("MathMode",-1);
                dtimer = prj->GetVarDef("runtimer",20);
            }else{
                CheckDlgButton(hwnd,IDC_PERFOMANCE,SC_GetScVar("status_perfomance",0));
                CheckDlgButton(hwnd,IDC_STEPS,SC_GetScVar("status_steps",0));
                per=SC_GetScVar("PerIdle",1);
                mmode=SC_GetScVar("MathMode",0);
                dtimer=SC_GetScVar("runtimer",20);
                runmode=SC_GetScVar("run_mode",0);
            }
            if(mmode==-1){
                CheckDlgButton(hwnd,IDC_ASENV1,1);
            }else  CheckRadioButton(hwnd,IDC_MATHMODE1,IDC_MATHMODE4,IDC_MATHMODE1+mmode);
            //CheckDlgButton(hwnd,IDC_BYIDLE,1);
            int k=0;
            for(int i=1;i<20;i++)
            {char s[10];wsprintf(s,"%d",i);
                if(i+1==per)k=i;
                SendDlgItemMessage(hwnd,IDC_PERIDLE,CB_ADDSTRING,0,(LPARAM)s);
            }
            SendDlgItemMessage(hwnd,IDC_PERIDLE,CB_SETCURSEL,k,0);

            switch(runmode){
                case -1:CheckDlgButton(hwnd,IDC_ASENV,1);break;
                case 0:CheckDlgButton(hwnd,IDC_BYIDLE,1);
                    CheckDlgButton(hwnd,IDC_BYTIMER,0);
                    break;
                case 1:CheckDlgButton(hwnd,IDC_BYIDLE,0);
                    CheckDlgButton(hwnd,IDC_BYTIMER,1);
                    break;
            }
            SetDlgItemInt(hwnd,IDC_TIMERSTEP,dtimer,1);
        }return TRUE;
        case PSP_VALID:if (wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                if (b->flags&PSP_INITIALIZE){
                    INT16 r;
                    if(IsIntValid(hwnd,IDC_TIMERSTEP,r,1,16384))b->Valid();else b->Invalid();
                }
            }break;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                TProject* prj=(TProject*)b->pdata;
                INT32 mmode=0;
                INT32 mode=0;
                if(!prj){
                    int mode=0;
                    SCSetScVarI("status_perfomance",IsDlgButtonChecked(hwnd,IDC_PERFOMANCE));
                    SCSetScVarI("status_steps",IsDlgButtonChecked(hwnd,IDC_STEPS));
                }
                for(int i=IDC_MATHMODE1;i<=IDC_MATHMODE4;i++){
                    if(IsDlgButtonChecked(hwnd,i)){mmode=i-IDC_MATHMODE1;}
                }
                if(IsDlgButtonChecked(hwnd,IDC_ASENV1))mmode=-1;

                INT32 per= SendDlgItemMessage(hwnd,IDC_PERIDLE,CB_GETCURSEL,0,0)+1;
                peridle=(int)per;

                if(IsDlgButtonChecked(hwnd,IDC_ASENV))mode=-1;
                if(IsDlgButtonChecked(hwnd,IDC_BYTIMER))mode=1;
                if(IsDlgButtonChecked(hwnd,IDC_HARDMODE))mode=2;
                BOOL error;
                int dtimer=GetDlgItemInt(hwnd,IDC_TIMERSTEP,&error,0);

                if(prj){
                    if(mode==-1){
                        prj->DeleteVar("run_mode");
                        prj->DeleteVar("PerIdle");
                        prj->DeleteVar("runtimer");
                    }else{
                        prj->SetVarI("run_mode",mode);
                        prj->SetVarI("PerIdle",per);
                        prj->SetVarI("runtimer",dtimer);
                    }
                    if(mmode==-1)prj->DeleteVar("MathMode");else prj->SetVarI("MathMode",mmode);
                }else{
                    SCSetScVarI("run_mode",mode);
                    SCSetScVarI("PerIdle",per);
                    SCSetScVarI("runtimer",dtimer);
                    SCSetScVarI("MathMode",mmode);
                }
            }return TRUE;
    }

    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK ProjectInfoProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TProject* prj=(TProject*)b->pdata;
            if(prj && prj->readonly){
                EnableWindow(GetDlgItem(GetParent(hwnd),IDOK),0);
            }
        }return TRUE;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TProject* prj=(TProject*)b->pdata;
            if(prj){
                char s[256];
                char m[256];
                TMemoryStream ms(TRUE);
                if(prj->vars)prj->vars->ToText(ms,1);
                ms.WriteWord(0);
                SetWindowText(GetDlgItem(hwnd,IDC_PRJINFO),(char*)ms.base);
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};


BOOL scCALLBACK EnvPreloadProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
#ifdef DBE
            CheckDlgButton(hwnd,IDC_PRELOADidapi,SC_GetScVar("preload_idapi",0));
#else
            EnableWindow(GetDlgItem(hwnd,IDC_PRELOADidapi),0);
#endif
            CheckDlgButton(hwnd,IDC_PRELOADvideo,SC_GetScVar("preload_video",0));
            CheckDlgButton(hwnd,IDC_PRELOADcompiler,SC_GetScVar("preload_compiler",0));
            CheckDlgButton(hwnd,IDC_PRELOAD3d,SC_GetScVar("preload_3d",0));
            CheckDlgButton(hwnd,IDC_DISABLESCR,SC_GetScVar("DisableScrSaver",1));
            CheckDlgButton(hwnd,IDC_MULTIRUN,SC_GetScVar("AllowMultiple",0));
#ifndef WIN32
            EnableWindow(GetDlgItem(hwnd,IDC_MULTIRUN),0);
#endif
            int mode=SCGetScVar("OnStartup",0);
            HWND combo=GetDlgItem(hwnd,IDC_STARTUP);
            int i=IDS_STARTUP;
            char s[256];
            while(LoadString(HResource,i,s,sizeof(s))){
                SendMessage(combo,CB_ADDSTRING,0,(LONG)s);
                i++;
            }
            SendMessage(combo,CB_SETCURSEL,mode,0);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){

                SCSetScVarI("preload_idapi",   IsDlgButtonChecked(hwnd,IDC_PRELOADidapi));
                SCSetScVarI("preload_video",   IsDlgButtonChecked(hwnd,IDC_PRELOADvideo));
                SCSetScVarI("preload_compiler",IsDlgButtonChecked(hwnd,IDC_PRELOADcompiler));
                SCSetScVarI("preload_3d",      IsDlgButtonChecked(hwnd,IDC_PRELOAD3d));
                SCSetScVarI("DisableScrSaver", IsDlgButtonChecked(hwnd,IDC_DISABLESCR));
#ifdef WIN32
                SCSetScVarI("AllowMultiple", IsDlgButtonChecked(hwnd,IDC_MULTIRUN));
#endif
                SCSetScVarI("OnStartup",SendDlgItemMessage(hwnd,IDC_STARTUP,CB_GETCURSEL,0,0));

            }return TRUE;
    }

    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
#ifndef RUNTIME
BOOL scCALLBACK EnvVarsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{

            CheckDlgButton(hwnd,IDC_ONESTEPVARS   ,SC_GetScVar("vars_1step",1));
            CheckDlgButton(hwnd,IDC_ENABLESETVARS ,SC_GetScVar("vars_logset",1));
            CheckDlgButton(hwnd,IDC_RESETONDEF    ,SC_GetScVar("vars_setondef",1));
            CheckDlgButton(hwnd,IDC_SAVEVARS	    ,SC_GetScVar("vars_save",1));
            CheckDlgButton(hwnd,IDC_PRELOADVRAS	 ,SC_GetScVar("vars_preload",1));

        }return TRUE;
        case PSP_CLOSE:if(wParam==1){

                SCSetScVarI("vars_1step",     IsDlgButtonChecked(hwnd,IDC_ONESTEPVARS));
                SCSetScVarI("vars_logset",    IsDlgButtonChecked(hwnd,IDC_ENABLESETVARS));
                SCSetScVarI("vars_setondef",  IsDlgButtonChecked(hwnd,IDC_RESETONDEF));
                SCSetScVarI("vars_save",      IsDlgButtonChecked(hwnd,IDC_SAVEVARS));
                SCSetScVarI("vars_preload",   IsDlgButtonChecked(hwnd,IDC_PRELOADVRAS));

            }return TRUE;
    }

    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK EnvSecProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    static int ids[]={IDC_USERNAME,IDC_ORGANISATION,IDC_EMAIL,IDC_ADRESS,IDC_TELEPHON,IDC_USERADD};

    switch(msg){
        case PSP_INITPAGE:{
            /*
    UINT32 f=SCGetScVar("security_opt",0);
    CheckDlgButton(hwnd,IDC_CANTIMAGES,(f&1)!=0);
    CheckDlgButton(hwnd,IDC_CANTLIBS,(f&2)!=0);
    CheckDlgButton(hwnd,IDC_READONLY,(f&4)!=0);
    CheckDlgButton(hwnd,IDC_NETWORK,(f&8)!=0);
    */
            char s[256];
            for(int i=0;i<6;i++){
                SCGetScVarS(user_id_names[i],s);
                SetDlgItemText(hwnd,ids[i],s);
            }
            CheckDlgButton(hwnd,IDC_COPYATTRIBUTE,SCGetScVar("user_copy",1));

            if(SCGetScVarS("library_password",s)){
                Decode(s);
                SetDlgItemText(hwnd,IDC_PASSWORD,s);
            }
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                UINT32 no=0;
                /*
     if(IsDlgButtonChecked(hwnd,IDC_CANTIMAGES))no|=1;
     if(IsDlgButtonChecked(hwnd,IDC_CANTLIBS))no|=2;
     if(IsDlgButtonChecked(hwnd,IDC_READONLY))no|=4;
     if(IsDlgButtonChecked(hwnd,IDC_NETWORK))no|=8;
     UINT32 f=SCGetScVar("security_opt",0);
      if(f!=no){
       SetScVarI("security_opt",no);
      }
     */
                SCSetScVarI("user_copy",IsDlgButtonChecked(hwnd,IDC_COPYATTRIBUTE));
                char s[256];
                for(int i=0;i<6;i++){
                    if(SendDlgItemMessage(hwnd,ids[i],EM_GETMODIFY,0,0)){
                        GetDlgItemText(hwnd,ids[i],s,sizeof(s));
                        char ss[256];
                        SCGetScVarS(user_id_names[i],ss);
                        if(lstrcmpi(ss,s)){
                            SCSetScVarS(user_id_names[i],s);
                        }

                    }}
                if(SendDlgItemMessage(hwnd,IDC_PASSWORD,EM_GETMODIFY,0,0)){
                    GetDlgItemText(hwnd,IDC_PASSWORD,s,sizeof(s));
                    Encode(s);
                    SCSetScVarS("library_password",s);
                }
            }return TRUE;
    }

    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK Env2dProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            UINT32 f=SCGetScVar("points_in_circle",32);
            HWND c=GetDlgItem(hwnd,IDC_POINTS);
            for(int i=3;i<48;i++){
                char s[5];
                wsprintf(s,"%d",i);
                SendMessage(c,CB_ADDSTRING,0,(LPARAM)s);
            }
            char s[256];
            wsprintf(s,"%d",(int)f);
            SetWindowText(c,s);
            UINT32 f2d=SCGetScVar("editor2d_opt",0);
            CheckDlgButton(hwnd,IDC_HAND,(f2d&2));
            if(SC_GetScVarS("template_vdr2d",s))SetDlgItemText(hwnd,IDC_2D_TPL_VDR,s);
            if(SC_GetScVarS("template_scheme2d",s))SetDlgItemText(hwnd,IDC_2D_TPL_SCHEME,s);
            if(SC_GetScVarS("template_image2d",s))SetDlgItemText(hwnd,IDC_2D_TPL_IMAGE,s);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDC_2D_TPL_IMAGEb :
                case IDC_2D_TPL_SCHEMEb:
                case IDC_2D_TPL_VDRb   :{
                    int id=0;
                    switch(command){
                        case IDC_2D_TPL_IMAGEb :id=IDC_2D_TPL_IMAGE;break;
                        case IDC_2D_TPL_SCHEMEb:id=IDC_2D_TPL_SCHEME;break;
                        case IDC_2D_TPL_VDRb   :id=IDC_2D_TPL_VDR;break;
                    }
                    char s[256];
                    GetDlgItemText(hwnd,id,s,sizeof(s));
                    if(FileDialog(hwnd,s,IDS_VDR,0)){
                        SetDlgItemText(hwnd,id,s);
                    }
                }return TRUE;
            }
        }break;
        case PSP_VALID:if (wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                if(b->flags&PSP_INITIALIZE){
                    BOOL lpfTranslated;
                    UINT f=GetDlgItemInt(hwnd,IDC_POINTS,&lpfTranslated,0);
                    if(lpfTranslated){
                        SCSetScVarI("points_in_circle",f);
                        b->Valid();
                        UINT32 _f2d=SCGetScVar("editor2d_opt",0);
                        UINT32 f2d=0;
                        if(IsDlgButtonChecked(hwnd,IDC_HAND))f2d|=2;
                        if(f2d!=_f2d){
                            SCSetScVarI("editor2d_opt",f2d);
                        };
                        char s[256];
                        GetDlgItemText(hwnd,IDC_2D_TPL_VDR,s,sizeof(s));SCSetScVarS("template_vdr2d",s);
                        GetDlgItemText(hwnd,IDC_2D_TPL_SCHEME,s,sizeof(s));SCSetScVarS("template_scheme2d",s);
                        GetDlgItemText(hwnd,IDC_2D_TPL_IMAGE,s,sizeof(s));SCSetScVarS("template_image2d",s);

                    }else {b->Invalid();

                        MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG4),_MSG_ERROR,MB_OK|MB_ICONINFORMATION);
                    }
                }
            }return TRUE;
    }

    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
#endif // runtime
BOOL scCALLBACK LangProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TWaitCursor w(hwnd);
            TPropertyButton*b=(TPropertyButton*)lParam;
            TCollection * items=new TCollection(10,10);
            b->data=(UINT32)items;
            HWND list=GetDlgItem(hwnd,IDC_LANGLIST);
            TFileIterator fcls(sc_path,
                   #ifdef WIN32
                               "r????s32.dll"
                   #else
                               "r????s16.dll"
                   #endif
                               );
            while (fcls.GetNext()){
                char * s=fcls.GetFileName();
                HINSTANCE hr=LoadLibrary(s);
#ifndef WIN32
                if(hr<HINSTANCE_ERROR)hr=0;
#endif
                if(hr){
                    items->Insert(NewStr(s));
                    char a[256];
                    LoadString(hr,IDS_str194,a,sizeof(a));
                    FreeLibrary(hr);
                    SendMessage(list,LB_ADDSTRING,0,(LPARAM)a);
                }
            }

            char a[256];
            LoadString(HResource,IDS_str194,a,sizeof(a));
            int i=(int)SendMessage(list,LB_FINDSTRINGEXACT,-1,(LPARAM)a);
            SendMessage(list,LB_SETCURSEL,i,0);

        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                TCollection * items=(TCollection *)b->data;
                HWND list=GetDlgItem(hwnd,IDC_LANGLIST);
                char aa[256]="",a[256]="";
                C_TYPE i=(C_TYPE)SendMessage(list,LB_GETCURSEL,i,0);
                SendMessage(list,LB_GETTEXT,i,(LPARAM)aa);
                LoadString(HResource,IDS_str194,a,sizeof(a));
                if(lstrcmpi(a,aa)){
                    MSGBOX(hwnd,MAKEINTRESOURCE(IDS_str196),_MSG_INFO,MB_OK|MB_ICONINFORMATION);
                    char * a=(char*)items->At(i);
                    WritePrivateProfileString("preferences","resource",a,ini_file);
                };
                for(i=0;i<items->count;i++)
                {
                    char * a=(char*)items->At(i);
                    delete a;
                }
                items->DeleteAll();
                delete items;
            }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK EnvLibProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            CheckDlgButton(hwnd,   IDC_CANCYR,SC_GetScVar("CanUseCyr",0));
            // CheckDlgButton(hwnd,IDC_NOTHING,1);
            TLibrary *lib =GetParentLibrary();
            for(C_TYPE i=0;i<lib->items->count;i++){
                char s[256];
                TLibrary *_lib =(TLibrary *)lib->items->At(i);
                if(
                   (_lib->flags&CLF_LIBRARY) &&
                   ((_lib->flags&(CLF_ADDLIB|CLF_PROJECT))==0l)
                   ){
                    _lib->GetPath(s);
                    SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_ADDSTRING,0,(LPARAM)s);
                } }
            i=1;
            do{
                char var[64];
                wsprintf(var,"addlib%d",i);
                char s[256];
                if(SC_GetScVarS(var,s)){
                    SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_ADDSTRING,0,(LPARAM)s);
                }else break;
                i++;
            }while(1);

        }return TRUE;
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_LOADLIBS:if(wNotifyCode==LBN_SELCHANGE){
m1:
                        int c=SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_GETCURSEL,0,0);
                        EnableWindow(GetDlgItem(hwnd,IDC_DELLIB),(c>0));
                        EnableWindow(GetDlgItem(hwnd,IDC_EDITLIB),(c>0));
                        break;
                    };
                    if(wNotifyCode!=LBN_DBLCLK) break;
                case IDC_EDITLIB:{
                    int c=SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_GETCURSEL,0,0);
                    char s[256];
                    if(c>0){
                        SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_GETTEXT,c,(LPARAM)s);
                        if(ChoseDirDialog(hwnd,s,"Path to library",1)){
                            //if(InputBox(hwnd,"Stratum","Path to lib",s)){
                            SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_DELETESTRING,c,0);
                            SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_INSERTSTRING,c,(LPARAM)s);
                        }}
                }break;
                case IDC_DELLIB:{
                    int c=SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_GETCURSEL,0,0);
                    if(c>0){
                        SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_DELETESTRING,c,0);
                    }
                    goto m1;
                }break;

                case IDC_ADDLIB:{
                    char s[256];
                    LoadString(HResource,IDS_str103,s,sizeof(s));
                    if(ChoseDirDialog(hwnd,s,"Path to new library",1)){
                        //		if(InputBox(hwnd,"Stratum","Path to new lib",s)){
                        if(SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)s)==LB_ERR)
                            SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_ADDSTRING,0,(LPARAM)s);
                    }
                }return TRUE;
            }
        }break;
        case PSP_CLOSE:if(wParam==1){
                SCSetScVarI("CanUseCyr", IsDlgButtonChecked(hwnd,IDC_CANCYR));
                int c=SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_GETCOUNT,0,0);
                char s[256],old[256];
                char var[64];
                char * _cmd="addlib%d";
                for(int i=1;i<c;i++){
                    SendDlgItemMessage(hwnd,IDC_LOADLIBS,LB_GETTEXT,i,(LPARAM)s);

                    wsprintf(var,_cmd,i);
                    if(SC_GetScVarS(var,old)){
                        if(!lstrcpy(old,s))break;
                    }
                    SCSetScVarS(var,s);
                };
                do{ // удаляем
                    wsprintf(var,_cmd,i);
                    char s[256];
                    if(SC_GetScVarS(var,s)){
                        SCDeleteScVar(var);
                    }else break;
                    i++;
                }while(1);
            }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

// Далее для синтаксического редактора
#ifndef RUNTIME
void SynataxSchemes(HWND hwnd,char * current){
    char sp[256];
    SendDlgItemMessage(hwnd,IDC_SCHEMES,CB_RESETCONTENT,0,0);
    LoadStr(IDS_str4,sp);
    SendDlgItemMessage(hwnd,IDC_SCHEMES,CB_ADDSTRING,0,(LONG)sp);
    GetTemplateDir(sp);
    TFileIterator fcls(sp,"*.syn");
    while (fcls.GetNext()){
        SendDlgItemMessage(hwnd,IDC_SCHEMES,CB_ADDSTRING,0,(LONG)fcls.GetFileName());
    }
    int index=(int)SendDlgItemMessage(hwnd,IDC_SCHEMES,CB_FINDSTRINGEXACT,-1,(LONG)current);
    if(index==CB_ERR)index=0;
    SendDlgItemMessage(hwnd,IDC_SCHEMES,CB_SETCURSEL,index,0);
};
void SyntaxColors(HWND hwnd){
    TPropertyButton*b=GetDlgPropertyButton(hwnd);
    if(b){
        TSColor* sc=(TSColor*)b->pdata;
        if(sc){
            int index=SendDlgItemMessage(hwnd,IDC_COLORLIST,LB_GETCURSEL,0,0);
            COLORREF c1,c2;
            BYTE style;
            if(index<sc->groups){
                c1=sc->fg[index];c2=sc->bk[index];
                style=sc->style[index];
            }else{
                c1=sc->_fg[index-sc->groups];c2=sc->_bk[index-sc->groups];
                style=sc->_style[index-sc->groups];
            }
            SendDlgItemMessage(hwnd,IDC_FONTCOLOR,COLORB_SETCOLOR,0,c1);
            SendDlgItemMessage(hwnd,IDC_BKCOLOR,COLORB_SETCOLOR,0,c2);

            // 1 - italic/ 2- bold /4 -underline

            CheckDlgButton(hwnd,IDC_SYN_ITALIC,(style&1)!=0);
            CheckDlgButton(hwnd,IDC_SYN_BOLD,(style&2)!=0);
            CheckDlgButton(hwnd,IDC_SYN_UNDERLINE,(style&4)!=0);

        }}
};


void SyntaxColorsE(HWND hwnd){
    BOOL e=(SendDlgItemMessage(hwnd,IDC_SCHEMES,CB_GETCURSEL,0,0)!=0);
    EnableWindow(GetDlgItem(hwnd,IDC_FONTCOLOR),e);
    EnableWindow(GetDlgItem(hwnd,IDC_BKCOLOR),e);
};

BOOL scCALLBACK EnvTextProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case COLORB_COLORCHANGE:{
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(b){
                TSColor* sc=(TSColor*)b->pdata;
                if(sc){
                    COLORREF newrgb=SendMessage((HWND)wParam,COLORB_GETCOLOR,0,0);
                    int index=SendDlgItemMessage(hwnd,IDC_COLORLIST,LB_GETCURSEL,0,0);
                    COLORREF *rgb=NULL;
                    if(lParam==IDC_FONTCOLOR){
                        rgb=(index<sc->groups)?(sc->fg+index):(sc->_fg+(index-sc->groups));
                    }else{
                        rgb=(index<sc->groups)?(sc->bk+index):(sc->_bk+(index-sc->groups));
                    }
                    sc->modifyed=TRUE;
                    *rgb=newrgb;
                }
            }
        }return TRUE;
        case WM_COMMAND:{
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            EV_COMMAND
                    switch(command){
        case IDC_SYN_UNDERLINE:
                case IDC_SYN_ITALIC:
                case IDC_SYN_BOLD:if(b){
                        TSColor* sc=(TSColor*)b->pdata;
                        if(sc){
                            int index=SendDlgItemMessage(hwnd,IDC_COLORLIST,LB_GETCURSEL,0,0);
                            BYTE *style;
                            if(index<sc->groups){
                                style=sc->style+index;
                            }else{
                                style=sc->_style+(index-sc->groups);
                            }
                            BOOL c=IsDlgButtonChecked(hwnd,command);
                            BYTE m=0;
                            switch(command){
                                case IDC_SYN_UNDERLINE:m=4;break;
                                case IDC_SYN_ITALIC:   m=1;break;
                                case IDC_SYN_BOLD:     m=2;break;
                            }
                            if(c)(*style)|=m;else (*style)&=~m;
                            sc->modifyed=TRUE;
                        }}break;

                case IDC_SYNTAXSETUP:{
                    SyntaxSetupDialog(hwnd);


                }break;
                case IDC_COLORLIST:{
                    if(wNotifyCode==LBN_SELCHANGE)SyntaxColors(hwnd);
                }return TRUE;
                case IDC_SAVEAS:{
                    if(b){
                        TSColor* sc=(TSColor*)b->pdata;
                        if(sc){
                            char s[256]="";
                            if(sc->file){
                                lstrcpy(s,sc->file);
                            }else{
                                GetTemplateDir(s);lstrcat(s,"\\save.syn");
                            }
                            if(FileDialog(hwnd,s,IDS_SYNTAXfiles,FD_SAVE)){
                                sc->ToFile(s);
                                if(!SCGetScVarS("syntax_scheme",s))LoadStr(IDS_str4,s);//lstrcpy(s,str4);
                                SynataxSchemes(hwnd,s);
                            }}
                    }return TRUE;
                };
                case IDC_SCHEMES:if(wNotifyCode==CBN_SELCHANGE){
                        if(b){
                            TSColor* sc=(TSColor*)b->pdata;
                            if(sc){
                                char s[256];
                                SendMessage(hwndCtl,CB_GETLBTEXT,
                                            (WPARAM)SendMessage(hwndCtl,CB_GETCURSEL,0,0),(LPARAM)s);
                                if(sc->modifyed && sc->file){

                                    if(MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG5),_MSG_QSC,MB_YESNO|MB_ICONQUESTION)==IDYES)
                                        sc->ToFile(sc->file);
                                }


                                sc->Rebuild(s);
                                SyntaxColors(hwnd);
                                SyntaxColorsE(hwnd);
                            }};
                    }return TRUE;
            };
        }break;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            char sp[260];
            CheckDlgButton(hwnd,IDC_USESYNTAX,SCGetScVar("use_syntax_edit",0));

            // !!!
            char s[256];
            if(!SCGetScVarS("syntax_scheme",s))LoadStr(IDS_str4,s);//lstrcpy(s,str4);
            SynataxSchemes(hwnd,s);
            TSColor*sc=new TSColor();
            b->pdata=sc;

            int i=IDS_SYNTAXITEMS;
            while(LoadString(HResource,i,s,sizeof(s))){
                SendDlgItemMessage(hwnd,IDC_COLORLIST,LB_ADDSTRING,0,(LONG)s);
                i++;
                sc->groups=i;
            }
            sc->groups-=IDS_SYNTAXITEMS;
            char ws[256];
            LoadStr(IDS_str36,ws);
            for(i=1;i<=10;i++){
                wsprintf(s,ws,i); //"Группа функций № %d"
                SendDlgItemMessage(hwnd,IDC_COLORLIST,LB_ADDSTRING,0,(LONG)s);
            };
            SendDlgItemMessage(hwnd,IDC_COLORLIST,LB_SETCURSEL,0,0);
            SyntaxColors(hwnd);
            SyntaxColorsE(hwnd);
        }return TRUE;
        case PSP_CLOSE:
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b->flags&PSP_INITIALIZE){
                TSColor*sc=(TSColor*)b->pdata;
                if(wParam==1){
                    char s[256];
                    HWND list=GetDlgItem(hwnd,IDC_SCHEMES);
                    if(SendMessage(list,CB_GETLBTEXT,(WPARAM)SendMessage(list,CB_GETCURSEL,0,0),(long)s))
                        SCSetScVarS("syntax_scheme",s);
                    SCSetScVarI("use_syntax_edit",IsDlgButtonChecked(hwnd,IDC_USESYNTAX));
                    if(sc->modifyed && sc->file){
                        sc->ToFile(sc->file);
                    }
                    if(syntaxcolor){
                        syntaxcolor->Rebuild(s);
                    }
                }
                if(sc)delete sc;
            }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
#endif // runtime
int EnvironmentDialod(HWND hwnd){
    TPropertyButton*b=new TPropertyButton(IDS_str38,HResource,"Env_Libs",Environment_options___Libraries); //"Библиотеки"
    b->Procedure=EnvLibProc;
    TPropertyButton*b1=b;

    b1=new TPropertyButton(IDS_str39,HResource,"Env_Calc",Environment_options___Calculate); //"Вычисления"
    b1->Procedure=EnvCalcProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str40,HResource,"Env_Method",Environment_options___Methods); //"Методы"
    b1->Procedure=EnvMethodProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str41,HResource,"Env_Preload",Environment_options___Load); // "Разное"
    b1->Procedure=EnvPreloadProc;
    b->Insert(b1);
#ifndef RUNTIME
    b1=new TPropertyButton(IDS_str42,HResource,"Env_Text",Environment_options___Editor);
    b1->Procedure=EnvTextProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str6,HResource,"Env_Vars",Environment_options___Vars);
    b1->Procedure=EnvVarsProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str50,HResource,"Env_Sec",Environment_options___User);
    b1->Procedure=EnvSecProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str51,HResource,"Env_2d",Environment_options___2D_Editor);
    b1->Procedure=Env2dProc;
    b->Insert(b1);
#endif // runtime
    b1=new TPropertyButton(IDS_str47,HResource,"Control_Bars",Environment_options___Toolbars);
    b1->Procedure=ControlBarsProc;
    b->Insert(b1);
    b1=new TPropertyButton(IDS_str195,HResource,"Env_Lang",Environment_options___Language);
    b1->Procedure=LangProc;
    b->Insert(b1);
    //  IDC_LANGLIST

    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str43),b,0);
    delete b;
    return rez;
};

BOOL scCALLBACK BmpSetupProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            EditSubClass(hwnd,IDC_SIZEXbmp,1,4096,1,UDF_MINMAX);
            EditSubClass(hwnd,IDC_SIZEYbmp,1,4096,1,UDF_MINMAX);
            TPropertyButton*b=(TPropertyButton*)lParam;
            BITMAP *bmp=(BITMAP*)b->data;
            SetDlgItemInt(hwnd,IDC_SIZEXbmp,bmp->bmWidth,1);
            SetDlgItemInt(hwnd,IDC_SIZEYbmp,bmp->bmHeight,1);
            int c=0;
            switch(bmp->bmBitsPixel){
                case 1:c=IDC_2;break;
                case 4:c=IDC_16;break;
                case 8:c=IDC_256;break;
                case 24:c=IDC_16m;break;
            }
            CheckDlgButton(hwnd,c,1);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                //TPropertyButton*b=(TPropertyButton*)lParam;
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                BITMAP *bmp=(BITMAP*)b->data;
                INT16 w,h;
                //	  double _a;
                if(
                   IsIntValid(hwnd,IDC_SIZEXbmp,w,1,4096)&&
                   IsIntValid(hwnd,IDC_SIZEYbmp,h,1,4096)){
                    BYTE bitcount=1;
                    if(IsDlgButtonChecked(hwnd,IDC_2))bitcount=1;
                    if(IsDlgButtonChecked(hwnd,IDC_16))bitcount=4;
                    if(IsDlgButtonChecked(hwnd,IDC_256))bitcount=8;
                    if(IsDlgButtonChecked(hwnd,IDC_16m))bitcount=24;
                    bmp->bmBitsPixel=(BYTE)bitcount;
                    bmp->bmWidth=w;
                    bmp->bmHeight=h;
                }else  b->Invalid();
            }else {b->Valid();}
        }return TRUE;
    }
    return FALSE;
};
int BitmapDialog(HWND hwnd,BITMAP*bmp,char *title){
    TPropertyButton*b=new TPropertyButton("Bmp",HResource,"Bmp_Setup");
    b->data=(DWORD)bmp;
    b->Procedure=BmpSetupProc;
    int rez=CreatePropertySheet(hwnd,title,b,0);
    delete b;
    return rez;
};
#ifndef RUNTIME
BOOL scCALLBACK SysInfoProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg){
        case PSP_INITPAGE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            HWND tsw=GetDlgItem(hwnd,IDC_TREEINFO);
            VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
            sw->NotifyWnd=mainwindow->HWindow;
            TreeItemData td;
            td.parent = NULL;
            td.IsLeaf = 0;
            td.IsImage = STRATUM_IMG;

            switch(b->di.d1){
                case 0:
                {
                    td.text = "Stratum Computer";
                    td.data = 1;
                    td.datatype=10;
                }break;
                case 1:
                {
                    td.datatype=29;
                    char s[256];
                    if (EQExist())
                    {
                        LoadStr(IDS_str45,s);
                        //td.text = str45;//"Системы уравнений";
                    }
                    else
                    {
                        //td.text = str46;//"Нет систем уравнений";
                        LoadStr(IDS_str46,s);
                        td.IsLeaf = 1;
                    }
                    td.text =s;
                    td.data = (DWORD)td.text;
                }break;
            }

            ::SendMessage(tsw,TVN_INSERTITEM,0,(long)&td);
            sw->NotifyWnd=mainwindow->HWindow;
            ::SendMessage(tsw,TV_SETSHIFT,1,0);

            ::SendMessage(tsw,TV_EXPAND,1,td.data);
        }return TRUE;
        case PSP_CLOSE:
            if(wParam==1)
            {
                //TPropertyButton*b=(TPropertyButton*)lParam;
            };return TRUE;
        case PSP_VALID:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            {
                b->Valid();
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
#endif
#ifndef RUNTIME
BOOL scCALLBACK FuncInfoProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            // TPropertyButton*b=(TPropertyButton*)lParam;
            HWND list=GetDlgItem(hwnd,IDC_FUNCTIONS);
            RECT rect;
            GetClientRect(list,&rect);
            SendMessage(list,LB_SETCOLUMNWIDTH,rect.right/3,0);
            compiler->FillList(list);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_FUNCHELP:{
                    HWND _hwnd=GetDlgItem(hwnd,IDC_FUNCTIONS);
                    int index=(int)SendMessage(_hwnd,LB_GETCARETINDEX,0,0);
                    char s[256]="";
                    SendMessage(_hwnd,LB_GETTEXT,index,(LPARAM)s);
                    HELP(s);
                }return TRUE;
                case IDC_FUNCTIONS:{
                    if (wNotifyCode==LBN_DBLCLK){
                        compiler->FunctionHelp(hwnd,(INT16)SendMessage(hwndCtl,LB_GETCARETINDEX,0,0));
                    }
                }return TRUE;
            }
        }break;
    }

    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
#endif
#ifndef RUNTIME
BOOL scCALLBACK SysMiskProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TMemoryStream ms(TRUE);
            char s[256];
            // wsprintf(s,"Stratum Computer for Windows. Build %d",__SC_BUILD__);
            LoadString(HResource,IDS_str104,s,sizeof(s));

            ms.WriteDOSSTR(s);

            char ws[256];
            LoadStr(IDS_str61,ws);
            wsprintf(s,ws,__DATE__);
            ms.WriteDOSSTR(s);
            LoadStr(IDS_str60,s);
            ms.WriteDOSSTR(s);

#ifdef DEMO
            ms.WriteDOSSTR("** DEMO ** DEMO ** DEMO ** DEMO ** DEMO ** DEMO **");
            ms.WriteDOSSTR("**      THIS VERSION OF STRATUM CAN'T SAVE      **");
            ms.WriteDOSSTR("** DEMO ** DEMO ** DEMO ** DEMO ** DEMO ** DEMO **");
#endif

#ifdef LOGON
            LoadStr(IDS_str59,s);
            ms.WriteDOSSTR(s);
#endif

#ifdef CHECKHEAP
            LoadStr(IDS_str58,s);
            ms.WriteDOSSTR(s);
#endif

#ifdef CAN_EDIT_BITMAP
            LoadStr(IDS_str57,s);
            ms.WriteDOSSTR(s);
#endif
            ms.WriteDOSSTR("");
            GetClassesInfo(ms);
            sc_vars->ToText(ms,1);
            ms.WriteWord(0);
            SetWindowText(GetDlgItem(hwnd,IDC_TREEINFO),(char*)ms.base);

        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
#endif
#ifndef RUNTIME
int SysInfoDialog(HWND hwnd){
    TPropertyButton*b=new TPropertyButton(IDS_str37,HResource,"System_Info",Information___Information);//"Информация"
    TPropertyButton*b1=b;
    b1->di.d1=0;
    b1->Procedure=SysInfoProc;

    b1=new TPropertyButton(IDS_str49,HResource,"System_Info",Information___Equations);//"Уравнения"
    b1->di.d1=1;
    b1->Procedure=SysInfoProc;
    b->Insert(b1);

    if (compiler){
        b1=new TPropertyButton(IDS_str44,HResource,"Func_Info",Information___Functions); //"Функции"
        b1->Procedure=FuncInfoProc;
        b->Insert(b1);
    }

    b1=new TPropertyButton(IDS_str32,HResource,"System_Edit",Information___Misc);//"Разное"
    b1->di.d1=1;
    b1->Procedure=SysMiskProc;
    b->Insert(b1);

    // V - максимальная закладка
    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str37),b,0); //"Информация"
    delete b;
    return rez;
};
#endif



BOOL ControlBarsDialog(HWND hwnd){
    TPropertyButton*b=new TPropertyButton(IDS_str47,HResource,"Control_Bars",Environment_options___Toolbars);//"Панели"
    b->Procedure=ControlBarsProc;
    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str48),b,0);//"Панели инструментов"
    delete b;
    return rez;
};

BOOL scCALLBACK InputProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            switch(msg){
        case WM_INITDIALOG:{
            SetWindowText(hwnd,inputtitle);
            SetDlgItemText(hwnd,IDC_INPUTTEXT,inputmsg);
            SetDlgItemText(hwnd,IDC_VALUE,inputline);
        }return TRUE;
        case WM_COMMAND:
            EV_COMMANDsimply
                    switch (command){
        case IDOK:{
                    GetDlgItemText(hwnd,IDC_VALUE,inputline,255);
                    EndDialog(hwnd,1);
                    return TRUE;}
                case IDCANCEL: EndDialog(hwnd,0);return TRUE;
            }
    }
    return FALSE;
};

int InputBox(HWND hwnd,char *title,char*msg, char*s){
    inputline=s;
    inputtitle=title;
    inputmsg=msg;
    if (DialogBox(HResource,MAKEINTRESOURCE(INPUT_BOX),hwnd,InputProc)){
        return 1;
    }else {
        lstrcpy(s,"");
        return 0;
    }
}

TMemoryStream* ObjectData(HSP2D hsp,HOBJ2D obj,UINT16 id)
{
    INT16 size = GetObjectData2d(hsp,obj,id,NULL,0,0);
    if (!size)
        return NULL;

    BYTE* data = new BYTE[size];
    if (GetObjectData2d(hsp, obj, id, data, 0, size) == size)
        return new TMemoryStream(data, size);
    else
        delete data;

    return NULL;
}

TSpaceData::TSpaceData(HSP2D _hsp)
{
    hsp=_hsp;
    sizex=0;
    sizey=0;
    style=0;
    wasdata = (GetObjectData2d(hsp, 0, UD_HYPERSPACE, &style, 0, 8) > 0);
}

void TSpaceData::Update(){
    INT16 size=0;
    if (style || ((sizex>0) && (sizey>0)))size=8;
    SetObjectData2d(hsp,0,UD_HYPERSPACE,&style,0,size);
};
void TPrintSpace::UpdatePage(HWND _page,int mode)
{
    switch(mode){
        case 0:
            Rscale1=GetDlgItem(_page,IDC_100);
            Rscale2=GetDlgItem(_page,IDC_FIT);
            Rscale3=GetDlgItem(_page,IDC_FIT2);
            Rscale4=GetDlgItem(_page,IDC_CUSTOMSCALE);
            Tscale=GetDlgItem(_page,IDC_SCALETEXT);
            Rpage1=GetDlgItem(_page,IDC_ALL);
            Rpage2=GetDlgItem(_page,IDC_CHUSE);
            Tpages=GetDlgItem(_page,IDC_PAGES);
            Cpagesenum = GetDlgItem(_page,IDC_PAGENUMBERS);
            Tcopies=GetDlgItem(_page,IDC_PAGECOUNT);
            break;
        case 1:
            Tfleft    =GetDlgItem(_page,IDC_LEFTf);
            Tftop     =GetDlgItem(_page,IDC_TOPf);
            Tfright   =GetDlgItem(_page,IDC_RIGHTf);
            Tfbottom  =GetDlgItem(_page,IDC_BOTTOMf);
            Taright   =GetDlgItem(_page,IDC_ARIGHT);
            Tabottom  =GetDlgItem(_page,IDC_ABOTTOM);
            Cause     =GetDlgItem(_page,IDC_ADDBORDER);
            Conly     =GetDlgItem(_page,IDC_ONLYEXTERNAL);
            break;
        case 2:
            Tsleft    =GetDlgItem(_page,IDC_LEFT);
            Tstop     =GetDlgItem(_page,IDC_TOP);
            Tsright   =GetDlgItem(_page,IDC_RIGHT);
            Tsbottom  =GetDlgItem(_page,IDC_BOTTOM);
            Cextent   =GetDlgItem(_page,IDC_EXTENT);
            break;
    }
};

BOOL scCALLBACK PrintProgressProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            return TRUE;
        }
    }
    return FALSE;
}

BOOL _SetPage(TPrintSpace*data,HWND progress,int d){
    char s[256];
    wsprintf(s,"Page %d",d+1);
    SetDlgItemText(progress,616,s);
    UpdateWindow(progress);
    for(int i=0;i<data->copies;i++)data->Print(d);
};

BOOL scCALLBACK PrintMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TPrintSpace*data=(TPrintSpace*)b->data;
            INT16 c;
            double d;
            if(IsIntValid(hwnd,IDC_PAGECOUNT,c,1,99) &&
               IsDoubleValid(hwnd,IDC_SCALETEXT,d,0.01,10000)
               ){
                b->Valid();
                data->copies=c;
            }else b->Invalid();
        }return 0;
        case PSP_CLOSE:if (wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                TPrintSpace*data=(TPrintSpace*)b->data;
                if(!ScGetKeyState(VK_SHIFT)){ // begin print
                    HWND progress=CreateDialog(HResource,MAKEINTRESOURCE(PRINT_PROGRESS),mainwindow->HWindow,PrintProgressProc);
                    if(data->allpages){
                        if(data->BeginPrint()){
                            for(int i=0;i<data->ypage;i++)
                                for(int j=0;j<data->xpage;j++){
                                    int d=j+i*data->xpage;
                                    _SetPage(data,progress,d);
                                }
                            data->EndPrint();
                        }
                    }else{
                        TCollection items(10,10);
                        char s[256];
                        char *ps=s;
                        char *_ps;
                        GetDlgItemText(hwnd,IDC_PAGES,s,sizeof(s));
                        do{
                            char item[256];

                            _ps=strstr(s,",");
                            if(_ps){lstrcpyn(item,ps,(_ps-ps)+1);ps=_ps+1;}else lstrcpy(item,ps);
                            char *_pp=strstr(item,"-");
                            if(_pp){
                                *_pp=' ';
                                int d1,d2;
                                if(sscanf(item,"%d %d",&d1,&d2)==2){
                                    if ((d1>0)&&(d1<=data->pagecount)&&
                                        (d2>0)&&(d2<=data->pagecount)&& (d1<d2)
                                        ){
                                        for(int i=d1;i<d2;i++){
                                            items.Insert((pointer)i);
                                        };
                                    }else goto noprint;
                                }else goto noprint;

                            }else{
                                int d=0;
                                if(sscanf(item,"%d",&d)==1){
                                    if ((d>0)&&(d<=data->pagecount)){
                                        items.Insert((pointer)d);
                                    }else goto noprint;
                                }else goto noprint;
                            }

                        }while(_ps);
                        if(data->BeginPrint()){
                            for(int i=0;i<items.count;i++)_SetPage(data,progress,((int)items.At(i))-1);
                            data->EndPrint();
                        }
                    }
                    goto afteprint;
noprint:
                    MSGBOX(progress,MAKEINTRESOURCE(IDS_MSG18),_MSG_ERROR,MB_ICONHAND);
afteprint:
                    DestroyWindow(progress);
                } // end print

                BOOL ch=1;//IsDlgButtonChecked(p,IDC_PRINTWITHSPACE);
                TMemoryStream ms;
                HSP2D hsp=data->hsp;
                data->Store(ms);
                SetObjectData2d(hsp,0,UD_PRINTPAGE,NULL,0,0);
                if(ch){
                    SetObjectData2d(hsp,0,UD_PRINTPAGE,ms.base,0,(INT16)ms.GetSize());
                }
            }break;
        case WM_INITDIALOG:{
            EditSubClass(hwnd,IDC_PAGECOUNT,1,99,1,UDF_MINMAX);
            TPropertyButton*b=(TPropertyButton*)lParam;
            TPrintSpace*data=(TPrintSpace*)b->data;
            SetDlgItemText(hwnd,IDC_PRINTERPRORT,data->szPort);
            {
                char *s=new char[4096];
                GetProfileString("Devices",NULL,"",s,4096);
                char *ps=s;
                while(*ps){
                    SendDlgItemMessage(hwnd,IDC_PRINTERS,CB_ADDSTRING,0,(LPARAM)ps);
                    ps=ps+lstrlen(ps)+1;
                }
                int i=(int)SendDlgItemMessage(hwnd,IDC_PRINTERS,CB_FINDSTRINGEXACT,-1,(LPARAM)(data->szPrinter));
                if(i<0)i=0;
                SendDlgItemMessage(hwnd,IDC_PRINTERS,CB_SETCURSEL,i,0);
                delete s;
            }

            data->UpdatePage(hwnd,0);
            HSP2D hsp=data->hsp;

            int size=GetObjectData2d(hsp,0,UD_PRINTPAGE,0,0,0);
            if(size){
                char *s=new char[size];
                GetObjectData2d(hsp,0,UD_PRINTPAGE,s,0,(INT16)size);
                TMemoryStream ms;
                ms.Write(s,size);
                ms.Seek(0);
                data->Restore(ms);
                //CheckDlgButton(GetParent(hwnd),IDC_PRINTWITHSPACE,1);
                delete s;
            }
        }return TRUE;
        case WM_COMMAND:
        {
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(b)
            {
                TPrintSpace*data=(TPrintSpace*)b->data;
                EV_COMMAND
                        switch (command){
                    //||(wNotifyCode==EN_UPDATE)
                    /*   case IDC_PAGECOUNT:if((wNotifyCode==EN_KILLFOCUS)){
       INT16 c;
        if(IsIntValid(hwnd,IDC_PAGECOUNT,c,1,99)){
        data->copies=c;
       }}break;*/
        case IDC_PRINTERS:
                    {
                        int i=SendDlgItemMessage(hwnd,IDC_PRINTERS,CB_GETCURSEL,0,0);
                        char s[256];
                        SendDlgItemMessage(hwnd,IDC_PRINTERS,CB_GETLBTEXT,i,(LPARAM)s);
                        data->UpdatePrinter(s);
                        SetDlgItemText(hwnd,IDC_PRINTERPRORT,data->szPort);
                        data->UpdateAllDialogs();
                    };return TRUE;
                    case IDC_ALL:if(SendMessage(hwndCtl,BM_GETCHECK,0,0))data->allpages=1;break;
                    case IDC_CHUSE:if(SendMessage(hwndCtl,BM_GETCHECK,0,0))data->allpages=0;break;
                    case IDC_PAGES:
                        if((wNotifyCode==EN_SETFOCUS))
                        {
                            data->allpages=0;
                            data->UpdateAllDialogs(1);
                        }
                        return TRUE;
                    case IDC_SCALETEXT:
                        if((wNotifyCode==EN_SETFOCUS))
                        {
                            data->SetScaleMode(3);
                        }
                        if((wNotifyCode==EN_UPDATE))
                        {
                            double d;
                            if(IsDoubleValid(hwnd,IDC_SCALETEXT,d,10000,0.01))
                            {
                                data->_SetScale(d);
                            }
                        }
                        return TRUE;
                    case IDC_100:
                    case IDC_FIT:
                    case IDC_FIT2:
                    case IDC_CUSTOMSCALE:
                        if(SendMessage(hwndCtl,BM_GETCHECK,0,0))
                            switch(command){
                                case IDC_100:data->SetScaleMode(0);break;
                                case IDC_FIT:data->SetScaleMode(1);break;
                                case IDC_FIT2:data->SetScaleMode(2);break;
                                case IDC_CUSTOMSCALE:data->SetScaleMode(3);break;
                            }
                        break;
                    case IDC_PROPERTIES:
                    {
                        //HWND parent=GetParent(hwnd);
                        data->Setup(GetParent(hwnd));
                    }break;
                }
            }
        }break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK PrintAddProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TPrintSpace*data=(TPrintSpace*)b->data;
            data->UpdatePage(hwnd,1);
            data->UpdateAllDialogs(4);
            EnableWindow(GetDlgItem(hwnd,IDC_MARKERS),0);
            EditSubClass(hwnd,IDC_LEFTf,0,0,1,UDF_MIN|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_RIGHTf,0,0,1,UDF_MIN|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_TOPf,0,0,1,UDF_MIN|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_BOTTOMf,0,0,1,UDF_MIN|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_ARIGHT,0,0,1,UDF_MIN|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_ABOTTOM,0,0,1,UDF_MIN|UDF_DOUBLE);

        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(b){
                TPrintSpace*data=(TPrintSpace*)b->data;
                if(data)
                    switch (command){
                        case IDC_ADDBORDER:{
                            data->addborders=IsDlgButtonChecked(hwnd,IDC_ADDBORDER);
                            EnableWindow(data->Taright ,data->addborders);
                            EnableWindow(data->Tabottom,data->addborders);
                        }break;
                        case IDC_ONLYEXTERNAL:{
                            data->onlyexternal=IsDlgButtonChecked(hwnd,IDC_ONLYEXTERNAL);
                        }break;
                    };
            }
        }break;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b->flags&PSP_INITIALIZE){
                TPrintSpace*data=(TPrintSpace*)b->data;
                RECT2D r;
                double a1,a2;
                b->Invalid();
                if(IsDoubleValid(hwnd,IDC_LEFTf,r.left,0,data->width)&&
                   IsDoubleValid(hwnd,IDC_RIGHTf,r.right,0,data->width)&&
                   IsDoubleValid(hwnd,IDC_TOPf,r.top,0,data->height)&&
                   IsDoubleValid(hwnd,IDC_BOTTOMf,r.bottom,0,data->height)&&
                   IsDoubleValid(hwnd,IDC_ARIGHT ,a1 , 0,data->width)&&
                   IsDoubleValid(hwnd,IDC_ABOTTOM,a2 , 0,data->height)
                   ){
                    if(((r.left+r.right)<data->width) && ((r.top+r.bottom)<data->width)){
                        data->fileds=r;

                        data->addf.x=a1;
                        data->addf.y=a2;

                        b->Valid();
                        data->UpdateAllDialogs(1);
                    }else{
                        MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG6),_MSG_ERROR,MB_OK|MB_ICONINFORMATION);
                    }
                }
            }
        }break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK PrintSrcProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            TPropertyButton*b=(TPropertyButton*)lParam;

            TPrintSpace*data=(TPrintSpace*)b->data;
            data->UpdatePage(hwnd,2);
            data->UpdateAllDialogs();
            EditSubClass(hwnd,IDC_LEFT,-32767,32767,1,UDF_MINMAX|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_RIGHT,-32767,32767,1,UDF_MINMAX|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_TOP,-32767,32767,1,UDF_MINMAX|UDF_DOUBLE);
            EditSubClass(hwnd,IDC_BOTTOM,-32767,32767,1,UDF_MINMAX|UDF_DOUBLE);

        }return TRUE;
        case WM_COMMAND:{
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(b){
                TPrintSpace*data=(TPrintSpace*)b->data;
                EV_COMMAND
                        switch (command){
        case IDC_UPDATE:data->rect2dcustom=data->rect2d;data->UpdateAllDialogs();break;
                    case IDC_EXTENT:data->SetExtent(SendMessage(hwndCtl,BM_GETCHECK,0,0));break;
                }
            }
        }break;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b->flags&PSP_INITIALIZE){
                TPrintSpace*data=(TPrintSpace*)b->data;
                RECT2D r;
                b->Invalid();
                if(IsDoubleValid(hwnd,IDC_LEFT,r.left,-32767,32767)&&
                   IsDoubleValid(hwnd,IDC_RIGHT,r.right,-32767,32767)&&
                   IsDoubleValid(hwnd,IDC_TOP,r.top,-32767,32767)&&
                   IsDoubleValid(hwnd,IDC_BOTTOM,r.bottom,-32767,32767)){
                    if(r.left<r.right && r.top<r.bottom){
                        data->rect2dcustom=r;
                        b->Valid();
                        data->UpdateAllDialogs();
                    }else{

                        MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG7),_MSG_ERROR,MB_OK|MB_ICONHAND);
                    }
                }
            }
        }break;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};


int SpacePrintDialog(HWND hwnd,HSP2D hsp){

    TPrintSpace data(hwnd,hsp);

    switch(data.errorcode){
        case 1:MSGBOX(hwnd,MAKEINTRESOURCE(IDS_NOPRINTER),_MSG_ERROR,MB_OK|MB_ICONHAND);return 0;
        case 2:MSGBOX(hwnd,MAKEINTRESOURCE(IDS_BADPRINTER),_MSG_ERROR,MB_OK|MB_ICONHAND);return 0;
        case 3:MSGBOX(hwnd,MAKEINTRESOURCE(IDS_EMPTYPRINT),_MSG_ERROR,MB_OK|MB_ICONHAND);return 0;
    }
    if(data.errorcode)return 0;

    TPropertyButton*b=new TPropertyButton(IDS_str64,HResource,"Print_Main",Print_page_dialog);
    b->Procedure=PrintMainProc;
    TPropertyButton*b1=b;


    b1=new TPropertyButton(IDS_str65,HResource,"Print_Border",Print_page_dialog);
    b1->Procedure=PrintAddProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str66,HResource,"Print_Source",Print_page_dialog);
    b1->Procedure=PrintSrcProc;
    b->Insert(b1);

    b1=b;
    while(b1){
        b1->data=(UINT32)&data;
        b1=b1->next;
    }

    int rez=CreatePropertySheetEx(hwnd,(char*)MAKEINTRESOURCE(IDS_str102),b,0,"Print_Property"); // Информация о переменной
    if(rez)data.Print(0);
    delete b;
    //
    return rez;
};
BOOL PrinterSetupDialog(HWND hwnd){
    PRINTDLG pd;
    // Set all structure members to zero.

    memset(&pd, 0, sizeof(PRINTDLG));

    // Initialize the necessary PRINTDLG structure members.

    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner = hwnd;
    pd.Flags =PD_NOPAGENUMS|PD_PRINTSETUP;
    if(PrintDlg(&pd) != 0){
        if (pd.hDevMode != NULL)
            GlobalFree(pd.hDevMode);
        if (pd.hDevNames != NULL)
            GlobalFree(pd.hDevNames);
        return TRUE;
    }
    return FALSE;
}
BOOL TPrintSpace::PrinterSetupDialog(HWND hwnd){
#ifdef WIN32
    HANDLE  hPrinter;
    BOOL rez=FALSE;
    if(OpenPrinter(
           szPrinter,	// address of printer or server name
           &hPrinter,	// address of printer or server handle
           NULL	// address of printer defaults structure
           )){
        rez=PrinterProperties(hwnd,hPrinter );
        ClosePrinter(hPrinter);
    }
    return rez;
#else
    PRINTDLG pd;
    // Set all structure members to zero.

    memset(&pd, 0, sizeof(PRINTDLG));

    // Initialize the necessary PRINTDLG structure members.

    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner = hwnd;
    pd.Flags =PD_NOPAGENUMS|PD_PRINTSETUP;
    if(PrintDlg(&pd) != 0){
        if (pd.hDevMode != NULL)
            GlobalFree(pd.hDevMode);
        if (pd.hDevNames != NULL)
            GlobalFree(pd.hDevNames);
        return TRUE;
    }
    return FALSE;
#endif

};


void _ChkProjectFlag(HWND hwnd,int id,char *name,int def){
    TProject*prj=(TProject*)GetCurrentProject();
    C_TYPE index;
    int i=2;
    if (prj->vars && prj->vars->GetVar(name,index))
    {i=SCGetScVar(name,def); }
    CheckDlgButton(hwnd,id,i);
};

void _SetProjectFlag(HWND hwnd,int id,char *name){
    int i=IsDlgButtonChecked(hwnd,id);
    TProject*prj=(TProject*)GetCurrentProject();
    if (i==2){
        if(prj->vars) prj->vars->DeleteVar(name);
    }else{
        if(!prj->vars)prj->vars = new TScVarManager();
        prj->vars->SetVarI(name,i);
        prj->SetModify();
    }
};


BOOL scCALLBACK ProjectVarsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{

            _ChkProjectFlag(hwnd,IDC_ONESTEPVARS   ,"vars_1step",1);
            _ChkProjectFlag(hwnd,IDC_ENABLESETVARS ,"vars_logset",1);
            _ChkProjectFlag(hwnd,IDC_RESETONDEF    ,"vars_setondef",1);
            _ChkProjectFlag(hwnd,IDC_SAVEVARS	     ,"vars_save",1);
            _ChkProjectFlag(hwnd,IDC_PRELOADVRAS	 ,"vars_preload",1);
            char s[256];
            TProject*prj=(TProject*)GetCurrentProject();
            if(prj->GetVarS("preload_file",s))
                SetDlgItemText(hwnd,IDC_PRELOADFILE,s);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                _SetProjectFlag(hwnd,IDC_ONESTEPVARS,"vars_1step"   );
                _SetProjectFlag(hwnd,IDC_ENABLESETVARS,"vars_logset"  );
                _SetProjectFlag(hwnd,IDC_RESETONDEF,"vars_setondef");
                _SetProjectFlag(hwnd,IDC_SAVEVARS,"vars_save"    );
                _SetProjectFlag(hwnd,IDC_PRELOADVRAS,"vars_preload" );
                char s[256];
                GetDlgItemText(hwnd,IDC_PRELOADFILE,s,sizeof(s));
                TProject*prj=(TProject*)GetCurrentProject();
                if(lstrlen(s) && lstrcmpi(s,"_preload.stt"))
                {
                    prj->SetVarS("preload_file",s);
                }else prj->DeleteVar("preload_file");

            }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL ProjectDialog(HWND hwnd,TProject*_project)
{
    TProject* _old=project;
    project=_project;
    TPropertyButton* b= new TPropertyButton(IDS_str6,HResource,"Project_Vars",Project_options___Vars); // Переменные
    b->Procedure=ProjectVarsProc;

    TPropertyButton*b1=new TPropertyButton(IDS_str13,HResource,"Lib_Main",Project_options___Library); // "Библиотеки"
    b1->Procedure=LibMainProc;
    b1->pdata=project->libs->At(0);
    b1->dwuser1=(DWORD)project;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str40,HResource,"Project_Method",Project_options___Methods); // Методы
    b1->pdata=project;
    b1->Procedure=EnvMethodProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str39,HResource,"Project_Calc",Project_options___Methods); // Методы
    b1->pdata=project;
    b1->Procedure=EnvCalcProc;
    b->Insert(b1);

    b1=new TPropertyButton(IDS_str1,HResource,"Project_Info",Project_options___Methods); // Методы
    b1->pdata=project;
    b1->Procedure=ProjectInfoProc;
    b->Insert(b1);


    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str52),b,0);
    delete b;
    project=_old;
    return rez;
};


TSColor::TSColor()
{
    memset(this,0,sizeof(TSColor));
    Reset();
    char s[256];
    if(!SCGetScVarS("syntax_scheme",s));
    Rebuild(s);
}

TSColor::~TSColor(){
    if(file){delete file;file=0;}
};
void TSColor::Rebuild(char *s)
{
    char _s[256];
    LoadStr(IDS_str4, _s);

    if(file)
    {
        delete file;
        file=0;
    }

    if (lstrcmpi(s, _s))
    {
        char sp[260];
        GetTemplateDir(sp);
        AddSlash(sp);
        lstrcat(sp,s);

        FromFile(sp);
        file=NewStr(sp);

    }
    else
    {
        SetAuto();
    }
    modifyed=FALSE;
}

TSColor::TSColor(TSColor&c)
{
    memcpy(this,&c,sizeof(TSColor));
};

void TSColor::Reset(){
    for(int i=0;i<sizeof(fg)/4;i++)fg[i]=RGB(255,0,255);
    for(i=0;i<sizeof(bk)/4;i++)bk[i]=RGB(0,0,255);
    for(i=0;i<sizeof(_fg)/4;i++)_fg[i]=RGB(128,0,0);
    for(i=0;i<sizeof(_bk)/4;i++)_bk[i]=RGB(255,255,255);
    for(i=0;i<sizeof(style);i++)style[i]=0;
    for(i=0;i<sizeof(_style);i++)_style[i]=0;
};

void TSColor::WriteStr(char*file,char*item,COLORREF _f,COLORREF _b,BYTE _s){
    int r,g,b;
    int r2,g2,b2;
    char s[256];
    r =GetRValue(_f);  g=GetGValue(_f);  b=GetBValue(_f);
    r2=GetRValue(_b); g2=GetGValue(_b); b2=GetBValue(_b);
    wsprintf(s,"RGB(%d,%d,%d),RGB(%d,%d,%d),%d",r,g,b,r2,g2,b2,_s);
    WritePrivateProfileString("Colors",item,s,file);
};

void TSColor::SetAuto(){
    fg[1]=fg[0]=textsyntax=GetSysColor(COLOR_BTNTEXT);
    bk[1]=bk[0]=bkgrndsyntax=GetSysColor(COLOR_BTNHIGHLIGHT);

    fg[2]=RGB(0,0,255);
    bk[2]=bkgrndsyntax;
    fg[3]=RGB(0,127,127);
    bk[3]=bkgrndsyntax;

    fg[4]=GetSysColor(COLOR_GRAYTEXT);
    bk[4]=bkgrndsyntax;
    fg[5]=GetSysColor(COLOR_HIGHLIGHTTEXT);
    bk[5]=GetSysColor(COLOR_HIGHLIGHT);

    fg[6]=RGB(0,127,0);
    bk[6]=bkgrndsyntax;

    fg[7]=RGB(255,255,255);
    bk[7]=RGB(255,0,0);


    for(int i=8;i<SYNTAX_ITEMS_C;i++){
        fg[i]=RGB(0,128,0);
        bk[i]=bkgrndsyntax;
    }
    for(i=0;i<SYNTAX_ITEMS_G;i++){
        _fg[i]=RGB(128,0,0);
        _bk[i]=bkgrndsyntax;
    }
};

void TSColor::Set(HWND hwnd){
    for(int i=0;i<8;i++){
        SYNTCOLOR sc;
        sc.scTEXT=fg[i];
        sc.scBK=bk[i];
        sc.scStyle=style[i];
        SendMessage(hwnd,ESM_SETCOLORS,i,(LPARAM)&sc); // Set
    }
};


void TSColor::ToFile(char*file){
    char item[256];
    int i=0;
    while(LoadString(HResource, IDS_SYNTAXITEMS2+i,item,sizeof(item))){
        WriteStr(file,item,fg[i],bk[i],style[i]);i++;
    }

    for(i=0;i<SYNTAX_ITEMS_G;i++){
        wsprintf(item,"Group%d",i+1);
        WriteStr(file,item,_fg[i],_bk[i],_style[i]);
    }
    modifyed=FALSE;
};

void TSColor::FromFile(char*file)
{
    Reset();
    char item[256];
    char s[256];
    BYTE _s;
    int i=0;
    COLORREF f,b;
    while( LoadString(HResource, IDS_SYNTAXITEMS2 + i, item, sizeof(item)) )
    {
        GetPrivateProfileString("Colors", item, "RGB(255,255,255), RGB(0,0,0)", s, 255, file);
        GetColor(s, f, b, _s);
        fg[i]=f;
        bk[i]=b;
        style[i]=_s;
        i++;
    }

    for(i=0;i<SYNTAX_ITEMS_G;i++){
        wsprintf(item,"Group%d",(i+1));
        GetPrivateProfileString("Colors",item,"RGB(255,255,255),RGB(0,0,0)",s,255,file);
        GetColor(s,f,b,_s);
        _fg[i]=f;
        _bk[i]=b;
        _style[i]=_s;
    }
};

void TSColor::GetColor(char*s,COLORREF& _f,COLORREF&_b,BYTE &_s)
{
    int r, g, b;
    int r2, g2, b2;
    int s1 = 0;
    _f = RGB(0, 0, 0);
    _b = RGB(255, 255, 255);

    if(sscanf(s, "RGB(%d,%d,%d),RGB(%d,%d,%d),%d", &r, &g, &b, &r2, &g2, &b2, &s1) > 0)
    {
        _f = RGB(r, g, b);
        _b = RGB(r2, g2, b2);
        _s = s1;
    }
};

WNDPROC editprev;

struct UDstruct
{
    double _min;
    double _max;
    double step;
    int    flag;
};

UDstruct*GetUD(HWND hwnd)
{
    return
        #ifdef WIN32
            (UDstruct *)GetProp(hwnd,"UD");
#else
            (UDstruct *)MAKELONG(GetProp(hwnd,"UDl"),GetProp(hwnd,"UDh"));
#endif
};

BOOL scCALLBACK EditSubProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    if(msg==WM_DESTROY)
    {
        UDstruct*u=GetUD(hwnd);
#ifdef WIN32
        RemoveProp(hwnd,"UD");
#else
        RemoveProp(hwnd,"UDl");RemoveProp(hwnd,"UDh");
#endif
        delete u;
    };
    if(msg==WM_VSCROLL)
    {
#ifdef WIN32
        int nScrollCode = (int) LOWORD(wParam);
#else
        int nScrollCode = wParam;
#endif
        switch (nScrollCode){
            case SB_LINEUP:
            case SB_LINEDOWN:
            {
                UDstruct *u=GetUD(hwnd);
                char s[64];
                double step=u->step;
                GetWindowText(hwnd,s,sizeof(s));
                if(u->flag&UDF_DOUBLE)
                {
                    double value;
                    int rz=StrToDouble(s,value);
                    if(!rz)value=0;
                    if(nScrollCode==SB_LINEUP)value+=step;
                    if(nScrollCode==SB_LINEDOWN)value-=step;
                    if((u->flag&UDF_MIN)&&(value<u->_min))value = u->_min;
                    if((u->flag&UDF_MAX)&&(value>u->_max))value = u->_max;
                    sprintf(s,"%g",value);
                }
                else
                {
                    // integer
                    char * endptr=(char *)s;
                    int value = (int)strtol(s,&endptr,10);
                    if (!((endptr==NULL) || ((*endptr)==0)))	value=0;
                    if(nScrollCode==SB_LINEUP)value+=u->step;
                    if(nScrollCode==SB_LINEDOWN)value-=u->step;
                    if((u->flag&UDF_MIN)&&(value<u->_min))value = u->_min;
                    if((u->flag&UDF_MAX)&&(value>u->_max))value = u->_max;
                    sprintf(s,"%d",(int)value);
                }
                SetWindowText(hwnd,s);
                //  	SendCommand(GetParent(hwnd),hwnd,EN_UPDATE);
            }}
        return 0;
    }
    return CallWindowProc(editprev, hwnd, msg, wParam, lParam);
}

BOOL EditSubClass(HWND hwnd,int id,double _min,double _max,double _step,int flags)
{
    if(id)	hwnd=GetDlgItem(hwnd,id);
    if(!hwnd)	return FALSE;
    UDstruct *u=new UDstruct;
    u->_min=_min;u->_max=_max;u->step=_step;u->flag=flags;
#ifdef WIN32
    SetProp(hwnd,"UD",(HANDLE)u);
#else
    SetProp(hwnd,"UDl",(HANDLE)LOWORD(u));
    SetProp(hwnd,"UDh",(HANDLE)HIWORD(u));
#endif
    editprev=(WNDPROC)SetWindowLong(hwnd,GWL_WNDPROC,(LPARAM)EditSubProc);
    ShowScrollBar(hwnd,SB_VERT,TRUE);
    return TRUE;
};

