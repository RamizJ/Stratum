/*
Copyright (c) 1995,1996  Virtual World
Module Name:
            _system.cpp
Author:
            Alexander Shelemekhov
*/
// функции для экспорта добавить в d:\bc\include\gwin2d3d\GWINFUNC.H

#include <windows.h>
//#include "_system.h"
#include "cairorenderer.h"
#include <math.h>
#define SetModify ps->modifyed=TRUE;
#include <mem.h>
#ifdef WIN32
#define  hmemcpy(p1,p2,p3) memcpy(p1,p2,p3)
#endif

#include "../../BC/GWIN2D3D/FILETYPE.H"
#include "../../BC/GWIN2D3D/GWINTYPE.H"

#include <dir.h>
#include <dos.h>
#include "camera3d.h"

BYTE showtime3d;
int GetPrivateInt(char *group,char* item,BOOL def=0){
    char s[80];
    GetPrivateProfileString(group,item,def?"ON":"OFF",s, sizeof(s),_graphics_ini);
    OemToAnsi(s,s);
    AnsiUpper(s);
    if ((lstrcmp(s,"OFF")==0)||(lstrcmp(s,"0")==0))return 0;
    return 1;
};

HPALETTE palette_256=0;
static HBITMAP axis_bmp=0;
static HBITMAP gray_bmp=0;
static HBRUSH gray_brush=0;
static HBITMAP cross_bmp=0;
static HBRUSH cross_brush=0;
HBITMAP noimage_bmp=0;
HBITMAP GetAxisBMP(){
    if(!axis_bmp){
        axis_bmp=LoadBitmap(hInstance,MAKEINTRESOURCE(AXIS_NAMES));
    }
    return axis_bmp;
};

HBRUSH GetGrayBrush(){
    if(!gray_bmp){
        gray_bmp=LoadBitmap(hInstance,"GRAY_BITMAP");
        gray_brush=CreatePatternBrush(gray_bmp);
    }
    return gray_brush;
}
HBRUSH GetCrossBrush(){
    if(!cross_bmp){
        cross_bmp=LoadBitmap(hInstance,"CROSS_BITMAP");
        cross_brush=CreatePatternBrush(cross_bmp);
    }
    return cross_brush;
}                                
UINT CFormat_2D,CFormat_3D;
PCollection spaces = NULL;

static PCollection usertypes=NULL;

USERTYPESTRUCT * GetUserType(char * type)
{
    if (!usertypes)
    {
        usertypes = new TCollection(10,10);return 0;
    }
    for(C_TYPE i=0; i < usertypes->count; i++)
    {
        USERTYPESTRUCT * ut=(USERTYPESTRUCT*)usertypes->At(i);
        if (!lstrcmp(type,ut->name))
            return ut;
    }
    return NULL;
};

_EXTERNC BOOL _EXPORT RegisterUserType2d(USERTYPESTRUCT *_t)
{
    USERTYPESTRUCT * us=GetUserType(_t->name);
    if(us)
    {
        if (_t->objproc==0)
        {
            _t->objproc=us->objproc;
            _t->flags=us->flags;
        }
        else
        {
            us->objproc = _t->objproc;
            us->flags   = _t->flags;
        }
        return -1;
    }
    if (lstrlen(_t->name)<2 || _t->objproc==NULL)
        return 0;

    USERTYPESTRUCT *t=new USERTYPESTRUCT;
    *t=*_t;
    t->name = NewStr(t->name);

    return usertypes->Insert(t)+1;
};

_EXTERNC HOBJ2D _EXPORT CreateUserObject2d(HSP2D hsp, USEROBJSTRUCT* us)
{
    PSpace2d ps=GetSpace(hsp);
    if (ps)
    {
        USERTYPESTRUCT * t=GetUserType(us->type);
        if (t)
        {
            ps->modifyed=TRUE;
            return ps->CreateUserObject2d(us,t);
        }
    }
    return 0;
};

_EXTERNC gr_float _EXPORT GetAngleByXY2d(gr_float x,gr_float y)
{
    return GetAngleByXY(x,y);
}

_EXTERNC HOBJ2D _EXPORT CreateControlObject2d(HSP2D hsp,
                                              LPCSTR lpszClassName,	/* address of registered class name	*/
                                              LPCSTR lpszWindowName,	/* address of window text	*/
                                              DWORD dwStyle,	/* window style	*/
                                              int x,	/* horizontal position of window	*/
                                              int y,	/* vertical position of window	*/
                                              int nWidth,	/* window width	*/
                                              int nHeight,	/* window height	*/
                                              INT16 id,
                                              DWORD ExdwStyle){
    PSpace2d ps=GetSpace(hsp);
    if (ps){
        SetModify
                return ps->CreateControl2d(lpszClassName,
                                           lpszWindowName,
                                           dwStyle, x, y, nWidth,nHeight, id, ExdwStyle);
    }
    return 0;
};


_EXTERNC DWORD _EXPORT SetControlStyle2d(HSP2D hSpace2d,HOBJ2D Object2d,DWORD style,WORD flags)
{
    TControl2d *pc =(TControl2d *)_GetObject2d(hSpace2d,Object2d,26);
    if (pc && pc->HWindow)
    {
        DWORD old=pc->dwStyle;
        if (flags&1)
        {
            if (pc->dwStyle != style)
            {
                pc->dwStyle=style;
                pc->space->modifyed=1;
                char s[64];
                GetClassName(pc->HWindow,s,sizeof(s));
                if(lstrcmpi(s,"BUTTON"))
                {
                    SendMessage(pc->HWindow,BM_SETSTYLE,(WPARAM) LOWORD(style),1);
                }
                SetWindowLong(pc->HWindow,GWL_STYLE,style);
            }
        }
        return old;
    }
    return 0;
};
_EXTERNC HWND _EXPORT GetControl2d(HSP2D hSpace2d,HOBJ2D Object2d){
    TControl2d *pc =(TControl2d *)_GetObject2d(hSpace2d,Object2d,26);
    if (pc)return (pc->HWindow);
    return 0;
}
_EXTERNC USEROBJSTRUCT * _EXPORT GetUserData2d(HSP2D hSpace2d,HOBJ2D Object2d){
    TUserObject2d *pu =(TUserObject2d *)_GetObject2d(hSpace2d,Object2d,25);
    if (pu)return &(pu->data);
    return 0;
}

_EXTERNC BOOL  _EXPORT GetInternal2d(INTERNAL2D*in){
    PSpace2d ps=GetSpace(in->hSpace2d);
    if(!ps)return FALSE;
    PTools2d ptool=NULL;
    if(in->tooltype){
        PHANDLECollection pt=ps->GetToolsById(in->tooltype);
        if (pt)ptool= (PTools2d)pt->GetByHandle(in->hobject);
        if(!ptool)return 0;
    }
    switch(in->what){
        case 10:if(in->tooltype==DIB2D || in->tooltype==DOUBLEDIB2D){
                PDib2d pd=(PDib2d)ptool->GetMainObject();
                char *text=(char*)(in->data1);
                if(pd){
                    lstrcpyn(text,pd->filename,in->data2);
                }else *text=0;
                return TRUE;
            }break;
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT SetTexturePath2d(HSP2D hSpace2d,char*s){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        if(ps->txtPath)delete ps->txtPath;
        ps->txtPath=NewStr(s);
        return TRUE;
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT  GetTexturePath2d(HSP2D hSpace2d,char*s){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps && s){
        if(ps->txtPath)lstrcpy(s,ps->txtPath);else {*s=0;return FALSE;}
        return TRUE;
    }
    *s=0;
    return FALSE;
};
_EXTERNC BOOL _EXPORT UpdateObject2d(HSP2D hsp,HOBJ2D Obj,RECT2D far*r){
    PSpace2d ps=GetSpace(hsp);
    if (ps){
        PObject2d po=(PObject2d)ps->GetObjByHANDLE(Obj);
        if (po){
            if (r){
                RECT2D _r=*r;
            }else{
                ps->AddObjectRect(po);
            }
            return TRUE;
        } }
    return FALSE;
};
inline _SPACE* __GetSpace(HSPACE h)
{
    if (h && (((UINT16)h)<=spaces->count))
        return (_SPACE*)spaces->At(((INT16)h)-(INT16)1);
    return NULL;
}

PSpace2d GetSpace(HSP2D h){
    PSpace2d ps=(PSpace2d)__GetSpace((HSPACE)h);
    if (ps && ps->WhoIsIt()==17458)return ps;
    return NULL;
}; // Возвращает указатель на Space2d по HANDLE
PSpace3d Get3dSpace(HSP3D h,PSpace2d & ps)
{
    PSpace3d _ps=(PSpace3d)__GetSpace((HSPACE)h);
    if (_ps && _ps->WhoIsIt()==17459){ps=_ps->space2d;return _ps; }
    return NULL;
};

HSPACE __InsertSpace(_SPACE *sp)
{
    UINT16 i;
    if(sp->WhoIsIt()==17458) //2d space
    {
        ((TSpace2d*)sp)->palette = palette_256;
    }

    for(i = 0; i < spaces->count; i++)
    {
        if (spaces->At(i)==NULL)
        {
            spaces->AtPut(i,sp);
            return (HSPACE)(i+1);
        }
    }
    if(spaces->count < 16384)
    {
        return (HSPACE)(spaces->Insert(sp)+1);
    }
    return 0;
}

PObject _GetObject(HSP2D h,HOBJ2D o,_SPACE** _ps)
{
    _SPACE *ps=__GetSpace((HSPACE)h);
    if (ps)
    {
        if (_ps)
            *_ps = ps;
        return ps->GetObjByHANDLE(o);
    }
    return NULL;
}

HSP2D InsertSpace(PSpace2d ps2d)
{
    HSP2D hsp = (HSP2D) __InsertSpace(ps2d);
    if (hsp)
    {
        ps2d->SetHANDLE(hsp);
        return hsp;
    }
    else
    {
        delete ps2d;
        _Error(EM_tomorespaces);
        return 0;
    }
} // Вставляет Space2d в коллекцию spaces и возвращает дескриптор

HSP3D InsertSpace3d(PSpace3d ps3d)
{
    HSP3D hsp=(HSP3D)__InsertSpace(ps3d);
    if (hsp){
        ps3d->_HSP3D=hsp;

        return hsp;
    }else {delete ps3d;_Error(EM_tomorespaces);return 0;}
} // Вставляет Space2d в коллекцию spaces и возвращает дескриптор


PObject2d _GetObject2d(HSP2D h,HOBJ2D o,int t)
{ PSpace2d ps=GetSpace(h);
    if (ps==NULL)return 0;
    PObject2d po=(PObject2d)ps->GetObjByHANDLE(o);
    if ((t)&&(po)&&(po->WhoIsIt()!=t))return NULL;
    return po;
}

//*------------------------------------------------------*
PToolsCollection GetTools(HSP2D h,TOOL_TYPE t)
{
    PSpace2d ps=(PSpace2d)__GetSpace((HSPACE)h);
    if (ps)
    {
        if (ps->WhoIsIt()==17458)
            return ps->GetToolsById(t);
        if (ps->WhoIsIt()==17459)
        {
            if(t==TEXTURE3D)
            {
                return ((PSpace3d)ps)->materials;
            }
        }
    }
    return NULL;
};

// Возвращает указатель на коллекцию по номеру t
// из пространства h
//*-------------------------------------------------------*
PTools2d GetTool(HSP2D h,TOOL_TYPE t,HOBJ2D i)
{
    PHANDLECollection pt=GetTools(h,t);
    if(pt) return (PTools2d)pt->GetByHandle(i);
    return NULL;
};
/* Возвращает указатель на элемент с дескриптором i в
          коллекции по номеру t из пространства h
*/
int __AddColor(char* ls,COLORREF cl)
{int r=GetRValue(cl),
            g=GetGValue(cl),
            b=GetBValue(cl);
    return wsprintf(ls,"(%d,%d,%d)",r,g,b);
}

void __AddROP2(char *s,int rop )
{
    switch(rop){
        case R2_BLACK       :lstrcat(s,"BLACK ");break;
        case R2_NOTMERGEPEN :lstrcat(s,"NOTMERGEPEN ");break;
        case R2_MASKNOTPEN  :lstrcat(s,"MASKNOTPEN ");break;
        case R2_NOTCOPYPEN  :lstrcat(s,"NOTCOPYPEN ");break;
        case R2_MASKPENNOT  :lstrcat(s,"MASKPENNOT ");break;
        case R2_NOT         :lstrcat(s,"NOT ");break;
        case R2_XORPEN      :lstrcat(s,"XORPEN ");break;
        case R2_NOTMASKPEN  :lstrcat(s,"NOTMASKPEN ");break;
        case R2_MASKPEN     :lstrcat(s,"MASKPEN ");break;
        case R2_NOTXORPEN   :lstrcat(s,"NOTXORPEN ");break;
        case R2_NOP         :lstrcat(s,"NOP ");break;
        case R2_MERGENOTPEN :lstrcat(s,"MERGENOTPEN ");break;
        case R2_COPYPEN     :/*lstrcat(s,"COPYPEN");*/break;
        case R2_MERGEPENNOT :lstrcat(s,"MERGEPENNOT ");break;
        case R2_MERGEPEN    :lstrcat(s,"MERGEPEN ");break;
        case R2_WHITE       :lstrcat(s,"WHITE ");break;
    }
}
/*
int _EXPORT GetToolText2d( HSP2D hSpace2d,int what,int htool,LPSTR s,int n,int f)
 {PToolsCollection ptc=GetTools(hSpace2d,what);
  if (ptc){
  int i=ptc->IndexOf(MK_FP(0,htool));
  PTools2d pt;
  if (i>-1)pt=(PTools2d)ptc->At(i);else return -1;
  if (pt){
  if (s && n>1){
  if (f&1)
    wvsprintf(s,"#%d ",&htool);else lstrcpy(s,"");
    char * ls;
  switch (what){
    case 1:{//pen
      PPen2d pp=(PPen2d)pt;

      switch (pp->style){
        case PS_SOLID:lstrcat(s,"SOLID");break;
        case PS_NULL:lstrcat(s,"NULL");break;
        case PS_DASH:lstrcat(s,"DASH");break;
        case PS_DASHDOT:lstrcat(s,"DASHDOT");break;
        case PS_DASHDOTDOT:lstrcat(s,"DASHDOTDOT");break;
        case PS_DOT:lstrcat(s,"DOT");break;
        }
        ls=s+lstrlen(s);
        ls+=wvsprintf(ls," [%d]",&(pp->width));
        __AddColor(ls,pp->color);
        __AddROP2(ls,pp->rop2);
      break;
      }
    case 2:{
        PBrush2d pb=(PBrush2d)pt;
      if (pb->dib)
        {lstrcat(s,"DIBPATTERN");ls=s+lstrlen(s);
         ls+=wvsprintf(ls," [%d] ",&(pb->dib->HANDLE));
         PDib2d pd=(PDib2d)pb->GetMainObject();
         if (pd->filename)
         lstrcat(ls,pd->filename);
        }else{
      switch (pb->style){
        case BS_SOLID   :lstrcat(s,"SOLID");break;
        case BS_NULL    :lstrcat(s,"NULL");break;
        case BS_HATCHED :lstrcat(s,"HATCHED ");
             switch (pb->hatch){
                case HS_HORIZONTAL:lstrcat(s,"HORIZONTAL");break;
                case HS_VERTICAL  :lstrcat(s,"VERTICAL");break;
                case HS_FDIAGONAL :lstrcat(s,"FDIAGONAL");break;
                case HS_BDIAGONAL :lstrcat(s,"BDIAGONAL");break;
                case HS_CROSS     :lstrcat(s,"CROSS");break;
                case HS_DIAGCROSS :lstrcat(s,"DIAGCROSS");break;
              }
             break;
        case BS_PATTERN :lstrcat(s,"PATTERN");break;
        case BS_INDEXED :lstrcat(s,"INDEXED");break;
        }
      if (pb->style!=BS_NULL)
        {ls=s+lstrlen(s);
         __AddColor(ls,pb->color);
        }
      }__AddROP2(ls,pb->rop2);
        break;
      };
    case 3:
    case 4:
    {PDib2d pd=(PDib2d)pt->GetMainObject();
     ls=s+lstrlen(s);
     wsprintf(ls,"[%d,%d] %lu %s",pd->W,pd->H,pd->NumClrs,pd->filename);
     break;}
    case 5:{PFont2d pf=(PFont2d)pt;
              ls=s+lstrlen(s);
              wsprintf(ls,"%s [%d,%d]",pf->font.lfFaceName,
                pf->font.lfWidth,pf->font.lfHeight);
            break;};
    case 6:{PString2d ps=(PString2d)pt;
              //lstrcat(s,ps->string);
                char * p=ps->string;
                ls=s+lstrlen(s);
                if (p){
                         while ((int)(s-ls)<(n-1) && p[0]!='\0'&&
                                            p[0]!='\r'&& p[0]!='\n')
                                  {ls[0]=p[0];p++;ls++;}
                        }	ls[0]='\0';
             break;}
    case 7:{ PText2d _p = (PText2d)pt;
                ls=s+lstrlen(s);
                wsprintf(ls,"%d",_p->text->count);
              break;}
    case 8:{ PSpace3d ps3=(PSpace3d)pt;
                ls=s+lstrlen(s);
                wsprintf(ls,"Num objects: %d",ps3->primary->count);
                break;
             }
    }
  if (f & 2){
  char ss[10];int h=pt->GetRefCount();
  wvsprintf(ss," %d",&h);
  int sl=lstrlen(s);
  if (sl<(n-lstrlen(ss))) lstrcat(s,ss);
  }}
  return i;
  }}return -1;
 }
*/

_EXTERNC INT16 _EXPORT GetToolNumByHANDLE(HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D tool)
{PToolsCollection pt=GetTools(hSpace2d,what);
    if (pt){
        PTools2d _pt= (PTools2d)pt->GetByHandle(tool);
        return pt->IndexOf(_pt);
    }
    return -1;
};

_EXTERNC BOOL   _EXPORT UpdateTool2d(HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D h,void*data){
    _SPACE* sp=__GetSpace((HSPACE)hSpace2d);
    if(sp){
        return sp->UpdateTool(what,h,data);
    }return 0;
};
_EXTERNC HOBJ2D _EXPORT GetToolHANDLEbyNum(HSP2D hSpace2d,TOOL_TYPE what,INT16 num)
{PToolsCollection pt=GetTools(hSpace2d,what);
    if (pt && pt->IsIndexValid(num))return ((PTools2d)(pt->At(num)))->_HANDLE;
    return 0;
};
_EXTERNC INT16 _EXPORT GetToolRef2d(HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D htool)
{PTools2d pt=GetTool(hSpace2d,what,htool);
    if (pt){ return pt->GetRefCount();
    }return 0;
}


_EXTERNC HGLOBAL _EXPORT CopyToMemory2d(HSP2D hSpace2d,HOBJ2D Object2d,INT32* size)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        if (Object2d==0)
            Object2d = ps->all->current;

        HGLOBAL hg=0;
        int h;
        {
            TMemoryStream mstream(FALSE);
            h = ps->_StoreObject(&mstream,Object2d);
            if (h && !mstream.status)
            {
                mstream.Truncate();
                hg = mstream.GetHANDLE();
                if (size)
                    *size = mstream.GetSize();
            }
        }
        return hg;
    }
    return 0;
}


_EXTERNC BOOL _EXPORT CopyToClipboard2d(HSP2D hSpace2d,HOBJ2D Object2d)
{
    PSpace2d ps=GetSpace(hSpace2d);
    int result=FALSE;
    if(ps && OpenClipboard(ps->hwnd))
    {
        EmptyClipboard();
        HGLOBAL hg = CopyToMemory2d(hSpace2d,Object2d,NULL);
        if ( ((int)hg) > 32)
        {
            if (SetClipboardData(CFormat_2D, hg))
                result = TRUE;
            else
                GlobalFree(hg);
        }
        CloseClipboard();
    }
    return result;
}

_EXTERNC HOBJ2D _EXPORT PasteFromMemory2d(HSP2D hSpace2d,HGLOBAL hg,POINT2D *org ,WORD flags)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        HOBJ2D h=0;
        TMemoryStream mstream(hg);
        POINT2D _org;
        if(org)
        {
            _org=*org;
            ps->CRDtoLP(&_org);
            org=&_org;
        }
        POINT2D __org;
        __org.x=__org.y=0;
        if(!org)org=&__org;
        if (!mstream.status)
            h = ps->MergeWithSpace(&mstream,*org,flags,ps->record);
        if (h)SetModify
                return h;
    }
    return 0;
}

