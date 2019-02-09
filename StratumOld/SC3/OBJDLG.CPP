#define FILE_ITERATOR
#include "_dialogs.h"
#include "winman.h"
#include <string.h>
#include "dbengine.h"
#include "dllman.h"
#include <stratum\sc_msg.h>
#include "password.h"
//#define NOUSERICHEDIT // запрет использования RTF

#ifndef NOUSERICHEDIT
#ifdef WIN32
#define MACPORT
#include <richedit.h>
#undef MACPORT
#endif
#endif

char* _plugins =
        #ifdef WIN32
        "PLUGINS32"
        #else
        "PLUGINS"
        #endif;
        #ifdef CF_RTF
        #undef CF_RTF
        #endif


        #ifdef PROTECT
        BOOL Register();
#endif

#ifndef RUNTIME
extern UINT CF_2D;
extern UINT CF_RTF;

BOOL SPasteProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    _STDHELP(0);
    switch(msg){
        case WM_INITDIALOG:{
            SetWindowLong(hwnd,DWL_USER,lParam);
            PASTESTRUCT*ps=(PASTESTRUCT*)lParam;
            SetDlgItemDouble(hwnd,IDC_PASTEX,ps->point.x);
            SetDlgItemDouble(hwnd,IDC_PASTEY,ps->point.y);
            EditSubClass(hwnd,IDC_PASTEX,0,0,1,UDF_DOUBLE);
            EditSubClass(hwnd,IDC_PASTEY,0,0,1,UDF_DOUBLE);

            if(OpenClipboard(hwnd)){
                HWND list=GetDlgItem(hwnd,IDC_PASTELIST);
                UINT cf=0;
                int index=0;
                while ((cf=EnumClipboardFormats(cf))!=0){
                    UINT f[]={CF_DIB,CF_TEXT,CF_METAFILEPICT,CFormat_OBJ,CF_2D,CF_RTF,CFormat_CLASS};
                    for(int i=0;i<sizeof(f)/sizeof(f[0]);i++)
                    {
                        if(cf==f[i]){
                            char s[256];
                            LoadString(HResource,IDS_CLIPNAMES+i,s,sizeof(s));
                            int j=(int)SendMessage(list,LB_ADDSTRING,0,(LPARAM)s);
                            SendMessage(list,LB_SETITEMDATA,j,cf);
                            if(cf==CFormat_OBJ)index=j;
                            break;
                        }
                    }
                };
                SendMessage(list,LB_SETCURSEL,index,0);
                CloseClipboard();
            }

            CheckDlgButton(hwnd,IDC_PASTEFONT,   (ps->flags&PFC_FONTS)!=0);
            CheckDlgButton(hwnd,IDC_PASTETEXT,   (ps->flags&PFC_TEXTS)!=0);
            CheckDlgButton(hwnd,IDC_PASTESTRING, (ps->flags&PFC_STRINGS)!=0);
            CheckDlgButton(hwnd,IDC_PASTEDDIB,   (ps->flags&PFC_DDIBS)!=0);
            CheckDlgButton(hwnd,IDC_PASTEDIB	  ,(ps->flags&PFC_DIBS)!=0);
            CheckDlgButton(hwnd,IDC_PASTEPEN	  ,(ps->flags&PFC_PENS)!=0);
            CheckDlgButton(hwnd,IDC_PASTEBRUSH  ,(ps->flags&PFC_BRUHS)!=0);
            CheckDlgButton(hwnd,IDC_PASTE3D  ,(ps->flags&PFC_3D)!=0);



        }return TRUE;
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_PASTELIST:if(wNotifyCode!=LBN_DBLCLK)return TRUE;
                case IDOK:{
                    PASTESTRUCT*ps=(PASTESTRUCT*)GetWindowLong(hwnd,DWL_USER);

                    if(IsDoubleValid(hwnd,IDC_PASTEX,ps->point.x,-32768.0,32768.0,TRUE)&&
                       IsDoubleValid(hwnd,IDC_PASTEY,ps->point.y,-32768.0,32768.0,TRUE)){

                        HWND list=GetDlgItem(hwnd,IDC_PASTELIST);
                        ps->cformat=SendMessage(list,LB_GETITEMDATA,SendMessage(list,LB_GETCURSEL,0,0),0);
                        ps->flags&=~PFC_ALL;

                        if(IsDlgButtonChecked(hwnd,IDC_PASTEFONT))   ps->flags|=PFC_FONTS;
                        if(IsDlgButtonChecked(hwnd,IDC_PASTETEXT))   ps->flags|=PFC_TEXTS;
                        if(IsDlgButtonChecked(hwnd,IDC_PASTESTRING)) ps->flags|=PFC_STRINGS;
                        if(IsDlgButtonChecked(hwnd,IDC_PASTEDDIB))   ps->flags|=PFC_DDIBS;
                        if(IsDlgButtonChecked(hwnd,IDC_PASTEDIB	))   ps->flags|=PFC_DIBS;
                        if(IsDlgButtonChecked(hwnd,IDC_PASTEPEN	))   ps->flags|=PFC_PENS;
                        if(IsDlgButtonChecked(hwnd,IDC_PASTEBRUSH	))   ps->flags|=PFC_BRUHS;
                        if(IsDlgButtonChecked(hwnd,IDC_PASTE3D	))   ps->flags|=PFC_3D;
                        EndDialog(hwnd,1);
                    }
                }break;
                case IDCANCEL:EndDialog(hwnd,0);break;

            }
        }break;;
    }
    return FALSE;
}
BOOL PasteDialog(HWND hwnd,PASTESTRUCT &ps)
{
    return DialogBoxParam(HResource,MAKEINTRESOURCE(PASTE_DIALOG),hwnd,SPasteProc,(LPARAM)&ps);
}
#endif
struct RTD3D{
    HSP3D  hsp3d;
    HOBJ2D object3d;
};

BOOL MR3DProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    _STDHELP(0)
            switch(msg)
    {
        case WM_INITDIALOG:
        {
            SetWindowLong(hwnd,DWL_USER,lParam);
            //RTD3D&r=*(RTD3D*)lParam;
            //SetDlgItemDouble(hwnd,IDC_txtXTILE,r);
        }return TRUE;
        case WM_COMMAND:
        {
            int i=0;
            EV_COMMAND
                    switch(command)
            {
        case IDC_RZ:i++;
                case IDC_RY:i++;
                case IDC_RX:
                {
                    double r[3]={0,0,0};
                    r[i]=1;
                    SetDlgItemDouble(hwnd,IDC_UPX,r[0]);
                    SetDlgItemDouble(hwnd,IDC_UPY,r[1]);
                    SetDlgItemDouble(hwnd,IDC_UPZ,r[2]);
                };return TRUE;
                case IDC_UPX:
                case IDC_UPY:
                case IDC_UPZ:
                {
                    if(wNotifyCode==EN_UPDATE)
                    {
                        double r[3]={0,0,0};
                        if(IsDoubleValid(hwnd,IDC_UPX,r[0],-32768.0,32768.0,FALSE)&&
                           IsDoubleValid(hwnd,IDC_UPY,r[1],-32768.0,32768.0,FALSE)&&
                           IsDoubleValid(hwnd,IDC_UPZ,r[2],-32768.0,32768.0,FALSE))
                        {
                            if(r[0])	i++;
                            if(r[1])	i++;
                            if(r[2])	i++;
                            if(i>1)  	{r[0]=r[1]=r[2]=0;}
                            CheckDlgButton(hwnd,IDC_RX,r[0]!=0);
                            CheckDlgButton(hwnd,IDC_RY,r[1]!=0);
                            CheckDlgButton(hwnd,IDC_RZ,r[2]!=0);
                        }
                    }return TRUE;
                }
                case IDOK:
                {
                    ROTATOR3D r;
                    if (IsDoubleValid(hwnd,IDC_UPX,r.VctSize.x,-32768.0,32768.0)&&
                        IsDoubleValid(hwnd,IDC_UPY,r.VctSize.y,-32768.0,32768.0)&&
                        IsDoubleValid(hwnd,IDC_UPZ,r.VctSize.z,-32768.0,32768.0)&&

                        IsDoubleValid(hwnd,IDC_ORGXc,r.VctOrg.x,-32768.0,32768.0)&&
                        IsDoubleValid(hwnd,IDC_ORGYc,r.VctOrg.y,-32768.0,32768.0)&&
                        IsDoubleValid(hwnd,IDC_ORGZc,r.VctOrg.z,-32768.0,32768.0)&&
                        IsDoubleValid(hwnd,IDC_ANGLE,r.Angle,-360.0,360.0))
                    {
                        r.Angle=M_PI*r.Angle/180;
                        RTD3D&_r=*(RTD3D*)GetWindowLong(hwnd,DWL_USER);;
                        RotateObject3d(_r.hsp3d,_r.object3d,&r);
                        //EndDialog(hwnd,1);
                    }
                }return TRUE;
                case IDCANCEL:
                {
                    EndDialog(hwnd,0);
                    return TRUE;
                };
            }
        }break;
    }
    return FALSE;
};

BOOL ManualRotateDialog3d(HWND hwnd,HSP3D hsp3d,HOBJ2D ho){
    RTD3D r;
    r.hsp3d = hsp3d;
    r.object3d=ho;
    return DialogBoxParam(HResource,MAKEINTRESOURCE(ROTATE3D_DIALOG),hwnd,MR3DProc,(LPARAM)&r);
};

void SendCommand(HWND hwnd,UINT cmd){
#ifdef WIN32
    SendMessage(hwnd,WM_COMMAND, MAKEWPARAM((UINT)(cmd),0),0);
#else
    SendMessage(hwnd,WM_COMMAND,cmd,0);
#endif
};

BOOL GetMaterialText3d(HSP3D hsp3d,HOBJ2D tool,char*ps,BOOL adduse);

void UpdateTxtCombo(HWND hwnd,int id,HSP3D hsp3d,HOBJ2D current){
    HWND list=GetDlgItem(hwnd,id);
    SendMessage(list, CB_RESETCONTENT,0,0);
    int i=0;
    HOBJ2D tool;
    while ((tool = GetNextTool2d((HSP2D)hsp3d,TEXTURE3D,tool))!=0){
        char s[256];
        GetMaterialText3d(hsp3d,tool,s,FALSE);
        SendMessage(list,CB_ADDSTRING,0,(LONG)s);
        if(current==tool)SendMessage(list,CB_SETCURSEL,i,0);
        i++;
    }
}

BOOL scCALLBACK VertToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            double r;
            if(SCGetScVarD("vertex_area",r)){
                SetDlgItemDouble(hwnd,IDC_txtXTILE,r);
                char s[64];
                BOOL _r=r<0;
                sprintf(s,_r?"%g%%":"%g",fabs(r));
                SetDlgItemText(hwnd,IDC_vertNEAR,s);
            }
        }return TRUE;
        case WM_SETTOOLVAR:{
            switch(wParam){
                case 10:{
                    char s[256];
                    wsprintf(s,"%d",(int)lParam);
                    SetDlgItemText(hwnd,IDC_VERTEXNUM,s);
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    if(b){  b->data = lParam;      }
                }break;
                case 11:{
                    POINT3D *p=(POINT3D *)lParam;
                    SetDlgItemDouble(hwnd,IDC_X,p->x);
                    SetDlgItemDouble(hwnd,IDC_Y,p->y);
                    SetDlgItemDouble(hwnd,IDC_Z,p->z);
                }break;
            }
        }return TRUE;
        case PSP_ACTIVATE:SendCommand(mainwindow->HWindow,CM_EDITVERTEXES3D);return TRUE;
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_vertNEAR:{
                    char text[64];
                    GetWindowText(hwndCtl,text,sizeof(text));
                    if(lstrlen(text)){
                        BOOL _p=FALSE;
                        char *_ps;
                        if((_ps=strstr(text,"%"))!=NULL) {_p=TRUE;*_ps=0;   }
                        double value;
                        if(StrToDouble(text,value)){
                            if ((_p && value>0 && value <=200)||(!_p)){
                                if(_p)value=-value;
                                SCSetScVarD("vertex_area",value);
                            }
                        }}else{
                        SCDeleteScVar("vertex_area");
                    }
                }break;
                case IDC_APPLY:{
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    if(b){
                        POINT3D p;
                        if(IsDoubleValid(hwnd,IDC_X,p.x,-32768.0,32768.0)&&
                           IsDoubleValid(hwnd,IDC_Y,p.y,-32768.0,32768.0)&&
                           IsDoubleValid(hwnd,IDC_Z,p.z,-32768.0,32768.0)){
                            SetPoints3d((HSP3D)b->hsp,(HOBJ2D)b->iuser1,&p,(INT16)b->data,1);
                        }
                    }
                }break;

            }}break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

void UpdatePrimitive(HWND hwnd,HSP3D hsp3d,HOBJ2D ho,INT16 item,BOOL get){
    PRIMITIVE3D p;
    INT16 pnums[MAX_IN_PRIMITIVE];
    POINT2D txt[MAX_IN_PRIMITIVE];
    if(GetPrimitives3d(hsp3d,ho,&p,item,1,pnums,
                       MAX_IN_PRIMITIVE,txt,MAX_IN_PRIMITIVE)){
        if(get){
            CheckDlgButton(hwnd,IDC_primSHADE,(p.flags&PRIMITIVE_NOSHADE)==0);
            CheckDlgButton(hwnd,IDC_primPOLYLINE,(p.flags&PRIMITIVE_POLYLINE)!=0);
            CheckDlgButton(hwnd,IDC_primDBL,(p.flags&PRIMITIVE_ALWAYS)!=0);
            CheckDlgButton(hwnd,IDC_txtUSE,(p.flags&PRIMITIVE_TEXTURED)!=0);
            EnableWindow(GetDlgItem(hwnd,IDC_EDITTXTPOINTS),(p.flags&PRIMITIVE_TEXTURED)!=0);
            UpdateTxtCombo(hwnd,IDC_TEXTURE,hsp3d,p.material);
        }else{
            if(IsDlgButtonChecked(hwnd,IDC_primPOLYLINE))
            {p.flags|=PRIMITIVE_POLYLINE;p.flags&=~PRIMITIVE_POLYGON;}else
            {p.flags|=PRIMITIVE_POLYGON;p.flags&=~PRIMITIVE_POLYLINE;}
            if(IsDlgButtonChecked(hwnd,IDC_primSHADE))p.flags&=~PRIMITIVE_NOSHADE;
            else p.flags|= PRIMITIVE_NOSHADE;
            if(IsDlgButtonChecked(hwnd,IDC_primDBL))p.flags|=PRIMITIVE_ALWAYS;
            else p.flags&=~PRIMITIVE_ALWAYS;
            if(IsDlgButtonChecked(hwnd,IDC_txtUSE))p.flags|=PRIMITIVE_TEXTURED;
            else p.flags&=~PRIMITIVE_TEXTURED;

            if(IsDlgButtonChecked(hwnd,IDC_primCLOSE) &&
               (p.flags&PRIMITIVE_POLYLINE))p.flags|=PRIMITIVE_CLOSELINE;
            else p.flags&=~PRIMITIVE_CLOSELINE;

            HWND list=GetDlgItem(hwnd,IDC_TEXTURE);
            int i=(int)SendMessage(list,CB_GETCURSEL,0,0);
            if(i>-1){
                HOBJ2D tool=GetToolHANDLEbyNum((HSP2D)hsp3d,TEXTURE3D,(INT16)i);
                p.material=tool;
            }
            SetPrimitives3d(hsp3d,ho,&p,item,1);
        }
        EnableWindow(GetDlgItem(hwnd,IDC_primCLOSE),(p.flags&PRIMITIVE_POLYLINE)!=0);
    }
    //  SetPrimitives3di(hsp3d,prim,&p,&item,1);
};


BOOL scCALLBACK PrimToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_SETTOOLVAR:{
            switch(wParam){
                case 50:{
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    if(b){  b->data = lParam; }
                    UpdatePrimitive(hwnd,(HSP3D)b->hsp,(HOBJ2D)b->iuser1,(INT16)b->data,TRUE);

                }break;
            }}break;
        case PSP_ACTIVATE:SendCommand(mainwindow->HWindow,CM_EDITPRIM3D );return TRUE;
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_TEXTURE:if(wNotifyCode==CBN_SELCHANGE){
                        TPropertyButton*b=GetDlgPropertyButton(hwnd);
                        if(!b)return FALSE;
                        UpdatePrimitive(hwnd,(HSP3D)b->hsp,(HOBJ2D)b->iuser1,(INT16)b->data,FALSE);
                    }return TRUE;
                case IDC_primSHADE:
                case IDC_primPOLYLINE:
                case IDC_primDBL:
                case IDC_txtUSE:if(wNotifyCode==BN_CLICKED){
                        TPropertyButton*b=GetDlgPropertyButton(hwnd);
                        if(!b)return FALSE;
                        UpdatePrimitive(hwnd,(HSP3D)b->hsp,(HOBJ2D)b->iuser1,(INT16)b->data,FALSE);
                    };return TRUE;
                case IDC_EDITTXTPOINTS:{
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    if(!b)return FALSE;
                    HSP3D hsp3d=(HSP3D)(b->hsp);
                    HOBJ2D ho=(HOBJ2D)(b->iuser1);
                    INT16 item=(INT16)(b->data);
                    PRIMITIVE3D p;
                    INT16 pnums[MAX_IN_PRIMITIVE];
                    POINT2D txt[MAX_IN_PRIMITIVE];
                    if(GetPrimitives3d(hsp3d,ho,&p,item,1,pnums,
                                       MAX_IN_PRIMITIVE,txt,MAX_IN_PRIMITIVE)){
                        int m=matrixs->MCreate(0,0,p.count-1,0,1,0);
                        for(int i=0;i<p.count;i++){
                            matrixs->MPut(m,i,0,txt[i].x);
                            matrixs->MPut(m,i,1,txt[i].y);
                        }
                        if(MatrixDialog(hwnd,m)){
                            for(i=0;i<p.count;i++){
                                txt[i].x=matrixs->MGet(m,i,0);
                                txt[i].y=matrixs->MGet(m,i,1);
                            }
                            SetPrimitives3d(hsp3d,ho,&p,item,1);
                        }}}return TRUE;
            }
        }break;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
void TxtC_Check(HWND hwnd,BOOL planar){
    EnableWindow(GetDlgItem(hwnd,IDC_txtCMAX),planar);
    EnableWindow(GetDlgItem(hwnd,IDC_txtCMIN),planar);
};

BOOL scCALLBACK TxtToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_DESTROY:{
            SendCommand(mainwindow->HWindow,CM_UNMODIFY3D);
        }break;
        case PSP_ACTIVATE:SendCommand(mainwindow->HWindow,CM_TEXTUREMAP);return TRUE;
        case PSP_INITPAGE:{
            int m=SCGetScVar("txtmap_type",0);
            int id=0;
            switch(m&0xf){
                case 0:id=IDC_txtPLANAR;break;
                case 1:id=IDC_txtBOX;break;
                case 2:id=IDC_txtCYLINDRICAL;break;
                case 3:id=IDC_txtSPHERICAL;break;
            }
            TxtC_Check(hwnd,(m&0xf)==0);
            CheckDlgButton(hwnd,id,1);
            if(m&256)CheckDlgButton(hwnd,IDC_txtXTILEflip,1);
            if(m&512)CheckDlgButton(hwnd,IDC_txtYTILEflip,1);
            double r;
            if(SCGetScVarD("txtmap_tilex",r)){
                SetDlgItemDouble(hwnd,IDC_txtXTILE,r);
            }
            if(SCGetScVarD("txtmap_tiley",r)){
                SetDlgItemDouble(hwnd,IDC_txtYTILE,r);
            }

            if(SCGetScVarD("txtmap_pl_maxC",r)){
                SetDlgItemDouble(hwnd,IDC_txtCMAX,r);
            }
            if(SCGetScVarD("txtmap_pl_minC",r)){
                SetDlgItemDouble(hwnd,IDC_txtCMIN,r);
            }

        }break;
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_txtCMAX:
                case IDC_txtCMIN:
                case IDC_txtYTILE:
                case IDC_txtXTILE:if(wNotifyCode == EN_UPDATE){
                        double r;
                        char *_var=NULL;
                        double _min=0,_max=49,_d=0;
                        switch(command){
                            case IDC_txtCMAX: _var="txtmap_pl_maxC" ;_d=_min=-1,_max=1;break;
                            case IDC_txtCMIN: _var="txtmap_pl_minC" ;_min=-1,_d=_max=1;break;
                            case IDC_txtYTILE:_var="txtmap_tiley";break;
                            case IDC_txtXTILE:_var="txtmap_tilex";break;
                        }
                        if(!IsDoubleValid(hwnd,command,r,_min,_max,FALSE))r=0;
                        if(r==_d)SCDeleteScVar(_var);else  SCSetScVarD(_var,r);
                    }break;
                case IDC_txtPLANAR:
                case IDC_txtCYLINDRICAL:
                case IDC_txtSPHERICAL:
                case IDC_txtBOX:{
                    int c=0;
                    switch(command){
                        case IDC_txtPLANAR:c=CM_TXTPLANAR;break;
                        case IDC_txtCYLINDRICAL:c=CM_TXTCYLINDER;break;
                        case IDC_txtSPHERICAL:c=CM_TXTSPHERE;break;
                        case IDC_txtBOX:c=CM_TXTBOX;break;
                    }
                    TxtC_Check(hwnd,command==IDC_txtPLANAR);
                    SendCommand(mainwindow->HWindow,c);
                }

                case IDC_txtXTILEflip:
                case IDC_txtYTILEflip:{
                    int m=0;
                    if(IsDlgButtonChecked(hwnd,IDC_txtBOX))m=1;
                    if(IsDlgButtonChecked(hwnd,IDC_txtCYLINDRICAL))m=2;
                    if(IsDlgButtonChecked(hwnd,IDC_txtSPHERICAL))m=3;
                    if(IsDlgButtonChecked(hwnd,IDC_txtXTILEflip))m|=256;
                    if(IsDlgButtonChecked(hwnd,IDC_txtYTILEflip))m|=512;
                    SCSetScVarI("txtmap_type",m);
                }break;
                case IDC_REMOVETEXTURE:{
                    SendCommand(mainwindow->HWindow,CM_REMOVETEXTURE);
                }break;
                case IDC_APPLY:{
                    SendCommand(mainwindow->HWindow,CM_APPLYTEXTURE);
                }break;
            }
        }
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}

BOOL scCALLBACK NLMethodProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(Environment_options___Methods)
            switch(msg){
        case WM_INITDIALOG:{
            TProject*prj=(TProject*)lParam;
            SetWindowLong(hwnd,DWL_USER,lParam);
            int d1=30;
            double d2=0.1,d3=0.001;
            BOOL optimize=0;

            if(prj){
                d1=prj->GetVarDef("equ_nl_maxstep",30);
                d2=prj->GetVarD("equ_nl_sense",0.1);
                d3=prj->GetVarD("equ_nl_error",0.01);
                optimize=prj->GetVarDef("equ_nl_optimize",0);

            }else{
                d1=sc_vars->GetVarDef("equ_nl_maxstep",30);
                sc_vars->GetVarD("equ_nl_sense",d2);
                sc_vars->GetVarD("equ_nl_error",d3);
                optimize=sc_vars->GetVarDef("equ_nl_optimize",0);
            }
            EditSubClass(hwnd,IDC_EQUITER,1,100,1,UDF_MINMAX);
            SetDlgItemInt(hwnd,IDC_EQUITER,d1,0);
            SetDlgItemDouble(hwnd,IDC_EQUCH,d2);
            SetDlgItemDouble(hwnd,IDC_EQUERROR,d3);
            CheckDlgButton(hwnd,IDC_EQUOPTIMIZE,optimize);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDOK:{
                    TProject*prj=(TProject*)GetWindowLong(hwnd,DWL_USER);
                    INT16 d1=30;
                    double d2=0.1,d3=0.001;
                    BOOL optimize=IsDlgButtonChecked(hwnd,IDC_EQUOPTIMIZE);
                    if(IsIntValid(hwnd,IDC_EQUITER,d1,0,16384)  &&
                       IsDoubleValid(hwnd,IDC_EQUCH,d2,0,1)   &&
                       IsDoubleValid(hwnd,IDC_EQUERROR,d3,0,1))
                    {
                        if(prj){
                            prj->SetVarI("equ_nl_maxstep",d1);
                            prj->SetVarD("equ_nl_sense",d2,-1);
                            prj->SetVarD("equ_nl_error",d3,-1);
                            prj->SetVarI("equ_nl_optimize",optimize);
                        }else{
                            SCSetScVarI("equ_nl_maxstep",d1);
                            SCSetScVarI("equ_nl_optimize",optimize);
                            SCSetScVarD("equ_nl_sense",d2);
                            SCSetScVarD("equ_nl_error",d3);
                        }
                        EndDialog(hwnd,1);
                    }
                    return TRUE;
                }
                case IDCANCEL:{
                    EndDialog(hwnd,0);return TRUE;
                };
            }
        }break;
    }
    return FALSE;
};
BOOL scCALLBACK LMethodProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(Environment_options___Methods)
            switch(msg){
        case WM_INITDIALOG:{
            TProject*prj=(TProject*)lParam;
            SetWindowLong(hwnd,DWL_USER,lParam);
            BOOL optimize;
            if(prj){
                optimize=prj->GetVarDef("equ_l_optimize",0);
            }else{
                optimize=sc_vars->GetVarDef("equ_l_optimize",0);
            }
            CheckDlgButton(hwnd,IDC_EQUOPTIMIZE,optimize);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDOK:{
                    TProject*prj=(TProject*)GetWindowLong(hwnd,DWL_USER);
                    BOOL optimize=IsDlgButtonChecked(hwnd,IDC_EQUOPTIMIZE);
                    if(prj){
                        prj->SetVarI("equ_l_optimize",optimize);
                    }else{
                        SCSetScVarI("equ_l_optimize",optimize);
                    }
                    EndDialog(hwnd,1);
                    return TRUE;
                }
                case IDCANCEL:{
                    EndDialog(hwnd,0);return TRUE;
                };
            }
        }break;
    }
    return FALSE;
};
BOOL NonLinearDialog(HWND hwnd,TProject*prj){
    return DialogBoxParam(HResource,MAKEINTRESOURCE(NRSetup),hwnd,NLMethodProc,(LPARAM)prj);
};
BOOL LinearDialog(HWND hwnd,TProject*prj){
    return DialogBoxParam(HResource,MAKEINTRESOURCE(LINEAR_DIALOG),hwnd,LMethodProc,(LPARAM)prj);
};


