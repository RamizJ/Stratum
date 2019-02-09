#include "_vmach.h"
#include "matrix.h"
#include "media.h"
#include <gwin2d3d\gwinfunc.h>
#include <mem.h>
#include "grtools.h"
#include "video\scvideo.h"
#include "dialogs.h"
#include "twindow.h"
#include "winman.h"
#include "hyper.h"
#include "stream_m.h"
// name "CreateMaterial3d" arg "HANDLE","STRING","STRING","COLORREF","FLOAT" ret "HANDLE" out xxx

void dbgetpixel()
{
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;
    HOBJ2D hdib=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    COLORREF c=DibGetPixel2d(hsp,hdib,x,y);
    PUSH_LONG(c);
}

void dbsetpixel()
{
    COLORREF c=POP_LONG;
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;
    HOBJ2D hdib=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(DibSetPixel2d(hsp,hdib,x,y,c));
}

void setrdib2d(){
    char * name=POP_PTR;
    HOBJ2D hdib=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    char s[256];
    pobject->GetFullFileName(name,s);
    PUSH_DOUBLE(SetRDib2d(hsp,hdib,s));
}
void setrddib2d(){
    char * name=POP_PTR;
    HOBJ2D hdib=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    char s[256];
    pobject->GetFullFileName(name,s);
    PUSH_DOUBLE(SetRDoubleDib2d(hsp,hdib,s));
}
void getrdib2d(){
    HOBJ2D hdib=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    char* f = GetTmpString();
    *f=0;
    GetRDib2d(hsp,hdib,f,256);
    PUSH_PTR(f)
}
void getrddib2d(){
    HOBJ2D hdib=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    char* f = GetTmpString();
    *f=0;
    GetRDoubleDib2d(hsp,hdib,f,256);
    PUSH_PTR(f)
}


void vm_sethyperjump()
{
    TCollection* items = new TCollection(10,10);
    int count = *((INT16*)codepoint);
    codepoint += 1;

    while(count)
    {
        char *s = POP_PTR;
        items->AtInsert(0,s);
        count--;
    }

    int mode = POP_DOUBLE;
    HOBJ2D obj = POP_HOBJ;
    HSP2D hsp = POP_HSP;

    if(mode >= 0)
    {
        THyperKey key(hsp, obj, NULL);
        key.SetMode(mode);
        int itemsCount = items->count;

        if(itemsCount > 5)
            itemsCount = 5;

        for(C_TYPE i = 0; i < itemsCount; i++)
        {
            char *s=(char*)items->At(i);
            switch(i)
            {
                case 0:
                    key.SetTarget(s);
                    break;

                case 1:
                    key.SetWinName(s);
                    break;

                case 2:
                    key.SetObject(s);
                    break;

                case 3:
                    key.SetEffect(s);
                    break;

                case 4:
                    key.SetAdd(s);
                    break;
            }
        }
        key.Update();
    }
    else
    {
        SetObjectData2d(hsp,obj,UD_HYPERKEY,NULL,0,0);
    }

    delete items;
    BOOL rez=GetObjectType2d(hsp,obj)!=0;
    PUSH_DOUBLE(rez)
}

void vm_getobject3dfrompoint2d()
{
    double * item=(double *)POP_PTR;
    LONG * primary=(LONG *)POP_PTR;
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HOBJ2D hobj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HOBJ2D  _primary;INT16 _item;
    HOBJ2D obj= GetObject3dFromPoint2d(hsp,hobj,&p,&_primary,&_item);
    if(obj){
        *primary=_primary;
        *item=_item;
    }
    PUSH_LONG(obj);

}
void vm_getnextobject()
{
    HOBJ2D hobj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    hobj = GetNextObject2d(hsp,hobj);
    PUSH_LONG(hobj);
}
void vm_getnexttool2d(){
    HOBJ2D hobj=POP_HOBJ;
    TOOL_TYPE type=(TOOL_TYPE)POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    hobj=GetNextTool2d(hsp,type,hobj);
    PUSH_LONG(hobj);
}

