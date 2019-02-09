#ifdef  WIN32
#undef _WINREG_
#endif

#include <stdlib.h>
#include "stream.h"
#include "winman.h"
#include <mem.h>
#include <shellapi.h>

#include "class.h"
#include "messages.h"

#include "hyper.h"
#include "browser.h"
#include "state.h"
#ifdef DBE
#include "dbengine.h"
#endif
#include "dialogs.h"
#include "sc3.rh"
#include "grtools.h"                                
#include "equ.h"
#include "project.h"
#include <stratum\stratum.h>
#include "systab.h"
#include "strings.rh"
#include "meta.h"
#include "meta_c.h"
#include "math.h"
#include "menu.h"
#include "msgh.h"
#include "video\scvideo.h"
#include "dynodata.h"
#include "multitouch.h"

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define long_to_point *(POINT*)((void*)(&lParam))

#define mWS_EXIT                   1ul // Выходит из конструктора

#define mWS_AUTOSCROLL         0x002ul
#define mWS_MDI                0x100ul
#define mWS_POPUP              0x200ul
#define mWS_DIALOG             0x400ul
#define mWS_CHILD              0x800ul
#define mWS_MODAL             0x1000ul
#define mWS_TOOL              0x2000ul
#define mWS_NOCAPTION         0x4000ul
#define mWS_MAXIMIZED         0x8000ul
#define mWS_MINIMIZED        0x10000ul
//
#define mWS_MAXIMIZEBOX      0x20000ul
#define mWS_MINIMIZEBOX      0x40000ul
#define mWS_HSCROLL          0x80000ul
#define mWS_VSCROLL         0x100000ul
#define mWS_BORDER          0x200000ul

#define mWS_NOSYSMENU       0x800000ul
//
#define mWS_BYSPACE        0x4000000ul
#define mWS_AUTOORG        0x8000000ul
#define mWS_SPACESIZE     0x10000000ul
#define mWS_NORESIZE      0x20000000ul
#define mWS_CORRECTPOS    0x40000000ul // Чтобы не вышло за пределы экрана
#define mWS_SHADOW        0x80000000ul //показываем тень для всплывающих окон
#define mWS_NOSHADOW     				 0x4ul //убираем тень для всплывающих окон

BOOL scCALLBACK DefPropertyItemProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
TPropertyButton*GetDlgPropertyButton(HWND hwnd);

int strpos(char*str,char*fragment);
extern HINSTANCE hInstance;

static int win_x,win_y,win_cx,win_cy;
static PObject obj_to_activate=NULL;
static char * __target=NULL;

typedef struct tagOPENMSG
{
    INT32 hSpace;
    char name[256];
    char target[256];
} OPENMSG;

struct mousemsg{ double x,y,keys; };
struct keymsg{double wVkey,Repeat,ScanCode;};
struct commandmsg{double idItem,wNotifyCode;};
struct sizemsg{double fwSizeType,nWidth,nHeight;};
struct minmaxmsg{double MaxSizeX,MaxPositionX,MinTrackSizeX,MaxTrackSizeX,MaxSizeY,MaxPositionY,MinTrackSizeY,MaxTrackSizeY;};
struct controlmsg{long idItem;double wNotifyCode;};

char *__effect_=NULL;

char *GetAbsolutePath(char *filename,char *tmp,TProject *prj=0)
{
    if(!prj)
        prj = (TProject*)GetCurrentProject();

    if(prj)
    {
        prj->GetFullFileName(filename,tmp);
        return tmp;
    }
    return filename;
};

TMetaFile * meta;
INT16 CALLBACK processmetaproc(PStream stream, INT16 code, UINT32 data)
{
    switch(code)
    {
        case ME_LOADPROJECT:
        {
            char s[256];
            stream->ReadStr(s,sizeof(s));
            LoadProject(s, project ? project->unloadondeactivate : FALSE);
        }break;

        case ME_CLICK:
        {
            char s[256];
            stream->ReadStr(s,sizeof(s));
            HOBJ2D obj = stream->ReadWord();
            HSP2D hsp = winmanager->GetHANDLE(s);
            POINT2D p;
            GetObjectOrgSize2d(hsp,obj,&p,0);
            winmanager->StdHyperJump(hsp, p, obj, HJ_NORECORD);
        }break;

        case ME_UNLOADPROJECT:
        {
            char s[256];
            stream->ReadStr(s,sizeof(s));
            TProject*p=GetProject(s);
            if(p){
                /////////////
                //    TProject*_prj=GetProject(p->projecttoactivate);
                //    p->nohandleunload=1;

                CloseProject(p);
                /*
    if(_prj){
     SetActiveProject(_prj);
     EXEC_FLAGS = _prj->_exec_flags;
      if(EXEC_FLAGS&EF_RUNNING)Run();
    }
*/
                /////////////
            }
        }break;

        case ME_CLOSEWINDOW:
        {
            char s[256];
            stream->ReadStr(s,sizeof(s));
            winmanager->CloseWindow(s);
        }break;

        case ME_REDOHYPER:
        {
            char s[256];
            stream->ReadStr(s,sizeof(s));

            HSP2D hsp = winmanager->GetHANDLE(s);
            POINT2D p = {0,0};
            TWinItem* w = winmanager->GetWindow(s);

            if(w)
            {
                RECT r;
                GetClientRect(w->HWindow,&r);

                p.x = r.right/2;
                p.y = r.bottom/2;
                DPtoLP2d(hsp, &p, 1);
                winmanager->StdHyperJump(hsp, p, 0, HJ_NORECORD | HJ_LOADHYPER);
            }

        }break;
            /*
  case ME_CREATEWINDOW:{
   THyperKey key(0,0);
   key.winname=stream->ReadStr();
   key.target=stream->ReadStr();
   HSP2D hsp=winmanager->GetHANDLE(key.winname);
   POINT2D p;
   winmanager->StdHyperJump(hsp,p,0,HJ_NORECORD|HJ_LOADHYPER);
  }break;*/
    }
    return 0;
}
/**************************************************************************

                    W I N D O W S   M A N A G E R

***************************************************************************/

void TMSGWindow::UpdateAScrollers()
{
    //if(item->autoscroll){
    AutoScroll(TRUE);
    //  }else{ UpdateScrollers(); }
};

void TMSGWindow::PostScrollTo()
{
    if(item->autoscroll)
    {
        AutoScroll(TRUE);
    }
};

void TMSGWindow::AutoScroll(BOOL all)
{
    RECT2D lastrect;
    RECT2D r;
    //memset(r,0,sizeof(r));
    BOOL sv=0;
    BOOL sh=0;
    RECT wr;
    if(GetSpaceExtent2d(hsp,r))
    {
        if(memcmp(&lastrect,&r,sizeof(r)) || all )
        {
            GetWindowRect(HWindow,&wr);
            POINT2D org,mul,div;
            GetOrgSpace2d(hsp,&org);
            GetScaleSpace2d(hsp,&mul,&div);
            RECT _r;
            _r.left  = r.left*mul.x/div.x;
            _r.right = r.right*mul.x/div.x;
            _r.top    = r.top  * mul.y/div.y;
            _r.bottom = r.bottom*mul.y/div.y;
            TWindow::SetScrollRange(_r);
        }
    }
    else
    {
        //if( all){
        if(hscroll)hscroll->Show(FALSE);
        if(vscroll)vscroll->Show(FALSE);
        //}
    }
};

void TMSGWindow::EvRButtonDown(WORD ,int xPos,int yPos)
{
    //POINT2D _point;_point.x=xPos;_point.y=yPos;
    switch(state)
    {
        case sfzoom:
            Zoom(xPos,yPos,FALSE);
            if(item->autoscroll)AutoScroll();
            break;
    }
};

BOOL TWindowManager::CanRedo()
{
    return (meta && meta->CanRedo());
};

BOOL TWindowManager::CanUndo()
{
    return (meta && meta->CanUndo());
};

void TWindowManager::DoRedo()
{
    meta->DoRedo();
};

void TWindowManager::DoUndo()
{
    meta->DoUndo(0);
};

void TMSGWindow::StdHyperJump(POINT2D& point2d, HOBJ2D _h_obj, UINT16 fwKeys)
{
    HOBJ2D _obj2d;
    HSP2D __hsp_ = 0;
    obj_to_activate = 0;

    PObject root;
    TProject* project = (TProject *)item->project;

    if(!project)
        return;

    if(project != ::project)
        return; // Only active project may work correctly

    root = project->object;
    BOOL protocol = FALSE;
    TMetaWriter writer(NULL);

    if (_h_obj)
        _obj2d = _h_obj;

    else if(!(fwKeys & HJ_LOADHYPER))
        _obj2d = GetObjectFromPoint2d(hsp, &point2d);

    if (_obj2d || fwKeys & HJ_LOADHYPER)
    {
        if(!(fwKeys & HJ_NORECORD))
        {
            int _size = GetObjectData2d(hsp, _obj2d, UD_HYPERKEY, NULL, 0,0);
            if(_size)
            {
                if(!meta)
                {
                    meta = new TMetaFile(processmetaproc, (UINT32)this);
                    meta->project = project;
                }
                writer.meta = meta;
                protocol = TRUE;
            }
        }

        THyperKey key(hsp, _obj2d, fwKeys & HJ_LOADHYPER ? meta->meta : 0);
        if(key.openmode==4)//выполняем системную команду
        {
            long l=atol(key.target);
            if(l>0)
            {
                _SendCommand(mainwindow->HWindow,l);return;
            }
            if(!lstrcmpi(key.target,"CM_PREVPAGE"))	{_SendCommand(mainwindow->HWindow,CM_PREVPAGE);return;}
            if(!lstrcmpi(key.target,"CM_NEXTPAGE"))	{_SendCommand(mainwindow->HWindow,CM_NEXTPAGE);return;}
            if(!lstrcmpi(key.target,"CM_STOP"))			{_SendCommand(mainwindow->HWindow,CM_PAUSE);	 return;}
            if(!lstrcmpi(key.target,"CM_CLEARALL"))	{_SendCommand(mainwindow->HWindow,CM_CLEARALL);return;}
        }
        if(key.openmode!=0 && protocol)
        {
            meta->NewMetaRecord(ME_CLICK);
            meta->meta->WriteStr(item->name);
            meta->meta->WriteWord(_obj2d);
        }
        switch(key.openmode)
        {
            case 4:
            {}return;
            case 3://ничего не делаем
            {}return;
            case 1://запускаем приложение
            {
                //if(key.target)WinExec(key.target,1);
                if(key.target)
                {
                    TProject *_prj=::project;
                    char buf[MAX_PATH];
                    char *_ps=key.target;
                    char *__ps=_ps;
                    BOOL rel=FALSE;
                    while(*__ps)
                    {
                        if(*__ps=='\\'){rel=TRUE;break;}
                        __ps++;
                    }
                    if(rel || key.target[0]=='.' || IsAbsolutePath(key.target)==1)
                    {
                        _prj->GetFullFileName(key.target,buf,1);
                    }
                    else
                        lstrcpy(buf,key.target);
                    char *_param="";
                    _ps=buf;
                    BOOL _k=TRUE;
                    while(*_ps)
                    {
                        if(*_ps=='"')_k=!_k;
                        if(*_ps==' ' && _k){_param=_ps;*_ps=0;break;}
                        _ps++;
                    }
                    _ps=buf;
                    //MessageBox(0,_ps,"awef",0);
                    ShellExecute(mainwindow->HWindow,
                                 NULL,	// address of string for operation to perform
                                 _ps,	// address of string for filename
                                 _param,	// address of string for executable-file parameters
                                 NULL,	// address of string for default directory
                                 SW_NORMAL 	// whether file is shown when opened
                                 );
                }
            };
                goto m1;
            case 2://загружаем проект
                if(key.target)
                {
                    TProject *_prj=::project;
                    //DWORD _e=EXEC_FLAGS;
                    char dst[270];
                    char name[270];
                    BOOL unloadondeactivate=_prj->unloadondeactivate;
                    if(unloadondeactivate)
                    {
                        if(_prj->realname)lstrcpy(name,_prj->realname);
                        else              lstrcpy(name,_prj->file);
                        meta->project=0;
                    }
                    _prj->GetFullFileName(key.target, dst, 1);
                    //_Message("StdHyperJump");
                    //_Message(key.target);
                    //MessageBox(0,dst,"awef",0);
                    if(LoadProject(dst, unloadondeactivate))
                    {
                        //meta->project=project; // Убрал, а то не работает
                        //_prj->_exec_flags=_e;
                        if(!unloadondeactivate)
                            ::project->SetProjectProp("activateproject",_prj->file);
                        if(protocol)
                        {
                            if(unloadondeactivate)
                            {
                                meta->NewUndoRecord(ME_LOADPROJECT);
                                meta->meta->WriteStr(name);
                            }else{
                                meta->NewUndoRecord(ME_UNLOADPROJECT);
                                meta->meta->WriteStr(::project->file);
                            }
                        }
                        if(key.add)
                        {
                            if(::project->hyperadd)
                                delete (::project->hyperadd);
                            ::project->hyperadd=new THyperAdd(key.add);
                            ::project->readonly|=::project->hyperadd->readonly;
                        }
                        ::project->OnLoad();
                    }else{
                        //if(meta && project)meta->project=project;
                        return;
                    }
                }goto m1;

        }//switch end

        if (key.target)
        {
            __target=key.target;
            char *winname=key.winname?key.winname:item->name;
            __effect_=key.effect;
            char s[128];
            char * _s;
            char * _obj=key.target;
            if (_obj && *_obj=='#')
            {
                obj_to_activate=root->GetReceiver(_obj+1);
                if (obj_to_activate)
                    _obj = obj_to_activate->_class->name;
            }
            if (item->mode==TWinItem::WIN_POPUP)
            {
                if (item->Parent)_s=item->Parent->name;
                else
                {
                    _s="Main Window";
                    if(item->popupparent)
                        _s=item->popupparent;
                }
                if(item->name==winname) winname=_s;
            }
            else _s=winname;
            // было : _s=item->name; //winname
            lstrcpy(s,_s);
            PClass _class=NULL;

            //Проверить на открытие в других окнах !!
            HSP2D _hsp=winmanager->Loaded(s,_obj,&_class);
            if (_hsp)
            {
                {
                    TWinItem *_i= winmanager->GetWindow(_hsp);
                    if (_i)_i->Activate();
                }//Активировать
                goto m1;
            }
            int orgx=100,orgy=100;
            BOOL _popup=FALSE;
            {
                TWinItem win(winname,0,_class,_class?NULL:_obj,0,0,0,0,mWS_EXIT,NULL);
                if (win.hsp)
                {
                    _hsp=win.hsp;
                    TSpaceData data(_hsp);
                    if (data.IsDefined())
                    {
                        _popup=(data.style&SWF_POPUP);
                        if (_popup && (key.winname==NULL))
                        {
                            winname="@PopupWindow";
                            if (item->mode==TWinItem::WIN_POPUP)
                            {
                                RECT r;
                                GetWindowRect(HWindow,&r);
                                orgx=r.left;
                                orgy=r.top;
                            }else{
                                POINT2D _p=point2d;
                                LPtoDP2d(hsp,&_p,1);
                                POINT _pp;PntToPnt(_pp,_p);
                                ClientToScreen(HWindow,&_pp);
                                orgx=_pp.x;
                                orgy=_pp.y;
                            }
                        }
                        if ((data.style&SWF_DIALOG) && (key.winname==NULL))
                            winname="@DialogWindow";
                    }
                }else return ;
                win.hsp=0;
            }
            if(protocol)
            {
                if(_popup)protocol=FALSE;
                else
                {
                    if(popup)
                    {
                        meta->NewMetaRecord(ME_REDOHYPER);
                        meta->meta->WriteStr(item->popupparent);
                        key.Store(meta->meta);
                    }else{
                        meta->NewMetaRecord(ME_CLICK);
                        meta->meta->WriteStr(item->name);
                        meta->meta->WriteWord(_obj2d);
                    }
                }
            }
            {
                if(protocol)
                {
                    if(winmanager->IsWindowExist(winname))
                    {
                        TWinItem *_item=winmanager->GetWindow(winname);
                        if(_item)
                        {
                            meta->NewUndoRecord(ME_REDOHYPER);
                            meta->meta->WriteStr(winname);
                            TWinItem *__item=_item;
                            if(popup)__item=winmanager->GetWindow(item->popupparent);
                            if(!__item)__item=_item;
                            if(__item->window->hyperstream)
                            {
                                __item->window->hyperstream->CopyTo(meta->meta,0,__item->window->hyperstream->GetSize());
                            }else{
                                THyperKey _key(0,0);
                                _key.winname=NewStr(__item->name);
                                _key.target=NewStr(__item->_classname?__item->_classname:__item->_filename);
                                _key.Store(meta->meta);
                            }
                        }
                    }else{
                        meta->NewUndoRecord(ME_CLOSEWINDOW);
                        meta->meta->WriteStr(winname);
                    }
                }
                HSP2D _h_hsp=winmanager->CreateWindowEx(winname,item->Parent->name,
                                                        obj_to_activate?obj_to_activate->_class->name:key.target,_hsp,orgx,orgy,
                                                        CW_USEDEFAULT,CW_USEDEFAULT,
                                                        "WS_BYSPACE|WS_CORRECTPOS");
                if(_h_hsp!=_hsp) DeleteSpace2d(_hsp);
                //*-------------------------------------*//
                if (_h_hsp)
                {
                    TWinItem *_i= winmanager->GetWindow(_h_hsp);
                    if (_i)
                    {
                        if(_popup)_i->popupparent=NewStr(item->name);
                        _i->Activate();
                        __hsp_=_i->hsp;

                        //if(protocol){
                        if(!_i->window->hyperstream)
                            _i->window->hyperstream=new TDOSStream(NULL,TDOSStream::stCreate);
                        _i->window->hyperstream->Seek(0);
                        key.Store(_i->window->hyperstream);
                        //hyperstream->Truncate();}
                    }
                }
                //*-------------------------------------*//
            }
        }
        __target=NULL;
m1:
        // посылается сообщение
        if (key.object)
        {
            obj_to_activate=root->GetReceiver(key.object);
            if(obj_to_activate)
            {
                TClassMSG *msg=CreateMSG(obj_to_activate,0,(INT16)WM_HYPERJUMP,(INT16)0,"_hSpace:HANDLE,_Target:STRING,_Window:STRING");
                msg->Update(0);

                struct
                {
                    UINT32 hsp;
                    char target[256];
                    char window[256];
                } targetmsg;
                targetmsg.hsp=(UINT32)__hsp_;
                lstrcpy(targetmsg.target,key.target);
                lstrcpy(targetmsg.window,key.winname);
                msg->Send(WM_HYPERJUMP,&targetmsg,FALSE);
                obj_to_activate=0;

                delete msg;
                //return;
            }
        }
        // посылается сообщение
    }
    //   if(protocol){meta->CloseMetaRecord();}
}

