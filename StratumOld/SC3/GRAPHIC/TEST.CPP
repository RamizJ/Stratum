/*
  WARNING!  WARNING!  WARNING!  WARNING!
  do not compile without 3d.def file
  stack may overflow !
*/
//#define _GRAPHIC_CPP_NAME
#define _3D_TOOLS_STATIC_
#include <gwin2d3d\gwinfunc.h>
#include <stdlib.h>
#include <mem.h>
#include <commdlg.h>
#include <dir.h>
#include <dos.h>
#include <math.h>
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#include "test.rh"
//#include "winhook.h"
#include "3dtools.h"
//#include <vfw.h>


#define __store
#define __path "c:\\sc2\\graphics\\"
#define SQR(x) ((x)*(x))

HOBJ2D obj3d,hcamera,hobject3d;
HSP3D  hsp3d;

#define sfdown3d 1
#define sfmove3d 2

POINT3D base,org3d,size3d;
POINT2D lastpoint;

int state=0;
void SetActiveControl(HSP2D hsp,HOBJ2D hObject,BOOL grp=TRUE){
    HWND hw=GetControl2d(hsp,hObject);
    char s[64];
    GetClassName(hw,s,sizeof(s));

    HOBJ2D hgroup=GetObjectParent2d(hsp,hObject);

    if(!lstrcmpi(s,"BUTTON")){
        LONG style=GetWindowLong(hw,GWL_STYLE);
        int _style=(int)(style&0xf);
        if((_style==BS_AUTORADIOBUTTON)||(_style==BS_RADIOBUTTON)){
            if(hgroup){
                INT16 c=GetGroupItemsNum2d(hsp,hgroup);
                if(grp){
                    HWND __hw=0;
                    for(INT16 i=0;i<c;i++){
                        HOBJ2D ho=GetGroupItem2d(hsp,hgroup,i);
                        HWND _hw=GetControl2d(hsp,ho);
                        if(!i)__hw=_hw;
                        if(SendMessage(_hw,BM_GETCHECK,0,0)){
                            SetFocus(_hw);
                            return;
                        }}
                    SetFocus(__hw);
                    SendMessage(__hw,BM_SETCHECK,1,0);
                }else{
                    for(INT16 i=0;i<c;i++){
                        HOBJ2D ho=GetGroupItem2d(hsp,hgroup,i);
                        HWND _hw=GetControl2d(hsp,ho);
                        SendMessage(_hw,BM_SETCHECK,ho==hObject,0);
                    }
                }
            }else	SendMessage(hw,BM_SETCHECK,1,0);
        }
    }


    SetFocus(hw);
};

typedef struct tagCHILDKEY{
    HWND   hwnd;
    HOBJ2D hObject;
    int    nVirtKey;
    long   lKeyData;
    BOOL   calldef;
}CHILDKEY;

BOOL DefSpaceWindowProc(HSP2D hsp, HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
        case WM_KEYDOWN:if(wParam==VK_TAB){
                BOOL shift=(GetAsyncKeyState(VK_SHIFT)<0);
                HOBJ2D next=(HOBJ2D)1;
                if(shift)next=GetTopObject2d(hsp);else
                    next=GetBottomObject2d(hsp);
                while(next){
                    if(shift)next=GetLowerObject2d(hsp,next);else
                        next=GetUpperObject2d(hsp,next);

                    if(next && GetObjectType2d(hsp,next)==otCONTROL2D){
                        SetActiveControl(hsp,next);
                        return FALSE;
                    };
                }
            }break;
        case WM_PARENTNOTIFY:if(wParam==WM_KEYDOWN){
                CHILDKEY &c1=*(CHILDKEY*)lParam;
                switch(c1.nVirtKey){
                    case VK_UP:
                    case VK_DOWN:{
                        int dlg=SendMessage(c1.hwnd,WM_GETDLGCODE,0,0);
                        if(!(dlg&DLGC_WANTARROWS)){
                            HOBJ2D hgroup=GetObjectParent2d(hsp,c1.hObject);
                            if(hgroup){
                                INT16 i=IsGroupContainObject2d(hsp,hgroup,c1.hObject);
                                if(i){
                                    INT16 c=GetGroupItemsNum2d(hsp,hgroup);
                                    i--;
                                    if(c1.nVirtKey==VK_DOWN){
                                        i++;
                                        if(i>=c)i=0;
                                    }else{
                                        if(i<=0)i=c-1;else i--;
                                    }
                                    HOBJ2D ho=GetGroupItem2d(hsp,hgroup,i);
                                    SetActiveControl(hsp,ho,FALSE);
                                }
                            }
                        }
                    }return FALSE;
                    case VK_TAB:{
                        BOOL shift=(GetAsyncKeyState(VK_SHIFT)<0);
                        int dlg=SendMessage(c1.hwnd,WM_GETDLGCODE,0,0);
                        if(!(dlg&DLGC_WANTTAB)){
                            c1.calldef=0;
                            HOBJ2D hgroup=GetObjectParent2d(hsp,c1.hObject);
                            if(hgroup){
m1:
                                HOBJ2D _hgroup;
                                _hgroup=GetObjectParent2d(hsp,hgroup);
                                if(_hgroup){
                                    hgroup=_hgroup;
                                    goto m1;
                                }
                            }


                            HOBJ2D next=c1.hObject;
m2:
                            if(shift)next=GetLowerObject2d(hsp,next);else
                                next=GetUpperObject2d(hsp,next);
                            if(!next){
                                if(shift)next=GetTopObject2d(hsp);else
                                    next=GetBottomObject2d(hsp);
                            }
                            if(next==c1.hObject)return FALSE;

                            if(GetObjectType2d(hsp,next)==otCONTROL2D){
                                if(!IsGroupContainObject2d(hsp,hgroup,next)){
                                    SetActiveControl(hsp,next);
                                    return FALSE;
                                }}
                            goto m2;
                        }
                    };
                }

            }break;
        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(hwnd,&ps);
            PaintSpace2d(hsp,hdc,&ps.rcPaint,0);
            EndPaint(hwnd,&ps);
        }return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK Video2dProc(HSP2D hsp,USEROBJSTRUCT*u,UINT command,DWORD lParam){
    switch(command){
        case OM_PAINT:{
            USERPAINT * up=(USERPAINT*)lParam;
            PatBlt(up->hDc,u->x,u->y,u->sizex,u->sizex,BLACKNESS );
        }break;
    }
    return 0;
}