void _SetVarData(PObject po,char *v,char *d,int f){
    {
        INT16 id=po->GetVarIdByName(v);
        if(id>-1){
            if(f)SetObjectVarA(po,id,d);
            else _SetObjectVarA(po,id,d);
        }
    }
    PObject _po=po->childObjects;
    while(_po){
        _SetVarData(_po,v,d,f);
        _po=_po->next;
    }
};
void AddVarName(TStringCollection* str,PObject po){
    if(po){
        PClass _class =po->_class;
        for(int j=0;j<_class->varcount;j++){
            char * v=_class->vars[j].name;
            C_TYPE index;
            if(!str->Search(v,index)){
                str->Insert(v);
            }}
        PObject _po=po->childObjects;
        while (_po){
            AddVarName(str,_po);
            _po=_po->next;
        }
    }
};

BOOL scCALLBACK SetVarProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            switch(msg){
        case WM_INITDIALOG:{
            TCollection *items=(TCollection *)lParam;
            SetWindowLong(hwnd,DWL_USER,(LPARAM)lParam);
            TStringCollection str(10,10);
            HWND list=GetDlgItem(hwnd,IDC_SETVARNAME);
            for(C_TYPE i=0;i<items->count;i++){
                AddVarName(&str,(PObject)(items->At(i)));
            }
            for(i=0;i<str.count;i++)
                SendMessage(list,CB_ADDSTRING,0,(LPARAM)(str.At(i)));
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDOK:{
                    char var[256],data[256];
                    int set=IsDlgButtonChecked(hwnd,IDC_SETVARASSET);
                    GetDlgItemText(hwnd,IDC_SETVARNAME,var,sizeof(var));
                    GetDlgItemText(hwnd,IDC_VARVALUE,data,sizeof(var));
                    TCollection *items=(TCollection *)GetWindowLong(hwnd,DWL_USER);
                    for(C_TYPE i=0;i<items->count;i++){
                        _SetVarData((PObject)items->At(i),var,data,set);
                    }
                    EndDialog(hwnd,1);return TRUE;
                }
                case IDCANCEL:{
                    EndDialog(hwnd,0);return TRUE;
                };
            }
        }break;
    }
    return FALSE;
}

BOOL SetVarDialog(HWND hwnd,TCollection*items){
    return DialogBoxParam(HResource,MAKEINTRESOURCE(SETVAR_DIALOG),hwnd,SetVarProc,(LPARAM)items);
};

int ListBoxProc(HWND hwnd,UINT msg,WPARAM&wParam,LPARAM&lParam,int _id);
BOOL scCALLBACK PlugProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){

    if(ListBoxProc(hwnd,msg,wParam,lParam,IDC_PLUGINS)==2)
    {
        HWND list=GetDlgItem(hwnd,IDC_PLUGINS);
        int i=(int)wParam;
        char s[256],s1[256],s2[256]="",s3[256];

        SendMessage(list,LB_GETTEXT,i,(LPARAM)s);
        char *ps=strstr(s,"(");
        if(ps){
            lstrcpyn(s1,s+1,ps-s);
            lstrcpyn(s2,ps+1,lstrlen(ps)-1);
        }else lstrcpy(s1,s+1);

        lstrcpy(s,"($plugins)");
        lstrcat(s,s1);

        if(lParam=='c'){
            if(dllmanager->LoadPlugin(s,0)){
                lstrcpy(s3,"1");
            }else {
                ReleaseCapture();
                return TRUE;
            }
        }else{
            if(dllmanager->FreeDLL(dllmanager->Search(s))) lstrcpy(s3,"");else return TRUE;
        }
        lstrcat(s3,",");lstrcat(s3,s2);
        ps=s3;
        if(!lstrcmp(s3,","))ps=NULL;
        WritePrivateProfileString(_plugins,s1,ps,ini_file);
        return TRUE;
    }
    _STDHELP(Options___Moduls)
            switch(msg){
        case WM_INITDIALOG:{
            char sp[256];
            GetFullFileNameEx(NULL,"($plugins)",sp);
            TFileIterator fcls(sp,"*.scx");
            while (fcls.GetNext()){
                char s[256]="u";
                char s1[256]="";
                wsprintf(s1,"($plugins)%s",fcls.GetFileName());
                if(dllmanager->Search(s1))s[0]='c';
                s1[0]=0;
                GetPrivateProfileString(_plugins,fcls.GetFileName(),"",s1,sizeof(s1),ini_file);
                char * ps;
                lstrcat(s,fcls.GetFileName());

                if(ps=strstr(s1,","))
                { if(lstrlen(ps)>2)wsprintf(s+lstrlen(s)," (%s)",ps+1);};
                SendDlgItemMessage(hwnd,IDC_PLUGINS,LB_ADDSTRING,0,(LPARAM)s);
            }
        }break;
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_PLUGINS:if(wNotifyCode==LBN_DBLCLK){
                        char s[256],s1[256];
                        int i=(int)SendMessage(hwndCtl,LB_GETCURSEL,0,0);
                        SendMessage(hwndCtl,LB_GETTEXT,i,(LPARAM)s);
                        if(s[0]=='c'){
                            char *ps=strstr(s,"(");
                            if(ps){
                                lstrcpyn(s1,s+1,(int)(ps-s));
                            }else lstrcpy(s1,s+1);
                            lstrcpy(s,"($plugins)");
                            lstrcat(s,s1);
                            TFCollection *f=dllmanager->Search(s);
                            if(f){
                                f->Procedure(SC_DLLDIALOG,(UINT32)hwnd,0);
                            }
                        }return TRUE;
                    }break;
                case IDOK:
                case IDCANCEL:{
                    EndDialog(hwnd,0);return TRUE;
                };
            }
        }break;
    }
    return FALSE;
}

BOOL PluginsDialog(HWND hwnd){
    return DialogBox(HResource,MAKEINTRESOURCE(PLUGINS_DIALOG),hwnd,PlugProc);
};

struct TXTSTR{
    HSP3D     hsp3d;
    HOBJ2D    txt;
    MATERIAL3D m;
};

void ChdkFTttkpo(HWND hwnd){
    int i = (int)SendDlgItemMessage(hwnd,IDC_TEXTURETYPE,CB_GETCURSEL,0,0);
    EnableWindow(GetDlgItem(hwnd,IDC_TEXTURELINK),i==0);
};

BOOL scCALLBACK TextureProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            switch(msg){
        case WM_INITDIALOG:{
            SetWindowLong(hwnd,DWL_USER,lParam);
            TXTSTR *txt=(TXTSTR *)lParam;
            SetDlgItemText(hwnd,IDC_TXTNAME,txt->m.name);
            SetDlgItemText(hwnd,IDC_TEXTUREMAP,txt->m.hfile);
            CheckDlgButton(hwnd,IDC_TEXTURELINK,!(txt->m.flags&TF_FILE));
            CheckDlgButton(hwnd,IDC_TXTNSCALECOLOR,(txt->m.flags&TF_CONSTCOLOR));

            SendDlgItemMessage(hwnd,IDC_AMBIENT,COLORB_SETCOLOR,0,txt->m.ambient.rgb);
            SendDlgItemMessage(hwnd,IDC_DIFFUSE,COLORB_SETCOLOR,0,txt->m.diffuse.rgb);
            SendDlgItemMessage(hwnd,IDC_EMITTANCE,COLORB_SETCOLOR,0,txt->m.emittance.rgb);
            SendDlgItemMessage(hwnd,IDC_SPECULAR,COLORB_SETCOLOR,0,txt->m.specular.rgb);

            SetDlgItemDouble(hwnd,IDC_TEXTUREXTILE,txt->m.defXtile);
            SetDlgItemDouble(hwnd,IDC_TEXTUREYTILE,txt->m.defYtile);

            SetDlgItemDouble(hwnd,IDC_SHINE	    ,txt->m.shine);
            SetDlgItemDouble(hwnd,IDC_TRANSPARENCY,txt->m.transparency);

            int i=IDS_TEXTURETYPE;
            char s[80];
            while(LoadString(HResource,i,s,sizeof(s))!=0){
                SendDlgItemMessage(hwnd,IDC_TEXTURETYPE,CB_ADDSTRING,0,(LPARAM)s);
                i++;
            }
            i=0;
            if(txt->m.exttype[0]){
                if(lstrcmpi(txt->m.exttype,"VIDEO2D")==0){i=1;
                    if(!strnicmp(txt->m.hfile,camera_header,lstrlen(camera_header)))i=2;
                }}
            SendDlgItemMessage(hwnd,IDC_TEXTURETYPE,CB_SETCURSEL,i,0);
            ChdkFTttkpo(hwnd);
        };
        case WM_COMMAND:{
            EV_COMMANDsimply
                    TXTSTR *txt=(TXTSTR *)GetWindowLong(hwnd,DWL_USER);
            switch (command){
                case IDC_TEXTURETYPE:ChdkFTttkpo(hwnd);return 0;
                case IDC_BROWSE:{
                    char s[256],s1[256];
                    GetDlgItemText(hwnd,IDC_TEXTUREMAP,s,sizeof(s));

                    if(IsAbsolutePath(s)==2){
                        char ss[256];
                        if(GetTexturePath2d(GetSpace2dBy3d(txt->hsp3d),ss)){
                            AddSlash(ss);
                            lstrcat(ss,s);
                            lstrcpy(s,ss);
                        }
                    }

                    int mode=IDS_BMP;
                    if((int)SendDlgItemMessage(hwnd,IDC_TEXTURETYPE,CB_GETCURSEL,0,0)==1)mode=IDS_AVI;
                    if(FileDialog(hwnd,s,mode,0)){
                        char * ps=s;
                        if(IsAbsolutePath(s)==1){
                            char ss[256];
                            if(GetTexturePath2d(GetSpace2dBy3d(txt->hsp3d),ss)){
                                AddSlash(ss);
                                lstrcpy(s1,s);
                                AnsiUpper(ss);AnsiUpper(s1);
                                if(strstr(s1,ss))ps+=strlen(ss);
                            }
                        }
                        SetDlgItemText(hwnd,IDC_TEXTUREMAP,ps);
                    }
                }break;
                case IDOK:{

                    if(!(IsDoubleValid(hwnd,IDC_TEXTUREXTILE,txt->m.defXtile,0.01,49)&&
                         IsDoubleValid(hwnd,IDC_TEXTUREYTILE,txt->m.defYtile,0.01,49)&&
                         IsDoubleValid(hwnd,IDC_SHINE,txt->m.shine,0.0,1.0)&&
                         IsDoubleValid(hwnd,IDC_TRANSPARENCY,txt->m.transparency,0.0,1.0)
                         ))return FALSE;

                    txt->m.color.rgb=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,0);

                    txt->m.ambient.rgb  = SendDlgItemMessage(hwnd,IDC_AMBIENT,COLORB_GETCOLOR,0  ,0);
                    txt->m.diffuse.rgb  = SendDlgItemMessage(hwnd,IDC_DIFFUSE,COLORB_GETCOLOR,0  ,0);
                    txt->m.emittance.rgb= SendDlgItemMessage(hwnd,IDC_EMITTANCE,COLORB_GETCOLOR,0,0);
                    txt->m.specular.rgb = SendDlgItemMessage(hwnd,IDC_SPECULAR,COLORB_GETCOLOR,0 ,0);


                    GetDlgItemText(hwnd,IDC_TXTNAME,txt->m.name,200);
                    GetDlgItemText(hwnd,IDC_TEXTUREMAP,txt->m.hfile,200);
                    txt->m.flags&=~(TF_FILE|TF_CONSTCOLOR);
                    int k=IsDlgButtonChecked(hwnd,IDC_TEXTURELINK);
                    if(!k) txt->m.flags|=TF_FILE;
                    if(IsDlgButtonChecked(hwnd,IDC_TXTNSCALECOLOR)) txt->m.flags|=TF_CONSTCOLOR;
                    k=SendDlgItemMessage(hwnd,IDC_TEXTURETYPE,CB_GETCURSEL,0,0);
                    memset(txt->m.exttype,0,sizeof(txt->m.exttype));
                    if(k){
                        txt->m.flags|=TF_EXTERNALDATA;
                        txt->m.flags|=TF_FILE;
                        switch(k){
                            case 2:if(strnicmp(txt->m.hfile,camera_header,lstrlen(camera_header)))
                                {
                                    lstrcpy(txt->m.hfile,camera_header);
                                    lstrcat(txt->m.hfile," /auto");
                                }
                            case 1:lstrcpy(txt->m.exttype,"VIDEO2D");break;
                        }

                    }else {txt->m.flags&=~TF_EXTERNALDATA;}

                    EndDialog(hwnd,1);

                    return TRUE;
                }
                case IDCANCEL:{
                    EndDialog(hwnd,0);return TRUE;
                }
            }}break;
    }
    return FALSE;
}

HOBJ2D TextureDialog(HWND hwnd,HSP3D hsp3d,HOBJ2D h){
    BOOL _new=FALSE;
    if(h==0){
        MATERIAL3D m;
        memset(&m,0,sizeof(m));
        m.poSize=sizeof(m);
        m.flags=TF_FILE;
        m.color.rgb=RGB(255,255,255);
        m.hdib=0;
        m.hfile="noname.bmp";//"pat.bmp";
        m.name="NONAME";
        h=CreateMaterial3d(hsp3d,&m);
        _new=FALSE;
    }

    TXTSTR t;
    memset(&t,0,sizeof(t));
    t.hsp3d = hsp3d;
    t.txt   =h;
    char s[256];
    char s1[256];
    t.m.name  = s;
    t.m.hfile = s1;
    if(GetMaterial3d(hsp3d,h,&(t.m))){
        if(DialogBoxParam(HResource,MAKEINTRESOURCE(TEXTURE_DIALOG),hwnd,TextureProc,(LPARAM)&t))
        {
            SetMaterial3d(hsp3d,h,&(t.m));
            return h;
        } else{
            if(_new) DeleteTool2d((HSP2D)hsp3d,TEXTURE3D,h);
        }}
    return FALSE;
};

/*

 Object2d dilaog

*/


C_TYPE DefChar(unsigned char ch);
int ExecuteFind(HWND edit,BYTE flags){
    int len=(int)SendMessage(edit,WM_GETTEXTLENGTH,0,0);
    if(len){
        UINT32 b=SCGetScVar("search_opt1",0);
        char s[256];
        char sr[256];
        SCGetScVarS("search_string",s);
        SCGetScVarS("replace_string",sr);
        char * text=new char[len+1];
        GetWindowText(edit,text,len+1);
        if(!(b&SR_CASE)){AnsiUpper(text);AnsiUpper(s);}



        delete text;
    }
};

void AddLayer(HWND combo,int start,int end,int current);

struct TABLESAVEstruct
{
    char * name;
    char *type;
};

#ifdef DBE
BOOL scCALLBACK SaveTableProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            HWND  hwndCtl;
    INT16 wNotifyCode;
    INT16 command;

    switch(msg){
        case WM_INITDIALOG:{
            SendDlgItemMessage(hwnd,IDC_TABLETYPE,CB_ADDSTRING,0,(LPARAM)"dBASE");
            SendDlgItemMessage(hwnd,IDC_TABLETYPE,CB_ADDSTRING,0,(LPARAM)"PARADOX");
            SendDlgItemMessage(hwnd,IDC_TABLETYPE,CB_ADDSTRING,0,(LPARAM)"TEXT");
            SetWindowLong(hwnd,DWL_USER,lParam);
        };
        case WM_COMMAND:{
            EV_COMMANDnoVar
                    switch (command){
mclose:
        case IDC_BROWSE:{
                    char s[256];
                    GetDlgItemText(hwnd,IDC_TABLENAME,s,sizeof(s));
                    if(FileDialog(hwnd,s,IDS_TABLEfiles,FD_SAVE))
                        SetDlgItemText(hwnd,IDC_TABLENAME,s);
                }break;
                case IDOK:{

                    TABLESAVEstruct*ts=(TABLESAVEstruct*)GetWindowLong(hwnd,DWL_USER);
                    GetDlgItemText(hwnd,IDC_TABLENAME,ts->name,260);
                    GetDlgItemText(hwnd,IDC_TABLETYPE,ts->type,256);
                    EndDialog(hwnd,TRUE);return TRUE;}

                case IDCANCEL: EndDialog(hwnd,0);return TRUE;
            }}break;

    }
    return FALSE;
}

#endif
BOOL SaveTableDialog(HWND hwnd,int table){
#ifdef DBE
    char name[260]="";
    char type[256]="";
    TABLESAVEstruct st;
    st.name=name;
    st.type=type;
    if(DialogBoxParam(HResource,MAKEINTRESOURCE(SAVETBL_DIALOG),hwnd,SaveTableProc,(LPARAM)&st)){
        DBCopyTo(table,name,type);
    }
#endif
    return FALSE;
}


struct TSQLstruct{
    int base;
    int table;
    COLORREF bk,fg;
    TStringCollection *str;
    int sizey;
};
/*
 COLORREF scTEXT;
 COLORREF scBK;
 BYTE scStyle; // 1 - italic/ 2- bold /4 -underline
*/
BOOL _EXPORT GetSQLColorProc(DWORD _data,char*str,int size,SYNTCOLOR*sc){
    char s[256];
    lstrcpyn(s,str,min(sizeof(s),size+1));
    AnsiUpper(s);
    TSQLstruct*sql=(TSQLstruct*)_data;
    C_TYPE i;
    sc->scTEXT=sql->fg;
    sc->scBK=  sql->bk;
    if(sql->str->Search(s,i)){
        sc->scStyle=2;
        return TRUE;
    }
    sc->scStyle=0;
    return FALSE;
}

void SetSQLSyntaxColor(TSQLstruct&sql,HWND edit){
    SETCOLOR _sc;
    _sc.data=(DWORD)&sql;
    _sc.GetColor=GetSQLColorProc;

    SendMessage(edit,ESM_SETCOLORPROC, 0,(LPARAM)&_sc);

    // lstrcpy(s,IDS_str4);
    // LoadStr(IDS_str4,s);
    TSColor *sc=new TSColor();
    sc->SetAuto();
    sc->Set(edit);

    sql.bk=sc->bkgrndsyntax;
    sql.fg=sc->textsyntax;

    delete sc;

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

    if(!SCGetScVarS("syntax_edit_font",s))lstrcpy(s,"Courier");
    lstrcpy(lf.lfFaceName,s);
    SendMessage(edit,ESM_SETCOPTIONS,256,(LPARAM)&lf);

};
#ifdef DBE
void __SetTables(HWND hwnd){

    int i=SendDlgItemMessage(hwnd,IDC_BASES,CB_GETCURSEL,0,0);
    int b=0;
    int j=0;
    while ((b=DBGetNextBase(b))!=0)
    {
        if(j==i){
            SendDlgItemMessage(hwnd,IDC_TABLES,CB_RESETCONTENT,0,0);
            int t=0;
            while(t=DBGetNextTable(b,t)){
                char s[256];
                DBGetTableDesc(t,s);
                SendDlgItemMessage(hwnd,IDC_TABLES,CB_ADDSTRING,0,(LPARAM)s);
            }
            SendDlgItemMessage(hwnd,IDC_TABLES,CB_SETCURSEL,0,0);
            return;
        }j++;
    };
}

BOOL scCALLBACK SQLChildProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(0)
            HWND  hwndCtl;
    INT16 wNotifyCode;
    INT16 command;

    switch(msg){
        case WM_SIZE:{
            int x = LOWORD(lParam);  /* width of client area  */
            int y = HIWORD(lParam); /* height of client area */
            HWND b=GetDlgItem(hwnd,IDC_CLOSETABLE);
            HWND b1=b;
            RECT r,r1;
            GetWindowRect(b,&r);
            r1=r;
            b=GetDlgItem(hwnd,IDC_EXECUTESQL);
            GetWindowRect(b,&r);

            MapWindowPoints(GetDesktopWindow(),hwnd,((POINT*)&r),2);
            SetWindowPos(b,0,x-(5+r.right-r.left),r.top,0,0,SWP_NOZORDER|SWP_NOSIZE);
            b=GetDlgItem(hwnd,IDC_SQLTEXT);
            GetWindowRect(b,&r);
            MapWindowPoints(GetDesktopWindow(),hwnd,((POINT*)&r),2);
            SetWindowPos(b,0,0,0,x-(10+r.left),r.bottom-r.top,SWP_NOZORDER|SWP_NOMOVE);

            b=GetDlgItem(hwnd,IDC_TABLERSLT);
            GetWindowRect(b,&r);
            MapWindowPoints(GetDesktopWindow(),hwnd,((POINT*)&r),2);
            //	 int _y=y-r.bottom;
            int _y=y-r.top-(r1.bottom-r1.top+10);

            if(_y>20){
                SetWindowPos(b,0,0,0,x-10,_y,SWP_NOZORDER|SWP_NOMOVE);
                SetWindowPos(b1,0,x-(5+r1.right-r1.left),y-(5+r1.bottom-r1.top),0,0,SWP_NOZORDER|SWP_NOSIZE);
            }
        };return TRUE;
        case WM_GETMINMAXINFO:{
            TSQLstruct*sql=(TSQLstruct*)GetWindowLong(hwnd,DWL_USER);

            MINMAXINFO FAR* lpmmi;
            lpmmi = (MINMAXINFO FAR*) lParam;
            lpmmi->ptMinTrackSize.x = 500;
            if(sql->table){
                lpmmi->ptMinTrackSize.y = 300;
            }else{
                lpmmi->ptMinTrackSize.y = sql->sizey;
                lpmmi->ptMaxTrackSize.y = sql->sizey;
            }
        }return TRUE;
        case WM_INITDIALOG:{
            TSQLstruct*sql=(TSQLstruct*)lParam;
            sql->sizey=0;
            SetSQLSyntaxColor(*sql,GetDlgItem(hwnd,IDC_SQLTEXT));
            SetWindowLong(hwnd,DWL_USER,lParam);
            int b=0;
            while (b=DBGetNextBase(b))
            {
                char s[256];
                DBGetBaseDesc(b,s);
                SendDlgItemMessage(hwnd,IDC_BASES,CB_ADDSTRING,0,(LPARAM)s);
            };
            SendDlgItemMessage(hwnd,IDC_BASES,CB_SETCURSEL,0,0);
            __SetTables(hwnd);
            goto mclose;
        };
        case WM_COMMAND:{
            EV_COMMANDnoVar
                    switch (command){
mclose:
        case IDC_CLOSETABLE:{
                    RECT r;
                    TSQLstruct*sql=(TSQLstruct*)GetWindowLong(hwnd,DWL_USER);
                    if(sql->table){
                        DBCloseTable(sql->table);sql->table=0;
                    }
                    HWND table=GetDlgItem(hwnd,IDC_TABLERSLT);
                    GetWindowRect(table,&r);
                    int dy=GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYDLGFRAME);
                    MapWindowPoints(GetDesktopWindow(),hwnd,((POINT*)&r),2);
                    int _y=r.top+dy;
                    if(sql->sizey==0){
                        sql->sizey=_y;
                    }
                    SetWindowPos(hwnd,HWND_TOP,0,0,r.right+10,_y,SWP_NOZORDER|SWP_NOMOVE);
                    ShowWindow(GetDlgItem(hwnd,IDC_CLOSETABLE),SW_HIDE);
                }return TRUE;
                case IDC_EXECUTESQL:{
                    TSQLstruct*sql=(TSQLstruct*)GetWindowLong(hwnd,DWL_USER);
                    char s[2048];
                    GetDlgItemText(hwnd,IDC_SQLTEXT,s,sizeof(s));
                    sql->table=DBSQL(sql->base,s,2);
                    if(sql->table){
                        RECT r;
                        HWND table=GetDlgItem(hwnd,IDC_TABLERSLT);
                        GetWindowRect(GetDlgItem(hwnd,IDC_CLOSETABLE),&r);
                        int dy=GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYDLGFRAME)+10;
                        MapWindowPoints(GetDesktopWindow(),hwnd,((POINT*)&r),2);
                        SetWindowPos(hwnd,HWND_TOP,0,0,r.right+10,r.bottom+dy,SWP_NOZORDER|SWP_NOMOVE);
                        ShowWindow(GetDlgItem(hwnd,IDC_CLOSETABLE),SW_SHOW);
                        TCursorWindow*cw=(TCursorWindow*)GetWindowLong(table,0);
                        cw->SetTable(sql->table,0);
                    }else{
                        int code=DBGetLastError();
                        if(code){
                            DBGetTextError(code,s);
                            MSGBOX(hwnd,s,_MSG_ERROR,MB_OK|MB_ICONINFORMATION);
                        }
                    }
                }return TRUE;
                case IDCANCEL: EndDialog(hwnd,0);return TRUE;
                case IDC_SQLHELP:HELP("SQL");return TRUE;
                case IDC_BASES:{ __SetTables(hwnd); }return TRUE;
            }}break;

    }
    return FALSE;

};
#endif
BOOL DoSqlDialog(HWND hwnd){
#ifdef DBE
    InitSyntax();
    HRSRC resource=FindResource(HResource,"sqlkeys","text");
    HGLOBAL res=LoadResource(HResource,resource);
    TSQLstruct sql;
#ifdef WIN32
    if(res)
#else
    if((int)res>32)
#endif
    {
        char *addr=(char*)LockResource(res);
        long size=SizeofResource(HResource,resource);

        TStringCollection st(10,10);
        sql.base=16384;
        sql.table=0;
        char * _ps=addr;
        while ((*addr)!=';'){
            if(*addr==','){
                char s[256];
                lstrcpyn(s,_ps,(addr-_ps)+1);
                if(addr-_ps)st.Insert(NewStr(s));
                _ps=addr+1;

            }
            addr++;
        }
        sql.str=&st;
        sql.sizey=0;
#ifdef WIN32
        UnlockResource(res);
        FreeResource(res);
#else
        FreeResource(res);
#endif
        int rez=DialogBoxParam(HResource,MAKEINTRESOURCE(SQL_DIALOG),hwnd,SQLChildProc,(LPARAM)&sql);
        for(C_TYPE i=0;i<st.count;i++)delete st.At(i);
        //rez=GetLastError();
        return rez;
    }
#endif
};
void SetDlgOrgSize2d(HWND hwnd,HSP2D hsp,HOBJ2D obj)
{
    POINT2D org,size;
    GetObjectOrgSize2d(hsp,obj,&org,&size);
    SetDlgItemDouble(hwnd,IDC_X,org.x);
    SetDlgItemDouble(hwnd,IDC_Y,org.y);
    SetDlgItemDouble(hwnd,IDC_SIZEX,size.x);
    SetDlgItemDouble(hwnd,IDC_SIZEY,size.y);
};