void TMSGWindow::EvLButtonDown(WORD modKeys,int xPos,int yPos)
{
    POINT2D point2d;point2d.x=xPos;point2d.y=yPos;
    DPtoLP2d(hsp,&point2d,1);
    switch(state)
    {
        case sfbase:
            if (!winmanager->captured)
            {
                StdHyperJump(point2d,0,modKeys);
            }break;
        case sfzoom:
            Zoom(xPos,yPos,TRUE);
            if(item->autoscroll)
                AutoScroll();
            break;
    }
}

void TMSGWindow::EvLButtonUp(WORD /*modKeys*/,int ,int){};

void TMSGWindow::EvMouseMove(WORD /*modKeys*/,int xPos,int  yPos)
{
    POINT2D point2d;point2d.x=xPos;point2d.y=yPos;
    DPtoLP2d(hsp,&point2d,1);
    switch(state)
    {
        case 0:
        {
            HOBJ2D _obj2d=GetObjectFromPoint2d (hsp,&point2d);
            if (_obj2d)
            {
                int _size=GetObjectData2d(hsp,_obj2d,UD_HYPERKEY,NULL,0,0);
                if (_size)
                    SetCursor(HResource,item->project==::project?IDC_PERST:IDC_PERSTno);
                else
                    SetDefCursor();
                //SetCursor(0,IDC_ARROW);
            }
            else
                SetDefCursor();
            //SetCursor(0,IDC_ARROW);
        };break;
    }
    SetCoordinates(point2d);
};

void TMSGWindow::SetState(int _state)
{
    if (_state==sfbase)
    {
        SetDefCursor();
        //SetCursor(0,IDC_ARROW);
    }
    state=_state;
};

LRESULT TMSGWindow::WndProc(UINT message, WPARAM  wParam,LPARAM lParam)
{
    switch (message){
        case WM_GETMINMAXINFO:if(item && item->nochangesize){
                if(!(EXEC_FLAGS&EF_STEPACTIVE)){
                    MINMAXINFO FAR* lpmmi = (MINMAXINFO FAR*) lParam;
                    if(item->size.x && item->size.y){
                        lpmmi->ptMaxSize.x=item->size.x;
                        lpmmi->ptMaxSize.y=item->size.y;
                        lpmmi->ptMinTrackSize=lpmmi->ptMaxTrackSize=lpmmi->ptMaxSize;
                    }}
            }break;
        case WM_SIZE:if(item){
                PostScrollTo();
            }break;
        case WM_CLOSE:if((item->mode==TWinItem::WIN_DIALOG) && (messages))
            {
                BOOL process;
                if(messages->WndProc(WM_CANCLOSE,0,0,process)==256)return 0;
            }break;
        case TW_SETFOCUS:if(wParam!=1)return 0;
        case WM_SETFOCUS:
        {//UpdateScaleBox();
            char s[256],cmd[32];
            LoadString(HResource,IDS_str119,cmd,sizeof(cmd));
            wsprintf(s,cmd   ,item->name);
            SCSetStatusText(1,s);
            UpdateScaleBox();
            SetFocus(HWindow);
            return 5;
        };
        case WM_SYSCOMMAND:{
            if (item && item->nosize){
                if(
                   (wParam==SC_SIZE)||
                   (wParam==SC_MAXIMIZE)||
                   (wParam==SC_MINIMIZE)){
                    return 0;
                }
            }
        }break;
        case WM_NCHITTEST:
            if (item && item->nosize){
                int lRetVal = (int)DefWindowProc(message, wParam, lParam);
                switch (lRetVal){
                    case HTTOP        :
                    case HTBOTTOM     :
                    case HTLEFT       :
                    case HTRIGHT      :
                    case HTTOPLEFT    :
                    case HTTOPRIGHT   :
                    case HTBOTTOMLEFT :
                    case HTBOTTOMRIGHT:lRetVal=HTBORDER ;
                }
                return  lRetVal;
            }
            break;
        case WM_NCPAINT:{
            if(shadow){
                RECT rr,rect;
                GetClientRect(HWindow,&rr);
                GetWindowRect(HWindow,&rect);

                rect.right  -= rect.left;
                rect.bottom -= rect.top;


                //InflateRect(&rr,1,1);
                rr.right+=1;
                rr.bottom+=1;

                HDC hdc=GetWindowDC(HWindow);
                HGDIOBJ pen=CreatePen(PS_SOLID,0,0);
                HGDIOBJ old=SelectObject(hdc,pen);

                POINT p[5]={{0,0},{rr.right,0},{rr.right,rr.bottom},{0,rr.bottom},{0,0}};
                Polyline(hdc,p,5);
                SelectObject(hdc,old);
                DeleteObject(pen);
                old=SelectObject(hdc,BGrayed);
                SetROP2(hdc,R2_MASKPEN);

                p[0].x=rect.right-8;p[0].y=8;
                p[1].x=rect.right;p[1].y=8;
                p[2].x=rect.right;p[2].y=rect.bottom;
                p[3].x=rect.right-8;p[3].y=rect.bottom;

                SelectObject(hdc,GetStockObject(NULL_PEN));
                Polygon(hdc,p,4);

                p[0].x=8;           p[0].y=rect.bottom-8;
                p[1].x=8;           p[1].y=rect.bottom;
                p[2].x=rect.right-8;p[2].y=rect.bottom;
                p[3].x=rect.right-8;p[3].y=rect.bottom-8;

                Polygon(hdc,p,4);
                SelectObject(hdc,old);


                ReleaseDC(HWindow,hdc);

            } break;
        }
        case WM_NCCALCSIZE:if(shadow){
                BOOL fCalcValidRects = (BOOL) wParam;            // valid-area flag
                NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS FAR*) lParam;  // address of data
                if(fCalcValidRects){
                    LRESULT lr=TSpaceWindow::WndProc(message,wParam,lParam);
                    /*
       lpncsp->rgrc[0]=lpncsp->rgrc[1];*/
                    lpncsp->rgrc[0].left++;
                    lpncsp->rgrc[0].top++;
                    lpncsp->rgrc[0].right-=9;
                    lpncsp->rgrc[0].bottom-=9;

                    //	   lpncsp->rgrc[0].right=;

                    if(vscroll && vscroll->show){
                        int w=GetSystemMetrics (SM_CXVSCROLL );
                        lpncsp->rgrc[2].right-=w;
                    }
                    if(hscroll && hscroll->show){
                        int h=GetSystemMetrics (SM_CYHSCROLL);
                        lpncsp->rgrc[2].bottom-=h;
                    }
                    return lr;
                }

            }break;
        case WM_KEYUP:
        case WM_KEYDOWN:{
            if (wParam==VK_ESCAPE && (item->mode == TWinItem::WIN_POPUP)){
                DestroyWindow(HWindow);return 0;
            }else return KeyDown(wParam,lParam);
        };
        case WM_KILLFOCUS:{
            if (closeOnLostFocus){
                DestroyWindow(HWindow);return 0;
            }
        }break;
    }
    if(inpsp_page){
        if(!DefPropertyItemProc(HWindow,message,wParam,lParam))
            return TSpaceWindow::WndProc(message,wParam,lParam);
    }else{
        return TSpaceWindow::WndProc(message,wParam,lParam);
    }
};

void TMSGWindow::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode)
{
    if(messages && messages->count)
    {
        commandmsg k;
        k.idItem=command;k.wNotifyCode=wNotifyCode;
        for(C_TYPE i=0;i<messages->count;i++)
        {
            TClassMSG* msg=messages->msg(i);
            if ( msg->code==WM_COMMAND )
            {
                msg->Send((INT16)WM_COMMAND,&k,0);
            }
        }
    }
    switch(command)
    {
        case CM_SPACEPROP:
            SpaceDialog(HWindow,hsp,0,0,0,0);break;
        case CM_ZOOM:
        {
            if (state==sfzoom)
            {
                SetState(sfbase);
                PressControlButton(CM_STATEbase);
            }
            else
            {
                SetState(sfzoom);
                SetCursor(HResource,IDC_ZOOMIN);
            }
        };break;
        default:
            TSpaceWindow::EvCommand(command,hwndCtl,wNotifyCode);
    }
    switch(command)
    {
        case CM_SCALEBOX:if(item->autoscroll) AutoScroll();break;
    }
};

LRESULT TMSGWindow::KeyDown(WPARAM wVkey,LPARAM ){
    switch(state){
        case sfzoom:{
            if (wVkey==VK_ESCAPE)SetState(sfbase);return 0;
        };
    }
    return 0;
};
int TMSGWindow::IsCommandDisable(int command){
    switch(command){
        case CM_SPACEPROP:return 0;
        case CM_ZOOM:if (scaling)return 0;return 200;
    }
    return TSpaceWindow::IsCommandDisable(command);
};

TMSGWindow::TMSGWindow(LPCREATESTRUCT l,HWND h):TSpaceWindow(l,h){
    hyperstream=NULL;
    item=0;
    scaling=TRUE;
    inpsp_page=FALSE;
    shadow=closeOnLostFocus=modal=popup=FALSE;

    //if (l->style&WS_MODAL)modal=1;
    //if (!(l->style&WS_CHILD))popup=2;
    SetState(0);
};
TMSGWindow::~TMSGWindow(){
    if (nonmdi)winmanager->PostDelete(HWindow);
    if(hyperstream)delete hyperstream;
};


TMSGItems::TMSGItems(HWND _HWindow,HSP2D _hsp):TCollection(10,10)
{
    HWindow=_HWindow;
    hsp=_hsp;
    lock=0;
};

TMSGItems::~TMSGItems()
{}

void  TMSGItems::FreeItem(pointer p)
{
    TClassMSG* msg=(TClassMSG*)p;
    if(msg->lflags&CMSG_INPROCESS)
    {
        msg->lflags|=CMSG_MUSTREMOVE;
    }
    else{delete msg;}
};

