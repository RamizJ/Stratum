/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            dialogs.cpp
Author:
            Alexander Shelemekhov
Description:
         (Class) and (Link) Dialog
*/
#undef NONLS
#ifndef RUNTIME
#define FILE_ITERATOR
#include "_dialogs.h"
#include <stratum\sc_msg.h>
#ifndef WIN32
#include <string.h>
#endif
#include "tscheme.h"
void AddLayer(HWND combo,int start,int end,int current);
//***********************************************************************//
//                                                                       //
//                Далее для ClassDialog                                  //
//                                                                       //
//***********************************************************************//
BYTE lastcompilestatus=1;

TVarData* vardata = NULL;

extern TPropertyButton*_lastactive;
#ifndef RUNTIME
extern TSColor*syntaxcolor;
#endif

void __UpdOkButton(HWND hwnd,PClass _class)
{
    if (_class->Protected())
    {
        EnableWindow(GetDlgItem(GetParent(hwnd),IDOK),0);
    }
}

void AllTrim(char *in)
{
    char *_in=in;
    while(*in && *in==' ')in++;
    if(*in){
        char *ps=in;while(*ps)ps++;ps--;
        while(*ps==' ' && ps>in){*ps=0;ps--;}
        if(in>_in && *in){
            while(*in){*_in=*in;_in++;in++;}
            *_in=0;
        }
        return;
    }
    *_in=0;
}