_EXTERNC HOBJ2D _EXPORT PasteFromClipboard2d(HSP2D  hSpace2d,POINT2D *org ,WORD flags)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps && IsClipboardFormatAvailable(CFormat_2D) && OpenClipboard(ps->hwnd))
    {
        HOBJ2D h=PasteFromMemory2d(hSpace2d,
                                   GetClipboardData(CFormat_2D),org ,flags);
        CloseClipboard();
        return h;
    }
    return 0;
}

//-------------------------------------------------
_EXTERNC HSP2D _EXPORT CreateFromMetaFile2d(HWND hwnd,LPSTR FileName,LPSTR path)
{ METASIGNATURE ms;
    TDOSStream * ds=new TDOSStream(FileName,TDOSStream::stOpen);
    if (ds->status==0)
    {ds->Read(&ms,sizeof(ms));
        if (ds->status)goto fail;
        if (ms.SIGNATURE!=0x4154454dL)goto fail;
        if (ms.minversion>currentfileversion)goto fail;
        ds->Seek(ms.image);
        TSpace2d::currentpath=path;
        PSpace2d ps=(PSpace2d)ds->Get(17458);
        if ((ps==NULL)||(ps->all==NULL))goto fail;
        ds->Read(&ps->mrecord,sizeof(META_RECORD));
        ps->hwnd=hwnd;
        ps->record=1;
        ps->meta=ds;
        ps->Invalidate();
        // Разобраться с точками расхождения
        return InsertSpace(ps);
    }
fail:
    delete ds;
    return 0;
};
_EXTERNC HSP2D _EXPORT CreateMetaFile2d(HSP2D hSpace2d,LPSTR FileName)
{PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->record==0))
    {METASIGNATURE ms;
        ms.SIGNATURE=0x4154454dL;
        ms.fileversion=currentfileversion;
        ms.minversion=currentfileversion;
        ms.dir=NULL;
        ms.image=sizeof(ms);
        TDOSStream * ds=new TDOSStream(FileName,TDOSStream::stCreate);
        if (ds->status==0)
        {ds->Write(&ms,sizeof(ms));
            ds->Put(ps);
            ps->record=1;
            ps->meta=ds;
            ps->WriteLastRecord(0l);
        }
        return hSpace2d;
    }
    return 0;
};
_EXTERNC BOOL _EXPORT CloseMetaFile2d(HSP2D hSpace2d)
{PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->record))
    {return ps->CloseMetaFile();
    }
    return 0;
};
_EXTERNC HSP2D _EXPORT GetMetaFile2d(HSP2D hSpace2d)
{PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->record))return hSpace2d;else return 0;
};
_EXTERNC BOOL _EXPORT SaveMetaFile2d(HSP2D hmf2d,LPSTR FileName)
{PSpace2d ps=GetSpace(hmf2d);
    if ((ps)&&(ps->record))
    {long s=ps->meta->GetSize();
        TDOSStream ds(FileName,TDOSStream::stCreate);
        if (ds.status==0){
            if (ps->meta->CopyTo(&ds,0,s)==s)
            {ps->WriteMetaPoints(&ds); return TRUE;}
        }}
    return 0;
};
_EXTERNC INT32  _EXPORT PlayMetaRecord(HSP2D hSpace2d,INT32 direction)
{PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->record))
    {INT32 i,h=0;
        if (direction>0)
        {for (i=0;i<direction;i++)
                if (ps->DoRedo())h++;else return h;
        }else
        {for (i=0;i>direction;i--)
                if (ps->DoUndo())h++;else return h;
        }
        return h;
    }
    return 0;
};
_EXTERNC int _EXPORT NewMetaGroup2d(HSP2D hSpace2d){
    PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->record)){
        ps->record_flag|=1;
        return 1;
    }
    return 0;
};
_EXTERNC INT32 _EXPORT PlayGroupMetaRecord2d(HSP2D hSpace2d,INT32 direction){
    PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->record))
    {int i,h=0;
        if (direction>0)
        {for (i=0;i<direction;i++)
                if (ps->DoRedo()&&(ps->mrecord.RedoCode>0))h++;else return h;
        }else
        {for (i=0;i>direction;i--)
                if (ps->DoUndo()&&(ps->mrecord.RedoCode>0))h++;else return h;
        }
        return h;
    }
    return 0;
};
_EXTERNC UINT16 _EXPORT GetNextPrevMetaRecord2d(HSP2D hSpace2d,INT16 direction){
    PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->record)){
        if (direction>0)return ps->mrecord.RedoCode;else{
            if (ps->mrecord.PrevRecord){
                META_RECORD mr;long pos=ps->meta->GetPos();
                ps->meta->Seek(ps->mrecord.PrevRecord);
                ps->meta->Read(&mr,sizeof(mr));
                ps->meta->Seek(pos);
                return mr.RedoCode;
            } } }return 0;
};

_EXTERNC BOOL _EXPORT IsSpaceChanged2d(HSP2D hSpace2d)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->modifyed;return 0;
};
_EXTERNC BOOL _EXPORT IsSpaceRectChanged2d(HSP2D hSpace2d){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        BOOL r=ps->rectchange;
        ps->rectchange=0;
        return r;
    }
    return 0;
};
_EXTERNC BOOL _EXPORT SetSpaceModify2d(HSP2D hSpace2d,BOOL modify)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){ps->modifyed=modify;return 1;}
    return 0;
};

_EXTERNC HOBJ2D _EXPORT SetCurrentTool2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D index )
{
    PHANDLECollection pt;
    if (what == 8)
    {
        PSpace2d ps=GetSpace(hSpace2d);
        if (ps)
            pt = ps->spaces3d;
        else
            pt = 0;
        index &= 0xff;
    }
    else
        pt=GetTools(hSpace2d,what);

    if (pt)
    {
        HOBJ2D last = pt->current;
        if((index==OID_NONE)||(pt->GetByHandle(index)))
        {
            pt->current = index;
            return last;
        }
        else return 0;
    }
    return 0;
}
/*
 Устанавливает текущий элемент и возвращает дескриптор предыдущего
 текущего (если его небыло то -1 ) ,если неудачно то 0
*/
_EXTERNC INT16 _EXPORT GetToolCount2d( HSP2D hSpace2d,TOOL_TYPE what )
{PHANDLECollection pt;
    if (what==8){PSpace2d ps=GetSpace(hSpace2d);
        if (ps)pt=ps->spaces3d;else pt=0;
    }else  pt=GetTools(hSpace2d,what);
    if (pt){return pt->count;
    }
    return 0;
};

_EXTERNC HOBJ2D _EXPORT GetCurrentTool2d( HSP2D hSpace2d,TOOL_TYPE what )
{if (what==8){
        PSpace2d ps=GetSpace(hSpace2d);
        if (ps){if ( ps->spaces3d->current>0)
                return 0;  /// ???
        }
    }else{
        PHANDLECollection pt=GetTools(hSpace2d,what);
        if (pt) return pt->current;} return 0;
};
/*
 возвращает дескриптор текущего (если его небыло то -1 )
 ,если неудачно то 0
*/
_EXTERNC UINT16 _EXPORT GetToolType2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D index ){
    PTools2d pp= GetTool(hSpace2d,what,index);
    if (pp)return pp->WhoIsIt();else return 0;
};

_EXTERNC BOOL _EXPORT  SetPenStyle2d( HSP2D hSpace2d, HOBJ2D index,INT16 Style )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        long s=ps->SetPenStyle(index,Style);
        if(s)
        {if (ps->record){
                ps->NewMetaRecord2(mfsetpenstyle,index,Style);

                ps->NewUndoRecord2(mfsetpenstyle,index,LOWORD(s));
                ps->CloseMetaRecord();}
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};
//*-------------------------------------------------------*
_EXTERNC HPEN _EXPORT CreateGDIPen2d(HSP2D hSpace2d,HOBJ2D hpen)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        PPen2d pp=(PPen2d)ps->pens->GetByHandle(hpen);
        if (pp)return ps->_CreatePen(pp);
    }
    return 0;
}
_EXTERNC HBRUSH _EXPORT CreateGDIBrush2d(HSP2D hSpace2d,HOBJ2D hbrush)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        PBrush2d pb=(PBrush2d)ps->bruhs->GetByHandle(hbrush);
        if (pb)return ps->_CreateBrush(pb);
    }
    return 0;
}
_EXTERNC INT16 _EXPORT IsGroupContainObject2d(HSP2D hSpace2d,HOBJ2D g,HOBJ2D o)
{  _SPACE* ps = __GetSpace((HSPACE)hSpace2d);
    if (ps)return ps->IsGroupContainObject2d(g,o);
    return 0;
}

_EXTERNC HOBJ2D _EXPORT  CreatePen2d( HSP2D hSpace2d,INT16 Style,INT16 Width, COLORREF rgb,INT16 Rop2 )
{
    PSpace2d ps = GetSpace(hSpace2d);

    if (ps)
    {
        HOBJ2D h = ps->CreatePen(Style,Width,rgb,Rop2);

        if ((h) && (ps->record))
        {
            ps->NewMetaRecord2(mfcreatepen,Style,Width);
            ps->meta->Write(&rgb,sizeof(rgb));
            ps->meta->WriteWord(Rop2);

            ps->NewUndoRecord2(mfdeletetool,1,h);
            ps->CloseMetaRecord();
        }if (h)SetModify
                return h;
    }
    return 0;
}

/*Создает кисть и возвращает его HANDLE, если неудачно то 0*/
_EXTERNC HOBJ2D _EXPORT  CreateBrush2d( HSP2D hSpace2d,INT16 Style,INT16 Hatch,
                                        COLORREF rgb,HOBJ2D DibIndex,INT16 Rop2 )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        HOBJ2D h=ps->CreateBrush(Style,Hatch,rgb,DibIndex,Rop2);
        if ((h)&&(ps->record))
        {
            ps->NewMetaRecord2(mfcreatebrush,Style,Hatch);
            ps->meta->Write(&rgb,sizeof(rgb));
            ps->meta->WriteWord(DibIndex);
            ps->meta->WriteWord(Rop2);
            ps->NewUndoRecord2(mfdeletetool,2,h);
            ps->CloseMetaRecord();
        }
        if(h)SetModify
                return h;
    }
    return 0;
}

_EXTERNC BOOL _EXPORT SetBrushPoints2d(HSP2D hSpace2d,HOBJ2D index
                                       ,double x0,double y0,double x1,double y1)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (!ps) return FALSE;

    long s=ps->SetBrushPoints(index,x0,y0,x1,y1);
    if(!s) return FALSE;

    /*	if (ps->record)
  {
        ps->NewMetaRecord2(mfsetpenwidth,index,Width);
        ps->NewUndoRecord2(mfsetpenwidth,index,LOWORD(s));
        ps->CloseMetaRecord();
  }*/
    SetModify
            return TRUE;
}

_EXTERNC BOOL _EXPORT SetBrushColors2d(HSP2D hSpace2d,HOBJ2D index
                                       ,COLORREF* colors,int colors_count)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (!ps) return FALSE;

    long s=ps->SetBrushColors(index,colors,colors_count);
    if(!s) return FALSE;

    /*	if (ps->record)
  {
        ps->NewMetaRecord2(mfsetpenwidth,index,Width);
        ps->NewUndoRecord2(mfsetpenwidth,index,LOWORD(s));
        ps->CloseMetaRecord();
  }*/
    SetModify
            return TRUE;
}

_EXTERNC BOOL _EXPORT  SetPenWidth2d( HSP2D hSpace2d,HOBJ2D index,INT16 Width )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        long s=ps->SetPenWidth(index,Width);
        if(s)
        {
            if (ps->record)
            {
                ps->NewMetaRecord2(mfsetpenwidth,index,Width);
                ps->NewUndoRecord2(mfsetpenwidth,index,LOWORD(s));
                ps->CloseMetaRecord();
            }
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT  SetPenColor2d( HSP2D hSpace2d,HOBJ2D index, COLORREF rgb )
{  PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        long s=ps->SetPenColor(index,rgb);
        if(s)
        {if (ps->record){
                ps->NewMetaRecord1(mfsetpencolor,index);
                ps->meta->Write(&rgb,sizeof(rgb));
                ps->NewUndoRecord1(mfsetpencolor,index);
                s&=0xffffffl;
                ps->meta->Write(&s,sizeof(rgb));
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;
        }
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT SetPenRop2d( HSP2D hSpace2d,HOBJ2D index,INT16 Rop2 )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        long s=ps->SetPenRop2(index,Rop2);
        if(s)
        {if (ps->record){
                ps->NewMetaRecord2(mfsetpenrop2,index,Rop2);
                ps->NewUndoRecord2(mfsetpenrop2,index,LOWORD(s));
                ps->CloseMetaRecord();
            }SetModify
                    return 1;
        }
    }
    return 0;
};
_EXTERNC INT16 _EXPORT GetPenStyle2d( HSP2D hSpace2d,HOBJ2D index ){
    PPen2d pp=(PPen2d) GetTool(hSpace2d,PEN2D,index);
    if (pp)return pp->style;else return 0;
};
_EXTERNC INT16 _EXPORT GetPenWidth2d( HSP2D hSpace2d,HOBJ2D  index ){
    PPen2d pp=(PPen2d) GetTool(hSpace2d,PEN2D,index);
    if (pp)return pp->width;else return 0;
};

_EXTERNC COLORREF _EXPORT GetPenColor2d(HSP2D hSpace2d,HOBJ2D index)
{
    PPen2d pp = (PPen2d)GetTool(hSpace2d, PEN2D, index);
    if (pp)return pp->color;else return 0;
};

_EXTERNC INT16 _EXPORT GetPenRop2d( HSP2D hSpace2d,HOBJ2D index )
{
    PPen2d pp=(PPen2d) GetTool(hSpace2d,PEN2D,index);
    if (pp)return pp->rop2;else return 0;
};
_EXTERNC BOOL _EXPORT SetBrushStyle2d( HSP2D hSpace2d,HOBJ2D index,INT16 Style )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        long s=ps->SetBrushStyle(index,Style);
        if(s)
        {if (ps->record){
                ps->NewMetaRecord2(mfsetbrushstyle,index,Style);
                ps->NewUndoRecord2(mfsetbrushstyle,index,LOWORD(s));
                ps->CloseMetaRecord();}
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT SetBrushHatch2d( HSP2D hSpace2d,HOBJ2D index,INT16 Hatch )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        long s=ps->SetBrushHatch(index,Hatch);
        if(s){if (ps->record){
                ps->NewMetaRecord2(mfsetbrushhatch,index,Hatch);
                ps->NewUndoRecord2(mfsetbrushhatch,index,LOWORD(s));
                ps->CloseMetaRecord();}
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};
_EXTERNC BOOL  _EXPORT SetBrushDibIndex2d( HSP2D hSpace2d,HOBJ2D index,HOBJ2D DibIndex )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        long s=ps->SetBrushDibIndex(index,DibIndex);
        if(s){if (ps->record){
                ps->NewMetaRecord2(mfsetbrushdibindex,index,DibIndex);
                ps->NewUndoRecord2(mfsetbrushdibindex,index,LOWORD(s));
                ps->CloseMetaRecord();}
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
}

_EXTERNC BOOL _EXPORT SetBrushColor2d( HSP2D hSpace2d,HOBJ2D index, COLORREF rgb )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        long s=ps->SetBrushColor(index,rgb);
        if(s)
        {
            if (ps->record)
            {
                ps->NewMetaRecord1(mfsetbrushcolor,index);
                ps->meta->Write(&rgb,sizeof(rgb));
                ps->NewUndoRecord1(mfsetbrushcolor,index);
                s&=0xffffffl;
                ps->meta->Write(&s,sizeof(rgb));
                ps->CloseMetaRecord();
            }
            SetModify;
            return TRUE;
        }
    }
    return FALSE;
}

_EXTERNC BOOL _EXPORT SetBrushRop2d( HSP2D hSpace2d,HOBJ2D index,INT16 Rop2 )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        long s=ps->SetBrushRop2(index,Rop2);
        if(s){if (ps->record){
                ps->NewMetaRecord2(mfsetbrushrop2,index,Rop2);
                ps->NewUndoRecord2(mfsetbrushrop2,index,LOWORD(s));
                ps->CloseMetaRecord();}
            SetModify
                    return 1;
        }
    }
    return 0;
}
_EXTERNC INT16  _EXPORT GetBrushStyle2d( HSP2D hSpace2d,HOBJ2D index )
{
    PBrush2d pb=(PBrush2d)GetTool(hSpace2d,BRUSH2D,index);
    if (pb)
        return pb->style;
    else
        return 0;
};

_EXTERNC INT16 _EXPORT GetBrushHatch2d( HSP2D hSpace2d,HOBJ2D index )
{
    PBrush2d pb=(PBrush2d) GetTool(hSpace2d,BRUSH2D,index);
    if (pb)return pb->hatch;else return 0;
};
_EXTERNC INT16 _EXPORT GetBrushDibIndex2d( HSP2D hSpace2d,HOBJ2D index )
{
    PBrush2d pb=(PBrush2d) GetTool(hSpace2d,BRUSH2D,index);
    if (pb && pb->dib)return pb->dib->_HANDLE;else return 0;
};
_EXTERNC COLORREF _EXPORT GetBrushColor2d( HSP2D hSpace2d,HOBJ2D index )
{
    PBrush2d pb=(PBrush2d) GetTool(hSpace2d,BRUSH2D,index);
    if (pb)return pb->color;else return 0;
};
_EXTERNC INT16  _EXPORT GetBrushRop2d( HSP2D hSpace2d,HOBJ2D index )
{
    PBrush2d pb=(PBrush2d) GetTool(hSpace2d,BRUSH2D,index);
    if (pb)return pb->rop2;else return 0;
}
// Groups

_EXTERNC HOBJ2D _EXPORT CreateGroup2d( HSP2D hSpace2d,HOBJ2D * Objects2d,INT16 NumObjects )
{
    _SPACE *ps = __GetSpace((HSPACE)hSpace2d);
    if (ps&& (!ps->Overflow()))
    {
        HOBJ2D h=ps->CreateGroup(Objects2d, NumObjects);
        if (h)
        {/*
            if (ps->record)
             {
              ps->NewMetaRecord1(mfcreategroup2d,NumObjects);
              ps->meta->Write(Objects2d,sizeof(int)*NumObjects);

              ps->NewUndoRecord1(mfdeletegroup2d,h);
              ps->CloseMetaRecord();
             }*/
            ps->_SetModify();
        }
        return h;
    }
    return 0;
}
_EXTERNC HOBJ2D _EXPORT CreateRgnGroup2d( HSP2D hSpace2d, RGNGROUPITEM * items,INT16 NumItems )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps&& (!ps->Overflow())){
        HOBJ2D h=ps->CreateRgnGroup(items,NumItems);
        if (h){
            if (ps->record)
            {
                ps->NewMetaRecord1(mfcreatergngroup2d,NumItems);
                ps->meta->Write(items,sizeof(RGNGROUPITEM)*NumItems);
                ps->NewUndoRecord1(mfdeletegroup2d,h);
                ps->CloseMetaRecord();
            }SetModify}
        return h;
    }
    return 0;
}

_EXTERNC BOOL _EXPORT SetRgnGroupItems2d( HSP2D hSpace2d,HOBJ2D group2d,
                                          RGNGROUPITEM * Objects2d,INT16 NumObject)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        if (ps->record) //meta
        {PGroup2d po=(PGroup2d)ps->GetObjByHANDLE(group2d);
            if ((po)&& (po->WhoIsIt()==4)&&ps->ChkFutureRGroupMember(Objects2d,
                                                                     NumObject,group2d))
            {
                ps->NewMetaRecord2(mfsetrgngroupitems,group2d,NumObject);
                ps->meta->Write(Objects2d,sizeof(RGNGROUPITEM)*NumObject);

                INT16 c=po->items->count;

                ps->NewUndoRecord2(mfsetrgngroupitems,group2d,c);
                RGNGROUPITEM * old=new RGNGROUPITEM[c];
                ps->GetRgnGroupItems(group2d,old,c);
                ps->meta->Write(old,sizeof(RGNGROUPITEM)*c);
                delete old;
                ps->CloseMetaRecord();
            }else return 0;
        }
        BOOL h=ps->SetRgnGroupItems(group2d,Objects2d,NumObject);
        if ((h==0)&&(ps->record))_Error(EM_internal);
        if (h)SetModify
                return TRUE;
    }
    return FALSE;
}