BOOL scCALLBACK ObjPlaceProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg){
        case WM_INITDIALOG:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b)
                if(b->data)
                    EnableWindow(GetDlgItem(GetParent(hwnd),IDOK),0);
        }
            return TRUE;
        case PSP_INITPAGE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            EditSubClass(hwnd,IDC_X,0,0,1,UDF_DOUBLE);
            EditSubClass(hwnd,IDC_Y,0,0,1,UDF_DOUBLE);
            EditSubClass(hwnd,IDC_SIZEX,1,0,1,UDF_DOUBLE|UDF_MIN);
            EditSubClass(hwnd,IDC_SIZEY,1,0,1,UDF_DOUBLE|UDF_MIN);
            EditSubClass(hwnd,IDC_ANGLE,0,0,1,UDF_DOUBLE);
            SetDlgOrgSize2d(hwnd,hsp,obj);
            double a=GetObjectAngle2d(hsp,obj);
            char text[16];
            a*=180.0/M_PI; //3.1415926536;
            sprintf(text,"%g",a);
            SetDlgItemText(hwnd,IDC_ANGLE,text);
            char s[256];s[0]=0;
            GetObjectName2d(hsp,obj,s,255);
            SetDlgItemText(hwnd,IDC_OBJECTNAME,s);
            WORD atr=GetAttribute2d(hsp,obj);
            if(atr&OF_DONTSELECT)CheckDlgButton(hwnd,IDC_NOSELECT,1);
            AddLayer(GetDlgItem(hwnd,IDC_LAYER),0,16,GetObjectLayer2d(hsp,obj));
        }
            return TRUE;
        case PSP_CLOSE:
        {
            if(wParam==1)
            {
                //TPropertyButton*b=(TPropertyButton*)lParam;
            }
        }
            return TRUE;
        case PSP_VALID:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE)
            {
                POINT2D org,size;
                double _a;
                if(
                   IsDoubleValid(hwnd,IDC_X,org.x,-32760,32760)&&
                   IsDoubleValid(hwnd,IDC_Y,org.y,-32760,32760)&&
                   IsDoubleValid(hwnd,IDC_SIZEX,size.x,0,32760)&&
                   IsDoubleValid(hwnd,IDC_SIZEY,size.y,0,32760)&&
                   IsDoubleValid(hwnd,IDC_ANGLE,_a,-360,360) )
                {
                    b->Valid();
                    if (wParam==1)
                    {
                        HSP2D hsp=b->hsp;
                        HOBJ2D obj=b->iuser1;
                        POINT2D _org,_size;
                        GetObjectOrgSize2d(hsp,obj,&_org,&_size);
                        _a*=M_PI/180.0;		//3.1415926536/180.0;
                        double a=GetObjectAngle2d(hsp,obj);
                        if (_org.x!=org.x ||_org.y!=org.y)
                            SetObjectOrg2d(hsp,obj,&org);
                        if (_size.x!=size.x ||_size.y!=size.y)
                            SetObjectSize2d(hsp,obj,&size);
                        if ((0.001)<(fabs(a-_a)))
                        {
                            ROTATOR2D r;
                            r.center.x=_org.x+size.x/2;
                            r.center.y=_org.y+size.y/2;
                            r.angle=_a-a;
                            RotateObject2d(hsp,obj,&r);
                        }
                        char s[256];
                        GetObjectName2d(hsp,obj,s,255);
                        char ss[256];
                        GetDlgItemText(hwnd,IDC_OBJECTNAME,ss,255);
                        if(lstrcmp(s,ss))
                            SetObjectName2d(hsp,obj,ss);
                        WORD atr=GetAttribute2d(hsp,obj);
                        WORD _atr=atr;
                        BOOL noselect=IsDlgButtonChecked(hwnd,IDC_NOSELECT);
                        if (noselect)
                            atr|=OF_DONTSELECT;
                        else
                            atr&=~OF_DONTSELECT;
                        if (atr!=_atr)
                            SetAttribute2d(hsp,obj,OF_DONTSELECT,noselect?ATTRSET:ATTRRESET);
                        INT16 layer=(INT16)((SendDlgItemMessage(hwnd,IDC_LAYER,CB_GETCURSEL,0,0)));
                        if(GetObjectLayer2d(hsp,obj)!=layer)
                            SetObjectLayer2d(hsp,obj,layer);
                    }
                }
                else
                    b->Invalid();
            }
            else
                b->Valid();
        }
            return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

void EnablePens(HWND HWindow,BOOL e){
    EnableWindow(GetDlgItem(HWindow,IDC_PENSTYLE),e);
    EnableWindow(GetDlgItem(HWindow,IDC_COLOR),e);
    EnableWindow(GetDlgItem(HWindow,IDC_WIDTH),e);
    EnableWindow(GetDlgItem(HWindow,IDC_ROP),e);
    EnableWindow(GetDlgItem(HWindow,IDC_ARROW1),e);
    EnableWindow(GetDlgItem(HWindow,IDC_ARROW2),e);
}
struct ARROW_DATA
{
    INT16 l;
    gr_float a;
};

#define A_15 (15/180.0*M_PI)
#define A_30 (30/180.0*M_PI)
#define A_45 (45/180.0*M_PI)
ARROW_DATA arrow_data[]={
    {5,A_15},
    {10,A_15},
    {15,A_15},
    {5,A_30},
    {10,A_30},
    {15,A_30},
    {5,A_45},
    {10,A_45},
    {15,A_45}
};

void SetArrow(HWND hwnd,gr_float l,gr_float a,BOOL f){
    int i=0;
    if (l>0 && a>0){
        for(int j=0;j<9;j++){
            if (arrow_data[j].l==l && arrow_data[j].a==a){
                i=j+1;
                if (f)i+=9;
                break;
            }}}
    SendMessage(hwnd,CB_SETCURSEL,i,0);
};

BOOL GetArrow(int i,gr_float &l,gr_float& a){
    if (i){
        int j=(i-1)%9;
        l=arrow_data[j].l;
        a=arrow_data[j].a;
        if (i>9)return TRUE;
    }else{l=0;a=0;return FALSE;}
    return FALSE;
}
char * pen_st[]={"SOLID","DASH","DOT","DASHDOT","DASHDOTDOT","NULL"};
BOOL scCALLBACK ObjPenProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_MEASUREITEM:{
            MEASUREITEMSTRUCT*lpmisCtl = (MEASUREITEMSTRUCT FAR*) lParam;
            lpmisCtl->itemWidth=80;
            lpmisCtl->itemHeight=21;
        }return TRUE;
        case WM_DRAWITEM:{
            DRAWITEMSTRUCT*lpdis = (DRAWITEMSTRUCT *) lParam;
            HGDIOBJ b;
            if(lpdis->itemState&ODS_SELECTED){
                b=CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
            }else b=CreateSolidBrush(GetSysColor(COLOR_WINDOW	));
            HGDIOBJ old=SelectObject(lpdis->hDC,b);
            PatBlt(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,
                   lpdis->rcItem.right-lpdis->rcItem.left,
                   lpdis->rcItem.bottom-lpdis->rcItem.top,PATCOPY
                   );
            SelectObject(lpdis->hDC,old);
            DeleteObject(b);
            POINT pp[5];
            pp[0].x=lpdis->rcItem.left+5;
            pp[1].x=lpdis->rcItem.right-5;
            pp[0].y=pp[1].y=(lpdis->rcItem.top+lpdis->rcItem.bottom)/2;
            Polyline(lpdis->hDC,pp,2);
            if ((int)lpdis->itemID>0){
                int i=lpdis->itemID-1;
                BOOL fill=i>8;
                i%=9;
                double x,y;
                x=arrow_data[i].l*cos(arrow_data[i].a);
                y=arrow_data[i].l*sin(arrow_data[i].a);
                if (lpdis->CtlID==IDC_ARROW2){
                    pp[1].x=lpdis->rcItem.left+5;
                    pp[2].x=pp[0].x=pp[1].x+x;
                }else{
                    pp[1].x=lpdis->rcItem.right-5;
                    pp[2].x=pp[0].x=pp[1].x-x;
                }
                pp[2].y=pp[0].y+y;
                pp[0].y=pp[0].y-y;
                if (fill){
                    SelectObject(lpdis->hDC,GetStockObject(BLACK_BRUSH));
                    Polygon(lpdis->hDC,pp,3);
                }else
                    Polyline(lpdis->hDC,pp,3);
            }
            if(lpdis->itemState&ODS_FOCUS){
                RECT r=lpdis->rcItem;
                r.left+=2;r.right-=2;
                r.top+=2;r.bottom-=2;
                DrawFocusRect(lpdis->hDC,&r);
            }
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDC_PENENABLE: EnablePens(hwnd,IsDlgButtonChecked(hwnd,IDC_PENENABLE));return 0;
            }
        }break;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HOBJ2D pen=(HOBJ2D)b->di.d1;
            INT16 width=0;int style=0;COLORREF color=0;int rop=R2_COPYPEN;
            HWND a1=GetDlgItem(hwnd,IDC_ARROW1),
                    a2=GetDlgItem(hwnd,IDC_ARROW2);
            for(int i=0;i<19;i++){
                SendMessage(a1, CB_ADDSTRING, 0, 0);
                SendMessage(a2, CB_ADDSTRING, 0, 0);
            }
            if(pen){
                HSP2D hsp=b->hsp;
                width=GetPenWidth2d(hsp,pen);
                style=GetPenStyle2d(hsp,pen);
                color=GetPenColor2d(hsp,pen);
                rop=GetPenRop2d(hsp,pen);
                CheckDlgButton(hwnd,IDC_PENENABLE,1);
                HOBJ2D obj=b->iuser1;
                ARROW2D a;
                if (GetVectorArrows2d(hsp,obj,&a)){
                    SetArrow(a1,a.Alength,a.Aangle,a.flags&1);
                    SetArrow(a2,a.Blength,a.Bangle,a.flags&2);
                }else{
                    SendMessage(a1,CB_SETCURSEL,0,0);
                    SendMessage(a2,CB_SETCURSEL,0,0);
                }
            }else{
                if (b->iuser1){
                    EnablePens(hwnd,0);
                }
            }
            SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_SETCOLOR,0,color);
            SetDlgItemInt(hwnd,IDC_WIDTH,width,0);
            EditSubClass(hwnd,IDC_WIDTH,0,256,1,UDF_MINMAX);
            for(i=0;i<sizeof(pen_st)/4;i++)
                SendDlgItemMessage(hwnd,IDC_PENSTYLE,CB_ADDSTRING,0,(LPARAM)pen_st[i]);
            SendDlgItemMessage(hwnd,IDC_PENSTYLE,CB_SETCURSEL,style,0);
            AddROP2(hwnd,IDC_ROP,rop);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                //TPropertyButton*b=(TPropertyButton*)lParam;
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                if (wParam==1){
                    INT16 width;
                    HSP2D hsp=b->hsp;
                    HOBJ2D obj=b->iuser1;
                    HOBJ2D pen=(HOBJ2D)b->di.d1;
                    COLORREF color=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,0);
                    INT16 style=(INT16)SendDlgItemMessage(hwnd,IDC_PENSTYLE,CB_GETCURSEL,0,0);
                    INT16 rop=(INT16)(SendDlgItemMessage(hwnd,IDC_ROP,CB_GETCURSEL,0,0)+1);
                    if (IsDlgButtonChecked(hwnd,IDC_PENENABLE)){
                        if(!IsIntValid(hwnd,IDC_WIDTH,width,0,256)){b->Invalid();return TRUE;}

                        if (!pen || (GetToolRef2d(hsp,PEN2D,pen)>1)){
                            pen=CreatePen2d(hsp,style,width,color,rop);
                            if (obj) SetPenObject2d(hsp,obj,pen);
                        }else{
                            SetPenStyle2d(hsp,pen,style);
                            SetPenColor2d(hsp,pen,color);
                            SetPenWidth2d(hsp,pen,width);
                            SetPenRop2d(hsp,pen,rop);
                        }
                        SetCurrentPen2d(hsp,pen);
                    }else{
                        if (obj && pen)
                        {
                            SetPenObject2d(hsp,obj,0);
                        }
                        DeletePen2d(hsp,pen);
                    }
                    ARROW2D a;
                    memset(&a,0,sizeof(a));
                    if (GetArrow((int)SendDlgItemMessage(hwnd,IDC_ARROW1,CB_GETCURSEL,0,0),a.Alength,a.Aangle))
                        a.flags|=1;
                    if (GetArrow((int)SendDlgItemMessage(hwnd,IDC_ARROW2,CB_GETCURSEL,0,0),a.Blength,a.Bangle))
                        a.flags|=2;
                    ARROW2D a2;
                    if (GetVectorArrows2d(hsp,obj,&a2)
                        && (a.Alength==a2.Alength)
                        && (a.Blength==a2.Blength)
                        && (a.Aangle==a2.Aangle)
                        && (a.Bangle==a2.Bangle)
                        && (a.flags==a2.flags)
                        ){;
                    }else{
                        SetVectorArrows2d(hsp,obj,&a);
                    }
                }
            }
            b->Valid();
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
void EnableBrushs(HWND HWindow,int e){
    EnableWindow(GetDlgItem(HWindow,IDC_BRUSHSTYLE),e);
    EnableWindow(GetDlgItem(HWindow,IDC_COLOR),e);
    EnableWindow(GetDlgItem(HWindow,IDC_HATCH),e);
    EnableWindow(GetDlgItem(HWindow,IDC_ROP),e);
    EnableWindow(GetDlgItem(HWindow,IDC_BROWSE),e);
}
void _EnableBrushs(HWND HWindow,int style){
    EnableWindow(GetDlgItem(HWindow,IDC_COLOR),style!=BS_PATTERN);
    EnableWindow(GetDlgItem(HWindow,IDC_BROWSE),style==BS_PATTERN);
    EnableWindow(GetDlgItem(HWindow,IDC_HATCH),style==BS_HATCHED);
}
typedef struct tag_LOGSTYLE{INT16 code;char *text;}_LOGSTYLE;
#pragma  +wpin
static _LOGSTYLE _lbs[]=
{{BS_NULL		,"NULL"},
 {BS_SOLID 		,"SOLID"},
 {BS_HATCHED	,"HATCED"},
 {BS_PATTERN	,"PATTERN"}
};
static _LOGSTYLE _lhs[]={
    {HS_HORIZONTAL,"HORIZONTAL"},
    {HS_VERTICAL  ,"VERTICAL"},
    {HS_FDIAGONAL ,"FDIAGONAL"},
    {HS_BDIAGONAL ,"BDIAGONAL"},
    {HS_CROSS     ,"CROSS"},
    {HS_DIAGCROSS ,"DIAGCROSS"}
};
void AddLogStyle(HWND hDlg,int id,_LOGSTYLE* ls,int n)
{for(int i=0;i<n;i++)
    {SendDlgItemMessage(hDlg,id,CB_ADDSTRING,0,(LPARAM)ls[i].text);
    }
}

BOOL scCALLBACK ObjBrushProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDC_BROWSE:{
                    TBrushTProperty*b=(TBrushTProperty*)GetDlgPropertyButton(hwnd);
                    if(b){
                        char s[256]="";
                        if(FileDialog(hwnd,s,IDS_BMP,FD_ASLINK)){
                            HOBJ2D hd=ImportBMP(b->hsp,s);
                            openaslink=0;
                            if(hd){
                                if(b->hdib){DeleteDib2d(b->hsp,b->hdib);b->hdib=0;}
                                b->hdib=hd;
                                HGLOBAL hg=GetDib2d(b->hsp,b->hdib);
                                SendDlgItemMessage(hwnd,IDC_BRUSHDIB,BMP_SETDIB,(WPARAM)hg,0);
                            }
                        }
                    }}return TRUE;
                case IDC_BRUSHENABLE: EnableBrushs(hwnd,IsDlgButtonChecked(hwnd,IDC_BRUSHENABLE));return TRUE;
                case IDC_BRUSHSTYLE: {
                    int style=(int)SendDlgItemMessage(hwnd,IDC_BRUSHSTYLE,CB_GETCURSEL,0,0);
                    style=_lbs[style].code;
                    _EnableBrushs(hwnd,style);
                }return TRUE;
            }
        }break;
        case PSP_INITPAGE:{
            TBrushTProperty*b=(TBrushTProperty*)lParam;
            HOBJ2D brush=(HOBJ2D)b->di.d1;
            //		int hatch=0;
            INT16 style=BS_SOLID;COLORREF color=RGB(255,255,255);int rop=R2_COPYPEN;
            INT16 hatch=0;
            if(brush){
                HSP2D hsp=b->hsp;
                //		hatch=GetBrushHatch2d(hsp,brush);
                style=GetBrushStyle2d(hsp,brush);
                color=GetBrushColor2d(hsp,brush);
                rop=GetBrushRop2d(hsp,brush);
                hatch=GetBrushHatch2d(hsp,brush);
                HOBJ2D hdib=GetBrushDibIndex2d(hsp,brush);
                if(hdib){
                    HGLOBAL hg=GetDib2d(hsp,hdib);
                    SendDlgItemMessage(hwnd,IDC_BRUSHDIB,BMP_SETDIB,(WPARAM)hg,0);
                }
                CheckDlgButton(hwnd,IDC_BRUSHENABLE,1);
                if (b->iuser1 && (GetObjectType2d(hsp,b->iuser1)==otRGROUP2D))
                    EnableWindow(GetDlgItem(hwnd,IDC_BRUSHENABLE),0);
                _EnableBrushs(hwnd,style);
            }else{
                if (!brush){ EnableBrushs(hwnd,0); }
            }
            SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_SETCOLOR,0,color);
            AddLogStyle(hwnd,IDC_BRUSHSTYLE,_lbs,4);
            int i;
            for(i=0;i<4;i++){
                if (_lbs[i].code==style){
                    SendDlgItemMessage(hwnd,IDC_BRUSHSTYLE,CB_SETCURSEL,i,0);break;
                }
            }
            AddLogStyle(hwnd,IDC_HATCH,_lhs,6);
            for(i=0;i<6;i++){
                if (_lhs[i].code==hatch){
                    SendDlgItemMessage(hwnd,IDC_HATCH,CB_SETCURSEL,i,0);break;
                }
            }
            AddROP2(hwnd,IDC_ROP,rop);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TBrushTProperty*b=(TBrushTProperty*)lParam;
                if(b->hdib){
                    DeleteDib2d(b->hsp,b->hdib);b->hdib=0;
                }
            };return TRUE;
        case PSP_VALID:{
            TBrushTProperty*b=(TBrushTProperty*)lParam;
            if (b->flags&PSP_INITIALIZE){
                if (wParam==1){
                    HSP2D hsp=b->hsp;
                    HOBJ2D obj=b->iuser1;
                    HOBJ2D brush=(HOBJ2D)b->di.d1;
                    HOBJ2D dib=brush?GetBrushDibIndex2d(hsp,brush):0;

                    COLORREF color=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,0);
                    INT16 style=(INT16)SendDlgItemMessage(hwnd,IDC_BRUSHSTYLE,CB_GETCURSEL,0,0);
                    style=_lbs[style].code;
                    INT16 hatch=(INT16)SendDlgItemMessage(hwnd,IDC_HATCH,CB_GETCURSEL,0,0);;
                    hatch=_lhs[hatch].code;

                    INT16 rop=(INT16)(SendDlgItemMessage(hwnd,IDC_ROP,CB_GETCURSEL,0,0)+1);

                    if (IsDlgButtonChecked(hwnd,IDC_BRUSHENABLE)){

                        if (!brush || (GetToolRef2d(hsp,BRUSH2D,brush)>1)){
                            brush=CreateBrush2d(hsp,style,hatch,color,(style==BS_PATTERN)?((b->hdib)?b->hdib:dib):0,rop);

                            if (obj) SetBrushObject2d(hsp,obj,brush);
                            else SetBkBrush2d(hsp,brush);
                        }else{
                            SetBrushStyle2d(hsp,brush,style);
                            SetBrushColor2d(hsp,brush,color);
                            SetBrushHatch2d(hsp,brush,hatch);
                            SetBrushRop2d(hsp,brush,rop);
                            if(style==BS_PATTERN){
                                if(b->hdib){SetBrushDibIndex2d(hsp,brush,b->hdib);b->hdib=0;}else dib=0;
                            }else {SetBrushDibIndex2d(hsp,brush,0);}
                            if(dib)DeleteDib2d(b->hsp,dib);
                        }
                        SetCurrentBrush2d(hsp,brush);
                    }else{
                        // Отменили кисть

                        if (brush){
                            if (obj) SetBrushObject2d(hsp,obj,0);
                            else SetBkBrush2d(hsp,0);
                        }
                        DeleteBrush2d(hsp,brush);
                        if(dib)DeleteDib2d(b->hsp,dib);
                    }
                }
            }
            b->Valid();
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

TBrushTProperty::TBrushTProperty(int ids,int  _helpctx):
    TPropertyButton(ids,HResource,"Obj_Brush",_helpctx){
    Procedure=ObjBrushProc;
    hdib=0;
};


BOOL scCALLBACK ObjPointsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case TBL_GETTEXT:{
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info){
                if (info->id==IDC_HLABEL && info->pos>=0 && info->pos<2){

                    lstrcpy(info->text,info->pos?"Y":"X");
                    return TRUE;
                }
                if (info->id==IDC_VLABEL){
                    wsprintf(info->text,"%d",info->pos);
                }

            }};break;

        case TBN_GETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            HSP2D   hsp=b->hsp;
            HOBJ2D object=b->iuser1;
            POINT2D point;
            GetVectorPoint2d(hsp,object,(INT16)ci->y,&point);
            switch(ci->x){
                case 0:sprintf(ci->text,"%g",point.x); break;
                case 1:sprintf(ci->text,"%g",point.y); break;
            }
        };return 0;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D     hsp=b->hsp;
            HOBJ2D object=b->iuser1;
            HWND hl=GetDlgItem(hwnd,IDC_HLABEL);
            HWND vl=GetDlgItem(hwnd,IDC_VLABEL);
            HWND br=GetDlgItem(hwnd,IDC_POINTLIST);
            SendMessage(br,TB_SETLABEL,(WPARAM)hl,(LPARAM)vl);

            TCollumn c[2]={TCollumn(0,100,CF_READONLY),TCollumn(0,100,CF_READONLY)};

            for(int i=0;i<2;i++){
                c[i]._min/=2;c[i]._max*=2;
                SendMessage(br,TB_ADDCOLLUMN,-1,LPARAM(&(c[i])));
            }
            i=GetVectorNumPoints2d(hsp,object);
            SendMessage(br,TB_SETRANGEY,0,i);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                // TPropertyButton*b=(TPropertyButton*)lParam;
            };return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};