// Class Variable Procedure
//фукция работы с окном отображения переменных имиджа (class_var)
BOOL ClassVarProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg)
    {
        case TBN_PAINTCELL:
        {
            CELLINFO* ci=(CELLINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(vardata && b)
            {
                C_TYPE y = vardata->GetRealIndex((C_TYPE)ci->y);
                PVarInfo pv=(PVarInfo)vardata->items->At(y);
                if(pv->type->isRGB())
                {
                    char s[256]="";
                    COLORREF color;
                    switch(ci->x)
                    {
                        case 2:
                        {
                            PObject po=(PObject)b->user1;
                            if (!po)return 0;
                            GetObjectVarA(po,y,s);
                        }break;
                        case 3:
                        {
                            lstrcpy(s,pv->def?pv->def:"");
                        }break;
                        default:return 0;
                    }
                    // получение цвета
                    COLORREF realcolor=0;
                    if(s[0]) StrToRGB(s,color,&realcolor);
                    HBRUSH hb=(color&0x01000000L)?BGrayed:CreateSolidBrush(realcolor);
                    HGDIOBJ _hb=SelectObject(ci->hDC,hb);
                    //рисуем прямоугольник для отображения цвета в переменной
                    int w=(ci->rect.bottom-ci->rect.top);
                    Rectangle(ci->hDC,ci->rect.left+2,ci->rect.top+2,ci->rect.left+w+1,ci->rect.top+w+1);
                    SelectObject(ci->hDC,_hb);
                    if(hb!=BGrayed)DeleteObject(hb);
                    SetTextColor(ci->hDC,0);
                    ExtTextOut(ci->hDC, ci->rect.left+w+3,ci->rect.top+2,ETO_CLIPPED,&(ci->rect),s,lstrlen(s),NULL);
                    ci->ok=TRUE;
                }
            }
        }return 0;

        case TBN_GETEDITRECT:
        {
            CELLINFO* ci=(CELLINFO*)lParam;
            if((ci->x==2) || (ci->x==3))
            {
                TPropertyButton*b=GetDlgPropertyButton(hwnd);
                if(vardata && b)
                {
                    C_TYPE y=vardata->GetRealIndex((C_TYPE)ci->y);
                    PVarInfo pv=(PVarInfo)vardata->items->At(y);
                    if(pv->type->isRGB())
                    {
                        if ((ci->x==2)&&((PObject)b->user1==NULL))return 0;
                        ci->rect.left += (ci->rect.bottom - ci->rect.top);
                    }
                }
            }
        }return 0;
        case TBN_DBLCLK:
        {
            CELLINFO* ci=(CELLINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(vardata && b)
            {
                C_TYPE y=vardata->GetRealIndex((C_TYPE)ci->y);
                PVarInfo pv=(PVarInfo)vardata->items->At(y);
                PObject po=(PObject)b->user1;
                if (pv->type->isRGB() && ((ci->x==2)||(ci->x==3)))
                {
                    CELLINFO _ci=*ci;
                    if (ci->x==2 && pv->type->isRGB() && po)
                    {
                        TProject*project=po->GetProject();
                        if(!project->readonly)
                        {
                            COLORREF c;
                            COLORREF*_c=(COLORREF*)GetObjectVar(po,y);
                            c= *_c;
                            //			 if(_ChoseColor(hwnd,c,NULL))
                            SendMessage((HWND)wParam,TB_GETCELLRECT,0,(LPARAM)&_ci);
                            POINT p;p.x=_ci.rect.left;p.y=_ci.rect.bottom;
                            ClientToScreen((HWND)wParam,&p);
                            if(ChoseColor(hwnd,p,c,(DWORD)hwnd,7))
                            {
                                char text[64];
                                RGBToStr(c,text);
                                INT16 id=GetObjectVarId(po->_class,pv->name);
                                SetObjectVarA(po,id,text);
                            }
                            else return TRUE;
                        }
                        else return TRUE;
                    }
                    if (ci->x==3)
                    {
                        TClass* _class=((TClass*)(b->user));
                        if(!(_class->Protected()))
                        {
                            COLORREF c=0;COLORREF realcolor=0;
                            if(pv->def)StrToRGB(pv->def,c,&realcolor);
                            //			if(_ChoseColor(hwnd,c,NULL))
                            SendMessage((HWND)wParam,TB_GETCELLRECT,0,(LPARAM)&_ci);
                            POINT p;p.x=_ci.rect.left;p.y=_ci.rect.bottom;
                            ClientToScreen((HWND)wParam,&p);
                            if(ChoseColor(hwnd,p,c,(DWORD)hwnd,7))
                            {
                                char s[256];
                                RGBToStr(c,s);
                                if(pv->def) delete pv->def;pv->def=0;
                                pv->def=NewStr(s);
                                vardata->flags|=VDF_MODIFY;
                            }
                            else return TRUE;
                        }
                        else return TRUE;
                    }
                    SendMessage((HWND)wParam,TB_UPDATECELL,0,(LPARAM)&_ci);
                }
            }
        }return TRUE;

        case TBN_SETTEXT:
        {
            CELLINFO* ci=(CELLINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(vardata && b)
            {
                C_TYPE y=vardata->GetRealIndex((C_TYPE)ci->y);
                PVarInfo pv=(PVarInfo)vardata->items->At(y);
                if (ci->x==2)
                {
                    PObject po=(PObject)b->user1;
                    if (po)
                    {
                        INT16 id=GetObjectVarId(po->_class,pv->name);
                        if(!SetObjectVarA(po,id,ci->text))
                        {ci->ok=FALSE;return 0;}
                        else
                            if(pv->type->isRGB())
                            {
                                CELLINFO _ci=*ci;
                                SendMessage((HWND)wParam,TB_GETCELLRECT,0,(LPARAM)&_ci);
                                InvalidateRect((HWND)wParam,&(_ci.rect),0);
                            }
                    }
                }
                if (ci->x==3)
                {
                    if (pv->def)delete pv->def;pv->def=0;
                    pv->def=NewStr(ci->text);
                    vardata->flags|=VDF_MODIFY;
                    if(pv->def && pv->type->isRGB())
                    {
                        CELLINFO _ci=*ci;
                        SendMessage((HWND)wParam,TB_GETCELLRECT,0,(LPARAM)&_ci);
                        InvalidateRect((HWND)wParam,&(_ci.rect),0);
                    }
                }
                if (ci->x==4)
                {
                    if (pv->info)delete pv->info;pv->info=0;
                    pv->info=NewStr(ci->text);
                    vardata->flags|=VDF_MODIFY;
                }
            }
            ci->ok=TRUE;
        }return 0;

        case TBL_LBUTTON:
        {
            TLABELINFO* info=(TLABELINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            C_TYPE y=vardata->GetRealIndex((C_TYPE)info->pos);
            PVarInfo pv=(PVarInfo)vardata->items->At(y);
            PObject po=(PObject)b->user1;
            if(po)
            {
                TProject * prj=po->GetProject();
                if(prj)
                {
                    if(prj->IsWatched(po,pv->name))
                        prj->RemoveWatch(po,pv->name);
                    else
                        prj->AddWatch(po,pv->name);
                    InvalidateRect(GetDlgItem(hwnd,IDC_VLABEL),NULL,0);
                }
            }
        }return 0;

        case TBL_GETTEXT:
        {
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info)
            {
                if (info->id==IDC_HLABEL && info->pos>=0 && info->pos<5)
                {
                    lstrcpy(info->text,vardata->GetTitle(info->pos));
                    return TRUE;
                }
                if (info->id==IDC_VLABEL)
                {
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    C_TYPE y=vardata->GetRealIndex((C_TYPE)info->pos);
                    PVarInfo pv=(PVarInfo)vardata->items->At(y);
                    PObject po=(PObject)b->user1;
                    if(po)
                    {
                        TProject * prj=po->GetProject();
                        if(prj && prj->IsWatched(po,pv->name)){lstrcpy(info->text,"•");return TRUE;}
                    }
                }
                lstrcpy(info->text,"");
                return TRUE;
            }
        }break;

        case TBN_GETTEXT:
        {
            CELLINFO* ci=(CELLINFO*)lParam;
            if (vardata && ci->y<vardata->items->count)
            {
                TPropertyButton*b=GetDlgPropertyButton(hwnd);
                C_TYPE y=vardata->GetRealIndex((C_TYPE)ci->y);
                PVarInfo pv=(PVarInfo)vardata->items->At(y);
                switch(ci->x)
                {
                    case 0:
                        lstrcpy(ci->text,pv->name);
                        if(pv->flags&VF_LEFT)ci->color=RGB(255,0,0);
                        break;
                    case 1:lstrcpy(ci->text,pv->type->GetClassName());break;
                    case 2:
                    {
                        PObject po=(PObject)b->user1;
                        if(po)
                        {
                            INT16 id=GetObjectVarId(po->_class,pv->name);
                            TSetVar *v=po->vardata;
                            GetObjectVarA(po,id,ci->text);
                            while(v)
                            {
                                if(!lstrcmpi(pv->name,v->name))
                                {
                                    if((v->data==0 && (0==*ci->text)) || (!lstrcmpi(ci->text,v->data)))
                                        ci->color=RGB(0,0,255);
                                    else
                                        ci->color=RGB(0,128,0);
                                    break;
                                }
                                v = v->next;
                            }
                        }
                        else
                            lstrcpy(ci->text,vardata->GetTitle(5));
                    }break;
                    case 3:lstrcpy(ci->text,pv->def?pv->def:"");break;
                    case 4:lstrcpy(ci->text,pv->info?pv->info:"");break;
                }
            }
            else
                lstrcpy(ci->text,"");
        } return 0;

        case PSP_ACTIVATE:
        {
            if (vardata->SubModifyed())
            {
                SendDlgItemMessage(hwnd,IDC_VARLIST,TB_SETRANGEY,0,vardata->GetNonLocalCount());
                CELLINFO ci;
                SendDlgItemMessage(hwnd,IDC_VARLIST,TB_GETACTIVE,0,(LPARAM)&ci);
                SendDlgItemMessage(hwnd,IDC_VARLIST,TB_UPDATECELL,0,(LPARAM)&ci);
                //TPropertyButton*b=(TPropertyButton*)lParam;
            }
            return TRUE;
        }

        case PSP_INITPAGE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            TClass* _class=((TClass*)(b->user));
            PObject po=(PObject)b->user1;
            //	__UpdOkButton(hwnd,_class);
            //	 RECT r,r1;
            HWND hl=GetDlgItem(hwnd,IDC_HLABEL);
            HWND vl=GetDlgItem(hwnd,IDC_VLABEL);
            HWND br=GetDlgItem(hwnd,IDC_VARLIST);
            int delta=2;
            int frombottom=0;
            int h=22;
            int w=20;
            RECT r;
            GetClientRect(hwnd,&r);
            int Height=r.bottom-(h+2*delta+frombottom);
            int Width=r.right-(w+2*delta);
            SetWindowPos(hl,0,delta+w,delta,Width,h,SWP_NOZORDER);
            SetWindowPos(vl,0,delta,delta+h,w,Height+delta,SWP_NOZORDER);
            SetWindowPos(br,0,w+delta,h+delta,Width,Height+delta,SWP_NOZORDER);
            SendMessage(br,TB_SETLABEL,(WPARAM)hl,(LPARAM)vl);
            int __mode= (_class->Protected())?CF_READONLY:0;
            int __mode2=(po && po->GetProject() && (!(po->GetProject()->readonly)))?0:CF_READONLY;
            if (!vardata)
                vardata=new TVarData(_class);

            TCollumn c[5]={TCollumn(0,90,CF_READONLY),TCollumn(0,70,CF_READONLY)
                           ,TCollumn(0,100,__mode2|CF_DRAW),TCollumn(0,100,__mode|CF_DRAW),TCollumn(0,400,__mode)};
            for(int i=0;i<5;i++)
            {
                c[i]._min/=2;c[i]._max*=2;
                SendDlgItemMessage(hwnd,IDC_VARLIST,TB_ADDCOLLUMN,-1,LPARAM(&(c[i])));
            }
            SendDlgItemMessage(hwnd,IDC_VARLIST,TB_SETRANGEY,0,vardata->GetNonLocalCount());
        }return TRUE;

        case PSP_CLOSE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            TClass* _class=((TClass*)(b->user));
            if (vardata)
            {
                if(!_class->Protected())
                    if (wParam==1) vardata->UpdateVarInfo();
                delete vardata;vardata=NULL;
            }
            return TRUE;
        }

        case PSP_VALID:
        {
        }
            return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}


//Выполняем компиляцию текста в окне hwnd
void CompileEqu(HWND hwnd)
{
    TPropertyButton*b=GetDlgPropertyButton(hwnd);
    TClass *_class=((TClass*)(b->user));
    if(!_class->Protected())
    {
        HWND edit=GetDlgItem(hwnd,IDC_EQUATION);
        int len=(int)SendMessage(edit,WM_GETTEXTLENGTH,0,0);
        char * text=new char[len+2];
        GetDlgItemText(hwnd,IDC_EQUATION,text,len+1);
        int cmpres=SetClassText(_class,vardata,text);
        if(cmpres)
        {
            lastcompilestatus=1;
            HWND _edit;
            InvalidateRect(edit,0,0);
            if(GetModelTextWindow(_class,NULL,&_edit))
            {
                SetWindowText(_edit,text);
            }
        }
        else
        {
            C_TYPE start,end;
            compiler->GetLastErrorPos(&start,&end);
#ifdef WIN32
            SendMessage(edit,EM_SETSEL,start,end);
#else
            SendMessage(edit,EM_SETSEL,0,MAKELONG(start,end));
#endif
            SetFocus(edit);
            lastcompilestatus=0;
        }
        char * result;
        switch (cmpres){
            case 1:
            {
                char s[64];
                LoadString(HResource,IDS_str232,s,sizeof(s));
                result=s;
                break;
            }
            case 2:
            {
                result=compiler->GetLastError();
                SendDlgItemMessage(hwnd,IDC_EQUATION,EM_SETMODIFY, 0, 0);
                break;
            }
            case 0:result=compiler->GetLastError();break;
        }
        SetDlgItemText(hwnd,IDC_COMPILESTATUS,result);
        delete text;
    }
}

// Class Equ Procedure
BOOL scCALLBACK ClassEquProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg){
        case EMN_FIND:
        {
            HWND edit=GetDlgItem(hwnd,IDC_EQUATION);
            switch(wParam){
                case 3:{SendMessage(edit,ESM_SEARCH,6,0);}return 0;
                case 1:
                    if(SearchTextDialog(hwnd,0))
                    {
                        SendMessage(edit,ESM_SEARCH,0,0);
                    }
                    return 0;
                case 2:
                {
                    int r=0;
                    if(r=SearchTextDialog(hwnd,1))
                    {
                        int flags=1;
                        if(r==2)flags|=16;
                        SendMessage(edit,ESM_SEARCH,flags,0);
                    }
                }
                    return 0;
            }
        }break;

        case EMN_HELP:
        {
            char *ps=(char *)lParam;
            HELP(ps);
        }return 0;
        case EMN_COMPILE:
        {
            CompileEqu(hwnd);
        }return 0;
        case EMN_RDOWN:
        {
            POINT pos;
            pos.x=(INT16)LOWORD(lParam);
            pos.y=(INT16)HIWORD(lParam);
            ClientToScreen(hwnd,&pos);
            TSTRATUMMENU sm;
            sm.hWindow=hwnd;
            sm.hType=SCMENU_SYNTAX;
            _TrackPopupMenuR(hwnd,pos,MENU_SYNTAX,&sm,FALSE);
        }break;
        case WM_COMMAND:
        {
            EV_COMMANDsimply
                    switch(command){
        case CM_EDITCUT:SendDlgItemMessage(hwnd,IDC_EQUATION,WM_CUT,0,0);break;
                case CM_EDITCOPY:SendDlgItemMessage(hwnd,IDC_EQUATION,WM_COPY,0,0);break;
                case CM_EDITPASTE:SendDlgItemMessage(hwnd,IDC_EQUATION,WM_PASTE,0,0);break;
                case CM_LOCALHELP:
                {
                    SendDlgItemMessage(hwnd,IDC_EQUATION,ESM_HELP,0,0);
                }break;
                case IDC_COMPILE:
                {
                    CompileEqu(hwnd);
                }
            }
        }return 0;
        case PSP_INITPAGE:
        {
            lastcompilestatus=1;
            TPropertyButton*b=(TPropertyButton*)lParam;
            HWND edit=GetDlgItem(hwnd,IDC_EQUATION);
            //SendMessage(edit,WM_SETFONT,(WPARAM)b->data,0);
            TClass* _class=((TClass*)(b->user));
            // __UpdOkButton(hwnd,_class);
            if (!vardata)
                vardata=new TVarData(_class);
            if (!_class->text)
                _class->UpdateItem(CR_TEXT);
            HWND _edit;
            if(GetModelTextWindow(_class,NULL,&_edit))
            {
                int l=GetWindowTextLength(_edit);
                if(l)
                {
                    char * s=new char[l+1];
                    GetWindowText(_edit,s,l+1);
                    SetWindowText(edit,s);
                    SendMessage(edit,EM_SETMODIFY, 0, 0);
                    delete s;
                }
                else
                    SetWindowText(edit,_class->text);
            }
            else
            {
                if (_class->text)
                    SetWindowText(edit,_class->text);
                SendMessage(edit,EM_SETMODIFY, 0, 0);
            }
            char name[256];
            GetClassName(edit,name,sizeof(name));
            if (!lstrcmpi(name,"SC_SyntaxEdit"))
            {
                SetDefaultSyntaxColor(_class,edit);
            }
            if (_class->Protected())
            {
                SendDlgItemMessage(hwnd,IDC_EQUATION,EM_SETREADONLY,1,0);
                EnableWindow(GetDlgItem(hwnd,IDC_COMPILE),0);
            }
            //SendMessage(edit,WM_SIZE, 0, 0);
        }return TRUE;
        case PSP_CLOSE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b->flags&PSP_INITIALIZE)
            {
                SendDlgItemMessage(hwnd,IDC_EQUATION,WM_SETFONT,(WPARAM)
                                   GetStockObject(ANSI_VAR_FONT),0);
            }
        }
            return TRUE;
        case PSP_VALID:
            if (wParam==1)
            {
                TPropertyButton*b=(TPropertyButton*)lParam;
                if (SendDlgItemMessage(hwnd,IDC_EQUATION,EM_GETMODIFY, 0, 0))
                {
#ifdef WIN32
                    ClassEquProc(hwnd,WM_COMMAND,MAKELONG(IDC_COMPILE,0),(LPARAM)hwnd);
#else
                    ClassEquProc(hwnd,WM_COMMAND,IDC_COMPILE,0);
#endif
                }
                if (lastcompilestatus)
                    b->Valid();
                else
                {
                    //MessageBox(hwnd,"Error in equation!","Error",MB_OK|MB_ICONINFORMATION);
                    MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG2),_MSG_ERROR,MB_OK|MB_ICONINFORMATION);
                    b->Invalid();
                }
                // Compile
            }
            return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