_EXTERNC BOOL _EXPORT SetRgnGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,INT16 ItemNum,
                                         RGNGROUPITEM * Object2d )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {RGNGROUPITEM  Old;
        if (ps->record) ps->GetRgnGroupItem(group2d,ItemNum,&Old);
        int r=ps->SetRgnGroupItem(group2d,ItemNum,Object2d);
        if(r){
            if (ps->record)
            {
                ps->NewMetaRecord2(mfsetrgngroupitem,group2d,ItemNum);
                ps->meta->Write(Object2d,sizeof(RGNGROUPITEM));
                ps->NewUndoRecord2(mfsetrgngroupitem,group2d,ItemNum);
                ps->meta->Write(&Old,sizeof(RGNGROUPITEM));
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;
        }

    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT AddRgnGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,INT16 ItemNum,RGNGROUPITEM * Object2d )
{PSpace2d ps=GetSpace(hSpace2d);
    if(ps)
    {int r=ps->AddRgnGroupItem(group2d,ItemNum,Object2d);
        if (r){
            if(ps->record)
            {
                ps->NewMetaRecord2(mfaddrgngroupitem,group2d,ItemNum);
                ps->meta->Write(Object2d,sizeof(RGNGROUPITEM));
                ps->NewUndoRecord2(mfdeletegroupitem2d,group2d,ItemNum);
                ps->CloseMetaRecord();
            }SetModify}
        return r;
    }
    return 0;
};
_EXTERNC BOOL _EXPORT GetRgnGroupItems2d( HSP2D hSpace2d,HOBJ2D group2d,
                                          RGNGROUPITEM * Object2d,INT16 MaxObjects )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->GetRgnGroupItems(group2d,Object2d,MaxObjects); return FALSE;
};
_EXTERNC BOOL _EXPORT GetRgnGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,INT16 ItemNum,
                                         RGNGROUPITEM * Object2d )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->GetRgnGroupItem(group2d,ItemNum,Object2d);
    return FALSE;
};

_EXTERNC BOOL _EXPORT DeleteObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{
    _SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    if (!ps)
        return 0;

    switch(Object2d)
    {
        case OID_RCENTER:return ((PSpace2d)ps)->DeleteRotateCenter2d();
        case OID_AXIS3d:return  ((PSpace2d)ps)->DeleteAxis3d();
        case OID_FRAME2d:return ((PSpace2d)ps)->DeleteEditFrame2d();
        case OID_FRAME3d:return ((PSpace2d)ps)->DeleteEditFrame3d();
    }

    PObject po = ps->GetObjByHANDLE(Object2d);
    if(!po)
        return 0;
    /*if (ps->record)
     {
      ps->NewMetaRecord1(mfdeleteobject2d,Object2d);
      ps->NewUndoRecord(mfloadobject__);
      ps->__WriteObject(ps->meta,po);
      ps->CloseMetaRecord();
    }*/
    if (ps->WhoIsIt() == 17458)
        ((PSpace2d)ps)->DeleteObject2d(Object2d);

    else
        ((PSpace3d)ps)->DeleteObject3d(Object2d);
    ps->_SetModify();

    return 1;
}
_EXTERNC BOOL _EXPORT IsObjectVisible(HSPACE hsp,HOBJ2D o){
    _SPACE* ps=__GetSpace(hsp);
    if(ps){
        PObject po= ps->GetObjByHANDLE(o);
        if(po){
            if (ps->WhoIsIt()==17458)return ((PSpace2d)ps)->IsVisible(po);
            else return ((PSpace3d)ps)->space2d->IsVisible(po);
        }
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT DeleteGroup2d( HSP2D hSpace2d,HOBJ2D Object2d )
{
    _SPACE* ps = __GetSpace((HSPACE)hSpace2d);
    if (ps)
    {
        PGroup2d po = (PGroup2d)ps->GetObjByHANDLE(Object2d);
        if ((po)&& (po->IsGroup()))
        {
            /*if (ps->record){
                      ps->NewMetaRecord1(mfdeletegroup2d,Object2d);
                        if (po->owner)
                                {
                                  ps->NewUndoRecord1(mfstartofgroup,mfcreategroup2d);
                                 }else ps->NewUndoRecord(mfcreategroup2d);
                      int c=po->items->count;
                      ps->meta->WriteWord(c);
                      int i;
                      for (i=0;i<c;i++)
                                ps->meta->WriteWord(((PObject)po->items->At(i))->_HANDLE);
                      if (po->owner)
                        {ps->meta->WriteWord(mfaddgroupitem2d);
                                ps->meta->WriteWord(po->owner->_HANDLE);
                                ps->meta->WriteWord(po->_HANDLE);
                                ps->meta->WriteWord(mfendofgroup);
                        }
                      ps->CloseMetaRecord();
                     };
                     */
            ps->DeleteGroup(Object2d);
            ps->_SetModify();
            return 1;
        }
    }return 0;
};

_EXTERNC BOOL _EXPORT SetGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,INT16 ItemNum,HOBJ2D Object2d )
{_SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    if (ps){
        //HOBJ2D old=ps->GetGroupItem(group2d,ItemNum);
        BOOL h=ps->SetGroupItem(group2d,ItemNum,Object2d );
        if (h){
            /*
            if (ps->record)
             {
              ps->NewMetaRecord2(mfsetgroupitem2d,group2d,ItemNum);
              ps->meta->WriteWord(Object2d);

              ps->NewUndoRecord2(mfsetgroupitem2d,group2d,ItemNum);
                     ps->meta->WriteWord(old);
                     ps->CloseMetaRecord();
             }
            */
            ps->_SetModify();}
        return TRUE;
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT SetGroupItems2d( HSP2D hSpace2d,HOBJ2D group2d,HOBJ2D * Object2d,
                                       INT16 NumObject )
{
    _SPACE* ps=GetSpace(hSpace2d);
    if (ps)
    {
        /*
             if (ps->record) //meta
             {PGroup2d po=(PGroup2d)ps->GetObjByHANDLE(group2d);
             if ((po)&& (po->IsGroup())&&ps->ChkFutureGroupMember(Object2d,
                        NumObject,group2d))
                    {
                      ps->NewMetaRecord2(mfsetgroupitems2d,group2d,NumObject);
                      ps->meta->Write(Object2d,sizeof(int)*NumObject);
                      C_TYPE c=po->items->count;

                      ps->NewUndoRecord2(mfsetgroupitems2d,group2d,c);
                      C_TYPE i;
                      for (i=0;i<c;i++)
                                ps->meta->WriteWord(((PObject)po->items->At(i))->_HANDLE);
                      ps->CloseMetaRecord();
                      }else return FALSE;
             }*/
        BOOL h = ps->SetGr.oup(group2d, Object2d, NumObject);
        // if ((h==0)&&(ps->record))_Error(EM_internal);
        if (h)
            ps->_SetModify();
        return TRUE;
    }
    return FALSE;
}

_EXTERNC BOOL _EXPORT AddGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,HOBJ2D Object2d )
{
    _SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    if (ps)
        if( ps->AddToGroup(group2d,Object2d))
        {
            ps->_SetModify();
            return 1;
        }
    return 0;
}


_EXTERNC BOOL _EXPORT DelGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,HOBJ2D Object2d)
{
    _SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    if(ps)
    {
        INT16 z=ps->DeleteFromGroup(group2d, Object2d);
        if (z)
        {
            /*if(ps->record){
            // Почемутораньше undo and  redo были поменяны местами
                      ps->NewMetaRecord2(mfdeletegroupitem2d,group2d,Object2d);
                      ps->NewUndoRecord(mfloadgroupitem2d);
                      ps->__WriteGroupItem(ps->meta,group2d,Object2d,z-1);
                      ps->CloseMetaRecord();
                      };*/
            ps->_SetModify();
            return TRUE;
        }
    }
    return FALSE;
}

_EXTERNC HOBJ2D _EXPORT GetGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,INT16 ItemNum )
{
    _SPACE *ps=__GetSpace((HSPACE)hSpace2d);
    if (ps)
        return ps->GetGroupItem(group2d,ItemNum);
    return FALSE;
};

_EXTERNC INT16 _EXPORT GetGroupItemsNum2d( HSP2D hSpace2d,HOBJ2D group2d )
{
    _SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    if (ps)
        return ps->GetGroupCount(group2d);

    return FALSE;
}
_EXTERNC BOOL _EXPORT GetGroupItems2d( HSP2D hSpace2d,HOBJ2D group2d,HOBJ2D * Object2d,
                                       INT16 MaxObjects )
{ _SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    if (ps)return ps->GetGroup(group2d,Object2d,MaxObjects);
    return FALSE;
};
// Object2d;
_EXTERNC HOBJ2D _EXPORT CreatePolyline2d( HSP2D hSpace2d,HOBJ2D Pen,HOBJ2D Brush, POINT2D * points,
                                          INT16 NumPoints )
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps && (!ps->Overflow()))
    {
        HOBJ2D h = ps->CreatePolyline2d(Pen,Brush,points,NumPoints);
        if (h)
        {
            if (ps->record)
            {
                ps->NewMetaRecord2(mfcreatepolyline,Pen,Brush);
                ps->meta->WriteWord(NumPoints);
                ps->meta->WritePOINTs(points,NumPoints);

                ps->NewUndoRecord1(mfdeleteobject2d,h);
                ps->CloseMetaRecord();
            }
            SetModify;
        }
        return h;
    }
    return 0;
}


_EXTERNC HOBJ2D _EXPORT CreateBitmap2d( HSP2D hSpace2d,
                                        HOBJ2D DibIndex, POINT2D* SrcOrg, POINT2D* SrcSize,
                                        POINT2D* DstOrg, POINT2D* DstSize, INT16 angle)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps&& (!ps->Overflow()))
    {
        ChkPnt(*SrcOrg);ChkPnt(*SrcSize);
        ChkPnt(*DstOrg);ChkPnt(*DstSize);
        HOBJ2D h=ps->CreateBitmap2d(DibIndex,*SrcOrg,*SrcSize,*DstOrg,*DstSize,angle);
        if(h)
        {
            if (ps->record)
            {
                ps->NewMetaRecord1(mfcreatebitmap,DibIndex);
                ps->meta->WritePOINT(SrcOrg);
                ps->meta->WritePOINT(SrcSize);
                ps->meta->WritePOINT(DstOrg);
                ps->meta->WritePOINT(DstSize);
                ps->meta->WriteWord(angle);
                ps->NewUndoRecord1(mfdeleteobject2d,h);
                ps->CloseMetaRecord();
            }
            SetModify
        }
        return h;
    }
    return 0;
};


_EXTERNC HOBJ2D _EXPORT CreateDoubleBitmap2d( HSP2D hSpace2d,HOBJ2D DoubleDibIndex,
                                              POINT2D* SrcOrg, POINT2D* SrcSize,
                                              POINT2D* DstOrg, POINT2D* DstSize,INT16 angle  )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps&& (!ps->Overflow())){
        ChkPnt(*SrcOrg);ChkPnt(*SrcSize);
        ChkPnt(*DstOrg);ChkPnt(*DstSize);

        HOBJ2D h=ps->CreateDoubleBitmap2d(DoubleDibIndex,
                                          *SrcOrg,*SrcSize,*DstOrg,*DstSize,angle);
        if (h){
            if (ps->record)
            {
                ps->NewMetaRecord1(mfcreatedoublebitmap,DoubleDibIndex);
                ps->meta->WritePOINT(SrcOrg);
                ps->meta->WritePOINT(SrcSize);
                ps->meta->WritePOINT(DstOrg);
                ps->meta->WritePOINT(DstSize);
                ps->meta->WriteWord(angle);

                ps->NewUndoRecord1(mfdeleteobject2d,h);
                ps->CloseMetaRecord();
            }SetModify}
        return h;
    }
    return 0;
}
_EXTERNC HOBJ2D _EXPORT CreateTextRaster2d( HSP2D hSpace2d,HOBJ2D TextIndex,
                                            POINT2D *Org, POINT2D *Size,POINT2D *Delta,INT16 DstAngle  )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps&& (!ps->Overflow())){
        ChkPnt(*Org);ChkPnt(*Size);
        ChkPnt(*Delta);

        HOBJ2D h=ps->CreateTextRaster2d(TextIndex,*Org,*Size,
                                        *Delta,DstAngle );
        if(h){
            if (ps->record)
            {
                ps->NewMetaRecord1(mfcreatetextraster,TextIndex);
                ps->meta->WritePOINT(Org);
                ps->meta->WritePOINT(Size);
                ps->meta->WritePOINT(Delta);
                ps->meta->WriteWord(DstAngle);
                ps->NewUndoRecord1(mfdeleteobject2d,h);
                ps->CloseMetaRecord();
            }SetModify}
        return h;
    }
    return 0;
}

// -- Only VectorObjects
_EXTERNC HOBJ2D _EXPORT GetPenObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{ PLine_2d pl=(PLine_2d)_GetObject2d(hSpace2d,Object2d,20);
    if ((pl)&&(pl->pen))return pl->pen->_HANDLE;
    return 0;
};
_EXTERNC BOOL _EXPORT SetPenObject2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D PenIndex )
{PSpace2d ps=GetSpace(hSpace2d);
    if(ps){
        DWORD h=ps->SetVectorPen2d(Object2d,PenIndex);
        if(h)
        {if(ps->record)
            {
                ps->NewMetaRecord2(mfsetpenobject,Object2d,PenIndex);
                ps->NewUndoRecord2(mfsetpenobject,Object2d,LOWORD(h));
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;
        };
    }
    return FALSE;
};
_EXTERNC HOBJ2D _EXPORT GetBrushObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{ PLine_2d pl=(PLine_2d)_GetObject2d(hSpace2d,Object2d,20);
    if ((pl)&&(pl->brush))return pl->brush->_HANDLE;
    return 0;
};
_EXTERNC BOOL _EXPORT SetBrushObject2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D BrushIndex )
{PSpace2d ps=GetSpace(hSpace2d);
    if(ps){
        DWORD h=ps->SetVectorBrush2d(Object2d,BrushIndex);
        if(h)
        {if(ps->record)
            {
                ps->NewMetaRecord2(mfsetbrushobject,Object2d,BrushIndex);
                ps->NewUndoRecord2(mfsetbrushobject,Object2d,LOWORD(h));
                ps->CloseMetaRecord();
            }SetModify
                    return 1;
        };
    }
    return 0;
};
_EXTERNC INT16 _EXPORT GetRgnCreateMode( HSP2D hSpace2d,HOBJ2D Rgn2d)
{
    PLine_2d pl=(PLine_2d)_GetObject2d(hSpace2d,Rgn2d,20);
    if (pl)return pl->GetRgnCreateMode();
    return 0;
};
_EXTERNC BOOL _EXPORT SetRgnCreateMode( HSP2D hSpace2d,HOBJ2D Rgn2d,HOBJ2D mode )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){int omode=ps->SetRgnCreateMode(Rgn2d,mode);
        if(omode){
            if (ps->record)
            {
                ps->NewMetaRecord2(mfsetrgncreatemode,Rgn2d,mode);
                ps->NewUndoRecord2(mfsetrgncreatemode,Rgn2d,omode);
                ps->CloseMetaRecord();
            }SetModify}
        return omode;
    }return 0;
};
_EXTERNC BOOL _EXPORT AddVectorPoint2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,
                                        INT16 pointNum,POINT2D *point ){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        ChkPnt(*point);
        int h=ps->AddVectorPoint2d(VectorObj2d, pointNum, *point);
        if(h)
        {
            if(ps->record)
            {
                ps->NewMetaRecord2(mfaddvectorpoint,VectorObj2d,pointNum);
                ps->meta->WritePOINT(point);
                ps->NewUndoRecord2(mfdelvectorpoint,VectorObj2d,pointNum);
                ps->CloseMetaRecord();
            };
            SetModify;
        }
        return TRUE;
    }
    return 0;
};
_EXTERNC BOOL  _EXPORT SetVectorArrows2d(HSP2D hSpace2d,HOBJ2D Object2d,ARROW2D*a){
    PSpace2d ps=GetSpace(hSpace2d);
    if(ps && ps->SetVectorArrows2d(Object2d,a)){
        SetModify
                return TRUE;
    }
    return FALSE;
};
_EXTERNC BOOL  _EXPORT GetVectorArrows2d(HSP2D hSpace2d,HOBJ2D Object2d,ARROW2D*a){
    PLine_2d pl=(PLine_2d)_GetObject2d(hSpace2d,Object2d,20);
    if (pl && pl->arrows){
        *a= *(pl->arrows);
        return TRUE;
    }else
    { memset(a,0,sizeof(ARROW2D)); }
    return FALSE;
};

_EXTERNC BOOL _EXPORT DelVectorPoint2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,INT16 pointNum)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        POINT2D p;
        if(ps->DelVectorPoint2d(VectorObj2d,pointNum,p)){
            if(ps->record)
            {
                ps->NewMetaRecord2(mfdelvectorpoint,VectorObj2d,pointNum);
                ps->NewUndoRecord2(mfaddvectorpoint,VectorObj2d,pointNum);
                ps->meta->WritePOINT(&p);
                ps->CloseMetaRecord();
            };SetModify
                    return TRUE;}
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT SetVectorPoints2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,
                                         POINT2D * points,INT16 NumPoints )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        if(ps->record)
        {
            PLine_2d pl=(PLine_2d)ps->GetObjByHANDLE(VectorObj2d);
            if ((pl==NULL)||(pl->WhoIsIt()!=20))return FALSE;
            ps->NewMetaRecord2(mfsetvectorpoints,VectorObj2d,NumPoints);
            ps->meta->WritePOINTs(points,NumPoints);
            ps->NewUndoRecord2(mfsetvectorpoints,VectorObj2d,pl->count);
            ps->meta->WritePOINTs(points,pl->count);
            ps->CloseMetaRecord();
        }
        if( ps->SetVectorPoints2d(VectorObj2d,points,NumPoints))
        {
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT SetVectorPoint2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,
                                        INT16 pointNum,POINT2D* point )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        ChkPnt(*point);
        POINT2D p=*point ;
        if(ps->SetVectorPoint2d(VectorObj2d,pointNum,p))
        {
            if(ps->record)
            {
                ps->NewMetaRecord2(mfsetvectorpoint,VectorObj2d,pointNum);
                ps->meta->WritePOINT(point);
                ps->NewUndoRecord2(mfsetvectorpoint,VectorObj2d,pointNum);
                ps->meta->WritePOINT(&p);
                ps->CloseMetaRecord();
            };
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT GetVectorPoint2d( HSP2D hSpace2d,
                                        HOBJ2D VectorObj2d,INT16 pointNum ,POINT2D * point)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps==NULL)return 0;
    PLine_2d pl=(PLine_2d)ps->GetObjByHANDLE(VectorObj2d);
    if((pl)&&(pl->WhoIsIt()==20))
    {
        if ((pl->count>pointNum)&&(pointNum>-1)&&point)
        {
            *point =pl->points[pointNum];
            ps->LPtoCRD(point);
            return TRUE;
        }
        else
        {
            if (point)
                point->x=point->y=0.0;
        }
    }
    return FALSE;
};