void TMSGItems::DoneHandler(void*project,UINT _msg){
    for(C_TYPE i=0;i<count;i++){
        TClassMSG* msg_=msg(i);

        if ( ((msg_->code==_msg)||
              ((_msg == WM_DESTROY)&&(msg_->code==WM_SPACEDONE)))

             ){
            msg_->Send(msg_->code,NULL,0);
        } }

    i=0;
    if (_msg==WM_DESTROY){
        // это только для не MSGWindow
        while (i<count){
            TClassMSG* msg_=msg(i);
            if((project==NULL)||(msg_->project==project))AtFree(i);else i++;

        }
    }else {
        while (i<count){
            TClassMSG* msg_=msg(i);
            if (msg_->flags&8)i++;//??? No done
            else AtFree(i);
        }}

};

void  TMSGItems::Update()
{
    C_TYPE i=0;
    while (i < count)
    {
        if (msg(i)->Update(1))
            i++;
        else
            AtFree(i);
    }
};

void  TMSGItems::Register(TClassMSG * msg)
{
    Insert(msg);
}

void  TMSGItems::UnRegister(HOBJ2D obj,UINT16 code)
{
    C_TYPE i=0;
    while(i<count)
    {
        TClassMSG* msg=((TClassMSG*)At(i));
        if (msg->object==obj && msg->code==code)
        {
            AtFree(i);
        }
        else i++;
    }
}

LRESULT TMSGItems::WndProc(UINT message, WPARAM wParam,LPARAM lParam,BOOL &process)
{
    if (count && (((EXEC_FLAGS & EF_STEPACTIVE) == 0) || (EXEC_FLAGS & EF_ONDIALG)) /*&& (state==0)*/)
    {
        process = TRUE;
        switch(message)
        {
            case PSP_VALID:
            {
                for(C_TYPE i=0;i<count;i++)
                {
                    TClassMSG* msg_=msg(i);
                    if ( msg_->code == message )
                    {
                        double r = wParam;
                        msg_->Send((INT16)message,&r,0);
                        TPropertyButton*b=GetDlgPropertyButton(HWindow);
                        if(b)
                        {
                            if(r)
                                b->Valid();
                            else
                                b->Invalid();
                        }
                    }
                }
            }break;

            case PSP_ACTIVATE:
            {
                for(C_TYPE i=0;i<count;i++)
                {
                    TClassMSG* msg_=msg(i);
                    if ( msg_->code==message )
                    {
                        msg_->Send((INT16)message,NULL,0);
                    }
                }
            }break;

            case WM_GETMINMAXINFO:
            {
                MINMAXINFO FAR* lpmmi = (MINMAXINFO FAR*) lParam;
                minmaxmsg k;
                k.MaxSizeX=lpmmi->ptMaxSize.x;
                k.MaxPositionX=lpmmi->ptMaxPosition.x;
                k.MinTrackSizeX=lpmmi->ptMinTrackSize.x;
                k.MaxTrackSizeX=lpmmi->ptMaxTrackSize.x;
                for(C_TYPE i=0;i<count;i++)
                {
                    TClassMSG* msg_=msg(i);
                    if ( msg_->code==message )
                    {
                        msg_->Send((INT16)message,&k,1);
                    }
                }
            }break;

            case WM_SIZE:
            {
                sizemsg k;
                k.fwSizeType = wParam;
                k.nWidth = LOWORD(lParam);
                k.nHeight = HIWORD(lParam);
                for(C_TYPE i = 0; i < count; i++)
                {
                    TClassMSG* msg_ = msg(i);
                    if(msg_->code == message)
                    {
                        msg_->Send((INT16)message, &k, 0);
                    }
                }
            }break;

            case WM_MOVE:
            {
                mousemsg k;
                k.x = (int) LOWORD(lParam);    // horizontal position
                k.y = (int) HIWORD(lParam);    // vertical position

                for(C_TYPE i=0;i<count;i++)
                {
                    TClassMSG* msg_ = msg(i);
                    if (msg_->code == message)
                    {
                        msg_->Send((INT16)message, &k, 0);
                    }
                }
            }break;

            case WM_COMMAND:
            {
#ifdef WIN32
                int  wNotifyCode = HIWORD(wParam); // notification code
                int  command = LOWORD(wParam);         // item, control, or accelerator identifier
                HWND hwndCtl = (HWND) lParam;      // handle of control
#else
                int  command = wParam;        /* control or menu item identifier      */
                HWND hwndCtl = (HWND) LOWORD(lParam); /* handle of control           */
                int wNotifyCode = HIWORD(lParam);    /* notification message        */
#endif
                HSP2D _hsp;
                HOBJ2D id;
                if(!(EXEC_FLAGS&EF_STEPACTIVE))
                {
                    if (hwndCtl && GetControlId2d(hwndCtl, &_hsp, &id))
                    {
                        controlmsg k;
                        k.idItem = (INT32)id;
                        k.wNotifyCode = wNotifyCode;

                        for(C_TYPE i = 0; i < count; i++)
                        {
                            TClassMSG* msg_ = msg(i);
                            if (msg_->code == WM_CONTROLNOTIFY &&
                                (msg_->object == 0 || msg_->object == id))
                            {
                                msg_->Send(WM_CONTROLNOTIFY,&k,0);
                            }
                        }
                    }
                    commandmsg k;
                    k.idItem = command;
                    k.wNotifyCode = wNotifyCode;
                    for(C_TYPE i = 0; i < count; i++)
                    {
                        TClassMSG* msg_=msg(i);
                        if ( msg_->code == message )
                            msg_->Send((INT16)message,&k,0);
                    }
                }
            }break;

            case WM_CANCLOSE:
            {
                double canclose = 1, cc;
                for(C_TYPE i = 0; i < count; i++)
                {
                    TClassMSG* msg_ = msg(i);
                    if (msg_->code==message)
                    {
                        cc = 1;
                        msg_->Send((INT16)message, &cc, 1);
                        if (cc==0)
                            canclose=0;
                        break;
                    }
                }
                if(!canclose)
                    return 256;
            }return 0;

            case WM_CHAR:
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                keymsg k;
                k.wVkey = wParam;
                k.Repeat = LOWORD(lParam);
                k.ScanCode = HIWORD(lParam) & 0xff;
                for(C_TYPE i = 0; i < count; i++)
                {
                    TClassMSG* msg_ = msg(i);
                    if (msg_->code==message || msg_->code==WM_ALLKEYMESSAGE)
                    {
                        msg_->Send((INT16)message, &k, 0);
                    }
                }
            }break;
                /************************************************************
         M O U S E   M O U S E   M O U S E   M O U S E
************************************************************/

            case WM_MOUSEMOVE:
            case WM_MBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONDBLCLK:
            case WM_RBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONDBLCLK:
            case WM_LBUTTONUP:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
                if(hsp)
                {
                    POINT2D p;
                    GetPOINT2D(hsp,&p,lParam);
                    mousemsg m2,m3;
                    HOBJ2D obj,primary;
                    m2.x = p.x, m2.y = p.y, m2.keys = wParam;

                    obj = GetObjectFromPoint2d(hsp, &p, &primary);
                    for(C_TYPE i = 0; i < count; i++)
                    {
                        TClassMSG* msg_ = msg(i);
                        if ((msg_->code == message) || (msg_->code == WM_ALLMOUSEMESSAGE))
                        {
                            if (msg_->flags & 1)
                            {
                                if (winmanager->capobj)
                                {
                                    if (winmanager->capobj==msg_->pobject)
                                    {
                                        if (msg_->flags & 4)
                                        {
                                            m3=m2;
                                            POINT2D pp;
                                            if(GetObjectOrgSize2d(hsp,msg_->object,&pp,NULL))
                                            {
                                                m3.x-=pp.x;m3.y-=pp.y;
                                                msg_->Send((INT16)message, &m3, 0);
                                            }
                                        }
                                        else
                                            msg_->Send((INT16)message,&m2,0);
                                    }
                                }
                                else
                                {
                                    if (obj==msg_->object)
                                    {
                                        if (msg_->flags & 4)
                                        {
                                            m3 = m2;
                                            POINT2D pp;
                                            if(GetObjectOrgSize2d(hsp,msg_->object,&pp,NULL))
                                            {
                                                m3.x-=pp.x;m3.y-=pp.y;
                                                msg_->Send((INT16)message,&m3,0);
                                            }
                                        }
                                        else
                                            msg_->Send((INT16)message,&m2,0);
                                    }
                                }
                            }
                            else
                                msg_->Send((INT16)message,&m2,0);
                        }
                    }
                }break;
            default:
            {}
        }
    }
    process=FALSE;
    return 0;
}

BOOL TMSGItems::RegisterObject(PObject po,HOBJ2D obj,INT16 code,UINT32 flags)
{
    if (lock)
    {
        char s[256]="Unable register object in process another message";
        AddObjectName(s);
        _Message(s);
        return 0;
    };

    //Если такое сообщение уже зарегистрировано, выводим ошибку
    for(C_TYPE i = 0; i < count; i++)
    {
        TClassMSG* _msg = msg(i);
        if(_msg->pobject == po && _msg->code == code &&	_msg->object == obj)
        {
            char s[256]="WARNING  attempt to reregister object ";
            AddObjectName(s);
            _Message(s);
            return FALSE;
        }
    }

    //Генерация имен задействованых переменных
    char* s;
    switch(code)
    {
        case PSP_VALID:				s="ValidType";break;
        case PSP_ACTIVATE:			s="";break;

        case WM_CONTROLNOTIFY:
            s="_hObject:HANDLE,wNotifyCode";
            break;

        case WM_SPACEINIT:
            s = "_hSpace:HANDLE,_WindowName:STRING,_Target:STRING";
            break;

        case WM_SPACEDONE:
        case WM_DESTROY:				s=NULL;break;
        case WM_GETMINMAXINFO:
            s="MaxSizeX,MaxPositionX,MinTrackSizeX,MaxTrackSizeX,MaxSizeY,MaxPositionY,MinTrackSizeY,MaxTrackSizeY";
            break;
        case WM_COMMAND:				s="idItem,wNotifyCode";break;
        case WM_SIZE:   				s="fwSizeType,nWidth,nHeight";break;
        case WM_MOVE:   				s="xPos,yPos";break;

        case WM_CANCLOSE:
            s="CanClose";
            break;

        case WM_ALLKEYMESSAGE:
        case WM_CHAR:
        case WM_KEYUP:
        case WM_KEYDOWN:
            s = "wVkey,Repeat,ScanCode";
            break;

        case WM_ALLMOUSEMESSAGE:
        case WM_MOUSEMOVE:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:	s="xPos,yPos,fwKeys";break;
        default:
            s="wParam,lParam";
    }
    TClassMSG* _msg = CreateMSG(po, obj, code, (INT16)flags, s);
    _msg->project = (TProject*)po->GetProject();
    _msg->Update(0);
    Register(_msg);
    return TRUE;
}

void TWinItem::Send()
{
    if (obj_to_activate)
    {
        TClassMSG* msg = CreateMSG(obj_to_activate,0,(INT16)WM_SPACEINIT,(INT16)flags,"_hSpace:HANDLE,_WindowName:STRING,_Target:STRING");
        msg->Update(0);
        OPENMSG openmsg;
        memset(&openmsg,0,sizeof(openmsg));
        openmsg.hSpace=(int)hsp;
        lstrcpy(openmsg.name, name);

        if(__target)
            lstrcpy(openmsg.target,__target);

        msg->Send(WM_SPACEINIT, &openmsg, FALSE);
        obj_to_activate=0;
        delete msg;
    }
}

void  TWinItem::SetSize()
{
    RECT r;
    GetWindowRect(HWindow,&r);
    size.x=r.right-r.left;
    size.y=r.bottom-r.top;
};

TWinItem::TWinItem(int,char*winname)
{
    memset(this,0,sizeof(TWinItem));
    TransparentLevel=100;
    TransparentColor=0;
    ChangeTransparentColor=false;
    project=GetCurrentProject();
    lstrcpyn(name,winname,MAX_WIN_NAME);
};

TWinItem::TWinItem(char*winname,TMDIChildWindow*m,HSP2D _hsp)
{
    memset(this,0,sizeof(TWinItem));
    TransparentLevel=100;
    TransparentColor=0;
    ChangeTransparentColor=false;
    lstrcpyn(name,winname,MAX_WIN_NAME);
    mode=WIN_MDI;
    hsp=_hsp;
    HWindow=m->HWindow;
    mdichild=m;
    system=1;
    //project=GetCurrentProject();
    if(m->prj)	project=m->prj;
    else 				project=GetCurrentProject();
    SetSize();
};

TWinItem::TWinItem(HWND _hwnd,char*winname,char *_data,DWORD __flags,DWORD win_style,PObject po)
{
    memset(this,0,sizeof(TWinItem));
    TransparentLevel=100;
    TransparentColor=0;
    ChangeTransparentColor=false;
    project=GetCurrentProject();
    lstrcpyn(name,winname,MAX_WIN_NAME);
    mode=WIN_DIALOG;
    PClass _class=NULL;
    char tmp[9999];
    char * _file=0;
    PClassListInfo _clInfo=GetClassListInfo(_data);
    if (_clInfo)
    {
        _class=_clInfo->GetClass();
        hsp=GetClassSpace(_class);
        _classname=NewStr(_data);
    }
    else
    {
        _file=GetAbsolutePath(_data,tmp);
        _filename=NewStr(_file);
        hsp=LdSpace2d(0,_file);
    };
    if(!hsp)return;
    DeleteEditFrame2d(hsp);
    TSpaceData data(hsp);
    DWORD _flags=__flags;
    win_cx=win_cy=0;
    if (data.IsDefined())
    {
        if(data.style&SWF_NORESIZE)
            nochangesize=1;
        if(data.sizex>0 && data.sizey>0)
        {
            win_cx=data.sizex;
            win_cy=data.sizey;
        }
        if(data.style&SWF_SPACESIZE)  _flags|=mWS_SPACESIZE;
        if(data.style&SWF_AUTOORG)    _flags|=mWS_AUTOORG;
        if(data.style&SWF_VSCROLL)		_flags|=mWS_VSCROLL;
        if(data.style&SWF_HSCROLL)  	_flags|=mWS_HSCROLL;
    }
    POINT2D spaceorg;
    spaceorg.x=spaceorg.y=0;
    RECT2D r;
    if(GetSpaceExtent2d(hsp,r)&&(win_cx==0)&&(win_cy==0))
    {
        win_cx=r.right-r.left;
        win_cy=r.bottom-r.top;
    }
    if (_flags&mWS_AUTOORG)
    {
        spaceorg.x=r.left;
        spaceorg.y=r.top;
        SetOrgSpace2d(hsp,&spaceorg);
    }
    _CreateSpaceWindow(0,win_style,_hwnd);
    if (window)
    {
        HWindow=window->HWindow;
        window->nonmdi=TRUE;
        if (_flags&mWS_VSCROLL){ (window->vscroll=new TScroller(window,-1000,1000,1,SB_VERT))->flags|=SF_MANUAL ;}
        if (_flags&mWS_HSCROLL){ (window->hscroll=new TScroller(window,-1000,1000,1,SB_HORZ))->flags|=SF_MANUAL ;}
        if (_flags&mWS_MODAL)window->modal=1;
        if (mode == WIN_POPUP)window->popup=1;
        if (window->hscroll || window->vscroll)
        {
            window->UpdateAScrollers();
            POINT2D p;
            if(GetOrgSpace2d(hsp,&p))
            {
                if(window->hscroll)window->hscroll->SetPos(p.x,0);
                if(window->vscroll)window->vscroll->SetPos(p.y,0);
            }
        }
        SetClientSize(win_cx,win_cy);
        winmanager->Insert(this);
        obj_to_activate=po;
        __target=NULL;
        Send();
        winmanager->Delete(this);
    }
    SetSize();
    if (window->hscroll || window->vscroll)window->UpdateAScrollers();
#ifdef LOCKEXITBUTTONOFWINDOW
    HMENU hMenuHandle;
    hMenuHandle = GetSystemMenu(HWindow, FALSE);
    if (hMenuHandle)
        DeleteMenu(hMenuHandle, SC_CLOSE, MF_BYCOMMAND);
#endif
};