// Class Icon Procedure

struct TCLICON{
    HSP2D hsp;
    HOBJ2D hdib;
    HOBJ2D bitmap;
    int    sizex,sizey;
    int    perline;
    int    count;
    LPBITMAPINFO info16;
    LPBITMAPINFO infom;
    void *bytes16;
    void *bytesm;
    HGLOBAL and,xor;
    TCLICON();
    ~TCLICON();

};


TCLICON::TCLICON(){
    memset(this,0,sizeof(TCLICON));
};

TCLICON::~TCLICON(){
    GlobalUnlock(xor);
    GlobalUnlock(and);
    DeleteSpace2d(hsp);
};
void PaintCLICON(HWND hwnd,LPARAM lParam){
    TPropertyButton*b=GetDlgPropertyButton(hwnd);
    DRAWITEMSTRUCT*lpdis = (DRAWITEMSTRUCT *) lParam;

    //     SetPixel(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,0);
    //     return;
    TCLICON *icon=(TCLICON *)b->pdata;
    if (!icon)return;
    int i=lpdis->itemID;
    POINT org;
    org.y=i/icon->perline;
    org.x=i-(icon->perline*org.y);
    org.x*=32;
    org.y*=32;

    if (i<icon->count){

        HDC dc=CreateCompatibleDC(lpdis->hDC);
        HDC dcbuf=CreateCompatibleDC(lpdis->hDC);
        HBITMAP hb=CreateCompatibleBitmap(lpdis->hDC,34,34);
        HBITMAP hbuf=CreateCompatibleBitmap(lpdis->hDC,32,32);
        SelectObject(dc,hb);
        SelectObject(dcbuf,hbuf);
        if(lpdis->itemState&ODS_SELECTED){
            HGDIOBJ b=CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
            HGDIOBJ old=SelectObject(dc,b);
            PatBlt(dc,0,0,34,34,PATCOPY);
            SelectObject(dc,old);
            DeleteObject(b);
        }else  PatBlt(dc,0,0,34,34,WHITENESS);
        {
            HGDIOBJ pen=CreatePen(PS_SOLID,0,(lpdis->itemState&ODS_SELECTED)?RGB(255,0,0):RGB(255,255,255));
            HGDIOBJ old=SelectObject(lpdis->hDC,pen);
            Rectangle(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,
                      lpdis->rcItem.left+36,lpdis->rcItem.top+36);
            SelectObject(lpdis->hDC,old);
            DeleteObject(pen);
        }
        org.y=(icon->sizey-32)-org.y; //??????? Чудеса
        StretchDIBits(dcbuf,0,0,32,32,org.x,	org.y,32,32,icon->bytesm,icon->infom,DIB_RGB_COLORS ,SRCCOPY);
        BitBlt(dc,1,1,32,32,dcbuf,0,0,SRCAND);
        StretchDIBits(dcbuf,0,0,32,32,org.x,	org.y,32,32,icon->bytes16,icon->info16,DIB_RGB_COLORS ,SRCCOPY);
        BitBlt(dc,1,1,32,32,dcbuf,0,0,SRCINVERT);
        BitBlt(lpdis->hDC,lpdis->rcItem.left+1,lpdis->rcItem.top+1,34,34,dc,0,0,SRCCOPY);
        DeleteDC(dc);
        DeleteDC(dcbuf);
        DeleteObject(hb);
        DeleteObject(hbuf);
    }

};


TCLICON *SetLIcon(PClass _class,HWND hwnd,char *_s)
{
    TCLICON *icon=new TCLICON;
    icon->hsp=CreateSpace2d(0,scheme_path);
    icon->hdib=CreateRDoubleDib2d(icon->hsp,_s);
    POINT2D p0,p_1;p0.x=p0.y=0;p_1.x=p_1.y=-1;
    icon->bitmap=CreateDoubleBitmap2d(icon->hsp,icon->hdib,&p0,&p_1,&p0,&p_1,0);
    POINT2D p;GetObjectOrgSize2d(icon->hsp,icon->bitmap,NULL,&p);
    icon->sizex=p.x;
    icon->sizey=p.y;
    icon->count = (p.x/32)*(p.y/32);
    icon->perline=p.x/32;

    HWND list=GetDlgItem(hwnd,IDC_ICONLIST);
    SendMessage(list,LB_RESETCONTENT,0,0);
    SendMessage(list,LB_SETCOLUMNWIDTH,34,0);
    for(int i=0;i<icon->count;i++)SendMessage(list,LB_ADDSTRING,0,0);
    if(_class)SendMessage(list,LB_SETCURSEL,min(icon->count,_class->deficon),0);
    icon->and=GetAndDib2d(icon->hsp,icon->hdib);
    icon->xor=GetXorDib2d(icon->hsp,icon->hdib);

    icon->info16= (LPBITMAPINFO)GlobalLock(icon->xor);
    icon->infom = (LPBITMAPINFO)GlobalLock(icon->and);
    icon->bytes16=((char*)(icon->info16))+ sizeof(BITMAPINFOHEADER)+(4<<(icon->info16->bmiHeader.biBitCount));
    icon->bytesm= ((char*)(icon->infom))+ sizeof(BITMAPINFOHEADER)+(4<<(icon->infom->bmiHeader.biBitCount));

    return icon;
};

void SetDefaultSrc(HSP2D hsp,HOBJ2D obj,int id,char *file);
BOOL scCALLBACK VarIconProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            b->pdata=NULL;
            b->pdata = SetLIcon(NULL,hwnd,"varpoint.dbm");
            AddLayer(GetDlgItem(hwnd,IDC_LAYER),16,24,GetObjectLayer2d(b->hsp,b->iuser1));
            char s[256];
            wsprintf(s,"#%d",b->iuser1);
            SetDlgItemText(hwnd,IDC_HANDLE,s);
            POINT2D org,size;
            if(GetBitmapSrcRect(b->hsp,b->iuser1,&org,&size)){
                TCLICON *icon=(TCLICON *)b->pdata;
                //       int x=icon->sizex/32;
                //       int y=icon->sizey/32;
                int i=(org.x/32)+(org.y/32)*(icon->sizex/32);
                SendDlgItemMessage(hwnd,IDC_ICONLIST,LB_SETCURSEL,i,0);
            };
        }break;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                INT16 layer=(INT16)((SendDlgItemMessage(hwnd,IDC_LAYER,CB_GETCURSEL,0,0))+16);
                SetObjectLayer2d(b->hsp,b->iuser1,layer);
                TCLICON *icon=(TCLICON *)b->pdata;
                INT16 Nicon=(INT16)SendDlgItemMessage(hwnd,IDC_ICONLIST,LB_GETCURSEL,0,0);
                SetDefaultSrc(b->hsp,b->iuser1,Nicon,"varpoint.dbm");
            }else {b->Valid();}
        }return TRUE;
        case PSP_CLOSE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->pdata){
                TCLICON *icon=(TCLICON *)b->pdata;
                delete icon;
                b->pdata=NULL;
            }
        } return TRUE;

        case WM_COMMAND:{  }break;
        case WM_MEASUREITEM:{
            MEASUREITEMSTRUCT*lpmisCtl = (MEASUREITEMSTRUCT FAR*) lParam;
            lpmisCtl->itemWidth=37;
            lpmisCtl->itemHeight=36;
        }return TRUE;
        case WM_DRAWITEM:{PaintCLICON(hwnd,lParam);}return TRUE;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}

BOOL VarPlaceDialog(HWND hwnd,HSP2D hsp2d,HOBJ2D hobj,PClass _class)
{
    TPropertyButton*
            b=new TPropertyButton(IDS_str8,HResource,(char*)"Var_Icon",Class_property_Icon);
    b->Procedure=VarIconProc;
    b->user=_class;
    b->pdata=NULL;
    b->hsp=hsp2d;
    b->iuser1=hobj;
    int rez=CreatePropertySheet(hwnd,MAKEINTRESOURCE(IDS_str218),b,0);
    delete b;
    return rez;
};