_EXTERNC INT16 _EXPORT GetVectorNumPoints2d( HSP2D hSpace2d,HOBJ2D VectorObj2d )
{ PLine_2d pl=(PLine_2d)_GetObject2d(hSpace2d,VectorObj2d,20);
    if (pl)return pl->count;
    return 0;
};
_EXTERNC BOOL _EXPORT GetVectorPoints2d( HSP2D hSpace2d,
                                         HOBJ2D VectorObj2d, POINT2D * points,INT16 MaxPoints )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps==NULL)return 0;
    PLine_2d pl=(PLine_2d)ps->GetObjByHANDLE(VectorObj2d);
    if((pl)&&(pl->WhoIsIt()==20)){
        INT16 i,j;j=min(pl->count,MaxPoints);
        for (i=0;i<j;i++)
            points[i]=pl->points[i];
        ps->LPtoCRD(points,MaxPoints);
        return TRUE;
    }else{
        if (points)
            for(INT16 i=0;i<MaxPoints;i++){points[i].x=points[i].y=0.0;  }
    }
    return FALSE;
};
//*-----------------------------------------------------&
_EXTERNC HOBJ2D _EXPORT GetDibObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{
    PBitmap_2d pb=(PBitmap_2d)_GetObject2d(hSpace2d,Object2d,21);
    if (pb)	return pb->dib->_HANDLE;
    return 0;
};
_EXTERNC BOOL _EXPORT SetDibObject2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D DibIndex )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {HOBJ2D h=ps->SetDibObject2d(Object2d,DibIndex);
        if (h){
            if (ps->record)
            {
                ps->NewMetaRecord2(mfsetdibobject,Object2d,DibIndex);
                ps->NewUndoRecord2(mfsetdibobject,Object2d,h);
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;}
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT SetBitmapSrcRect(HSP2D hSpace2d,HOBJ2D Object2d,
                                       POINT2D*org,POINT2D *size)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        POINT2D oo=*org,os=*size;
        if(ps->SetBitmapSrcRect(Object2d,oo,os))
        {
            if (ps->record)
            {
                ps->NewMetaRecord1(mfsetbitmapsrcrect,Object2d);
                ps->meta->WritePOINT(org);
                ps->meta->WritePOINT(size);
                ps->NewUndoRecord1(mfsetbitmapsrcrect,Object2d);
                ps->meta->WritePOINT(&oo);
                ps->meta->WritePOINT(&os);
                ps->CloseMetaRecord();
            }
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT GetBitmapSrcRect(HSP2D hSpace2d,HOBJ2D Obj2d,POINT2D FAR * org,POINT2D FAR* size)
{PBitmap_2d pb=(PBitmap_2d)_GetObject2d(hSpace2d,Obj2d,0);
    if ((pb)&&((pb->WhoIsIt()==21)||(pb->WhoIsIt()==22)))
    {
        if  (org)*org=pb->SrcOrigin;
        if (size)*size=pb->SrcSize;
        return TRUE;
    }return FALSE;
};
_EXTERNC HOBJ2D _EXPORT GetDoubleDibObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{PDoubleBitmap_2d pb=(PDoubleBitmap_2d)_GetObject2d(hSpace2d,Object2d,22);
    if (pb)
    {
        return pb->dib->_HANDLE;
    }return 0;
};
_EXTERNC BOOL _EXPORT SetDoubleDibObject2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D DoubleDibIndex )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {HOBJ2D h=ps->SetDoubleDibObject2d(Object2d,DoubleDibIndex);
        if(h){
            if (ps->record)
            {
                ps->NewMetaRecord2(mfsetdoubledibobject,Object2d,DoubleDibIndex);
                ps->NewUndoRecord2(mfsetdoubledibobject,Object2d,h);
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;}
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT SetTextDelta2d( HSP2D hSpace2d,HOBJ2D Object2d ,POINT2D *p){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {POINT2D po=*p;
        int h=ps->SetTextDelta2d(Object2d,po);
        if(h){
            if (ps->record)
            {
                ps->NewMetaRecord1(mfsettextdelta,Object2d);
                ps->meta->WritePOINT(p);
                ps->NewUndoRecord1(mfsettextdelta,Object2d);
                ps->meta->WritePOINT(&po);
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;}
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT GetTextDelta2d( HSP2D hSpace2d,HOBJ2D Object2d,POINT2D *d){
    PText_2d pb=(PText_2d)_GetObject2d(hSpace2d,Object2d,23);
    if (pb){ *d=pb->Delta;return TRUE;
    }return FALSE;
};

_EXTERNC BOOL _EXPORT SetTextObject2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D TextIndex )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {HOBJ2D h=ps->SetTextObject2d(Object2d,TextIndex);
        if(h){
            if (ps->record)
            {
                ps->NewMetaRecord2(mfsettextobject,Object2d,TextIndex);
                ps->NewUndoRecord2(mfsettextobject,Object2d,h);
                ps->CloseMetaRecord();
            }SetModify
                    return 1;}
    }
    return 0;
};
// -- for 3D Space Proective
_EXTERNC HSP3D _EXPORT Get3dSpace2d( HSP2D hSpace2d,HOBJ2D Object2d )
{PView3d_2d pb=(PView3d_2d)_GetObject2d(hSpace2d,Object2d,24);
    if (pb)
    {
        INT16 hsp3d=spaces->IndexOf(pb->space3d)+1;
        return (HSP3D)hsp3d;
    }return 0;
};
_EXTERNC BOOL _EXPORT Set3dSpace2d( HSP2D hSpace2d,HOBJ2D Object2d, HSP3D hSpace3d )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {HSP3D h=ps->Set3dSpace2d(Object2d,hSpace3d);
        if(h){
            if (ps->record)
            {
                ps->NewMetaRecord2(mfset3dspace2d,Object2d,LOBYTE(hSpace3d));
                ps->NewUndoRecord2(mfset3dspace2d,Object2d,LOBYTE(h));
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;}
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT Set3dViewCamera3d(HSP2D hSpace2d,HOBJ2D Pr3dObject2d,
                                        HOBJ2D hCamera )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        if (ps->Set3dViewCamera(Pr3dObject2d,
                                hCamera,ps->record)){SetModify};
        return TRUE;
    }
    return FALSE;
};
_EXTERNC HOBJ2D _EXPORT Get3dViewCamera3d(HSP2D hSpace2d,HOBJ2D Pr3dObject2d)
{PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Pr3dObject2d,24);
    if (pv){return pv->camera->_HANDLE;}
    return 0;
};
//--------------------------------------------------------------------------
_EXTERNC BOOL _EXPORT SetObjectOrg2d( HSP2D hSpace2d,HOBJ2D Object2d, POINT2D* NewOrigin )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        POINT2D op= *NewOrigin;
        ChkPnt(op);
        
        BOOL h=ps->SetObjectOrg2d(Object2d, op);
        if(h)
        {
            if ((ps->record)&&(Object2d<OID_RESERVED)&&(ps->_dragobj!=Object2d))
            {
                ps->NewMetaRecord1(mfsetobjectorg2d,Object2d);
                ps->meta->WritePOINT(NewOrigin);

                ps->NewUndoRecord1(mfsetobjectorg2d,Object2d);
                ps->meta->WritePOINT(&op);
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT SetObjectSize2d( HSP2D hSpace2d,HOBJ2D Object2d, POINT2D *NewSize )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        POINT2D op=*NewSize;
        ChkPnt(op);
        if ((ps->record)&&(Object2d<OID_RESERVED))
        {
            PObject po=ps->GetObjByHANDLE(Object2d);
            if (po)
            {
                ps->NewMetaRecord1(mfsetobjectsize2d,Object2d);
                ps->meta->WritePOINT(NewSize);
                ps->NewUndoRecord1(mfcoordinates,Object2d);
                po->StoreCoordinates(ps->meta);
                ps->CloseMetaRecord();
            }
        }
        if(ps->SetObjectSize2d(Object2d,op))
        {
            SetModify;
            return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT RotateObject2d( HSP2D hSpace2d,HOBJ2D Object2d, ROTATOR2D * rotator )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        ChkPnt(rotator->center);
        rotator->angle = _round(rotator->angle);
        PObject po=ps->GetObjByHANDLE(Object2d);
        if (po)
        {
            if ((ps->record) && (Object2d < OID_RESERVED))
            {
                ps->NewMetaRecord1(mfrotateobject2d,Object2d);
                ps->meta->Write(rotator,sizeof(ROTATOR2D));
                ps->NewUndoRecord1(mfcoordinates,Object2d);
                po->StoreCoordinates(ps->meta);
                ps->CloseMetaRecord();
            }
            SetModify;

            ps->RotateObject2d(Object2d, rotator);
            return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC gr_float _EXPORT GetObjectAngle2d( HSP2D hSpace2d,HOBJ2D Object2d)
{
    PObject po=_GetObject2d(hSpace2d,Object2d,0);
    if (po)
    {
        if (po->WhoIsIt()==50)
        {
            return ((PEditFrame)po)->angle;
        }
        return po->GetAngle();
    }
    return 0.0;
}

_EXTERNC BOOL _EXPORT SetObjectAlpha2d(HSP2D hSpace2d,HOBJ2D Object2d,gr_float alpha /*0.0 .. 1.0*/)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PObject po=ps->GetObjByHANDLE(Object2d);
        if (po)
        {
            /*if ((ps->record)&&(Object2d<OID_RESERVED))
            {
                ps->NewMetaRecord1(mfsetbjectalpha2d,Object2d);
                ps->meta->Write(alpha,sizeof(gr_float));
                ps->NewUndoRecord1(mfalpha,Object2d);
                //po->StoreCoordinates(ps->meta);
                ps->CloseMetaRecord();
            }*/
            ps->SetObjectAlpha2d(Object2d,alpha);
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC gr_float _EXPORT GetObjectAlpha2d(HSP2D hSpace2d,HOBJ2D Object2d)
{
    PObject2d po=_GetObject2d(hSpace2d,Object2d,0);
    if (po)
    {
        return po->GetAlpha();
    }
    return 0.0;
}


//--------- Cлужебные функции -----------------
HOBJ2D __SGetObject(PObject2d po,HOBJ2D parent)
{ while ((po->owner)&&(po->owner->_HANDLE!=parent)) po=(PObject2d)po->owner;
    if (po->owner)return po->_HANDLE;
    return -po->_HANDLE;
}
//----------------------------------------------

PObject2d __GetObj2dFromPoint2d(PSpace2d ps,POINT2D Point,UINT32 layer)
{
    C_TYPE j = 32760;
    PObject2d _po = NULL;
    int k;
    for (int i = ps->primary->count-1; i>=0; i--)
    {
        PObject2d po = (PObject2d)ps->primary->At(i);

        if ((ps->IsVisible(po)) &&
            ((po->options &OF_HARDDONTSELECT)==0) &&
            ((po->options & OF_DONTSELECT) ? (ps->State & SF_DONTSELECT) : 1))
        {
            //Находим ближайший к клику объект
            k = po->GetDistToMouse(Point);
            if (k)
            {
                if (k < j)
                {
                    _po = po;
                    j = k;
                }
            }
            else
                return po;
        }
    }

    if ((_po) && (k<10))
        return _po;

    return 0;
}

#define OF_HIDDEN         1
#define OF_EDITING        2
#define OF_HARDDONTSELECT 4
#define OF_DONTSELECT     8

_EXTERNC DWORD _EXPORT SetSpaceParam2d( HSP2D hSpace2d,UINT16 command,DWORD data)
{
    switch(command)
    {
        case  SP_GETPALETTE:
            return (DWORD)palette_256;
    }
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
        return ps->SetSpaceParam(command,data);
    return 0;
}

_EXTERNC HOBJ2D  _EXPORT GetObjectFromPoint2d(HSP2D hSpace2d, POINT2D* _Point,HOBJ2D * primary ,UINT32 layer)
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
    {
        POINT2D Point = *_Point;
        ps->CRDtoLP(&Point);

        if ((ps->RCenter)&&(ps->RCenter->GetDistToMouse(Point)<2))
            return OID_RCENTER;

        if (ps->Frame)
        {
            HOBJ2D h=ps->GetFrameCursor(Point);
            if (h)
                return OID_FRAME2d1+(h-1);
        }
        PObject2d po= __GetObj2dFromPoint2d(ps,Point,layer);
        if (po)
        {
            if(primary)
                *primary = po->_HANDLE;

            return ps->GetGrandOwner(po);
        }
    }
    return 0;
}

_EXTERNC HOBJ2D _EXPORT GetSubObjectFromPoint2d( HSP2D hSpace2d,HOBJ2D ParentObject,POINT2D*_Point,HOBJ2D * primary ,UINT32 layer)
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
    {
        POINT2D Point =*_Point;
        ps->CRDtoLP(&Point);
        PObject2d po = __GetObj2dFromPoint2d(ps,Point,layer);
        if (po)
        {
            if (primary)
                *primary = po->_HANDLE;
            return
                    __SGetObject(po,ParentObject);
        }
    }
    return 0;
}

_EXTERNC INT16 _EXPORT GetLinePointFromPoint2d( HSP2D hSpace2d,HOBJ2D Vect, POINT2D* _Point )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {PLine_2d pl=(PLine_2d)ps->GetObjByHANDLE(Vect);
        if ((pl)&&(pl->WhoIsIt()==20))
        {INT16 i;POINT2D c;
            POINT2D Point =*_Point;
            ps->CRDtoLP(&Point);
            for (i=0;i<pl->count;i++)
            {
                c=pl->points[i];
                if ((fabs(c.x-Point.x)<2) && (fabs(c.y-Point.y)<2))return i;
            }}}
    return -1;
}
_EXTERNC BOOL _EXPORT GetObjectOrgSize2d( HSP2D hSpace2d, HOBJ2D Object2d,POINT2D *o,POINT2D *s )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        PObject po=(PObject)ps->GetObjByHANDLE(Object2d);
        if (po)
        {
            if (Object2d>=OID_FRAME2d1 && Object2d<=OID_FRAME2d8)
            {
                if (o)*o=ps->GetFramePoint(Object2d-OID_FRAME2d1+1);
                if (s){s->x=0;s->y=0;}
            }
            else
            {
                if (o)*o=po->GetOrigin2d();
                if (s)*s=po->GetSize2d();
            }
            if(o)ps->LPtoCRD(o);
            return TRUE;
        }
    }
    if (o)
    {
        o->x = 0.0;
        o->y=0.0;
    }
    if (s)
    {
        s->x=0.0;
        s->y=0.0;
    }

    return FALSE;
};

_EXTERNC BOOL  _EXPORT GetObjectRect2d(HSP2D hSpace2d,HOBJ2D Object2d,RECT2D*r)
{
    TObject2d *po =_GetObject2d(hSpace2d,Object2d);
    if(po)
    {
        RECT _r;
        _r=po->GetRect(1);
        r->left=_r.left;
        r->right=_r.right;
        r->top=_r.top;
        r->bottom=_r.bottom;
        return TRUE;
    }
    return FALSE;
}; // view
//   http://www.intel.com/design/pentium/cpuinfo.zip

_EXTERNC BOOL _EXPORT GetSpaceExtent2d(HSP2D hSpace2d,RECT2D &rect,BOOL usecrd){
    BOOL set=0;
    PSpace2d ps=GetSpace(hSpace2d);
    if(ps){
        RECT2D r;

        C_TYPE zo;
        HOBJ2D obj;
        for(zo=0;zo<ps->primary->count;zo++)
        {
            PObject po=(PObject )ps->primary->At(zo);
            if(ps->IsVisible(po)){
                POINT2D org=po->GetOrigin2d();;
                POINT2D size=po->GetSize2d();
                if (!set){
                    set=TRUE;
                    r.left=org.x;r.top=org.y;
                    r.right=org.x+size.x;r.bottom=org.y+size.y;
                }else{
                    if (org.x<r.left)r.left=org.x;
                    if (org.y<r.top)r.top=org.y;
                    if (r.right<org.x+size.x)r.right=org.x+size.x;
                    if (r.bottom<org.y+size.y)r.bottom=org.y+size.y;
                }
            }}

        if(set){
            if(usecrd)ps->LPtoCRD((POINT2D*)&r,2);
            rect=r;
        };
    }
    return set;
}

_EXTERNC HOBJ2D _EXPORT CreateDib2d( HSP2D hSpace2d, HGLOBAL Dib )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if(ps)
    {
        HOBJ2D h=ps->CreateDib(Dib);
        if(h)
        {
            if (ps->record)
            {
                ps->NewMetaRecord(mfcreatedib);
                ps->dibs->PutItem(ps->meta, ps->dibs->GetByHandle(h));
                ps->NewUndoRecord(mfdeletetool);
                ps->meta->WriteWord(3);
                ps->meta->WriteWord(h);
                ps->CloseMetaRecord();
            }
            SetModify
        }
        return h;
    }
    return 0;
}

_EXTERNC HOBJ2D _EXPORT CreateDib2dFromResource( HSP2D hSpace2d,
                                                 HINSTANCE hInstance,LPCSTR lpszBitmap)
{
    HRSRC resource= FindResource(hInstance,lpszBitmap,RT_BITMAP);
    if (resource)
    {
        HGLOBAL res=LoadResource(hInstance,resource );
        if(res)
        {
            int good=1;
            void * adr1 =LockResource(res);
#ifdef WIN32
            long size=SizeofResource(hInstance,resource);
#else
            long size=GlobalSize(res);
#endif
            HGLOBAL dib=GlobalAlloc(GMEM_MOVEABLE,size);
            void * adr2=GlobalLock(dib);
            if (adr1 && adr2)
                hmemcpy(adr2,adr1,size);
            else good=0;
            GlobalUnlock(dib);
            UnlockResource(res);
            FreeResource(res);
            if (good)
                return CreateDib2d(hSpace2d,dib);
        }
    }
    return 0;
}

_EXTERNC HOBJ2D _EXPORT FCreateDib2d(HSP2D hSpace2d,LPSTR FileName)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        TDOSStream ds(FileName,TDOSStream::stOpenRead);
        if (ds.status)return 0;
        PDib2d pd=new TDib2d(0,&ds);
        if (pd==NULL)return 0;
        if (pd->dib==NULL){delete pd;return 0;}
        HOBJ2D h=ps->dibs->InsertObject(pd);
        if (ps->record)
        {
            ps->NewMetaRecord(mfcreatedib);
            ps->dibs->PutItem(ps->meta,ps->dibs->GetByHandle(h));
            ps->NewUndoRecord(mfdeletetool);
            ps->meta->WriteWord(3);
            ps->meta->WriteWord(h);
            ps->CloseMetaRecord();
        }
        SetModify
                return h;
    }
    return 0;
};

_EXTERNC HOBJ2D _EXPORT FCreateDoubleDib2d(HSP2D hSpace2d,LPSTR FileName)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        TDOSStream ds(FileName, TDOSStream::stOpenRead);
        if (ds.status)
            return 0;

        PDoubleDib2d pd = new TDoubleDib2d(0, &ds);

        if (pd==NULL)
            return 0;

        if (pd->dib==NULL || pd->andbitmap ==NULL || pd->andbitmap->dib==NULL)
        {
            delete pd;
            return 0;
        }
        HOBJ2D h=ps->ddibs->InsertObject(pd);
        if (ps->record)
        {
            ps->NewMetaRecord(mfcreatedoubledib);
            ps->ddibs->PutItem(ps->meta,ps->ddibs->GetByHandle(h));
            ps->NewUndoRecord(mfdeletetool);
            ps->meta->WriteWord(4);
            ps->meta->WriteWord(h);
            ps->CloseMetaRecord();
        }
        SetModify;
        return h;
    }
    return 0;
};

_EXTERNC COLORREF _EXPORT DibGetPixel2d( HSP2D hSpace2d,HOBJ2D hdib,int x,int y)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PDib2d pd=(PDib2d)ps->dibs->GetByHandle(hdib);
        if(pd)
        {
            return pd->GetPixel(x,y,NULL);
        }
    }
    return 0x10000000ul;
}

_EXTERNC BOOL _EXPORT DibSetPixel2d( HSP2D hSpace2d,HOBJ2D hdib,int x,int y,COLORREF rgb)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PDib2d pd=(PDib2d)ps->dibs->GetByHandle(hdib);
        if(pd)
        {
            bool r=pd->SetPixel(x,y,rgb);
            if(r)ps->ChkDib(pd);
            return r;
        }
    }
    return false;
}


_EXTERNC COLORREF _EXPORT DoubleDibGetPixel2d( HSP2D hSpace2d,HOBJ2D hDDib,int x,int y)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PDoubleDib2d pdd=(PDoubleDib2d)ps->ddibs->GetByHandle(hDDib);
        if(pdd)
            return pdd->GetPixel(x,y,NULL);
    }
    return 0x10000000ul;
}

_EXTERNC HOBJ2D _EXPORT CreateRDib2d( HSP2D hSpace2d,LPSTR FileName)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        HOBJ2D h=ps->CreateRDib2d(FileName);
        if (ps->record && h)
        {
            ps->NewMetaRecord(mfrcreatedib);
            ps->meta->WriteStr(FileName);
            ps->NewUndoRecord(mfdeletetool);
            ps->meta->WriteWord(3);
            ps->meta->WriteWord(h);
            ps->CloseMetaRecord();
        }
        SetModify
                return h;
    }
    return 0;
}