void vm_getmousepos()
{
    double*y=(double*)POP_PTR;
    double*x=(double*)POP_PTR;
    char  *win=POP_PTR;

    TWinItem*w=winmanager->GetWindow(win);
    if(w)
    {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(w->HWindow,&p);
        if(w->hsp)
        {
            POINT2D pp;
            pp.x = p.x;
            pp.y = p.y;
            DPtoLP2d(w->hsp,&pp,1);

            *x=pp.x;
            *y=pp.y;
        }
        else
        {
            *x=p.x;
            *y=p.y;
        }
        PUSH_DOUBLE(1);
    }
    else
    {
        *x=0;
        *y=0;
        PUSH_DOUBLE(0);
    }
}
void vm_loadmenu(){
    UINT32 flags=POP_DOUBLE;
    char  *menu=POP_PTR;
    char  *win=POP_PTR;
    char dst[256];
    pobject->GetFullFileName(menu,dst);
    PUSH_DOUBLE(winmanager->LoadMenu(win,dst,flags));
};
void vm_deletemenu(){
    char  *win=POP_PTR;
    PUSH_DOUBLE(winmanager->DeleteMenu(win));
};

void vm_checkmenu(){
    UINT32 f=POP_DOUBLE;
    UINT32 id=POP_DOUBLE;
    char  *win=POP_PTR;
    PUSH_DOUBLE(winmanager->CheckMenuItem(win,id,f));
};
void vm_enablemenu(){
    UINT32 f=POP_DOUBLE;
    UINT32 id=POP_DOUBLE;
    char  *win=POP_PTR;
    PUSH_DOUBLE(winmanager->EnableMenuItem(win,id,f));
};

void vm_folderdialog()
{
    UINT32 flags=POP_DOUBLE;
    char *dir=POP_PTR;
    char *title=POP_PTR;
    char *out=GetTmpString();
    lstrcpy(out,dir);
    if(!ChoseDirDialog(mainwindow->HWindow,out,title,flags))*out=0;
    PUSH_PTR(out);
}

void vm_getmaterials(){
    char * s=POP_PTR;
    HSP3D hsp3d=POP_HSP3D;
    HOBJ2D h=GetMaterialByName3d(hsp3d,s);
    PUSH_HOBJ(h);
}
void vm_creatematerial(){
    MATERIAL3D m;
    memset(&m,0,sizeof(m));
    m.poSize=sizeof(m);
    m.flags=POP_DOUBLE;
    m.transparency=POP_DOUBLE;
    m.shine=POP_DOUBLE;
    m.emittance.rgb=POP_LONG;
    m.specular.rgb=POP_LONG;
    m.diffuse.rgb=POP_LONG;
    m.color.rgb=POP_LONG;
    m.hfile=POP_PTR;
    m.name=POP_PTR;

    char dst[256];
    pobject->GetFullFileName(m.hfile,dst);
    m.hfile=dst;
    HSP3D hsp3d=POP_HSP3D;
    HOBJ2D h=CreateMaterial3d(hsp3d,&m);
    PUSH_HOBJ(h);
}
void vm_removetexture(){
    HOBJ2D obj=POP_HOBJ;
    HSP3D  hsp3d=POP_HSP3D;
    PUSH_DOUBLE(RemoveTexture3d(hsp3d,obj));
};
void vm_applytextutre(){
    int count = *((INT16*)codepoint); codepoint+=1;
    double *p=NULL;
    if(count){p=new double[count];
        INT16 i = count;
        while (i--)
        { p[i] = POP_DOUBLE; }
    }
    double tmaxc=1;
    double tminc=-1;
    double tv=1;
    double th=1;
    int flags=0;
    if(count>0){flags=p[0];
        if(count>1){th=p[1];
            if(count>2){tv=p[2];
                if(count>3){tminc=p[3];
                    if(count>4) tmaxc=p[4];
                }}}}
    if(p)delete p;
    HOBJ2D texture=POP_HOBJ;
    HOBJ2D hframe=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP3D  hsp3d=POP_HSP3D;
    PUSH_DOUBLE(ApplyTexture(hsp3d,obj,hframe,texture,flags,th,tv,tminc,tmaxc));
};