BOOL scCALLBACK ClassIconProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){

    switch(msg){
        case WM_MEASUREITEM:{
            MEASUREITEMSTRUCT*lpmisCtl = (MEASUREITEMSTRUCT FAR*) lParam;
            //	 lpmisCtl->itemWidth=140; //37
            //	 lpmisCtl->itemHeight=50; //36
            lpmisCtl->itemWidth=38;
            lpmisCtl->itemHeight=36;

        }return TRUE;
        case WM_DRAWITEM:{PaintCLICON(hwnd,lParam);}return TRUE;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            PClass _class=((PClass)b->user);
            CheckDlgButton(hwnd,IDC_SIZEable,(_class->flags&CF_SIZEABLE)!=0);
            {
                HWND lib=GetDlgItem(hwnd,IDC_ICONLIB);
                TFileIterator fcls(scheme_path,"*.dbm");
                char *cif=_class->GetIconFile();
                while (fcls.GetNext()){
                    char *_s=fcls.GetFileName();
                    if(lstrcmpi(_s,"varpoint.dbm")){
                        SendMessage(lib,CB_ADDSTRING,0,(LPARAM)_s);
                    }
                }
                int i=(int)SendMessage(lib,CB_FINDSTRINGEXACT,-1,(LPARAM)cif);
                SendMessage(lib,CB_SETCURSEL,i,0);
                EnableWindow(lib,i>1);
                b->pdata = SetLIcon(_class,hwnd,cif);
                EnableWindow(lib,_class->Protected()==0);
            }
            CheckDlgButton(hwnd,IDC_USEICON,(_class->flags&CF_USEDEFICON)!=0);

            EnableWindow(GetDlgItem(hwnd,IDC_USEICON),_class->Protected()==0);
            EnableWindow(GetDlgItem(hwnd,IDC_SIZEable),_class->Protected()==0);

        }return TRUE;

        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_ICONLIB:if(wNotifyCode==CBN_SELCHANGE){
                        char s[256];
                        int i=SendDlgItemMessage(hwnd,IDC_ICONLIB,CB_GETCURSEL,0,0);
                        SendDlgItemMessage(hwnd,IDC_ICONLIB,CB_GETLBTEXT,i,(LPARAM)s);
                        TPropertyButton*b=GetDlgPropertyButton(hwnd);
                        PClass _class=((PClass)b->user);
                        if(b){
                            TCLICON *icon=(TCLICON *)b->pdata;
                            delete   icon;
                            b->pdata=NULL;
                            b->pdata = SetLIcon(_class,hwnd,s);
                            InvalidateRect(GetDlgItem(hwnd,IDC_ICONLIST),0,0);
                        };
                    }return TRUE;
            }
        };break;
        case PSP_CLOSE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            PClass _class=((PClass)b->user);
            if((wParam==1) && (!_class->Protected())){

                BOOL use=0;
                BOOL sizeable=(IsDlgButtonChecked(hwnd,IDC_SIZEable)!=0);

                if (sizeable!=((_class->flags&CF_SIZEABLE)!=0))
                { if (sizeable)_class->flags|=CF_SIZEABLE;else
                        _class->flags&=~CF_SIZEABLE;
                    _class->SetModify();
                }

                if (IsDlgButtonChecked(hwnd,IDC_USEICON))use=TRUE;
                INT16 Nicon=(INT16)SendDlgItemMessage(hwnd,IDC_ICONLIST,LB_GETCURSEL,0,0);
                char s[256];
                int i=SendDlgItemMessage(hwnd,IDC_ICONLIB,CB_GETCURSEL,0,0);
                SendDlgItemMessage(hwnd,IDC_ICONLIB,CB_GETLBTEXT,i,(LPARAM)s);
                BOOL newname=FALSE;
                if(lstrcmpi(_class->GetIconFile(),s)){
                    newname=TRUE;
                    if(_class->IconName){delete _class->IconName;_class->IconName=NULL;}
                    if(lstrcmpi(_class->GetIconFile(),s)){
                        _class->IconName=NewStr(s);
                    }
                }
                if ((use != ((_class->flags&CF_USEDEFICON)!=0))||
                    (_class->deficon!=Nicon)||(newname)){
                    _class->deficon=Nicon;
                    if (use)_class->flags|=CF_USEDEFICON;else
                        _class->flags&=~CF_USEDEFICON;
                    _class->SetModify();
                    UpdateClassDefIcon(_class);
                }
            };
            if (b->pdata){
                TCLICON *icon=(TCLICON *)b->pdata;
                delete icon;
                b->pdata=NULL;
            }
        }
            return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
            }else {b->Valid();}
        }return TRUE;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

// Class Main Procedure
void UpdClassName(HWND hwnd,HWND hw,PClass _class)
{
    if(_class )
    {
        char s[256];
        GetDlgItemText(hw,IDC_CLASSNAME,s,sizeof(s));
        AllTrim(s);
        char wintitle[128];
        LoadStr(IDS_str11,wintitle);
        lstrcat(wintitle,": ");
        lstrcat(wintitle,s);
        if(_class->Protected())
        {
            char ss[64];
            LoadString(HResource,IDS_str169,ss,sizeof(ss));
            lstrcat(wintitle,ss);
        }
        SetWindowText(hwnd,wintitle);
    }
}
void CheckClassFlag(HWND hwnd,PClass _class,int id,UINT32 f)
{
    int ns=IsDlgButtonChecked(hwnd,id);
    if ((ns!=0) != ((_class->flags & f)!=0))
    {
        if (ns)
            _class->flags |= f;

        else
            _class->flags &= ~f;

        _class->SetModify();
    }
}

BOOL scCALLBACK ClassFlagsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    _STDHELP(Class_property_Adding);
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            SetWindowLong(hwnd,DWL_USER,lParam);
            PClass _class=(PClass)lParam;
            CheckDlgButton(hwnd,IDC_DIASBLEEDIT,(_class->flags&CF_PROTECTED)!=0);
            CheckDlgButton(hwnd,IDC_STRUCTURE,(_class->flags&CF_STRUCTURE)!=0);
            CheckDlgButton(hwnd,IDC_NOSAVEVAR,(_class->flags&CF_NOSAVEVAR)!=0);

            CheckDlgButton(hwnd,IDC_AUTOCREATELINK,(_class->flags&CF_AUTOLINK)!=0);
            CheckDlgButton(hwnd,IDC_AUTODELETELINK,(_class->flags&CF_AUTORMLINK)!=0);

            BOOL _protected=_class->Protected();
            EnableWindow(GetDlgItem(hwnd,IDC_DIASBLEEDIT),!_protected);
            EnableWindow(GetDlgItem(hwnd,IDC_STRUCTURE),!_protected);
            EnableWindow(GetDlgItem(hwnd,IDC_NOSAVEVAR),!_protected);
            EnableWindow(GetDlgItem(hwnd,IDOK),!_protected);
            EnableWindow(GetDlgItem(hwnd,IDC_AUTOCREATELINK),!_protected);
            EnableWindow(GetDlgItem(hwnd,IDC_AUTODELETELINK),!_protected);

            EnableWindow(GetDlgItem(hwnd,IDC_HLPFILE),!_protected);
            if(_class->helpfile)SetDlgItemText(hwnd,IDC_HLPFILE,_class->helpfile);
            return TRUE;
        }

        case WM_COMMAND:
        {
            EV_COMMANDsimply;
            switch(command)
            {
                case IDOK:
                {
                    PClass _class = (PClass)GetWindowLong(hwnd, DWL_USER);

                    if(!_class->Protected())
                    {
                        CheckClassFlag(hwnd,_class,IDC_STRUCTURE      , CF_STRUCTURE);
                        CheckClassFlag(hwnd,_class,IDC_DIASBLEEDIT    , CF_PROTECTED);
                        CheckClassFlag(hwnd, _class, IDC_NOSAVEVAR      , CF_NOSAVEVAR);
                        CheckClassFlag(hwnd,_class,IDC_AUTOCREATELINK , CF_AUTOLINK);
                        CheckClassFlag(hwnd,_class,IDC_AUTODELETELINK , CF_AUTORMLINK);

                        HWND edit=GetDlgItem(hwnd,IDC_HLPFILE);
                        if(SendMessage(edit,EM_GETMODIFY, 0, 0 )){
                            if(_class->helpfile){delete _class->helpfile;_class->helpfile=0;}
                            char s[256];
                            GetWindowText(edit,s,sizeof(s));
                            if(lstrlen(s)){
                                _class->helpfile=NewStr(s);
                                _class->SetModify();
                            }
                        }
                    }
                    EndDialog(hwnd,TRUE);
                    return TRUE;
                }

                case IDCANCEL:
                    EndDialog(hwnd,FALSE);
                    return TRUE;
            }
        }break;
    }
    return FALSE;
}
BOOL scCALLBACK ClassMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TSpaceWindow* w=(TSpaceWindow*)GetWindowLong(GetDlgItem(hwnd,IDC_CLASSLOGO),0);
            if(w){
                if(GetColorPalette2d(w->hsp))SetColorPalette2d(w->hsp,NULL);
            }
            TPropertyButton*b=(TPropertyButton*)lParam;
            TClass *_class=((TClass*)b->user);
            if (_class){SetDlgItemText(hwnd,IDC_CLASSNAME,_class->name);
                char filename[256];
                _class->GetClassFile(filename,FALSE);
                int l=lstrlen(filename);
                if (l>5){ lstrcpy(filename+(l-4),".CLS");	}

                SetDlgItemText(hwnd,IDC_CLASSFILE,filename);
                _class->GetCreateTime(filename,sizeof(filename));
                SetDlgItemText(hwnd,IDC_TIME,filename);

                if (!_class->description) _class->UpdateItem(CR_INFO);
                if (_class->description)
                    SetDlgItemText(hwnd,IDC_DESCRIPTION,_class->description);
                SendDlgItemMessage(hwnd,IDC_DESCRIPTION,EM_SETMODIFY,0, 0);
                if (_class->Protected()){
                    SendDlgItemMessage(hwnd,IDC_DESCRIPTION,EM_SETREADONLY,1,0);
                    SendDlgItemMessage(hwnd,IDC_CLASSNAME,EM_SETREADONLY,1,0);
                }
                PClassListInfo pcli=_class->GetClassListInfo();
                if(pcli){
                    HWND libs=GetDlgItem(hwnd,IDC_LIBS);
                    SendMessage(libs,CB_ADDSTRING,0,(LPARAM)pcli->library->path);
                    SendMessage(libs,CB_SETCURSEL,0,0);
                    EnableWindow(libs,0);
                }
            }

        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                char name[80];
                TPropertyButton*b=(TPropertyButton*)lParam;
                TClass *_class=((TClass*)b->user);

                GetDlgItemText(hwnd,IDC_CLASSNAME,name,79);
                AllTrim(name);
                if (lstrcmp(_class->name,name)){
                    ChangeClassName(_class,name);
                }

                // Далее описание
                if (SendDlgItemMessage(hwnd,IDC_DESCRIPTION,EM_GETMODIFY, 0, 0)){
                    int len=(int)SendDlgItemMessage(hwnd,IDC_DESCRIPTION,WM_GETTEXTLENGTH,0,0);
                    if (len){
                        char * text=new char[len+2];
                        GetDlgItemText(hwnd,IDC_DESCRIPTION,text,len+1);
                        if (_class->description){
                            if (lstrcmp(_class->description,text)){
                                delete _class->description;
                                _class->description=NewStr(text);_class->SetModify();
                            }
                        }else {_class->description=NewStr(text);_class->SetModify();}
                        delete text;
                    }else{
                        if (_class->description)
                        {
                            delete _class->description;
                            _class->description=NULL;
                            _class->NS.description=1;
                            _class->SetModify();
                        }
                    }
                }
            }return TRUE;
        case PSP_VALID:{
            if (wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                if (b->flags&PSP_INITIALIZE){
                    int len=(int)SendDlgItemMessage(hwnd,IDC_CLASSNAME,WM_GETTEXTLENGTH,0,0);
                    if (len<3 || len>64){
                        _Error(EM_BADCLASSNAMELEN);
                        b->Invalid();
                        return FALSE;
                    }
                    char name[80];
                    GetDlgItemText(hwnd,IDC_CLASSNAME,name,79);
                    AllTrim(name);

                    if (!IsNameValid(name)){
                        _Error(EM_CLASSNAMEERROR);
                        //	MessageBox(hwnd,"Неверное имя класса","Error",MB_OK|MB_ICONSTOP);
                        b->Invalid(); return FALSE;
                    }
                    PClassListInfo pcli=GetClassListInfo(name);
                    if (pcli &&(!(pcli->flags&CLF_LOADED) || pcli->_class!=b->user)){
                        _Error(EM_DUPLICATENAME,0,name);
                        b->Invalid(); return FALSE;
                    }
                    b->Valid();
                }}
            return TRUE;
        };
        case WM_INITDIALOG:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TClass *_class=((TClass*)b->user);
            if (_class){SetDlgItemText(hwnd,IDC_CLASSNAME,_class->name);}
            UpdClassName(GetParent(hwnd),hwnd,_class);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMAND;
            switch(command){
                case IDC_ADVANSED:{
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    if(b){
                        DialogBoxParam(HResource,MAKEINTRESOURCE(CLASS_FLAGS),hwnd,ClassFlagsProc,(long)b->user);
                    }}return FALSE;
                case IDC_CLASSNAME:if(wNotifyCode==EN_UPDATE)
                    {
                        TPropertyButton*b=GetDlgPropertyButton(hwnd);
                        if(b){
                            TClass *_class=((TClass*)b->user);
                            UpdClassName(GetParent(hwnd),hwnd,_class);
                        }
                    }return FALSE;
            }

        }break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