_EXTERNC BOOL _EXPORT SetRDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR FileName)
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        char str[256];
        BOOL h=ps->SetRDib2d(dib,FileName,str);
        if(h){
            if (ps->record)
            {
                ps->NewMetaRecord1(mfrsetdib,dib);
                ps->meta->WriteStr(FileName);
                ps->NewUndoRecord1(mfrsetdib,dib);
                ps->meta->WriteStr(str);
                ps->CloseMetaRecord();
            }SetModify
                    return TRUE;
        }
    }
    return 0;
}

_EXTERNC int _EXPORT CreateRDoubleDib2d( HSP2D hSpace2d,LPSTR FileName)
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        HOBJ2D h=ps->CreateRDoubleDib2d(FileName);
        if(h){
            if (ps->record)
            {ps->NewMetaRecord(mfrcreatedoubledib);
                ps->meta->WriteStr(FileName);
                ps->NewUndoRecord2(mfdeletetool,4,h);
                ps->CloseMetaRecord();
            }SetModify}
        return h;
    }
    return 0;
}
_EXTERNC INT16 _EXPORT GetRDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR dst,INT16 c)
{
    PRefToDib2d pd=(PRefToDib2d) GetTool(hSpace2d,DIB2D,dib);
    if (pd && pd->WhoIsIt()==110)
    {
        INT16 l=min(c,((INT16)(lstrlen(pd->name))+1));
        lstrcpyn(dst,pd->name,l);
        return l;
    }
    return 0;
};

_EXTERNC INT16 _EXPORT GetRDoubleDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR dst,INT16 c)
{
    PRefToDoubleDib2d pd= (PRefToDoubleDib2d) GetTool(hSpace2d,DOUBLEDIB2D,dib);
    if (pd && pd->WhoIsIt()==111)
    {
        INT16 l=min(c,(((INT16)lstrlen(pd->name))+1));
        lstrcpyn(dst,pd->name,l);
        return l;
    }return 0;
}

_EXTERNC BOOL _EXPORT SetRDoubleDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR FileName)
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        char str[80];
        BOOL h=ps->SetRDoubleDib2d(dib,FileName,str);
        if(h){
            if (ps->record)
            {
                ps->NewMetaRecord1(mfrsetdoubledib,dib);
                ps->meta->WriteStr(FileName);
                ps->NewUndoRecord1(mfrsetdoubledib,dib);
                ps->meta->WriteStr(str);
                ps->CloseMetaRecord();
            }SetModify}
        return h;
    }
    return 0;
}

_EXTERNC BOOL _EXPORT SetDib2d( HSP2D hSpace2d,HOBJ2D DibIndex, HGLOBAL Dib )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PDib2d pd=(PDib2d)ps->dibs->GetByHandle(DibIndex);
        if (pd==NULL)return 0;
        int t=pd->WhoIsIt()==ttDIB2D;
        if (ps->record && t)
        {
            TDib2d db(Dib);db.SetFreeDib(FALSE);
            if (db.dib==NULL)return 0;
            ps->NewMetaRecord1(mfsetdib,DibIndex);
            db._Store(ps->meta);
            ps->NewUndoRecord1(mfsetdib,DibIndex);
            pd->_Store(ps->meta);
            ps->CloseMetaRecord();
        }
        BOOL h=ps->SetDib(DibIndex, Dib);
        if (h==0 && ps->record && t)_Error(EM_internal);
        if (h && (!t))
        {
            PDib2d pp=(PDib2d)pd->GetMainObject();
            if (pp==NULL){_Error(EM_internal);return 0;}
            {
                TDOSStream ds(pp->filename,TDOSStream::stCreate);
                if(!ds.status)pp->_Store(&ds);
            }
            for(C_TYPE i=0;i<spaces->count;i++)
            {
                PSpace2d _ps=(PSpace2d)spaces->At(i);
                if (_ps && (_ps!=ps))
                    for(C_TYPE j=0;j<_ps->dibs->count;j++)
                    {
                        pd=(PDib2d)_ps->dibs->At(j);
                        if (pd->GetMainObject()==pp)_ps->ChkDib(pd);
                    }
            }
        }
        if(h){SetModify}
        return TRUE;
    }
    return FALSE;
};

_EXTERNC HGLOBAL _EXPORT GetDib2d( HSP2D hSpace2d,HOBJ2D DibIndex )
{
    PDib2d pd=(PDib2d) GetTool(hSpace2d,DIB2D,DibIndex);
    if (pd)
    {
        pd=(PDib2d)pd->GetMainObject();
        if(pd)return pd->dib;
    }
    return NULL;
}

_EXTERNC BOOL _EXPORT Dib2dSaveAs(HSP2D hSpace2d,HOBJ2D DibIndex,char *FileName){
    BOOL rez=FALSE;
    PDib2d pd=(PDib2d) GetTool(hSpace2d,DIB2D,DibIndex);
    if (pd){

        TDOSStream * ds=new TDOSStream(FileName,TDOSStream::stCreate);
        if (ds->status==0){ pd->_Store(ds);rez=TRUE;}
        delete ds;
    }
    return rez;
};
_EXTERNC BOOL _EXPORT DoubleDib2dSaveAs(HSP2D hSpace2d,HOBJ2D DibIndex,char *FileName){
    BOOL rez=FALSE;
    PDoubleDib2d pd=(PDoubleDib2d) GetTool(hSpace2d,DOUBLEDIB2D,DibIndex);
    if (pd){
        TDOSStream * ds=new TDOSStream(FileName,TDOSStream::stCreate);
        if (ds->status==0){ pd->_Store(ds);rez=TRUE;}
        delete ds;
    }
    return rez;
};

// DoubleDibs
_EXTERNC HOBJ2D _EXPORT CreateDoubleDib2d( HSP2D hSpace2d, HGLOBAL XorDib, HGLOBAL AndDib )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        HOBJ2D h=ps->CreateDoubleDib(XorDib,AndDib);
        if(h)
        {
            if (ps->record)
            {
                ps->NewMetaRecord(mfcreatedoubledib);
                ps->dibs->PutItem(ps->meta,ps->ddibs->GetByHandle(h));
                ps->NewUndoRecord2(mfdeletetool,DOUBLEDIB2D,h);
                ps->CloseMetaRecord();
            }
            SetModify
        }
        return h;
    }
    return 0;
};

_EXTERNC BOOL _EXPORT SetDoubleDib2d( HSP2D hSpace2d, HOBJ2D DoubleDibIndex,
                                      HGLOBAL XorDib, HGLOBAL AndDib )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PDoubleDib2d pd=(PDoubleDib2d)ps->ddibs->GetByHandle(DoubleDibIndex);
        if (pd==NULL)return 0;
        int t=pd->WhoIsIt()==104;
        if (ps->record && t)
        {
            TDoubleDib2d db(XorDib,AndDib);
            db.SetFreeDib(0);
            if (db.dib==NULL||db.andbitmap->dib==NULL)return 0;
            ps->NewMetaRecord1(mfsetdoubledib,DoubleDibIndex);
            db._Store(ps->meta);
            ps->NewUndoRecord1(mfsetdoubledib,DoubleDibIndex);
            pd->_Store(ps->meta);
            ps->CloseMetaRecord();
        }
        int h=ps->SetDoubleDib(DoubleDibIndex,XorDib,AndDib);
        if (h==0 && ps->record && t)_Error(EM_internal);
        if (h && (!t))
        {
            PDoubleDib2d pp=(PDoubleDib2d)pd->GetMainObject();
            if (pp==NULL){_Error(EM_internal);return 0;}
            {
                TDOSStream ds(pp->filename,TDOSStream::stCreate);
                if(!ds.status)pp->_Store(&ds);
            }
            for(INT16 i=0;i<spaces->count;i++)
            {
                PSpace2d _ps=(PSpace2d)spaces->At(i);
                if (_ps && (_ps!=ps))
                    for(int j=0;j<_ps->ddibs->count;j++)
                    {
                        pd=(PDoubleDib2d)_ps->ddibs->At(j);
                        if (pd->GetMainObject()==pp)_ps->ChkDoubleDib(pd);
                    }
            }
        }
        if(h){SetModify}
        return TRUE;
    }
    return 0;
};

_EXTERNC HGLOBAL _EXPORT GetXorDib2d( HSP2D hSpace2d,HOBJ2D DoubleDibIndex )
{
    PDoubleDib2d pd=(PDoubleDib2d) GetTool(hSpace2d,DOUBLEDIB2D,DoubleDibIndex);
    if (pd)
    {
        PDoubleDib2d _pd=((PDoubleDib2d)pd->GetMainObject());
        if(_pd)
            return _pd->dib;
    }
    return NULL;
};

_EXTERNC HGLOBAL _EXPORT GetAndDib2d( HSP2D hSpace2d,HOBJ2D DoubleDibIndex )
{
    PDoubleDib2d pd=(PDoubleDib2d) GetTool(hSpace2d,DOUBLEDIB2D,DoubleDibIndex);
    if (pd)
    {
        PDoubleDib2d _pd=((PDoubleDib2d)pd->GetMainObject());
        if(_pd)
            return _pd->andbitmap->dib;
    }
    return NULL;
};

// String
_EXTERNC HOBJ2D _EXPORT CreateString2d( HSP2D hSpace2d, LPSTR String )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        HOBJ2D h=ps->CreateString2d(String);
        if(h)
        {
            if (ps->record)
            {
                ps->NewMetaRecord(mfcreatestring);
                ps->meta->WriteStr(String);
                ps->NewUndoRecord2(mfdeletetool,6,h);
                ps->CloseMetaRecord();
            }
            SetModify;
        }
        return h;
    }
    return 0;
};

_EXTERNC BOOL _EXPORT SetLogString2d( HSP2D hSpace2d, HOBJ2D index, LPSTR String )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        char * oldstr=NULL;
        if (ps->record )
        {
            PString2d pc=(PString2d)GetTool(hSpace2d,STRING2D,index);
            if (pc)
                oldstr = NewStr(pc->GetString());
            //meta Запоминаем старую строку
        }
        int h = ps->SetLogString2d(index, String);
        if (ps->record)
        {
            if (h)
            {
                ps->NewMetaRecord1(mfsetlogstring,index);
                ps->meta->WriteStr(String);

                ps->NewUndoRecord1(mfsetlogstring,index);
                ps->meta->WriteStr(oldstr);
                ps->CloseMetaRecord();
            }
            if (oldstr) delete oldstr;
        }
        if(h){SetModify}
        return TRUE;
    }
    return FALSE;
};
_EXTERNC INT16 _EXPORT DeleteUnusedTools2d( HSP2D hSpace2d,TOOL_TYPE what)
{PSpace2d ps=GetSpace(hSpace2d);
    if (what>7 || what < 0)return 0;
    PToolsCollection pt =ps ? ps->GetToolsById(what):NULL;
    if ((ps)&&(pt)){
        if (ps->record){
            /*	ps->NewMetaRecord(mfdeleteunusedtools);
                ps->meta->WriteWord(what);*/
            ps->NewMetaRecord1(mfdeleteunusedtools,what);
            ps->NewUndoRecord(mfloadtools);
            ps->meta->WriteWord(what);
            ;}//meta
        int d=ps->DeleteUnusedTool(what,ps->meta);
        if (ps->record){
            ps->meta->WriteWord(d);
            ps->CloseMetaRecord();
            ;}//meta
        if(d){SetModify}
        return d;
    }
    return 0;
}
_EXTERNC BOOL _EXPORT DeleteTool2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D index)
{
    _SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    PToolsCollection pt =GetTools(hSpace2d,what);
    pointer p=pt->GetByHandle(index);
    if((ps) && (pt) && p && (pt->MayDelete(p)))
    {
        /*			if (ps->record)
            {
             ps->NewMetaRecord2(mfdeletetool,what,index);
             ps->NewUndoRecord1(mfcreatetool,what);
             pt->PutItem(ps->meta,pt->GetByHandle(index));
             ps->CloseMetaRecord();
            }
*/
        int h=0;
        if(ps->WhoIsIt()==17458)h=((PSpace2d)ps)->DeleteTool2d(what,index);
        else h=pt->_DeleteObject(index);

        ps->_SetModify();
        return h;
    }
    return 0;
}

_EXTERNC INT16 _EXPORT GetLogString2d( HSP2D hSpace2d,HOBJ2D index, LPSTR String,INT16 MaxLen )
{PString2d ps=(PString2d)GetTool(hSpace2d,STRING2D,index);
    if (ps)
    {if (String){
            if (lstrcpyn(String,ps->string,MaxLen));
            return lstrlen(String);
        }
        return lstrlen(ps->string);
    }
    return 0;
};

_EXTERNC HOBJ2D _EXPORT CreateFont2d( HSP2D hSpace2d, LOGFONT2D * LogFont)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps && LogFont->lfSize == sizeof(LOGFONT2D))
    {
        int h = ps->CreateFont2d(LogFont);
        if (h)
        {
            if (ps->record)
            {
                ps->NewMetaRecord(mfcreatefont);
                ps->meta->Write(LogFont,sizeof(LOGFONT2D));
                ps->NewUndoRecord2(mfdeletetool,5,h);
                ps->CloseMetaRecord();
            }
            SetModify
        }
        return h;
    }
    return 0;
};

_EXTERNC BOOL _EXPORT SetLogFont2d( HSP2D hSpace2d,HOBJ2D index, LOGFONT2D * LogFont )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if(ps && LogFont->lfSize==sizeof(LOGFONT2D))
    {
        LOGFONT2D lf;
        PFont2d pf=(PFont2d)GetTool(hSpace2d,FONT2D,index);
        if (pf)
        {
            if (ps->record)
            {lf = pf->font2d;}
            if(ps->SetLogFont2d(index,LogFont))
            {
                if (ps->record)
                {
                    ps->NewMetaRecord1(mfsetlogfont,index);
                    ps->meta->Write(LogFont,sizeof(LOGFONT2D));
                    ps->NewUndoRecord1(mfsetlogfont,index);
                    ps->meta->Write(&lf,sizeof(LOGFONT2D));
                    ps->CloseMetaRecord();
                    //meta
                }
                SetModify
                        return index;
            }
        }
    }
    return 0;
};

_EXTERNC BOOL _EXPORT GetLogFont2d( HSP2D hSpace2d,HOBJ2D index, LOGFONT2D * Logfont )
{
    PFont2d pf=(PFont2d)GetTool(hSpace2d,FONT2D,index);
    if((pf)&&(Logfont))
    {
        *Logfont =pf->font2d;
        return TRUE;
    }
    return 0;
};


//text
_EXTERNC BOOL _EXPORT PaintText2d(HSP2D hSpace2d,HDC hdc,RECT upd,HOBJ2D htext,int angle)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PText2d pt=(PText2d)ps->texts->GetByHandle(htext);
        if (pt)
        {
            POINT2D p,s;p.x=0;p.y=0;s.x=16384;s.y=16384;
            TText_2d pt2=TText_2d(p,s,pt);
            pt2.Delta=p;
            pt2.angle=angle;
            POINT2D dv=ps->scale_div,ml=ps->scale_mul,or=ps->org;
            ps->org=p;ps->scale_div.x=1;ps->scale_div.y=1;
            ps->scale_mul=ps->scale_div;
            POINT vorg;
            GetViewportOrgEx(hdc,&vorg);
            ps->membitmaporg.x = - vorg.x;
            ps->membitmaporg.y = - vorg.y;
            ps->_DrawText(hdc,&pt2,upd);
            ps->scale_div=dv;ps->scale_mul=ml;ps->org=or;
            return 1;
        }
    }
    return 0;
};

_EXTERNC BOOL _EXPORT GetTextSize2d(HSP2D hSpace2d,HOBJ2D htext,POINT2D*size)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        PText2d pt=(PText2d)ps->texts->GetByHandle(htext);
        if (pt)
        {
            POINT2D p;
            p.x=0;p.y=0;
            TText_2d pt2(p,p,pt);
            pt2.Delta=p;
            pt2.angle=0;
            *size=ps->CalcTextSize(&pt2);
            return TRUE;
        }
    }
    return FALSE;
};

_EXTERNC HOBJ2D _EXPORT CreateText2d( HSP2D hSpace2d, LOGTEXT * Text,INT16 NumItems )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if(ps)
    {
        int h=ps->CreateText(Text, NumItems);
        if(h)
        {
            if (ps->record)
            {
                ps->NewMetaRecord1(mfcreatetext,NumItems);
                ps->meta->Write(Text,sizeof(LOGTEXT)*NumItems);
                ps->NewUndoRecord2(mfdeletetool,7,h);
                ps->CloseMetaRecord();
            }
            SetModify
        }
        return h;
    }
    return 0;
};

_EXTERNC BOOL _EXPORT SetLogText2d( HSP2D hSpace2d,HOBJ2D index, LOGTEXT * Text,INT16 NumItems )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        int r = ps->SetLogText2d(index,Text,NumItems,ps->record);
        if(r){SetModify}
        return r;
    }
    return 0;
};

_EXTERNC INT16 _EXPORT GetTextNumItems2d( HSP2D hSpace2d,HOBJ2D index )
{
    PText2d pt=(PText2d)GetTool(hSpace2d,TEXT2D,index);
    if (pt)
        return pt->text->count;
    return 0;
};

_EXTERNC int _EXPORT GetTextObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{
    PText_2d pb=(PText_2d)_GetObject2d(hSpace2d,Object2d,23);
    if (pb)
    {
        return pb->text->_HANDLE;
    }return 0;
};

_EXTERNC HOBJ2D _EXPORT GetLogText2d(HSP2D hSpace2d,HOBJ2D index,LOGTEXT* Text,INT16 MaxItems)
{
    PText2d pt=(PText2d)GetTool(hSpace2d,TEXT2D,index);
    if(pt)
    {
        int i,j=min(MaxItems,pt->text->count);
        for(i=0;i<j;i++)
        {
            ((PTextRecord)(pt->text->At(i)))->GetLogText(Text+i);
        }
        return j;
    }
    return 0;
};

_OBJECT* TStream::AllocateObject(CHUNK&chunk)
{
    _OBJECT*obj=NULL;
    if ((chunk.code>999) && (chunk.code<1200))
    {
        PCollection pc=NULL;
        switch(chunk.code)
        {
            case 1000:pc=(PCollection)new TCollection(this);break;
            case 1004:pc=(PCollection)new TPenCollection(this);break;
            case 1005:pc=(PCollection)new TBrushCollection(this);break;
            case 1006:pc=(PCollection)new TDibCollection(this);break;
            case 1007:pc=(PCollection)new TDoubleDibCollection(this);break;
            case 1008:pc=(PCollection)new TFontCollection(this);break;
            case 1009:pc=(PCollection)new TStringCollection(this);break;
            case 1010:pc=(PCollection)new TTextCollection(this);break;
            case 1011:pc=(PCollection)new TLOGTextCollection(this);break;
            case 1012:pc=(PCollection)new T3dCollection(this);break;
            case 1020:pc=(PCollection)new TObjectCollection(this);break;
            case 1021:pc=(PCollection)new TPrimaryCollection(this);break;
            case otDATAITEMS:pc=(PCollection)new TDataItems(this);break;
            case 1023:break;
            case otMATERIALS3D:pc=(PCollection)new TMaterialls3d(this);break;
        }
        if (pc)
        {
            pc->LoadItems(this);
            obj = pc;
        }
        else
            return FALSE;
    }
    else
        switch(chunk.code)
        {
            case     1:obj=(_OBJECT*)new TGroup(this,chunk._pos);            break;
            case     3:obj=(_OBJECT*)new TGroup2d(this,chunk._pos);          break;
            case     4:obj=(_OBJECT*)new TRGroup2d(this,chunk._pos);         break;
            case     5:obj=(_OBJECT*)new TGroup3d(this,chunk._pos);          break;
            case    10:obj=(_OBJECT*)new TObject3d(this,chunk._pos);         break;
            case    11:obj=(_OBJECT*)new TCamera3d(this,chunk._pos);         break;
            case    12:obj=(_OBJECT*)new TLight3d(this,chunk._pos);          break;
            case    20:obj=(_OBJECT*)new TLine_2d(this,chunk._pos);          break;
            case    21:obj=(_OBJECT*)new TBitmap_2d(this,chunk._pos);        break;
            case    22:obj=(_OBJECT*)new TDoubleBitmap_2d(this,chunk._pos);  break;
            case    23:obj=(_OBJECT*)new TText_2d(this,chunk._pos);          break;
            case    24:obj=(_OBJECT*)new TView3d_2d(this,chunk._pos);        break;
            case    25:obj=(_OBJECT*)new TUserObject2d(this,chunk._pos);     break;
            case    26:obj=(_OBJECT*)new TControl2d(this,chunk._pos);        break;

            case    50:obj=(_OBJECT*)new TEditFrame(this,chunk._pos);        break;
            case    51:obj=(_OBJECT*)new TRotateCenter(this,chunk._pos);     break;

            case   101:obj=(_OBJECT*)new TPen2d(this,chunk._pos);            break;
            case   102:obj=(_OBJECT*)new TBrush2d(this,chunk._pos);          break;
            case   103:obj=(_OBJECT*)new TDib2d(this,chunk._pos);            break;
            case   104:obj=(_OBJECT*)new TDoubleDib2d(this,chunk._pos);      break;
            case   105:obj=(_OBJECT*)new TFont2d(this,chunk._pos);           break;
            case   106:obj=(_OBJECT*)new TString2d(this,chunk._pos);         break;
            case   107:obj=(_OBJECT*)new TText2d(this,chunk._pos);           break;

            case   110:obj=(_OBJECT*)new TRefToDib2d(this,chunk._pos);       break;
            case   111:obj=(_OBJECT*)new TRefToDoubleDib2d(this,chunk._pos); break;

            case   112:obj=(_OBJECT*)new TMaterial3d(this,chunk._pos);       break;
            case   200:obj=(_OBJECT*)new TPNGDib2d(this,chunk._pos);       break;
            case 17458:obj=(_OBJECT*)new TSpace2d(this);                     break;
            case 17459:obj=(_OBJECT*)new TSpace3d(this,chunk._pos);          break;
        }

    if(obj && fileversion>=0x300)
    {
        while(chunk._pos>GetPos() && (status==0))
        {
            CHUNK _chunk;
            _chunk.InitRead(chunk.st);
            obj->ReadChunk(_chunk);
            _chunk.Check();
        }
    }
    return obj;
};
//*-----------------------------------------------------------------*