void vm_stringex(){
    char * cmd=POP_PTR;
    double f=POP_DOUBLE;
    /*
 char *out=GetTmpString();
 BOOL _int=FALSE;
 BOOL error=FALSE;
 BOOL alpha=FALSE;
 char *ps=cmd;
 while (*ps){
  if(isalpha(*ps)){
   if(alpha)error=TRUE;
   switch(*ps){
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    case 'X':_int=TRUE;break;
    case 'f':
    case 'e':
    case 'g':
    case 'E':
    case 'G':break;
    default:error=TRUE;
   }
   alpha=TRUE;
  }
  ps++;
 }
 char _cmd[256]="%";lstrcat(_cmd,cmd);
 if(_int)
 *out=0;
 PUSH_PTR(out);
 */
}

void vm_getvarcount()
{
    char *_classname=POP_PTR;
    PClass _class=GetClassByName(_classname);
    int count=0;
    if(_class)
    {
        count=_class->varcount;
    }
    PUSH_DOUBLE(count);
};

void vm_getvarinfo()
{
    char * info=POP_PTR;
    char * def=POP_PTR;
    char * type=POP_PTR;
    char * name=POP_PTR;
    int  i=POP_DOUBLE;
    char *_classname=POP_PTR;

    PClass _class=GetClassByName(_classname);
    if(_class && i>=0 && i<_class->varcount)
    {
        PVarInfo  pv=_class->vars+i;
        lstrcpy(info,pv->info);
        lstrcpy(def,pv->def);
        lstrcpy(type,pv->classType->GetClassName());
        lstrcpy(name,pv->name);
        PUSH_DOUBLE(1);
        return;
    }
    PUSH_DOUBLE(0)
};


void vm_getrvalue()
{
    COLORREF color=POP_LONG;
    PUSH_DOUBLE(GetRValue(color));
};

void vm_getgvalue()
{
    COLORREF color=POP_LONG;
    PUSH_DOUBLE(GetGValue(color));
};

void vm_getbvalue()
{
    COLORREF color=POP_LONG;
    PUSH_DOUBLE(GetBValue(color));
};


void v_createfromfile3d()
{
    char * file = POP_PTR;
    HSP3D hsp3d = POP_HSP3D;
    char dst[256];
    pobject->GetFullFileName(file,dst);
    HOBJ2D object3d=ImportObject3d(hsp3d,dst,0);
    PUSH_HOBJ(object3d);
}

void vm_setcalcorder()
{
    INT16 order=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    char * name = POP_PTR;
    PClass _class=GetClassByName(name);
    double rez=0;
    if(_class)
        rez = _class->SetObjectOrder(obj,order-1);

    PUSH_DOUBLE(rez);
}

void vm_getcalcorder()
{
    HOBJ2D obj=POP_HOBJ;
    char * name = POP_PTR;
    INT16 order=-1;
    PClass _class=GetClassByName(name);
    if(_class) order = _class->GetChildById(obj)+1;
    PUSH_DOUBLE(order);
}

void vm_cameraproc()
{
    double set=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PObject po=SCGetObject(POP_PTR);
    PUSH_DOUBLE(CameraProc3d(po,hsp3d,obj,set));
}

void vm_setobjectbase3d()
{
    POINT3D p;
    p.z=POP_DOUBLE;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(SetObjectBase3d(hsp3d,obj,&p));
}

void vm_getbase3d()
{
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MGetBase3d(Q,hsp3d,obj));
}

void vm_trcamera3d()
{
    double f3=POP_DOUBLE;
    double f2=POP_DOUBLE;
    double f1=POP_DOUBLE;
    int mode=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    HOBJ2D hcamera=Get3dViewCamera3d(hsp,obj);
    HSP3D hsp3d=Get3dSpace2d(hsp,obj);
    BOOL r=0;
    if (hsp3d && hcamera){
        CAMERA3D cam;
        if(GetCamera3d(hsp3d,hcamera,&cam)){
            if(TransformCamera3d(hsp,obj,cam,mode,f1,f2,f3)){
                r=SetCamera3d(hsp3d,hcamera,&cam);
            }
        }
    }
    PUSH_DOUBLE(r)
}

void vm_getmatrix3d()
{
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MGetMatrix3d(Q,hsp3d,obj));
}

void vm_setmatrix3d()
{
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MSetMatrix3d(Q,hsp3d,obj));
}