BOOL GetObjectTypeName2d(INT16 type,char*s,int size)
{
    UINT16 types[]={otGROUP,otGROUP2D,otRGROUP2D,otGROUP3D,otCAMERA3D,otLIGHT3D,otOBJECT3D,otLINE2D,
                    otBITMAP2D,otDOUBLEBITMAP2D,otTEXT2D,otVIEW3D2D,otEDITFRAME2D,otROTATECENTER2D,
                    otFRAME3D,otAXIS3D,otCONTROL2D,otUSEROBJECT2D};
    for(INT16 i=0;i<sizeof(types)/sizeof(types[0]);i++){
        if(type==types[i])break;
    }
    return  LoadString(HResource,IDS_OBJTYPES2D+i,s,size);
}
void AddVarItem(PClass _class,TCollection*items,char *vn,char*lv){
    if(lv && vn && lv[0] && vn[0]){
        INT16 id=_class->GetVarIdByName(vn,FALSE);
        if(id>-1){
            items->AtPut(id,NewStr(lv));
        }}
};

void GetBmpString(LPBITMAPINFOHEADER info,char*ps); //in Twindow.cpp
struct TCLASS_ITEMS{
    PClass _class;
    TCollection * items;
};
BOOL scCALLBACK ObjVarsProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case TBL_GETTEXT:{
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info){
                if (info->id==IDC_HLABEL && info->pos>=0 && info->pos<2){
                    lstrcpy(info->text,info->pos?"External var":"This class var");
                    return TRUE;
                }
            }};break;
        case TBN_SETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            int l=lstrlen(ci->text);
            for(int i=0;i<l;i++){
                if(ci->text[i]==';' || ci->text[i]==','){
                    MessageBox(hwnd,"Invalid chars \";\",\",\" in string","Input error",MB_ICONSTOP|MB_OK);
                    return TRUE;
                }
            }

            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            PClass _class=((TCLASS_ITEMS*)b->pdata)->_class;
            TCollection *items=((TCLASS_ITEMS*)b->pdata)->items;
            if(items->At((C_TYPE)ci->y)){delete items->At((C_TYPE)ci->y);items->AtPut((C_TYPE)ci->y,NULL);}
            if(ci->text && ci->text[0]){
                items->AtPut((C_TYPE)ci->y,NewStr(ci->text));
            }
        };break;
        case TBN_GETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            PClass _class=((TCLASS_ITEMS*)b->pdata)->_class;
            TCollection *items=((TCLASS_ITEMS*)b->pdata)->items;
            switch(ci->x){
                case 0:{
                    if(_class->vars[ci->y].info)
                        wsprintf(ci->text,"%s (%s)",_class->vars[ci->y].name,_class->vars[ci->y].info);
                    else
                        wsprintf(ci->text,"%s",_class->vars[ci->y].name);
                };break;//sprintf(ci->text,"%g",point.x); break;
                case 1:{
                    if(items->At(ci->y))lstrcpy(ci->text,(char*)(items->At(ci->y)));else
                        ci->text[0]=0;
                };//sprintf(ci->text,"%g",point.y); break;
            }
        };return 0;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            PClass _class=((TCLASS_ITEMS*)b->pdata)->_class;
            TCollection *items=new TCollection(_class->varcount,0);
            for(C_TYPE j=0;j<_class->varcount;j++)  items->Insert(NULL);
            ((TCLASS_ITEMS*)b->pdata)->items=items;
            INT16 _size=GetObjectData2d(hsp,obj,UD_VARS,NULL,0,0);
            char *str=NULL;
            if(_size){
                str=new char[_size+1];
                str[_size]=0;
                GetObjectData2d(hsp,obj,UD_VARS,str,0,_size);
                char *ps=str;
                char *vn=str;
                char *lv=NULL;
                while(*ps){
                    if(*ps==';'){
                        *ps=0;
                        AddVarItem(_class,items,vn,lv);
                        vn=ps+1;}
                    if(*ps==','){*ps=0;lv=ps+1;}
                    ps++;
                }
                AddVarItem(_class,items,vn,lv);
            };
            if(str)delete str;
            HWND hl=GetDlgItem(hwnd,IDC_HLABEL);
            HWND br=GetDlgItem(hwnd,IDC_ILINKVARS);
            SendMessage(br,TB_SETLABEL,(WPARAM)hl,(LPARAM)0);

            TCollumn c[2]={TCollumn(0,200,CF_READONLY),TCollumn(0,100,0)};

            for(int i=0;i<2;i++){
                c[i]._min/=2;c[i]._max*=2;
                SendMessage(br,TB_ADDCOLLUMN,-1,LPARAM(&(c[i])));
            }

            SendMessage(br,TB_SETRANGEY,0,_class->varcount);

        }return TRUE;
        case PSP_CLOSE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            PClass _class=((TCLASS_ITEMS*)b->pdata)->_class;
            TCollection *items=((TCLASS_ITEMS*)b->pdata)->items;
            if(wParam==1){

                HSP2D hsp=b->hsp;
                HOBJ2D obj=b->iuser1;

                char s[256];
                int _size=0;
                for(int i=0;i<_class->varcount;i++){
                    if(items->At(i)){
                        _size+=lstrlen(_class->vars[i].name)+1;
                        _size+=lstrlen((char*)(items->At(i)))+1;
                    }
                }
                char *str=new char[_size];
                str[0]=0;
                for(i=0;i<_class->varcount;i++){
                    if(items->At(i)){
                        if(str[0])lstrcat(str,";");
                        lstrcat(str,_class->vars[i].name);
                        lstrcat(str,",");
                        lstrcat(str,(char*)items->At(i));
                    }
                }
                SetObjectData2d(hsp,obj,UD_VARS,str,0,lstrlen(str)+1);
                delete str;
            }
            if(items){
                for(C_TYPE i=0;i<items->count;i++){
                    char *ps=(char*)items->At(i);
                    if(ps)delete ps;
                }
                delete items;
            }
        };return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}

BOOL scCALLBACK ObjInfoProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            char s[2048]="";
            INT16 type=GetObjectType2d(hsp,obj);
            char pps[256];
            GetObjectTypeName2d(type,pps,sizeof(pps));

            //lstrcpy(s,ps);lstrcat(s,"\n ");
            char cmd[256];
            LoadString(HResource,IDS_str123,cmd,sizeof(cmd));
            sprintf(s,cmd,pps,obj);

            if((type!=otGROUP3D)&&(type!=otGROUP2D)&&(type!=otRGROUP2D)){
                INT16 zp=GetZOrder2d(hsp,obj);
                LoadString(HResource,IDS_str124,cmd,sizeof(cmd));
                wsprintf(s+lstrlen(s),cmd ,zp);
            }

            if (type==otOBJECT3D||type==otGROUP3D){
                INT32 numvertex=GetNumPoints3d((HSP3D)hsp,obj);
                INT32 numprim=GetNumPrimitives3d((HSP3D)hsp,obj);
                wsprintf(s+lstrlen(s),"Vertexes: %ld\r\nPrimitives: %ld\r\n",numvertex,numprim);
            };
            if(type==otCONTROL2D){
                HWND control=GetControl2d(hsp,obj);
                DWORD style=GetWindowLong(control,GWL_STYLE);
                char classname[64];
                GetClassName(control,classname,sizeof(classname));
                wsprintf(s+lstrlen(s),"Class:%s\r\nStyle: %ld\r\n",classname,style);
            }
            //"Nude" +art+ +"female" -Sculpture
            char *_ps=s+lstrlen(s);
            switch(type){
                case otVIEW3D2D:{
                    HSP3D _hsp3d=Get3dSpace2d(hsp,obj);
                    INT32  np=GetNumSpacePoints3d(_hsp3d);
                    INT32  np_=GetNumSpacePrimitives3d(_hsp3d);
                    char cmd[256];
                    LoadString(HResource,IDS_str122,cmd,sizeof(cmd));
                    sprintf(s+lstrlen(s),cmd,_hsp3d,np,np_);
                }break;
                case otBITMAP2D:{
                    HOBJ2D hd=GetDibObject2d(hsp,obj);
                    HGLOBAL hg=GetDib2d(hsp,hd);
                    if(hg){
                        BITMAPINFOHEADER *bh=(BITMAPINFOHEADER *)GlobalLock(hg);
                        GetBmpString(bh,_ps);
                        GlobalUnlock(hg);
                    }else{
                        lstrcpy(_ps,"No image available");
                    }
                }break;
                case otDOUBLEBITMAP2D:{
                    HOBJ2D hd=GetDoubleDibObject2d(hsp,obj);
                    HGLOBAL hg=GetXorDib2d(hsp,hd);
                    BITMAPINFOHEADER *bh=(BITMAPINFOHEADER *)GlobalLock(hg);
                    GetBmpString(bh,_ps);
                    GlobalUnlock(hg);
                }break;
                case otLINE2D:{
                    int pc=GetVectorNumPoints2d(hsp,obj);
                    LoadString(HResource,IDS_str125,cmd,sizeof(cmd));
                    wsprintf(pps,cmd,pc);
                    if(pc){
                        POINT2D * p=new POINT2D[pc];
                        GetVectorPoints2d(hsp,obj,p,pc);
                        double l=0;
                        for(INT16 i=1;i<pc;i++){
                            double dx=(p[i-1].x)-(p[i].x);
                            double dy=(p[i-1].y)-(p[i].y);
                            double _l=sqrt(dx*dx+dy*dy);
                            l+=_l;
                        }
                        LoadString(HResource,IDS_str126,cmd,sizeof(cmd));
                        sprintf(s+lstrlen(s),cmd,l);
                        delete p;
                    }

                };break;
            };
            SetWindowText(GetDlgItem(hwnd,IDC_INFO),s);
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
            };return TRUE;
        case PSP_VALID:{
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK ObjVideoProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            FRAMEPROP fp;
            GetFrameProp2d(hsp,obj,&fp);
            VIDEOPROP VP;
            GetVideoProp(fp.video,&VP);

            if (fp.flags&V2_TRANSPARENT){
                CheckDlgButton(hwnd,IDC_TRANSPARENT,1);
                SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_SETCOLOR,0,fp.color);
            }else EnableWindow(GetDlgItem(hwnd,IDC_COLOR),0);

            CheckDlgButton(hwnd,IDC_VIDEOCLOSE,(fp.flags&V2_AUTODELETE)!=0);
            CheckDlgButton(hwnd,IDC_SYNC,(fp.flags&V2_SYNC)!=0);
            CheckDlgButton(hwnd,IDC_AUTOPLAY,(fp.flags&V2_AUTOPLAY)!=0);
            CheckDlgButton(hwnd,IDC_VIDEOCYCLE,(fp.flags&V2_CYCLED)!=0);

            EditSubClass(hwnd,IDC_COLORSENSE,0,255,1,UDF_MINMAX);
            SetDlgItemInt(hwnd,IDC_COLORSENSE,fp.sense,0);

            HV2D hv=0;
            int i=0;
            while((hv=GetNextVideo(hv))!=0){
                VIDEOPROP vp;
                GetVideoProp(hv,&vp);
                SendDlgItemMessage(hwnd,IDC_VIDEOS,CB_ADDSTRING,0,(LPARAM)vp.file);
                if (hv==fp.video)
                    SendDlgItemMessage(hwnd,IDC_VIDEOS,CB_SETCURSEL,i,0);
                i++;
            }
            EditSubClass(hwnd,IDC_VIDEOPOS,VP.start,VP.end,1,UDF_MINMAX);
            SetDlgItemInt(hwnd,IDC_VIDEOPOS,fp.pos,1);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch (command){
        case IDC_DETAIL:{
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    HSP2D hsp=b->hsp;
                    HOBJ2D obj=b->iuser1;
                    FRAMEPROP fp;
                    GetFrameProp2d(hsp,obj,&fp);
                    VideoInfoDialog(hwnd,fp.video);
                };return 0;
                case IDC_TRANSPARENT:
                    EnableWindow(GetDlgItem(hwnd,IDC_COLOR),IsDlgButtonChecked(hwnd,IDC_TRANSPARENT));
            }}break;;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                HSP2D hsp=b->hsp;
                HOBJ2D obj=b->iuser1;
                FRAMEPROP fp,fp1;
                GetFrameProp2d(hsp,obj,&fp1);
                fp=fp1;
                BOOL tr=IsDlgButtonChecked(hwnd,IDC_TRANSPARENT);
                if (tr) fp.flags|=V2_TRANSPARENT;else fp.flags&=~V2_TRANSPARENT;

                BOOL sy=IsDlgButtonChecked(hwnd,IDC_SYNC);
                BOOL cl=IsDlgButtonChecked(hwnd,IDC_VIDEOCLOSE);

                if (sy) fp.flags|=V2_SYNC;else fp.flags&=~V2_SYNC;
                if (cl) fp.flags|=V2_AUTODELETE;else fp.flags&=~V2_AUTODELETE;

                BOOL ap=IsDlgButtonChecked(hwnd,IDC_AUTOPLAY);
                BOOL cy=IsDlgButtonChecked(hwnd,IDC_VIDEOCYCLE);
                if(ap) fp.flags|=V2_AUTOPLAY;else fp.flags&=~V2_AUTOPLAY;
                if(cy) fp.flags|=V2_CYCLED;else fp.flags&=~V2_CYCLED;

                fp.color=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,fp.color);
                INT16 d1=30;
                if(IsIntValid(hwnd,IDC_COLORSENSE,d1,0,255)){fp.sense=d1; }
                BOOL t;
                fp.pos=GetDlgItemInt(hwnd,IDC_VIDEOPOS,&t,1);

                int i=(int)SendDlgItemMessage(hwnd,IDC_VIDEOS,CB_GETCURSEL,0,0);
                char vf[260];
                SendDlgItemMessage(hwnd,IDC_VIDEOS,CB_GETLBTEXT,i,(LPARAM)vf);
                HV2D hv=0;

                while((hv=GetNextVideo(hv))!=0){
                    VIDEOPROP vp;
                    GetVideoProp(hv,&vp);
                    if(!lstrcmpi(vp.file,vf)){
                        fp.video=hv;break;
                    }
                }
                if (fp.flags != fp1.flags || fp.color != fp1.color ||
                    (fp.video != fp1.video) || (fp.sense!=fp1.sense)||
                    (fp.video!=fp1.video))
                    SetFrameProp2d(hsp,obj,&fp);
                if(fp.pos!=fp1.pos)FrameSetPos2d(hsp,obj,fp.pos);
            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                INT16 d1=30;
                if(IsIntValid(hwnd,IDC_COLORSENSE,d1,0,255))b->Valid();else b->Invalid();
            }}return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK ObjBmpRefProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{
            EV_COMMAND
                    switch(command){
        case IDC_BROWSE:{
                    TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    if(b){
                        HWND edit=GetDlgItem(hwnd,IDC_REFFILE);
                        char s[256]="";
                        GetDlgItemText(hwnd,IDC_REALFILE,s,sizeof(s));
                        if(FileDialog(hwnd,s,IDS_BMP,0)){
                            {
                                char base[256];
                                char relative[256];
                                if(GetTexturePath2d(b->hsp,base)){
                                    if(GetRelativePath(base,s,relative)){
                                        SetWindowText(edit,relative);
                                    }
                                }

                                SendMessage(edit,EM_SETMODIFY,1,0);
                            }}}
                }return TRUE;
            }}break;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            int type=GetObjectType2d(hsp,obj);
            char s[256];
            char realfile[256];
            INTERNAL2D in;
            in.hSpace2d=hsp;
            in.what=10;
            in.data1=(DWORD)realfile;
            in.data2=sizeof(realfile);

            if (type==otBITMAP2D){
                HOBJ2D dib=GetDibObject2d(hsp,obj);
                GetRDib2d(hsp,dib,s,sizeof(s));
                in.tooltype=DIB2D;
                in.hobject=dib;
            }else{
                HOBJ2D dib=GetDoubleDibObject2d(hsp,obj);
                GetRDoubleDib2d(hsp,dib,s,sizeof(s));
                in.hobject=dib;
                in.tooltype=DOUBLEDIB2D;
            }
            SetDlgItemText(hwnd,IDC_REFFILE,s);
            if(GetInternal2d(&in)){
                if(realfile[0]){
                    ShowWindow(GetDlgItem(hwnd,IDC_REFICON),SW_HIDE);
                }else
                {
                    char s[128];
                    LoadString(HResource,IDS_str222,s,sizeof(s));
                    SetDlgItemText(hwnd,IDC_REFWARNING,s); }
                SetDlgItemText(hwnd,IDC_REALFILE,realfile);
            }
        }return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                if (wParam==1){
                    HSP2D hsp=b->hsp;
                    HOBJ2D obj=b->iuser1;
                    int type=GetObjectType2d(hsp,obj);

                    HWND edit=GetDlgItem(hwnd,IDC_REFFILE);
                    if(SendMessage(edit,EM_GETMODIFY,0,0)){
                        char s[256];
                        GetWindowText(edit,s,sizeof(s));
                        if (type==otBITMAP2D){
                            HOBJ2D dib=GetDibObject2d(hsp,obj);
                            SetRDib2d(hsp,dib,s);
                        }else{
                            HOBJ2D dib=GetDoubleDibObject2d(hsp,obj);
                            SetRDoubleDib2d(hsp,dib,s);
                        }
                    }
                }
            }
        }return TRUE;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);

}
BOOL scCALLBACK ObjBmpProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    RECT static oldrect;
    switch(msg){
        case PSP_INITPAGE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            int type=GetObjectType2d(hsp,obj);
            TViewDIB*win=(TViewDIB*)GetWindowLong(GetDlgItem(hwnd,IDC_BMP),0);
            RECT r;
            POINT2D o,s;
            BOOL frame=IsVideoFrame2d(hsp,obj);
            if (frame)
            {
                FRAMEPROP fp;
                GetFrameProp2d(hsp,obj,&fp);
                r=fp.src;
            }
            else
            {
                GetBitmapSrcRect(hsp,obj,&o,&s);
                r.left=o.x;r.top=o.y;
                r.right=o.x+s.x;
                r.bottom=o.y+s.y;
            }
            oldrect=r;
            if (win)
            {
                win->NewDib(hsp,obj,type==otBITMAP2D?DIB2D:DOUBLEDIB2D,(HOBJ2D)b->di.d1);
                win->SetRect(r);
                //      State2dSaveAs(win->hsp,"c:\\1.vdr");
            }
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                //TPropertyButton*b=(TPropertyButton*)lParam;
            };return TRUE;
        case PSP_VALID:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE)
            {
                if (wParam==1)
                {
                    HSP2D hsp=b->hsp;
                    HOBJ2D obj=b->iuser1;
                    TViewDIB*win=(TViewDIB*)GetWindowLong(GetDlgItem(hwnd,IDC_BMP),0);
                    RECT r=win->GetRect();
                    if (r.left!=oldrect.left ||r.right!=oldrect.right ||
                        r.top !=oldrect.top || r.bottom != oldrect.bottom)
                    {
                        POINT2D o,s,_o,_s;
                        BOOL frame=IsVideoFrame2d(hsp,obj);
                        FRAMEPROP fp;
                        if (frame)
                        {
                            GetFrameProp2d(hsp,obj,&fp);
                            _o.x=fp.src.left;_o.y=fp.src.top;
                            _s.x=fp.src.right-fp.src.left;_s.y=fp.src.bottom-fp.src.top;
                            fp.src=r;
                        }
                        else
                        {
                            GetBitmapSrcRect(hsp,obj,&_o,&_s);}
                        o.x=r.left;o.y=r.top;
                        s.x=r.right-r.left;s.y=r.bottom-r.top;
                        if (_s.x!=s.x || _s.y !=s.y || _o.x!=o.x || _o.y!=o.y)
                            if (frame)SetFrameProp2d(hsp,obj,&fp);
                            else	SetBitmapSrcRect(hsp,obj,&o,&s);
                        //SetObjectSize2d(hsp,obj,&s);
                    }
                }
            }else {b->Valid();}
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

int ChoseFontDialog(HWND parent,LOGFONT& lf,char * /*title*/){
    CHOOSEFONT cf;
    memset(&cf, 0, sizeof(CHOOSEFONT));
    cf.lStructSize = sizeof(CHOOSEFONT);
    cf.hwndOwner = parent;
    cf.lpLogFont = &lf;
    cf.Flags = CF_SCREENFONTS | CF_EFFECTS|CF_INITTOLOGFONTSTRUCT;
    cf.rgbColors = RGB(0,0,0);
    cf.nFontType = SCREEN_FONTTYPE;
    return ChooseFont(&cf);
}

int scCALLBACK EnumFontFamProc1(const ENUMLOGFONT FAR*lpnlf,
                                const NEWTEXTMETRIC FAR*tm,int FontType, LPARAM  lParam)
{
    HWND combo=(HWND)lParam;
    if(tm->tmPitchAndFamily&TMPF_TRUETYPE)
    {
        SendMessage(combo,CB_ADDSTRING,0,(LPARAM)lpnlf->elfLogFont.lfFaceName);
    }
    return TRUE;
};

void __SetFont(HWND hwnd,LOGFONT2D&lf,BYTE f=7)
{
    HWND combo=GetDlgItem(hwnd,IDC_TEXT);
    if(f&1)
    {
        HGDIOBJ hf=(HGDIOBJ)SendMessage(combo,WM_GETFONT,0,0);
        SendMessage(combo,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
        DeleteObject(hf);
    }
    if(f&2)
    {
        HFONT hf=CreateFontIndirect((LOGFONT*)&(lf.lfHeight));
        SendMessage(combo,WM_SETFONT,(WPARAM)hf,0);
    }
    if(f&4)
    {
        InvalidateRect(combo,NULL,TRUE);
    }
};

int __GetFont1(char *s,BOOL &er)
{
    int i;
    if(strstr(s,"pt"))
    {
        if(sscanf(s,"%dpt",&i))return i;
    }
    else
    {
        if (sscanf(s,"%d",&i))return -i;
    }
    er=TRUE;
    return 0;
}

BOOL __GetFont(char *s,LOGFONT2D &lf)
{
    int i1=0,i2=0;
    BOOL error=0;
    char *ps;
    if(ps=strstr(s,"/"))
    {
        char s1[256];
        lstrcpyn(s1,s,ps-s+1);
        i1=__GetFont1(s1,error);
        i2=__GetFont1(ps+1,error);
    }
    else
    {
        i1=__GetFont1(s,error);
    }
    if(!error)
    {
        lf.lfHeight=i1;
        lf.lfWidth=i2;
    }
    return !error;
}

#ifndef NOUSERICHEDIT
#ifdef WIN32

static DWORD CALLBACK RtfStreamIn(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    *pcb=((PStream)dwCookie)->Read(pbBuff,cb);
    return 0;
};

static DWORD CALLBACK RtfStreamOut(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    *pcb=((PStream)dwCookie)->Write(pbBuff,cb);
    return 0;
};

void GetSelData(HWND hwnd)
{
    CHARFORMAT2 cf; // This is defined in RICHEDIT.H
    cf.cbSize = sizeof(cf);
    cf.dwMask      = CFM_ALL;
    HWND hRTFWnd=GetDlgItem(hwnd,IDC_TEXT);
    SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

    HWND combo=GetDlgItem(hwnd,IDC_FONTNAME2D);
    int i=(int)SendMessage(combo,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)cf.szFaceName);
    if(i>-1)	SendMessage(combo,CB_SETCURSEL,i,0);
    else 			SetWindowText(combo,cf.szFaceName);

    char s[64];
    double scale=((double)cf.yHeight)/20.0;
    sprintf(s,"%g",scale);

    combo=GetDlgItem(hwnd,IDC_FONTSIZE2D);
    i=(int)SendMessage(combo,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)cf.szFaceName);
    if(i>-1)	SendMessage(combo,CB_SETCURSEL,i,0);
    else 			SetWindowText(combo,s);

    PressControlButton(CM_TEXTBOLD     ,(cf.dwEffects&CFE_BOLD  )!=0);
    PressControlButton(CM_TEXTITALIC   ,(cf.dwEffects&CFE_ITALIC)!=0);
    PressControlButton(CM_TEXTUNDERLINE,(cf.dwEffects&CFE_UNDERLINE)!=0);
    PressControlButton(CM_TEXTSTRIKE   ,(cf.dwEffects&CFE_STRIKEOUT)!=0);
};