TWinItem::TWinItem(char*winname,HSP2D _hsp,PClass _class,char*filename,
                   int x,int y,int sizex,int sizey,DWORD __flags,HWND _parent)
{
    memset(this,0,sizeof(TWinItem));

    TransparentLevel=100;
    TransparentColor=0;
    ChangeTransparentColor=false;

    project = GetCurrentProject();

    DWORD _flags=0;
    DWORD win_style=0;
    mode = WIN_MDI;

    if (_class && filename)
        return;

    lstrcpyn(name,winname,MAX_WIN_NAME);

    if (_hsp)
        hsp = _hsp;

    else
    {
        if (_class)
        {
            hsp = GetClassSpace(_class);
        }
        if (filename)
        {
            if (filename && lstrlen(filename))
            {
                hsp = LdSpace2d(0,filename);
                DeleteEditFrame2d(hsp);
            }
        }
        if(!hsp)
            hsp = CreateSpace2d(0, scheme_path);
    }
    if (__flags & mWS_EXIT)
        return;

    win_x = x;
    win_y = y;
    win_cx=sizex;
    win_cy=sizey;

    if (_class)
        _classname = NewStr(_class->name);

    if (filename)
        _filename = NewStr(filename);

    BYTE showtype = 0;

    if (hsp)
    {
        if (__flags & mWS_BYSPACE)
        {
            TSpaceData data(hsp);
            if (data.IsDefined())
            {
                if (data.style & SWF_NORESIZE)
                {
                    _flags |= mWS_NORESIZE;
                }
                if (data.sizex > 0 && data.sizey > 0)
                {
                    win_cx = data.sizex;
                    win_cy = data.sizey;
                }
                if(data.style & SWF_NORESIZE)
                    nochangesize = 1;

                if(data.style & SWF_MINIMIZE)
                    showtype = 2;

                if(data.style & SWF_MAXIMIZE)
                    showtype = 1;

                if(data.style & SWF_SPACESIZE)
                    _flags |= mWS_SPACESIZE;

                if(data.style & SWF_AUTOORG)
                    _flags |= mWS_AUTOORG;

                if(data.style & SWF_DIALOG)
                    mode = WIN_DIALOG;//_flags|=mWS_DIALOG;

                if(data.style & SWF_POPUP)
                {
                    mode=WIN_POPUP;
                    if(!(__flags&mWS_NOSHADOW))
                        _flags|=mWS_SHADOW;
                };
                if (data.style&SWF_MDI)
                    mode=WIN_MDI;   //_flags|=mWS_MDI;

                //if (data.style&SWF_CHILD)    mode=WIN_CHILD; //_flags|=mWS_MDI;
                if (data.style & SWF_MODAL)
                    _flags |= mWS_MODAL;

                if (data.style&SWF_VSCROLL)
                    _flags |= mWS_VSCROLL;

                if (data.style & SWF_HSCROLL)
                    _flags |= mWS_HSCROLL;

                if (data.style & SWF_AUTOSCROLL)
                    _flags |= mWS_AUTOSCROLL;
            }
        }
        if (__flags & mWS_SPACESIZE)
            _flags |= mWS_SPACESIZE;
        if (__flags & mWS_AUTOORG)
            _flags |= mWS_AUTOORG;
        if (__flags & mWS_MODAL)
            _flags |= mWS_MODAL;
        if (__flags & mWS_VSCROLL)
            _flags |= mWS_VSCROLL;
        if (__flags&mWS_HSCROLL)   _flags|=mWS_HSCROLL;
        if (__flags&mWS_NORESIZE)  _flags|=mWS_NORESIZE;
        if (__flags&mWS_AUTOSCROLL)_flags|=mWS_AUTOSCROLL;
        if (__flags&mWS_BORDER)		 _flags|=mWS_BORDER;

        if (__flags&mWS_DIALOG)mode=WIN_DIALOG;
        if (__flags&mWS_POPUP )mode=WIN_POPUP;
        if (__flags&mWS_MDI   )mode=WIN_MDI;
        if (__flags&mWS_CHILD )mode=WIN_CHILD;
        if (__flags&mWS_MINIMIZED)showtype=2;
        if (__flags&mWS_MAXIMIZED)showtype=1;

        //*---------------------------------------------*/
        POINT2D spaceorg;
        spaceorg.x = spaceorg.y = 0;
        if (_flags & mWS_NORESIZE)
            nosize = 1;

        if ((_flags & mWS_SPACESIZE) || (_flags&mWS_AUTOORG))
        {
            RECT2D r;
            if(GetSpaceExtent2d(hsp,r))
            {
                if (_flags&mWS_SPACESIZE)
                {
                    win_cx=r.right-r.left;
                    win_cy=r.bottom-r.top;
                }
                if (_flags&mWS_AUTOORG)
                {
                    spaceorg.x=r.left;spaceorg.y=r.top;
                    SetOrgSpace2d(hsp,&spaceorg);
                }
            }
        }

        /* Это до создания окна */
        DWORD exStyle=0;
        switch(mode)
        {
            case WIN_DIALOG:
            {
                win_style=WS_THICKFRAME | 0x204L | WS_POPUP;
                if(__flags&mWS_TOOL)exStyle|=WS_EX_TOOLWINDOW|WS_EX_TOPMOST;
                if(!(__flags&mWS_NOSYSMENU))win_style|=WS_SYSMENU;
                if(!(__flags&mWS_NOCAPTION))win_style|=WS_CAPTION;
                if(__flags&mWS_MAXIMIZEBOX)win_style|=WS_MAXIMIZEBOX;
                if(__flags&mWS_MINIMIZEBOX)win_style|=WS_MINIMIZEBOX;
                if((_flags&mWS_NORESIZE)==0 && !(__flags&mWS_NOCAPTION))
                {win_style|=WS_SIZEBOX; }
            }break;
            case WIN_POPUP:
            {
                win_style=WS_POPUP;
                if ((_flags&mWS_SHADOW)==0)win_style|=WS_BORDER;
            }break;
            case WIN_MDI:
            {
                win_style=WS_CHILD;
            }break;

            case WIN_CHILD:
            {
                if (!_parent)
                {
                    DeleteSpace2d(hsp);
                    hsp=0;
                    return;
                };
                win_style=WS_CHILD;
            }break;
        };
        // <>если нужно то добавляем бордюр
        if(_flags & mWS_BORDER)
            win_style |= WS_BORDER;
#ifdef WIN32
        if(win_style&WS_BORDER && ((mode!=WIN_DIALOG) || (_flags&mWS_BORDER)))
            exStyle|=WS_EX_CLIENTEDGE;
#endif
        _CreateSpaceWindow(exStyle, win_style, mainwindow->HWindow);

        if (window)
        {
            winmanager->Insert(this);
            Send();
            winmanager->Delete(this);

            window->nonmdi=TRUE;
            autoscroll=0;
            if (_flags & mWS_AUTOSCROLL)
            {
                autoscroll=1;
                memset(&(window->lastrect), 0, sizeof(window->lastrect));
            }

            if (_flags&mWS_VSCROLL){ (window->vscroll=new TScroller(window,-1000,1000,1,SB_VERT))->flags|=SF_MANUAL ;}
            if (_flags&mWS_HSCROLL){ (window->hscroll=new TScroller(window,-1000,1000,1,SB_HORZ))->flags|=SF_MANUAL ;}
            if (_flags&mWS_MODAL)window->modal=1;
            if (mode == WIN_POPUP)window->popup=1;

            if (window->hscroll || window->vscroll) window->UpdateAScrollers();
            switch(mode)
            {
                case WIN_DIALOG:
                {
                    if (win_cx!=CW_USEDEFAULT && win_cy!=CW_USEDEFAULT)
                        SetClientSize(win_cx,win_cy);
                    SetWindowText(window->HWindow,winname);
                }break;
                case WIN_POPUP:
                    window->closeOnLostFocus=TRUE;break;
                case WIN_CHILD:
                {	/*
                    HWND _parent=parent->HWindow;
                    if(parent->mode==WIN_MDI)_parent=parent->mdichild->_client;
                    */
                    SetParent(window->HWindow,_parent);
                }break;
                case WIN_MDI:
                {
                    if (win_cx!=CW_USEDEFAULT && win_cy!=CW_USEDEFAULT)
                        SetClientSize(win_cx,win_cy);
                    CreateMDI(x,y,win_cx,win_cy);
                    window->nonmdi=FALSE;
                }break;
            };

            if (window->hscroll || window->vscroll)
            {
                POINT2D p;
                if(GetOrgSpace2d(hsp,&p))
                {
                    if(window->hscroll)window->hscroll->SetPos(p.x,0);
                    if(window->vscroll)window->vscroll->SetPos(p.y,0);
                }
            }
            RECT r,rd;
            HWND hw=_parent?_parent:window->HWindow;
            GetWindowRect(hw,&r);
            //GetClientRect( GetDesktopWindow(),&rd);
#ifdef WIN32
            SystemParametersInfo(SPI_GETWORKAREA,0,&rd,0);
#else
            GetWindowRect(GetDesktopWindow(),&rd);
#endif
            switch(mode)
            {
                case WIN_POPUP:UpdateWindow(mainwindow->HWindow);
                case WIN_DIALOG:
                case WIN_CHILD:
                {
                    HWindow=window->HWindow;
                    if (win_cx==CW_USEDEFAULT && win_cy==CW_USEDEFAULT)
                    {
                        win_cx=300;
                        win_cy=200;
                    }
                    // <переводим координаты окна в координаты пространства>
                    if(mode==WIN_CHILD && _parent)
                    {
                        char s[64];
                        GetClassName(_parent,s,sizeof(s));
                        if(!lstrcmpi(s,"SC_MSG_WINDOW"))
                        {
                            TMSGWindow*ww=(TMSGWindow*)GetWindowLong(_parent,0);
                            if(ww && ww->hsp)
                            {
                                POINT2D pp;
                                pp.x=x;pp.y=y;
                                LPtoDP2d(ww->hsp,&pp,1);
                                x=pp.x;y=pp.y;
                            }
                        }
                    }
                    // <\переводим координаты окна в координаты пространства>
                    if ((__flags&mWS_CORRECTPOS) ||
                        (win_x==CW_USEDEFAULT && win_y==CW_USEDEFAULT))
                    {
                        if (mode==WIN_POPUP)
                        {
                            if (_flags&mWS_SHADOW)
                            {
                                rd.right-=10;rd.bottom-=10;
                            }
                            if (x<rd.left)x=rd.left;
                            if (y<rd.top)y=rd.top;
                            if (((r.right-r.left)+x)>rd.right)x=rd.right-(r.right-r.left);
                            if (((r.bottom-r.top)+y)>rd.bottom)y=rd.bottom-(r.bottom-r.top);
                        }
                        if (mode==WIN_DIALOG)
                        {
                            x=((rd.right-rd.left)-(r.right-r.left))/2;
                            y=((rd.bottom-rd.top)-(r.bottom-r.top))/2;
                        }
                    }
                    HWindow=window->HWindow;
                    int _cx=win_cx,_cy=win_cy;
                    if (_flags&mWS_SHADOW)
                    {
                        _cx+=10;_cy+=10;
                        POINT2D org;
                        GetOrgSpace2d(hsp,&org);
                        //org.x;org.y;
                        SetOrgSpace2d(hsp,&org);
                        window->shadow=TRUE;
                        /*
                        #ifdef WIN32
                            SetWindowLong(HWindow,GWL_EXSTYLE
                        ,GetWindowLong(HWindow,GWL_EXSTYLE)WS_EX_TOPMOST
                            );
                        #endif
                        */
                        win_cx=_cx;
                        win_cy=_cy;
                    }
                    if(showtype)
                    {
                        ShowWindow(HWindow, (showtype == 1) ? SW_SHOWMAXIMIZED : SW_SHOWMINIMIZED);
                    }
                    else
                    {
                        int setpos = SWP_SHOWWINDOW | SWP_NOZORDER;
                        if((mode!=WIN_CHILD) && (mode!=WIN_DIALOG))
                        {
                            SetClientSize(_cx,_cy);
                            setpos|=SWP_NOSIZE;
                        }
                        if(window->shadow)
                        {
                            HWND h=GetDesktopWindow();
                            RECT r;
                            GetWindowRect(HWindow,&r);
                            InvalidateRect(h,&r,0);
                            UpdateWindow(h);
                        }
                        SetWindowPos(HWindow,0,x,y,win_cx,win_cy,setpos);
                    }
                }	break;
                case WIN_MDI:
                {
                    if(showtype)
                    {
                        ShowWindow(HWindow,(showtype==1)?SW_SHOWMAXIMIZED:SW_SHOWMINIMIZED);
                    }
                }break;
            };
        }
        else
        {
            // Не получилось создать окно
            DeleteSpace2d(hsp);hsp=0;
        }
    }
    SetSize();
    if (window->hscroll || window->vscroll)
        window->UpdateAScrollers();

#ifdef LOCKEXITBUTTONOFWINDOW
    HMENU hMenuHandle;
    hMenuHandle = GetSystemMenu(HWindow, FALSE);
    if (hMenuHandle)
        DeleteMenu(hMenuHandle, SC_CLOSE, MF_BYCOMMAND);
#endif
};