void vm_getbase3d_2()
{
    double*z=(double*)POP_PTR;
    double*y=(double*)POP_PTR;
    double*x=(double*)POP_PTR;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    POINT3D base;
    if(GetObjectBase3d(hsp3d,obj,&base))
    {
        *x=base.x;*y=base.y;*z=base.z;
        PUSH_DOUBLE(1);
    }
    else
    {
        PUSH_DOUBLE(0);
    }
}

void vm_sweep()
{
    UINT32 flags=POP_DOUBLE;
    COLORREF rgb_color=POP_LONG;
    INT16 asteps=POP_DOUBLE;
    double z=POP_DOUBLE;
    double y=POP_DOUBLE;
    double x=POP_DOUBLE;
    double angle=POP_DOUBLE;
    int Qr=POP_DOUBLE;
    int Qp=POP_DOUBLE;
    HOBJ2D old=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_HOBJ(matrixs->MSweep3d(hsp3d,old,Qp,Qr,angle,x,y,z,asteps,rgb_color,flags));
}

void vm_getdim3d()
{
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MGetDim3d(Q,hsp3d,obj));
};

void vm_getpoints3d(){
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MGetPoints3d(Q,hsp3d,obj));
};

void vm_setpoints3d(){
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MSetPoints3d(Q,hsp3d,obj));
};

void vm_transform3d(){
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MTransform3d(Q,hsp3d,obj));
};

void vm_rotate3d(){
    double angle=POP_DOUBLE;
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MRotate3d(Q,hsp3d,obj,angle));
};

void vm_transformpoints3d(){
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MTransformPoints3d(Q,hsp3d,obj));
};

void vm_rotatepoints3d(){
    double angle=POP_DOUBLE;
    int Q=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MRotatePoints3d(Q,hsp3d,obj,angle));
};

void vm_makeface3d(){
    int f=POP_DOUBLE;
    int color=POP_LONGi;
    int Qc=POP_DOUBLE;
    int Qp=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_HOBJ(matrixs->MMakeFace3d(hsp3d,obj,Qp,Qc,color,f));
};

void vm_getspace3d(){
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HSP(Get3dSpace2d(hsp,obj));
};
//----------
void vm_createdefcamera3d(){
    CAMERA3D cam;
    MakeDefaultCamera(&cam,POP_DOUBLE);
    HSP3D hsp=POP_HSP3D;
    PUSH_HOBJ(CreateCamera3d(hsp,&cam));
};
void vm_fittocamera3d(){
    double range=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HOBJ2D view3d=POP_HOBJ;
    HSP2D  hsp=POP_HSP;
    PUSH_DOUBLE(TrCameraToFeet(hsp,view3d,obj,range));
};
void vm_switchtocamera3d(){
    HOBJ2D camera=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(Set3dViewCamera3d(hsp,obj,camera));
}

void vm_getactivecamera3d(){
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(Get3dViewCamera3d(hsp,obj));
};
void vm_setcolor3d(){
    union{
        COLOR3D  c3d;
        COLORREF cr;
    };
    cr=POP_LONG;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp=POP_HSP3D;
    BOOL r=SetObjectColor3d(hsp,obj,&c3d);
    PUSH_DOUBLE(r);
};
void vm_getcolor3d(){
    union{
        COLOR3D  c3d;
        COLORREF cr;
    };
    cr=RGB(0,0,0);
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp=POP_HSP3D;
    GetObjectColor3d(hsp,obj,&c3d);
    PUSH_LONG(cr);
};

void vm_createspace3d(){
    HSP2D hsp=POP_HSP;
    PUSH_HSP((HSP2D)CreateSpace3d(hsp));
};

void vm_deletespace3d(){
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(DeleteSpace3d(hsp3d));
}

void vm_createprojection(){
    POINT2D org,size;
    size.y=POP_DOUBLE;
    size.x=POP_DOUBLE;
    org.y=POP_DOUBLE;
    org.x=POP_DOUBLE;
    HOBJ2D camera=POP_HOBJ;
    HSP3D hsp=POP_HSP3D;
    PUSH_HOBJ(Create3dProjection2d(hsp,camera,&org,&size));
};


// Cистемы координат