HINSTANCE hInstance;
int GetFileName(HWND hwnd,char * s){
    OPENFILENAME ofn;
    char szDirName[256];
    char szFile[256], szFileTitle[256];
    UINT  i, cbString;
    char  chReplace;    /* string separator for szFilter */
    char  szFilter[256];

    /* Get the current directory name, and store in szDirName */

    getcurdir(0,szDirName);
    lstrcpy(szDirName, "X:\\");
    szDirName[0] = 'A' + getdisk();
    getcurdir(0, szDirName+3);
    lstrcpy(szFile,"");
    if ((cbString = LoadString(hInstance,1,
                               szFilter, sizeof(szFilter))) == 0) {
        return 0;
    }
    chReplace = szFilter[cbString - 1]; /* retrieve wildcard */


    for (i = 0; szFilter[i] != '\0'; i++) {
        if (szFilter[i] == chReplace)
            szFilter[i] = '\0';
    }
    /* Set all structure members to zero. */
    _fmemset(&ofn, 0, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szDirName;
    ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    /* Perform file operations. */
    if (GetOpenFileName(&ofn)) {
        lstrcpy(s,ofn.lpstrFile);
        return 1;
    }else{
        DWORD error=CommDlgExtendedError();
        int j=error;
    }
    return 0;}

HSP2D hsp;
HWND tools=NULL,tools2=NULL;
HOBJ2D hp,hb,hdib,hDIB,hbmp,hpl,hmove3=0,hmove=0,hfont,hfont2,hstr,htext,h3,
hrt,grp,pnum=(HOBJ2D)-1,cmove=0;
POINT2D  delta,ddelta;

static char szAppName[]="_GRAPH_DLL_Demo";
HOBJ2D view3=0;
int rcenter=0,wasmove=0;
static int nxOrg,nyOrg,cxClient,cyClient,nHScroll,nVScroll,cxChar,cyChar,
nWidth,nHeight;

void _MakeCamera(HSP2D h2,HSP3D h3){
    CAMERA3D camera;
    MakeDefaultCamera(&camera,0x20);
    camera.poBackground=RGB(128,128,128);
    //camera.poOrg.z=0;
    //camera.poOrg.y=1000;

    camera.poNear_Clip = 0;
    camera.poFar_Clip  = 3000;

    camera.poOrg.x=0;
    camera.poOrg.y=0;
    camera.poOrg.z=0;

    camera.poDirection.x=1;
    camera.poDirection.y=0;
    camera.poDirection.z=0;

    camera.poUp.y=100;
    camera.poUp.x=100;
    camera.poFlags|=CAMERA_PERSPECTIVE;
    camera.poFocus=100;

    //camera.poUp.z=100;
    camera.poRenderType|=RENDER_TEXTURED;
    lstrcpy(camera.poRenderName,"ZBuffer");
    //camera.poRenderType=(camera.poRenderType&~RENDER_SHADED);
    hcamera=CreateCamera3d(h3,&camera);

    POINT2D org,size;
    org.x=100;
    org.y=100;
    size.x=400;
    size.y=400;

    HOBJ2D hview=Create3dProjection2d(h3,hcamera, &org, &size );
    SetAttribute2d(h2,hview,2,ATTRSET);

    //TrCameraToFeet(h2,hview,0,0.5);


    hmove=GetNextObject2d(hsp,0);
};


LRESULT CALLBACK _export
WndProc( HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
    int n;
    switch (message){
        case WM_CREATE:
        {cxChar=8;cyChar=8;nWidth=2048;nHeight=2048;
        }
        case WM_SIZE:
            POINT2D Org2d;
            GetOrgSpace2d(hsp,&Org2d);
            nxOrg=Org2d.x;nyOrg=Org2d.y;
            cxClient=LOWORD(lParam);cyClient=HIWORD(lParam);
            nHScroll=max(0,nWidth-cxClient);
            nVScroll=max(0,nHeight-cyClient);
            nxOrg=min(nxOrg,nHScroll);
            nyOrg=min(nyOrg,nVScroll);
            SetScrollRange(hwnd,SB_HORZ,0,nHScroll,TRUE);
            SetScrollPos(hwnd,SB_HORZ,nxOrg,TRUE);
            SetScrollRange(hwnd,SB_VERT,0,nVScroll,TRUE);
            SetScrollPos(hwnd,SB_VERT,nyOrg,TRUE);

            return 0;
        case WM_HSCROLL:
            switch (wParam){
                case SB_TOP:n=0;break;
                case SB_BOTTOM:n=nHScroll;break;
                case SB_LINEUP:n=max(0,nxOrg-cxChar);break;
                case SB_LINEDOWN:n=min(nxOrg+cxChar,nHScroll);break;
                case SB_PAGEUP:n=max(0,nxOrg-cxClient);break;
                case SB_PAGEDOWN:n=min(nxOrg+cxClient,nHScroll);break;
                case SB_THUMBPOSITION:n=LOWORD(lParam);break;
                default: n=nxOrg;
            }if (n!=nxOrg)
            {
                ScrollWindow(hwnd,nxOrg-n,0,NULL,NULL);
                nxOrg=n;
                POINT2D Org2d;Org2d.x=nxOrg;Org2d.y=nyOrg;
                SetOrgSpace2d(hsp,&Org2d);
                SetScrollPos(hwnd,SB_HORZ,nxOrg,TRUE);
                UpdateWindow(hwnd);
            }
            return 0;
        case WM_VSCROLL:
            switch (wParam){
                case SB_TOP:n=0;break;
                case SB_BOTTOM:n=nVScroll;break;
                case SB_LINEUP:n=max(0,nyOrg-cyChar);break;
                case SB_LINEDOWN:n=min(nyOrg+cyChar,nVScroll);break;
                case SB_PAGEUP:n=max(0,nyOrg-cyClient);break;
                case SB_PAGEDOWN:n=min(nyOrg+cyClient,nVScroll);break;
                case SB_THUMBPOSITION:n=LOWORD(lParam);break;
                default: n=nyOrg;
            }if (n!=nyOrg)
            {
                ScrollWindow(hwnd,0,nyOrg-n,NULL,NULL);
                nyOrg=n;
                POINT2D Org2d;Org2d.x=nxOrg;Org2d.y=nyOrg;
                SetOrgSpace2d(hsp,&Org2d);
                SetScrollPos(hwnd,SB_VERT,nyOrg,TRUE);
                UpdateWindow(hwnd);
            }
            return 0;
        case WM_KEYDOWN:{

            if (wParam>='0' && wParam<='9'){
                CAMERA3D camera;
                GetCamera3d(hsp3d,hcamera,&camera);
                switch(wParam){
                    case '8':camera.poOrg.x+=10.0;break;
                    case '2':camera.poOrg.x-=10.0;break;
                    case '4':camera.poOrg.y+=10.0;break;
                    case '6':camera.poOrg.y-=10.0;break;
                    case '1':camera.poOrg.z+=10.0;break;
                    case '7':camera.poOrg.z-=10.0;break;
                    default:return 0;
                }
                SetCamera3d(hsp3d,hcamera,&camera);
                return 0;
            }


            ROTATOR3D r;
            memset(&r,0,sizeof(r));
            r.VctOrg.y=0;
            r.Angle=M_PI/50;

            switch(wParam){
                case VK_SPACE:
                {if (rcenter)
                    {
                        DeleteRotateCenter2d(hsp);
                        rcenter=0;
                    }
                    else
                    {
                        POINT p;GetCursorPos(&p);
                        ScreenToClient(hwnd,&p);
                        POINT2D _p;
                        PntToPnt(_p,p);
                        DPtoLP2d(hsp,&_p,1);
                        CreateRotateCenter2d(hsp,&_p);
                        rcenter=1;
                    }}return 0;
                case VK_DOWN:r.Angle=-r.Angle;
                case VK_UP: r.VctSize.x=100;break;
                case VK_RIGHT:r.Angle=-r.Angle;
                case VK_LEFT:r.VctSize.y=100;break;
                case VK_PRIOR:r.Angle=-r.Angle;
                case VK_NEXT:r.VctSize.z=100;break;
                default:goto m45;
            }
            //  r.Angle=0;
            CRDSYSTEM crd;
            crd.type=LOCAL_COORDINATE;
            crd.hObject=obj3d;
            SetCrdSystem3d(hsp3d,&crd);
            RotateObject3d(hsp3d,obj3d,&r);
            POINT3D p;
            p.x=1.10;
            p.y=1.10;
            p.z=1.10;
            if(wParam==VK_PRIOR)SetObjectSize3d(hsp3d,obj3d,&p);
m45:;
        }break;

        case WM_COMMAND:
            switch (wParam)
            {
                case CM_DRIVERINFO:{
                    if(hmove){
                        CAMERA3D camera;
                        hcamera=Get3dViewCamera3d(hsp,hmove);
                        hsp3d=Get3dSpace2d(hsp,hmove);
                        if(GetCamera3d(hsp3d,hcamera,&camera)){
                            Driver3dDialog(hwnd,camera.poRenderName);
                        }
                    }
                };break;
                case CM_INSERTBAR:{
                    COLOR3D color;
                    color.c.r=255;
                    color.c.b=255;
                    color.c.g=0;
                    // MakeBar3d(hsp3d,color,40,40,40);
                };break;
                case CM_INSERTTORE:{
                    COLOR3D color;
                    color.c.r=255;
                    color.c.b=255;
                    color.c.g=0;

                    // MakeTore3d(hsp3d,0,color,10,50,10,50);
                };break;
                case CM_GRID:{
                    DWORD flags=SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
                    SetSpaceParam2d(hsp,SP_SETSPACEFLAG,flags^SF_GRID);
                }break;
                case CM_SAVE:State2dSaveAs( hsp,"c:\\demo.vdr");return 0;
                case CM_LOAD:{
                    char s[256];
                    if (GetFileName(hwnd,s)){

#ifdef WIN32
#define TO_OPENREAD       OF_READ|OF_SHARE_DENY_WRITE
#else
#define TO_OPENREAD       READ|OF_SHARE_DENY_WRITE
#endif

                        UINT file=_lopen(s,TO_OPENREAD);
                        if (file){
                            INT16 flag=0;
                            _lread(file,&flag,2);
                            _lclose(file);
                            HSP2D hsp1=0;
                            if (flag==0x4d4d){
                                hsp1=CreateSpace2d(hwnd,0);
                                HSP3D _hsp3d=CreateSpace3d(hsp1);
                                HOBJ2D _obj3d=Import3DS(_hsp3d,s);
                                if (!_obj3d){
                                    DeleteSpace2d(hsp1);
                                    return 0;
                                }else {
                                    _MakeCamera(hsp1,_hsp3d);
                                    hsp3d=_hsp3d;
                                    hobject3d=_obj3d;
                                }
                            }else hsp1=LoadSpace2d(hwnd,s,__path,NULL);
                            if (hsp1){ DeleteSpace2d(hsp);hsp=hsp1;
                                hsp3d=GetNextSpace3d(hsp,0);
                            }
                            InvalidateRect(hwnd,0,0);
                        }
                    }
                }
                    return 0;
                case CM_CREATEMETAFILE:CreateMetaFile2d(hsp,NULL);
                    return 0;
                case CM_SAVE_METAFILE:
                    SaveMetaFile2d(hsp,"c:\\demo.mfl");return 0;
                case CM_FROMMETA:
                {DeleteSpace2d(hsp);
                    hsp=CreateFromMetaFile2d(hwnd,"c:\\l.mfl",__path);
                    return 0;
                }
                case CM_PERSPECTIVE:{
                    CAMERA3D camera;
                    GetCamera3d(hsp3d,hcamera,&camera);
                    camera.poFlags^=CAMERA_PERSPECTIVE;
                    SetCamera3d(hsp3d,hcamera,&camera);
                }break;
                case CM_DIRECT:{
                    CAMERA3D camera;
                    GetCamera3d(hsp3d,hcamera,&camera);
                    camera.poRenderType^=RENDER_NOBUFFER;
                    SetCamera3d(hsp3d,hcamera,&camera);
                }break;
                case CM_RENDER:{
                    CAMERA3D camera;
                    GetCamera3d(hsp3d,hcamera,&camera);
                    if(camera.poRenderName[0]==0){
                        lstrcpy(camera.poRenderName,"ZBuffer");
                    }else lstrcpy(camera.poRenderName,"");

                    if(camera.poRenderType==RENDER_BOUNDED)
                        camera.poRenderType=RENDER_FACETED;
                    else
                        camera.poRenderType=RENDER_BOUNDED;

                    SetCamera3d(hsp3d,hcamera,&camera);
                }break;
                case CM_ZOOMIN:
                {
                    CAMERA3D camera;
                    GetCamera3d(hsp3d,hcamera,&camera);
                    camera.poExtent.x*=(2.0/3.0);
                    camera.poExtent.y*=(2.0/3.0);

                    SetCamera3d(hsp3d,hcamera,&camera);
                }return 0;
                case CM_ZOOMOUT:
                {CAMERA3D camera;
                    GetCamera3d(hsp3d,hcamera,&camera);
                    camera.poExtent.x*=1.5;
                    camera.poExtent.y*=1.5;

                    SetCamera3d(hsp3d,hcamera,&camera);
                }return 0;
                case CM_ACTUALSIZE:{
                    POINT3D org;
                    GetObjectBase3d(hsp3d,obj3d,&org);
                    org.x+=5;
                    SetObjectOrg3d(hsp3d,obj3d,&org);
                    SetCurrentObject2d((HSP2D)hsp3d,obj3d);
                    POINT2D ppp;
                    ppp.x=1;ppp.y=1;
                    CreateEditFrame3d(hsp,&ppp);
                    /* POINT2D p1,p2;p1.x=100;p1.y=100;p2.x=100;p2.y=100;
  SetScaleSpace2d(hsp,&p1,&p2);return 0;
  */
                    break;
                }
                case CM_V25:{
                    POINT3D aa[10];

                    for(int i=0;i<10;i++){
                        double a=((double)i/10.0)*M_PI*2;
                        aa[i].x=20+50*cos(a);
                        aa[i].y=i*10;
                        aa[i].z=20+50*sin(a);
                    }
                    INT16 start=GetNumPoints3d(hsp3d,obj3d);
                    AddPoints3d(hsp3d,obj3d,aa,10);
                    //AddPoints3d(hsp3d,obj3d,aa,10);
                    PRIMITIVE3D p;
                    memset(&p,0,sizeof(p));
                    p.count=10;
                    INT16 npoints[10];
                    p.flags=PRIMITIVE_POLYGON;
                    p.color.c.r=255;
                    p.color.c.g=255;
                    p.color.c.b=155;
                    p.points=npoints;
                    for( i=0;i<10;i++)npoints[i]=start+i;
                    CreatePrimitives3d(hsp3d,obj3d,&p,1);
                    break;
                }
                case CM_V50:{
                    POINT3D p[20];
                    GetPoints3d(hsp3d,obj3d,p,0,20);
                    for(INT16 i=0;i<20;i++){
                        p[i].x*=0.76;
                        p[i].y*=0.8;
                    }
                    SetPoints3d(hsp3d,obj3d,p,0,20);
                    break;
                }
                case CM_V200:{
                    POINT2D p1,p2;p1.x=200;p1.y=200;p2.x=100;p2.y=100;
                    SetScaleSpace2d(hsp,&p1,&p2);return 0;
                }
                case CM_V400:{
                    POINT2D p1,p2;p1.x=400;p1.y=400;p2.x=100;p2.y=100;
                    SetScaleSpace2d(hsp,&p1,&p2);return 0;
                }
                case CM_EDITUNDO:
                {PlayMetaRecord(hsp,-1);return 0;
                }
                case CM_EDITREDO:
                {PlayMetaRecord(hsp,1);return 0;
                }
                case CM_TOOLS:
                    if (tools){DestroyWindow(tools);tools=NULL;}else
                    {/*tools = CreateWindow(tools_name,
                    "Tools",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|
                    WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,
                    400,250,400,200,hwnd,NULL,
                    hInstance,NULL);

     ShowWindow(tools,SW_SHOW);
     */
                    }
                    return 0;
                case CM_TOOLS2:
                    /*
    if (tools2){DestroyWindow(tools2);tools2=NULL;}else
    {tools2 = CreateWindow(tools_name,
                    "Tools",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|
                    WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,
                    300,150,400,200,hwnd,NULL,
                    (HINSTANCE)hInstance,NULL);
     ShowWindow(tools2,SW_SHOW);
    }*/
                    return 0;
                case CM_Z_ORDERTO_TOP:ObjectToTop2d(hsp,GetCurrentObject2d(hsp));return 0;
                case CM_ZBOTTOM:ObjectToBottom2d(hsp,GetCurrentObject2d(hsp));return 0;
                case CM_Z1:
                {HOBJ2D obj;obj=GetCurrentObject2d(hsp);
                    if (obj)
                    {
                        SetZOrder2d(hsp,obj,GetZOrder2d(hsp,obj)+1);
                    }
                    return 0;
                }
                case CM_Z2:
                {HOBJ2D obj;obj=GetCurrentObject2d(hsp);
                    if (obj)
                    {
                        SetZOrder2d(hsp,obj,GetZOrder2d(hsp,obj)-1);
                    }
                    return 0;
                }
                case CM_EDITCOPY:
                {HOBJ2D curobj;
                    curobj=GetCurrentObject2d(hsp);
                    if (curobj) CopyToClipboard2d(hsp,curobj); return 0;
                }
                case CM_EDITPASTE:
                {POINT _p;
                    GetCursorPos(&_p);
                    ScreenToClient(hwnd,&_p);
                    POINT2D p;
                    PntToPnt(p,_p);
                    DPtoLP2d(hsp,&p,1);
                    PasteFromClipboard2d(hsp,&p,PFC_SETCURRENT|PFC_SETFRAME|
                                         PFC_MOVEOBJECT|PFC_ALL);
                    return 0;
                }
                case CM_EDITDELETE:
                {HOBJ2D curobj;
                    curobj=GetCurrentObject2d(hsp);
                    if (curobj) DeleteObject2d(hsp,curobj);
                    return 0;
                }
            }
            break;
        case WM_DESTROY:
            DeleteSpace2d(hsp);
            PostQuitMessage(0);
            return 0;

            // 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d 2d 3d
        case WM_LBUTTONDBLCLK:{
            POINT2D pp;
            pp.x=LOWORD(lParam);pp.y=HIWORD(lParam);
            DPtoLP2d(hsp,&pp,1);
            HOBJ2D h=GetObjectFromPoint2d(hsp,&pp);
            if (h)
                if (GetObjectType2d(hsp,h)==24){
                    SetCurrentObject2d(hsp,h);
                    DeleteEditFrame2d(hsp);
                    view3=h;
                    POINT3D so;
                    so.x=0.0;so.y=0.0;so.z=0.0;
                    CreateRotateAxis3d(hsp,&so,50);
                    hsp3d=Get3dSpace2d(hsp,view3);
                }
            return 0;
        }
        case WM_LBUTTONDOWN:
            POINT2D pp;
            GetPOINT2D(hsp ,&pp,lParam);
            hmove=GetObjectFromPoint2d(hsp,&pp);
            if (hmove)
            {
                GetObjectOrgSize2d(hsp,hmove,&delta,NULL);
                delta.x=pp.x-delta.x;delta.y=pp.y-delta.y;
                ddelta=pp;wasmove=0;
                if (NotReserved(hmove)){
                    SetCurrentObject2d(hsp,hmove);
                    POINT2D _1;
                    _1.x=8;
                    _1.y=8;
                    CreateEditFrame2d(hsp,&_1);

                    switch (GetObjectType2d(hsp,hmove)){
                        case otVIEW3D2D:{
                            static old3d=0;
                            HOBJ2D prim;
                            INT16 item;
                            hcamera=Get3dViewCamera3d(hsp,hmove);
                            hsp3d=Get3dSpace2d(hsp,hmove);
                            obj3d=GetObject3dFromPoint2d(hsp,hmove,&pp,&prim,&item);

                            if (old3d)SetAttribute3d(hsp3d,old3d,STATE_AXIS3D|STATE_BOUNDS,ATTRRESET);
                            old3d=0;
                            if (obj3d){

                                //SetObjectBase3d(hsp3d,obj3d,&_org3d);
                                state=sfdown3d;
                                SetCapture(hwnd);
                                lastpoint=pp;

                                obj3d=prim;
                                old3d=obj3d;
                                //  obj3d=prim;
                                SetCurrentObject2d((HSP2D)hsp3d,obj3d);

                                SetAttribute3d(hsp3d,obj3d,STATE_AXIS3D|STATE_BOUNDS,ATTRSET);

                                PRIMITIVE3D p;
                                INT16 pnums[10];
                                if(GetPrimitives3d(hsp3d,prim,&p,item,1,pnums,10,NULL,NULL))
                                {
                                    p.color.rgb=RGB(255,0,0);
                                    if(wParam&MK_CONTROL){
                                        SetPrimitives3di(hsp3d,prim,&p,&item,1);
                                    }
                                }
                                //   old3d=prim;
                                POINT2D ppp;
                                ppp.x=1;ppp.y=1;
                                //	CreateEditFrame3d(hsp,&ppp);
                            }
                        };break;
                    }
                }
            }else
            {if (GetCurrentObject2d(hsp)>0)
                    SetCurrentObject2d(hsp,0);
                DeleteEditFrame2d(hsp);
            }
            return 0;
        case WM_MOUSEMOVE:
            pp.x=LOWORD(lParam);pp.y=HIWORD(lParam);
            DPtoLP2d(hsp,&pp,1);
            switch (state){

                case sfdown3d:{
                    /*
        GetObjectOrgSize3d(hsp3d,obj3d,&org3d,&size3d);
        base.x=org3d.x+size3d.x/2.0;
        base.y=org3d.y+size3d.y/2.0;
        base.z=org3d.z+size3d.z/2.0;
        */
                    CRDSYSTEM crd;
                    crd.type=WORLD_COORDINATE;
                    crd.hObject=0;//obj3d;
                    SetCrdSystem3d(hsp3d,&crd);

                    GetObjectBase3d(hsp3d,obj3d,&base);

                    POINT2D _org2d;
                    LP3dtoLP2d(hsp,hmove,&_org2d,&base,1);

                    ddelta.x=_org2d.x-lastpoint.x;
                    ddelta.y=_org2d.y-lastpoint.y;
                    state=sfmove3d;
                };
                case sfmove3d:{

                    POINT3D _org3d;
                    int _mode=2;
                    HOBJ2D object2d=hmove;
                    HOBJ2D object3d=object3d;

                    // 0 x-y
                    // 1 y-z
                    // 2 z-x

                    POINT2D _p;
                    _p.x=pp.x+ddelta.x;
                    _p.y=pp.y+ddelta.y;
                    // _p - новые координаты центра объекта в координатах пространства
                    PushCrdSystem3d(hsp3d);

                    CRDSYSTEM crd;
                    crd.type=LOCAL_COORDINATE;
                    crd.hObject=obj3d;
                    SetCrdSystem3d(hsp3d,&crd);
                    POINT3D _b,_b_;

                    if(GetObjectBase3d(hsp3d,object3d,&_b)){
                        _b_=_b;
                        TransformPoint3d(hsp,object2d,&_b_,1,1);
                        // Перевод базы в систему координат проекции

                        POINT2D po;
                        LP2dtoView(hsp,object2d,&_p,&po,1);
                        POINT3D _p2d;
                        _p2d.x=po.x;
                        _p2d.y=po.y;
                        _p2d.z=_b_.z;

                        POINT3D points[3],_points[3];
                        for(int i=0;i<3;i++){
                            _points[i]=_b;
                            switch(_mode){
                                case 0:
                                    if(i==1)_points[i].x+=100;
                                    if(i==2)_points[i].y+=100;
                                    break;
                                case 1:
                                    if(i==1)_points[i].z+=100;
                                    if(i==2)_points[i].y+=100;
                                    break;
                                case 2:
                                    if(i==1)_points[i].x+=100;
                                    if(i==2)_points[i].z+=100;
                                    break;
                            }

                        }
                        if(!LP3dtoLP3d(hsp3d,_points,points,3,LP3D_CURRENT,LP3D_VIEW,object2d))return 0;
                        double a,b,c,d;
                        c= points[0].x*(points[1].y - points[2].y)
                                -points[1].x*(points[0].y - points[2].y)
                                +points[2].x*(points[0].y - points[1].y);

                        d=-(points[0].x*(points[1].y*points[2].z  -  points[2].y*points[1].z)-
                                points[1].x*(points[0].y*points[2].z  -  points[2].y*points[0].z)+
                                points[2].x*(points[0].y*points[1].z  -  points[1].y*points[0].z));

                        a=  points[0].y*(points[1].z-points[2].z)
                                -points[1].y*(points[0].z-points[2].z)
                                +points[2].y*(points[0].z-points[1].z);
                        b=-(points[0].x*(points[1].z-points[2].z)
                                -points[1].x*(points[0].z-points[2].z)
                                +points[2].x*(points[0].z-points[1].z));

                        double r=sqrt(a*a+b*b+c*c);
                        if (c<0){r=-r;}
                        if (r!=0){
                            a=a/r;b=b/r;c=c/r;d=d/r;
                            // 0 x-y
                            // 1 y-z
                            // 2 z-x

                            switch(0){
                                case 0:if(fabs(c)>0) _p2d.z=-(d+a*_p2d.x+b*_p2d.y)/c;break;
                                case 1:if(fabs(a)>0) _p2d.x=-(d+c*_p2d.z+b*_p2d.y)/a;break;
                                case 2:if(fabs(b)>0) _p2d.y=-(d+a*_p2d.x+c*_p2d.z)/b;break;

                            }
                            TransformPoint3d(hsp,object2d,&_p2d,1,0);
                            // Перевод обратно в мировую систему координат
                            POINT3D _pcd; //
                            LP3dtoLP3d(hsp3d,&_p2d,&_pcd,1,LP3D_WORLD,LP3D_CURRENT,0);
                            // Перевод в текущую систему координат
                            SetObjectBase3d(hsp3d,object3d,&_pcd);

                        }}
                    PopCrdSystem3d(hsp3d);
                }break;

                default:
                    if((wParam&MK_LBUTTON)&&(hmove))
                    {
                        if (!wasmove)
                        {if ((abs(ddelta.x-pp.x)>1) || (abs(ddelta.y-pp.y)>1))
                            {if (NotReserved(hmove))DeleteEditFrame2d(hsp);
                                SetCapture(hwnd);
                                BeginDragDrop2d(hsp,hmove);
                                wasmove=1;
                            }else return 0;
                        }
                        pp.x-=delta.x;pp.y-=delta.y;
                        SetObjectOrg2d( hsp,hmove,&pp);
                    }else
                    {
                        HOBJ2D c;
                        c=GetFrameCursor2d(hsp,&pp);
                        HCURSOR cur;
                        switch ((INT16)c)
                        {
                            case 0:cur=LoadCursor(NULL, IDC_ARROW);break;
                                /*
case ((INT16)OID_RCENTER):cur=LoadCursor(NULL, IDC_SIZE);break;
case (INT16)OID_FRAME2d1:cur=LoadCursor(NULL, IDC_SIZENWSE);break;
case (INT16)OID_FRAME2d2:cur=LoadCursor(NULL, IDC_SIZENESW);break;
case (INT16)OID_FRAME2d3:cur=LoadCursor(NULL, IDC_SIZENWSE);break;
case (INT16)OID_FRAME2d4:cur=LoadCursor(NULL, IDC_SIZENESW);break;

case (INT16)OID_FRAME2d5:cur=LoadCursor(NULL, IDC_SIZENS);break;
case (INT16)OID_FRAME2d6:cur=LoadCursor(NULL, IDC_SIZEWE);break;
case (INT16)OID_FRAME2d7:cur=LoadCursor(NULL, IDC_SIZENS);break;
case (INT16)OID_FRAME2d8:cur=LoadCursor(NULL, IDC_SIZEWE);break;
*/
                            default:cur=LoadCursor(NULL, IDC_SIZE);break;
                        }
                        SetCursor(cur);
                    }}
            return 0;
        case WM_LBUTTONUP:
            POINT2D _1;_1.x=8;	_1.y=8;
            switch(state){
                case sfmove3d:
                case sfdown3d:ReleaseCapture();
                    state=0;
                default:
                    if (wasmove){
                        if (NotReserved(hmove)){
                            CreateEditFrame2d(hsp,&_1);
                            EndDragDrop2d(hsp,hmove);
                        }else
                            if (IsReserved(hmove)) UpdateFromFrame2d(hsp);
                        ReleaseCapture();
                        hmove=0;
                    }
                    state=0;
            }
            return 0;
    }
    //--------------------

    if(DefSpaceWindowProc(hsp, hwnd,message,wParam,lParam)) return DefWindowProc(hwnd,message,wParam,lParam);
    return 0;
}

int WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR,int nCmdShow)
{
    ::hInstance=hInstance;

    HWND hwnd;
    MSG msg;
    if (!hPrevInstance){
        WNDCLASS wndclass;
        wndclass.style         = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
        wndclass.lpfnWndProc   = WndProc;
        wndclass.cbClsExtra    = 0;
        wndclass.cbWndExtra    = 0;
        wndclass.hInstance     = hInstance;
        wndclass.hIcon         = LoadIcon(hInstance,MAKEINTRESOURCE(APP_ICON));
        wndclass.hCursor       = NULL;
        wndclass.hbrBackground = NULL;
        wndclass.lpszMenuName  = MAKEINTRESOURCE(MENU_1);
        wndclass.lpszClassName = szAppName;
        RegisterClass(&wndclass);
    }
    hwnd = CreateWindow( szAppName,
                         "Graphic Demo V 0.01",
                         WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL|WS_CLIPCHILDREN,
                         100,0,
                         //            CW_USEDEFAULT,
                         //            CW_USEDEFAULT,
                         640,
                         480,
                         NULL,
                         NULL,
                         hInstance,
                         NULL);

    hsp=LoadSpace2d(hwnd,"d:\\temp\\converters\\Obj1_CutVertexs.vdr",__path,"d:\\temp\\converters");
    goto loop;

    if(0){
        hsp=LoadSpace2d(hwnd,"face.vdr",__path,NULL);
        HSP3D hsp3d=GetNextSpace3d(hsp,0);
        HOBJ2D obj=2;//GetNextObject2d((HSP2D)hsp3d,0);
        POINT3D Origin;
        POINT3D Size;
        GetObjectOrgSize3d(hsp3d,obj, &Origin ,&Size);
        INT16 np=GetNumPrimitives3d(hsp3d,obj);
        PRIMITIVE3D p;
        INT16 pnums[10];
        POINT2D txt[10];
        POINT3D  points[4];
        MATERIAL3D m;
        m.poSize=sizeof(m);
        m.flags=TF_FILE;
        m.color.rgb=RGB(255,255,255);
        m.hdib=0;
        m.hfile="alex.bmp";//"pat.bmp";
        m.name="Mylene Famer";
        HOBJ2D hm=CreateMaterial3d(hsp3d,&m);

        for(INT16 i=0;i<np;i++){
            GetPrimitives3d(hsp3d,obj,&p,i,1,pnums,10,txt,10);
            GetPoints3di( hsp3d,obj,points,pnums,p.count);
            p.material=hm;
            p.flags|=PRIMITIVE_TEXTURED;
            double k=1;
            for(int j=0;j<p.count;j++){
                txt[j].x=k*(points[j].x-Origin.x)/Size.x;
                txt[j].y=k*(points[j].y-Origin.y)/Size.y;
            }
            p.texturePOINTS=txt;
            SetPrimitives3di(hsp3d,obj,&p,&i,1);
        }
        goto loop;
    }



    //POINT2D org;
    //GetOrgSpace2d(hsp,&org);
    //SetBkBrush2d(hsp,0);
    //long size=0;
    //HGLOBAL hg=SaveToMemory2d(hsp,&size);

    //goto loop;
    {

        HSP2D hsp1=CreateSpace2d(hwnd,__path);
        hsp=CreateSpace2d(hwnd,__path);


        POINT2D porg;
        porg.x=20;
        porg.y=0;
        SetOrgSpace2d(hsp,&porg);

        hsp3d=CreateSpace3d(hsp);
        /*
if (_argc>0 ){
obj3d=Import3DS(hsp3d,_argv[1]);
HOBJ2D lips=GetObjectByName3d(hsp3d,0,"2dblk01");
COLOR3D c;
c.rgb=RGB(255,0,0);
SetObjectColor3d(hsp3d,lips,&c);
};
*/

        if (FALSE){
            COLOR3D color;
            color.c.r=255;
            color.c.b=255;
            color.c.g=0;
            // MakeTore3d(hsp3d,color,10,50,10,50);
            // MakeCube3d(hsp3d,color,40);
            // MakeSphere3d(hsp3d,color,50,16,32);
            //MakeCylinder3d(hsp3d,0,color,100,100,100,10,1);
        };


        if (TRUE){
            MATERIAL3D m;
            m.poSize=sizeof(m);
            m.flags=TF_FILE;
            m.color.rgb=RGB(255,255,255);
            m.hdib=0;
            m.hfile="CEDFENCE.bmp";//"pat.bmp";
            m.name="Mylene Famer";
            HOBJ2D hm=CreateMaterial3d(hsp3d,&m);

            m.hfile="alex.bmp";//"pat.bmp";
            m.name="Alexander Shelemekhov";
            m.flags|=TF_CONSTCOLOR;

            HOBJ2D hm1=CreateMaterial3d(hsp3d,&m);
            // Квадрат(Куб)
            COLOR3D color;
            color.c.r=255;
            color.c.b=255;
            color.c.g=0;

            HOBJ2D obj=CreateObject3d(hsp3d);
            double sizey=80;
            double sizex=80;
            double sizez=20;



            POINT3D aa[8];



            sizey/=2.0;
            sizex/=2.0;
            sizez/=2.0;

            aa[0].y=-sizey;aa[0].x=-sizex;
            aa[1].y=-sizey;aa[1].x=sizex;
            aa[2].y=sizey; aa[2].x=sizex;
            aa[3].y=sizey; aa[3].x=-sizex;

            aa[4].y=-sizey;aa[4].x=-sizex;
            aa[5].y=-sizey;aa[5].x=sizex;
            aa[6].y=sizey; aa[6].x=sizex;
            aa[7].y=sizey; aa[7].x=-sizex;

            aa[0].z=aa[1].z=aa[2].z=aa[3].z=sizez;
            aa[4].z=aa[5].z=aa[6].z=aa[7].z=-sizez;

            AddPoints3d(hsp3d,obj,aa,8);
            PRIMITIVE3D p[6];

            memset(&p,0,sizeof(p));

            INT16 npoints[24]={
                0,1,2,3,
                1,5,6,2,
                5,4,7,6,
                4,0,3,7,
                3,2,6,7,
                4,5,1,0
            };

            POINT2D tp[4*6];
            for(int i=0;i<6;i++){
                p[i].flags=PRIMITIVE_POLYGON;//|PRIMITIVE_ALWAYS;
                p[i].color=color;
                p[i].points=npoints+i*4;
                p[i].count=4;
                p[i].material=hm;
                tp[i*4].x=0;tp[i*4].y=0;
                tp[i*4+1].x=2;tp[i*4+1].y=0;
                tp[i*4+2].x=2;tp[i*4+2].y=2;
                tp[i*4+3].x=0;tp[i*4+3].y=2;
                p[i].texturePOINTS=tp+i*4;
                p[i].flags|=PRIMITIVE_TEXTURED|PRIMITIVE_ALWAYS;
                if(i==2){
                    p[i].material=hm1;
                }
            };

            CreatePrimitives3d(hsp3d,obj,p,6);

            // return obj;


            // HOBJ2D cube1= MakeBar3d(hsp3d,color,40,40,40);
            // HOBJ2D cube1=MakeCube3d(hsp3d,color,40);
            // SetAttribute3d(hsp3d,cube1,STATE_NOSHADED,ATTRSET);
            // HOBJ2D cube2=MakeCube3d(hsp3d,40);
            // POINT3D org;org.x=org.y=org.z=0;
            // SetObjectOrg3d(hsp3d,cube2,&org);
            // obj3d=cube1;
        }
        _MakeCamera(hsp,hsp3d);

    }

loop:

    POINT2D pp[2];
    pp[0].x=100;pp[0].y=10;
    pp[1].x=100;pp[1].y=100;
    CreatePolyline2d(hsp,
                     CreatePen2d(hsp,PS_SOLID,0 ,RGB(255,0,0), R2_COPYPEN)
                     ,0 ,pp,2);
    if(0){long time_start=GetTickCount();

        ROTATOR3D r;
        memset(&r,0,sizeof(r));
        r.VctOrg.y=1.0;
        r.Angle=M_PI/50;
        for(int i=0;i<1000;i++)
            RotateObject3d(hsp3d,obj3d,&r);

        long time_end=GetTickCount();
        time_end-=time_start;
        char s[256];
        wsprintf(s," Итого :%ld ",time_end);
        MessageBox(hwnd,s,"Test",MB_OK);
    }
    /*
 CRDSYSTEM2D c;
 c.type=XRYU_COORDINATE2D;
 c.center.x=10.0;
 c.center.y=20.0;

 SetCrdSystem2d(hsp,&c);*/

    HACCEL akeys=LoadAccelerators(hInstance,MAKEINTRESOURCE(MENU_1));
    ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);


    {/*
CreateControlObject2d(hsp,
                  "EDIT",	// address of registered class name
                  "test",	// address of window text
                  WS_VISIBLE|WS_CHILD|WS_BORDER,	// window style
                  100,	// horizontal position of window
                  100,	// vertical position of window
                  200,	// window width
                  20,	// window height
                  1, 0);*/
        SetSpaceParam2d(hsp,SP_SETSPACEFLAG,0);
    }
    while (GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        TranslateAccelerator(hwnd,akeys,&msg);
        DispatchMessage(&msg);
    }
    // DoneGraphics2d();
    // FreeLibrary(hinstTool);

    return msg.wParam;
}