void TWinItem::_CreateSpaceWindow(DWORD exStyle,DWORD f,HWND _parent_)
{
    DWORD __style=f|WS_CLIPCHILDREN;

#ifdef WIN32
    HWND hsw=CreateWindowEx(exStyle,
                        #else
    HWND hsw=CreateWindow(
            #endif
                "SC_MSG_WINDOW", "", __style, win_x, win_y,
                win_cx, win_cy, _parent_, NULL, hInstance, NULL);

    window=(TMSGWindow*)GetWindowLong(hsw,0);

    RegistrationWindowOnMultitouch(hsw);
    if (window)
    {
        window->item = this;
        window->hsp = hsp;
        AssignWindowToSpace2d(hsp, hsw);
        window->ProcessNewSpace();
        window->UpdateScaleBox();
        //SetMenu(_parent_,mainwindow->menu); //???
    }
};

// Browse
TWinItem::TWinItem(char*winname,int table,char *fields,DWORD _flags)
{
    memset(this,0,sizeof(TWinItem));
    TransparentLevel=100;
    TransparentColor=0;
    ChangeTransparentColor=false;
#ifdef DBE
    project=GetCurrentProject();
    flags=_flags;
    browse=1;
    lstrcpyn(name,winname,MAX_WIN_NAME);

    HWND b=CreateWindow("CURSOR_WINDOW",0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_CHILD,
                        CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                        mainwindow->HWindow,(HMENU)10,(HINSTANCE)hInstance,NULL);
    _window=(TWindow*)GetWindowLong(b,0);

    if (_window){
        CreateMDI();
        ((TCursorWindow*)_window)->SetTable(table,fields);
        ((TCursorWindow*)_window)->inmdi=TRUE;

        //  InvalidateRect(HWindow,0,0);
    }
#endif
};

TWinItem::TWinItem(TWindow *w,char*winname)
{
    memset(this,0,sizeof(TWinItem));
    TransparentLevel=100;
    TransparentColor=0;
    ChangeTransparentColor=false;
    project=NULL;
    flags=0;
    browse=0;
    lstrcpyn(name,winname,MAX_WIN_NAME);
    _window=(TWindow*)w;
    HWindow=w->HWindow;
};

BOOL  TWinItem::SetTable(HOBJ2D hobj,int t,char* c){
#ifdef DBE
    if(browse && hobj==0){
        ((TCursorWindow*)_window)->SetTable(t,c);
        return TRUE;
    }

    if(hsp && hobj){
        HWND hwnd = GetControl2d(hsp,hobj);
        if (hwnd ){
            char lpszClassName[64];
            GetClassName(hwnd,lpszClassName,sizeof(lpszClassName));
            if (!lstrcmpi(lpszClassName,"CURSOR_WINDOW")){
                TCursorWindow*b=(TCursorWindow*)GetWindowLong(hwnd,0);;
                if(b){
                    b->SetTable(t,c);
                    return TRUE;
                }}
        }}
#endif
    return FALSE;
};


HSP2D TWinItem::GetClassSpace(PClass _class)
{
    HSP2D _hsp = GetClassScheme(_class->name, NULL, TRUE);
    // Получаем копию пространства
    DeleteEditFrame2d(_hsp);
    return _hsp;
}

void TMSGWindow::ProcessNewSpace()
{
    INT16 i=1;
    HOBJ2D obj;
    FRAMEPROP fp;
    while( (obj = GetObjectFromZOrder2d(hsp, i)) !=0 )
    {
        INT16 t = GetObjectType2d(hsp,obj);
        switch(t)
        {
            case otUSEROBJECT2D:
                if(GetFrameProp2d(hsp,obj,&fp))
                {
                    if(fp.flags & V2_AUTOPLAY)
                    {
                        VIDEOPROP vp;
                        GetVideoProp(fp.video,&vp);
                        UINT32 flags=0;
                        if(fp.flags & V2_CYCLED)
                            flags |= PF_CYCLED;
                        VideoPlay2d(hsp,obj,vp.start,vp.end,0,flags);
                    }
                }break;
        }
        i++;
    }

    WORD W = (WORD)SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
    W &= ~((WORD)0x01);
    SetSpaceParam2d(hsp, SP_SETSPACEFLAG, W);
}

void TMSGWindow::Replace(HSP2D _hsp,char * effect)
{
    if(messages)
        messages->DoneHandler(NULL, WM_SPACEDONE);

    // Грохаем старое
    DeleteSpace2d(hsp);
    hsp = _hsp;
    if(messages)
    {
        messages->hsp = hsp;
        for(C_TYPE i = 0; i < messages->count; i++)
        {
            TClassMSG* msg = messages->msg(i);
            if (msg->code == WM_SPACEINIT)
            {
                OPENMSG openmsg;
                openmsg.hSpace = (INT32)hsp;
                lstrcpy(openmsg.name, item->name);
                lstrcpy(openmsg.target, __target);
                msg->Send(WM_SPACEINIT, &openmsg, FALSE);
            }
        }
    }
    AssignWindowToSpace2d(hsp,HWindow);
    ProcessNewSpace();
    UpdateScaleBox();
    // Присоединяем

    // Делаем эффект
    /*
  if (effect){
    HDC dc=GetDC(HWindow);
     RECT r;
     GetClientRect(HWindow,&r);
     if (r.right && r.bottom){
      HBITMAP hbmp=CreateCompatibleBitmap(dc,r.right,r.bottom);
      HDC mdc=CreateCompatibleDC(dc);
      SelectObject(mdc,hbmp);
      PaintSpace2d(hsp,mdc,&r,PF_DIRECT);

     if (!lstrcmpi(effect,"Horisontal")){
     int count=10;
     int dy=(r.bottom/count)+1;
     int y=0;
     while (y<dy){
      for(int i=0;i<count;i++){
         for(int j=0;j<50;j++)
          BitBlt(dc,0,y+(r.bottom/count)*i,r.right,1,mdc,0,y+(r.bottom/count)*i,SRCCOPY);
        }
        y++;
      }}
     if (!lstrcmpi(effect,"Vertical")){
     int count=10;
     int dx=(r.right/count)+1;
     int x=0;
     while (x<dx){
      for(int i=0;i<count;i++){
         for(int j=0;j<30;j++)
          BitBlt(dc,x+(r.right/count)*i,0,1,r.bottom,mdc,x+(r.right/count)*i,0,SRCCOPY);
        }
        x++;
      }}

      DeleteDC(mdc);
     }
    ReleaseDC(HWindow,dc);
  }else InvalidateRect(HWindow,NULL,0);
  */
    // InvalidateRect(HWindow,NULL,0);
    //SetCursor(0,IDC_ARROW);
    SetDefCursor();
};

void  TWinItem::Activate()
{
    if (mdichild)
    {
        HWND mdi=mainwindow->GetMDI();
        HWND toactive=mdichild->HWindow;
        SendMessage(mdi,WM_MDIACTIVATE,(WPARAM)toactive,0);
        //*------Когда был POPUP то без этого не работае ----------------*//
        if (GetFocus()!=mdichild->_client){ SetFocus(mdichild->_client); }
        //*--------------------------------------------------------------*//
    }
    else
    {
        ShowWindow(window->HWindow,SW_SHOWNA);
        SetFocus(window->HWindow);
    }
};

HSP2D TWinItem::Replace(PClass _class,char*file,HSP2D _hsp)
{
    if (_class==NULL && (file==NULL|| *file==0))
        return hsp;

    if (_class)
    {
        if (!lstrcmp(_class->name,_classname))
            return hsp;
    }

    if (file)
    {
        if (!lstrcmp(file,_filename))
            return hsp;
    }

    if (!_hsp)
    {
        if (_class)
        {
            _hsp=GetClassSpace(_class);
        }
        else
        {
            _hsp=LdSpace2d(0,file);
            DeleteEditFrame2d(_hsp);
        }
    }

    if (!_hsp)
        return hsp;

    TSpaceData data(_hsp);
    if (data.style & SWF_AUTOORG)
    {
        RECT2D r;
        if(GetSpaceExtent2d(_hsp,r))
        {
            if (data.style&SWF_SPACESIZE){
                win_cx=r.right-r.left;
                win_cy=r.bottom-r.top;
            }
            if (data.style&SWF_AUTOORG){
                POINT2D  org=TPoint(r.left,r.top);
                SetOrgSpace2d(_hsp,&org);
            }
        }
    }



    if (_classname){delete _classname;_classname=NULL;}
    if (_filename){delete _filename;_filename=NULL;}

    hsp=_hsp;
    Send();


    POINT2D org;
    GetOrgSpace2d(_hsp,&org);

    if (data.style&SWF_VSCROLL){
        if(!(window->vscroll))
            (window->vscroll=new TScroller(window,-1000,1000,1,SB_VERT))->flags|=SF_MANUAL;
        window->vscroll->Show(TRUE);
        //   window->vscroll->SetPos(org.y);
    }else{
        //if(window->vscroll)window->vscroll->SetRange(0,0);
        if(window->vscroll){
            window->vscroll->Show(FALSE);
            delete window->vscroll;window->vscroll=NULL;
        }
        //    void Show(BOOL show);
    }

    if (data.style&SWF_HSCROLL){
        if(!(window->hscroll))
            (window->hscroll=new TScroller(window,-1000,1000,1,SB_HORZ))->flags|=SF_MANUAL;
        window->hscroll->Show(TRUE);

    }else{
        // if(window->hscroll)window->hscroll->SetRange(0,0);
        if(window->hscroll){
            window->hscroll->Show(FALSE);
            delete window->hscroll;window->hscroll=NULL;
        }
    }
    autoscroll=0;
    if (data.style&SWF_AUTOSCROLL)
    {
        autoscroll=1;
        memset(&(window->lastrect),0,sizeof(window->lastrect));
    }
    //???
    if (!(data.style&SWF_HSCROLL))ShowScrollBar(window->HWindow,SB_HORZ,0);
    if (!(data.style&SWF_VSCROLL))ShowScrollBar(window->HWindow,SB_VERT,0);

    window->Replace(hsp,__effect_);
    BYTE showtype=0;
    BOOL resize=FALSE;
    BOOL zoomed=IsZoomed(HWindow);
    if (data.style&SWF_MINIMIZE){showtype=2;resize=TRUE;}
    if (data.style&SWF_MAXIMIZE)showtype=1;

    if(!showtype){
        resize=(data.style&SWF_SPACESIZE)||(data.sizex>0 && data.sizey>0);
    }

    if ((!zoomed) || resize){
        if(showtype){
            ShowWindow(HWindow,(showtype==1)?SW_SHOWMAXIMIZED:SW_SHOWMINIMIZED);
        }else{
            if((data.sizex>0 && data.sizey>0) || (data.style&SWF_SPACESIZE)){
                if(zoomed)ShowWindow(HWindow,SW_NORMAL);
                if(data.style&SWF_SPACESIZE) SetClientSize(win_cx,win_cy);else
                    SetClientSize(data.sizex,data.sizey);
            }}
    }

    InvalidateRect(window->HWindow,NULL,0);
    if (window->hscroll || window->vscroll){
        if (window->hscroll) window->hscroll->SetPos(org.x,FALSE);
        if (window->vscroll) window->vscroll->SetPos(org.y,FALSE);
    }

    window->UpdateAScrollers();

    __effect_=NULL;


    // HandleMessage(WM_SPACEINIT);
    if (_class)_classname=NewStr(_class->name);
    if (file)_filename=NewStr(file);
    return hsp;
};
void TWinItem::Destroy(){
    if (mdichild){
        if (mainwindow)
            SendMessage(mainwindow->GetMDI(),WM_MDIDESTROY,(WPARAM)mdichild->HWindow,0);
    }else{
        if (window){
            window->closeOnLostFocus=FALSE;
            DestroyWindow(window->HWindow);
        }  }
};
TWinItem::~TWinItem(){

    if (_classname)delete _classname;
    if (_filename)delete _filename;
    if (popupparent)delete popupparent;
};
void TWinItem::CreateMDI(int x,int y,int sizex,int sizey){
    RECT r;
    r.left=x;
    r.top=y;
    r.right=sizex;
    r.bottom=sizey;
    mdichild=mainwindow->CreateMDIChild(name,window,0,&r,MODE_MODEL);
    mdichild->flags|=WF_PROJECT|WF_PROGRAM;
    HWindow=mdichild->HWindow;
};

TWindowManager::TWindowManager():TSortedCollection(10,10)
{
    captured=0;
    capflags=0;
    capobj=NULL;
    allmdi=new TCollection(10,10);
    InsertWin(new TWinItem(mainwindow,"@StratumFrame@"));
};

TWindowManager::~TWindowManager()
{
    if(meta)
        delete meta;
    meta=NULL;
    for(int i=0;i<count;i++)
        FreeItem(At(i));
    delete allmdi;
};

C_TYPE TWindowManager::InsertWin(TWinItem*item)
{
    if (item->window)
    {
        C_TYPE index=Insert(item);
        return index;
    }
    delete item;
    return 0;
};

HSP2D TWindowManager::GetSpace(C_TYPE id)
{
    TWinItem* i=(TWinItem*)At(id);
    if (i)
        return i->hsp;
    return 0;
};

TWinItem* TWindowManager::GetWindow(char* name)
{
    TWinItem item(0, name);
    C_TYPE i;
    if (Search(&item,i))
        return (TWinItem*)At(i);
    return NULL;
};

TWinItem*TWindowManager::GetWindow(HSP2D hsp)
{
    if(hsp)
    {
        for(C_TYPE i=0;i<count;i++)
        {
            TWinItem* w=(TWinItem*)At(i);
            if (w->window && w->hsp==hsp)
                return w;
        }
    }
    return 0;
};

BOOL  TWindowManager::PostCreateMDIWindow(char*name,TMDIChildWindow*w,HSP2D _hsp){
    TWinItem*item=new TWinItem(name,w,_hsp);
    w->flags|=WF_HSP;
    item->project=w->prj;
    Insert(item);
    return TRUE;
};

HSP2D TWindowManager::_PostCreateWindow(C_TYPE i)
{
    if (i < count && i > -1)
        return GetSpace(i);

    return 0;
}

HSP2D TWindowManager::LoadSpace(char*/*wname*/,char*/*filename*/){
    return 0;
};
int TWindowManager::OpenBrowseWindow(char*wname,int table,char* style){
#ifdef DBE
    if (IsTableExist(table))
    {
        TWinItem*w=GetWindow(wname);
        if (w)return 0;
        //DWORD f=
        ProcessFlags(style);
        TWinItem*item=new TWinItem(wname,table,style,0);
        return InsertWin(item);
    }
#endif
    return 0;
};
void TMSGWindow::GetSpaceInfo(char *s){
    wsprintf(s,"Открыто в окне:%s ",item->name);
    char name[128];
    if(GetWindowText(item->HWindow,name,128)){
        wsprintf(s+lstrlen(s)," - [%s]",name);
    };

};

DWORD TWindowManager::ProcessFlags(char *style,RECT */*r*/)
{
    win_x=CW_USEDEFAULT,
            win_y=CW_USEDEFAULT,
            win_cx=CW_USEDEFAULT,
            win_cy=CW_USEDEFAULT;
    DWORD f=0;
    char ss[256];
    lstrcpy(ss,style);
    AnsiUpper(ss);

    if (strpos(ss,"WS_MAXIMIZED")>-1)		f|=mWS_MAXIMIZED;
    if (strpos(ss,"WS_MINIMIZED")>-1)		f|=mWS_MINIMIZED;
    if (strpos(ss,"WS_NOCAPTION")>-1)		f|=mWS_NOCAPTION;
    if (strpos(ss,"WS_TOOL")>-1)				f|=mWS_TOOL;
    if (strpos(ss,"WS_MAXIMIZEBOX")>-1)	f|=mWS_MAXIMIZEBOX;
    if (strpos(ss,"WS_MINIMIZEBOX")>-1)	f|=mWS_MINIMIZEBOX;
    if (strpos(ss,"WS_NOSYSMENU")>-1)		f|=mWS_NOSYSMENU;

    if (strpos(ss,"WS_BYSPACE")>-1)			f|=mWS_BYSPACE;
    if (strpos(ss,"WS_AUTOORG")>-1)			f|=mWS_AUTOORG;
    if (strpos(ss,"WS_AUTOSCROLL")>-1) 	f|=mWS_AUTOSCROLL;
    if (strpos(ss,"WS_HSCROLL")>-1)			f|=mWS_HSCROLL;
    if (strpos(ss,"WS_VSCROLL")>-1)			f|=mWS_VSCROLL;

    if (strpos(ss,"WS_CHILD")>-1)				f|=mWS_CHILD;
    if (strpos(ss,"WS_POPUP")>-1)				f|=mWS_POPUP;
    if (strpos(ss,"WS_DIALOG")>-1)			f|=mWS_DIALOG;
    if (strpos(ss,"WS_MDI")>-1)					f|=mWS_MDI;
    if (strpos(ss,"WS_MODAL")>-1)				f|=mWS_MODAL;

    if (strpos(ss,"WS_NOSHADOW")>-1)		f|=mWS_NOSHADOW;
    if (strpos(ss,"WS_SPACESIZE")>-1)		f|=mWS_SPACESIZE;
    if (strpos(ss,"WS_NORESIZE")>-1)		f|=mWS_NORESIZE;
    if (strpos(ss,"WS_CORRECTPOS")>-1)	f|=mWS_CORRECTPOS;
    if (strpos(ss,"WS_BORDER")>-1)			f|=mWS_BORDER;
    if (strpos(ss,"WS_NOBORDER")>-1)		f|=mWS_BORDER;
    return f;
};


HSP2D TWindowManager::LoadSpaceWindow(char*wname,char*filename,char* style)
{
    TWinItem*w=GetWindow(wname);
    char s[9999];
    filename=GetAbsolutePath(filename,s);
    if (w)
    {
        if(w->_filename){delete w->_filename;w->_filename=NULL;}
        return w->Replace(NULL,filename);
    }
    DWORD f=ProcessFlags(style);
    TWinItem*item=new TWinItem(wname,0,NULL,filename,CW_USEDEFAULT,CW_USEDEFAULT,
                               CW_USEDEFAULT,CW_USEDEFAULT,f);
    //new TWinItem(wname,filename,f);
    return _PostCreateWindow(InsertWin(item));
};

HSP2D TWindowManager::OpenSchemeWindow(char*wname,char* _classn,char* style)
{
    if(*wname=='@')
    {
        if(!lstrcmpi(wname,"@StratumFrame@"))
            return 0;
#ifndef RUNTIME
        if(!lstrcmpi(wname,"@Watch@")) {mainwindow->Cm_Watch();}
        else if(!lstrcmpi(wname,"@Text@")){mainwindow->CreateTextWindow(GetClassByName(_classn));}
        else if(!lstrcmpi(wname,"@Scheme@")){mainwindow->CreateSchemeWindow(_classn,NULL);}
        else if(!lstrcmpi(wname,"@Image@")){mainwindow->CreateImageWindow(_classn);}
        else if(!lstrcmpi(wname,"@Messages@")){mainwindow->Cm_Message();}
        else if(!lstrcmpi(wname,"@Libraries@")){mainwindow->Cm_Libs();}
        else if(!lstrcmpi(wname,"@Hierarchy@")){mainwindow->Cm_Hier();}
#endif
    }
    else
    {
        TWinItem*w=GetWindow(wname);
        PClass _class=GetClassByName(_classn);
        if (w)
        {
            if(w->_classname)
            {
                delete w->_classname;
                w->_classname=NULL;
            }
            return w->Replace(_class, NULL);
        }
        if (_class && _class->flags & CF_SCHEME)
        {
            DWORD f=ProcessFlags(style);
            TWinItem*item=new TWinItem(wname,0,_class,NULL,CW_USEDEFAULT,CW_USEDEFAULT,
                                       CW_USEDEFAULT,CW_USEDEFAULT,f);
            //new TWinItem(wname,_class,f);
            return _PostCreateWindow(InsertWin(item));
        }
        return 0;
    }
    return 0;
}

HSP2D TWindowManager::Loaded(char*wname,char *data,PClass * __class){
    TWinItem*w=GetWindow(wname);
    PClass _class=NULL;
    char * _file=0;
    PClassListInfo _clInfo=GetClassListInfo(data);
    if (_clInfo)_class=_clInfo->GetClass();else _file=data;
    if(__class)(*__class)=_class;

    if(w){
        if (_file){
            if (w->_filename && (!lstrcmpi(w->_filename,_file)))return w->window->hsp;
        }
        if (_class){
            if (w->_classname && (!lstrcmpi(w->_classname,_class->name)))return w->window->hsp;
        }}
    return 0;
};
/*
HSP2D TWindowManager::CreateDlgWindow(HWND parent,char*wname,char *data,int x,int y,int cx,int cy,char *style){
PClass _class=NULL;
char * _file=0;
PClassListInfo _clInfo=GetClassListInfo(data);
if (_clInfo)_class=_clInfo->GetClass();else _file=data;
    DWORD f=ProcessFlags(style);
    win_x=x;win_y=y;
    win_cx=cx;win_cy=cy;
    TWinItem*item=new TWinItem(wname,0,_class,_file,win_x,win_y,win_cx,win_cy,f,parent);
    return _PostCreateWindow(InsertWin(item));

};
*/
HSP2D  TWindowManager::_CreateDialog(HWND parent,char*wname,char * classsname,BOOL part,PObject po)
{
    char s[256];
    if(!wname)
    {
        int i=0;
        do
        {
            wsprintf(s,"dlg_%ld_%s_%d",parent,classsname,i);
            wname=s;
            i++;
        }while(GetWindow(s));

    }
    DWORD style=0;
    if(part)
    {
        style= 0x204L | WS_CHILD | WS_VISIBLE;
    }
    else
    {
        style=WS_THICKFRAME | 0x204L | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
    }
    TWinItem*item=new TWinItem(parent,wname,classsname,0,style,po);
    if(item->window && part)
    {
        item->window->inpsp_page=TRUE;
    }
    return _PostCreateWindow(InsertWin(item));
};

HSP2D TWindowManager::CreateWindowEx_(char* wname, char *parent, char *data, HSP2D _hsp,
                                      int x, int y, int cx, int cy, char *style)
{
    if(lstrlen(wname)==0)
        return 0;

    TWinItem* winItem = GetWindow(wname);
    TWinItem* parentWinItem = GetWindow(parent);

    //если передали имя родительского окна,
    //но такого окна не существует, то выходим
    if (lstrlen(parent) && (parentWinItem==0))
        return 0;

#ifdef WIN32
    if(x == 32768)
        x = CW_USEDEFAULT;

    if(y== 32768)
        y = CW_USEDEFAULT;

    if(cx == 32768)
        cx = CW_USEDEFAULT;

    if(cy == 32768)
        cy = CW_USEDEFAULT;
#endif

    PClass _class=NULL;
    char tmp[9999];
    char * _file=0;
    PClassListInfo _clInfo = GetClassListInfo(data);

    if (_clInfo)
        _class=_clInfo->GetClass();
    else
        _file = GetAbsolutePath(data,tmp);

    if(winItem)
        return winItem->Replace(_class, _file, _hsp);

    DWORD f = ProcessFlags(style);
    win_x=x;
    win_y=y;
    win_cx=cx;
    win_cy=cy;
    HWND _parent=0;

    if(parentWinItem)
    {
        _parent = parentWinItem->HWindow;
        if(parentWinItem->mode == TWinItem::WIN_MDI)
            _parent = parentWinItem->mdichild->_client;
    }

    TWinItem* item = new TWinItem(wname, _hsp, _class, _file, win_x, win_y, win_cx, win_cy, f, _parent);
    if(item && parentWinItem && (item->mode == TWinItem::WIN_CHILD))
        item->Parent = parentWinItem;

    return _PostCreateWindow(InsertWin(item));
};

int TWindowManager::PostDelete(HWND wnd){
    for(C_TYPE i=0;i<count;i++){
        TWinItem* w=(TWinItem*)At(i);

        if (w->HWindow==wnd){
            w->flags&=~WF_PROGRAM;
            AtDelete(i);
            w->window=NULL;
            w->mdichild=NULL;
            delete w;
            return 1;}
    }
    return 0;
};

int   TWindowManager::CloseWindow(char*wname)
{
    TWinItem*w=GetWindow(wname);
    if (w)
    {/*
    w->flags&=~WF_PROGRAM;
    TMDIChildWindow*_window=w->mdichild;
    _window->flags&=~WF_PROGRAM;
    Free(w);*/
        w->Destroy();
        return 1;
    }
    return 0;
};

void TWindowManager::CloseAll(void * project,BOOL system)
{
    C_TYPE i = 0;
    while (i < count)
    {
        TWinItem*w=(TWinItem*)At(i);
        if(w->window && w->window->messages && ((w->project==project) || (project==0)))
        {
            w->window->messages->DoneHandler(project);
        }
        if (w && ((w->project==project) || (project==0)) && (w->system==system))
        {
            w->Destroy();
        }
        else
            i++;
    }
    if(meta && (meta->project==project || project==NULL))
    {
        if(meta)
            delete meta;meta=NULL;
    }
    if(mainwindow && mainwindow->tmenu && mainwindow->tmenu->project==project)
    {
        mainwindow->tmenu->Remove();
        mainwindow->tmenu=NULL;
    }
};

int  TWindowManager::SaveSpace(char*wname,char*filename)
{
    TWinItem*w=GetWindow(wname);
    if (w && w->window)
    {
        State2dSaveAs(w->window->hsp,filename);
        return	1;
    }
    return 0;
}

int   TWindowManager::Compare(pointer key1,pointer key2)
{
    return lstrcmp(((TWinItem*)key1)->name, ((TWinItem*)key2)->name);
};

void  TWindowManager::FreeItem(pointer p){
    TWinItem* w=(TWinItem*)p;
    delete w;
};
int   TWindowManager::SetWindowTitle(char*wname,char*txt){
    TWinItem*w=GetWindow(wname);
    if (w){
        SetWindowText(w->HWindow,txt);
        return	1;
    }return 0;
};
int TWindowManager::GetWindowTitle(char*wname,char*txt){
    TWinItem*w=GetWindow(wname);
    if (w){
        return	GetWindowText(w->HWindow,txt,256);;
    }return 0;
};

int TWindowManager::SetWindowOrg(char*wname,int x,int y)
{
    TWinItem*w=GetWindow(wname);
    if (w)
    {
        if(w && w->Parent && w->Parent->hsp)
        {
            POINT2D _p;
            _p.x=x;_p.y=y;
            LPtoDP2d(w->Parent->hsp,&_p,1);
            x=_p.x;y=_p.y;
        }
        return ::SetWindowPos(w->HWindow,0,x,y,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
    }
    return 0;
};

int TWindowManager::SetWindowSize(char*wname,int sx,int sy)
{
    TWinItem*w=GetWindow(wname);
    if (w)
    {
        int r=::SetWindowPos(w->HWindow,0,0,0,sx,sy,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
        w->SetSize();
        if (w->hsp && w->window && (w->window->hscroll || w->window->vscroll))w->window->UpdateAScrollers();
        return r;
    }
    return 0;
};

int   TWindowManager::ShowWindow(char*wname,int nCmdShow){
    TWinItem*w=GetWindow(wname);
    if (w){
        return ::ShowWindow(w->HWindow,nCmdShow);
    }return 0;
};

int   TWindowManager::SetWindowPos(char*wname,int x,int y,int sx,int sy)
{
    TWinItem*w=GetWindow(wname);
    if (w)
    {
        if(w && w->Parent && w->Parent->hsp)
        {
            POINT2D _p;
            _p.x=x;_p.y=y;
            LPtoDP2d(w->Parent->hsp,&_p,1);
            x=_p.x;y=_p.y;
        }
        int r=::SetWindowPos(w->HWindow,0,x,y,sx,sy,SWP_NOZORDER|SWP_NOACTIVATE);
        w->SetSize();
        if (w->hsp && w->window && (w->window->hscroll || w->window->vscroll))
            w->window->UpdateAScrollers();
        return r;
    }
    return 0;
};

int   TWindowManager::IsWindowVisible(char*wname){
    TWinItem*w=GetWindow(wname);
    if (w){return ::IsWindowVisible(w->HWindow);
    }return 0;
};

int TWindowManager::WindowRect(char*wname,RECT &r)
{
    TWinItem*w=GetWindow(wname);
    if (w)
    {
        GetWindowRect(w->HWindow,&r);
        return 1;
    }
    r.left=r.top=r.bottom=r.right=0;
    return 0;
};

int TWindowManager::IsIconic(char*wname){
    TWinItem*w=GetWindow(wname);
    if (w){return ::IsIconic(w->HWindow);
    }return 0;
};
BOOL   TWindowManager::IsWindowExist(char*wname){
    TWinItem*w=GetWindow(wname);
    if (w){return TRUE;}
    return FALSE;
};
int TWindowManager::GetNameByHSP(HSP2D hsp,char *name){
    TWinItem*w=GetWindow(hsp);
    if (w){lstrcpy(name,w->name);return 1;}return 0;
};
HSP2D TWindowManager::GetHANDLE(char *name){
    TWinItem*w=GetWindow(name);
    if (w){return w->hsp;}return 0;
};

int TWindowManager::BringWindowToTop(char*wname){
    TWinItem*w=GetWindow(wname);
    if (w){
        return ::BringWindowToTop(w->HWindow);
    }return 0;
};
int TWindowManager::UpdateMSG(){
    for(int i=0;i<count;i++){
        TWinItem*w=(TWinItem*)At(i);
        if (w->hsp && w->window && w->window->messages){
            w->window->messages->Update();
        }
    }
    return 0;
};
void   TWindowManager::CascadeWindows(){
    mainwindow->Cm_Cascade();

};
void  TWindowManager::Tile(BOOL vertical){
    if (vertical)mainwindow->Cm_TileV();else mainwindow->Cm_TileH();
};
void   TWindowManager::ArrangeIcons(){
    mainwindow->Cm_Arrange();
};
BOOL  TWindowManager::InputBox(char *title,char*msg, char*s){
    return ::InputBox(mainwindow->HWindow,title,msg,s);
}

void TWindowManager::StdHyperJump(HSP2D hsp,POINT2D& p,HOBJ2D obj,UINT16 fwKeys)
{
    if (TWinItem* w = GetWindow(hsp))
        w->window->StdHyperJump(p,obj,fwKeys);
}

BOOL  TWindowManager::mFileDialog(char * s, char * szFilter, BOOL save)
{
    return _FileDialog(mainwindow->HWindow, s, szFilter, save ? FD_SAVE : 0);
}

BOOL TWindowManager::GetWindowProp(char*wname,char*info,char *data){
    TWinItem*w=GetWindow(wname);
    if (w){
        if(w->window && w->hsp){
            if(!lstrcmpi(info,"classname")){
                if(w->_classname){
                    lstrcpy(data,w->_classname);
                    return TRUE;
                }
                return FALSE;
            }
            if(!lstrcmpi(info,"filename")){
                if(w->_filename){
                    lstrcpy(data,w->_filename);
                    return TRUE;
                }
                return FALSE;
            }
        }
        if(!strncmpi(info,"collumnwidth:",lstrlen("collumnwidth:")))
        {
            if(w->browse)
            {
                TCursorWindow*cursor=(TCursorWindow*)w->window;
                TBrowser*browser=(TBrowser*)GetWindowLong(cursor->browser,0);
                int index=atol(info+lstrlen("collumnwidth:"));
                if((index>0) && (index<=browser->items->count))
                {
                    TCollumn *c=(TCollumn *)browser->items->At((int)index-1);
                    wsprintf(data,"%d",c->current);
                }
                return TRUE;
            } }
    }
    return FALSE;
};

BOOL TWindowManager::PostStep()
{
    for(C_TYPE i=0;i<count;i++)
    {
        TWinItem*w=(TWinItem*)At(i);
        if(w->autoscroll && IsSpaceRectChanged2d(w->hsp))
        {
            w->window->AutoScroll();
        }
    }
    return TRUE;
};

BOOL TWindowManager::SetScrollRange(char*wname,BOOL horisontal,int p1,int p2)
{
    TWinItem*w=GetWindow(wname);
    if (w)
    {
        if (horisontal)
        {
            if (w->window->hscroll)w->window->hscroll->SetRange(p1,p2);
            else{
                POINT2D p;
                GetOrgSpace2d(w->hsp,&p);
                (w->window->hscroll=new TScroller(w->window,p1,p2,1,SB_HORZ))->flags|=SF_MANUAL;
                w->window->hscroll->SetPos(p.x,0);
                w->window->hscroll->SetRange(p1,p2);
            }
        }else{
            if (w->window->vscroll)w->window->vscroll->SetRange(p1,p2);
            else{
                POINT2D p;
                GetOrgSpace2d(w->hsp,&p);
                (w->window->vscroll=new TScroller(w->window,p1,p2,1,SB_VERT))->flags|=SF_MANUAL;
                w->window->vscroll->SetPos(p.y,0);
                w->window->vscroll->SetRange(p1,p2);
            }
        }
        return TRUE;
    }
    return FALSE;
};

BOOL  TWindowManager::SetTable(char *wname,int t,char*s)
{
    TWinItem*w=GetWindow(wname);
    if (w)
        return w->SetTable(0, t, s);

    return 0;
}
BOOL  TWindowManager::SetTable(HSP2D hsp,HOBJ2D hobj,int t,char*s){
    TWinItem*w=GetWindow(hsp);
    if (w)return w->SetTable(hobj,t,s);
    return 0;
};

int  TWindowManager::GetClientWidth(char*wname)
{
    TWinItem*w=GetWindow(wname);
    if(w)
    {
        RECT r;
        GetClientRect(w->HWindow,&r);
        if(w->hsp)
            return (((double)r.right)/GetScaleSpace2d(w->hsp));
        return r.right;
    }
    return 0;
};

int  TWindowManager::GetClientHeight(char*wname){
    TWinItem*w=GetWindow(wname);
    if (w){
        RECT r;GetClientRect(w->HWindow,&r);
        if(w->hsp)return (((double)r.bottom)/GetScaleSpace2d(w->hsp));
        return r.bottom;
    }return 0;
};

void TWinItem::SetClientSize(int x,int y){
    int _x=x,_y=y;

    if(mode==WIN_MDI){
        if(mdichild){mdichild->GetWinSizeByClient(_x,_y);
            if(window && window!=mdichild)window->GetWinSizeByClient(_x,_y);
        }else{
            window->GetWinSizeByClient(_x,_y);
            int fy=GetSystemMetrics(SM_CYCAPTION);
            int bx=GetSystemMetrics(SM_CXFRAME);
            int by=GetSystemMetrics(SM_CYFRAME);
            _x=_x+bx*2;
            _y=_y+fy+by*2;
        }
        if (mdichild){
            ::SetWindowPos(HWindow,0,0,0,_x,_y,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
        }else {win_cx=_x;win_cy=_y;}
    }else {
        window->GetWinSizeByClient(_x,_y);
        win_cx=_x;win_cy=_y;
        ::SetWindowPos(HWindow,0,0,0,_x,_y,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
    }
};

//Добавил Марченко С.В.

BOOL (WINAPI *SetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);
const long WS_EX_LAYERED=0x80000;
const long LWA_COLORKEY=0x01;//не будут отображаться те цвета, которые указаны во втором параметре SetLayeredWindowAttributes
const long LWA_ALPHA=0x02;   //прозрачность всего окна, уровень прозрачности устанавливается в третьем параметре SetLayeredWindowAttributes


int TWinItem::SetTransparent()
{
    HINSTANCE hMod = LoadLibrary("user32.dll");
    if(hMod)
    {
        SetLayeredWindowAttributes =(int(__stdcall*)(HWND,COLORREF,BYTE,DWORD))GetProcAddress(hMod,"SetLayeredWindowAttributes");
        if(SetLayeredWindowAttributes)
        {
            if(SetWindowLong(HWindow,GWL_EXSTYLE,GetWindowLong(HWindow, GWL_EXSTYLE) | WS_EX_LAYERED))
            {
                TransparentLevel=min(100,max(0,TransparentLevel)); //допустимое значение - 0..100
                if(ChangeTransparentColor)
                    SetLayeredWindowAttributes(HWindow,TransparentColor,(255 * TransparentLevel) / 100,LWA_ALPHA|LWA_COLORKEY);
                else
                    SetLayeredWindowAttributes(HWindow,0,(255 * TransparentLevel) / 100,LWA_ALPHA);
                FreeLibrary(hMod);
                return 1;
            }
        }
        FreeLibrary(hMod);
    }
    return 0;
}

int TWinItem::SetTransparent(int NewLevel)
{
    TransparentLevel=min(100,max(0,NewLevel)); //допустимое значение - 0..100
    return SetTransparent();
}

int TWinItem::SetTransparent(COLORREF NewColor)
{
    TransparentColor = NewColor;
    ChangeTransparentColor = true;
    return SetTransparent();
}


int TWinItem::SetWindowRegion(long Region)
{
    if(Region==0)
    {
        //POINT p;p.x=10;p.y=10;
        //HRGN Reg = CreatePolygonRgn(&p,1, WINDING);
        //if(GetWindowRgn(HWindow, 0))
        //if(Reg ||1)
        return SetWindowRgn(HWindow, NULL, true);
        //else
        //  return 0;
    }

    if(dyno_mem->index_by_H(Region) < 0)
        return 0;

    if(dyno_mem->GetCount(Region) % 2 != 0 || dyno_mem->GetCount(Region) < 6)
        return 0;

    POINT* p = new POINT[dyno_mem->GetCount(Region)/2];
    double data;
    for(int i=0; i < dyno_mem->GetCount(Region); i += 2)
    {
        if(lstrcmp(dyno_mem->GetType(Region,i), "FLOAT")!=0)
            return 0;

        dyno_mem->SetVar(Region, i, "FLOAT", 0x80|0, &data);
        p[i/2].x = data;
        if(lstrcmp(dyno_mem->GetType(Region,i+1), "FLOAT")!=0)
            return 0;

        dyno_mem->SetVar(Region, i+1, "FLOAT", 0x80|0, &data);
        p[i/2].y = data;
    }
    HRGN Reg = CreatePolygonRgn(p, dyno_mem->GetCount(Region)/2, WINDING);
    return SetWindowRgn(HWindow, Reg, true);
}
// -конец- Добавил Марченко С.В.

BOOL TWindowManager::SetOrg(HSP2D hsp, gr_float x, gr_float y)
{
    TWinItem*w=GetWindow(hsp);
    if (w)
    {
        POINT2D p;
        GetOrgSpace2d(hsp,&p);
        if (p.x != x || p.y != y)
        {
            p.x = x;
            p.y = y;
            SetOrgSpace2d(hsp, &p);

            if(w->window->hscroll)w->window->hscroll->SetPos(p.x,0);
            if(w->window->vscroll)w->window->vscroll->SetPos(p.y,0);
            InvalidateRect(w->window->HWindow,0,0);
        }
        return TRUE;
    }
    return FALSE;
}

int  TWindowManager::SetClientSize(char*wname,int x, int y)
{
    TWinItem*w=GetWindow(wname);
    if (w){w->SetClientSize(x,y);
        w->SetSize();
        return 1;
    }return 0;
}

COLORREF  TWindowManager::ColorDialog(char*title,COLORREF rgb)
{
    COLORREF newrgb=rgb;
    if(_ChoseColor(mainwindow->HWindow,newrgb,title))
        return newrgb;

    return rgb;
}

BOOL _UnRegisterObject(TWinItem*w,PObject po,INT16 code)
{
    if (w && w->window->messages)
    {
        if (w->window->messages->lock)
        {
            _Message("Unable unregister object");
            return 0;
        };
        C_TYPE i=0;
        BOOL remove=FALSE;
        while(i<w->window->messages->count)
        {
            TClassMSG *msg=w->window->messages->msg(i);
            if ((msg->pobject==po) && (code==0 || code==msg->code))
            {
                w->window->messages->AtFree(i);
                remove=TRUE;
            }
            else
                i++;
        }
        if(!(w->window->messages->count) && remove)
        {
            DeleteCollection(w->window->messages);
            w->window->messages=NULL;
        }
        return remove;
    }
    return 0;
}
BOOL TWindowManager::UnRegisterObject(PObject po, HSP2D hsp, INT16 code)
{
    if (po==NULL)return 0;
    TWinItem* w = GetWindow(hsp);
    return _UnRegisterObject(GetWindow(hsp), po, code);
}

BOOL TWindowManager::UnRegisterObject(PObject po, char* winname, INT16 code)
{
    if (po==NULL)return 0;
    return _UnRegisterObject(GetWindow(winname), po, code);
}

BOOL TWindowManager::RegisterObject(PObject po, HSP2D hsp, HOBJ2D obj, INT16 code, UINT32 flags)
{
    if (po == NULL)
        return 0;

    TWinItem* w = GetWindow(hsp);
    if (w)
    {
        if(!w->window->messages)
            w->window->messages = new TMSGItems(w->window->HWindow, hsp);

        w->window->messages->RegisterObject(po, obj, code, flags);
    }
    return 0;
}

BOOL TWindowManager::RegisterObject(PObject po, char * winname,HOBJ2D obj,INT16 code,UINT32 flags)
{
    if (po==NULL)
        return 0;
    TWinItem* w = GetWindow(winname);
    if (w)
    {
        if(!w->window->messages)
            w->window->messages = new TMSGItems(w->window->HWindow,w->hsp);

        w->window->messages->RegisterObject(po,obj,code,flags);
    }
    return 0;
}

HMENU MyGetMenuItem(HMENU m,int cmd)
{
    int cItems = GetMenuItemCount(m);
    for (int pos = 0; pos < cItems; pos++)
    {
        int id = GetMenuItemID(m,pos);
        if (id==-1)
        {
            HMENU submenu=GetSubMenu(m,pos);
            if (submenu)
            {
                HMENU _m=MyGetMenuItem(submenu,cmd);
                if(_m)
                    return _m;
            }
        }
        else
        {
            if (id==cmd)
            {
                return m;
            }
        }
    }
    return NULL;
}

void MyCheckMenuItem(HMENU m,int cmd,BOOL check)
{
    int cItems = GetMenuItemCount(m);
    for (int pos = 0; pos < cItems; pos++) {
        int id = GetMenuItemID(m,pos);
        if (id==-1){
            HMENU submenu=GetSubMenu(m,pos);
            if (submenu)MyCheckMenuItem(submenu,cmd,check);
        }else {
            if (id==cmd){
                CheckMenuItem(m,pos,MF_BYPOSITION|(check?MF_CHECKED:MF_UNCHECKED));
            }
        }
    }
};
void MyEnableMenuItem(HMENU m,int cmd,BOOL check){
    int cItems = GetMenuItemCount(m);
    for (int pos = 0; pos < cItems; pos++) {
        int id = GetMenuItemID(m,pos);
        if (id==-1){
            HMENU submenu=GetSubMenu(m,pos);
            if (submenu)MyEnableMenuItem(submenu,cmd,check);
        }else {
            if (id==cmd){
                EnableMenuItem(m,pos,MF_BYPOSITION|(check?MF_ENABLED:(MF_DISABLED|MF_GRAYED)));
            }
        }
    }
};
BOOL TWindowManager::CheckMenuItem(char * s,int id,UINT32 f)
{
    TWinItem*w=GetWindow(s);
    if(w && w->HWindow){
        TWindow*_w=(TWindow*)GetWindowLong(w->HWindow,0);
        if(_w->tmenu){
            MyCheckMenuItem(_w->tmenu->menu,id,f);
            return TRUE;
        }
    }
    return FALSE;
};
BOOL TWindowManager::EnableMenuItem(char * s,int id,UINT32 f)
{
    TWinItem*w=GetWindow(s);
    if(w && w->HWindow){
        TWindow*_w=(TWindow*)GetWindowLong(w->HWindow,0);
        if(_w->tmenu){
            MyEnableMenuItem(_w->tmenu->menu,id,f);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL TWindowManager::LoadMenu(char * s,char *menu,UINT32 f){
    TWinItem*w=GetWindow(s);
    if(w && w->HWindow){
        TWindow*_w=(TWindow*)GetWindowLong(w->HWindow,0);
        TDOSStream st(menu,TDOSStream::stOpenRead);
        TMenu* menu=new TMenu(&st);
        if(!menu->Ok()){delete menu;return FALSE;}
        if(_w->tmenu)DeleteMenu(s);
        menu->project=GetCurrentProject();
        _w->tmenu=menu;
        return menu->Assign(_w);
        //SetMenu(w->HWindow,mainwindow->menu);
    }
    return FALSE;
};

BOOL TWindowManager::DeleteMenu(char * s){
    TWinItem*w=GetWindow(s);
    if(w && w->HWindow){
        TWindow*_w=(TWindow*)GetWindowLong(w->HWindow,0);
        if(_w->tmenu){
            delete _w->tmenu;
            _w->tmenu=NULL;
            return TRUE;
        }
    }
    return FALSE;
};

TMDIChildWindow * TWindowManager::GetChild(int id){
    for(int i=0;i<allmdi->count;i++){
        TMDIChildWindow *m=(TMDIChildWindow *)allmdi->At(i);
#ifdef WIN32
        int _id=GetWindowLong(m->_client,GWL_ID);
#else
        int _id=GetWindowWord(m->_client,GWW_ID);
#endif
        if (id==_id)return m;
    }
    return 0;
};

void TWindowManager::HideProjectWindows()
{
    for(int i=0;i<count;i++)
    {
        TWinItem* w = (TWinItem*)At(i);
        if(w->project)
            ::ShowWindow(w->HWindow,SW_HIDE);
    }
};

void TWindowManager::ShowProjectWindows(void*p){
    for(int i=0;i<count;i++)
    {
        TWinItem*w=(TWinItem*)At(i);
        if(w->project && ((w->project==p)||(p==0)))::ShowWindow(w->HWindow,SW_SHOW);
    }
};

BOOL TWindowManager::setcapture(PObject po, HSP2D hsp, UINT32 flags)
{
    if (po == NULL)
        return FALSE;

    TWinItem* w = GetWindow(hsp);
    BOOL ok = 0;
    for(C_TYPE i = 0; i < w->window->messages->count; i++)
    {
        TClassMSG* msg = w->window->messages->msg(i);
        if (msg->pobject == po)
        {
            ok = 1;
            break;
        }
    }
    if (ok)
    {
        capobj = po;
        captured = w;
        capflags = flags;
        SetCapture(w->window->HWindow);
    }
    return TRUE;
}

BOOL TWindowManager::releasecapture()
{
    if (captured)
    {
        ReleaseCapture();
        captured=NULL;
        capobj=NULL;
        return TRUE;
    }
    return FALSE;
};


//*----------------------------------------------------*//
const UINT H_TARGET  = 1;
const UINT H_WINNAME = 2;
const UINT H_OBJECT  = 3;
const UINT H_MODE    = 4;
const UINT H_EFFECT  = 5;
const UINT H_TIME    = 6;
const UINT H_DISABLED =7;
const UINT H_ADD      =8;

void THyperKey::Store(PStream ps,BOOL storeNULL){
    if (target){ps->WriteWord(H_TARGET);ps->WriteStr(target);  }
    if (winname){ps->WriteWord(H_WINNAME);ps->WriteStr(winname); }
    if (object){ps->WriteWord(H_OBJECT);ps->WriteStr(object); }
    if (effect){ps->WriteWord(H_EFFECT);ps->WriteStr(effect); }
    if (openmode){ps->WriteWord(H_MODE);ps->WriteWord(openmode);}
    if (time){ps->WriteWord(H_TIME);ps->WriteWord(time);}
    if (disabled)ps->WriteWord(H_DISABLED);
    if (add){ps->WriteWord(H_ADD);ps->WriteStr(add); }


    if(storeNULL)ps->WriteWord(0);
};

void THyperKey::Load(PStream st){
    if (st){
        int code=0;
        while ((code=st->ReadWord())!=0){
            switch(code){
                case H_TARGET:target=st->ReadStr();break;
                case H_WINNAME:winname=st->ReadStr();break;;
                case H_OBJECT:object=st->ReadStr();break;
                case H_MODE:openmode=st->ReadWord();break;
                case H_EFFECT:effect=st->ReadStr();break;
                case H_TIME:time=st->ReadWord();break;
                case H_ADD:add=st->ReadStr();break;
                case H_DISABLED:disabled=1;break;
            }}}
};

THyperKey::THyperKey(HSP2D _hsp,HOBJ2D _obj,PStream ps)
{
    memset(this, 0, sizeof(THyperKey));

    if(ps)
        Load(ps);
    else
    {
        hsp = _hsp;
        obj = _obj;
        st = ObjectData(hsp, obj, UD_HYPERKEY);
        Load(st);
    }
}

THyperKey::Update()
{
    TMemoryStream ms(TRUE);
    Store(&ms,FALSE);
    INT16 __size=(INT16)ms.GetSize();
    if (__size>2)
    {
        SetObjectData2d(hsp,obj,UD_HYPERKEY,ms.base,0,0);
        SetObjectData2d(hsp,obj,UD_HYPERKEY,ms.base,0,__size);
    }else SetObjectData2d(hsp,obj,UD_HYPERKEY,NULL,0,0);
    return 0;
};

THyperKey::~THyperKey(){
    if     (st)delete st;
    if (target)delete target;
    if(winname)delete winname;
    if(add)delete add;
    if (object) delete object;
    if (effect)delete effect;
};
void THyperKey::_SetStr(char*&_target,char*t){
    if (lstrcmp(_target,t)){
        if (_target){
            delete _target;
            _target=NULL;};
        if (t && (*t!=0))_target=NewStr(t);
        modify=1;
    }
};

TAnime::TAnime(char*_a){
    memset(this,0,sizeof(TAnime));
    name=NewStr(_a);
    time2=10.0;
    vars=new TCollection(5,5);
};

TAnime::TAnime(PStream ps){
    memset(this,0,sizeof(TAnime));
    name=ps->ReadStr();
    flags=ps->ReadLong();
    ps->Read(&time1,sizeof(time1)*2);
    vars=new TCollection(5,5);
    C_TYPE c=ps->ReadWord();
    for(C_TYPE i=0;i<c;i++){
        TAniVar*v=new TAniVar;
        v->name=ps->ReadStr();
        char type [65];
        ps->ReadStr(type,sizeof(type));
        v->type=GetClassListInfo(type);
        v->data=ps->ReadStr();
    }
    c=ps->ReadWord();
    if(c)next = new TAnime(ps);
};

TAnime::~TAnime(){
    if (name)delete name;
    for(C_TYPE i=0;i<vars->count;i++)
    {
        TAniVar*v=(TAniVar*)vars->At(i);
        delete v;
    }
    if(next)delete next;
};

void TAnime::Store(PStream ps){
    ps->WriteStr(name);
    ps->WriteLong(flags);
    ps->Write(&time1,sizeof(time1)*2);
    ps->WriteWord(vars->count);
    for(C_TYPE i=0;i<vars->count;i++)
    {TAniVar*v=(TAniVar*)vars->At(i);
        ps->WriteStr(v->name);
        ps->WriteStr(v->type->GetClassName());
        ps->WriteStr(v->data);
    }
    if(next){ps->WriteWord(1);next->Store(ps);}else ps->WriteWord(0);
};
TAniVar::~TAniVar(){
    if(name)delete name;
    if(data)delete data;
};
PClass TAnime::GetClass(BOOL dlg){
    char s[64];
    if(dlg)wsprintf(s,"Animate_Dialog_%s",name);
    else wsprintf(s,"Animate_Proc_%s",name);
    return GetClassByName(s);
};

void TAnime::Insert(TAnime*a){
    if(next)next->Insert(a);else next=a;
};

//-----------------
INT16  TMetaFile::ProcessMetaRecord(INT16 code)
{
    if(proc)
    {
        return proc(meta, code, data);
    }
    return 0;
}

TMetaFile::TMetaFile(PROCESSMETAPROC p,UINT32 _data)
{
    memset(this,0,sizeof(TMetaFile));
    data = _data;
    proc = p;
    meta = new TDOSStream(NULL,TDOSStream::stCreate);
    Init();
}

TMetaFile::TMetaFile(PStream ps,PROCESSMETAPROC p,UINT32 _data){
    memset(this,0,sizeof(TMetaFile));
    data=_data;
    proc=p;
    meta =ps;
    Init();
}

void TMetaFile::Init()
{
    METASIGNATURE ms;
    ms.SIGNATURE = 0x4154454dL;
    ms.fileversion = currentmetaversion;
    ms.minversion = currentmetaversion;
    ms.dir = NULL;
    ms.image = sizeof(ms);

    if (meta->status==0)
    {
        meta->Write(&ms,sizeof(ms));
        WriteLastRecord(0l);
    }
}

TMetaFile::~TMetaFile()
{
    if(meta)
    {
        meta->Truncate();
        delete meta;
    }
}

void TMetaFile::WriteLastRecord(long pp)
{
    mrecord.PrevRecord=pp;
    mrecord.RedoCode=0;
    mrecord.UndoCode=0;
    mrecord.UndoPos=0;
    mrecord.NextRecord=0;
    meta->Write(&mrecord,sizeof(META_RECORD));
    meta->Truncate();
    meta->Reset();
}

void TMetaFile::NewMetaRecord(INT16 code)
{
    if ((meta->status==0)&&((record_flag & 6)==0))
    {
        mrecord.NextRecord=meta->GetPos();
        mrecord.RedoCode=record_flag&1?(-code):(code);
        record_flag&=(0xfe);
        record_flag|=2;
    }
    else
    {
        /*
  if(!(record_flag&4)){
    NewUndoRecord(ME_NONE);
  }
  CloseMetaRecord();*/
        _Error(EM_internal);
    }
}

void TMetaFile::NewUndoRecord(INT16 code)
{
    if ((meta->status==0)&&((record_flag & 6)==2))
    {
        mrecord.UndoCode=code;
        mrecord.UndoPos=meta->GetPos()-mrecord.NextRecord;
        record_flag |=6;
    }
    else
        _Error(EM_internal);
}

void TMetaFile::CloseMetaRecord()
{
    if ((meta->status==0)&&((record_flag & 6)==6))
    {
        long p=meta->GetPos(),p1=mrecord.NextRecord-sizeof(META_RECORD);
        mrecord.NextRecord=p;
        meta->Seek(p1);
        meta->Write(&mrecord,sizeof(META_RECORD));
        meta->Seek(p);
        WriteLastRecord(p1);
        record_flag &=~6;
    }
    else
        _Error(EM_internal);
};

INT16  TMetaFile::DoRedo()
{
    if ((meta->status==0))
    {
        if (mrecord.NextRecord)
        {
            INT16 rc=mrecord.RedoCode;
            ProcessMetaRecord((UINT16)abs(rc));
            meta->Seek(mrecord.NextRecord);
            meta->Read(&mrecord,sizeof(META_RECORD));
            return rc;
        }
        return 0;
    }
    else
    {
        _Error(EM_internal);
        return 0;
    }
};

INT16  TMetaFile::DoUndo(UINT _code)
{
    if ((meta->status==0))
    {
        BOOL retry =TRUE;

        while(retry)
        {
            retry = FALSE;
            if (mrecord.PrevRecord)
            {
                INT16 uc;
                long _pos=meta->GetPos();
                META_RECORD _mrecord=mrecord;
                meta->Seek(mrecord.PrevRecord);
                meta->Read(&mrecord,sizeof(META_RECORD));
                long pos=meta->GetPos();
                meta->Seek(pos+mrecord.UndoPos);
                uc=mrecord.UndoCode;

                if(uc == ME_UNLOADPROJECT)
                {
                    if(::project && ::project->hyperadd)
                    {
                        if(::project->hyperadd->save==2)
                        {
                            if(!mainwindow->CanCloseProject())
                            {
                                meta->Seek(_pos);
                                mrecord=_mrecord;
                                return 0;
                            }
                        }
                    }
                }
                if(_code==0 || (_code==uc))
                    ProcessMetaRecord(uc);
                else
                    retry=TRUE;

                meta->Seek(pos);
                if(!retry)
                    return uc;
            }
        };return 0;
    }else {_Error(EM_internal);return 0;}
}

TMetaWriter::TMetaWriter(TMetaFile*_meta){
    meta=_meta;
};
BOOL TMetaFile::CanUndo(){
    if (mrecord.PrevRecord)return TRUE;
    return FALSE;
};
BOOL TMetaFile::CanRedo(){
    if(mrecord.NextRecord)return TRUE;
    return FALSE;
};

TMetaWriter::~TMetaWriter(){
    if(meta)
        if(meta->record_flag&6){
            if(!(meta->record_flag&4)){
                meta->NewUndoRecord(ME_NONE);
            }
            meta->CloseMetaRecord();
        }
};