void pushcrdsystem3d(){
    double r=PushCrdSystem3d(POP_HSP3D);
    PUSH_DOUBLE(r);
};
void popcrdsystem3d(){
    double r=PopCrdSystem3d(POP_HSP3D);
    PUSH_DOUBLE(r);
}
void selectlocalcrd3d(){
    CRDSYSTEM crd;
    crd.type=LOCAL_COORDINATE;
    crd.hObject=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    if(crd.hObject==-1)crd.type=WORLD_COORDINATE;
    BOOL r=SetCrdSystem3d(hsp3d,&crd);
    PUSH_DOUBLE(r);
};
void selectworldcrd3d(){
    CRDSYSTEM crd;
    crd.type=WORLD_COORDINATE;
    crd.hObject=0;
    BOOL r=SetCrdSystem3d(POP_HSP3D,&crd);
    PUSH_DOUBLE(r);
};
void selectviewcrd3d(){

};

void vm_addobjectpoint3d(){
    POINT3D p;
    p.z=POP_DOUBLE;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(AddPoints3d(hsp3d,obj,&p));
};
void vm_setobjectpoint3d(){
    POINT3D p;
    p.z=POP_DOUBLE;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    INT16 pn=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(SetPoints3d(hsp3d,obj,&p,pn,1));
};
void vm_getobjectpoint3d(){
    POINT3D p;memset(&p,0,sizeof(p));
    double*z=(double*)POP_PTR;
    double*y=(double*)POP_PTR;
    double*x=(double*)POP_PTR;
    INT16 pn=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    if(GetPoints3d(hsp3d,obj,&p,pn,1)){pn=1;*x=p.x;*y=p.y;*y=p.y;}else pn=0;
    PUSH_DOUBLE(pn);
};

void vm_getnumpoints3d(){
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(GetNumPoints3d(hsp3d,obj));
};
void vm_delpoint3d(){
    INT16 n=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(DelPoints3d(hsp3d,obj,n,1));
};
void vm_addprimitive3d(){
    PRIMITIVE3D p;
    memset(&p,0,sizeof(p));
    p.count = *((INT16*)codepoint); codepoint+=1;
    if(p.count){p.points=new INT16[p.count];
        INT16 i = p.count;
        while (i--)
        { p.points[i] = POP_DOUBLE; }
    }
    p.color.rgb=POP_LONG;
    p.flags=POP_DOUBLE;
    HOBJ2D obj  = POP_HOBJ;
    HSP3D hsp3d =POP_HSP3D;
    PUSH_DOUBLE(CreatePrimitives3d(hsp3d,obj,&p,1));
    if(p.points)delete p.points;
}
void vm_setprimitive3d(){
    PRIMITIVE3D p;
    memset(&p,0,sizeof(p));
    p.count = *((INT16*)codepoint); codepoint+=1;
    if(p.count){p.points=new INT16[p.count];
        INT16 i = p.count;
        while (i--)
        { p.points[i] = POP_DOUBLE; }
    }
    p.color.rgb=POP_LONG;
    p.flags=POP_DOUBLE;
    INT16 prim=POP_DOUBLE;
    HOBJ2D obj  = POP_HOBJ;
    HSP3D hsp3d =POP_HSP3D;
    PUSH_DOUBLE(SetPrimitives3di(hsp3d,obj,&p,&prim,1));
    if(p.points)delete p.points;
}
void vm_delprimitive3d(){
    INT16 n=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(DelPrimitives3d(hsp3d,obj,&n,1));
};