_EXTERNC HSP2D _EXPORT CreateSpace2d(HWND hwnd,LPSTR path)
{
    return InsertSpace(new TSpace2d(hwnd, path));
}

_EXTERNC HSP2D _EXPORT LoadSpace2d(HWND hwnd, LPSTR FileNameState2d, LPSTR path,LPSTR texturePath )
{
    TDOSStream stream(FileNameState2d,TDOSStream::stOpenRead);

    TSpace2d::currentpath=path;
    TSpace2d::texturepath=texturePath;
    HCURSOR hc=SetCursor(LoadCursor(0,IDC_WAIT));
    if(stream.ReadWord()==17458)
    {
        PSpace2d ps=new TSpace2d(&stream);
        if (ps)
        {
            if (ps->all && ps->primary)
            {
                ps->hwnd=hwnd;
                ps->filename=NewStr(FileNameState2d);
                SetCursor(hc);
                return InsertSpace(ps);
            }
            delete ps;
        }
    }
    SetCursor(hc);
    return 0;
}

_EXTERNC HSP2D _EXPORT LoadFromMemory2d( HWND hwnd,HGLOBAL block,LPSTR path,LPSTR texturePath)
{
    TMemoryStream s(block);
    TSpace2d::currentpath = path;
    TSpace2d::texturepath = texturePath;

    if(s.ReadWord()==17458)
    {
        PSpace2d ps = new TSpace2d(&s);

        if (ps->all && ps->primary)
        {
            ps->hwnd=hwnd;
            ps->Invalidate();
            return InsertSpace(ps);
        }
        delete ps;
    }
    return 0;
}

_EXTERNC BOOL _EXPORT GetActualSize2d(HSP2D hSpace2d,HOBJ2D Object2d,POINT2D * size)
{
    PObject2d po=_GetObject2d(hSpace2d,Object2d,0);
    PSpace2d ps=GetSpace(hSpace2d);
    if(po)
    {
        switch (po->WhoIsIt())
        {
            case 21:
            {
                PDib2d dib=(PDib2d)((PBitmap_2d)po)->dib->GetMainObject();
                size->x=dib->W;
                size->y=dib->H;
                return 2;
            }
            case 22:
            {
                PDoubleDib2d dib=(PDoubleDib2d)((PDoubleBitmap_2d)po)->dib->GetMainObject();
                size->x=dib->W;
                size->y=dib->H;
                return 2;
            }
            case 23:
            {
                *size = ps->CalcTextSize((PText_2d)po);
                return 2;
            }
            case 25:
            {
                TUserObject2d*uo=(TUserObject2d*)po;
                if(uo->type->objproc(0,&(uo->data),OM_ACTUALSIZE,(DWORD)size))
                    return 2;
            }// none break;
            default:
            {
                *size=po->GetSize2d();
                return 1;
            }
        }
    }
    return FALSE;
}

_EXTERNC HWND _EXPORT AssignWindowToSpace2d(HSP2D hSpace2d,HWND hwnd)
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
    {
        return ps->SetHWND(hwnd);
    }
    return (HWND) - 1;
}

_EXTERNC HWND _EXPORT GetSpaceWindow2d(HSP2D hSpace2d)
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
    {
        return ps->hwnd;
    }
    return NULL;
}

_EXTERNC char * _EXPORT GetSpaceFile2d(HSP2D hSpace2d)
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->filename;
    return 0;
}
_EXTERNC BOOL _EXPORT SetSpaceFile2d(HSP2D hSpace2d,char *filename){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        if(ps->filename){delete ps->filename;ps->filename=NULL;}
        ps->filename=NewStr(filename);
        return TRUE;
    }
    return FALSE;
}

_EXTERNC BOOL _EXPORT DPtoLP2d( HSP2D hSpace2d ,POINT2D FAR* lppt,INT16 cPoints)
{
    PSpace2d ps = GetSpace(hSpace2d);

    if (ps)
    {
        ps->_DpToLp(lppt,cPoints);
        ps->LPtoCRD(lppt,cPoints);
        return TRUE;
    }
    return 0;
}
_EXTERNC BOOL _EXPORT LPtoDP2d( HSP2D hSpace2d ,POINT2D FAR* lppt,INT16 cPoints)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps) {ps->_LpToDp(lppt,cPoints);
        ps->CRDtoLP(lppt,cPoints);
        return TRUE;
    }
    return 0;
};

BOOL SaveState(PSpace2d ps,PStream st)
{
    st->WriteWord(otSPACE2D);
    ps->Store(st);
}

_EXTERNC BOOL _EXPORT State2dSaveAs( HSP2D hSpace2d,LPSTR name)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if(ps)
    {
        if (ps->filename) delete ps->filename;
        ps->filename=NewStr(name);
        HCURSOR hc=SetCursor(LoadCursor(0,IDC_WAIT));
        TDOSStream s(name,TDOSStream::stCreate);
        if (s.status==0)
        {
            //s.(ps);
            SaveState(ps,&s);
            SetCursor(hc);
            if (s.status==0){ps->modifyed=0; return 1;}
        }
    }
    return 0;
};

_EXTERNC HGLOBAL _EXPORT SaveToMemory2d( HSP2D hSpace2d,INT32 * size)
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {	  TMemoryStream s(FALSE);
        if (s.status==0){
            //s.Put(ps);
            SaveState(ps,&s);
            if (s.status==0){
                s.Truncate();
                s.GetHANDLE();
                *size=s.GetSize();
                return s.GetHANDLE();
            }
        }
    }
    return 0;
};

_EXTERNC BOOL _EXPORT State2dSave( HSP2D hSpace2d)
{ PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->filename)){
        char s[256];
        lstrcpy(s,ps->filename);
        return State2dSaveAs(hSpace2d,s);

    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT DeleteSpace2d( HSP2D hSpace2d ){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        if(ps->WhoIsIt()==17459){
            if(  ((TSpace3d*)ps)->views->count>0)return FALSE;
        }
        delete ps;
        spaces->AtPut(((UINT16)hSpace2d)-1,NULL);
        return TRUE;
    }
    return FALSE;
}
//*---------------------------------------------------------*
void _CreatePalette(){
    palette_256=0;
    if (GetPrivateInt("color", "palette",1)==0)return;
    HDC _dc=GetDC(0);
    if ((GetDeviceCaps(_dc, RASTERCAPS) & RC_PALETTE) != 0) {
        int NUMENTRIES=GetDeviceCaps(_dc, SIZEPALETTE);
        LOGPALETTE FAR* plgpl;
        int plsize=sizeof(LOGPALETTE) + NUMENTRIES * sizeof(PALETTEENTRY);
        plgpl = (LOGPALETTE*) new BYTE[plsize];

        plgpl->palNumEntries = NUMENTRIES;
        plgpl->palVersion = 0x300;
        int cn=5;//NUMENTRIES/3;
        int ir,ig,ib,i;
        i=0;
        GetSystemPaletteEntries(_dc, 0,NUMENTRIES,plgpl->palPalEntry );
        i=GetDeviceCaps(_dc,NUMRESERVED)/2;
        int j,b;
        for(ir=0;ir<=cn;ir++)
            for(ig=0;ig<=cn;ig++)
                for(ib=0;ib<=cn;ib++)
                {
                    plgpl->palPalEntry[i].peRed = LOBYTE((ir*255)/cn);
                    plgpl->palPalEntry[i].peGreen = LOBYTE((ig*255)/cn);
                    plgpl->palPalEntry[i].peBlue = LOBYTE((ib*255)/cn);
                    plgpl->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
                    b=0;j=0;
                    while ((j<256)&&(!b)){
                        b=(
                                    plgpl->palPalEntry[i].peRed ==plgpl->palPalEntry[j].peRed &&
                                    plgpl->palPalEntry[i].peGreen ==plgpl->palPalEntry[j].peGreen &&
                                    plgpl->palPalEntry[i].peBlue ==plgpl->palPalEntry[j].peBlue
                                    );
                        j++;if(j==10)j=246;
                    }
                    if (!b)i++;
                }
        int _gray=(246-i)+2;
        for(j=1;j<_gray-1;j++){
            plgpl->palPalEntry[i].peRed = LOBYTE((j*255)/_gray);
            plgpl->palPalEntry[i].peGreen = LOBYTE((j*255)/_gray);
            plgpl->palPalEntry[i].peBlue = LOBYTE((j*255)/_gray);
            plgpl->palPalEntry[i].peFlags = PC_RESERVED;
            i++;
        }
        palette_256 = CreatePalette(plgpl);
        delete plgpl;
    }
    ReleaseDC(0,_dc);
}

LRESULT CALLBACK _export
Gr_WndProc( HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
        case WM_CREATE:return 0;
            /*
    case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
 }*/
    }
    return DefWindowProc(hwnd,message,wParam,lParam);
};
char _graphics_ini[256];

void InitSystem()
{
#ifdef WIN32
    char *s;
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char Filename[MAXFILE];
    char ext[MAXEXT];
    int flags;
    GetModuleFileName(hInstance,Filename,sizeof(Filename));
    flags=fnsplit(Filename,drive,dir,file,ext); 	wsprintf(_graphics_ini,"%s%sgraphics.ini",drive,dir);
#else
    wsprintf(_graphics_ini,"graphics.ini");
#endif

    spaces=new TCollection(10,10);
    palette_256=0;
    _CreatePalette();
    CFormat_2D=RegisterClipboardFormat(CLIP_2D);
    CFormat_3D=RegisterClipboardFormat(CLIP_3D);
    showtime3d=GetPrivateInt("render", "showtime",0);
};

void Done3dSystem();
void DoneLibs2d();
void DoneSystem(){
    C_TYPE i;
    if(axis_bmp)::DeleteObject(axis_bmp);
    if(gray_brush)::DeleteObject(gray_brush);
    if(gray_bmp)::DeleteObject(gray_bmp);
    if(cross_brush)::DeleteObject(cross_brush);
    if(cross_bmp)::DeleteObject(cross_bmp);
    if(noimage_bmp)::DeleteObject(noimage_bmp);
    if (spaces){
        for (i=0;i<spaces->count;i++)
        {if (spaces->At(i)){DeleteSpace2d((HSP2D)(i+1));spaces->AtPut(i,0);}}
        //while (spaces->count){DeleteSpace2d(1);spaces->AtDelete(0);}
        delete spaces;
        if (palette_256) DeleteObject(palette_256);
        DeleteCollection (TSpace2d::_dibs);
        DeleteCollection (TSpace2d::_ddibs);
        spaces=0;
    }

    // DestroyWindow(GR_WINDOW);
    Done3dSystem();

    if(usertypes){
        for(i=0;i<usertypes->count;i++){
            USERTYPESTRUCT * ut=(USERTYPESTRUCT *)usertypes->At(i);
            if (ut->name)delete ut->name;
            delete ut;
        }
        usertypes->DeleteAll();
        delete usertypes;
        usertypes=NULL;
    }
    DoneLibs2d();
};
_EXTERNC void _EXPORT DoneGraphics2d()
{DoneSystem();
};
_EXTERNC HSP2D _EXPORT GetNextSpace2d(HSP2D hsp)
{UINT16 i=0;
    if (hsp){
        while (i<spaces->count && (i+1!=(UINT16)hsp))i++;
        i++;
    }
    _SPACE *sp;
    while ((i<spaces->count) && (((sp=(_SPACE*)spaces->At(i))==NULL) ||
                                 (sp && sp->WhoIsIt()==17459)))i++;
    if (i>=spaces->count)return 0;
    return (HSP2D)(i+1);
};
//*---------------------------------------------------------*
_EXTERNC UINT16 _EXPORT GetObjectType2d3d(HSPACE hSpace,HOBJ2D Object )
{
    PObject po = _GetObject((HSP2D)hSpace,Object);
    if (po)
        return po->WhoIsIt();
    return 0;
}

_EXTERNC BOOL _EXPORT PushCrdSystem(HSPACE h){
    _SPACE*ps = __GetSpace(h);
    if(ps){
        return ps->PushCrd();
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT PopCrdSystem(HSPACE h){
    _SPACE*ps = __GetSpace(h);
    if(ps){
        return ps->PopCrd();
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT SetCrdSystem2d(HSP2D hSpace2d,CRDSYSTEM2D*crd){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {//ps->_SetModify();
        return ps->SetCrd(crd);
    }
    return FALSE;
};
_EXTERNC BOOL _EXPORT GetCrdSystem2d(HSP2D hSpace2d,CRDSYSTEM2D*crd){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        if(ps->crd_used){
            memcpy(crd,&(ps->crd_system),sizeof(CRDSYSTEM2D));
        }else{
            memset(crd,0,sizeof(CRDSYSTEM2D));
            crd->type=MMTEXT_COORDINATE2D;
        }
        return TRUE;
    }return FALSE;
};
//*---------------------------------------------------------*
_EXTERNC BOOL _EXPORT SetOrgSpace2d( HSP2D hSpace2d, POINT2D *Org2d )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        ps->org.x = floor(Org2d->x);
        ps->org.y = floor(Org2d->y);

        for(C_TYPE i = 0; i < ps->primary->count; i++)
        {
            TControl2d* c = (TControl2d*)(ps->primary->At(i));
            if(c->WhoIsIt() == 26)
            {
                c->UpdateSize(0, 0);
                ps->Invalidate();
            }
        }
        return TRUE;
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT GetOrgSpace2d( HSP2D hSpace2d ,POINT2D * org)
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
    {
        *org= ps->org;
        return TRUE;
    }
    else
    {
        org->y = org->x = 0;
    }

    return FALSE;
};

_EXTERNC BOOL _EXPORT SetScaleSpace2d( HSP2D hSpace2d, POINT2D *_MUL,POINT2D *_DIV )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        POINT2D _mul=ps->scale_mul,_div=ps->scale_div;
        if(ps->SetScale2d(*_MUL,*_DIV))
        {
            if (ps->record)
            {
                ps->NewMetaRecord(mfsetscale2d);
                ps->meta->WritePOINT(_MUL);
                ps->meta->WritePOINT(_DIV);
                ps->NewUndoRecord(mfsetscale2d);
                ps->meta->WritePOINT(&_mul);
                ps->meta->WritePOINT(&_div);
                ps->CloseMetaRecord();
            }
            SetModify
                    return 1;
        }
    }return 0;
};

_EXTERNC BOOL _EXPORT SetSpaceRenderEngine2d(HSP2D hSpace2d,INT32 id)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if(ps)
    {
        ps->RenderEngine=id;
        ps->Invalidate();
        SetModify
                return TRUE;
    }return FALSE;
};

_EXTERNC BOOL _EXPORT SetBkBrush2d( HSP2D hSpace2d,HOBJ2D h)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        DWORD s=ps->SetBkBrush(h);
        if ((s)&&(ps->record))
        {/*ps->NewMetaRecord(mfsetbkbrush);
                      ps->meta->WriteWord(h);*/
            ps->NewMetaRecord1(mfsetbkbrush,h);
            /*  ps->NewUndoRecord(mfsetbkbrush);
                      ps->meta->WriteWord(LOWORD(s));*/
            ps->NewUndoRecord1(mfsetbkbrush,LOWORD(s));
            ps->CloseMetaRecord();
        }
        SetModify
                return TRUE;
    }return FALSE;
};
_EXTERNC HOBJ2D _EXPORT GetBkBrush2d( HSP2D hSpace2d)
{ PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->BkBrush))
        return ps->BkBrush->_HANDLE;
    return 0;
}
_EXTERNC BOOL _EXPORT GetScaleSpace2d( HSP2D hSpace2d, POINT2D * _MUL,POINT2D *_DIV  )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        *_MUL=ps->scale_mul;
        *_DIV=ps->scale_div;
        return TRUE;
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT GetUpdRect2d( HSP2D hSpace2d, RECT * UpdRect )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        if (ps->ischanged) *UpdRect =ps->updaterect;
        else { UpdRect->left=0 ; UpdRect->right=0;
            UpdRect->top=0  ; UpdRect->bottom=0;	 }
        return 1;}  else return 0;
};
_EXTERNC BOOL _EXPORT GetUpdRgn2d( HSP2D hSpace2d, HRGN & hUpdRgn )
{ PSpace2d ps=GetSpace(hSpace2d);hUpdRgn=0;
    if (ps)return 1;else return 0;
};
_EXTERNC HPALETTE _EXPORT GetColorPalette2d( HSP2D hSpace2d )
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->palette;else return 0;
}

_EXTERNC HPALETTE _EXPORT  SetColorPalette2d( HSP2D hSpace2d ,HPALETTE hp){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        HPALETTE hp1=ps->palette;
        ps->palette=hp;
        return hp1;
    }
    else return 0;
};

void PaintBmpProc(long org,long ul,long ur,long W,int&dx,int&cx){

    // ul  | UpdRect->left
    // ur  | UpdRect->right
    // W   | ps->BkBrush.W
    // org | ps->org
    //   int _dx=(org<0)?(ul-org):(ul+org);
    int _c1=(ul+org+40000l)/W;
    dx=(int)(_c1*W-(ul+org+40000l));
    int sx=(ur-ul)-dx;
    cx =sx/W;
    if(sx<cx*W)cx++;cx++;
    // cx-=_c1;

}