// Class Child Procedure


BOOL scCALLBACK ClassChildProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{

            EditSubClass(hwnd,IDC_X,0,0,1,UDF_DOUBLE);
            EditSubClass(hwnd,IDC_Y,0,0,1,UDF_DOUBLE);
            EditSubClass(hwnd,IDC_SIZEX,1,0,1,UDF_DOUBLE|UDF_MIN);
            EditSubClass(hwnd,IDC_SIZEY,1,0,1,UDF_DOUBLE|UDF_MIN);

            TPropertyButton*b=(TPropertyButton*)lParam;
            PClass _parent=((PClass)b->pdata);
            HOBJ2D handle=b->iuser1;
            if(_parent){
                //		__UpdOkButton(hwnd,_parent);
                INT16 num=_parent->GetChildById(handle);

                SetDlgItemText(hwnd,IDC_CHILDNAME,_parent->child[num].name);
                if (_parent->Protected()){
                    SendDlgItemMessage(hwnd,IDC_CHILDNAME,EM_SETREADONLY,1,0);
                }
                char s[256];
                wsprintf(s,"#%d",handle);
                SetDlgItemText(hwnd,IDC_HANDLE,s);
                if (_parent->scheme && _parent->scheme->Loaded()){
                    HSP2D hsp=_parent->scheme->GetSpace();
                    AddLayer(GetDlgItem(hwnd,IDC_LAYER),16,24,GetObjectLayer2d(hsp,handle));
                    SetDlgOrgSize2d(hwnd,hsp,handle);
                    if(!(_parent->Protected())){
                        EnableWindow(GetDlgItem(hwnd,IDC_X),1);
                        EnableWindow(GetDlgItem(hwnd,IDC_Y),1);
                        if ((_parent->child[num].type->GetClass()->flags)&CF_SIZEABLE){
                            EnableWindow(GetDlgItem(hwnd,IDC_SIZEX),1);
                            EnableWindow(GetDlgItem(hwnd,IDC_SIZEY),1);
                        }}
                    EnableWindow(GetDlgItem(hwnd,IDC_OBJINFO),!(_parent->Protected()));
                    EnableWindow(GetDlgItem(hwnd,IDC_LAYER),!(_parent->Protected()));
                    INT16 _size=GetObjectData2d(hsp,handle,UD_OBJINFO,NULL,0,0);
                    char *str=NULL;
                    if(_size){
                        str=new char[_size+1];
                        str[_size]=0;
                        GetObjectData2d(hsp,handle,UD_OBJINFO,str,0,_size);
                        SetDlgItemText(hwnd,IDC_OBJINFO,str);
                        delete str;
                    }
                }else{
                    EnableWindow(GetDlgItem(hwnd,IDC_LAYER),0);
                    EnableWindow(GetDlgItem(hwnd,IDC_OBJINFO),0);
                    EnableWindow(GetDlgItem(hwnd,IDC_CHILDNAME),0);

                    SetDlgItemDouble(hwnd,IDC_X,_parent->child[num].x);
                    SetDlgItemDouble(hwnd,IDC_Y,_parent->child[num].y);
                }

            }
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                if (b->flags&PSP_INITIALIZE){
                    PClass _parent=((PClass)b->pdata);
                    if(_parent){
                        HOBJ2D handle=b->iuser1;
                        INT16 num=_parent->GetChildById(handle);

                        if (SendDlgItemMessage(hwnd,IDC_CHILDNAME,EM_GETMODIFY, 0, 0L)){
                            int l=(int)SendDlgItemMessage(hwnd,IDC_CHILDNAME,WM_GETTEXTLENGTH, 0, 0L);
                            char *s=NULL;
                            if(l){
                                s = new char[l+1];
                                GetDlgItemText(hwnd,IDC_CHILDNAME,s,l+1);}
                            if (_parent->child[num].name)delete _parent->child[num].name;
                            _parent->child[num].name=s;
                            _parent->SetModify();

                            if (_parent->scheme && _parent->scheme->Loaded()){
                                HSP2D hsp=_parent->scheme->GetSpace();
                                SetObjectName2d(hsp,handle,s);
                            }
                        }
                        if (_parent->scheme && _parent->scheme->Loaded()){
                            HSP2D hsp=_parent->scheme->GetSpace();
                            INT16 layer=(INT16)((SendDlgItemMessage(hwnd,IDC_LAYER,CB_GETCURSEL,0,0))+16);
                            SetObjectLayer2d(hsp,handle,layer);

                            POINT2D org,size;
                            if(
                               IsDoubleValid(hwnd,IDC_X,org.x,-32760,32760)&&
                               IsDoubleValid(hwnd,IDC_Y,org.y,-32760,32760)&&
                               IsDoubleValid(hwnd,IDC_SIZEX,size.x,0,32760)&&
                               IsDoubleValid(hwnd,IDC_SIZEY,size.y,0,32760))
                            {
                                POINT2D _org,_size;
                                GetObjectOrgSize2d(hsp,handle,&_org,&_size);
                                BOOL u=FALSE;
                                if (_org.x!=org.x ||_org.y!=org.y){
                                    HWND HWindow=GetSpaceWindow2d(hsp);
                                    TSchemeWindow*scheme=(TSchemeWindow*)SendMessage(HWindow,scwnd_message,0,0);
                                    if(scheme){
                                        scheme->SetObjectPos(org);
                                        scheme->UpdateScrollers();
                                        u=TRUE;
                                    }
                                }
                                if ((_parent->child[num].type->GetClass()->flags)&CF_SIZEABLE){
                                    if (_size.x!=size.x ||_size.y!=size.y) u|=SetObjectSize2d(hsp,handle,&size);
                                }
                                if(u){
                                    _parent->child[num].x=org.x;
                                    _parent->child[num].y=org.y;
                                }
                            }
                            HWND edit=GetDlgItem(hwnd,IDC_OBJINFO);
                            int _size=(int)GetWindowTextLength(edit);
                            char * str=NULL;
                            if(_size){
                                _size++;
                                str=new char[_size];
                                GetWindowText(edit,str,_size);
                            }
                            SetObjectData2d(hsp,handle,UD_OBJINFO,str,0,_size);
                            if(str)delete str;
                        }
                    }}
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            b->Valid();
            if (b->flags&PSP_INITIALIZE){
                PClass _parent=((PClass)b->pdata);
                if (_parent->scheme && _parent->scheme->Loaded()){
                    POINT2D org,size;
                    if(
                       IsDoubleValid(hwnd,IDC_X,org.x,-32760,32760)&&
                       IsDoubleValid(hwnd,IDC_Y,org.y,-32760,32760)&&
                       IsDoubleValid(hwnd,IDC_SIZEX,size.x,0,32760)&&
                       IsDoubleValid(hwnd,IDC_SIZEY,size.y,0,32760)){b->Valid(); }else b->Invalid();
                }
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

// Class Dialog
BOOL ClassDialog(HWND hwnd,PClass _class,PObject _object,PLibrary _lib,PClass*__class,PClass _parent,HOBJ2D handle)
{
    if (_object && !_class)
        _class=_object->_class;

    int newclass=(_class==NULL);
    if (_parent==NULL && _object && _object->parent)
        _parent=_object->parent->_class;

    if (handle==0 && _object)handle=_object->GetHandle(NULL);
#ifdef LOGON
    {
        char s[256];
        char * __s=_class?_class->name:"NULL";
        wsprintf(s,"Зашел в ClassDialog(%s)",__s);
        LogMessage(s);
        TLOGoffset __offset(2);
    }
#endif
    if (newclass)
    {
        char s[256];lstrcpy(s,"StratumClass");
        GetUniqueClassName(s);
        _class=CreateNewClass(s,NULL,NULL,_lib);
    }
    if (!_class)	return 0;
    TPropertyButton*b=new TPropertyButton(IDS_str5,HResource,"Class_Main",Class_property_Main);  // Основное
    b->Procedure=ClassMainProc;

    TPropertyButton* b1 = new TPropertyButton(IDS_str6,HResource,"Class_Vars",Class_property_Vars); // Переменные
    b1->Procedure = ClassVarProc;
    b->Insert(b1);
    if(SCGetScVar("use_syntax_edit",0) && InitSyntax())
        b1=new TPropertyButton(IDS_str7,HResource,"Class_Syntax",Class_property_Text); // Текст
    else
        b1=new TPropertyButton(IDS_str7,HResource,"Class_Equ",Class_property_Text); // Текст
    b1->Procedure=ClassEquProc;
    b->Insert(b1);
    b1=b;
    while (b1)
    {
        b1->user=_class;
        b1->user1=_object;
        b1=b1->next;
    };
    b1=new TPropertyButton(IDS_str8,HResource,"Class_Icon",Class_property_Icon); //Иконка
    b1->Procedure=ClassIconProc;
    b1->user=_class;
    b1->pdata=NULL;
    b->Insert(b1);
    if (handle && _parent)
    {
        b1=new TPropertyButton(IDS_str9,HResource,"Class_Child",Class_property_Object); //Объект
        b1->Procedure=ClassChildProc;
        b1->pdata=_parent;
        b1->user=_class;
        b1->iuser1=handle;
        b->Insert(b1);
        if (_parent->scheme && _parent->scheme->Loaded())
        {
            b1=new TPropertyButton(IDS_str10,HResource,"Obj_Hyper",Class_property_Hyperbase); //Гипербаза
            b1->Procedure=ObjHyperProc;
            b->Insert(b1);
            b1->hsp=_parent->scheme->GetSpace();
            b1->iuser1=handle;
        }
    }
    int toactivate=2;
    char s[256]="";
    if(SCGetScVarS("LastClassDlg",s))
    {
        b1=b;
        int i=0;
        while (b1)
        {
            if(!lstrcmpi(b1->templ,s)){toactivate=i;break;}
            b1=b1->next;i++;
        };
    }
    int rez=CreatePropertySheet(hwnd,"",b,newclass?0:toactivate);
    if(_lastactive)
    {
        SCSetScVarS("LastClassDlg",_lastactive->templ);
        _lastactive=NULL;
    }
    if (rez)
    {
        TClassListInfo *pcli=_class->GetClassListInfo();
        if(_class->flags&CF_MODIFY)_class->UpdateClassFile();
        if(newclass)
        {
            UpdateClassWindows(UW_CREATE,UW_CLASS,pcli,_lib);
            if (__class)*__class=_class;
        }
    }
    else
        if (newclass) DeleteClass(_class->name);
    delete b;
    return rez;
};



//***********************************************************************//
//                                                                       //
//                Далее для Link Диалог.                                 //
//                                                                       //
//***********************************************************************//

class TLinkEntryItem:public TLinkEntryInfo{
public:
    BOOL autolink;
    TLinkEntryItem(){autolink=TRUE; }
};
class TLinkData{
public:
    HOBJ2D obj1Id;
    HOBJ2D obj2Id;
    HOBJ2D handle;       // дескриптор на схеме
    HOBJ2D varhandle;    //
    HSP2D  hsp;
    PClass _c1,_c2;   // классы соединяемых объектов
    DWORD flags;
    TCollection *items;
    TLinkData(PClass _class,HSP2D,HOBJ2D id);
    ~TLinkData();
    C_TYPE Insert(char*,char*,BYTE flags=0); // 1 - auto link
    BOOL Delete(C_TYPE);
    BOOL CreateInfo(PLinkInfo);
private:
    void DeleteEntry(C_TYPE);
};

TLinkData::TLinkData(PClass _class,HSP2D _hsp,HOBJ2D id){
    INT16 num;
    PLinkInfo _link=_class->GetLinkById(id,&num);
    if (!_link) _Error(EM_BADLINKHANDLE,(INT16)id);
    obj1Id=_link->obj1Id;
    obj2Id=_link->obj2Id;
    handle=_link->handle;
    varhandle=_link->varhandle;
    flags=_link->flags;
    hsp=_hsp;
    int i;
    if (obj1Id){
        i=_class->GetChildById(obj1Id);
        if (i>-1)_c1=_class->child[i].type->_class;}else _c1=_class;

    if (obj2Id){
        i=_class->GetChildById(obj2Id);
        if (i>-1)_c2=_class->child[i].type->_class;}else _c2=_class;

    items=new TCollection(_class->linkcount,4);
    for(i=0;i<_link->count;i++)
        Insert(_link->items[i].name1,_link->items[i].name2);
};

TLinkData::~TLinkData(){
    for(C_TYPE i=0;i<items->count;i++)DeleteEntry(i);
    items->DeleteAll();
    delete items;
};
void TLinkData::DeleteEntry(INT16 i){
    if (i>-1){
        TLinkEntryItem* entry=(TLinkEntryItem*)(items->At(i));
        delete entry->name1;
        delete entry->name2;
        delete entry;
    }
};

C_TYPE TLinkData::Insert(char* n1,char* n2,BYTE flags){
    TLinkEntryItem* entry=new TLinkEntryItem;
    entry->name1=NewStr(n1);
    entry->name2=NewStr(n2);
    entry->autolink=(flags&1)!=0;
    return items->Insert(entry);
};

BOOL TLinkData::Delete(C_TYPE i){
    DeleteEntry(i);
    items->AtDelete(i);
    return TRUE;
};

BOOL TLinkData::CreateInfo(PLinkInfo _link){
    _link->obj1Id=obj1Id;
    _link->obj2Id=obj2Id;
    _link->handle=handle;
    _link->varhandle=varhandle;
    _link->flags=flags;
    _link->count=items->count;

    _link->items=new TLinkEntryInfo[items->count];
    for(int i=0;i<items->count;i++){
        TLinkEntryItem* entry=(TLinkEntryItem*)(items->At(i));
        _link->items[i].name1=NewStr(entry->name1);
        _link->items[i].name2=NewStr(entry->name2);
    }
    return 1;
};


BOOL scCALLBACK LinkMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case TBN_SETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            if(!lstrcmp(ci->text,""))return 0;
            char * pst=ci->text;
            char __ss_[256];
            char *pp=strstr(ci->text," : ");
            if(pp){
                lstrcpyn(__ss_,ci->text,(pp-ci->text)+1);
                pst=__ss_;
            }

            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            TLinkData *data=(TLinkData *)b->user;
            TLinkEntryItem* entry=(TLinkEntryItem*)(data->items->At((int)ci->y));
            if (ci->x==0){
                delete(entry->name1);entry->name1=NewStr(pst);
                int id=GetObjectVarId(data->_c1,pst);
                int id2=GetObjectVarId(data->_c2,entry->name2);
                BOOL dt=(data->_c1->vars[id].type!=data->_c2->vars[id2].type);
                if (dt||entry->autolink){
                    int index=-1;
                    BOOL namematch=FALSE;
                    for(int i=0;i<data->_c2->varcount;i++){
                        if ((data->_c2->vars[i].type==data->_c1->vars[id].type)&&
                            ((data->_c2->vars[i].flags&VF_LOCAL)==0))
                        {
                            if(!lstrcmpi(data->_c2->vars[i].name,data->_c1->vars[id].name)){
                                namematch=TRUE;
                                index=i;    break;
                            }
                            if(index==-1)index=i;
                        }}
                    if(!dt && !namematch)return 0; // Не устанавливаем имя если не поменяся тип
                    // и не совпали имена
                    CELLINFO ci1;
                    ci1.x=2;ci1.y=ci->y;
                    ci1.text=data->_c2->vars[index].name;
                    delete(entry->name2);entry->name2=NewStr(ci1.text);
                    SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_SETTEXT,0,(LPARAM)&ci1);
                    if(dt)entry->autolink=TRUE;
                }  }
            if (ci->x==2){
                delete(entry->name2);entry->name2=NewStr(pst);
                entry->autolink=FALSE;
            }
        }return 0;
        case TBN_UPDATECOMBO:{
            HWND combo=(HWND)wParam;
            CELLINFO* ci=(CELLINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            TLinkData *data=(TLinkData *)b->user;
            TLinkEntryInfo* entry=(TLinkEntryInfo*)(data->items->At((int)ci->y));
            if(ci->x==0){
                int k=0,focus=0;
                for(int i=0;i<data->_c1->varcount;i++){
                    for(int j=0;j<data->_c2->varcount;j++){
                        if ((data->_c2->vars[j].type==data->_c1->vars[i].type)&&
                            ((data->_c1->vars[i].flags&VF_LOCAL)==0)&&
                            ((data->_c2->vars[j].flags&VF_LOCAL)==0))
                        {
                            char ss[512];
                            char *ps=data->_c1->vars[i].name;
                            if(data->_c1->vars[i].info){
                                wsprintf(ss,"%s : %s",ps,data->_c1->vars[i].info);
                                ps=ss;
                            }
                            SendMessage(combo,CB_ADDSTRING,0,(LPARAM)ps);
                            if (!lstrcmp(data->_c1->vars[i].name,entry->name1))focus=k;
                            k++;break;
                        }
                    }
                }
                SendMessage(combo,CB_SETCURSEL,focus,0);
            }
            if(ci->x==2){
                PClassListInfo type=0;
                int i;
                for(i=0;i<data->_c1->varcount;i++){
                    if (!lstrcmp(data->_c1->vars[i].name,entry->name1)){
                        type=data->_c1->vars[i].type;break;}
                }
                int j=0,k=0;
                for(i=0;i<data->_c2->varcount;i++){
                    if ((data->_c2->vars[i].type==type)&&
                        ((data->_c2->vars[i].flags&VF_LOCAL)==0)){
                        char ss[512];
                        char *ps=data->_c2->vars[i].name;
                        if(data->_c2->vars[i].info){
                            wsprintf(ss,"%s : %s",ps,data->_c2->vars[i].info);
                            ps=ss;
                        }
                        SendMessage(combo,CB_ADDSTRING,0,(LPARAM)ps);
                        if (!lstrcmp(data->_c2->vars[i].name,entry->name2))j=k;
                        k++;
                    }
                }
                SendMessage(combo,CB_SETCURSEL,j,0);
            }
        };return 0;
        case TBN_GETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            TLinkData *data=(TLinkData *)b->user;
            TLinkEntryInfo* entry=(TLinkEntryInfo*)(data->items->At((int)ci->y));

            switch(ci->x){
                case 0:lstrcpy(ci->text,entry->name1);break;
                case 1:lstrcpy(ci->text,"<===>");break;
                case 2:lstrcpy(ci->text,entry->name2);
            }
        };return 0;
        case TBL_GETTEXT:{
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info){
                if (info->id==IDC_HLABEL && (info->pos==0 || info->pos==2)){
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    TLinkData *data=(TLinkData *)b->user;
                    lstrcpy(info->text,(info->pos==0)?data->_c1->name:data->_c2->name);
                }else lstrcpy(info->text,"");}
        };break;

        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            PClass _class=(PClass)b->user1;
            if(_class->Protected())EnableWindow(GetDlgItem(GetParent(hwnd),IDOK),0);
            TLinkData *data=(TLinkData *)b->user;
            RECT r,r1;
            {
                HWND hl=GetDlgItem(hwnd,IDC_HLABEL);
                HWND br=GetDlgItem(hwnd,IDC_LINKVARLIST);
                GetWindowRect(hl,&r);
                GetWindowRect(br,&r1);
                ScreenToClient(hwnd,(POINT*)&r);
                ScreenToClient(hwnd,((POINT*)&r)+1);
                ScreenToClient(hwnd,(POINT*)&r1);
                ScreenToClient(hwnd,((POINT*)&r1)+1);
                int h=22;
                SetWindowPos(hl,0,0,0,r.right-r.left,h,SWP_NOZORDER|SWP_NOMOVE);
                SetWindowPos(br,0,r.left,r.top+h,
                             r1.right-r1.left,r1.bottom-(r.top+h),SWP_NOZORDER);
                SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_SETLABEL,
                                   (WPARAM)GetDlgItem(hwnd,IDC_HLABEL),
                                   0);
            } int size=(((r.right-r.left)-GetSystemMetrics(SM_CXHTHUMB))-46)/2;
            TCollumn c[3]={TCollumn(0,size,CF_COMBOLIST),TCollumn(0,40,CF_READONLY),TCollumn(0,size,CF_COMBOLIST)};
            for(int i=0;i<3;i++)
                SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_ADDCOLLUMN,-1,LPARAM(&(c[i])));
            SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_SETRANGEY,0,data->items->count);
            EnableWindow(GetDlgItem(hwnd,IDC_REMOVELINK),data->items->count>0);
            SendDlgItemMessage(hwnd,IDC_CLASS1,TW_SETCLASS,0,(LPARAM)data->_c1);
            SendDlgItemMessage(hwnd,IDC_CLASS2,TW_SETCLASS,0,(LPARAM)data->_c2);

            BOOL addenable=FALSE;
            for(i=0;i<data->_c1->varcount;i++){
                for(int j=0;j<data->_c2->varcount;j++){
                    if ((data->_c2->vars[j].type==data->_c1->vars[i].type)&&
                        ((data->_c1->vars[i].flags&VF_LOCAL)==0)&&
                        ((data->_c2->vars[j].flags&VF_LOCAL)==0)){
                        addenable=TRUE;
                        i=data->_c1->varcount;
                        break;
                    }
                }};
            EnableWindow(GetDlgItem(hwnd,IDC_ADDLINK),addenable);

        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
            };return TRUE;
        case PSP_VALID:{
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if (b==NULL)return TRUE;
            TLinkData *data=(TLinkData *)b->user;
            switch(command){
                case  IDC_ADDLINK:{ // Добавление
                    char *name2=NULL,*name1=NULL;

                    for(int c=0;c<2;c++)//делаем два перебора списка переменных
                    {
                        name2=name1=NULL;
                        for(INT16 j=0;j<data->_c1->varcount;j++)//перебираем переменные первого класса
                        {
                            if((data->_c1->vars[j].flags&VF_LOCAL)==0)//если переменная не локальная
                            {
                                BOOL _try=TRUE;
                                if(c==0)//если перебираем список переменных первый раз
                                {//то проверяем участвует ли переменная в связи
                                    name2=name1=NULL;
                                    for(C_TYPE k=0;k<data->items->count;k++)//перебираем уже имеющиеся элементы связи
                                    {
                                        TLinkEntryInfo*info=(TLinkEntryInfo*)data->items->At(k);
                                        if(!lstrcmpi(info->name1,data->_c1->vars[j].name))//если переменная уже участвует в связи
                                        {_try=0;break;}//то пропускаем ее
                                    }
                                }
                                if(_try)//если переменная еще не участвует в связи
                                {
                                    for(INT16 i=0;i<data->_c2->varcount;i++)//перебираем список переменных второго класса
                                    {
                                        if((data->_c1->vars[j].type==data->_c2->vars[i].type)&&//если тип у переменных одинаковый
                                           ((data->_c2->vars[i].flags&VF_LOCAL)==0))//и вторая переменная не локальная
                                        {
                                            if(name2)//если вторая переменная уже выбрана
                                            {
                                                if(!lstrcmpi(name1,data->_c2->vars[i].name))//если имя первой переменной совпадает с именем второй переменной
                                                {
                                                    name2=data->_c2->vars[i].name;//выбираем вторую переменную
                                                    j=data->_c1->varcount;//заканчиваем перебор переменных первого класса
                                                    break;
                                                };
                                            }else{
                                                name2=data->_c2->vars[i].name;//выбираем первую и вторую переменные
                                                name1=data->_c1->vars[j].name;
                                            }
                                        }
                                    }// i
                                }
                                if(name1 && name2)//если переменные выбраны
                                {
                                    if(c==0)//и перебираем переменные первый раз
                                    {
                                        if(lstrcmpi(name1,name2))//и имена переменных не совпадают
                                            name2=NULL;//снимаем выбор со второй переменной
                                        else
                                            break;//или заканчиваем перебор переменных
                                    }
                                }
                            }//j
                            if(name1 && name2)break;//если переменные выбраны, то закачиваем их перебор
                        };
                    }
                    if(name2 && name1)//если переменные выбраны
                    {
                        data->Insert(name1,name2,1);//то добавляем элемент связи
                        SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_SETRANGEY,0,data->items->count);
                        EnableWindow(GetDlgItem(hwnd,IDC_REMOVELINK),1);
                    }
                }//c
                    return TRUE;
                case  IDC_REMOVELINK: // Удаление
                {
                    CELLINFO ci;
                    SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_GETACTIVE,0,(LPARAM)&ci);
                    data->Delete((INT16)ci.y);
                    INT16 a=(INT16)ci.y;
                    if (a>=data->items->count)a=(C_TYPE)(data->items->count-1);ci.y=a;
                    SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_SETRANGEY,0,data->items->count);
                    SendDlgItemMessage(hwnd,IDC_LINKVARLIST,TB_SETACTIVE,0,(LPARAM)&ci);
                    EnableWindow(GetDlgItem(hwnd,IDC_REMOVELINK),data->items->count>0);
                }	return TRUE;
            }
        }break;// WM_COMMAND
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL scCALLBACK LinkMiskProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            TLinkData *data=(TLinkData *)b->user;
            HOBJ2D pen=GetPenObject2d(data->hsp,data->handle);
            COLORREF rgb;
            rgb=GetPenColor2d(data->hsp,pen);
            SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_SETCOLOR,0,rgb);
            INT16 width=GetPenWidth2d(data->hsp,pen);
            SetDlgItemInt(hwnd,IDC_LINKWIDTH,width,0);
            EditSubClass(hwnd,IDC_LINKWIDTH,0,10,1,UDF_MINMAX);
            CheckDlgButton(hwnd,IDC_DISABLELINK,(data->flags&LF_DISABLED)!=0);
            CheckDlgButton(hwnd,IDC_AUTOGENLINK,(data->flags&LF_AUTOLINK)!=0);


            AddLayer(GetDlgItem(hwnd,IDC_LAYER),24,32,GetObjectLayer2d(data->hsp,data->handle));
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                TLinkData *data=(TLinkData *)b->user;
                BOOL er;
                INT16 w=(INT16)GetDlgItemInt(hwnd,IDC_LINKWIDTH,&er,0);
                HOBJ2D pen=GetPenObject2d(data->hsp,data->handle);

                COLORREF rgb=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,0);
                SetPenColor2d(data->hsp,pen,rgb);
                INT16 layer=(INT16)((SendDlgItemMessage(hwnd,IDC_LAYER,CB_GETCURSEL,0,0))+24);
                SetObjectLayer2d(data->hsp,data->handle,layer);

                BOOL dis=(IsDlgButtonChecked(hwnd,IDC_DISABLELINK)!=0);

                //		if (dis!=((data->flags&LF_DISABLED)!=0)) {
                if (dis){data->flags|=LF_DISABLED;
                    SetPenStyle2d(data->hsp,pen,PS_DOT);
                    SetPenWidth2d(data->hsp,pen,0);
                }else
                {
                    SetPenStyle2d(data->hsp,pen,PS_SOLID);
                    SetPenWidth2d(data->hsp,pen,w);
                    data->flags&=~LF_DISABLED;
                }
                //		 }
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                //	  TLinkData *data=(TLinkData *)b->user;
                BOOL good;
                int w=GetDlgItemInt(hwnd,IDC_LINKWIDTH,&good,0);
                if (!good || w<0 || w>10){
                    MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG3),_MSG_ERROR,MB_OK|MB_ICONSTOP);
                    b->Invalid();
                }else {b->Valid();}
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL LinksDialog(HWND hwnd,PClass _class,HSP2D hsp,HOBJ2D id){
    TPropertyButton*b=new TPropertyButton(IDS_str5,HResource,"Link_Main",Link_options___Main);
    b->Procedure=LinkMainProc;
    TPropertyButton*b1=new TPropertyButton(IDS_str14,HResource,"Link_Add",Link_options___Additional);
    b1->Procedure=LinkMiskProc;
    b->Insert(b1);
    b1=b;
    TLinkData data(_class,hsp,id);
    while (b1){
        b1->user=(&data);
        b1->user1=_class;
        b1=b1->next;
    };
    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str15),b);
    if (rez){
        TLinkInfo info;
        data.CreateInfo(&info);
        ModifyLink(_class,id,&info);
        info.Done();
    }
    delete b;
    return 1;
};