BOOL scCALLBACK ObjRtfProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg){
        case WM_NOTIFY:
        {
            int idCtrl = (int) wParam;
            switch(idCtrl){
                case IDC_TEXT:
                {
                    SELCHANGE*sc=(SELCHANGE*)lParam;
                    if (sc->nmhdr.code==EN_SELCHANGE)
                    {
                        GetSelData(hwnd);
                    }
                }break;
            }
        };
        case WM_COMMAND:
        {
            EV_COMMAND
                    switch(command){
        case CM_TEXTBKCOLOR:
                case CM_TEXTFGCOLOR:
                case CM_TEXTBOLD     :
                case CM_TEXTITALIC   :
                case CM_TEXTUNDERLINE:
                case CM_TEXTSTRIKE   :
                {
                    CHARFORMAT2 cf; // This is defined in RICHEDIT.H
                    memset(&cf,0,sizeof(cf));
                    cf.cbSize = sizeof(cf);
                    HWND hRTFWnd=GetDlgItem(hwnd,IDC_TEXT);
                    switch(command){
                        case CM_TEXTBKCOLOR:
                        {
                            cf.dwMask      = CFM_BACKCOLOR;
                            SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);
                            RECT r;
                            GetWindowRect(GetDlgItem(hwnd,IDC_TOOLBARrtf),&r);
                            POINT p;p.x=r.left+80;p.y=r.bottom;
                            if(cf.dwEffects&CFE_AUTOBACKCOLOR)
                                cf.crBackColor=0x01000000L;
                            if(!ChoseColor(hwnd,p,cf.crBackColor,(DWORD)hwnd,5))
                                return 0;
                            if(cf.crBackColor&0x01000000L)
                            {
                                cf.dwEffects|=CFE_AUTOBACKCOLOR;
                                cf.crBackColor=0;
                            }
                            else
                            {
                                cf.dwEffects&=~CFE_AUTOBACKCOLOR;
                            }
                            cf.dwMask    = CFM_BACKCOLOR;
                        }break;
                        case CM_TEXTFGCOLOR:
                        {
                            cf.dwMask      = CFM_COLOR;
                            SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);
                            RECT r;
                            GetWindowRect(GetDlgItem(hwnd,IDC_TOOLBARrtf),&r);
                            POINT p;p.x=r.left+64;p.y=r.bottom;
                            if(!ChoseColor(hwnd,p,cf.crTextColor,(DWORD)hwnd,1))return 0;
                            cf.dwEffects&=~CFE_AUTOCOLOR;
                            cf.dwMask      = CFM_COLOR;
                        }break;
                        case CM_TEXTBOLD     :cf.dwMask = CFM_BOLD;cf.dwEffects=IsControlButtonPressed(command)?CFE_BOLD:0;break;
                        case CM_TEXTITALIC   :cf.dwMask = CFM_ITALIC;cf.dwEffects=IsControlButtonPressed(command)?CFE_ITALIC	:0;break;
                        case CM_TEXTUNDERLINE:cf.dwMask = CFM_UNDERLINE;cf.dwEffects=IsControlButtonPressed(command)?CFE_UNDERLINE:0;break;
                        case CM_TEXTSTRIKE   :cf.dwMask = CFM_STRIKEOUT ;cf.dwEffects=IsControlButtonPressed(command)?CFE_STRIKEOUT:0;break;
                    }

                    SendMessage(hRTFWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                    SetFocus(hRTFWnd);
                };break;
                case IDC_FONTNAME2D:
                {
                    CHARFORMAT2 cf; // This is defined in RICHEDIT.H
                    cf.cbSize = sizeof(cf);
                    HWND hRTFWnd=GetDlgItem(hwnd,IDC_TEXT);
                    //SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);
                    GetDlgItemText(hwnd,IDC_FONTNAME2D,cf.szFaceName, sizeof(cf.szFaceName));
                    cf.dwMask  = CFM_FACE;
                    SendMessage(hRTFWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                    GetSelData(hwnd);
                }break;
                case IDC_FONTSIZE2D:
                {
                    int i=0;
                    char s[10];
                    if((wNotifyCode==CBN_SELENDOK)||(wNotifyCode==CBN_SELCHANGE))
                    {
                        SendDlgItemMessage(hwnd,IDC_FONTSIZE2D,CB_GETLBTEXT,
                                           SendDlgItemMessage(hwnd,IDC_FONTSIZE2D,CB_GETCURSEL,0,0),(LPARAM)s);
                        i=1;
                    }
                    if((wNotifyCode==CBN_EDITCHANGE))
                    {
                        GetDlgItemText(hwnd,IDC_FONTSIZE2D,s,sizeof(s));
                        i=1;
                    }
                    if(i)
                    {
                        double d;
                        if(StrToDouble(s,d))
                        {
                            CHARFORMAT2 cf; // This is defined in RICHEDIT.H
                            cf.cbSize = sizeof(cf);
                            cf.dwMask = CFM_SIZE;
                            HWND hRTFWnd=GetDlgItem(hwnd,IDC_TEXT);
                            //SendMessage(hRTFWnd, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);
                            cf.dwMask  = CFM_SIZE;
                            cf.yHeight = d*20;
                            SendMessage(hRTFWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                        }
                    }
                }break;
            }
        }break;
        case PSP_INITPAGE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D text=(HOBJ2D)b->di.d1;
            //SetDlgItemText(hwnd,IDC_FONTNAME2D,lf.lfFaceName);
            // ComboBox
            // Font Name
            HWND combo=GetDlgItem(hwnd,IDC_FONTNAME2D);
            HDC hdc=GetDC(hwnd);
            EnumFontFamilies(
                        hdc,	// handle of device control
                        NULL,	// address of family-name string
                        (FONTENUMPROC)EnumFontFamProc1,	// address of callback function
                        (LPARAM)combo 	// address of application-supplied data
                        );
            ReleaseDC(hwnd,hdc);
            combo=GetDlgItem(hwnd,IDC_FONTSIZE2D);
            // Font Size
            BYTE static scales[]={6,8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,60,72};
            for(int i=0;i<sizeof(scales);i++)
            {
                char s[64];
                wsprintf(s,"%d",scales[i]);
                SendMessage(combo,CB_ADDSTRING,0,(LPARAM)s);
            };
            TMemoryStream *ms=new TMemoryStream();
            if(WriteRTF(hsp,text,ms))
            {
                EDITSTREAM es;
                es.dwCookie=(DWORD)ms;
                es.dwError=0;
                es.pfnCallback=RtfStreamIn;
                ms->Seek(0);
                HWND hwndRtf=GetDlgItem(hwnd,IDC_TEXT);
                SetFocus(hwndRtf);
                SendMessage(hwndRtf,EM_STREAMIN,SF_RTF,(LPARAM)&es);
                SendMessage(hwndRtf,EM_SETEVENTMASK ,0,ENM_SELCHANGE);
                SendMessage(hwndRtf,EM_SETMODIFY ,0,0);
                CHARRANGE cr;
                cr.cpMin=0;cr.cpMax=-1;
                SendMessage(hwndRtf, EM_EXSETSEL ,0,(LPARAM)&cr);
                GetSelData(hwnd);
                /*
     {
      ms->Seek(0);
      TDOSStream ds("d:\\text1.rtf",TDOSStream::stCreate);
      ms->CopyTo(&ds,0,ms->GetSize());
     }
*/
            }
            delete ms;
        }return TRUE;
        case PSP_ACTIVATE:
        {
            HWND hwndRtf=GetDlgItem(hwnd,IDC_TEXT);
            SetFocus(hwndRtf);
        };break;
        case PSP_VALID:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE)
            {
                if (wParam==1)
                {
                    HWND hwndRtf=GetDlgItem(hwnd,IDC_TEXT);
                    int m=SendMessage(hwndRtf,EM_GETMODIFY,0,0);
                    if(m)
                    {
                        HSP2D hsp=b->hsp;
                        HOBJ2D obj=b->iuser1;
                        HOBJ2D text=(HOBJ2D)b->di.d1;
                        TMemoryStream *ms=new TMemoryStream();
                        EDITSTREAM es;
                        es.dwCookie=(DWORD)ms;
                        es.dwError=0;
                        es.pfnCallback=RtfStreamOut;
                        SendMessage(hwndRtf,EM_STREAMOUT,SF_RTF,(LPARAM)&es);
                        { // запись в файл после работы в диалоге
                            //ms->Seek(0);
                            //TDOSStream ds("d:\\text.rtf",TDOSStream::stCreate);
                            //ms->CopyTo(&ds,0,ms->GetSize());
                        }
                        ms->Seek(0);
                        HOBJ2D _htext=ReadRTF(hsp,ms);
                        delete ms;
                        SetTextObject2d(hsp,obj,_htext);
                        FullDeleteTool2d(hsp,TEXT2D,text);
                    }
                }
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}
#endif
#endif
BOOL scCALLBACK ObjTextProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    static LOGFONT2D lf;
    switch(msg){
        case WM_COMMAND:
        {
            EV_COMMAND
                    switch (command)
            {
        case IDC_USEBG:
                {
                    EnableWindow(GetDlgItem(hwnd,IDC_COLOR2),IsDlgButtonChecked(hwnd,IDC_USEBG));
                }break;
                case IDC_ITALIC:
                {
                    lf.lfItalic=!lf.lfItalic;__SetFont(hwnd,lf);
                    if(IsDlgButtonChecked(hwnd,IDC_ITALIC))lf.lfItalic=1;else lf.lfItalic=0;
                    __SetFont(hwnd,lf);
                }break;
                case IDC_UNDERLINE:
                {
                    if(IsDlgButtonChecked(hwnd,IDC_UNDERLINE))lf.lfUnderline=1;else lf.lfUnderline=0;
                    __SetFont(hwnd,lf);
                }break;
                case IDC_STRIKEOUT:
                {
                    if(IsDlgButtonChecked(hwnd,IDC_STRIKEOUT))lf.lfStrikeOut=1;else lf.lfStrikeOut=0;
                    __SetFont(hwnd,lf);
                }break;
                case IDC_BOLD:
                {
                    if(IsDlgButtonChecked(hwnd,IDC_BOLD))lf.lfWeight=FW_BOLD;else lf.lfWeight=0;
                    __SetFont(hwnd,lf);
                }break;
                case IDC_FONTNAME2D:
                {
                    GetDlgItemText(hwnd,IDC_FONTNAME2D,lf.lfFaceName,sizeof(lf.lfFaceName));
                    __SetFont(hwnd,lf);
                }break;
                case IDC_FONTSIZE2D:
                    //if((wNotifyCode==CBN_SELENDOK)||  (wNotifyCode==CBN_SELCHANGE))
                {
                    BYTE i=0;
                    char s[256];
                    if ((wNotifyCode==CBN_SELENDOK)||(wNotifyCode==CBN_SELCHANGE))
                    {
                        SendDlgItemMessage(hwnd,IDC_FONTSIZE2D,CB_GETLBTEXT,
                                           SendDlgItemMessage(hwnd,IDC_FONTSIZE2D,CB_GETCURSEL,0,0),(LPARAM)s);
                        i=1;
                    }
                    if ((wNotifyCode==CBN_EDITCHANGE))
                    {
                        GetDlgItemText(hwnd,IDC_FONTSIZE2D,s,sizeof(s));i=1;
                    }
                    if(i)
                    {
                        __GetFont(s,lf);
                    }
                    __SetFont(hwnd,lf);
                }break;
                    break;
            }
        }break;
        case PSP_INITPAGE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            //		int obj=b->iuser1;
            HOBJ2D text=(HOBJ2D)b->di.d1;
            LOGTEXT lt;
            GetLogText2d(hsp,text,&lt,2);
            INT16 len=GetLogString2d(hsp,lt.ltStringIndex,0,0);
            if (len){
                char * str=new char [len+1];
                GetLogString2d(hsp,lt.ltStringIndex,str,(INT16)(len+1));
                SetDlgItemText(hwnd,IDC_TEXT,str);
                delete str;
            }
            SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_SETCOLOR,0,lt.ltFgColor);
            SendDlgItemMessage(hwnd,IDC_COLOR2,COLORB_SETCOLOR,0,lt.ltBgColor&0x2ffffffl);
            CheckDlgButton(hwnd,IDC_USEBG,(lt.ltBgColor&0x1000000l)==0);
            EnableWindow(GetDlgItem(hwnd,IDC_COLOR2),(lt.ltBgColor&0x1000000l)==0);
            GetLogFont2d(hsp,lt.ltFontIndex,&lf);
            //		SetDlgItemText(hwnd,IDC_FONTNAME2D,lf.lfFaceName);
            // ComboBox
            // Font Name
            HWND combo=GetDlgItem(hwnd,IDC_FONTNAME2D);
            HDC hdc=GetDC(hwnd);
            EnumFontFamilies(
                        hdc,	// handle of device control
                        NULL,	// address of family-name string
                        (FONTENUMPROC)EnumFontFamProc1,	// address of callback function
                        (LPARAM)combo 	// address of application-supplied data
                        );
            ReleaseDC(hwnd,hdc);
            char s[256];
            int i=(int)SendMessage(combo,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)lf.lfFaceName);
            if(i>-1){
                SendMessage(combo,CB_SETCURSEL,i,0);
            }
            combo=GetDlgItem(hwnd,IDC_FONTSIZE2D);
            // Font Size
            for(i=5;i<78;i++){
                wsprintf(s,"%d",i);
                SendMessage(combo,CB_ADDSTRING,0,(LPARAM)s);
                if(i>16 && i<36)i++;
                if(i>36)i+=5;
            };

            if(lf.lfHeight<=0){
                wsprintf(s,"%d",-lf.lfHeight);
            }else{
                wsprintf(s,"%dpt",lf.lfHeight);
            }
            if(lf.lfWidth){
                if(lf.lfWidth<0){
                    wsprintf(s+lstrlen(s),"/%d",-lf.lfWidth);
                }else{
                    wsprintf(s+lstrlen(s),"/%dpt",lf.lfWidth);
                }}

            i=(int)SendMessage(combo,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)lf.lfFaceName);
            if(i>-1){
                SendMessage(combo,CB_SETCURSEL,i,0);
            }else SetWindowText(combo,s);
        }
            __SetFont(hwnd,lf,2);
            CheckDlgButton(hwnd,IDC_BOLD,lf.lfWeight==FW_BOLD);
            CheckDlgButton(hwnd,IDC_ITALIC,lf.lfItalic!=0);
            CheckDlgButton(hwnd,IDC_UNDERLINE,lf.lfUnderline!=0);
            CheckDlgButton(hwnd,IDC_STRIKEOUT,lf.lfStrikeOut!=0);


            return TRUE;
        case PSP_CLOSE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b->flags&PSP_INITIALIZE){
                __SetFont(hwnd,lf,1);
            };
        };return TRUE;
        case PSP_VALID:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE)
            {
                if (wParam==1)
                {
                    HSP2D hsp=b->hsp;
                    HOBJ2D obj=b->iuser1;
                    HOBJ2D text=(HOBJ2D)b->di.d1;
                    LOGTEXT lt;
                    LOGTEXT lt1;
                    GetLogText2d(hsp,text,&lt,1);
                    lt1=lt;

                    if(SendDlgItemMessage(hwnd,IDC_TEXT,EM_GETMODIFY,0,0))
                    {
                        int l=GetWindowTextLength(GetDlgItem(hwnd,IDC_TEXT));
                        char *str=0;
                        if (l)
                        {
                            str=new char[l+1];
                            GetWindowText(GetDlgItem(hwnd,IDC_TEXT),str,l+1);
                        }
                        if (GetToolRef2d(hsp,STRING2D,lt1.ltStringIndex)>1 || (GetToolRef2d(hsp,TEXT2D,text)>1))
                            lt1.ltStringIndex=CreateString2d(hsp,str);
                        else
                            SetLogString2d(hsp,lt1.ltStringIndex,str);
                        if (str)
                            delete str;
                    }// строка

                    LOGFONT2D lf1;
                    GetLogFont2d(hsp,lt.ltFontIndex,&lf1);
                    if (lstrcmp(lf1.lfFaceName,lf.lfFaceName)
                        ||(lf1.lfItalic!=lf.lfItalic)||(lf1.lfUnderline != lf.lfUnderline)
                        ||(lf1.lfStrikeOut != lf.lfStrikeOut)||(lf1.lfWeight != lf.lfWeight)
                        ||(lf1.lfCharSet != lf.lfCharSet)||(lf1.lfHeight != lf.lfHeight)
                        ||(lf1.lfWidth != lf.lfWidth)	)
                    {
                        if (GetToolRef2d(hsp,FONT2D,lt1.ltFontIndex)>1 || (GetToolRef2d(hsp,TEXT2D,text)>1))
                            lt1.ltFontIndex=CreateFont2d(hsp,&lf);
                        else
                            SetLogFont2d(hsp,lt1.ltFontIndex,&lf);
                    } // шрифт

                    lt1.ltFgColor=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,0);
                    lt1.ltBgColor=SendDlgItemMessage(hwnd,IDC_COLOR2,COLORB_GETCOLOR,0,0);
                    if(!IsDlgButtonChecked(hwnd,IDC_USEBG))
                        lt1.ltBgColor|=0x1000000l;

                    if ((lt1.ltFgColor!=lt.ltFgColor)||
                        (lt1.ltBgColor!=lt.ltBgColor)||
                        (lt1.ltFontIndex!=lt.ltFontIndex)||
                        (lt1.ltStringIndex!=lt.ltStringIndex)){
                        if (GetToolRef2d(hsp,TEXT2D,text)>1){
                            text=CreateText2d(hsp,&lt1,1);
                            SetTextObject2d(hsp,obj,text);
                        }else  SetLogText2d(hsp,text,&lt1,1);
                    }
                    SetCurrentText2d(hsp,text);
                }
            }
            b->Valid();
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
void StdControlCheck(HWND hwnd,DWORD style){

    if (style&WS_TABSTOP)CheckDlgButton(hwnd,IDC_TABSTOP,1);
    if (style&WS_GROUP)CheckDlgButton(hwnd,IDC_GROUPcnt,1);
    if (style&WS_DISABLED)CheckDlgButton(hwnd,IDC_DISABLED,1);
    if (style&WS_VSCROLL)CheckDlgButton(hwnd,IDC_VSCROLL,1);
    if (style&WS_HSCROLL)CheckDlgButton(hwnd,IDC_HSCROLL,1);
    if (style&WS_BORDER)CheckDlgButton(hwnd,IDC_WSBORDER,1);
};
void ChkStyle(HWND hwnd,int id,DWORD&style,DWORD bit){
    if(GetDlgItem(hwnd,id)){
        if(IsDlgButtonChecked(hwnd,id))style|=bit;else style&=~bit;
    }
}

void StdControlGetCheck(HWND hwnd,DWORD &style){
    ChkStyle(hwnd,IDC_TABSTOP,style,WS_TABSTOP);
    ChkStyle(hwnd,IDC_GROUPcnt,style,WS_GROUP);
    ChkStyle(hwnd,IDC_DISABLED,style,WS_DISABLED);
    ChkStyle(hwnd,IDC_VSCROLL,style,WS_VSCROLL);
    ChkStyle(hwnd,IDC_HSCROLL,style,WS_HSCROLL);
    ChkStyle(hwnd,IDC_WSBORDER,style,WS_BORDER);
};

WORD button_styles[]={IDC_PUSHBUTTON,IDC_DEFPUSH,IDC_CHECKBOX,IDC_ACHECKBOX,
                      IDC_RADIO,IDC_3STATE,IDC_A3STATE,
                      IDC_GROUPBOX,0,IDC_ARADIO};

BOOL scCALLBACK ObjControlProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D object=b->iuser1;
            HWND hwndctl=GetControl2d(hsp,object);
            DWORD style=GetWindowLong(hwndctl,GWL_STYLE);
            StdControlCheck(hwnd,style);
            switch(b->di.d1){
                case OT_EDIT:{
                    int id=IDC_ALLIGN1;
                    if (style&ES_CENTER)id=IDC_ALLIGN3;
                    if (style&ES_RIGHT)id=IDC_ALLIGN2;
                    CheckDlgButton(hwnd,id,1);

                    id=IDC_CASEINS;
                    if (style&ES_UPPERCASE)id=IDC_UPPERCASE;
                    if (style&ES_LOWERCASE)id=IDC_LOWERCASE;
                    CheckDlgButton(hwnd,id,1);

                    if (style&ES_AUTOVSCROLL)CheckDlgButton(hwnd,IDC_AUTOV,1);
                    if (style&ES_AUTOHSCROLL)CheckDlgButton(hwnd,IDC_AUTOH,1);
                    if (style&ES_PASSWORD)CheckDlgButton(hwnd,IDC_PASSWORD,1);
                    if (style&ES_OEMCONVERT)CheckDlgButton(hwnd,IDC_OEMCONVERSION,1);
                    if (style&ES_NOHIDESEL)CheckDlgButton(hwnd,IDC_KEEPSEL,1);
                    if (style&ES_MULTILINE)CheckDlgButton(hwnd,IDC_MULTYLINE,1);
                    if (style&ES_READONLY)CheckDlgButton(hwnd,IDC_READONLY,1);
                    if (style&ES_WANTRETURN)CheckDlgButton(hwnd,IDC_WANTRETURN,1);

                }break;
                case OT_COMBOBOX:{

                    if ((style&CBS_DROPDOWNLIST)==CBS_DROPDOWNLIST)CheckDlgButton(hwnd,IDC_DROPDOWNLIST,1);else
                    {
                        if ((style&CBS_DROPDOWN)==CBS_DROPDOWN)CheckDlgButton(hwnd,IDC_DROPDOWN,1);
                        if ((style&CBS_SIMPLE)==CBS_SIMPLE)CheckDlgButton(hwnd,IDC_SIMPLY,1);
                    }

                    if (style&CBS_SORT)CheckDlgButton(hwnd,IDC_SORTED,1);
                    if (style&CBS_NOINTEGRALHEIGHT)CheckDlgButton(hwnd,IDC_INTEGRALH,1);
                    if (style&CBS_OEMCONVERT)CheckDlgButton(hwnd,IDC_OEMCONVERSION,1);
                    if (style&CBS_DISABLENOSCROLL)CheckDlgButton(hwnd,IDC_VSCROLLALL,1);
                    if (style&CBS_AUTOHSCROLL)CheckDlgButton(hwnd,IDC_AUTOH,1);
                }break;
                case OT_BUTTON:{
                    int _style=(int)(style&0xf);
                    int id;
                    if (_style>=sizeof(button_styles)/sizeof(WORD))_style=0;
                    id=button_styles[_style];
                    CheckDlgButton(hwnd,id,1);
                    char s[256];
                    GetWindowText(hwndctl,s,256);
                    SetDlgItemText(hwnd,IDC_TEXT,s);
                }break;
                case OT_LISTBOX:{
                    if (style&LBS_SORT)CheckDlgButton(hwnd,IDC_SORTED,1);
                    if (style&LBS_DISABLENOSCROLL)CheckDlgButton(hwnd,IDC_VSCROLLALL,1);
                    if (style&LBS_NOINTEGRALHEIGHT)CheckDlgButton(hwnd,IDC_INTEGRALH,1);
                    if (style&LBS_MULTICOLUMN)CheckDlgButton(hwnd,IDC_MULTYCOLUMN,1);
                    if (style&LBS_NOREDRAW)CheckDlgButton(hwnd,IDC_NOREDRAW,1);
                    if (style&LBS_MULTIPLESEL)CheckDlgButton(hwnd,IDC_MULTIPLESEL,1);
                    if (style&LBS_EXTENDEDSEL)CheckDlgButton(hwnd,IDC_EXTENDSEL,1);
                    if (style&LBS_USETABSTOPS)CheckDlgButton(hwnd,IDC_USETABSTOP,1);
                    if (style&LBS_WANTKEYBOARDINPUT)CheckDlgButton(hwnd,IDC_WANTKEY,1);

                }break;
            }
        }return TRUE;
        case PSP_CLOSE:if(wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                HSP2D hsp=b->hsp;
                HOBJ2D object=b->iuser1;
                HWND hwndctl=GetControl2d(hsp,object);
                DWORD style=SetControlStyle2d(hsp,object,style,0);;
                StdControlGetCheck(hwnd,style);
                switch(b->di.d1){
                    case OT_EDIT:{
                        ChkStyle(hwnd,IDC_AUTOV,style,ES_AUTOVSCROLL);
                        ChkStyle(hwnd,IDC_AUTOH,style,ES_AUTOHSCROLL);
                        ChkStyle(hwnd,IDC_PASSWORD,style,ES_PASSWORD);
                        ChkStyle(hwnd,IDC_OEMCONVERSION,style,ES_OEMCONVERT);
                        ChkStyle(hwnd,IDC_KEEPSEL,style,ES_NOHIDESEL);
                        ChkStyle(hwnd,IDC_MULTYLINE,style,ES_MULTILINE);
                        ChkStyle(hwnd,IDC_READONLY,style,ES_READONLY);
                        ChkStyle(hwnd,IDC_WANTRETURN,style,ES_WANTRETURN);

                        style&=~(ES_CENTER|ES_RIGHT);
                        ChkStyle(hwnd,IDC_ALLIGN3,style,ES_CENTER);
                        ChkStyle(hwnd,IDC_ALLIGN2,style,ES_RIGHT);

                        style&=~(ES_UPPERCASE|ES_LOWERCASE);
                        ChkStyle(hwnd,IDC_UPPERCASE,style,ES_UPPERCASE);
                        ChkStyle(hwnd,IDC_LOWERCASE,style,ES_LOWERCASE);


                    }break;
                    case OT_COMBOBOX:{
                        ChkStyle(hwnd,IDC_SORTED,style,CBS_SORT);
                        ChkStyle(hwnd,IDC_INTEGRALH,style,CBS_NOINTEGRALHEIGHT);
                        ChkStyle(hwnd,IDC_OEMCONVERSION,style,CBS_OEMCONVERT);
                        ChkStyle(hwnd,IDC_VSCROLLALL,style,CBS_DISABLENOSCROLL);
                        ChkStyle(hwnd,IDC_AUTOH,style,CBS_AUTOHSCROLL);

                        style&=~(CBS_DROPDOWNLIST);
                        if(IsDlgButtonChecked(hwnd,IDC_DROPDOWNLIST))style|=(CBS_DROPDOWNLIST);
                        else{
                            if(IsDlgButtonChecked(hwnd,IDC_DROPDOWN))style|=CBS_DROPDOWN;
                            else style|=CBS_SIMPLE;
                        }
                    }break;
                    case OT_BUTTON:{
                        for(int i=0;i<sizeof(button_styles)/2;i++)
                            if (IsDlgButtonChecked(hwnd,button_styles[i])){
                                style&=~0xf;
                                style|=i;
                                break;
                            }
                        char s[256];
                        GetDlgItemText(hwnd,IDC_TEXT,s,256);
                        SetWindowText(hwndctl,s);

                    }break;
                    case OT_LISTBOX:{
                        ChkStyle(hwnd,IDC_SORTED,style,LBS_SORT);
                        ChkStyle(hwnd,IDC_VSCROLLALL,style,LBS_DISABLENOSCROLL);
                        ChkStyle(hwnd,IDC_INTEGRALH,style,LBS_NOINTEGRALHEIGHT);
                        ChkStyle(hwnd,IDC_MULTYCOLUMN,style,LBS_MULTICOLUMN);
                        ChkStyle(hwnd,IDC_NOREDRAW,style,LBS_NOREDRAW);
                        ChkStyle(hwnd,IDC_MULTIPLESEL,style,LBS_MULTIPLESEL);
                        ChkStyle(hwnd,IDC_EXTENDSEL,style,LBS_EXTENDEDSEL);
                        ChkStyle(hwnd,IDC_USETABSTOP,style,LBS_USETABSTOPS);
                        ChkStyle(hwnd,IDC_WANTKEY,style,LBS_WANTKEYBOARDINPUT);
                    }break;
                }

                SetControlStyle2d(hsp,object,style,1);

            };return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
            }else {b->Valid();}
        }return TRUE;
    }
    return FALSE;
};