_EXTERNC BOOL _EXPORT PaintSpace2d( HSP2D hSpace2d, HDC hDc, RECT * UpdRect,WORD flags)
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        if(ps->RenderEngine==1)
        {
            CairoRenderer* cr=new CairoRenderer(ps,hDc,UpdRect,flags);
            cr->RenderSpace2D();
            delete cr;
        }
        else
        {
            ps->wndupdaterect=(*UpdRect);
            int W=UpdRect->right-UpdRect->left,
                    H=UpdRect->bottom-UpdRect->top;

            HGDIOBJ hbmp = (flags & PF_DIRECT) ?
                        0:
                        CreateCompatibleBitmap(hDc,W,H),hbmpold;
            HDC memdc;
            if (hbmp)
            {
                memdc=CreateCompatibleDC(hDc);
                hbmpold=SelectObject(memdc,hbmp);
                ps->membitmaporg.x=UpdRect->left;
                ps->membitmaporg.y=UpdRect->top;
            }
            else
            {
                memdc=hDc;
                ps->membitmaporg.x=0;
                ps->membitmaporg.y=0;
            }

            POINT2D delta = ps->org;
            //ps->_LpToDp(&delta,1);
            if (hbmp)
            {
                INT16 _x=(UpdRect->left +delta.x);
                INT16 _y=(UpdRect->top +delta.y);
                delta.x=-(_x&7);
                delta.y=-(_y&7);
            }
            else
            {
                INT16 _x=(delta.x);
                INT16 _y=(delta.y);
                delta.x=-(_x&7);
                delta.y=-(_y&7);
            }
#ifdef WIN32
            SetStretchBltMode(memdc,COLORONCOLOR/*HALFTONE*/);
            SetBrushOrgEx(memdc,delta.x,delta.y,NULL);
#else
            SetStretchBltMode(memdc,STRETCH_DELETESCANS);
            SetBrushOrg(memdc,delta.x,delta.y);
#endif


            HPALETTE mpal,spal;
            if(palette_256)
            {
                spal=SelectPalette(hDc,palette_256,0);
                RealizePalette(hDc);
                if (hbmp)
                    mpal=SelectPalette(memdc,palette_256,0);
            }


            if (ps->BkBrush)// Рисуем фон
            {
                PDib2d dib = ps->BkBrush->dib ?
                            (PDib2d)ps->BkBrush->dib->GetMainObject() :
                            NULL;

                if ((dib) && (dib->H != 8 || dib->W != 8))
                {
                    if(!(dib->bitmap))
                    {
                        dib->imagesize.x=dib->W;
                        dib->imagesize.y=dib->H;
                        ps->DIBtoBITMAP(dib,hDc);
                    }
                    if(dib->bitmap)
                    {
                        int dx,dy,cx,cy;
                        PaintBmpProc(ps->org.x,UpdRect->left,UpdRect->right,dib->W,dx,cx);
                        PaintBmpProc(ps->org.y,UpdRect->top,UpdRect->bottom,dib->H,dy,cy);
                        HDC mdc=CreateCompatibleDC(hDc);
                        HGDIOBJ hbmpold=SelectObject(mdc,dib->bitmap);

                        for(int i=0;i<cy;i++)
                        {
                            int y=dib->H*i+dy;
                            for(int j=0;j<cx;j++)
                            {
                                int x=dib->W*j+dx;
                                if (dib->bitmap)
                                {
                                    BitBlt(memdc,x,y,dib->W,dib->H,mdc,0,0,SRCCOPY);
                                    //  TextOut(memdc,x,y,"Test",4);
                                }
                            }
                        }
                        SelectObject(mdc,hbmpold);
                        DeleteDC(mdc);
                    }
                }
                else
                {
                    HGDIOBJ  b=ps->_CreateBrush(ps->BkBrush);
                    HGDIOBJ  bo=SelectObject(memdc,b);
                    if (hbmp)
                        PatBlt(memdc,0,0,W,H,PATCOPY);
                    else
                        PatBlt(hDc,UpdRect->left,UpdRect->top,W,H,PATCOPY);
                    SelectObject(memdc,bo);
                    DeleteObject(b);
                }
            }
            else
            {
                if (!(ps->State&SF_GRID))
                {
                    if (hbmp)
                        PatBlt(memdc,0,0,W,H,WHITENESS);
                    else
                        PatBlt(memdc,UpdRect->left,UpdRect->top,W,H,WHITENESS);
                }
            }//end фон



            if (ps->State & SF_GRID)// Рисуем сетку
            {
                HGDIOBJ b=GetCrossBrush();
                HGDIOBJ  bo=SelectObject(memdc,b);
                DWORD fdwRop;
                COLORREF oldt,oldbk;
                if (ps->BkBrush)
                {
                    fdwRop=PATINVERT;
                    oldt=SetTextColor(memdc,RGB(51,51,51));
                    oldbk=SetBkColor(memdc,RGB(0,0,0));
                }
                else
                {
                    fdwRop=PATCOPY;
                    oldt=SetTextColor(memdc,RGB(204,204,204));
                    oldbk=SetBkColor(memdc,RGB(255,255,255));
                }
                UnrealizeObject(b);
                //SetBrushOrgEx(memdc,delta.x,delta.y,NULL);
                if(hbmp)
                    PatBlt(memdc,0,0,W,H,fdwRop);
                else
                    PatBlt(hDc, UpdRect->left, UpdRect->top, W,H, fdwRop);

                SelectObject(memdc,bo);
                SetTextColor(memdc,oldt);
                SetBkColor(memdc,oldbk);
            }//end сетка

            ps->SetTextMode(memdc);
            RECT r=(*UpdRect);
            ps->_DpToLp((LPPOINT) &r, 2);

            { // Идет проверка на необходимость увеличения области,
                // чтоб не было полос
                BOOL 	b1=ps->scale_mul.x > ps->scale_div.x,
                        b2=ps->scale_mul.y > ps->scale_div.y;
                if (b1 || b2)
                {
                    RECT r1=r;
                    ps->_LpToDp((LPPOINT) &r1, 2);

                    if (r1.bottom<UpdRect->bottom)
                        r.bottom++;

                    if (r1.top>UpdRect->top)
                        r.top--;

                    if (r1.right<UpdRect->right)
                        r.right++;

                    if (r1.left>UpdRect->left)
                        r.left--;

                    if (r.top==r.bottom)
                        r.top--;

                    if (r.left==r.bottom)
                        r.left--;
                }
            }

            TSpace2d::paint_flags=flags;
            /*View*/
            ps->View(memdc,r);
            if (memdc!=hDc)
            {
#ifdef WIN32
                SetViewportOrgEx(memdc,0,0,NULL);
#else
                SetViewportOrg(memdc,0,0);
#endif
                BitBlt(hDc,UpdRect->left,UpdRect->top,W,H,memdc,0,0,SRCCOPY);
            }
            if (palette_256)
            {
                SelectPalette(memdc,mpal,0);
                if (hbmp) SelectPalette(hDc,spal,0);
            }
            if (hbmp)
            {
                SelectObject(memdc,hbmpold);
                DeleteDC(memdc);
                DeleteObject(hbmp);
            }
            return 1;
        }
    }
    return 0;
};

//*---------------------------------------------------------*
_EXTERNC BOOL _EXPORT ObjectToTop2d( HSP2D hSpace2d,HOBJ2D Object2d )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        int z = ps->ObjectToEnd2d(Object2d,1);
        if(z)
        {
            if (ps->record)
            {
                ps->NewMetaRecord1(mfZtotop,Object2d);
                ps->NewUndoRecord2(mfZset,Object2d,z);
                ps->CloseMetaRecord();
            }
            SetModify
                    return TRUE;
        }
    }
    return FALSE;
}

_EXTERNC BOOL _EXPORT ObjectToBottom2d( HSP2D hSpace2d, HOBJ2D Object2d )
{  PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        int z=ps->ObjectToEnd2d(Object2d,0);
        if(z){
            if(ps->record)
            {
                ps->NewMetaRecord1(mfZtobottom,Object2d);
                ps->NewUndoRecord2(mfZset,Object2d,z);
                ps->CloseMetaRecord();
            }SetModify}
        return TRUE;
    }return 0;
}
_EXTERNC BOOL _EXPORT SwapObject2d( HSP2D hSpace2d,HOBJ2D First2d,HOBJ2D Second2d )
{  PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        if (ps->SwapObject(First2d,Second2d))
        {if(ps->record)
            {
                ps->NewMetaRecord2(mfZswap,First2d,Second2d);

                ps->NewUndoRecord2(mfZswap,First2d,Second2d);
                ps->CloseMetaRecord();
            }
            SetModify
                    return TRUE;
        }
    }return FALSE;
}
_EXTERNC BOOL _EXPORT SetZOrder2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D ZOrder )
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        int z=ps->SetZOrder(Object2d,ZOrder);
        if(z){
            if(ps->record)
            {
                ps->NewMetaRecord2(mfZset,Object2d,ZOrder);

                ps->NewUndoRecord2(mfZset,Object2d,z);
                ps->CloseMetaRecord();
            }SetModify}
        return TRUE;
    }return FALSE;
}

_EXTERNC INT16 _EXPORT GetZOrder2d( HSP2D hSpace2d,HOBJ2D Object2d )
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
    {
        return ps->GetZOrder( Object2d );
    }
    return 0;
}
_EXTERNC HOBJ2D _EXPORT GetObjectFromZOrder2d( HSP2D hSpace2d,INT16 ZOrder )
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
        if ( (ZOrder>0) && (ZOrder <= ps->primary->count) )
            return ((PObject)ps->primary->At(ZOrder-1))->_HANDLE;
    return 0;
}
_EXTERNC HOBJ2D _EXPORT GetTopObject2d( HSP2D hSpace2d )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
        if (ps->primary->count)
        {
            int i = (ps->primary->count)-1;
            return ((PObject)ps->primary->At(i))->_HANDLE;
        }
    return 0;
}

_EXTERNC HOBJ2D _EXPORT GetBottomObject2d( HSP2D hSpace2d )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
        if (ps->primary->count)
            return ((PObject)ps->primary->At(0))->_HANDLE;
    return 0;
}

_EXTERNC HOBJ2D _EXPORT GetUpperObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        int i = ps->GetPrimaryOrder(Object2d);
        if ((i > -1) && (i < ps->primary->count - 1))
            return ((PObject)ps->primary->At(i+1))->_HANDLE;
    }
    return 0;
}

_EXTERNC HOBJ2D _EXPORT GetLowerObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{  PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    { int i=ps->GetPrimaryOrder(Object2d);
        if (i>0)
            return ((PObject)ps->primary->At(i-1))->_HANDLE;
    }return 0;
}

//*---------------------------------*
_EXTERNC HSP3D _EXPORT CreateSpace3d( HSP2D hSpace2d)
{  PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        PSpace3d ps3=ps->CreateSpace3d();
        if(ps3){
            HSP3D hsp3d=InsertSpace3d(ps3);
            if (ps->record && hsp3d)
            {ps->NewMetaRecord(mfcreate3dspace);
                ps->NewUndoRecord3d(mfdeletespace3d,ps3->_HANDLE);
                ps->CloseMetaRecord();
            }SetModify
                    return hsp3d;
        }
    }
    return 0;
}
_EXTERNC HOBJ2D _EXPORT SetCurrentObject2d( HSP2D hSpace2d,HOBJ2D Object2d )
{_SPACE *ps=__GetSpace((HSPACE)hSpace2d);
    if ((ps)&&(Object2d<OID_RESERVED))
    {HOBJ2D old=ps->all->current;
        if (ps->SetCurrent(Object2d))
        { return old;	 }
    }
    return 0;
};
_EXTERNC HOBJ2D _EXPORT GetCurrentObject2d( HSP2D hSpace2d )
{
    _SPACE *ps = __GetSpace((HSPACE)hSpace2d);
    HOBJ2D h=0;
    if (ps)
        h = ps->all->current;
    if (h < 0)
        h=0;

    return h;
};
_EXTERNC BOOL _EXPORT LockUpdChange2d(HSP2D hSpace2d,INT16 lock)
{
    PSpace2d ps = GetSpace(hSpace2d);
    if (ps)
    {
        if (lock)
            ps->Lock();
        else
            ps->UnLock();
        if (ps->record)
        {
            //_notrealized();
        }
        return TRUE;
    }
    return FALSE;
}
_EXTERNC HOBJ2D _EXPORT CreateEditFrame2d( HSP2D hSpace2d,POINT2D* SizerSize ,BYTE f)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->CreateEditFrame2d(*SizerSize ,f);
    return 0;
};
_EXTERNC HOBJ2D _EXPORT CreateEditFrame3d(HSP2D hSpace2d,POINT2D* pSize)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->CreateEditFrame3d(*pSize);
    return 0;
};
_EXTERNC HOBJ2D _EXPORT CreateRotateAxis3d( HSP2D hSpace2d,POINT3D *center, int size)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps) return ps->CreateAxis3d(*center,size);else return 0;
};
_EXTERNC HOBJ2D _EXPORT GetFrameCursor2d(HSP2D hSpace2d,POINT2D *_p)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        POINT2D p=*_p;
        ps->CRDtoLP(&p);
        HOBJ2D c=0;
        if ((ps->Axis3d)||(ps->Frame3d))
        {
            POINT2D pt;
            if(ps->Axis3d)
            {//ps->Axis3d->view->SetXYZab(0);
                pt=ps->Axis3d->view->FromLP2d(p,FALSE);
            }else
            {//ps->Frame3d->view->SetXYZab(0);
                pt=ps->Frame3d->view->FromLP2d(p,FALSE);
            }
            c=ps->GetFrame3dCursor(pt);
            if (c)return c;
        }
        if ((ps->RCenter)&&(ps->RCenter->GetDistToMouse(p)<2)) return OID_RCENTER;
        c=ps->GetFrameCursor(p);
        if (c)c+=(OID_FRAME2d1-1);
        return c;
    }
    return 0;
}
_EXTERNC HOBJ2D _EXPORT CreateRotateCenter2d( HSP2D hSpace2d, POINT2D *center )
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps)return ps->CreateRotateCenter2d(*center);
    return 0;
};
_EXTERNC BOOL _EXPORT SetAttribute2d( HSP2D hSpace2d,HOBJ2D Obj2dIndex,UINT16 Attr,ATTR_MODE mode )
{
    _SPACE*ps;
    PObject po=_GetObject(hSpace2d,Obj2dIndex,&ps);
    if (po)
    {
        int old = po->options;
        po->SetState(Attr,mode,__GetSpace((HSPACE)hSpace2d));
        ps->_SetModify();
        /*
          PSpace2d ps=GetSpace(hSpace2d);
          if (ps->record)
          {
            ps->NewMetaRecord2(mfsetattribute2d,Obj2dIndex,Attr);
            ps->meta->WriteWord(mode);
            ps->NewUndoRecord2(mfsetattribute2d,Obj2dIndex,old);
            ps->meta->WriteWord(ATTRPUT);
            ps->CloseMetaRecord();
          }SetModify*/
        return TRUE;
    }
    return 0;
}

_EXTERNC BOOL _EXPORT UpdateFromFrame2d(HSP2D hSpace2d)
{PSpace2d ps=GetSpace(hSpace2d);
    if ((ps)&&(ps->Frame)&&(ps->all->current>0)
        && ps->Frame->object)
    {PObject2d po=ps->Frame->object;
        double a=po->GetAngle();
        if ((a!=ps->Frame->angle)&&(ps->RCenter))
        {ROTATOR2D rotor;
            rotor.center=ps->RCenter->center;
            rotor.angle=ps->Frame->angle-a;
            ps->Lock();
            RotateObject2d(hSpace2d,po->_HANDLE,&rotor);
            POINT2D psz=ps->Frame->psize;
            int f=ps->Frame->options;
            ps->DeleteEditFrame2d();
            ps->CreateEditFrame2d(psz,f);
            ps->UnLock();
        }else{
            POINT2D _o=ps->Frame->_origin,_s=ps->Frame->_size,_b=ps->Frame->_base;
            POINT2D posize=po->GetSize2d(),poorigin=po->GetOrigin2d();
            BOOL crdused=ps->crd_used;
            ps->crd_used=FALSE;

            if ((posize.x!=_s.x)||
                (posize.y!=_s.y))
                SetObjectSize2d(hSpace2d,po->_HANDLE,&_s);
            if ((poorigin.x!=_o.x)||
                (poorigin.y!=_o.y)){
                RotatePoint(_o,_b,ps->Frame->angle);
                SetObjectOrg2d(hSpace2d,po->_HANDLE,&_o);
            }
            ps->crd_used=crdused;
            return TRUE;
        }
    }
    return FALSE;
}
_EXTERNC UINT16 _EXPORT GetAttribute2d( HSP2D hSpace2d,HOBJ2D Obj2dIndex )
{
    PObject po=_GetObject(hSpace2d,Obj2dIndex);
    if (po) return po->options;return 0;
}

_EXTERNC BOOL _EXPORT SetObjectPTR2d(HSP2D hSpace2d,HOBJ2D Object2d,DWORD ptr)
{
    _SPACE *ps;
    PObject po=_GetObject(hSpace2d,Object2d,&ps);
    if(po)
    {
        po->PTR=ptr;
        ps->_SetModify();
        return  TRUE;
    }
    return 0;
};

_EXTERNC DWORD _EXPORT GetObjectPTR2d(HSP2D hSpace2d,HOBJ2D Object2d)
{ PObject po=_GetObject2d(hSpace2d,Object2d);
    if(po)return po->PTR;
    return 0l;
};
_EXTERNC BOOL _EXPORT SetObjectName2d(HSP2D hSpace2d,HOBJ2D Object2d,char*name)
{ _SPACE *ps;
    PObject po=_GetObject(hSpace2d,Object2d,&ps);
    if(po){
        if (po->name)delete po->name;po->name=NULL;
        if (lstrlen(name))po->name=NewStr(name);
        ps->_SetModify();
        return  TRUE;
    }
    return FALSE;
};

_EXTERNC INT16 _EXPORT GetObjectName2d(HSP2D hSpace2d,HOBJ2D Object2d,char*name,INT16 size)
{
    PObject po = _GetObject(hSpace2d,Object2d);
    if(po)
    {
        if (name==NULL || size==0)
            return lstrlen(po->name);

        if (po->name)
        {
            lstrcpyn(name,po->name,size);
            return  lstrlen(name);
        }
        *name=0;
        return TRUE;
    }
    return 0;
};

HOBJ2D _GetByName(PObject po,char*name)
{
    if (!lstrcmp(po->name, name))
        return po->_HANDLE;

    if (po->IsGroup())
    {
        PGroup pg = (PGroup)po;
        for(C_TYPE i=0;i<pg->items->count;i++)
        {
            PObject _po = (PObject)pg->items->At(i);
            HOBJ2D r = _GetByName(_po,name);
            if (r)
                return r;
        }
    }
    return 0;
};

_EXTERNC HOBJ2D _EXPORT GetObjectByName2d(HSP2D hSpace2d, HOBJ2D groupHandle, char* name)
{
    if (groupHandle)
    {
        PObject groupObject = _GetObject(hSpace2d, groupHandle);
        if (groupObject)
            return _GetByName(groupObject, name);
    }
    else
    {
        _SPACE * ps = __GetSpace((HSPACE)hSpace2d);
        if (ps)
        {
            for(C_TYPE i = 0; i < ps->all->count; i++)
            {
                HOBJ2D objectHandle = _GetByName((PObject)(ps->all->At(i)), name);
                if (objectHandle)
                    return objectHandle;
            }
        }
    }
    return 0;
};

_EXTERNC HOBJ2D _EXPORT GetObjectParent2d(HSP2D hSpace2d,HOBJ2D Object2d)
{ PObject po=_GetObject(hSpace2d,Object2d,0);
    if ((po)&&(po->owner)) return po->owner->_HANDLE;
    return 0;
};
_EXTERNC HOBJ2D _EXPORT GetNextTool2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D htool)
{ PHANDLECollection pt=GetTools(hSpace2d,what);
    if (pt)return pt->GetNextHandle(htool);
    return 0;
};

_EXTERNC HOBJ2D _EXPORT GetNextObject2d( HSP2D hSpace2d,HOBJ2D Object2d)
{
    _SPACE* ps=__GetSpace((HSPACE)hSpace2d);
    if (ps)
        return ps->all->GetNextHandle(Object2d);

    return 0;
}

_EXTERNC HSP3D _EXPORT GetNextSpace3d( HSP2D hSpace2d,HSP3D hSpace3d)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){return (HSP3D)ps->spaces3d->GetNextHandle(LOBYTE(hSpace3d));
        //	if (h) return  h|(hSpace2d << 8);
    }
    return 0;
};

_EXTERNC BOOL _EXPORT BeginDragDrop2d(HSP2D hSpace2d,HOBJ2D obj2d)
{ PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        ps->_dragobj=obj2d;
        GetObjectOrgSize2d(hSpace2d,obj2d,&(ps->_dragobjorg),NULL);
        return TRUE;
    }
    return 0;
};

_EXTERNC BOOL _EXPORT EndDragDrop2d(HSP2D hSpace2d,HOBJ2D obj2d)
{PSpace2d ps=GetSpace(hSpace2d);
    if (ps){
        if ((ps->_dragobj==obj2d)&&(ps->record))
        {
            ps->NewMetaRecord1(mfsetobjectorg2d,obj2d);
            POINT2D p;
            GetObjectOrgSize2d(hSpace2d,obj2d,&p,NULL);
            ps->meta->WritePOINT(&p);
            ps->NewUndoRecord1(mfsetobjectorg2d,obj2d);
            ps->meta->WritePOINT(&(ps->_dragobjorg));
            ps->CloseMetaRecord();
            ps->_dragobj=-1;
            return TRUE;
        };
        ps->_dragobj=-1;
    }return FALSE;
};