void vm_getcolors3d()
{
    int count=POP_DOUBLE;
    int from=POP_DOUBLE;
    int i=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MGetColors3d(i,hsp3d,obj,from,count));
};
void vm_setcolors3d(){
    int count=POP_DOUBLE;
    int from=POP_DOUBLE;
    int i=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(matrixs->MSetColors3d(i,hsp3d,obj,from,count));
};
void vm_getnumprimitives3d(){
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    PUSH_DOUBLE(GetNumPrimitives3d(hsp3d,obj));
};
void vm_createobject3d(){
    HSP3D hsp3d=POP_HSP3D;
    PUSH_HOBJ(CreateObject3d(hsp3d));
};
void vm_duplicateobject3d(){
    HOBJ2D obj=POP_HOBJ;
    HSP3D hsp3d=POP_HSP3D;
    long size;
    HGLOBAL block=CopyToMemory3d(hsp3d,obj,&size);
    obj=PasteFromMemory3d(hsp3d,block,NULL);
    GlobalFree(block);
    PUSH_HOBJ(obj);
};
void vm_getcontroltextH(){
    int h=POP_LONGi;
    HOBJ2D obj =POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp,obj);
    TStream*st=streams->GetStream(h);
    int l=0;
    if(hwnd && st){
        l=GetWindowTextLength(hwnd);
        if(l){
            char * buf=new char[l+1];
            GetWindowText(hwnd,buf,l+1);
            st->Write(buf,l+1);
            delete buf;
        }}
    PUSH_DOUBLE(l);
};
void vm_setcontroltextH(){
    int h=POP_LONGi;
    HOBJ2D obj = POP_HOBJ;
    HSP2D hsp = POP_HSP;
    HWND hwnd = GetControl2d(hsp,obj);
    TStream*st=streams->GetStream(h);
    if (hwnd && st){
        int l=st->GetSize();
        if(l<65536){
            long p=st->GetPos();
            st->Seek(0);
            char *buf=new char [l+1];
            st->Read(buf,l);
            buf[l]=0;
            SetWindowText(hwnd,buf);
            delete buf;
            st->Seek(p);
            PUSH_DOUBLE(1);
        }}else {PUSH_DOUBLE(0);}
};


void vm_videogetmarker(){
    VIDEOMARKER vm;
    memset(&vm,0,sizeof(vm));

    double*_y=(double*)POP_PTR;
    double*_x=(double*)POP_PTR;
    vm.color_to  = POP_LONG;
    vm.color_from = POP_LONG;
    vm.color  = POP_LONG;
    vm.hv2d=POP_LONGi;
    BOOL rez=GetVideoMarker2d(&vm);
    *_y=vm.point.y;
    *_x=vm.point.x;
    PUSH_DOUBLE(rez);

};
void vm_writeframe(){
    HV2D hv=POP_LONGi;
    PUSH_DOUBLE(WriteVideoFrame2d(hv));
};
void vm_videocompressdialog(){
    HV2D hv=POP_LONGi;
    BOOL ret=VideoInfoDialog(NULL,hv);
    PUSH_DOUBLE(ret);
}
void vm_beginwritevideo(){
    char *flags=POP_PTR;
    int cy=POP_DOUBLE;
    int cx=POP_DOUBLE;
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;
    char *file=POP_PTR;
    HSP2D hsp=POP_HSP;
    PUSH_LONG(BeginWriteVideo2d(hsp,file,x,y,cx,cy,flags));
}
void vm_openvideo()
{
    UINT16 flags=POP_DOUBLE;
    char * Fn=POP_PTR;
    HV2D hv=OpenVideo(Fn,flags);
    PUSH_LONG(hv);
};

void vm_closevideo(){
    HV2D hv=(HV2D)POP_LONGi;
    BOOL r=CloseVideo(hv);
    PUSH_DOUBLE(r);
};
void vm_createvideoframe2d(){
    DWORD flags=POP_DOUBLE;
    gr_float sizey=POP_DOUBLE;
    gr_float sizex=POP_DOUBLE;
    gr_float y=POP_DOUBLE;
    gr_float x=POP_DOUBLE;
    HV2D  hv=(HV2D)POP_LONGi;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(CreateVideoFrame2d(hsp,hv,x,y,sizex,sizey,NULL,flags));
};
void vm_setvideosrc2d(){
    gr_float sizey=POP_DOUBLE;
    gr_float sizex=POP_DOUBLE;
    gr_float y=POP_DOUBLE;
    gr_float x=POP_DOUBLE;
    HOBJ2D  obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    FRAMEPROP fp;
    if(GetFrameProp2d(hsp,obj,&fp)){
        fp.src.left=x;
        fp.src.right=x+sizex;
        fp.src.top=y;
        fp.src.bottom=y+sizey;
        PUSH_DOUBLE(SetFrameProp2d(hsp,obj,&fp));
    }
    PUSH_DOUBLE(0);
}
void vm_videosetpos(){
    long pos=POP_DOUBLE;
    HV2D hv=POP_LONGi;
    BOOL r=VideoSetPos(hv,pos);
    PUSH_DOUBLE(r);
};
void vm_framesetpos2d(){
    long pos=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r=FrameSetPos2d(hsp,obj,pos);
    PUSH_DOUBLE(r);
};