BOOL scCALLBACK ObjGroupProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D object=b->iuser1;
            HWND tsw=GetDlgItem(hwnd,IDC_TREEINFO);
            VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
            sw->NotifyWnd=mainwindow->HWindow;
            __InsertObject2d(tsw,0,hsp,object);
            ::SendMessage(sw->NotifyWnd,TVN_GETITEMINFO,(WPARAM)tsw,MAKELONG(hsp,object));
            ::SendMessage(tsw,TV_SETSHIFT,1,0);
            ::SendMessage(tsw,TV_EXPAND,1,MAKELONG(hsp,object));
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK Obj3dProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D object=b->iuser1;
            HSP3D hsp3d=Get3dSpace2d(hsp,object);
            HWND tsw=GetDlgItem(hwnd,IDC_TREEINFO);
            VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
            sw->NotifyWnd=mainwindow->HWindow;

            if(b->di.d1==1){

                TreeItemData td;
                td.parent=0;
                td.IsLeaf = (GetNextObject3d(hsp3d,0)==0);
                td.datatype=TREE_TEXTURE3D;
                td.data = MAKELONG(hsp3d,0);;
                td.IsImage = WORLD_IMG;
                td.text="Textures";
                ::SendMessage(tsw,TVN_INSERTITEM,0,(long)&td);

            }
            if(b->di.d1==0){
                __InsertSpace3d2d(tsw,0,hsp3d);
            }

            ::SendMessage(sw->NotifyWnd,TVN_GETITEMINFO,(WPARAM)tsw,MAKELONG(hsp3d,0));
            ::SendMessage(tsw,TV_SETSHIFT,1,0);
            ::SendMessage(tsw,TV_EXPAND,1,MAKELONG(hsp3d,0));
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL AddUseToolText2d(HSP2D hsp,TOOL_TYPE t,HOBJ2D tool,char *s){
    INT16 ref=GetToolRef2d(hsp,t,tool);
    char cmd[64];
    if (ref){
        LoadString(HResource,IDS_str127,cmd,sizeof(cmd));
        wsprintf(s+lstrlen(s),cmd ,ref);
    }else {
        LoadString(HResource,IDS_str128,cmd,sizeof(cmd));
        lstrcat(s,cmd);
    }
    return TRUE;
};
BOOL GetMaterialText3d(HSP3D hsp3d,HOBJ2D tool,char*ps,BOOL adduse){
    MATERIAL3D m;
    memset(&m,0,sizeof(m));
    m.poSize=sizeof(m);
    char s[256];
    char s2[256];
    m.hfile=s;
    m.name=s2;
    *ps=0;
    if(!GetMaterial3d((HSP3D)hsp3d,tool,&m))return FALSE;
    wsprintf(ps,"%s",m.name);
    if(m.flags&TF_FILE){
        wsprintf(ps+lstrlen(ps)," file: %s",m.hfile);
    }
    if(adduse){
        AddUseToolText2d((HSP2D)hsp3d,TEXTURE3D,tool,ps);
    }
    return TRUE;
}
void RebuildTexturesList(HWND list,HSP3D hsp3d){
    SendMessage(list,LB_RESETCONTENT,0,0);
    int i=0;
    HOBJ2D tool,ct=GetCurrentTool2d((HSP2D)hsp3d,TEXTURE3D);
    while ((tool=GetNextTool2d((HSP2D)hsp3d,TEXTURE3D,tool))!=0){
        char s[256];
        GetMaterialText3d(hsp3d,tool,s,TRUE);
        SendMessage(list,LB_ADDSTRING,0,(LONG)s);
        if(ct==tool)SendMessage(list,LB_SETCURSEL,i,0);
        i++;
    }
}
BOOL scCALLBACK SpaceTexturesProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b->di.d1==1){
                HSP2D   hsp    = b->hsp;
                HOBJ2D  object = b->iuser1;
                b->hsp         = (HSP2D)Get3dSpace2d(hsp,object);
            }
            HSP3D hsp3d=(HSP3D)b->hsp;


            HOBJ2D object3d=GetCurrentObject2d((HSP2D)hsp3d);
            EnableWindow(GetDlgItem(hwnd,IDC_APPLYTEXTURE),GetObjectType3d(hsp3d,object3d)==otOBJECT3D);
            RebuildTexturesList(GetDlgItem(hwnd,IDC_TEXTURES),hsp3d);
        }return TRUE;
        case WM_COMMAND:{
            TPropertyButton*b=GetDlgPropertyButton(hwnd);
            if(b){
                HSP3D hsp3d=(HSP3D)b->hsp;
                EV_COMMAND
                        switch(command){
        case IDC_APPLYTEXTURE:{
                        int i=(int)SendDlgItemMessage(hwnd,IDC_TEXTURES,LB_GETCURSEL,0,0);
                        HOBJ2D tool=GetToolHANDLEbyNum((HSP2D)hsp3d,TEXTURE3D,(INT16)i);
                        if(tool){
                            HOBJ2D object3d=GetCurrentObject2d((HSP2D)hsp3d);
#ifndef RUNTIME
                            ReplaceTexture3d(hsp3d,object3d,0,tool);
#endif             
                            RebuildTexturesList(GetDlgItem(hwnd,IDC_TEXTURES),hsp3d);
                        }
                    }return TRUE;
                    case IDC_TEXTURES:{
                        if(wNotifyCode==LBN_SELCHANGE){
                            HWND list=GetDlgItem(hwnd,IDC_TEXTURES);
                            int i=(int)SendMessage(list,LB_GETCURSEL,0,0);

                            HOBJ2D tool=GetToolHANDLEbyNum((HSP2D)hsp3d,TEXTURE3D,(INT16)i);
                            SetCurrentTool2d((HSP2D)hsp3d,TEXTURE3D,tool);
                            return TRUE;
                        }
                        if(wNotifyCode!=LBN_DBLCLK) return TRUE;}
                    case IDC_EDITTEXTURE:{
                        HWND list=GetDlgItem(hwnd,IDC_TEXTURES);
                        int i=(int)SendMessage(list,LB_GETCURSEL,0,0);
                        HOBJ2D tool=GetToolHANDLEbyNum((HSP2D)hsp3d,TEXTURE3D,(INT16)i);
                        if(TextureDialog(hwnd,hsp3d,tool)){
                            SendMessage(list,LB_DELETESTRING,i,0);
                            char s[256];
                            GetMaterialText3d(hsp3d,tool,s,TRUE);
                            SendMessage(list,LB_INSERTSTRING,i,(LONG)s);
                            SendMessage(list,LB_SETCURSEL,i,0);
                        }
                    }return TRUE;
                    case IDC_DELTEXTURE:{
                        HWND list=GetDlgItem(hwnd,IDC_TEXTURES);
                        int i=(int)SendMessage(list,LB_GETCURSEL,0,0);
                        HOBJ2D tool=GetToolHANDLEbyNum((HSP2D)hsp3d,TEXTURE3D,(INT16)i);
                        if(GetToolRef2d((HSP2D)hsp3d,TEXTURE3D,tool)){;
                        }else{
                            if(DeleteTool2d((HSP2D)hsp3d,TEXTURE3D,tool)){
                                SendMessage(list,LB_DELETESTRING,i,0);
                            }
                        }
                    }return TRUE;
                    case IDC_ADDTEXTURE:{
                        HOBJ2D tool=TextureDialog(hwnd,hsp3d,0);
                        if(tool){
                            HWND list=GetDlgItem(hwnd,IDC_TEXTURES);
                            char s[256];
                            GetMaterialText3d(hsp3d,tool,s,TRUE);
                            int i=GetToolNumByHANDLE((HSP2D)hsp3d,TEXTURE3D,tool);
                            SendMessage(list,LB_INSERTSTRING,i,(LONG)s);
                            SendMessage(list,LB_SETCURSEL,i,0);
                            SetCurrentTool2d((HSP2D)hsp3d,TEXTURE3D,tool);
                        }

                    }return TRUE;
                }};
        }break; // WM_COMMAND
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
/*
BOOL scCALLBACK AnimeTimeProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
int command;
switch(msg){
    case PSP_INITPAGE:{
      TPropertyButton*b=(TPropertyButton*)lParam;
      TAnime*a=(TAnime*)b->pdata;

        SetDlgItemDouble(hwnd,IDC_TIME1,a->time1);
        SetDlgItemDouble(hwnd,IDC_TIME2,a->time2);

      }return TRUE;
        case PSP_CLOSE:if(wParam==1){
      //TPropertyButton*b=(TPropertyButton*)lParam;
     };return TRUE;
    case PSP_VALID:{
    TPropertyButton*b=(TPropertyButton*)lParam;

    if (b->flags&PSP_INITIALIZE){
     if (wParam==1){
      double t1,t2;
     if(
       IsDoubleValid(hwnd,IDC_TIME1,t1,0,100000)&&
       IsDoubleValid(hwnd,IDC_TIME2,t2,0,10000)&& (t1<t2)
      ){b->Valid();
        TAnime*a=(TAnime*)b->pdata;
        a->time1=t1;
        a->time2=t2;

      BOOL d=IsDlgButtonChecked(hwnd,IDC_DISABLE);
      if (d)a->flags|=OF_DONTSELECT;else a->flags&=~OF_DONTSELECT;

      }else b->Invalid();
     }
     }
    }break;
   }
 return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK AnimeCustomProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
int command;
switch(msg){

    case WM_INITDIALOG:{
      TPropertyButton*b=(TPropertyButton*)lParam;
      TAnime*a=(TAnime*)b->pdata;
      PClass _class=a->GetClass(TRUE);
      if(_class){
       HSP2D hsp;

       // hsp=winmanager->CreateDlgWindow(hwnd,"Animate_Dialog",_class->name,0,0,	CW_USEDEFAULT,CW_USEDEFAULT,"WS_CHILD|WS_BYSPACE");

        HWND _hwnd=GetSpaceWindow2d(hsp);
        RECT r;
        GetWindowRect(_hwnd,&r);
        SetWindowPos(hwnd,0,0,0,(r.right-r.left),(r.bottom-r.top),SWP_NOZORDER|SWP_NOMOVE);
      }
      return TRUE;
    };
    case PSP_INITPAGE:{
      TPropertyButton*b=(TPropertyButton*)lParam;

      }return TRUE;
      case PSP_CLOSE:{
      TPropertyButton*b=(TPropertyButton*)lParam;
      if(wParam==1){
      //TPropertyButton*b=(TPropertyButton*)lParam;
     };}return TRUE;
    case PSP_VALID:{
    TPropertyButton*b=(TPropertyButton*)lParam;

       if (b->flags&PSP_INITIALIZE){

       }

    }break;
   }
 return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};



BOOL AnimateDiloag(HWND hwnd,TAnime*a){
 TPropertyButton*b=new TPropertyButton("Основное",HResource,"Anime_main"); //"Положение"
  b->Procedure=AnimeTimeProc;

  TPropertyButton*b1=new TPropertyButton("Основное",HResource,"Empty_Dialog"); //"Положение"
  b1->Procedure=AnimeCustomProc;
  b->Insert(b1);
   b1=b;
  while (b1){
    b1->pdata=(void*)a;
    b1=b1->next;
  };
 int rez=CreatePropertySheet(hwnd,a->name,b,0);
 delete b;
 return rez;
};
*/

BOOL scCALLBACK SearchTextProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ switch(msg){
        case WM_INITDIALOG:{
            UINT32 b=SCGetScVar("search_opt1",0);
            char s[256];
            if(SCGetScVarS("search_string",s)) SetDlgItemText(hwnd,IDC_SEARCHKEY,s);
            if(lParam){
                if(SCGetScVarS("replace_string",s)) SetDlgItemText(hwnd,IDC_REPLACEKEY,s);
                CheckDlgButton(hwnd,IDC_PROMPTONREPLACE,(b&SR_PROMPT)!=0);
            }
            CheckDlgButton(hwnd,IDC_CASESENSITIVE,(b&SR_CASE)!=0);
            CheckDlgButton(hwnd,IDC_WHOLEWORDS,(b&SR_WORDS)!=0);

            if(b&SR_SELECT)
                CheckDlgButton(hwnd,IDC_SELECTSCOPE,1);
            else CheckDlgButton(hwnd,IDC_GLOBALSCOPE,1);
            if(b&SR_BACK)
                CheckDlgButton(hwnd,IDC_LOOKBUCK,1);
            else CheckDlgButton(hwnd,IDC_LOOKFORWARD,1);
            if(b&SR_FROMCURSOR)CheckDlgButton(hwnd,IDC_FROMCURSOR ,1);
            else CheckDlgButton(hwnd,IDC_ENTIRESCOPE ,1);
        }return TRUE;

        case WM_COMMAND:
            EV_COMMANDsimply
                    switch(command){
        case IDCANCEL     :EndDialog(hwnd,0);return TRUE;
                case IDC_REPLACEALL:{
                    char s[256];
                    GetDlgItemText(hwnd,IDC_REPLACEKEY,s,sizeof(s));
                    SCSetScVarS("replace_string",s);
                };
                case IDOK:{
                    char s[256];
                    GetDlgItemText(hwnd,IDC_SEARCHKEY,s,sizeof(s));
                    SCSetScVarS("search_string",s);
                    int b=0;
                    b=IsDlgButtonChecked(hwnd,IDC_CASESENSITIVE)?SR_CASE:0;
                    if(IsDlgButtonChecked(hwnd,IDC_WHOLEWORDS))b|=SR_WORDS;
                    if(GetDlgItem(hwnd,IDC_PROMPTONREPLACE)){
                        if(IsDlgButtonChecked(hwnd,IDC_PROMPTONREPLACE))b|=SR_PROMPT;
                    }
                    if(IsDlgButtonChecked(hwnd,IDC_SELECTSCOPE))b|=SR_SELECT;
                    if(IsDlgButtonChecked(hwnd,IDC_LOOKBUCK))b|=SR_BACK;
                    if(IsDlgButtonChecked(hwnd,IDC_FROMCURSOR))b|=SR_FROMCURSOR;

                    SCSetScVarI("search_opt1",b);
                    EndDialog(hwnd,(command==IDC_REPLACEALL)?2:1);return TRUE;}
            }break;
    }
    return FALSE;
};
BOOL SearchTextDialog(HWND hwnd,BOOL replace){
    return DialogBoxParam(HResource,MAKEINTRESOURCE(replace?REPLACETEXT_DIALOG:SEARCHTEXT_DIALOG),hwnd,SearchTextProc,replace);
};

int strpos(char*str,char*fragment);
char * temstring=NULL;
/*
BOOL scCALLBACK GetAnimateProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
//HWND  hwndCtl;
//INT16 wNotifyCode;
//INT16 command;
switch(msg){
    case WM_INITDIALOG:{
     HWND list = GetDlgItem(hwnd,IDC_CLASSLIST);
     C_TYPE index;
     TSortedCollection *classes= GetClasses();
      TClassListInfo ci;
      ci.classname="Animate_";
     classes->Search(&ci,index);
     while(index<classes->count){
       char *_s = ((PClassListInfo)classes->At(index))->GetClassName();
       if(strpos(_s,"Animate_")==0){
       int l=lstrlen(_s);
       char s[65];
       lstrcpyn(s,_s+8,sizeof(s));
       s[5]=0;
       if(!lstrcmpi(s,"Proc_")){
         _s+=13;
         SendMessage(list,LB_ADDSTRING,0,(LPARAM)_s);
        }
       }else break;
       index++;
     }
     SendMessage(list,LB_SETCURSEL,0,0);
    }return TRUE;
    case WM_COMMAND:{
     EV_COMMAND
        switch (command){
        case IDC_CLASSLIST:{
         if(wNotifyCode!=LBN_DBLCLK)break;
        };
        case IDOK:{
          HWND list = GetDlgItem(hwnd,IDC_CLASSLIST);
          int i=SendMessage(list,LB_GETCURSEL,0,0);
          char s[256];
          SendMessage(list,LB_GETTEXT,i,(LPARAM)s);
          temstring=NewStr(s);
          EndDialog(hwnd,1);return TRUE;
         }
         case IDCANCEL: EndDialog(hwnd,0);return TRUE;
        }
       }break;
       }
 return FALSE;
};




BOOL scCALLBACK ObjAnimeProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
int command;
switch(msg){
  case WM_COMMAND:{
    EV_COMMANDsimplyNoVar
     switch (command){
      case CM_ADDANIME:{
         if(DialogBoxParam(HResource,MAKEINTRESOURCE(ANIMATE_DIALOG),hwnd,GetAnimateProc,0)){
          if(temstring){
           TAnime * a=new TAnime(temstring);
           if(AnimateDiloag(hwnd,a)){
             TPropertyButton*b=GetDlgPropertyButton(hwnd);
             if(b->pdata){
              b->pdata=a;
             }else{
              ((TAnime*)b->pdata)->Insert(a);
             }
           }else delete a;
           delete temstring;
          }
         }
      }break;
      case CM_PROPERTIES:{

      }break;
      case CM_DELETE:{

      }break;
     }
   }break;
    case TVN_ITEMMENU:{
    mainwindow->lasttreeitem=lParam;
    DWORD data=lParam;
    HWND tree=(HWND)wParam;
    TreeItemData td;
    td.data=data;
  HMENU  p=0;
  if(!::SendMessage(tree,TVN_GETITEMINFO,0,(long)&td))return 0;
 switch(td.datatype){
      case TREE_ANIITEM:{
           p=CreatePopupMenu();
                AppendMenu(p,MF_ENABLED,CM_PROPERTIES, "Properties..."

           );
           AppendMenu(p,MF_ENABLED,CM_DELETE,"Удалить");

          }
      case TREE_ANIMATE:{
            if(!p)p=CreatePopupMenu();
            AppendMenu(p,MF_ENABLED,CM_ADDANIME,"Добавить");
          }break;
  }
   if(p){
       EnableWindow(hwnd,FALSE);
       ::TrackPopupMenu(p,TPM_LEFTALIGN ,TWindow::lastmousepos.x,
       TWindow::lastmousepos.y, 0,hwnd,NULL);
       EnableWindow(hwnd,TRUE);
       FullDeleteMenu(p);
       SetFocus(hwnd);
    }
    }break;
    case PSP_INITPAGE:{
      TPropertyButton*b=(TPropertyButton*)lParam;
       HSP2D hsp=b->hsp;
       HOBJ2D obj=b->iuser1;
       HWND tsw=GetDlgItem(hwnd,IDC_TREEINFO);
       VTreeWindow*sw=(VTreeWindow*)GetWindowLong(tsw,0);
              TreeItemData td;
              td.parent = NULL;
              td.IsLeaf = 0;
              td.IsImage = 0;
              td.datatype = TREE_ANIMATE;
              td.data=(DWORD)hsp;

              td.text="Animation";
         TMemoryStream *ms= ObjectData(hsp,obj,UD_ANIMATE);

         if (ms && ms->GetSize()>4){

          ms->SetVersion(ms->ReadWord());
          TAnime * a=new TAnime(ms);
          b->pdata=(void*)a;
          td.data=(DWORD)b->pdata;
         };
         ::SendMessage(tsw,TVN_INSERTITEM,0,(long)&td);
          sw->NotifyWnd=hwnd;//mainwindow->HWindow;
         ::SendMessage(tsw,TV_EXPAND,1,td.data);

      }return TRUE;
    }
 return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
*/

void ProcessHotspot(HWND hwnd)
{
    int hotspot=(IsDlgButtonChecked(hwnd,IDC_HOTSPOT));
    EnableWindow(GetDlgItem(hwnd,IDC_JUMPSTYLE),hotspot);
    EnableWindow(GetDlgItem(hwnd,IDC_TARGET),hotspot);
    EnableWindow(GetDlgItem(hwnd,IDC_TARGETOBJ),hotspot);
    EnableWindow(GetDlgItem(hwnd,IDC_EFFECT),hotspot);
    EnableWindow(GetDlgItem(hwnd,IDC_EFFECTSETUP),hotspot);
    EnableWindow(GetDlgItem(hwnd,IDC_WINNAME),hotspot);
    EnableWindow(GetDlgItem(hwnd,IDC_HYPERADD),hotspot);
    EnableWindow(GetDlgItem(hwnd,IDC_ADVANSED),hotspot);
}

BOOL scCALLBACK ObjHyperProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    HWND hwndCtl;
    int wNotifyCode;
    int command;

    switch(msg)
    {
        case PSP_INITPAGE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            int i=IDS_TARGETMODE;
            char s[256];
            while(LoadString(HResource,i++,s,sizeof(s)))
            {
                SendDlgItemMessage(hwnd,IDC_JUMPSTYLE,CB_ADDSTRING,0,(LPARAM)s);
            }


            char *se[]={"None","Vertical","Horisontal","Morph"};
            for(i=0;i<sizeof(se)/4;i++)
                SendDlgItemMessage(hwnd,IDC_EFFECT,CB_ADDSTRING,0,(LPARAM)se[i]);

            THyperKey *hk=new THyperKey(hsp,obj);
            SetDlgItemText(hwnd,IDC_TARGET,hk->target);
            SetDlgItemText(hwnd,IDC_TARGETOBJ,hk->object);
            if(hk->add)SetDlgItemText(hwnd,IDC_HYPERADD,hk->add);
            SendDlgItemMessage(hwnd,IDC_JUMPSTYLE,CB_SETCURSEL,hk->openmode,NULL);
            int ef=0;
            if (hk->effect)
                for(i=1;i<sizeof(se)/4;i++)
                {
                    if (!lstrcpy(se[i],hk->effect))
                    {
                        ef=i;break;
                    }
                }
            SendDlgItemMessage(hwnd,IDC_EFFECT,CB_SETCURSEL,ef,NULL);
            b->pdata=hk;
            int _size=GetObjectData2d(hsp,obj,UD_HYPERKEY,NULL,0,0);
            CheckDlgButton(hwnd,IDC_HOTSPOT,(_size!=0));
            ProcessHotspot(hwnd);
            SetDlgItemText(hwnd,IDC_WINNAME,hk->winname);
        }goto m1;//return TRUE;

        case PSP_CLOSE:
        {
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=b->hsp;
            HOBJ2D obj=b->iuser1;
            THyperKey *hk=(THyperKey *)b->pdata;
            if (hk)
            {
                if(wParam==1)
                {
                    int hotspot=(IsDlgButtonChecked(hwnd,IDC_HOTSPOT));
                    if (hotspot){
                        char s[256];
                        GetDlgItemText(hwnd,IDC_TARGET,s,256);
                        hk->SetTarget(s);
                        GetDlgItemText(hwnd,IDC_TARGETOBJ,s,256);
                        hk->SetObject(s);
                        GetDlgItemText(hwnd,IDC_WINNAME,s,256);
                        hk->SetWinName(s);
                        GetDlgItemText(hwnd,IDC_HYPERADD,s,256);
                        hk->SetAdd(s);

                        if (SendDlgItemMessage(hwnd,IDC_EFFECT,CB_GETCURSEL,0,NULL)){
                            GetDlgItemText(hwnd,IDC_EFFECT,s,256);
                            hk->SetEffect(s);
                        }else hk->SetEffect(NULL);
                        hk->SetMode((int)SendDlgItemMessage(hwnd,IDC_JUMPSTYLE,CB_GETCURSEL,0,NULL));
                    }else{
                        SetObjectData2d(hsp,obj,UD_HYPERKEY,NULL,0,0);
                    }
                    if (hk->IsModifyed())hk->Update();
                }
                delete hk;
            }
        }return TRUE;

        case PSP_VALID:
        {}return TRUE;

        case WM_COMMAND:
            EV_COMMANDnoVar;
            switch (command)
            {
                case IDC_JUMPSTYLE:
                    if(wNotifyCode==CBN_SELCHANGE)
                    {
m1:
                        int i = SendDlgItemMessage(hwnd,IDC_JUMPSTYLE,CB_GETCURSEL,0,0);
                        char * ps="";
                        char s[256];
                        LoadStr(IDS_TARGETDESCR+i,s);
                        ps=s;
                        SetDlgItemText(hwnd,IDC_TARGETtxt,ps);
                        HWND list=GetDlgItem(hwnd,IDC_TARGET);
                        GetWindowText(list,s,sizeof(s));
                        SendMessage(list,CB_RESETCONTENT,0,0);

                        switch(i){
                            case 0:{
                                void  * p=GetCurrentProject();
                                if(p){
                                    TSortedCollection *classes= GetClasses();
                                    for(C_TYPE i=0;i<classes->count;i++){
                                        PClassListInfo pcli=(PClassListInfo)classes->At(i);
                                        if(pcli->GetProject()==p){
                                            SendMessage(list,CB_ADDSTRING,0,(LPARAM)pcli->GetClassName());
                                        }
                                    }
                                }}break;
                            case 4:{  int i=IDS_SYSCOMMANDS;
                                char ss[256];
                                while(LoadString(HResource,i++,ss,sizeof(ss))){
                                    SendMessage(list,CB_ADDSTRING,0,(LPARAM)ss);
                                }
                            }break;
                        }
                        SetWindowText(list,s);
                    }break;
                case IDC_HOTSPOT:{
                    ProcessHotspot(hwnd);
                }return TRUE;
                case IDC_ADVANSED:{
                    //TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    //HSP2D hsp=b->hsp;
                    // int obj=b->iuser1;
                };return TRUE;
                case IDC_EFFECT:{
                    int cs=(int)SendDlgItemMessage(hwnd,IDC_EFFECT,CB_GETCURSEL,0,0);
                    EnableWindow(GetDlgItem(hwnd,IDC_EFFECTSETUP),cs!=0);
                };return TRUE;
                case IDC_EFFECTSETUP:{
                    //TPropertyButton*b=GetDlgPropertyButton(hwnd);
                    //HSP2D hsp=b->hsp;
                    //int obj=b->iuser1;
                };return TRUE;
            }return FALSE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};