_EXTERNC HOBJ2D _EXPORT Create3dProjection2d(HSP3D hSpace3d,
                                             HOBJ2D hCamera,
                                             POINT2D *DstOrg, POINT2D *DstSize  )
{PSpace2d ps2;
    PSpace3d ps3=Get3dSpace(hSpace3d,ps2);
    if ((ps3)&&(ps2)&& (!ps2->Overflow()))
    {HOBJ2D h=ps2->Create3dProjection2d(hSpace3d,*DstOrg,*DstSize,
                                        hCamera);
        if(h){
            if (ps2->record)
            {
                ps2->NewMetaRecord1(mfcreate3dprojection,LOBYTE(hSpace3d));
                ps2->meta->WritePOINT(DstOrg);
                ps2->meta->WritePOINT(DstSize);
                ps2->meta->WriteWord(hCamera);

                ps2->NewUndoRecord1(mfdeleteobject2d,h);
                ps2->CloseMetaRecord();
            }ps2->modifyed=1;}
        return h;
    }
    return 0;
};
//*--------------------------------------------------------------------*
_EXTERNC int _EXPORT __SaveView3dAs( HSP2D /*hSpace2d*/,int /*obj2d*/,LPSTR /*name*/)
{/* PSpace2d ps=GetSpace(hSpace2d);
            if (ps)
             {PView3d_2d pv=(PView3d_2d)ps->GetObjByHANDLE(obj2d);
              if (pv->WhoIsIt()==24){
              TDOSStream s(name,TDOSStream::stCreate);
                 if (s.status==0){
              pv->SetXYZab();
              POINT2D delta=pv->projector->poOffset;
              ps->_LpToDp(&delta,1);
              s.Write(&delta,sizeof(delta));
                pv->space3d->_SaveView(&s);
                if (s.status==0)return 1;
                } }
             }*/
    return 0;
};
_EXTERNC BOOL _EXPORT PointNearLine2d(POINT2D *l1,POINT2D *l2,POINT2D *p){
    return PointNearLine(*l1,*l2,*p);
};

_EXTERNC BOOL _EXPORT GetPOINT2D(HSP2D hSpace2d ,POINT2D FAR* point,LPARAM lParam){
    point->x=(INT16)LOWORD(lParam);
    point->y=(INT16)HIWORD(lParam);
    return DPtoLP2d(hSpace2d,point,1);
};
_EXTERNC BOOL _EXPORT EmptySpace2d(HSP2D hSpace2d){
    PSpace2d ps=GetSpace(hSpace2d);
    if (ps)
    {
        char path[256];
        HWND hwnd;
        lstrcpy(path,ps->path);
        hwnd=ps->hwnd;
        delete ps;
        ps=new TSpace2d(hwnd,path);
        spaces->AtPut(((UINT16)hSpace2d)-1,ps);
        ps->SetHANDLE(hSpace2d);
        ps->Invalidate();
        SetModify;
        return TRUE;
    }
    return FALSE;
};

_EXTERNC BOOL _EXPORT SetObjectData2d(HSP2D hsp,HOBJ2D Object2d,UINT16 id,void * far data,UINT16 start,UINT16 size){
    _SPACE*ps=__GetSpace((HSPACE)hsp);
    if (ps){
        if (ps->SetData(Object2d,id,start,size,data)){
            ps->_SetModify();return TRUE;
        } }
    return FALSE;
}

_EXTERNC UINT16 _EXPORT GetObjectData2d(HSP2D hsp, HOBJ2D Object2d, UINT16 id, void* far data, UINT16 start, UINT16 size)
{
    _SPACE* ps = __GetSpace((HSPACE)hsp);
    if (ps)
    {
        size = ps->GetData(Object2d, id, data, start, size);
        return size;
    }
    return 0;
}

_EXTERNC INT32 _EXPORT IsObjectsIntersect2d(HSPACE hsp,HOBJ2D obj1,HOBJ2D obj2,UINT32 flags){
    _SPACE* ps = __GetSpace(hsp);
    if(ps){
        return ps->IsIntersect(obj1,obj2,flags);
    }
    return 0;
}


_EXTERNC BOOL _EXPORT TrCameraToFeet(HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D Object3d,gr_float m){
    TView3d_2d*pv =(TView3d_2d *)_GetObject2d(hSpace2d,Object2d,24);
    if (pv && pv->items->count){
        long minx,maxx,miny,maxy;
        BOOL set=0;
        for(C_TYPE i=0;i<pv->items->count;i++){
            _OBJECT3D*obj=(_OBJECT3D*)pv->items->At(i);
            if ((Object3d==0)||(obj->object->_HANDLE==Object3d)){
                if (!set){
                    minx=obj->org.x;
                    miny=obj->org.y;
                    maxx=minx+obj->size.x;
                    maxy=miny+obj->size.y;
                    set=TRUE;
                }else{
                    minx=min(obj->org.x,minx);
                    miny=min(obj->org.y,miny);
                    maxx=(obj->org.x+obj->size.x,maxx);
                    maxy=(obj->org.y+obj->size.y,maxy);
                }}
        }
        if (!set)return FALSE;
        POINT2D size;
        size.x=((double)(maxx-minx))/pv->__scale;
        size.y=((double)(maxy-miny))/pv->__scale;


        double ext_x,ext_y,ext;
        //  ext_x=(pv->camera->camera.poExtent.x*(size.x)/(pv->size.x*m));
        //  ext_y=(pv->camera->camera.poExtent.y*(size.y)/(pv->size.y/m));
        ext_x=size.x/m;
        ext_y=size.y/m;
        ext=max(ext_x,ext_y);
        pv->camera->camera.poExtent.x=ext;
        pv->camera->camera.poExtent.y=ext;
        pv->camera->Recalc();
        if (pv->space2d)pv->space2d->_SetModify();
        return TRUE;
    }
    return FALSE;
};

TSpace3d::~TSpace3d(){
    if (views) {
        views->DeleteAll();delete views;views=NULL;
    }

    DeleteCollection(all);all=0;
    if (primary){primary->DeleteAll();DeleteCollection(primary);primary=0;}

    if (materials){
        DeleteCollection(materials);materials=NULL;
    }
    C_TYPE index=spaces->IndexOf(this);
    if (index>-1){
        spaces->AtPut(index,0);
    }
};

_EXTERNC UINT  _EXPORT GetFileType2d(char*s,UINT16*)
{
    TDOSStream st(s, TDOSStream::stOpenRead);
    if (st.status)return 0;
    UINT16 id=st.ReadWord();;
    switch(id)
    {
        case 0x4D4D:return file3d_3DS;
        case 17458:return vect_VDR;
        case otSPACE3Df:
        case 17459:return file_SPACE3D;
        case 0x454d:
            if (st.ReadWord()==0x4154)
                return vect_MFL;
            else
                st.Seek(2);
            break;
    }

    if (id==0x4d42)//BMP format
    {
        st.Seek(14);
        BITMAPINFOHEADER bih;
        st.Read(&bih,sizeof(bih));
        if(bih.biCompression==BI_RLE8 ||
           bih.biCompression==BI_RLE4)
            return raster_RLE;

        long offset,bitsAlloc;
        INT16 bitCount = bih.biBitCount;
        long NumClrs;
        int colorAlloc=0;
        if(bitCount<9)
        {
            if (!bih.biClrUsed)
            {
                bih.biClrUsed = NColors(bitCount);
                NumClrs = bih.biClrUsed;
            }
            else
                NumClrs =bih.biClrUsed;
            //int colorRead = (bih.biClrUsed )* sizeof(RGBQUAD);
            colorAlloc = (INT16)(NumClrs * sizeof(RGBQUAD)); // size of color tables
        }
        int W = (int)bih.biWidth;
        int H = (int)bih.biHeight;
        bitsAlloc = ScanBytes(W, bitCount) * H;
        offset=bitsAlloc+colorAlloc+sizeof(bih)+14l;
        st.Seek(offset);
        WORD id=st.ReadWord();
        if (id==0x4d42)
        { //DOUBLE BMP format
            st.Seek(offset+14l);
            st.Read(&bih,sizeof(bih));
            if (bih.biBitCount==1)
                return raster_DOUBLE;
        }
        return raster_BMP;
    }

    if (id==0xd8ffu){//JPG
        st.Seek(2);
        id =st.ReadWord();
        if (id==0xe0ffu){
            id =st.ReadWord();
            char s[8];st.Read(s,5);s[5]=0;
            if (!lstrcmp(s,"JFIF")){return raster_JPG;}
        }
    }

    if (id==0x4949 || id ==0x4D4D){ //TIFF
        st.Seek(2);
        id =st.ReadWord();
        if(id==42){return raster_TIFF;}
    }
    if (LOBYTE(id)==10){ //PCX
        st.Seek(2);
        id=st.ReadWord();
        if (LOBYTE(id)==1){
            st.Seek(64);
            id=st.ReadWord();
            if (!LOBYTE(id)){return raster_PCX;}
        }}

    {  //GIF & PNG
        char s[8];
        char s_png[8]={137,80,78,71,13,10,26,10};
        st.Seek(0);
        st.Read(s,8);
        if ((s[0]=='G') &&(s[1]=='I') && (s[2]=='F') && (s[3]=='8'))
        {return raster_GIF;}
        if(!memcmp(s,s_png,8)){return raster_PNG;}
    }
    {
        st.Seek(0);
        char s[5];
        st.Read(s,4);
        s[4]=0;
        if (!lstrcmp(s,"RIFF")){
            st.Seek(8);
            st.Read(s,4);
            if (!lstrcmp(s,"AVI "))return video_AVI;
        }
    }
    {
        st.Seek(4);
        char s[5];
        st.Read(s,4);
        s[4]=0;
        if (!lstrcmp(s,"mdat")){
            return video_MOV;
        }
    }

    {
        st.Seek(4);
        WORD w=st.ReadWord();
        if(w==0xaf11u) return video_FLI;
        if(w==0xaf12u) return video_FLC;

    }

    //#define vect_VDR      256
    //#define vect_MFL      257

    {
        st.Seek(0);
        if (st.ReadWord()==1){
            if (st.ReadWord()==9){
                st.ReadWord();
                if (st.ReadLong()==st.GetSize())return vect_WMF;
            }
        }
    }
    { //ICO // WMF
        st.Seek(0);
        DWORD id=st.ReadLong();
        if (id==0x00010000L)return raster_ICO;
        if (id==0x9AC6CDD7L)return vect_WMF;

    }

    {
        st.Seek(0);
        struct {
            BYTE  IDl;
            BYTE  colortype;
            BYTE  imagetype;
            INT16 color1;
            INT16 colorL;
            BYTE  colorsize;

            INT16 x,y;
            INT16 width;
            INT16 height;
            BYTE  bitperpixel;
            BYTE  flags;

        }TGAh;

        st.Read(&TGAh,sizeof(TGAh));
        if((TGAh.colortype==1) || (TGAh.colortype==0)){
            if(TGAh.colortype==1){
                if((TGAh.colorsize==15)||(TGAh.colorsize==16)||(TGAh.colorsize==24)||(TGAh.colorsize==32));else goto m1;
            }
            if(TGAh.colortype==0){
                if(TGAh.color1 || TGAh.colorL || TGAh.colorsize)goto m1;
            }
            switch(TGAh.imagetype){
                case 0:
                case 1:
                case 2:
                case 3:
                case 9:
                case 10:
                case 11:{
                    return raster_TGA;
                }
            }

        }
m1:
    }
    return 0;
};

_EXTERNC HOBJ2D _EXPORT ImportRaster2d(HSP2D hsp,char*filename,UINT type,BOOL *dbl)
{
    PSpace2d ps=GetSpace(hsp);
    if (ps)
    {
        if(!type)
            type = GetFileType2d(filename,NULL);

        if(!type)
            return FALSE;

        struct ICONDIRENTRY
        {
            BYTE  bWidth;
            BYTE  bHeight;
            BYTE  bColorCount;
            BYTE  bReserved;
            WORD  wPlanes;
            WORD  wBitCount;
            DWORD dwBytesInRes;
            DWORD dwImageOffset;
        };

        typedef struct ICONDIR
        {
            WORD          idReserved;
            WORD          idType;
            WORD          idCount;
        } ICONHEADER;

        ICONHEADER hdr;
        if (type == raster_DOUBLE)
        {
            if(dbl)	(*dbl)=TRUE;
            return FCreateDoubleDib2d(hsp,filename);
        }

        if (type==raster_ICO)
        {
            TDOSStream st(filename,TDOSStream::stOpenRead);
            if (st.status)return 0;
            st.Read(&hdr,sizeof(hdr));
            BOOL readfirst=1;
            ICONDIRENTRY entry,entry1;
            while (hdr.idCount)
            {
                st.Read(&entry1,sizeof(entry1));
                if (readfirst)
                {
                    entry=entry1;
                }
                else
                {
                    if((entry1.bColorCount>entry.bColorCount) ||
                       (entry1.bColorCount==0 && entry.bColorCount!=0))
                        entry=entry1;
                    else
                    {
                        if ((entry1.bWidth>entry.bWidth)&&(entry1.bHeight>entry.bHeight))
                            entry=entry1;
                    }
                }
                //if (readfirst || entry1.bColorCount>entry.bColorCount)entry=entry1;
                hdr.idCount--;
                readfirst=0;
            }
            //	if (entry.bColorCount==16)
            {
                HGLOBAL hx,hm;
                {
                    //TDOSStream outx("~icon_$$._$_",TDOSStream::stCreate);
                    TMemoryStream outx;
                    TMemoryStream outm;
                    BITMAPINFOHEADER header;
                    st.Seek(entry.dwImageOffset);
                    st.Read(&header,sizeof(header));
                    BITMAPINFOHEADER mask=header;
                    mask.biHeight/=2;
                    int bc=(int)header.biBitCount;
                    if(bc==0)
                    {
                        switch(entry1.bColorCount)
                        {
                            case 0:bc=24;break;
                            case 1:bc=1;break;
                            case 16:bc=4;break;
                            case 255:bc=8;break;
                        }
                    }
                    int size=(((long)bc)*((long)entry.bHeight)*((long)entry.bWidth)/8);
                    mask.biSizeImage=size;
                    mask.biClrUsed=entry.bColorCount;
                    int cc=(1<<(mask.biBitCount));
                    entry.bColorCount=(BYTE)cc;
                    outx.Write(&mask,sizeof(mask));
                    for(int i=0;i<cc;i++)
                        outx.WriteLong(st.ReadLong());
                    char * buf=new char[size];
                    st.Read(buf,size);
                    outx.Write(buf,size);
                    //mono
                    //fileheader.bfOffBits=2*4+sizeof(fileheader)+sizeof(BITMAPINFOHEADER);
                    //outm.Write(&fileheader,sizeof(fileheader));
                    size=ScanBytes(mask.biWidth,1)*(int)mask.biHeight;
                    mask.biBitCount=1;
                    mask.biClrUsed=2;
                    mask.biSizeImage=size;
                    mask.biClrUsed=2;
                    outm.Write(&mask,sizeof(mask));
                    outm.WriteLong(0);
                    outm.WriteLong(0xfffffful);
                    st.Read(buf,size);
                    outm.Write(buf,size);
                    delete buf;

                    hx=GlobalAlloc(GMEM_MOVEABLE	,outx.GetSize());
                    hm=GlobalAlloc(GMEM_MOVEABLE	,outm.GetSize());

                    void * _p=GlobalLock(hx);
                    void * base=GlobalLock(outx.GetHANDLE());
#ifdef WIN32
                    memcpy(_p,base,outx.GetSize());
#else
                    hmemcpy(_p,base,outx.GetSize());
#endif
                    GlobalUnlock(outx.GetHANDLE());
                    GlobalUnlock(hx);

                    _p=GlobalLock(hm);
                    base=GlobalLock(outm.GetHANDLE());
#ifdef WIN32
                    memcpy(_p,base,outm.GetSize());
#else
                    hmemcpy(_p,base,outm.GetSize());
#endif
                    GlobalUnlock(outm.GetHANDLE());
                    GlobalUnlock(hm);
                }
                HOBJ2D hdib=CreateDoubleDib2d(hsp,hx,hm);
                if(dbl)(*dbl)=TRUE;
                return hdib;
            }
        }
        PDib2d pdib=ImportRasterImage(filename,type,dbl);
        if(pdib)
        {
            HOBJ2D h=0;
            if(*dbl)
            {
                h=ps->ddibs->InsertObject((PDoubleDib2d)pdib);
            }
            else
            {
                h=ps->dibs->InsertObject(pdib);
            }
            if(h){SetModify}
            return h;
        }
    }
    return 0;
};

TDib2d * ImportRasterImage(char*filename,UINT filetype,BOOL *dbl)
{
    if(!filetype)filetype=GetFileType2d(filename,NULL);
    if(!filetype)return FALSE;
    if(dbl)*dbl =FALSE;

    char * name;

    switch(filetype)
    {
        case raster_BMP:
        {
            TDOSStream st(filename, TDOSStream::stOpenRead);
            if (st.status)return 0;
            TDib2d * pd = new TDib2d(0,&st);
            if(!(pd->dib)){delete pd;pd=0;}
            return pd;
        }
        case raster_PNG  :
        {
            PPNGDib2d dib = new TPNGDib2d(filename);
            if(!dib->GetSurface())
            {delete dib;dib=0;}
            return dib;
        }
            //name="PNG";
            break;
        case raster_TGA  :name="TGA";break;
        case raster_JPG  :name="JPG";break;
        case raster_GIF  :name="GIF";break;
        case raster_PCX  :name="PCX";break;
        case raster_LBM  :name="LBM";break;
        case raster_TIFF :name="TIF";break;
        case raster_RLE  :name="RLE";break;
        default:return 0;
    };

    char libname[80];
    lstrcpy(libname,name);
#ifdef WIN32
    lstrcat(libname,"dll32.dll");
#else
    lstrcat(libname,"dll.dll");
#endif
    HINSTANCE lib=LoadLibrary(libname);
#ifdef WIN32
    if (lib)
#else
    if (lib>HINSTANCE_ERROR)
#endif
    {
        TDib2d *Pdib=NULL;
        char temp[9999];lstrcpy(temp,"");

#ifdef WIN32
        HGLOBAL (WINAPI *_LoadFile)(char*,char*);
        BOOL (WINAPI *_SetProgress)(void*);
#else
        HGLOBAL (far _pascal * _LoadFile)(char far*,char far*);
        BOOL (far _pascal *_SetProgress)(void*);
#endif

        (FARPROC)_LoadFile = GetProcAddress(lib,"LOADFILE");
        if (_LoadFile)
        {
            HGLOBAL block=_LoadFile(filename,temp);
            FreeLibrary(lib);
            if (block)
            {
                Pdib = new TDib2d(block);
                if (!(Pdib && Pdib->dib)){GlobalFree(block);
                    delete Pdib;Pdib=NULL;
                }
            }
            else
            {
                if (lstrlen(temp))
                {
                    BOOL ddok=FALSE;
                    {
                        TDOSStream st(temp, TDOSStream::stOpenRead);
                        if (st.status)return 0;
                        if(dbl)
                        {
                            INT16 t= GetFileType2d(temp,0);
                            if(t==raster_DOUBLE)
                            {
                                PDoubleDib2d pd=new TDoubleDib2d(0,&st);
                                ddok=(pd->dib && pd->andbitmap && pd->andbitmap->dib);
                                if(!ddok){delete pd;pd=NULL;}
                                else{
                                    Pdib=(TDib2d*)pd;
                                    if(dbl)*dbl=TRUE;
                                }
                            }
                        }
                        if(!ddok)
                        {
                            Pdib=new TDib2d(0,&st);
                            if(!(Pdib->dib)){
                                delete Pdib;Pdib=NULL;
                            }
                        }
                    }
                    OFSTRUCT ofstruct;
                    OpenFile(temp,&ofstruct,OF_DELETE);
                }
            }
        }
        else _Error(EM_filterloadproc);

        return Pdib;
    }else{
        char temp[260];
        wsprintf(temp,"[ %s ]\nDll import module for [%s] file type not found!\n\n%s - file format for raster images.",libname,name,name);
        MessageBox(0,temp,"Graphic Library",MB_TASKMODAL|MB_ICONHAND);
    }
    return 0;
};
char * TSpace2d::texturepath=NULL;