//***********************************************************************//
//                                                                       //
//                Далее для Syntax Editor                                //
//                                                                       //
//***********************************************************************//

BOOL _EXPORT GetColorProc(DWORD _data, char* str, int size, SYNTCOLOR* sc)
{

    char s[256];
    lstrcpyn(s, str, min(sizeof(s),size+1));

    if(!syntaxcolor)
        syntaxcolor = new TSColor();

    sc->scStyle = 0;

    if(_data)
    {
        PClass _class = (PClass) _data;
        for(INT16 i = 0; i < _class->varcount; i++)
        {
            if(!lstrcmpi(_class->vars[i].name, s))
            {
                sc->scTEXT = syntaxcolor->fg[12];
                sc->scBK   = syntaxcolor->bk[12];
                sc->scStyle = syntaxcolor->style[12];
                return TRUE;
            }
        }
    }
    if(compiler)
    {
        int i = compiler->GetColor(s,size);
        if(i > 0)
        {
            i--;
            sc->scTEXT = syntaxcolor->_fg[i];
            sc->scBK   = syntaxcolor->_bk[i];
            sc->scStyle = syntaxcolor->_style[i];
            return TRUE;
        }
        else
        {
            if(i<0)
            {
                switch(i)
                {
                    case -1:
                    {
                        sc->scTEXT = syntaxcolor->fg[8];
                        sc->scBK   = syntaxcolor->bk[8];
                        sc->scStyle = syntaxcolor->style[8];
                        return TRUE;
                    }
                    case -2:
                    {
                        sc->scTEXT = syntaxcolor->fg[9];
                        sc->scBK   = syntaxcolor->bk[9];
                        sc->scStyle = syntaxcolor->style[9];
                        return TRUE;
                    }
                    case -3:
                    {
                        sc->scTEXT = syntaxcolor->fg[10];
                        sc->scBK   = syntaxcolor->bk[10];
                        sc->scStyle = syntaxcolor->style[10];
                        return TRUE;
                    }

                }
            }
            else
            {
                PClassListInfo _clInfo=GetClassListInfo(s);
                if(_clInfo){
                    PClass _class=_clInfo->GetClass();
                    if(_class && _class->flags&CF_PROCEDURE){
                        sc->scTEXT = syntaxcolor->fg[11];
                        sc->scBK   = syntaxcolor->bk[11];
                        sc->scStyle = syntaxcolor->style[11];
                        return TRUE;
                    }
                    goto m1;
                }
            }
        }
m1:
        sc->scTEXT=syntaxcolor->fg[1];;
        sc->scBK=syntaxcolor->bk[1];
        sc->scStyle = syntaxcolor->style[1];
        return TRUE;
    }
    return FALSE;
};