////////////////////////////////////////////////////////////////////////////////

BOOL Object2dDialog(HWND hwnd,HSP2D hsp,HOBJ2D object,int type,PClass _class)
{
    if (!type)type=GetObjectType2d(hsp,object);
    int active=2;
    TPropertyButton*b=new TPropertyButton(IDS_str16,HResource,"Obj_Placement",Object2D_options___Place); //"Положение"
    b->Procedure=ObjPlaceProc;
    b->data=SetSpaceParam2d(hsp,SP_GETREADONLY,0);
    TPropertyButton*b1;
    if (type==otLINE2D)
    {
        b1=new TPropertyButton(IDS_str17,HResource,"Obj_Pen",Object2D_options___Line); //"Линия"
        (HOBJ2D)(b1->di.d1) = GetPenObject2d(hsp,object);
        b1->Procedure=ObjPenProc;
        b->Insert(b1);
    }
    if (type==otLINE2D || type==otRGROUP2D)
    {
        b1=new TBrushTProperty(IDS_str18,Object2D_options___Fill);
        //  TPropertyButton(IDS_str18,HResource,"Obj_Brush",Object2D_options___Fill); //"Заливка"
        if (type==otLINE2D)
            ((HOBJ2D)b1->di.d1)=GetBrushObject2d(hsp,object);
        else
            b1->data=(DWORD)GetBrushObject2d(hsp,GetGroupItem2d(hsp,object,0));
        //  b1->Procedure=ObjBrushProc;
        b->Insert(b1);
    }
    if (type==otLINE2D)
    {
        b1=new TPropertyButton(IDS_str205,HResource,"Obj_linePoints",Object2D_options___Points); //"Линия"
        (HOBJ2D)(b1->di.d1) = GetPenObject2d(hsp,object);
        b1->Procedure=ObjPointsProc;
        b->Insert(b1);
    }
    if (type==otBITMAP2D || type==otDOUBLEBITMAP2D)
    {
        BOOL showrect=TRUE;
        HOBJ2D dib=0;
        if (type==otBITMAP2D)
        {
            dib=GetDibObject2d(hsp,object);
            if(GetDib2d(hsp,dib)==0)
                showrect=0;
        }
        else
        {
            dib=GetDoubleDibObject2d(hsp,object);
            if(GetXorDib2d(hsp,dib)==0)showrect=0;
        }
        if(showrect)
        {
            b1=new TPropertyButton(IDS_str19,HResource,"Obj_Bmp",Object2D_options___BMP); //"Битовая карта"
            if (type==otBITMAP2D)
                (HOBJ2D)b1->di.d1=dib;
            else
                (HOBJ2D)b1->di.d1=dib;
            b1->Procedure=ObjBmpProc;
            b->Insert(b1);
            active=1;
        }
        BOOL ref=FALSE;
        if (type==otBITMAP2D)
        {
            INT16 t=GetToolType2d(hsp,DIB2D,dib);
            ref=(t==ttREFTODIB2D);
        }
        else
        {
            INT16 t=GetToolType2d(hsp,DOUBLEDIB2D,dib);
            ref=(t==ttREFTODOUBLEDIB2D);
        }
        if(ref)
        {
            b1=new TPropertyButton(IDS_str221,HResource,"Obj_Ref",0); //"Битовая карта"
            b1->Procedure=ObjBmpRefProc;
            b->Insert(b1);
            b1->di.d1=(type==otBITMAP2D);
            if(!showrect)active=1;
        }
    }

    if (type==otTEXT2D)
    {
#ifndef NOUSERICHEDIT
#ifdef WIN32
        if(!hRTFLib)
        {
            BYTE static try_to_load=1;
            if(try_to_load)
            {
                hRTFLib = LoadLibrary("RICHED20.DLL");
                try_to_load=0;
                if(!hRTFLib)_Message("LoadLibrary 'RICHED20.DLL' Failed.");
            }
        }
        if(hRTFLib)
        {
            b1=new TPropertyButton(IDS_str20,HResource,"Obj_Text_Rtf",Object2D_options___Text); //"Текст"
            (HOBJ2D)b1->di.d1=GetTextObject2d(hsp,object);
            b1->Procedure=ObjRtfProc;
            b->Insert(b1);
        }
        else
        {
#endif
#endif
            b1=new TPropertyButton(IDS_str20,HResource,"Obj_Text",Object2D_options___Text); //"Текст"
            (HOBJ2D)b1->di.d1=GetTextObject2d(hsp,object);
            b1->Procedure=ObjTextProc;
            b->Insert(b1);
#ifndef NOUSERICHEDIT
#ifdef WIN32
        }
#endif
#endif
        active=1;
    };

    if ((type==otGROUP2D)||(type==otRGROUP2D))
    {
        b1=new TPropertyButton(IDS_str21,HResource,"Obj_Group",Object2D_options___Group); //"Группа"
        b1->Procedure=ObjGroupProc;
        b->Insert(b1);
        active=1;
    };

    if (type==otVIEW3D2D)
    {
        b1=new TPropertyButton("3d",HResource,"Obj_Group",Object2D_options___View3D);
        b1->Procedure=Obj3dProc;
        b->Insert(b1);
        b1=new TPropertyButton(IDS_str204,HResource,"Space_Textures",Object2D_options___Materials);
        b1->Procedure=SpaceTexturesProc;
        b1->di.d1=1;
        b->Insert(b1);
        active=1;
    };

    if (type==otCONTROL2D)
    {
        HWND hwnd=GetControl2d(hsp,object);
        char _type[64];
        if ( GetClassName(hwnd,_type,sizeof(_type)))
        {
            if (!lstrcmpi(_type,"COMBOBOX"))
            {
                b1=new TPropertyButton("ComboBox",HResource,"Obj_Combo",Object2D_options___Windows_elemens);
                b1->Procedure=ObjControlProc;
                b1->di.d1=OT_COMBOBOX;
                b->Insert(b1);
                active=1;
            }
            if (!lstrcmpi(_type,"EDIT"))
            {
                b1=new TPropertyButton("Edit",HResource,"Obj_Edit",Object2D_options___Windows_elemens);
                b1->Procedure=ObjControlProc;
                b1->di.d1=OT_EDIT;
                b->Insert(b1);
                active=1;
            }
            if (!lstrcmpi(_type,"BUTTON"))
            {
                b1=new TPropertyButton("Button",HResource,"Obj_Button",Object2D_options___Windows_elemens);
                b1->Procedure=ObjControlProc;
                b1->di.d1=OT_BUTTON;
                b->Insert(b1);
                active=1;
            }
            if (!lstrcmpi(_type,"LISTBOX"))
            {
                b1=new TPropertyButton("Listbox",HResource,"Obj_Listbox",Object2D_options___Windows_elemens);
                b1->Procedure=ObjControlProc;
                b1->di.d1=OT_LISTBOX;
                b->Insert(b1);
                active=1;
            }
        }
    }

    if (IsVideoFrame2d(hsp,object))
    {
        b1=new TPropertyButton(IDS_str206,HResource,"Obj_Bmp",Object2D_options___BMP);
        b1->di.d1=0;
        b1->Procedure=ObjBmpProc;
        b->Insert(b1);
        active=1;
        b1=new TPropertyButton(IDS_str207,HResource,"Obj_Video",Object2D_options___Video);
        b1->di.d1=0;
        b1->Procedure=ObjVideoProc;
        b->Insert(b1);
    }

    active=1;
    b1=new TPropertyButton(IDS_str10,HResource,"Obj_Hyper",Object2D_options___Hyperbase); //"Гипербаза"
    b1->Procedure=ObjHyperProc;
    b->Insert(b1);
    TCLASS_ITEMS ci;
    if(_class&& _class->varcount)
    {
        b1=new TPropertyButton(IDS_str6,HResource,"Obj_Vars",Object2D_options___Vars);
        b1->Procedure=ObjVarsProc;
        ci._class=_class;
        b1->pdata=&ci;
        ci.items=NULL;
        b->Insert(b1);
    }

    b1=new TPropertyButton(IDS_str1,HResource,"Obj_Info",Object2D_options___Info); //"Инфо"
    b1->Procedure=ObjInfoProc;
    b->Insert(b1);
    b1=b;
    while (b1)
    {
        b1->hsp=hsp;
        b1->iuser1=object;
        b1=b1->next;
    }

    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str24),b,active);
    delete b;
    if(type==otTEXT2D && rez)
    {
        POINT2D _size;GetActualSize2d(hsp,object,&_size);
        POINT2D size;GetObjectOrgSize2d(hsp,object,NULL,&size);
        if (size.x!=_size.x || size.y!=_size.y)SetObjectSize2d(hsp,object,&_size);
    }
    return rez;
};

BOOL scCALLBACK Obj3dPlaceProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=GetSpace2dBy3d((HSP3D)b->hsp);
            if(SetSpaceParam2d(hsp,SP_GETREADONLY,0)){
                EnableWindow(GetDlgItem(GetParent(hwnd),IDOK),0);
            }
        }break;

        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP3D hsp3d=(HSP3D)b->hsp;
            HOBJ2D object=b->iuser1;
            POINT3D org,size;
            GetObjectOrgSize3d(hsp3d,object,&org,&size);
            POINT3D base;
            GetObjectBase3d(hsp3d,object,&base);
            INT16 type=GetObjectType2d((HSP2D)hsp3d,object);
            switch (type){
                case otLIGHT3D:
                case otCAMERA3D:{
                    EnableWindow(GetDlgItem(hwnd,IDC_SIZEX3d),0);
                    EnableWindow(GetDlgItem(hwnd,IDC_SIZEY3d),0);
                    EnableWindow(GetDlgItem(hwnd,IDC_SIZEZ3d),0);
                }break;
                case otOBJECT3D:break;
            }
            SetDlgItemDouble(hwnd,IDC_ORGX3d,base.x);
            SetDlgItemDouble(hwnd,IDC_ORGY3d,base.y);
            SetDlgItemDouble(hwnd,IDC_ORGZ3d,base.z);
            SetDlgItemDouble(hwnd,IDC_SIZEX3d,size.x);
            SetDlgItemDouble(hwnd,IDC_SIZEY3d,size.y);
            SetDlgItemDouble(hwnd,IDC_SIZEZ3d,size.z);
            char s[256];s[0]=0;
            GetObjectName3d(hsp3d,object,s,255);
            SetDlgItemText(hwnd,IDC_OBJECTNAME,s);

            WORD atr=GetAttribute2d((HSP2D)hsp3d,object);
            if (atr&OF_DONTSELECT)CheckDlgButton(hwnd,IDC_NOSELECT,1);
            if (atr&1)CheckDlgButton(hwnd,IDC_HIDDEN,1);


        }return TRUE;
        case PSP_VALID:{

            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                HSP3D hsp3d=(HSP3D)b->hsp;
                HOBJ2D object=b->iuser1;

                if (wParam==1){
                    {
                        POINT3D _org,_size,_base;
                        if(
                           IsDoubleValid(hwnd,IDC_ORGX3d,_base.x,-32000,32000)&&
                           IsDoubleValid(hwnd,IDC_ORGY3d,_base.y,-32000,32000)&&
                           IsDoubleValid(hwnd,IDC_ORGZ3d,_base.z,-32000,32000)&&
                           IsDoubleValid(hwnd,IDC_SIZEX3d,_size.x,-10000,10000)&&
                           IsDoubleValid(hwnd,IDC_SIZEY3d,_size.y,-10000,10000)&&
                           IsDoubleValid(hwnd,IDC_SIZEZ3d,_size.z,-10000,10000)){
                            POINT3D org,size;
                            GetObjectOrgSize3d(hsp3d,object,&org,&size);
                            POINT3D base;
                            GetObjectBase3d(hsp3d,object,&base);
                            if(base.x!=_base.x || base.y!=_base.y || base.z!=_base.z)
                                SetObjectBase3d(hsp3d,object,&_base);

                            /*
      if(org.x!=_org.x || org.y!=_org.y || org.z!=_org.z)
        SetObjectOrg3d(hsp3d,object,&_org);
      if(size.x!=_size.x || size.y!=_size.y || size.z!=_size.z)
        SetObjectSize3d(hsp3d,object,&_size);
      */
                            char  s[256];
                            GetObjectName3d(hsp3d,object,s,255);
                            char ss[256];
                            GetDlgItemText(hwnd,IDC_OBJECTNAME,ss,255);

                            if(lstrcmp(s,ss)){ SetObjectName3d(hsp3d,object,ss); }

                            WORD atr=GetAttribute2d((HSP2D)hsp3d,object);
                            WORD _atr=atr;
                            BOOL noselect=IsDlgButtonChecked(hwnd,IDC_NOSELECT);
                            BOOL hidden=IsDlgButtonChecked(hwnd,IDC_HIDDEN);
                            if (noselect)atr|=OF_DONTSELECT;else atr&=~OF_DONTSELECT;
                            if (hidden)atr|=1;else atr&=~1;
                            if (atr!=_atr){SetAttribute2d((HSP2D)hsp3d,object,OF_DONTSELECT,noselect?ATTRSET:ATTRRESET);
                                SetAttribute2d((HSP2D)hsp3d,object,1,hidden?ATTRSET:ATTRRESET);
                            }
                        }}}
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};



BOOL scCALLBACK Obj3dMaterialProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP3D hsp3d=(HSP3D)b->hsp;
            HOBJ2D object=b->iuser1;
            union{
                COLOR3D color;
                COLORREF colorref;
            };
            GetObjectColor3d(hsp3d,object,&color);
            SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_SETCOLOR,0,colorref);
            UINT16 attr=GetAttribute3d(hsp3d,object);
            CheckDlgButton(hwnd,IDC_WIREFRAME,(attr&STATE_WIREFRAME)!=0);
            CheckDlgButton(hwnd,IDC_NOSHADED,(attr&STATE_NOSHADED)!=0);
            CheckDlgButton(hwnd,IDC_ALLFACES,(attr&STATE_ALLFACES)!=0);

        }return TRUE;
        case PSP_CLOSE:{
            if (wParam==1){
                TPropertyButton*b=(TPropertyButton*)lParam;
                HSP3D hsp3d=(HSP3D)b->hsp;
                HOBJ2D object=b->iuser1;
                union{
                    COLOR3D color;
                    COLORREF colorref;
                };
                COLORREF _color;
                INT16 rez=GetObjectColor3d(hsp3d,object,&color);
                _color=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,0);
                UINT16 attr=GetAttribute3d(hsp3d,object);
                UINT16 _attr=attr&~(STATE_WIREFRAME|STATE_NOSHADED|STATE_ALLFACES);
                if(IsDlgButtonChecked(hwnd,IDC_WIREFRAME))_attr|=STATE_WIREFRAME;
                if(IsDlgButtonChecked(hwnd,IDC_NOSHADED))_attr|=STATE_NOSHADED;
                if(IsDlgButtonChecked(hwnd,IDC_ALLFACES))_attr|=STATE_ALLFACES;

                if (SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_ISCHANGE,0,0)){
                    colorref=_color;
                    SetObjectColor3d(hsp3d,object,&color);
                }
                if (_attr!=attr)SetAttribute3d(hsp3d,object,_attr,ATTRPUT);
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK Obj3dCam_Loc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            HSP2D hsp=GetSpace2dBy3d((HSP3D)b->hsp);
            if(SetSpaceParam2d(hsp,SP_GETREADONLY,0)){
                EnableWindow(GetDlgItem(GetParent(hwnd),IDOK),0);
            }
        }break;
        case PSP_INITPAGE:{

            TPropertyButton*b=(TPropertyButton*)lParam;

            CAMERA3D& camera=*((CAMERA3D*)(b->pdata));
            HSP3D hsp3d=(HSP3D)b->hsp;
            HOBJ2D object=b->iuser1;

            SetDlgItemDouble(hwnd,IDC_ORGXc,camera.poOrg.x);
            SetDlgItemDouble(hwnd,IDC_ORGYc,camera.poOrg.y);
            SetDlgItemDouble(hwnd,IDC_ORGZc,camera.poOrg.z);
            HWND target=GetDlgItem(hwnd,IDC_TARGETc);

            char s[256];
            LoadStr(IDS_str25,s);
            SendMessage(target,CB_ADDSTRING,0,(LPARAM)s); // Смещение
            LoadStr(IDS_str26,s);
            SendMessage(target,CB_ADDSTRING,0,(LPARAM)s); // Цель
            SendMessage(target,CB_SETCURSEL,(camera.poFlags&CAMERA_TARGET)?1:0,0);

            SetDlgItemDouble(hwnd,IDC_SIZEXc,camera.poDirection.x);
            SetDlgItemDouble(hwnd,IDC_SIZEYc,camera.poDirection.y);
            SetDlgItemDouble(hwnd,IDC_SIZEZc,camera.poDirection.z);

            SetDlgItemDouble(hwnd,IDC_UPX,camera.poUp.x);
            SetDlgItemDouble(hwnd,IDC_UPY,camera.poUp.y);
            SetDlgItemDouble(hwnd,IDC_UPZ,camera.poUp.z);
            s[0]=0;
            GetObjectName3d(hsp3d,object,s,255);
            SetDlgItemText(hwnd,IDC_OBJECTNAME,s);

        }return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                HSP3D hsp3d=(HSP3D)b->hsp;
                HOBJ2D object=b->iuser1;
                CAMERA3D& camera=*((CAMERA3D*)(b->pdata));

                if( IsDoubleValid(hwnd,IDC_ORGXc,camera.poOrg.x,-1e9,1e9)&&
                    IsDoubleValid(hwnd,IDC_ORGYc,camera.poOrg.y,-1e9,1e9)&&
                    IsDoubleValid(hwnd,IDC_ORGZc,camera.poOrg.z,-1e9,1e9)&&

                    IsDoubleValid(hwnd,IDC_SIZEXc,camera.poDirection.x,-1e9,1e9)&&
                    IsDoubleValid(hwnd,IDC_SIZEYc,camera.poDirection.y,-1e9,1e9)&&
                    IsDoubleValid(hwnd,IDC_SIZEZc,camera.poDirection.z,-1e9,1e9)&&

                    IsDoubleValid(hwnd,IDC_UPX,camera.poUp.x,-1e9,1e9)&&
                    IsDoubleValid(hwnd,IDC_UPY,camera.poUp.y,-1e9,1e9)&&
                    IsDoubleValid(hwnd,IDC_UPZ,camera.poUp.z,-1e9,1e9)){
                    b->Valid();
                    if (wParam==1){
                        int dirtype=(int)SendDlgItemMessage(hwnd,IDC_TARGETc,CB_GETCURSEL,0,0);
                        if (dirtype==1)camera.poFlags|=CAMERA_TARGET;
                        else camera.poFlags&=~CAMERA_TARGET;
                        char s[256];
                        GetObjectName3d(hsp3d,object,s,255);
                        char ss[256];
                        GetDlgItemText(hwnd,IDC_OBJECTNAME,ss,255);
                        if(lstrcmp(s,ss)){ SetObjectName3d(hsp3d,object,ss); }
                        //        _camera=camera;
                    }
                }else b->Invalid();
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK Obj3dCam_Scale(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case PSP_INITPAGE:{

            TPropertyButton*b=(TPropertyButton*)lParam;
            CAMERA3D& camera=*((CAMERA3D*)(b->pdata));
            //HSP3D hsp3d=(HSP3D)b->hsp;
            // HOBJ2D object=b->iuser1;

            SetDlgItemDouble(hwnd,IDC_SCALEX,camera.poExtent.x);
            SetDlgItemDouble(hwnd,IDC_SCALEY,camera.poExtent.y);
            SetDlgItemDouble(hwnd,IDC_SCALEZ,camera.poExtent.z);
            SetDlgItemDouble(hwnd,IDC_NEARCLIP,camera.poNear_Clip);
            SetDlgItemDouble(hwnd,IDC_FARCLIP,camera.poFar_Clip);

            SetDlgItemDouble(hwnd,IDC_DELTAY,camera.poOffset.y);
            SetDlgItemDouble(hwnd,IDC_DELTAX,camera.poOffset.x);

            SetDlgItemDouble(hwnd,IDC_FOCUS,camera.poFocus);

            HWND target=GetDlgItem(hwnd,IDC_PERSPECTIVE);
            char s[256];
            LoadStr(IDS_str27,s);

            SendMessage(target,CB_ADDSTRING,0,(LPARAM)s); //"Без перспективы"
            LoadStr(IDS_str28,s);
            SendMessage(target,CB_ADDSTRING,0,(LPARAM)s); //"Прямая"

            SendMessage(target,CB_SETCURSEL,(camera.poFlags&CAMERA_PERSPECTIVE)?1:0,0);

        }return TRUE;
        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if (b->flags&PSP_INITIALIZE){
                CAMERA3D& _camera=*((CAMERA3D*)(b->pdata));
                CAMERA3D camera=_camera;
                //     HSP3D hsp3d=(HSP3D)b->hsp;
                //     HOBJ2D object=b->iuser1;

                if ( IsDoubleValid(hwnd,IDC_SCALEX,camera.poExtent.x,0,1e9)&&
                     IsDoubleValid(hwnd,IDC_SCALEY,camera.poExtent.y,0,1e9)&&
                     IsDoubleValid(hwnd,IDC_SCALEZ,camera.poExtent.z,0,1e9)&&

                     IsDoubleValid(hwnd,IDC_DELTAY,camera.poOffset.y,0,1)&&
                     IsDoubleValid(hwnd,IDC_DELTAX,camera.poOffset.x,0,1)&&
                     IsDoubleValid(hwnd,IDC_NEARCLIP,camera.poNear_Clip,-1e9,1e9)&&

                     IsDoubleValid(hwnd,IDC_FARCLIP,camera.poFar_Clip,-1e9,1e9)&&
                     IsDoubleValid(hwnd,IDC_FOCUS,camera.poFocus,-1,1e9)
                     ){
                    b->Valid();

                    if (wParam==1){
                        int persp=(int)SendDlgItemMessage(hwnd,IDC_PERSPECTIVE,CB_GETCURSEL,0,0);

                        if(persp)camera.poFlags|=CAMERA_PERSPECTIVE;else

                            camera.poFlags&=~CAMERA_PERSPECTIVE;
                        _camera=camera;
                    }
                }else b->Invalid();
            }
        }return TRUE;
    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};