void vm_videogetpos(){
    HV2D hv=POP_LONGi;
    VIDEOPROP vp;
    if(GetVideoProp(hv,&vp)){PUSH_DOUBLE(vp.pos);}else {PUSH_DOUBLE(0);}
};

void vm_framegetpos(){
    HOBJ2D hframe=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    FRAMEPROP fp;
    if(GetFrameProp2d(hsp,hframe,&fp)){PUSH_DOUBLE(fp.pos);}else {PUSH_DOUBLE(0);}
};
void vm_framegethvideo(){
    HOBJ2D hframe=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    FRAMEPROP fp;
    if(GetFrameProp2d(hsp,hframe,&fp)){PUSH_LONG(fp.video);}else {PUSH_LONG(0);}
};


void vm_videoplay2d(){
    long flags=POP_DOUBLE;
    long rate=POP_DOUBLE;
    long end=POP_DOUBLE;
    long start=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL  r=VideoPlay2d(hsp,obj,start,end,rate,flags);
    PUSH_DOUBLE(r);
};

void vm_videopause2d(){
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r=VideoPause2d(hsp,obj);
    PUSH_DOUBLE(r);
};
void vm_videoresume2d(){
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r=VideoResume2d(hsp,obj);
    PUSH_DOUBLE(r);
};
void vm_videostop2d(){
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r=VideoStop2d(hsp,obj);
    PUSH_DOUBLE(r);
};