void SetDefaultSyntaxColor(PClass _class,HWND edit){
    SETCOLOR _sc;
    _sc.data=(DWORD)_class;
    _sc.GetColor=GetColorProc;
    SendMessage(edit,ESM_SETCOLORPROC, 0,(LPARAM)&_sc);

    // lstrcpy(s,IDS_str4);
    // LoadStr(IDS_str4,s);

    if(!syntaxcolor)syntaxcolor=new TSColor();

    syntaxcolor->Set(edit);
    UINT32 o=SCGetScVar("syntax_edit_opt",3);
    char s[256];
    SendMessage(edit,ESM_SETCOPTIONS,1,(o&1)!=0);
    SendMessage(edit,ESM_SETCOPTIONS,2,(o&2)!=0);
    SendMessage(edit,ESM_SETCOPTIONS,3,(o&4)!=0);
    SendMessage(edit,ESM_SETCOPTIONS,4,(o&8)!=0);
    SendMessage(edit,ESM_SETCOPTIONS,5,(o&16)!=0);

    LOGFONT lf;
    memset(&lf,0,sizeof(lf));
    INT32 fs=SCGetScVar("syntax_edit_fontsize",10);
    lf.lfHeight=(int)fs;
    if(SCGetScVarS("syntax_edit_font",s)){
        lstrcpy(lf.lfFaceName,s);
    }else{
#ifdef WIN32
        LCID lcid=GetSystemDefaultLCID();
        WORD _id=(lcid)&0xff;
        if(_id==LANG_RUSSIAN){lstrcpy(lf.lfFaceName,"Courier New Cyr");
            //   MessageBox(mainwindow->HWindow,"Russian","Russian",MB_OK);
        }else
#endif
            lstrcpy(lf.lfFaceName,"Courier");

    }
    //  MessageBox(mainwindow->HWindow,lf.lfFaceName,lf.lfFaceName,MB_OK);
    SendMessage(edit,ESM_SETCOPTIONS,256,(LPARAM)&lf);
};