BOOL scCALLBACK Obj3dCam_Render(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case (int)IDC_3DRENDERSETUP:{
                    char s[256];
                    GetDlgItemText(hwnd,IDC_RENDER,s,sizeof(s));
                    Driver3dDialog(hwnd,s);
                }break;
            }
        }break;
        case PSP_INITPAGE:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            CAMERA3D& camera=*((CAMERA3D*)(b->pdata));
            //	   HSP3D hsp3d=(HSP3D)b->hsp;
            //	   HOBJ2D object=b->iuser1;
            HWND target=GetDlgItem(hwnd,IDC_RENDER);
            int k=-1,i=0;
            INT16 render=0;
            char name[256];

            while ((render=GetNextRenderDriver3d(render,name,sizeof(name)))!=0){
                SendMessage(target,CB_ADDSTRING,0,(LPARAM)name);
                if (!lstrcmpi(name,camera.poRenderName))k=i;
                i++;
            }
            if (k>-1)SendMessage(target,CB_SETCURSEL,k,0);
            else{SetWindowText(target,camera.poRenderName);
            }

            HWND rm=GetDlgItem(hwnd,IDC_RENDERMODE);
            /*
    for(i=0;i<sizeof(render_mode)/4;i++){
       SendMessage(rm,CB_ADDSTRING,0,(LPARAM)render_mode[i]);
    }*/
            i=IDS_RENDERMODES;
            while(LoadString(HResource,i++,name,sizeof(name))){
                SendMessage(rm,CB_ADDSTRING,0,(LPARAM)name);
            }

            SendMessage(rm,CB_SETCURSEL,camera.poRenderType&RENDER_TYPEMASK,0);

            if (camera.poRenderType&RENDER_SHADED)CheckDlgButton(hwnd,IDC_SHADING,1);
            if (camera.poRenderType&RENDER_USELIGHT)CheckDlgButton(hwnd,IDC_USELIGHT,1);
            if (camera.poRenderType&RENDER_FOG)CheckDlgButton(hwnd,IDC_FOG,1);
            if (camera.poRenderType&RENDER_ZSORT)CheckDlgButton(hwnd,IDC_ZSORT,1);
            if (camera.poRenderType&RENDER_ALLFACES)CheckDlgButton(hwnd,IDC_ALLFACES,1);
            if (camera.poRenderType&RENDER_WAITCURSOR)CheckDlgButton(hwnd,IDC_WAITCURSOR,1);
            if (camera.poRenderType&RENDER_NOBUFFER)CheckDlgButton(hwnd,IDC_NOBUFFER,1);
            if (camera.poRenderType&RENDER_TRANSPARENT)CheckDlgButton(hwnd,IDC_TRANSPARENT,1);
            if (camera.poRenderType&RENDER_TEXTURED)CheckDlgButton(hwnd,IDC_camTEXTURES,1);


            SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_SETCOLOR,0,camera.poBackground);
        }return TRUE;

        case PSP_VALID:{
            TPropertyButton*b=(TPropertyButton*)lParam;
            if(b->flags&PSP_INITIALIZE){
                if (wParam==1){


                    CAMERA3D& camera=*((CAMERA3D*)(b->pdata));
                    camera.poBackground=SendDlgItemMessage(hwnd,IDC_COLOR,COLORB_GETCOLOR,0,0);
                    if(IsDlgButtonChecked(hwnd,IDC_SHADING)   )camera.poRenderType|=RENDER_SHADED;else camera.poRenderType&=~RENDER_SHADED;
                    if(IsDlgButtonChecked(hwnd,IDC_USELIGHT)  )camera.poRenderType|=RENDER_USELIGHT;else camera.poRenderType&=~RENDER_USELIGHT;
                    if(IsDlgButtonChecked(hwnd,IDC_FOG     )  )camera.poRenderType|=RENDER_FOG;else camera.poRenderType&=~RENDER_FOG;
                    if(IsDlgButtonChecked(hwnd,IDC_ZSORT   )  )camera.poRenderType|=RENDER_ZSORT;else camera.poRenderType&=~ RENDER_ZSORT;
                    if(IsDlgButtonChecked(hwnd,IDC_ALLFACES)  )camera.poRenderType|=RENDER_ALLFACES;else camera.poRenderType&=~RENDER_ALLFACES;
                    if(IsDlgButtonChecked(hwnd,IDC_WAITCURSOR))camera.poRenderType|=RENDER_WAITCURSOR;else camera.poRenderType&=~RENDER_WAITCURSOR;
                    if(IsDlgButtonChecked(hwnd,IDC_NOBUFFER))camera.poRenderType|=RENDER_NOBUFFER;else camera.poRenderType&=~RENDER_NOBUFFER;
                    if(IsDlgButtonChecked(hwnd,IDC_TRANSPARENT))camera.poRenderType|=RENDER_TRANSPARENT;else camera.poRenderType&=~RENDER_TRANSPARENT;

                    if(IsDlgButtonChecked(hwnd,IDC_camTEXTURES))camera.poRenderType|=RENDER_TEXTURED;else camera.poRenderType&=~RENDER_TEXTURED;

                    int RMODE=(int)SendDlgItemMessage(hwnd,IDC_RENDERMODE,CB_GETCURSEL,0,0);
                    GetDlgItemText(hwnd,IDC_RENDER,camera.poRenderName,sizeof(camera.poRenderName));
                    camera.poRenderType&=~RENDER_TYPEMASK;
                    camera.poRenderType|=RMODE;
                }
            }
        }return TRUE;

    }
    return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL TexturesDialog(HWND hwnd,HSP3D hsp3d){
    TPropertyButton*b=NULL;
    b=new TPropertyButton(IDS_str204,HResource,"Space_Textures",0);
    b->Procedure=SpaceTexturesProc;
    b->di.d1=0;
    b->hsp=(HSP2D)hsp3d;
    int rez=CreatePropertySheet(hwnd,MAKEINTRESOURCE(IDS_str208),b,0);
    delete b;
    return rez;


};
BOOL Object3dDialog(HWND hwnd,HSP3D hsp,HOBJ2D object){
    INT16 type=GetObjectType2d((HSP2D)hsp,object);
    TPropertyButton*b=NULL,*b1;
    CAMERA3D camera;

    switch(type){
        case otCAMERA3D:{
            GetCamera3d(hsp,object,&camera);
            b=new TPropertyButton(IDS_str69,HResource,"Obj3d_Cam_Location",Object_3D_options___Camera_place);
            b->Procedure=Obj3dCam_Loc;
            b->pdata=&camera;

            b1=new TPropertyButton(IDS_str68,HResource,"Obj3d_Camera",Object_3D_options___Scale);
            b1->Procedure=Obj3dCam_Scale;
            b1->pdata=&camera;
            b->Insert(b1);


            b1=new TPropertyButton(IDS_str67,HResource,"Obj3d_Cam_Render",Object_3D_options___Driver);
            b1->Procedure=Obj3dCam_Render;
            b1->pdata=&camera;
            b->Insert(b1);
        }break;
        default:
            b=new TPropertyButton(IDS_str16 //Положение
                                  ,HResource,"Obj3d_Placement",Object2D_options___Info);
            b->Procedure=Obj3dPlaceProc;

            b1=new TPropertyButton(IDS_str32,HResource,"Obj3d_Material",Object_3D_options___Misc);
            b1->Procedure=Obj3dMaterialProc;
            b->Insert(b1);
    }

    if (type==otGROUP3D){
        b1=new TPropertyButton(IDS_str21,HResource,"Obj_Group",Object_3D_options___Info);
        b1->Procedure=ObjGroupProc;
        b->Insert(b1);
    }


    b1=new TPropertyButton(IDS_str1,HResource,"Obj_Info",Object_3D_options___Info);
    b1->Procedure=ObjInfoProc;
    b->Insert(b1);

    b1=b;

    while (b1){
        b1->hsp=(HSP2D)hsp;
        b1->iuser1=object;
        b1=b1->next;
    };
    int rez=CreatePropertySheet(hwnd,INTtoCHAR(IDS_str70),b);
    if(rez){
        if(type==otCAMERA3D)
        {
            CAMERA3D _camera;
            GetCamera3d(hsp,object,&_camera);
            if (memcmp(&_camera,&camera,sizeof(camera))){
                SetCamera3d(hsp,object,&camera);
            }
        }
    }
    delete b;
    return rez;
};

char *rop2_st[]={
    "BLACK",
    "NOTMERGEPEN",
    "MASKNOTPEN",
    "NOTCOPYPEN",
    "MASKPENNOT",
    "NOT",
    "XORPEN",
    "NOTMASKPEN",
    "MASKPEN",
    "NOTXORPEN",
    "NOP",
    "MERGENOTPEN",
    "COPYPEN",
    "MERGEPENNOT",
    "MERGEPEN",
    "WHITE"};

void GetRopString(INT16 r,char *s){
    *s=0;
    if (r>0 && r<=(sizeof(rop2_st)/4))
    {lstrcpy(s,rop2_st[r-1]);
    }
};
void GetPenStyleString(INT16 r,char *s){
    *s=0;
    if (r>=0 && r<(sizeof(pen_st)/4))
    {lstrcpy(s,pen_st[r]);
    }
};
void GetBrushStyleString(INT16 style,char *s){
    *s=0;
    for(int i=0;i<4;i++){
        if (_lbs[i].code==style){
            lstrcpy(s,_lbs[i].text);
            return;
        }}
};
void EndAbout(HWND hwnd,BOOL r){
    // HFONT HFont = (HFONT )GetWindowLong(hwnd,DWL_USER,(LONG)HFont);
    HFONT HFont = (HFONT )SendDlgItemMessage(hwnd,IDC_WARNING,WM_GETFONT,(WPARAM)HFont,0);
    SendDlgItemMessage(hwnd,IDC_WARNING,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
    DeleteObject(HFont);
    EndDialog(hwnd,r);
}
#ifdef PROTECT
BOOL scCALLBACK RemindProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            ShowWindow(GetDlgItem(hwnd,IDC_ORDERINFO),SW_HIDE);
        }return TRUE;
        case WM_COMMAND:
            EV_COMMANDsimply
                    switch(command){
        case IDHELP:{
                    ShowWindow(GetDlgItem(hwnd,IDC_ORDERINFO),SW_SHOW);
                    EnableWindow(GetDlgItem(hwnd,IDHELP),0);
                    SetDlgItemText(hwnd,IDC_ORDERINFO,
                                   "You can view the detailed order form\r\n on the Stratum 2000 web site or in the help file.\r\n"
                                   "If You select 'Web site' Stratum 2000 will launch Your web browser to connect to the Stratum 2000 web site "
                                   "( http://www.stratum.ac.ru/scorder.htm )\r\n\r\n"
                                   "The quickest method being ONLINE REGISTRATION via a SECURE SERVER with a credit card.\r\n"
                                   "You can also purchase and register STRATUM 2000 via a TOLL-FREE Voice Order Line at"
                                   "1-877-REGSOFT (1-877-734-7638), or by fax at 770-497-9234.\r\n"
                                   "The Product ID#10155 is REQUIRED for both Voice and Fax orders.\r\n"
                                   "Copyright (c) 1991-1999 STRATUME MODELING GROUP."
                                   );
                    HWND idc=GetDlgItem(hwnd,IDOK);
                    SetWindowText(idc,"&Web Site");
#ifdef WIN32
                    SetWindowLong(idc,GWL_ID,6);
#else
                    SetWindowWord(idc,GWW_ID,6);
#endif
                    SetDlgItemText(hwnd,IDC_REGISTER,"&Help file");
                };return TRUE;
                case IDC_REGISTER:EndDialog(hwnd,IsWindowVisible(GetDlgItem(hwnd,IDC_ORDERINFO))?5:2);return TRUE;
                case IDCANCEL:EndDialog(hwnd,IsWindowVisible(GetDlgItem(hwnd,IDC_ORDERINFO))?1:0);return TRUE;
                case 6:EndDialog(hwnd,6);return TRUE;
                case IDOK:EndDialog(hwnd,1);return TRUE;
            }break;
    }
    return FALSE;
};
#endif

BOOL scCALLBACK AboutProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            EnableWindow(GetDlgItem(hwnd,CM_SYSTEMINFO),(lParam&1)==0);
            char s3[512];
#ifdef PROTECT
            if(IsRegistered()){
#endif
                char s1[256];
                //   char s2[128];
                char s4[128];

                SCGetScVarS("user_name",s1);
                //   SCGetScVarS("user_org",s2);
                SCGetScVarS("user_email",s4);
                //     wsprintf(s3," %s\r\n %s\r\n %s",s1,s2,s4);
                wsprintf(s3," %s\r\n %s",s1,s4);

#ifdef PROTECT

            } else{ lstrcpy(s3,"Unregistered version");
                char s[256];
                GetWindowText(hwnd,s,sizeof(s));
                lstrcat(s," - UNREGISTERED VERSION !");
                SetWindowText(hwnd,s);
            }
#endif
            SetDlgItemText(hwnd,IDC_USERNAME,s3);

            char s[32];
            LoadString(HResource,IDS_str120,s,sizeof(s));
            HFONT HFont = CreateFont(12, 0, 0, 0, 0, /* specify pt size    */
                                     0, 0, 0, SCcharset, 0, 0, 0, 0,s );
            SendDlgItemMessage(hwnd,IDC_WARNING,WM_SETFONT,(WPARAM)HFont,0);
            //     SetWindowLong(hwnd,DWL_USER,(LONG)HFont);
            if(lParam&2){
                RECT r,r1;
                GetWindowRect(hwnd,&r);
                GetWindowRect(GetDlgItem(hwnd,IDC_AGAIN),&r1);
                SetWindowPos(hwnd,0,0,0,r.right-r.left,(r.bottom-r.top)+(r1.bottom-r1.top)*5/4,SWP_NOMOVE|SWP_NOZORDER);
                CheckDlgButton(hwnd,IDC_AGAIN,1);
#ifdef RUNTIME
                EnableWindow(GetDlgItem(hwnd,CM_SYSTEMINFO),0);
#endif
            }else ShowWindow(GetDlgItem(hwnd,IDC_AGAIN),SW_HIDE);

#ifdef PROTECT
            if(IsRegistered())ShowWindow(GetDlgItem(hwnd,IDC_REGISTER),SW_HIDE);
#else
            ShowWindow(GetDlgItem(hwnd,IDC_REGISTER),SW_HIDE);
#endif

        }return TRUE;
        case WM_COMMAND:
            EV_COMMANDsimply
                    switch(command){
#ifdef PROTECT
        case IDC_REGISTER:if(Register()){
                        ShowWindow(GetDlgItem(hwnd,IDC_REGISTER),SW_HIDE);
                    }break;
#endif                       
                case IDC_FORUM:MSGBOX(hwnd,MAKEINTRESOURCE(IDS_str94),"FORUM",MB_OK|MB_ICONINFORMATION);break;
#ifndef RUNTIME
                case CM_SYSTEMINFO:SysInfoDialog(hwnd);return 0;
#endif
                case IDCANCEL:EndAbout(hwnd,0);return TRUE;
                case IDOK:
                    if(IsWindowVisible(GetDlgItem(hwnd,IDC_AGAIN))){
                        if(!IsDlgButtonChecked(hwnd,IDC_AGAIN)){
                            SCSetScVarI("OnStartup",0);
                        }
                    } EndAbout(hwnd,1);return TRUE;
            }break;
    }
    return FALSE;
};

BOOL scCALLBACK ScaleProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(Scale_dialog)
            static double*__scale;
    switch(msg){
        case WM_INITDIALOG:{
            INT16 a[]={5,10,25,33,50,100,150,200,300,400,500,600};
            __scale=((double*)lParam);
            int is=(*__scale)*100;
            char ss[20];
            sprintf(ss,"%d%%",is);
            int k=-1;
            for(int i=0;i<sizeof(a)/sizeof(a[0]);i++){
                char s[20];
                sprintf(s,"%d%%",(int)a[i]);
                SendDlgItemMessage(hwnd,IDC_SCALE,CB_ADDSTRING,0,(LPARAM)s);
                if(!lstrcmp(ss,s)){k=i;}
            }
            if(k>-1)SendDlgItemMessage(hwnd,IDC_SCALE,CB_SETCURSEL,k,0);else
                SetDlgItemText(hwnd,IDC_SCALE,ss);
        }return TRUE;
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch (command){
        case IDOK:{
                    char s[20];
                    GetDlgItemText(hwnd,IDC_SCALE,s,sizeof(s));
                    int l=lstrlen(s);
                    if (l>0){
                        if (s[l-1]=='%'){
                            s[l-1]=0;
                            double scale=atof(s);
                            if((scale>=0.01) && (scale <501)){
                                *__scale=scale/100.0;
                                EndDialog(hwnd,1);return TRUE;
                            }
                        }
                    }
                    MSGBOX(hwnd,MAKEINTRESOURCE(IDS_MSG8),_MSG_ERROR,MB_OK|MB_ICONINFORMATION);
                    return TRUE;
                }
                case IDCANCEL:{
                    EndDialog(hwnd,0);return TRUE;

                }
            }
        }break;
    }
    return FALSE;
};

double ScaleDialog(HWND hwnd,double s){
    int rez=DialogBoxParam(HResource,"Scale_Dialog",hwnd,ScaleProc,(LPARAM)&s);
    if(rez)return s;
    return 0;
};
int scCALLBACK EnumFontFamProc(const ENUMLOGFONT FAR*lpnlf,
                               const NEWTEXTMETRIC FAR*tm,int FontType, LPARAM  lParam)
{
    HWND combo=(HWND)lParam;
    if(!(tm->tmPitchAndFamily&TMPF_FIXED_PITCH))
    {
        SendMessage(combo,CB_ADDSTRING,0,(LPARAM)lpnlf->elfLogFont.lfFaceName);
    }
    return TRUE;
};


BOOL scCALLBACK SyntaxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    _STDHELP(Environment_options___Editor_options	)
            switch(msg){

        case WM_INITDIALOG:{
            HWND combo=GetDlgItem(hwnd,IDC_FONT);
            HDC hdc=GetDC(hwnd);
            EnumFontFamilies(
                        hdc,	// handle of device control
                        NULL,	// address of family-name string
                        (FONTENUMPROC)EnumFontFamProc,	// address of callback function
                        (LPARAM)combo 	// address of application-supplied data
                        );

            ReleaseDC(hwnd,hdc);
            char s[256];

            INT32 fs=SCGetScVar("syntax_edit_fontsize",10);
            if(!SCGetScVarS("syntax_edit_font",s))lstrcpy(s,"Courier");
            int i=(int)SendMessage(combo,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)s);

            SendMessage(combo,CB_SETCURSEL,i,0);
            combo=GetDlgItem(hwnd,IDC_FONTSIZE);

            for(i=7;i<25;i++){
                wsprintf(s,"%d",i);
                SendMessage(combo,CB_ADDSTRING,0,(LPARAM)s);
                if(i==fs){
                    SendMessage(combo,CB_SETCURSEL,i-7,0);
                }
            };

            UINT32 o=SCGetScVar("syntax_edit_opt",3);
            CheckDlgButton(hwnd,IDC_PERSISTENT,(o&1)!=0);
            CheckDlgButton(hwnd,IDC_INDENT, (o&2)!=0);
            CheckDlgButton(hwnd,IDC_BRIEF,  (o&4)!=0);
            CheckDlgButton(hwnd,IDC_VSCROLL,(o&8)!=0);
            CheckDlgButton(hwnd,IDC_HSCROLL,(o&16)!=0);

            SetDlgItemText(hwnd,IDC_TEXT,(char*)lParam);
        }return TRUE;
        case WM_COMMAND:
            EV_COMMANDsimply
                    switch(command){
        case IDOK:{
                    UINT32 no=0;
                    int i=SendDlgItemMessage(hwnd,IDC_FONTSIZE,CB_GETCURSEL,0,0)+7;

                    INT32 o=SCGetScVar("syntax_edit_fontsize",3);
                    if(o!=no)SCSetScVarI("syntax_edit_fontsize",i);
                    i=SendDlgItemMessage(hwnd,IDC_FONT,CB_GETCURSEL,0,0);
                    char s[256];
                    SendDlgItemMessage(hwnd,IDC_FONT,CB_GETLBTEXT,i,(LPARAM)s);
                    SCSetScVarS("syntax_edit_font",s);

                    if(IsDlgButtonChecked(hwnd,IDC_PERSISTENT))no|=1;
                    if(IsDlgButtonChecked(hwnd,IDC_INDENT))no|=2;
                    if(IsDlgButtonChecked(hwnd,IDC_BRIEF))no|=4;
                    if(IsDlgButtonChecked(hwnd,IDC_VSCROLL))no|=8;
                    if(IsDlgButtonChecked(hwnd,IDC_HSCROLL))no|=16;
                    o=SCGetScVar("syntax_edit_opt",3);
                    if(o!=no)SCSetScVarI("syntax_edit_opt",no);
                    EndDialog(hwnd,0);

                }return TRUE;
                case IDCANCEL:EndDialog(hwnd,0);return TRUE;

            }break;
    }
    return FALSE;
};
BOOL SyntaxSetupDialog(HWND hwnd){
    return DialogBoxParam(HResource,MAKEINTRESOURCE(EDITOR_OPTIONS),hwnd,SyntaxProc,0);
}
int AboutDialog(HWND hwnd,int a){
    RegisterIcons();
    return DialogBoxParam(HResource,MAKEINTRESOURCE(ABOUT_DIALOG),hwnd,AboutProc,a);
}
#ifdef PROTECT
BOOL ReminderDialog(HWND hwnd){
    switch(DialogBox(HResource,MAKEINTRESOURCE(REMIND_DIALOG),hwnd,RemindProc)){
        case 5:HELP("order form");return TRUE;
        case 6:LaunchURL("http://www.stratum.ac.ru/scorder.htm");return TRUE;
        case 3:return TRUE;
        case 2:return Register();break;
        case 1:return TRUE;
    }
    return FALSE;
};
#endif
void AddROP2(HWND hwnd,int id,int cur){

    for(int i=0;i<sizeof(rop2_st)/4;i++)
        SendDlgItemMessage(hwnd,id,CB_ADDSTRING,0,(LPARAM)rop2_st[i]);
    SendDlgItemMessage(hwnd,id,CB_SETCURSEL,cur-1,0);
}

extern "C" BOOL _SDKPROC SCObject2dDialog(HWND hwnd,HSP2D hsp,HOBJ2D object){
    return Object2dDialog(hwnd,hsp,object,0);
};
extern "C" BOOL _SDKPROC SCObject3dDialog(HWND hwnd,HSP3D hsp,HOBJ2D object){
    return Object3dDialog(hwnd,hsp,object);
};
extern "C" BOOL  _SDKPROC SCTextureDialog(HWND hwnd,HSP3D hsp3d,HOBJ2D ht){
    return TextureDialog(hwnd,hsp3d,ht);
};
extern "C" BOOL _SDKPROC SCSpacePrintDialog(HWND hwnd,HSP2D hsp){
    return SpacePrintDialog(hwnd,hsp);
};
/*
extern "C" BOOL _SDKPROC SCChoseColor(HWND hwnd,COLORREF & clr,char*title){
 return _ChoseColor(hwnd,clr,title);
};
extern "C" BOOL _SDKPROC SCAboutDialog(HWND hwnd,int mode){
 return AboutDialog(hwnd,mode);
};
extern "C" BOOL _SDKPROC SCMatrixDialog(HWND hwnd,int matrix){
 return MatrixDialog(hwnd,matrix);
};
extern "C" BOOL _SDKPROC SCChoseFontDialog(HWND parent,LOGFONT& lf,char * title){
 return ChoseFontDialog(parent,lf,title);
};
extern "C" BOOL _SDKPROC SCSysInfoDialog(HWND hwnd){
#ifdef RUNTIME
return FALSE;
#else
 return SysInfoDialog(hwnd);
#endif
};
extern "C" BOOL _SDKPROC SCEnvironmentDialod(HWND hwnd){
 return EnvironmentDialod(hwnd);
};
extern "C" BOOL _SDKPROC SCProjectDialog(HWND hwnd,TProject*project){
#ifdef RUNTIME
return FALSE;
#else
 return ProjectDialog(hwnd,project);
#endif
};
extern "C" int _SDKPROC SCIsAbsolutePath(char *file){
 return IsAbsolutePath(file);
};
// 0 - Ошибка
// 1 - Absolute
// 2 - relative
extern "C" void _SDKPROC SCAddSlash(char *p){
  AddSlash(p);
};
*/
HWND extern title;

int MSGBOX(HWND hwnd ,LPCSTR c1,LPCSTR c2,int c)
{
    char s1[256],s2[256];
    if(!HIWORD(c1))
    {
        if(!LoadString(HResource,LOWORD(c1),s1,sizeof(s1)))c1="**";
        else   c1=s1;
    }
    if(!HIWORD(c2))
    {
        if(!LoadString(HResource,LOWORD(c2),s2,sizeof(s2)))c2="**";
        else   c2=s2;
    }
    if(title)
        ShowWindow(title,SW_HIDE);
    return MessageBox(hwnd,c1,c2,c);
};

int CreatePropertySheet(HWND hwnd,const char*title,TPropertyButton*b,int active){
    return CreatePropertySheetEx(hwnd,title,b,active,"Property");
};


/*
BOOL SCClassDialog(HWND,PCPObject,PLibчbrary),PC,PC,PC,PC
BOOL SCLinksDialog(HWND hwnd,PClass _class,HSP2D hsp,HOBJ2D id);
BOOL SCInputBox(HWND hwnd,char *title,char*msg, char*s);

BOOL SCVariable2dDialog(HWND hwnd,PObject,INT16 index);
BOOL SCTableDialog(HWND hwnd,int table);
*/

char * colorname=0;
UINT scCALLBACK  ColorHook(HWND hwnd, UINT msg, WPARAM, LPARAM){
    switch(msg){
        case WM_INITDIALOG:{
            SetWindowText(hwnd,colorname);
            return TRUE;
        }
    }
    return FALSE;
};

COLORREF aclrCust[16]={
    RGB(0x00,0x00,0x00),RGB(0x00,0x00,0x80),RGB(0x00,0x80,0x00),
    RGB(0x00,0x80,0x80),RGB(0x80,0x00,0x00),RGB(0x80,0x00,0x80),
    RGB(0x80,0x80,0x00),RGB(0xC0,0xC0,0xC0),RGB(0x80,0x80,0x80),
    RGB(0x00,0x00,0xFF),RGB(0x00,0xFF,0x00),RGB(0x00,0xFF,0xFF),
    RGB(0xFF,0x00,0x00),RGB(0xFF,0x00,0xFF),RGB(0xFF,0xFF,0x00),
    RGB(0xFF,0xFF,0xFF)};
BOOL _ChoseColor(HWND hwnd,COLORREF & clr,char*name)
{
    CHOOSECOLOR cc;
    memset(&cc, 0, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hwnd;
    cc.rgbResult = clr;
    cc.lpCustColors = aclrCust;
    cc.Flags = CC_RGBINIT|CC_ENABLEHOOK ;
    cc.lpfnHook=ColorHook;
    colorname=name;
    if(ChooseColor(&cc))
    {
        colorname=0;
        clr=cc.rgbResult;
        return 1;
    }
    colorname=0;
    return 0;
}

HINSTANCE hRTFLib=0;