void Commands3d(){

    commands[VM_SETOBJECTBASE3D ]=  vm_setobjectbase3d;
    commands[VM_GETSPACE3D     ]=  vm_getspace3d;
    commands[VM_GETDIM3D   ]= vm_getdim3d;
    commands[VM_GETOBJECTBASE3D]= vm_getbase3d;
    commands[VM_GETBASE3D_2]=vm_getbase3d_2;
    commands[VM_SETMATRIX3D]=vm_setmatrix3d;
    commands[VM_GETMATRIX3D]=vm_getmatrix3d;
    commands[VM_CAMERAPROC]=vm_cameraproc;
    commands[VM_TRANSFORM3D]=vm_transform3d;
    commands[VM_ROTATE3D]=vm_rotate3d;
    commands[VM_TRANSFORMPOINTS3D]=vm_transformpoints3d;
    commands[VM_ROTATEPOINTS3D]=vm_rotatepoints3d;


    commands[VM_CREATEDEFCAMERA3D]=vm_createdefcamera3d;
    commands[VM_SWITCHTOCAMERA3D ]=vm_switchtocamera3d;
    commands[VM_GETACTIVECAMERA3D]=vm_getactivecamera3d;
    commands[VM_CREATESPACE3D    ]=vm_createspace3d;
    commands[VM_DELETESPACE3D    ]=vm_deletespace3d;
    commands[VM_CREATEPROJECTION ]=vm_createprojection;

    commands[VM_GETPOINTS3D ]=vm_getpoints3d;
    commands[VM_SETPOINTS3D ]=vm_setpoints3d;
    commands[VM_SETCOLOR3D]=vm_setcolor3d;
    commands[VM_GETCOLOR3D]=vm_getcolor3d;

    commands[VM_MAKEFACE3D]= vm_makeface3d;
    commands[VM_FITTOCAMERA3D]= vm_fittocamera3d;

    commands[PUSHCRDSYSTEM3D ]=pushcrdsystem3d;
    commands[POPCRDSYSTEM3D  ]=popcrdsystem3d;
    commands[SELECTLOCALCRD3D]=selectlocalcrd3d;
    commands[SELECTWORLDCRD3D]=selectworldcrd3d;
    commands[SELECTVIEWCRD3D ]=selectviewcrd3d;

    commands[VM_CREATEOBJECT3D    ]=vm_createobject3d;
    commands[VM_DELPRIMITIVE3D    ]=vm_delprimitive3d;
    commands[VM_GETNUMPRIMITIVES3D]=vm_getnumprimitives3d;
    commands[VM_ADDPRIMITIVE3D    ]=vm_addprimitive3d;
    commands[VM_DELPOINT3D        ]=vm_delpoint3d;
    commands[VM_GETNUMPOINTS3D    ]=vm_getnumpoints3d;
    commands[VM_ADDOBJECTPOINT3D  ]=vm_addobjectpoint3d;
    commands[VM_SWEEPEXTRUDE]=vm_sweep;
    commands[VM_SETOBJECTPOINT3D ]=vm_setobjectpoint3d;
    commands[VM_GETOBJECTPOINT3D ]=vm_getobjectpoint3d;
    commands[VM_SETPRIMITIVE3D   ]=vm_setprimitive3d;
    commands[VM_TRCAMERA3D]=vm_trcamera3d;

    commands[VM_OPENVIDEO]=         vm_openvideo;
    commands[VM_CLOSEVIDEO]=        vm_closevideo;
    commands[VM_CREATEVIDEOFRAME2D]=vm_createvideoframe2d;
    commands[VM_VIDEOSETPOS]=       vm_videosetpos;
    commands[VM_FRAMESETPOS2D]=     vm_framesetpos2d;
    commands[VM_VIDEOPLAY2D]=       vm_videoplay2d;
    commands[VM_VIDEOPAUSE2D]=      vm_videopause2d;
    commands[VM_VIDEORESUME2D]=     vm_videoresume2d;
    commands[VM_VIDEOSTOP2D]=       vm_videostop2d;
    commands[VM_SETVIDEOSRC2D]=     vm_setvideosrc2d;
    commands[VM_BEGINWRITEVIDEO   ]=vm_beginwritevideo;
    commands[VM_VIDEOCOMPRESSDIALOG]=vm_videocompressdialog;
    commands[VM_WRITEFRAME        ]=vm_writeframe;
    commands[VM_VIDEOGETPOS]       =vm_videogetpos;
    commands[VM_FRAMEGETPOS]       =vm_framegetpos;
    commands[VM_FRAMEGETHVIDEO]    =vm_framegethvideo;

    commands[VM_SETCALCORDER]=  vm_setcalcorder;
    commands[VM_GETCALCORDER]=  vm_getcalcorder;
    commands[V_CREATEFROMFILE3D]=v_createfromfile3d;

    commands[VM_GETRVALUE]=vm_getrvalue;
    commands[VM_GETGVALUE]=vm_getgvalue;
    commands[VM_GETBVALUE]=vm_getbvalue;
    commands[VM_VIDEOGETMARKER]=vm_videogetmarker;

    commands[VM_GETVARINFO ]=vm_getvarinfo;
    commands[VM_GETVARCOUNT]=vm_getvarcount;
    commands[VM_SETCOLORS3D]=vm_setcolors3d;
    commands[VM_GETCOLORS3D]=vm_getcolors3d;
    commands[V_STRINGEX]=vm_stringex;
    commands[VM_CREATEMATERIAL]=vm_creatematerial;
    commands[VM_APPLYTEXTUTRE]= vm_applytextutre;
    commands[VM_REMOVETEXTURE]= vm_removetexture;
    commands[V_FOLDERDIALOG]=vm_folderdialog;
    commands[VM_GETMATERIALs]=vm_getmaterials;
    commands[VM_LOADMENU  ]=vm_loadmenu;
    commands[VM_DELETEMENU]=vm_deletemenu;
    commands[VM_GETMOUSEPOS]=vm_getmousepos;
    commands[VM_GETNEXTTOOL2D]=vm_getnexttool2d;
    commands[VM_GETNEXTOBJECT]=vm_getnextobject;
    commands[VM_GETOBJECT3DFROMPOINT2D]=vm_getobject3dfrompoint2d;
    commands[VM_SETHYPERJUMP]=vm_sethyperjump;
    commands[SETRDIB2D  ]=setrdib2d;
    commands[SSETRDDIB2D]=setrddib2d;
    commands[GETRDIB2D  ]=getrdib2d;
    commands[GETRDDIB2D ]=getrddib2d;
    commands[VM_GETCONTROLTEXTH]=vm_getcontroltextH;
    commands[VM_SETCONTROLTEXTH]=vm_setcontroltextH;
    commands[VM_CHECKMENU]=vm_checkmenu;
    commands[VM_ENABLEMENU]= vm_enablemenu ;
    commands[GETPIXEL2D]=dbgetpixel;
    commands[VM_SETPIXEL2D]=dbsetpixel;

    commands[VM_DUPLICATEOBJECT3D]=vm_duplicateobject3d;

};