// class info dialog
BOOL scCALLBACK ClassMiskProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_CLOSE:{
            TPropertyButton*b=(TPropertyButton*)lParam;

        }return TRUE;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;

            TMemoryStream ms(TRUE);
            PClass _class=(PClass)b->user;
            char s[256];
            sprintf(s,"Image name:%s",_class->name);
            ms.WriteDOSSTR(s);
            lstrcpy(s,"File name :");
            _class->GetClassFile(s+lstrlen(s),FALSE);
            ms.WriteDOSSTR(s);
            lstrcpy(s,"Created: ");
            _class->GetCreateTime(s+lstrlen(s),80);
            ms.WriteDOSSTR(s);

            if(_class->varcount){
                int maxname=4;
                int maxtype=4;
                int maxdef=7;
                for(int i=0;i<_class->varcount;i++){
                    PVarInfo pv= _class->vars+i;
                    int j=lstrlen(pv->name);
                    if(j>maxname)maxname=j;
                    j=lstrlen(pv->type->GetClassName());
                    if(j>maxtype)maxtype=j;
                    j=lstrlen(pv->def?pv->def:"no default");
                    if(j>maxdef)maxdef=j;
                };
                sprintf(s,"Variables :%d",_class->varcount);
                ms.WriteDOSSTR(s);
                maxname++;maxtype++;maxdef++;
                char cmd[128];
                sprintf(cmd,"%%%ds|%%%ds|%%%ds|%%s",maxname,maxtype,maxdef);
                sprintf(s,cmd,"Name","Type","Default","Information");
                ms.WriteDOSSTR(s);
                for(i=0;i<_class->varcount;i++){
                    PVarInfo pv= _class->vars+i;
                    sprintf(s,cmd,pv->name,pv->type->GetClassName(),
                            pv->def?pv->def:"no default",pv->info?pv->info:"No information");
                    ms.WriteDOSSTR(s);
                }
            }else{ms.WriteDOSSTR("No Variables");}
            ms.WriteDOSSTR("");
            sprintf(s,"Sub objects :%d",_class->childcount);
            ms.WriteDOSSTR(s);
            for(int i=0;i<_class->childcount;i++){
                PChildInfo ci= _class->child+i;
                sprintf(s,"HANDLE :%d  %s",ci->handle,ci->type->GetClassName());

                for(int j=0;j<_class->childcount;j++)if(j!=i){
                    if(ci->handle==_class->child[j].handle){lstrcat(s," * ERROR *");break;}
                }
                ms.WriteDOSSTR(s);
            }
            if(_class->linkcount){
                sprintf(s,"Links %d",_class->linkcount);
                ms.WriteDOSSTR(s);
                for(int i=0;i<_class->linkcount;i++){
                    PLinkInfo pl=_class->link+i;
                    sprintf(s,"Object %d to Object %d Link line handle(%d) (linked vars :%d)",pl->obj1Id,pl->obj2Id,pl->handle,pl->count);
                    ms.WriteDOSSTR(s);
                    for(int j=0;j<pl->count;j++){
                        TLinkEntryInfo * _pl=pl->items+j;
                        sprintf(s,"\t%s - %s",_pl->name1,_pl->name2);
                        ms.WriteDOSSTR(s);
                    }
                }
            }
            ms.WriteWord(0);
            HWND edit=GetDlgItem(hwnd,IDC_TREEINFO);
            HFONT HFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
            SendMessage(edit,WM_SETFONT,(WPARAM)HFont,0);
            SetWindowText(edit,(char*)ms.base);

        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL ClassInfoDialog(HWND hwnd,PClass _class){
    TPropertyButton*b=new TPropertyButton("Image information",HResource,"System_Edit",0);//"Информация"
    b->user=_class;
    b->Procedure=ClassMiskProc;
    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str37),b,0); //"Информация"
    delete b;
    return rez;
};


#endif
