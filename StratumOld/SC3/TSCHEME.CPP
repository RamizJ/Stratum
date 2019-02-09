/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            tscheme.cpp
Author:
            Alexander Shelemekhov
*/
#include "_tscheme.h"

BOOL ScGetKeyState(int key)
{
    return (GetAsyncKeyState(key)&0x8000u) != NULL;
}

#ifndef RUNTIME
UINT CF_RTF=0;

const char *axis_str[]={"X-Y-Z","X-Y","Z-X","Z-Y","X","Y","Z"};
extern HINSTANCE hInstance;

UINT CF_2D=RegisterClipboardFormat(CLIP_2D);
UINT clip_3d=RegisterClipboardFormat(CLIP_3D);


BOOL AdjustObject(HSP3D,HOBJ2D){
    return 0;
}

TSchemeWindow::TSchemeWindow(LPCREATESTRUCT l,HWND h):
    TSpaceWindow(l,h)
{
    //int sizeofthis=sizeof(TSchemeWindow);
    BYTE *_this=(BYTE *)this;
    BYTE *__this=_this+sizeof(TSpaceWindow);

    int sizeofthis = (int)(sizeof(TSchemeWindow) - sizeof(TSpaceWindow));
    memset(__this,0,sizeofthis);

    CanUseDelete=1;
    vscroll=new TScroller(this,-1000,1000,1,SB_VERT);
    hscroll=new TScroller(this,-1000,1000,1,SB_HORZ);
    vscroll->singlestep=4;
    hscroll->singlestep=4;
    vscroll->flags|=SF_SCHEME;
    hscroll->flags|=SF_SCHEME;
    scaling=TRUE;
    ShowControlBar(20,FALSE);
    pasteflags=PFC_ALL;

    if(!CF_RTF)
        CF_RTF=RegisterClipboardFormat("Rich Text Format");
};

void TSchemeWindow::DeleteFrames(HSP2D _hsp)
{
    if (!_hsp)
        _hsp=hsp;

    if(_hsp!=hsp && linkframe)
        DeleteLinkFrame(_hsp);

    if(__objects2d)
    {
        HOBJ2D  h= GetObjectByName2d(_hsp,__objects2d,"$@#_frames2d_$@#");
        FullDeleteObject2d(_hsp,h);
        DeleteGroup2d(_hsp,__objects2d);
        if(_hsp ==hsp)
            __objects2d=0;
    }
    if(__objects)
    {
        DeleteGroup2d(_hsp,__objects);
        if(_hsp ==hsp)
            __objects=0;
    }
    if (__frame)
    {
        FullDeleteObject2d(_hsp,__frame);
        if (_hsp==hsp)
        {
            __frame=0;
        }
    }
};

BOOL TSchemeWindow::CanClose()
{
    if(!readonly){
        switch(hardmode)
        {
            case EDITOR:
            {
                if (IsSpaceChanged2d(hsp))
                {
                    char * filename=GetSpaceFile2d(hsp);
                    char s[2560]="";
                    char ss[1280];
                    LoadString(HResource,IDS_MSG13_,ss,sizeof(ss));
                    if (!filename)filename="Noname00.vdr";

                    wsprintf(s,ss,filename);

                    int id= MSGBOX(HWindow,s,_MSG_QSC,MB_DEFBUTTON1|MB_YESNOCANCEL|MB_ICONQUESTION);
                    switch(id)
                    {
                        case IDYES:
                        {
                            SetState(sfbase);
                            return Cm_Save();
                        }return FALSE;

                        case IDCANCEL:
                            return FALSE;
                    }
                }
            }break;

            case IMAGE_EDITOR:
            {
                updateparent=TRUE;
                if (IsSpaceChanged2d(hsp))
                {
                    char s[256];
                    char ss[128];
                    LoadString(HResource,IDS_MSG14_,ss,sizeof(ss));
                    wsprintf(s,ss,_class->name);

                    int id= MSGBOX(HWindow,s,_MSG_QSC,MB_DEFBUTTON1|MB_YESNOCANCEL|MB_ICONQUESTION);
                    switch(id)
                    {
                        case IDNO:
                            updateparent=FALSE;
                            return TRUE;

                        case IDCANCEL:
                            return FALSE;
                    }
                }
            }break;
        }
    }// !readonly
    return TRUE;
}

TSchemeWindow::~TSchemeWindow()

{
#ifdef CAN_EDIT_BITMAP
#endif
    link=0;
    AssignWindowToSpace2d(hsp,0);
    if(addmode3d)DoneAddMode();
    SetState(sfbase);
    DeleteEditFrame2d(hsp);
    if(scheme)
        scheme->window=NULL;
    switch(hardmode)
    {
        case SCHEME_EDITOR:
            CloseClassScheme(_class);
            break;

        case IMAGE_EDITOR:
            CloseClassImage(_class,updateparent);
            break;

        case EDITOR:DeleteSpace2d(hsp);
            break;
    }
    hsp=0;
    if (_path)
        delete _path;
    if (grid)
        delete grid;
    SetStatusBeginText();
};
void TSchemeWindow::UpdateMousePos()
{
    POINT2D p;
    p.x=20;p.y=20;DPtoLP2d(hsp,&p,1);
    _mousepos=p;
};

int TSchemeWindow::IsCommandDisable(int command)
{
    // Могут работать в режиме ТОЛЬКО ЧТЕНИЕ
    switch(command){
        case CM_GOTOSHORTCUT:
            return 0; // Она появляется толкокогда возможно
        case CM_IMGNAMES:
            if(hardmode!=SCHEME_EDITOR)
                return DM_NOTSCHEME;
            return 0;

        case CM_PAGEIMAGE_PROP:
            return 0;

        case CM_COPYBK:
        case CM_SAVEBK:return 0;// Только из Popup Menu
        case CM_SEARCHAGAIN:{
            UINT32 _mode=SCGetScVar("search_what_tofind",0);
            if(_mode==0 && object && (__objects==NULL))return 0;
            if(_mode==1 && link)return 0;
            if(_mode==2 && object2d)return 0;
            return (DM_CANAGAIN);
        }
        case CM_SEARCH:if(GetNextObject2d(hsp,0))return 0;else return DM_NOOBJECTONSCHEME;
    }
    if(state<sfbase3d){
        switch(command){
            case CM_SETVARrect:
                if((__objects||object) && _object)return 0;return DM_NOOBJECT3D;

            case CM_SAVESCHEME:return DM_READONLY;
            case CM_STATEdrag:
            case CM_ZOOM:
            case CM_FILESAVEAS:
            case CM_STATEbase:return 0;
            case CM_HELPONIMAGE:if(object)return 0;return DM_NOOBJECT;
            case CM_EDITTEXTURES:
            case CM_EDITVIEW3D:{
                if (object2d){
                    INT16 t=GetObjectType2d(hsp,object2d);
                    if (t==otVIEW3D2D)return 0;else return DM_NO3DPROJECTION;
                }
            }return DM_NOOBJECT2D;
            case CM_SPACEPROP:return 0;
            case CM_SAVEOBJECT:
            case CM_COPYASBITMAP:
            {
                if (object2d){
                    INT16 t=GetObjectType2d(hsp,object2d);
                    if (t==otBITMAP2D || t==otDOUBLEBITMAP2D)return 0;else return DM_NOBITMAP;
                }
            }return DM_NOOBJECT2D;

            case CM_FILESAVE:{
                if (hardmode==EDITOR)return 0;
                return DM_CANTSAVESCHEME;
            }

            case CM_EDITCOPY:{ if (object || __objects || object2d ||link || __objects2d)return 0;return DM_NOOBJECT;};
            case CM_IMAGE:
            case CM_scTEXTEDIT:
                if(object)
                {
                    INT16 id=_class->GetChildById(object);
                    if (id>-1)
                        return 0;
                }
                return DM_NOIMAGE;
            case CM_SCHEME:
                if(object){
                    INT16 id=_class->GetChildById(object);
                    if (id>-1){
                        TClass *c__class=_class->childs[id].type->GetClass();
                        if (c__class->Protected() && ((c__class->flags&CF_SCHEME)==0)) return DM_PROTECTED;
                        return 0;
                        //return DM_PROTECTED;
                    }
                };return DM_NOIMAGE;
            case CM_OBJECTVARLOAD:
            case CM_OBJECTVARSAVE:
            case CM_OBJECTVARDEF:return ((object && _object || __objects)?0:DM_NOIMAGE);
            case CM_PROPERTIES:
            case CM_OBJECTEDIT:if ( object || object2d || link)return 0;return DM_NOOBJECT;
            case CM_HELPONSCHEME:return 0;
        }
    }else{
        switch(command){
            case CM_STATEbase:return 0;
            case CM_SAVEOBJECTAS:
            case CM_PROPERTIES:if(object3d)return 0;return DM_NOOBJECT3D;
            case CM_EDITCOPY: { if (object3d)return 0;return DM_NOOBJECT3D;};
        }
    }

    if ((command>=CM_VARITEM)&& ((CM_VARITEM+38)>=command)){return 0;}
    if(readonly){
        int rez= TSpaceWindow::IsCommandDisable(command);
        if(DM_NOHANDLER==rez)return DM_READONLY;
        return rez;
    }
    // Дальше НЕ Могут работать в режиме ТОЛЬКО ЧТЕНИЕ

    if (command>=CM_LAYERSstart && CM_LAYERSend>=command){return 0;}
    if ((command>=CM_camera_begin) && (command<CM_camera_end) && (object2d))return 0;

    if(command>=CM_AXIS3Dmode && command<=CM_AXIS3DmodeEnd)
    {
        if(hsp3d)return 0;else return 200;
    };

    // Общие
    switch(command){
        case CM_GETTRANSPARENT:if (object2d){
                if(IsVideoFrame2d(hsp,object2d))return 0;else return DM_NOVIDEOFRAME;
            }return DM_NOOBJECT2D;
        case CM_IMPORTOBJ:return 0;
        case CM_3DNEWDEFCAMERA:
        case CM_3DCLONECAMERA :if (object2d){
                INT16 t=GetObjectType2d(hsp,object2d);
                if (t==otVIEW3D2D)return 0;else return DM_NO3DPROJECTION;
            }return DM_NOOBJECT2D;

    }


    if (state<sfbase3d){
        switch(command){
            case CM_POLYFILLMODEALTERNATE:
            case CM_POLYFILLMODEWINDING  :
            case CM_POLYCOMBINEMODE_AND:
            case CM_POLYCOMBINEMODE_OR :
            case CM_POLYCOMBINEMODE_XOR :
            case CM_POLYCOMBINEMODE_DIFF:return 0;

#ifdef CAN_EDIT_BITMAP
            case CM_EDITBITMAP:
#endif
            {
                if (object2d){
                    INT16 t=GetObjectType2d(hsp,object2d);
                    if (t==otBITMAP2D || t==otDOUBLEBITMAP2D)return 0;else return DM_NOBITMAP;
                }
            }return DM_NOOBJECT2D;
            case CM_ADJUSTIMAJES:
            case CM_CONVERTTOCLASS:if(__objects)return 0;return DM_NOOBJECT2D;
            case CM_STATEview3d:if ( state==sfbase)return 0;return 200;
            case CM_OLE:return DM_NOTREALIZED;
            case CM_REPLACECHILD:if(object || __objects)return 0;return DM_NOOBJECT;

            case CM_STATEwinedit:
            case CM_STATEwincombo:
            case CM_STATEwincheck:
            case CM_STATEwinradio:
            case CM_STATEwinbutton:
            case CM_STATEwinlistbox:return 0;

            case CM_EDITPOINTS:if(object2d){
                    if(GetObjectType2d(hsp,object2d)!=otLINE2D)return DM_NOLINE;
                    return((  state == sfbase)||
                           ((state>=sfeditline_start) && (sfeditline_end>=state)))?0:DM_NOHANDLER;
                }return DM_NOOBJECT2D;
            case CM_AVINEXT:
            case CM_AVIPREV:
            case CM_AVINOAUDIO:
            case CM_AVICYCLE :
            case CM_AVIPLAY  :
            case CM_AVISTART :
            case CM_AVIEND   :
            case CM_AVIPAUSE :
            case CM_AVIRESUME:
            case CM_AVISTOP  :if (object2d){
                    if(IsVideoFrame2d(hsp,object2d)){

                        switch(command){
                            case CM_AVINEXT:
                            case CM_AVIPREV:{FRAMEPROP fp;
                                if(GetFrameProp2d(hsp,object2d,&fp)){
                                    if(command==CM_AVIPREV){
                                        return fp.pos>0?0:DM_OVERAVIPOS;
                                    }
                                    VIDEOPROP vp;
                                    GetVideoProp(fp.video,&vp);
                                    return fp.pos<vp.end?0:DM_OVERAVIPOS;
                                }
                            }
                        }
                        return 0;}else return DM_NOVIDEOFRAME;
                }return DM_NOOBJECT2D;
            case CM_HOOKCONTROLS:{WORD w=(WORD)SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
                return w&SF_HOOKMOUSE;
            };
            case CM_LAYERGRID:
            case CM_LAYERCLASSES:
            case CM_LAYERLINKS:
            case CM_LAYEROTHER:{
                DWORD flags=0;
                switch(command){
                    case CM_LAYERGRID:return (grid && grid->visible);
                    case CM_LAYERCLASSES:flags=0xff0000ul;break;
                    case CM_LAYERLINKS:flags=0xff000000ul;break;
                    case CM_LAYEROTHER:flags=0xfffful;break;
                }
                if(!_class)return DM_NOTSCHEME;
                DWORD layers=~(SetSpaceParam2d(hsp,SP_GETLAYERS,0));
                return ((layers&flags)!=0);
            }
            case CM_BACKONE:
            case CM_TOBACK:{
                if (object2d){
                    INT16 t=GetObjectType2d(hsp,object2d);
                    if((t==otGROUP2D)||(t==otRGROUP2D))return DM_GROUP;
                    if(GetLowerObject2d(hsp,object2d)>0)return 0;
                    return DM_OBJECTONBOTTOM;
                }
            }return DM_NOOBJECT2D;

            case CM_FORWARDONE:
            case CM_TOFRONT	:{
                if (object2d){
                    INT16 t=GetObjectType2d(hsp,object2d);
                    if((t==otGROUP2D)||(t==otRGROUP2D))return DM_GROUP;
                    if (GetUpperObject2d(hsp,object2d)>0)return 0;return DM_OBJECTONTOP;
                }
            }return DM_NOOBJECT2D;
            case CM_GRID:return 0;
            case CM_SPECIALPASTE:
            case CM_EDITPASTE:
                if((IsClipboardFormatAvailable(CFormat_OBJ)  && _class)
                   ||(IsClipboardFormatAvailable(CFormat_CLASS) && _class)
                   || IsClipboardFormatAvailable(CF_2D)
                   || IsClipboardFormatAvailable(CF_DIB)
                   || IsClipboardFormatAvailable(CF_TEXT)
                   || IsClipboardFormatAvailable(CF_RTF)	)
                    return 0;
                else
                    return DM_EMPTYCLIPBOARD;
            case CM_DUPLICATE:{ if (object || __objects || object2d ||link || __objects2d)return 0;return DM_NOOBJECT;};
            case CM_INSERTOBJECT:
            case CM_INSERTOBJECTm:
            case CM_NEWVARPOINTm:
            case CM_NEWVARPOINT:
            case CM_CREATENEWOBJECTm:
            case CM_CREATENEWOBJECT:if (_class)return 0;return DM_NOTSCHEME;
            case CM_DELFROMGROUP:{
                if (object2d){
                    if(GetObjectParent2d(hsp,object2d))return 0;else return DM_NOPARENT;
                }
            }return DM_NOOBJECT2D;
            case CM_ADDTOGROUP:
            case CM_UNGROUP:{
                if (object2d){
                    INT16 t=GetObjectType2d(hsp,object2d);
                    if (t==otGROUP2D || t==otRGROUP2D)return 0;
                    return DM_NOGROUP;
                }
                return DM_NOOBJECT2D;
            };
            case CM_ACTUALSIZE:{
                if (object2d)return 0;
                if (object && _class){
                    INT16 id=_class->GetChildById(object);
                    if (id>-1){
                        if ((_class->childs[id].type->GetClass()->flags)&CF_SIZEABLE)return 0;

                    }
                } return DM_NOWITHIMAGEnosize;
            }
            case CM_CALCORDER:
                if(hardmode!=SCHEME_EDITOR)return DM_NOTSCHEME;
                if(_class && _class->childcount>1)return 0;
                return DM_TOSMALLIMAGES;
                ;break;
            case CM_EDITCUT:
            case CM_EDITDELETE: if (link || object || object2d || __objects  || __objects2d)return 0;return DM_NOOBJECT;


            case CM_SCOBJECT:
            case CM_SCLINK  :    if(hardmode!=SCHEME_EDITOR)return DM_NOTSCHEME;return 0;
            case CM_OBJECTDELETE:return ((object||link||object2d)?0:DM_NOOBJECT);

            case CM_ZUPPER      :
            case CM_ZLOWER      : return (object2d?0:DM_NOOBJECT2D);

                //----------------------------
            case CM_STATEbase     :
            case CM_STATEpoly     :
            case CM_STATEline     :
            case CM_STATErect     :
            case CM_STATEarc      :
            case CM_STATEroundrect:
            case CM_STATEellipse  :
            case CM_STATEbitmap   :
            case CM_STATEdouble   :
            case CM_STATEavi      :
            case CM_STATEtext     :return 0;
            case CM_STATEgroup    :
            case CM_STATErgngroup :if (GetNextObject2d(hsp,0)>0)return 0;
                else return DM_NOOBJECTONSCHEME;
                //----------------------------

        }}else
    {
        //disable3d
        switch(command){
            case CM_APPLYTEXTURE:
            case CM_TXTPLANAR  :
            case CM_TXTBOX     :
            case CM_TXTCYLINDER:
            case CM_TXTSPHERE  :if(htmpframe)return 0;return 200;

            case CM_EDITPASTE:
                if(IsClipboardFormatAvailable(clip_3d))
                    return 0;
                return DM_EMPTYCLIPBOARD;
            case CM_REMOVETEXTURE:
            case CM_DUPLICATE:{ if (object3d)return 0;return DM_NOOBJECT3D;};
            case CM_STATEgroup:if (GetNextObject2d((HSP2D)hsp3d,0)>0)return 0;
                else return DM_NOOBJECTONSCHEME3D;
            case CM_DELFROMGROUP:{
                if (object3d==0)return DM_NOOBJECT3D;
                if (GetObjectParent3d(hsp3d,object3d))return DM_NOPARENT;
            }return 0;
            case CM_EDITTEXTURES:
            case CM_3Dmove:
            case CM_3Dscale:
            case CM_3Drotate:
            case CM_3Dscnrotate:
            case CM_primBAR3D:
            case CM_primTORE3D:
            case CM_primCYLINDER3D:
            case CM_primSPHERE3D:
            case CM_primCONE3D:
            case CM_primSUBSPHERE3D:
            case CM_3Dsystem:
            case CM_primPYRAMID3D:
            case CM_3Daxis  :return 0;
            case CM_MANUALROTATE3D:
            case CM_3Daxislock:;
            case CM_EDITCUT:
            case CM_MODIFY3D:
            case CM_EDITPRIM3D:
            case CM_EDITVERTEXES3D:
            case CM_TEXTUREMAP:
            case CM_EDITDELETE:if (object3d)return 0;return DM_NOOBJECT3D;
            case CM_ADDTOGROUP:
            case CM_UNGROUP:if(object3d){
                    INT16 type=GetObjectType3d(hsp3d,object3d);
                    if (type==otGROUP3D)return 0;
                    return DM_NOGROUP;
                }
                return DM_NOOBJECT3D;
                //   case

        }}
    return TSpaceWindow::IsCommandDisable(command);
};
void TSchemeWindow::Cm_ObjVarSave(){
    PObject po=GetActiveObject();
    if (po){
        char s[256]="";

        if(FileDialog(HWindow,s,IDS_STATE,FD_SAVE)){
            SaveState(po,s);
        } }
};
void TSchemeWindow::Cm_ObjVarLoad(){
    PObject po=GetActiveObject();
    if (po){
        char s[256]="";
        if(FileDialog(HWindow,s,IDS_STATE,0)){
            LoadState(po,s);
        }
    }
};

PObject TSchemeWindow::GetActiveObject(){
    if (object && _object ){
        return  _object->_GetObject(object);
    }
    return 0;
};

void TSchemeWindow::SetControlButton(){

    //  if(mode3d)PressControlButton(mode3d); // Для 3d

    int b=-1;
    switch(state){
        case sfnames:          b=CM_IMGNAMES;break;
        case sfcalcorder:      b=CM_CALCORDER;break;
        case sfbase3d:         if(mode3d){b=mode3d;break;}
        case sfbase:           b=CM_STATEbase;break;
        case sfLbase:          b=CM_SCLINK;break;
        case sfzoom:           b=CM_ZOOM;break;
        case sfwindrag:        b=CM_STATEdrag;break;
        case sfHELP:           b=CM_HELPONSCHEME;break;
        case sfnewline:        b=CM_STATEline;break;
        case sfnewtext:        b=CM_STATEtext;break;
        case sfnewbitmap:      b=CM_STATEbitmap;break;
        case sfnewdoublebitmap:b=CM_STATEdouble;break;
        case sfnewavi:         b=CM_STATEavi;break;
        case sfnewarc:         b=CM_STATEarc;break;
        case sfnewrect:        b=CM_STATErect;break;
        case sfnewpolyline:    b=CM_STATEpoly;break;//
        case sfnewgroup3d:
        case sfnewgroup:       b=CM_STATEgroup;break;
        case sfnewrgngroup:    b=CM_STATErgngroup;break;
        case sfnewcircle:      b=CM_STATEellipse;break;
        case sfnewroundrect:   b=CM_STATEroundrect;break;
        case sfaddtogroup:     if (object2d){
                if (GetObjectType2d(hsp,object2d)==otGROUP2D)b=CM_STATEgroup;else
                    if (GetObjectType2d(hsp,object2d)==otRGROUP2D)b=CM_STATErgngroup;
            }break;
        case sfnewWinedit:     b=CM_STATEwinedit;break;
        case sfnewWincombo:    b=CM_STATEwincombo;break;
        case sfnewWincheck:    b=CM_STATEwincheck;break;
        case sfnewWinradio:    b=CM_STATEwinradio;break;
        case sfnewWinbutton:   b=CM_STATEwinbutton;break;
        case sfnewWinlist:     b=CM_STATEwinlistbox;break;
        case sftransprent:     b=CM_GETTRANSPARENT;break;
    };
    if (b!=-1) PressControlButton(b);
    PressControlButton(CM_3Daxislock,hlocalcs!=0);
    UpdateLayers();
};

void TSchemeWindow::__ImportObject(char*s,int type,POINT *_point)
{
    if (!type)
        type=GetFileType(s,0);
    if (!type)
    {
        _Error(EM_UNCNOWFORMAT);
        return;
    }

    if(state>=sfbase3d)
    {
        SetState(sfbase3d);
        object3d = ImportObject3d(hsp3d,s,type);
        CreateEditFrame3d();
        return ;
    }
    HOBJ2D obj=0;
    POINT2D p;
    if(_point)
    {
        p.x=_point->x;p.y=_point->y;
        DPtoLP2d(hsp,&p,1);
    }
    else
        p=GetPastePoint();


    //*------- Р А С Т Р О В Ы Е    ----------------*//
    if(type>0 && type<raster_END)
    {
        HOBJ2D hddib=0,hdib=0;
        if (type==raster_DOUBLE || type==raster_ICO)
        {
            hddib	= ImportDBM(hsp,s,type);

reloaddouble:
            if (hddib)
            {
                POINT2D so,ss,vs;
                so.x=0;  so.y=0;
                ss.x=-1; ss.y=-1;
                vs=ss;
                obj=CreateDoubleBitmap2d(hsp,hddib,&so,&ss,&p,&vs);
            }
        }
        else
        {
            HOBJ2D hdib=ImportBMP(hsp,s,type,&hddib);
            if(hddib)
                goto reloaddouble;
            if (hdib)
            {
                POINT2D so,ss,vs;
                so.x=0;  so.y=0;
                ss.x=-1; ss.y=-1;
                vs=ss;
                obj=CreateBitmap2d(hsp,hdib,&so,&ss,&p,&vs);
            }
        }
    }
    else
    {
        HSP2D _hsp;
        switch(type)
        {
            //*------------ В И Д Е О -------------------*//
            case video_AVI:
            {
                HV2D hv2d=OpenVideo(s,0);//V2_CYCLED
                if (hv2d)
                {
                    obj=CreateVideoFrame2d(hsp,hv2d,p.x,p.y,-1,-1,NULL,V2_SYNC);
                }
            } break;
                //*------------ WMF -------------------*//
            case vect_WMF:
            {
                if(SimplyExchange(_hsp,s,"wmf",0))goto m1;
            }break;
                //*------------ VDR -------------------*//
            case vect_VDR:
            {
                _hsp=LdSpace2d(0,s);
m1:
                if (_hsp)
                {
                    if(GetNextObject2d(_hsp,0))
                    {
                        HOBJ2D h=MakeSingleGroup(_hsp);
                        long size;
                        HGLOBAL b=CopyToMemory2d(_hsp,h,&size);
                        obj=PasteFromMemory2d(hsp,b,&p,PFC_ALL|PFC_MOVEOBJECT);
                        GlobalFree(b);
                    }
                    DeleteSpace2d(_hsp);
                }
            }break;
            case file3d_3DS:
            {
                if (InitTools3d())
                {
                    HSP3D  _hsp3d=CreateSpace3d(hsp);
                    HOBJ2D group=Import3DS(_hsp3d,s);
                    hcamera=0;
                    if (group)
                    {
                        while ((hcamera=GetNextObject3d(_hsp3d,hcamera))!=0)
                        {
                            if (GetObjectType2d((HSP2D)_hsp3d,hcamera)==otCAMERA3D)
                                break;
                        }
                        if (!hcamera)
                        {
                            MakeDefaultCamera(&camera3d,0);
                            hcamera=CreateCamera3d(_hsp3d,&camera3d);
                            SetObjectName3d(_hsp3d,hcamera,"Sc Default Camera");
                        }
                        POINT2D org,size;
                        size.x=400;
                        size.y=400;

                        obj=Create3dProjection2d(_hsp3d,hcamera, &p, &size );
                        TrCameraToFeet(hsp,object2d,0,0.8);
                    }
                    else
                        DeleteSpace3d(_hsp3d);
                }
            }break;
            default:
                _Error(EM_UNCNOWFORMAT);
        }
    }
    if (obj)
    {
        object2d=obj;
        AdjustZOrder(object2d);
        PostCreateObject2d(object2d);
    }
}

void TSchemeWindow::Cm_ImportObject(int str)
{
    char s[256]="";
    openaslink=0;
    if(FileDialog(HWindow,s,str,FD_ASLINK))
    {
        __ImportObject(s,0);
    }
    openaslink=0;
};

void TSchemeWindow::Cm_NewBitmap()
{
    BITMAP bmp;
    bmp.bmWidth=100;
    bmp.bmHeight=100;
    bmp.bmBitsPixel=4;

    char s[256];
    LoadString(HResource,IDS_str110,s,sizeof(s));

    if (BitmapDialog(HWindow,&bmp,s))
    {
        HGLOBAL block=_CreateDIB2d(bmp.bmWidth,bmp.bmHeight,
                                   bmp.bmBitsPixel,RGB(255,255,255));
        if (block)
        {
            HOBJ2D hdib=CreateDib2d(hsp,block);
            if (hdib)
            {
                POINT2D p=GetPastePoint();
                POINT2D so,ss,vs;
                so.x=0;so.y=0;
                ss.x=-1;ss.y=-1;
                vs=ss;
                HOBJ2D obj=CreateBitmap2d(hsp,hdib,&so,&ss,&p,&vs);
                if (obj)
                {
                    object2d=obj;
                    AdjustZOrder(object2d);
                    PostCreateObject2d(object2d);
                }
            }
            else
                GlobalFree(block);
        }
    }
};

void TSchemeWindow::Cm_NewDoubleBitmap(){
    BITMAP bmp;
    bmp.bmWidth=100;
    bmp.bmHeight=100;
    bmp.bmBitsPixel=4;
    char s[256];
    LoadString(HResource,IDS_str92,s,sizeof(s));
    if (BitmapDialog(HWindow,&bmp,s)){
        HGLOBAL xor=_CreateDIB2d(bmp.bmWidth,bmp.bmHeight,bmp.bmBitsPixel,RGB(255,255,255));
        HGLOBAL and=_CreateDIB2d(bmp.bmWidth,bmp.bmHeight,1,RGB(255,255,255));
        if (and && xor){
            HOBJ2D hddib=CreateDoubleDib2d(hsp,xor,and);
            if (hddib){
                POINT2D p=GetPastePoint();
                POINT2D so,ss,vs;
                so.x=0;so.y=0;
                ss.x=-1;ss.y=-1;
                vs=ss;
                HOBJ2D obj=CreateDoubleBitmap2d(hsp,hddib,&so,&ss,&p,&vs);
                if (obj){
                    object2d=obj;
                    AdjustZOrder(object2d);
                    PostCreateObject2d(object2d);
                } }else {GlobalFree(xor);GlobalFree(and);}
        }else{
            if (xor)GlobalFree(xor);
            if (and)GlobalFree(and);
        } }
};

void TSchemeWindow::Cm_NewObject(WORD mode)
{
    int ns;
    switch(mode){
        case CM_STATEbase:mode3d=0;if(hsp3d){ns=sfbase3d;}else ns=sfbase;break;
        case CM_STATEline:ns=sfnewline;SetCursor(HResource,IDC_LINE);break;
        case CM_STATEpoly:ns=sfnewpolyline;SetCursor(HResource,IDC_LINE);break;
        case CM_STATEtext:ns=sfnewtext;SetCursor(0,IDC_IBEAM);break;//
        case CM_STATEbitmap:Cm_NewBitmap();return;
        case CM_STATEdouble:Cm_NewDoubleBitmap();return;
        case CM_STATEavi:Cm_ImportObject(IDS_AVI);return;
        case CM_STATErect:ns=sfnewrect;SetCursor(HResource,IDC_RECT);break;
        case CM_STATEarc:ns=sfnewarc;SetCursor(HResource,IDC_ARC);break;
        case CM_STATEgroup:
            if(hsp3d){
                ns=sfnewgroup3d;object3d=0;
            }else{ns=sfnewgroup;object2d=0;}
            SetCursor(HResource,IDC_GROUP);
            break;

        case CM_STATErgngroup:ns=sfnewrgngroup;SetCursor(HResource,IDC_RGNGROUP);object2d=0;break;//
        case CM_STATEellipse:ns=sfnewcircle;SetCursor(HResource,IDC_CIRCLE);break;
        case CM_STATEroundrect:ns=sfnewroundrect;SetCursor(HResource,IDC_ROUNDRECT);break;

        case CM_STATEwinedit:    ns=sfnewWinedit;SetCursor(HResource,IDC_winEDIT);break;
        case CM_STATEwincombo:   ns=sfnewWincombo;SetCursor(HResource,IDC_COMBO);break;
        case CM_STATEwincheck:   ns=sfnewWincheck;SetCursor(HResource,IDC_CHECK);break;
        case CM_STATEwinradio:   ns=sfnewWinradio;SetCursor(HResource,IDC_RADIOcursor);break;
        case CM_STATEwinbutton:  ns=sfnewWinbutton;SetCursor(HResource,IDC_BUTTON);break;
        case CM_STATEwinlistbox: ns=sfnewWinlist;SetCursor(HResource,IDC_LIST);break;
            //	 case CM_STATEwintable:
            //	 case CM_STATEwincolor:
        default: ns=sfbase;
    };
    if (link)link=0;
    if(!hsp3d)DeleteFrame();
    SetState(ns);
};

LRESULT TSchemeWindow::WndProc(UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message)
    {
        case WM_CAPTURECHANGED:
            if(captured)
            {
                captured=FALSE;
                switch(state)
                {
                    case sfrectdrag:
                    {
                        SetCursor(0,IDC_ARROW);

                        SetState(sfrectbase);
                        if(__frame)
                            ShowObject2d(hsp,__frame);
                        if(__objects2d)
                        {
                            HOBJ2D  h= GetObjectByName2d(hsp,__objects2d,"$@#_frames2d_$@#");
                            if(h)ShowObject2d(hsp,h);
                        }
                        if(_class)
                            _class->UpdateCoordinates();
                        UpdateScrollers();
                    }break;

                    case sfrectselect:
                    case sfrectselect1:
                        DeleteFrames();
                        DeleteEditFrame2d(hsp);
                        SetState(sfbase);
                        break;

                    case sfOTrack:
                        CreateImageFrame();
                        _class->UpdateCoordinates();
                        UpdateScrollers();
                        SetState(sfbase);
                        break;

                    case sftransprent1:
                        tmpdata=0;
                        SetState(sftransprent);break;
                    case sfwindrag1:SetState(sfwindrag);break;
                    case sfrectdown:
                    case sfLClipBase:
                    case sfLDrag:UnDrag(lastpoint);break;
                    case sfLTrack:UnTrackLine(lastpoint,FALSE);break;
                    case sfctrllbtndown2d:
                    case sflbuttondown2d:SetState(sfbase);break;
                    case sfnewline1:SetState(sfbase);_DeleteObject2d();break;
                    case sfdrag:if (object2d<OID_RESERVED){
                            CreateEditFrame2d();
                            EndDragDrop2d(hsp,object2d);
                            UpdateScrollers();
                        }else{
                            if (object2d>OID_RCENTER) UpdateFromFrame2d(hsp);
                            object2d=GetCurrentObject2d(hsp);
                        }
                        SetState(sfbase);break;
                    case sfeditline_btndn:SetState(sfeditline);break;
                    case sfeditline_drag:SetCursor(HResource,IDC_POINT);
                        SetState(sfeditline);break;
                    case sfeditline_add:;
                    case sfeditline_delete:SetState(sfeditline);
                        SetCursor(HResource,IDC_POINT);break;
                    case sfWinwrect: CreateControl2d(lastpoint,TPoint(-1,-1),substate);break;
                    case sfWinwrect1:{
                        POINT2D org,size;
                        GetObjectOrgSize2d(hsp,OID_FRAME2d,&org,&size);
                        DeleteEditFrame2d(hsp);
                        ReleaseCapture();
                        CreateControl2d(org,size,substate);
                    }break;
                    case sfnewgrouprect:SetState(substate);break;
                    case sfnewgrouprect1:{
                        POINT2D org,size;
                        TRestoreCrd crd(hsp);
                        GetObjectOrgSize2d(hsp,OID_FRAME2d,&org,&size);
                        object2d = CreateRectGroup2d(org,size,substate==sfnewrgngroup);
                        if (object2d)SetCurrentObject(object2d);else DeleteEditFrame2d(hsp);
                        SetState(sfbase);
                    }break;
                    case sfimgaesize2:{
                        UpdateFromFrame2d(hsp);
                        INT16 count=GetGroupItemsNum2d(hsp,object);
                        if (count==2){
                            POINT2D org,size;
                            GetObjectOrgSize2d(hsp,object,&org,&size);
                            HOBJ2D obj=GetGroupItem2d(hsp,object,0);
                            SetObjectOrg2d(hsp,obj,&org);
                            SetObjectSize2d(hsp,obj,&size);
                            INT16 id=_class->GetChildById(object);
                            _class->UpdateCoordinates();
                            if (id>-1){_class->childs[id].flags|=CHF_WASSIZED;}
                        }
                    }
                    case sfimgaesize1:SetState(sfbase);break;

                    case sfeditpoint3d1:PopCrdSystem3d(hsp3d);SetState(sfeditpoint3d);
                        if(tmpdata)delete tmpdata;tmpdata=NULL;
                        break;
                    case sfdrag3d:{
                        switch(mode3d){
                            case CM_3Drotate:{
                                delete tmpdata;
                                tmpdata=NULL;
                                PopCrdSystem3d(hsp3d);
                                SetState(sfbase3d);
                            };
                            case CM_3Dmove:PopCrdSystem3d(hsp3d);
                            case CM_3Dscale:CreateEditFrame3d();SetState(sfbase3d);break;
                            default:SetState(sfbase3d);break;
                        };
                    }break;
                    case sfscndrag3d:{  CAMERA3D cam;
                        GetCamera3d(hsp3d,hcamera,&cam);
                        cam.poRenderType=camera3d.poRenderType;
                        lstrcpy(cam.poRenderName,camera3d.poRenderName);
                        camera3d=cam;
                        SetCamera3d(hsp3d,hcamera,&camera3d);
                        SetState(sfcamera3d);
                    }  break;
                    case sfscnbtndown3d:SetState(sfcamera3d);break;
                    case sflbuttondown3d:SetState(sfbase3d); break;

                }
            }break;
        case TW_UPDATECLASSWINDOWS:
        {UPDATESTRUCT*us=(UPDATESTRUCT*)lParam;
            if(_class && (us->what==UW_CLASS)){
                PClassListInfo pcl=(PClassListInfo)us->data;
                if(pcl->flags&CLF_LOADED){
                    if(pcl->GetClass()==_class){
                        char s[256]="";
                        switch(hardmode){
                            case IMAGE_EDITOR:GetImageWindowName(_class,s);break;
                            case SCHEME_EDITOR:GetSchemeWindowName(_class,s);break;
                            default:return 0;
                        }
                        SetWindowText(GetParent(HWindow),s);
                    }}
            }}return 0;
        case WM_KEYDOWN:return KeyDown(wParam,lParam);
        case WM_KEYUP:return KeyUp(wParam,lParam);

        case TW_SETFOCUS:if(wParam==1){
                SetControlButton();
                UpdateScaleBox();
                SetFocus(HWindow);

                return 5;}else
            {
                if(addmode3d)DoneAddMode();

            }break;
        case WM_RENDERALLFORMATS:{
            RenderFormat(CF_TEXT);
            RenderFormat(CF_RTF);
            RenderFormat(CF_DIB);
        }return 0;
        case WM_RENDERFORMAT:{
            UINT uFormat = (UINT) wParam;
            RenderFormat(uFormat);
        }return 0;
    }
    return TSpaceWindow::WndProc(message,wParam,lParam);
};

void TSchemeWindow::Cm_Avi(int command){
    int cycle=IsControlButtonPressed(CM_AVICYCLE);
    int noaudio=IsControlButtonPressed(CM_AVINOAUDIO);

    if(noaudio)noaudio=V2_NOAUDIO;
    if(cycle)cycle=V2_LOOPING;

    FRAMEPROP fp;
    if (GetFrameProp2d(hsp,object2d,&fp) && fp.video){
        VIDEOPROP vp;
        GetVideoProp(fp.video,&vp);
        switch(command){
            case CM_AVINEXT:{FRAMEPROP fp;
                if(GetFrameProp2d(hsp,object2d,&fp)){
                    FrameSetPos2d(hsp,object2d,fp.pos+1);
                }
            }break;
            case CM_AVIPREV:{FRAMEPROP fp;
                if(GetFrameProp2d(hsp,object2d,&fp)){
                    FrameSetPos2d(hsp,object2d,fp.pos-1);
                }
            }break;
            case CM_AVIPAUSE:VideoPause2d(hsp,object2d);break;
            case CM_AVIRESUME:VideoResume2d(hsp,object2d);break;
            case CM_AVISTOP :VideoStop2d(hsp,object2d);break;
            case CM_AVIPLAY :
                if (fp.paused || (fp.flags&V2_SYNC && vp.paused ))
                    VideoResume2d(hsp,object2d);else{
                    UINT32 flags=noaudio;
                    if(IsControlButtonPressed(CM_AVICYCLE))flags|=PF_CYCLED;
                    VideoPlay2d(hsp,object2d,vp.start,vp.end,0,flags);
                }

                break;
            case CM_AVISTART:FrameSetPos2d(hsp,object2d,vp.start);break;
            case CM_AVIEND  :FrameSetPos2d(hsp,object2d,vp.end);break;
        }
    }
};

void TSchemeWindow::UpdateLayers(){
    return ;
};
void TSchemeWindow::CreateEditFrame3d(){
    _DeleteEditFrame3d();
    HFRAME3D=object3d;
    SetAttribute3d(hsp3d,HFRAME3D,STATE_BOUNDS|STATE_AXIS3D,ATTRSET);
};
void TSchemeWindow::_DeleteEditFrame3d(){
    if(HFRAME3D){
        SetAttribute3d(hsp3d,HFRAME3D,STATE_BOUNDS|STATE_AXIS3D,ATTRRESET);
        HFRAME3D=0;
    }
};

void TSchemeWindow::PrepareNewProjection(){
    //    hlocalcs=0;
    //    PressControlButton(CM_3Daxislock,0);
    DeleteFrame();
    DeleteRotateObj3d();
    ::CreateEditFrame2d(hsp,&sizer,FRAME_HASLINES);
    if (object3d && (GetObjectType3d(hsp3d,OID_FRAME3d)==otFRAME3D))
        CreateEditFrame3d();
    else _DeleteEditFrame3d();
    GetObjectOrgSize2d(hsp,object2d,&view_org,&view_size);
    hcamera=Get3dViewCamera3d(hsp,object2d);
    GetCamera3d(hsp3d,hcamera,&camera3d);
    //     if(mode3d==CM_3Dscnrotate)CreateRotateObj3d();
    ShowControlBar(30,FALSE);
};
BOOL TSchemeWindow::InProjection(POINT2D&p){
    return ((view_org.x<=p.x)&&(view_org.y<=p.y)&&
            ((view_org.x+view_size.x)>=p.x)&&
            ((view_org.y+view_size.y)>=p.y));
};
void TSchemeWindow::CreateImageFrame(){
    SetCurrentObject2d(hsp,object);
    ::CreateEditFrame2d(hsp,&sizer,FRAME_MASK);
};
char * GetAbsolutePath(char *filename,char *tmp,TProject *prj=0);

void TSchemeWindow::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode)
{
    if (command>=CM_VARITEM && command<=CM_VARITEM+38)
    {
        if(_object)
        {
            PObject po=_object->_GetObject(object);
            Variable2dDialog(HWindow,po,command-CM_VARITEM);
            return;
        }
    }
    if (command>=CM_LAYERSstart && CM_LAYERSend>=command)
    {
        DWORD layers=SetSpaceParam2d(hsp,SP_GETLAYERS,0);
        DWORD mask=1l<<(command-CM_LAYERSstart);
        layers^=mask;
        SetSpaceParam2d(hsp,SP_SETLAYERS,layers);
    }
    if(command>=CM_AXIS3Dmode && command<=CM_AXIS3DmodeEnd && hsp3d)
    {
        SetComboStr(CM_3Daxis,(char*)axis_str[command - CM_AXIS3Dmode]);
    }
    if (((command>=CM_CAMERA) && (command<(CM_CAMERA+100)))
        ||((command>=CM_DRIVER3D) && (command<(CM_DRIVER3D+20)))
        )
    {
        HOBJ2D hcamera=0;
        HOBJ2D _hcamera=Get3dViewCamera3d(hsp,object2d);
        HSP3D _hsp3d=Get3dSpace2d(hsp,object2d);
        if (_hsp3d && _hcamera)
        {
            INT16 k=0;
            if ((command>=CM_DRIVER3D) && (command<(CM_DRIVER3D+20)))
            {
                INT16 render=0;
                char r_name[256];
                while ((render=GetNextRenderDriver3d(render,r_name,sizeof(r_name)))!=0)
                {
                    if (render==(command-CM_DRIVER3D))
                    {
                        CAMERA3D camera;
                        if(GetCamera3d(_hsp3d,_hcamera,&camera))
                        {
                            lstrcpy(camera.poRenderName,r_name);
                            SetCamera3d(_hsp3d,_hcamera,&camera);
                        }
                        return;
                    }
                }
            }
            else
            {
                hcamera=0;
                while ((hcamera=GetNextObject3d(_hsp3d,hcamera))!=0)
                {
                    if (GetObjectType3d(_hsp3d,hcamera)==otCAMERA3D)
                    {
                        if (k==(command-CM_CAMERA))
                        {
                            if (hcamera!=_hcamera)
                            {
                                Set3dViewCamera3d(hsp,object2d,hcamera);
                                TSchemeWindow::hcamera=hcamera;
                                GetCamera3d(hsp3d,hcamera,&camera3d);
                            }
                        }
                        k++;
                    }
                }
            }
        }
        return;
    }
    switch(command)
    {
        case CM_GOTOSHORTCUT:
        {
            HOBJ2D obj2d=0;
            if(object)obj2d=object;
            if(object2d)obj2d=object2d;
            int _size=GetObjectData2d(hsp,obj2d,UD_HYPERKEY,NULL,0,0);
            if(_size)
            {
                THyperKey key(hsp,obj2d,0);
                if(key.openmode==0)
                {
                    char tmp[256];
                    char * _file=0;
                    PClassListInfo _clInfo=GetClassListInfo(key.target);
                    if (_clInfo)
                    {
                        Vm_OpenClassScheme(key.target,0 /*flags*/);
                    }
                    else
                    {
                        char *_file=GetAbsolutePath(key.target,tmp);
                        mainwindow->Cm_Load(_file);
                    }
                }
            }
        }break;
        case CM_HELP:HELP("Scheme");break;
        case CM_PAGEIMAGE_PROP:
            if(_class)
            {
                PClass _parent=0;
                HOBJ2D handle=0;
                if(_object && _object->parent)
                {
                    _parent=_object->parent->_class;
                    if(_parent->scheme && _parent->scheme->hsp)
                    {
                        INT16 i;
                        handle=_object->GetHandle(&i);
                    }
                    else
                        _parent=0;
                }
                ClassDialog(HWindow,_class,_object,NULL,NULL,_parent,handle);
            }
            break;
        case CM_GETTRANSPARENT:
            if(object2d)
            {
                if(IsVideoFrame2d(hsp,object2d))
                {
                    SetCursor(HResource,IDC_GETTRANSPARENT);
                    SetState(sftransprent);
                }
            }
            break;
        case CM_STATEview3d:
        {
            POINT2D _org2d=GetPastePoint();
            DeleteFrames(hsp);
            object2d=MergeObject2d("projection3d","VIEW3D",_org2d);
            CreateEditFrame2d();
        }break;
        case CM_SAVEOBJECTAS:
        {
            if(state<sfbase3d)
            {
            }
            else
            {
                if(object3d)
                {
                    char s[256]="";
                    if(FileDialog(HWindow,s,IDS_3DFILES,FD_SAVE))
                    {
                        long size;
                        _DeleteEditFrame3d();
                        HGLOBAL block=CopyToMemory3d(hsp3d,object3d,&size);
                        CreateEditFrame3d();
                        TDOSStream st(s,TDOSStream::stCreate);
                        if(!st.status)
                        {
#ifdef WIN32
                            void * adr=GlobalLock(block);
#else
                            const void huge* adr=(void huge*)GlobalLock(block);
#endif
                            st.Write(((BYTE*)adr),size);
                            GlobalUnlock(block);
                        }
                        GlobalFree(block);
                    }
                }
            }
        }break;
        case CM_COPYBK:
        {
            HOBJ2D bk=GetBkBrush2d(hsp);
            if(bk)
            {
                bk=GetBrushDibIndex2d(hsp,bk);
                if(bk)
                {
                    HGLOBAL hg=GetDib2d(hsp,bk);
                    CopyDib(hg);
                }
            }
        }break;
        case CM_SAVEBK:
        {
            HOBJ2D bk=GetBkBrush2d(hsp);
            bk=GetBrushDibIndex2d(hsp,bk);
            if(bk)
            {
                char s[256]="";
                if(FileDialog(HWindow,s,IDS_BMPonly,FD_SAVE))
                {
                    Dib2dSaveAs(hsp,bk,s);
                }
            }
        }break;
        case CM_LOCALMENU:
            if ((state==sfbase)||(state==sfbase3d)||(state==sfrectbase))
            {
                EvRButtonDown(0,0,0);
            }
            break;
        case CM_SEARCH:
        {
            BYTE f=1;
            if(hardmode==SCHEME_EDITOR)
                f=(_object==0);
            else
                f=7;
            if(SearchDialog(HWindow,f))
            {
                if(!DoFind(TRUE))
                    MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG30),_MSG_INFO,MB_ICONINFORMATION|MB_OK);
            }
        }break;
        case CM_SEARCHAGAIN:
        {
            if(!DoFind(FALSE))
                MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG31),_MSG_INFO,MB_ICONINFORMATION|MB_OK);
        }break;
        case CM_ADJUSTIMAJES:
            if(__objects)
            {
                INT16 count=GetGroupItemsNum2d(hsp,__objects);
                if (count)
                {
                    HOBJ2D * items=new HOBJ2D[count];
                    GetGroupItems2d(hsp,__objects,items,count);
                    for(INT16 i=0;i<count;i++)
                    {
                        HOBJ2D _o=object;
                        HOBJ2D _os=__objects;
                        __objects=0;
                        HOBJ2D o=items[i];
                        POINT2D org,_org;
                        GetObjectOrgSize2d(hsp,o,&org,NULL);
                        _org=org;
                        Snap(org);
                        if((org.x!=_org.x)||(org.y!=_org.y))
                        {
                            object=o;
                            SetObjectPos(org);
                            SetObjectOrg2d(hsp,GetGroupItem2d(hsp,__frame,i),&org);
                        }
                        object=_o;
                        __objects=_os;
                    }
                    delete items;
                }
            }
            break;
        case CM_CONVERTTOCLASS:
        {
            char s[256];
            lstrcpy(s, "StratumClass");
            GetUniqueClassName(s);
            HOBJ2D o = ConvertToOne(s);
            if(o)
                object=o;
            CreateImageFrame();
        }break;

        case CM_SETVARrect:
        {
            if(__objects)
            {
                INT16 count=GetGroupItemsNum2d(hsp,__objects);
                if (count)
                {
                    HOBJ2D * items=new HOBJ2D[count];
                    GetGroupItems2d(hsp,__objects,items,count);
                    TCollection _items(count,0);
                    for(C_TYPE i=0;i<count;i++)
                    {
                        _items.Insert(_object->_GetObject(items[i]));
                    }
                    delete items;
                    SetVarDialog(HWindow,&_items);
                }
            }
            if(object)
            {
                TCollection _items(1,0);
                _items.Insert(_object->_GetObject(object));
                SetVarDialog(HWindow,&_items);
            }
        }break;

        case CM_REPLACECHILD:
        {
            if( _class)
            {
                char name[256];
                lstrcpy(name,"");
                if (NewChildDialog(HWindow,_class,name) && lstrlen(name)>0)
                {
                    if (object)
                    {
                        TWaitCursor wait(HWindow);
                        object=ReplaceChild(_class,object,name);
                        SetState(sfbase);
                        CreateImageFrame();
                    }
                    if (__objects)
                    {
                        INT16 count=GetGroupItemsNum2d(hsp,__objects);
                        if (count)
                        {
                            TWaitCursor wait(HWindow);
                            HOBJ2D * items=new HOBJ2D[count];
                            GetGroupItems2d(hsp,__objects,items,count);
                            DeleteFrames();
                            for(INT16 i=0;i<count;i++)ReplaceChild(_class,items[i],name);
                            delete items;
                            SetState(sfbase);
                        }
                    }
                }
            }
        }break;
        case CM_HELPONIMAGE:
        {
            if (object && _class)
            {
                INT16 i=_class->GetChildById(object);
                TClass *_c=_class->childs[i].type->GetClass();
                if(_c)_c->Help();
            }
        } break;
        case CM_SAVEOBJECT:
        {
            if(object2d)
            {
                INT16 t=GetObjectType2d(hsp,object2d);
                char s[256]="";
                if(FileDialog(HWindow,s,IDS_BMPonly,FD_SAVE))
                {
                    switch(t){
                        case otBITMAP2D:Dib2dSaveAs(hsp,GetDibObject2d(hsp,object2d),s);break;
                        case otDOUBLEBITMAP2D:DoubleDib2dSaveAs(hsp,GetDoubleDibObject2d(hsp,object2d),s);break;
                    }
                }
            }
        }break;
        case CM_COPYASBITMAP:
        {
            if(object2d)
            {
                INT16 t=GetObjectType2d(hsp,object2d);
                HGLOBAL hg=0;
                switch(t){
                    case otBITMAP2D:hg=GetDib2d(hsp,GetDibObject2d(hsp,object2d));break;
                    case otDOUBLEBITMAP2D:hg=GetXorDib2d(hsp,GetDoubleDibObject2d(hsp,object2d));break;
                }
                CopyDib(hg);
            }
        } break;
        case CM_3DNEWDEFCAMERA:
        {
            HSP3D _h=Get3dSpace2d(hsp,object2d);
            if(_h)
            {
                CAMERA3D cam;
                MakeDefaultCamera(&cam,0);
                // AdjustObject(_hsp3d,group);
                HOBJ2D hn=CreateCamera3d(_h,&cam);
                SetObjectName3d(_h,hn,"Sc Default Camera");
            }
        }break;
        case CM_MANUALROTATE3D:
        {
            PushCrdSystem3d(hsp3d);
            CRDSYSTEM crd;
            crd.type=GetCrdType();
            crd.hObject=hlocalcs?hlocalcs:object3d;
            SetCrdSystem3d(hsp3d,&crd);
            ManualRotateDialog3d(HWindow,hsp3d,GetObject3d());
            PopCrdSystem3d(hsp3d);
        }break;
        case CM_3DCLONECAMERA:
        {
            HSP3D _h=Get3dSpace2d(hsp,object2d);
            if(_h)
            {
                HOBJ2D h=Get3dViewCamera3d(hsp,object2d);
                CAMERA3D cam;
                if(GetCamera3d(_h,h,&cam))
                {
                    HOBJ2D hn=CreateCamera3d(_h,&cam);
                    char s[256];
                    GetObjectName2d((HSP2D)_h,h,s,sizeof(s));
                    lstrcat(s,"(2)");
                    SetObjectName2d((HSP2D)_h,hn,s);
                }
            }
            break;
        }
        case CM_3Daxislock:
        {
            if(IsControlButtonPressed(CM_3Daxislock))
                hlocalcs=object3d;
            else
                hlocalcs=0;
        } return;
        case CM_3Dsystem:
        case CM_3Daxis  :return;
        case CM_RENDER0         :
        case CM_RENDER1         :
        case CM_RENDER2         :
        case CM_RENDER3         :
        case CM_RENDER4         :
        case CM_RENDER_USELIGHT :
        case CM_RENDER_TEXTURE  :
        case CM_RENDER_HAZE     :
        case CM_RENDER_FOG      :
        case CM_RENDER_SHADED   :
        case CM_RENDER_ZSORT    :
        case CM_RENDER_ALLFACES :
            //	CM_DRIVER3D         500
        case CM_FITALL3D        :
        case CM_EDITCAMERA3D    :
        {
            HOBJ2D _hcamera=Get3dViewCamera3d(hsp,object2d);
            HSP3D _hsp3d=Get3dSpace2d(hsp,object2d);
            CAMERA3D camera3d;
            GetCamera3d(_hsp3d,_hcamera,&camera3d);
            int mode=camera3d.poRenderType&RENDER_TYPEMASK;
            switch(command){
                case CM_RENDER0         :mode=0;break;
                case CM_RENDER1         :mode=1;break;
                case CM_RENDER2         :mode=2;break;
                case CM_RENDER3         :mode=3;break;
                case CM_RENDER4         :mode=4;break;
                case CM_RENDER_USELIGHT :camera3d.poRenderType^=RENDER_USELIGHT;break;
                case CM_RENDER_HAZE     :camera3d.poRenderType^=RENDER_HAZE;break;
                case CM_RENDER_FOG      :camera3d.poRenderType^=RENDER_FOG;break;
                case CM_RENDER_TEXTURE  :camera3d.poRenderType^=RENDER_TEXTURED;break;
                case CM_RENDER_SHADED   :camera3d.poRenderType^=RENDER_SHADED;break;
                case CM_RENDER_ZSORT    :camera3d.poRenderType^=RENDER_ZSORT;break;
                case CM_RENDER_ALLFACES :camera3d.poRenderType^=RENDER_ALLFACES;break;
                case CM_FITALL3D        :TrCameraToFeet(hsp,object2d,0,0.8);return;
                case CM_EDITCAMERA3D    :
                {
                    Object3dDialog(HWindow,_hsp3d,_hcamera);
                    return;
                }
                default:return;
            }
            camera3d.poRenderType&=~RENDER_TYPEMASK;
            camera3d.poRenderType|=mode;
            SetCamera3d(_hsp3d,_hcamera,&camera3d);
        }break;
        case CM_3Dscnrotate:
        {
            mode3d=0;
            if(hrotate)
                SetState(sfbase3d);
            else
            {
                object3d=0;_DeleteEditFrame3d();
                CreateRotateObj3d();SetState(sfcamera3d);
            }
        }break;
        case CM_3Dmove:
        case CM_3Dscale:
        case CM_3Drotate:
        {
            mode3d=command;
            DeleteRotateObj3d();
            PressControlButton(command);
            if(state>sfbase3d)
                SetState(sfbase3d);
        }break;
        case CM_FILESAVE:
            if(hardmode==EDITOR)
            {
                BOOL sm=IsSpaceChanged2d(hsp);
                Cm_Save(TRUE);
                if (sm && hardmode!=EDITOR)
                    SetSpaceModify2d(hsp,TRUE);
                break;
            }
        case CM_FILESAVEAS:
        {
            BOOL sm=IsSpaceChanged2d(hsp);
            Cm_SaveAs(hardmode==EDITOR);
            if (sm && hardmode!=EDITOR)	SetSpaceModify2d(hsp,TRUE);
        }break;
        case CM_EDITTEXTURES:
        {
            if (object2d)
            {
                int t=GetObjectType2d(hsp,object2d);
                if (t==otVIEW3D2D)
                {
                    TexturesDialog(HWindow,Get3dSpace2d(hsp,object2d));
                }
            }
        }break;
        case CM_EDITVIEW3D:
            if (object2d)
            {
                int t=GetObjectType2d(hsp,object2d);
                if (t==otVIEW3D2D)
                {
                    SetState(sfbase3d);
                    hsp3d=Get3dSpace2d(hsp,object2d);
                    SetCurrentObject3d(hsp3d,-1);
                    object3d=0;
                    PrepareNewProjection();
                    break;
                }
            }
            break;
        case CM_REMOVETEXTURE:RemoveTexture3d(hsp3d,object3d);break;
        case CM_APPLYTEXTURE:if(htmpframe)ApplyTexture();break;
        case CM_TXTPLANAR  :if(htmpframe)CreateTextureFrame(0);break;
        case CM_TXTBOX     :if(htmpframe)CreateTextureFrame(1);break;
        case CM_TXTCYLINDER:if(htmpframe)CreateTextureFrame(2);break;
        case CM_TXTSPHERE  :if(htmpframe)CreateTextureFrame(3);break;
        case CM_UNMODIFY3D:
        {
            if(addmode3d)
                DoneAddMode(FALSE);
        }break;
        case CM_MODIFY3D:
        {
            hmain3d=object3d;
            CreateToolPopup(CM_MODIFY3D);
        }break;
        case CM_EDITPRIM3D:
        {
            if(addmode3d!=AM3D_PRIMITIVES)DoneAddMode(TRUE);
            addmode3d = AM3D_PRIMITIVES;
        };break;
        case CM_EDITVERTEXES3D:
        {
            if(addmode3d!=AM3D_VERTEXES)DoneAddMode(TRUE);
            addmode3d = AM3D_VERTEXES;
            _DeleteEditFrame3d();
            htmpframe=object3d;
            SetAttribute3d(hsp3d,object3d,STATE_POINTS|STATE_AXIS3D,ATTRSET);
            SetState(sfeditpoint3d);
        };break;
        case CM_TEXTUREMAP:
        {
            if(addmode3d!=AM3D_TEXTURE)DoneAddMode(TRUE);
            addmode3d=AM3D_TEXTURE;
            CreateTextureFrame((SCGetScVar("txtmap_type",0)&0x0f));
        }break;
#ifdef CAN_EDIT_BITMAP
        case CM_EDITBITMAP:
        {
            if (object2d)
            {
                INT16 t=GetObjectType2d(hsp,object2d);
                if (t==otBITMAP2D || t==otDOUBLEBITMAP2D)
                {
                    for(int i=0;i<winmanager->allmdi->count;i++)
                    {
                        TMDIChildWindow * wc=(TMDIChildWindow *)winmanager->allmdi->At(i);
                        if (wc && wc->MODE==MODE_BMPEDIT)
                        {
                            TBMPChild *w=(TBMPChild *)wc;
                            if (w->hsp==hsp && w->object2d==object2d)
                            {
                                SendMessage(mainwindow->GetMDI(),WM_MDIACTIVATE,(WPARAM)wc->HWindow,0);
                                return;
                            }
                        }
                    }
                    HGLOBAL _xor=0,_and=0;
                    switch(t){
                        case otBITMAP2D :_xor=GetDib2d(hsp,GetDibObject2d(hsp,object2d));break;
                        case otDOUBLEBITMAP2D:
                        {
                            HOBJ2D hddib=GetDoubleDibObject2d(hsp,object2d);
                            _xor=GetXorDib2d(hsp,hddib);
                            _and=GetAndDib2d(hsp,hddib);
                        }break;
                    }
                    if (_xor)
                    {
                        char s[256];
                        LoadString(HResource,IDS_str93,s,sizeof(s));
                        TBMPChild *w=mainwindow->CreateBmpWindow(s  ,_xor,_and);
                        w->mdiparent = GetMDIParent(HWindow);
                        w->object2d=object2d;
                        w->hsp=hsp;
                    }
                }
            }
        }break;
#endif
        case CM_HIDEOBJECT:Cm_HideObject();break;
        case CM_IMPORTOBJ:Cm_ImportObject(state<sfbase3d?IDS_impOBJECTS:IDS_3DFILES);break;
        case CM_SPACEPROP:SpaceDialog(HWindow,hsp,0,grid,_class,_object);break;
        case CM_HOOKCONTROLS:
        {
            WORD W=(WORD)SetSpaceParam2d(hsp,SP_GETSPACEFLAG,0);
            W=W^(WORD)0x01;
            SetSpaceParam2d(hsp,SP_SETSPACEFLAG,W);
        }break;
        case CM_ADDTOGROUP:
        {
            if(hsp3d)
                if (object3d)
                    SetState(sfaddtogroup3d);
                else
                    if (object2d)
                    {
                        DeleteFrame();
                        SetState(sfaddtogroup);
                    }
        }break;
        case CM_DELFROMGROUP:
        {
            HSP2D _hsp;
            HOBJ2D _hobj;
            if(hsp3d)
            {
                _hsp=(HSP2D)hsp3d;
                _hobj=object3d;
            }
            else
            {
                _hsp=hsp;
                _hobj=object2d;
            }
            HOBJ2D g=GetObjectParent2d(_hsp,_hobj);
            if (g && IsGroupContainObject2d(_hsp,g,_hobj))
                DelGroupItem2d(_hsp,g,_hobj);
        }break;
        case CM_UNGROUP:
        {
            if(hsp3d)
            {
                _DeleteEditFrame3d();DeleteGroup2d((HSP2D)hsp3d,object3d);
                SetState(sfbase3d);
                return;
            }
            else
                if (DeleteGroup2d(hsp,object2d))
                {
                    DeleteFrame();
                    SetState(sfbase);
                }
        }break;
        case CM_ACTUALSIZE:if (object2d)
            {
                POINT2D _size;GetActualSize2d(hsp,object2d,&_size);
                POINT2D size;GetObjectOrgSize2d(hsp,object2d,NULL,&size);
                if (size.x!=_size.x || size.y!=_size.y)
                    SetObjectSize2d(hsp,object2d,&_size);
            } break;
        case CM_EDITPOINTS:
            if (object2d && GetObjectType2d(hsp,object2d)==otLINE2D)
            {
                if ((state>=sfeditline_start) && (sfeditline_end>=state))
                {
                    SetState(sfbase);
                    SetCurrentObject(object2d);
                }
                else
                {
                    SetState(sfeditline);
                    DeleteFrame();
                    SetAttribute2d(hsp,object2d,2,ATTRSET);
                    SetFocus(HWindow);
                    SetCursor(HResource,IDC_POINT);
                    PressControlButton(CM_EDITPOINTS,1);
                }
            }
            break;
        case CM_AVINEXT:
        case CM_AVIPREV:
        case CM_AVIPAUSE:
        case CM_AVIRESUME:
        case CM_AVISTOP :
        case CM_AVIPLAY :
        case CM_AVISTART:
        case CM_AVIEND  :Cm_Avi(command);break;

        case CM_LAYERGRID:
        case CM_LAYERCLASSES:
        case CM_LAYERLINKS:
        case CM_LAYEROTHER:
        {
            if (command==CM_LAYERGRID)
            {
                if (grid)
                {
                    if (grid->visible)
                        grid->Remove();
                    else
                        grid->SetGrid();
                }
            }
            else
            {
                DWORD layers=SetSpaceParam2d(hsp,SP_GETLAYERS,0);
                switch(command){
                    case CM_LAYERCLASSES:layers^=0xff0000ul;break;
                    case CM_LAYERLINKS:layers^=0xff000000ul;break;
                    case CM_LAYEROTHER:layers^=0xfffful;break;
                }
                SetSpaceParam2d(hsp,SP_SETLAYERS,layers);
                UpdateLayers();
            }
        }break;
        case CM_BACKONE: SetZOrder2d(hsp,object2d,(INT16)(GetZOrder2d(hsp,object2d)-1));break;
        case CM_TOBACK:  ObjectToBottom2d(hsp,object2d);break;
        case CM_FORWARDONE:SetZOrder2d(hsp,object2d,(INT16)(GetZOrder2d(hsp,object2d)+1));break;
        case CM_TOFRONT	:ObjectToTop2d(hsp,object2d);break;
            //-----------------------------------------------
        case CM_primBAR3D:
        case CM_primTORE3D:
        case CM_primCYLINDER3D:
        case CM_primSPHERE3D:
        case CM_primCONE3D:
        case CM_primSUBSPHERE3D:
        case CM_primPYRAMID3D:
        {
            _DeleteEditFrame3d();
            COLOR3D color;
            color.c.r=255;
            color.c.b=255;
            color.c.g=255;
            color.c.a=0;
            char *name="Object";
            if (InitTools3d())
            {
                switch(command){
                    case CM_primBAR3D:name="BOX";object3d=MakeBar3d(hsp3d,color,10,10,10);break;
                    case CM_primTORE3D:name="TORE";object3d=MakeTore3d(hsp3d,0,color,5,20,16,32);break;
                    case CM_primCYLINDER3D:name="CYLINDER";object3d=MakeCylinder3d(hsp3d,0,color,10,10,20,16,0);break;
                    case CM_primSPHERE3D:name="SPHERE";object3d=MakeSphere3d(hsp3d,0,color,10,16,16);break;
                    case CM_primCONE3D:name="CONE";object3d=MakeCylinder3d(hsp3d,0,color,10,0,20,16,0);break;
                    case CM_primPYRAMID3D:name="PYRAMID";object3d=MakeCylinder3d(hsp3d,0,color,10,0,20,4,0);break;
                    case CM_primSUBSPHERE3D:
                    {
                        name="SUBSHERE";
                        COLOR3D color;
                        color.rgb=RGB(255,255,255);
                        double R=10;INT16 c1=16, c2=16;
                        POINT3D* pp=new POINT3D[c1];
                        for(INT16 i=0;i<c1;i++)
                        {
                            double a=((double)i/(c1-1))*M_PI_2;
                            pp[i].y=0;
                            pp[i].x=R*sin(a);
                            pp[i].z=R*cos(a);
                        }
                        pp[c1-1].x=pp[0].x;
                        ROTATOR3D r;
                        memset(&r,0,sizeof(r));
                        r.VctSize.z=R;
                        r.Angle=-M_PI*2.0;
                        object3d=MakeFace3d(hsp3d,0,pp,c1,&r,
                                            NULL,c2,color,NULL,NULL,1);
                        delete pp;
                    }break;//CM_primSUBSPHERE3D
                }
                if(object3d)
                {
                    CreateEditFrame3d();
                    SetObjectName2d((HSP2D)hsp3d,object3d,name);
                }
            }
        }break;
        case CM_HELPONSCHEME:SetState(sfHELP);SetCursor(HResource,IDC_HELPcursor);break;
        case CM_STATEdrag:
            if(state==sfwindrag)
                SetState(sfbase);
            else
            {
                SetState(sfwindrag);
                SetCursor(HResource,IDC_HAND);
            }
            break;
        case CM_ZOOM:        if(state==sfzoom)SetState(sfbase);else Cm_Zoom();break;
        case CM_SCOBJECT:    SetState(sfbase);break;
        case CM_SCLINK  :SetCursor(HResource,IDC_NEWLINK); SetState(sfLbase);break;
        case CM_STATEgroup:if(__objects2d){CreateGroupFromSelection();break;}
        case CM_STATEwinedit:
        case CM_STATEwincombo:
        case CM_STATEwincheck:
        case CM_STATEwinradio:
        case CM_STATEwinbutton:
        case CM_STATEwinlistbox:
            //case CM_STATEwintable:
            //case CM_STATEwincolor:
        case CM_STATEarc:
        case CM_STATEbase:
        case CM_STATEpoly:
        case CM_STATEline:
        case CM_STATEtext:
        case CM_STATEbitmap:
        case CM_STATEdouble:
        case CM_STATEavi:
        case CM_STATErect:
        case CM_STATErgngroup:
        case CM_STATEellipse:
        case CM_STATEroundrect: Cm_NewObject((INT16)command);break;
            //-----------------------------------------------
            // case CM_UNZOOM:SetState(sfbase);break;
        case CM_GRID:SpaceDialog(HWindow,hsp,1,grid,_class,_object);break;
        case CM_DUPLICATE:Cm_DuplicateObj();
            if (object2d && state<sfbase3d)
                PostCreateObject2d(object2d);
            break;

        case CM_OBJECTVARDEF:
        {
            if (object)
            {
                PObject po=_object->_GetObject(object);
                if (po)
                {
                    po->ClearAndDef();
                    po->GetProject()->UpdateWatchWindow();
                }
            }
            if (__objects)
            {
                INT16 count=GetGroupItemsNum2d(hsp,__objects);
                if (count)
                {
                    HOBJ2D * items=new HOBJ2D[count];
                    GetGroupItems2d(hsp,__objects,items,count);
                    DeleteFrames();
                    for(INT16 i=0;i<count;i++)
                    {
                        PObject po=_object->_GetObject(items[i]);
                        if(po)
                        {
                            po->SetDefault();
                            po->_CopyState(TObject::OLD_TO_NEW);
                            po->GetProject()->UpdateWatchWindow();
                        }
                    }
                    delete items;
                }
            }
        }break;
        case CM_IMGNAMES:if (state!=sfnames){SetState(sfnames);Cm_Names();}break;
        case CM_CALCORDER:if (state!=sfcalcorder){SetState(sfcalcorder);Cm_CalcOrder();}break;
        case CM_OBJECTVARSAVE:Cm_ObjVarSave();break;
        case CM_OBJECTVARLOAD:Cm_ObjVarLoad();break;
        case CM_POLYCOMBINEMODE_AND:
        case CM_POLYCOMBINEMODE_OR :
        case CM_POLYCOMBINEMODE_XOR :
        case CM_POLYCOMBINEMODE_DIFF:
        case CM_POLYFILLMODEALTERNATE:
        case CM_POLYFILLMODEWINDING  :Cm_Poly(command);break;
        case CM_OBJECTPLACEMENT:
        case CM_OBJECTEDIT:Cm_EditObject();break;
        case CM_SPECIALPASTE:
        {
            PASTESTRUCT ps;
            memset(&ps,0,sizeof(ps));
            ps.point=GetPastePoint();
            ps.flags=pasteflags;
            if(PasteDialog(HWindow,ps))
            {
                lastpoint=ps.point;
                UINT16 pf=pasteflags;pasteflags=ps.flags;
                lastclickvalid=1;
                /*
      if(CFormat_OBJ==ps.cformat)
      {
        if(_class)Cm_PasteObj(FALSE);
      }
      else */
                ProcessClipboardFormat(ps.cformat);
                lastclickvalid=0;
                pasteflags=pf;
            }
        };break;
        case CM_EDITPASTE:
            if(state<sfbase3d)
            {
                UINT cf=0;
                if(OpenClipboard(HWindow))
                {
                    while ((cf=EnumClipboardFormats(cf))!=0)
                    {
                        if(ProcessClipboardFormat(cf))
                            break;
                        if(GetClipboardOwner()!=HWindow)
                            OpenClipboard(HWindow);
                    }
                    CloseClipboard();
                }
            }
            else
            {
                if(IsClipboardFormatAvailable(clip_3d))
                {
                    POINT3D org;memset(&org,0,sizeof(org));
                    _DeleteEditFrame3d();
                    object3d=PasteFromClipboard3d(hsp3d,&org);
                    CreateEditFrame3d();
                }
            }
            break;
        case CM_EDITCOPY	:
            if(state<sfbase3d)
            {
                if (_class && (object||__objects || link || __objects2d) &&(object2d==0))
                    Cm_CopyObj();
                else
                    if (object2d||__objects2d)
                        Cm_Copy();
            }
            else
                CopyToClipboard3d(hsp3d,object3d);
            break;
        case CM_EDITCUT  :Cm_Cut();break;
        case CM_EDITDELETE:
        case CM_OBJECTDELETE:
            if(!readonly)
            {
                SetBallonData(0,0,0);
                if (state<sfbase3d)
                {
                    if (object2d)
                        _DeleteObject2d();
mdo: 		if (link)
                    {
                        if (MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG15),_MSG_QSC,MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2)==IDYES)
                        {
                            DeleteLinkFrame();
                            RemoveLink(_class,link,0);
                            link=0;
                            oldlink=0;
                        }
                    }
                    else
                    {
                        if (object)
                        {
                            if(RemoveChildren(_class,object))
                                object=0;
                        }
                        else
                        {
                            if ((__objects || __objects2d)&& (MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG16),_MSG_QSC,MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2)==IDYES))
                            {
                                if(__objects2d)
                                    FullDeleteObject2d(hsp,__objects2d);__objects2d=0;
                                if(__objects)
                                {
                                    INT16 count=GetGroupItemsNum2d(hsp,__objects);
                                    if (count)
                                    {
                                        TWaitCursor wait(NULL);
                                        HOBJ2D * items=new HOBJ2D[count];
                                        GetGroupItems2d(hsp,__objects,items,count);
                                        DeleteFrames();
                                        for(INT16 i=0;i<count;i++)
                                            RemoveChildren(_class,items[i],1,(i==(count-1)));
                                        delete items;
                                    }
                                }
                                SetState(sfbase);
                            }
                        }
                    }
                }
                else
                    if (object3d)
                        _DeleteObject3d();
            }
            break;
        case CM_CREATENEWOBJECTm:GetNewImagePos();//UpdateMousePos();
        case CM_CREATENEWOBJECT:
        {
            PClass __class;
            TLibrary*_lib=GetDefaultLib();
            if(_class && _class->listinfo && _class->listinfo->library)
                _lib=_class->listinfo->library;
            if(ClassDialog(HWindow,NULL,NULL,_lib,&__class))
            {
                HOBJ2D _obj=0;
                Snap(_mousepos);
                AddChildren(_class,__class->name,&_obj,_mousepos.x,_mousepos.y);
                object=_obj;
                SetState(sfbase);
                CreateImageFrame();
            }
        }break;
        case CM_NEWVARPOINTm:UpdateMousePos();
        case CM_NEWVARPOINT:
        {
            CreateVarPlace(_class,hsp,lastpoint.x,lastpoint.y);
        }break;
        case CM_LINKPROPERTIES:
        case CM_PROPERTIES:
            if (link)
            {OpenLinksDialog(link);break;}
            if (object3d && (state>=sfbase3d)){Object3dDialog(HWindow,hsp3d,object3d);return;}
            if (object){ OpenClassDialog(object);break;}
            if (object2d){Cm_EditObject();}
            break;
        case CM_INSERTOBJECTm:/*UpdateMousePos();*/GetNewImagePos();Cm_NewObject(CM_STATEbase);object=0;
        case CM_INSERTOBJECT:
        {
            //? dialogs
            char name[256]="";
            if (NewChildDialog(HWindow,_class,name) && lstrlen(name)>0)
            {
                AddChildren(_class,name,&object,_mousepos.x,_mousepos.y);
                SetState(sfbase);
                CreateImageFrame();
            }
        }break;
        case CM_scTEXTEDIT:
        case CM_IMAGE:
        {
            if (_class)
            {
                char* c;
                INT16 id=_class->GetChildById(object);
                if (id>-1)
                {
                    c=_class->childs[id].type->GetClassName();
                    switch (command){
                        case CM_IMAGE:
                            mainwindow->CreateImageWindow(c);
                            break;
                        case CM_scTEXTEDIT:
                            mainwindow->CreateTextWindow(_class->childs[id].type->GetClass());
                            break;
                    }
                }
            }
        };break;
        case CM_SCHEME:
        {
            if(_class)
            {
                PObject o=0;
                char* c=0;
                if (_object)
                {
                    o=_object->_GetObject(object);
                    c=o->_class->name;
                }
                else
                {
                    INT16 id=_class->GetChildById(object);
                    if (id>-1) c=_class->childs[id].type->GetClassName();
                }
                mainwindow->CreateSchemeWindow(c,o);
            }
        }break;
        default:
            TSpaceWindow::EvCommand(command,hwndCtl,wNotifyCode);
    }
}


void TSchemeWindow::Cm_PasteDIB(){
    if(OpenClipboard(HWindow))
    {
        HGLOBAL mem=GetClipboardData(CF_DIB);
        int good=1;
        void * adr1 =GlobalLock(mem);
        long size=GlobalSize(mem);
        HGLOBAL dib=GlobalAlloc(GMEM_MOVEABLE,size);
        void * adr2=GlobalLock(dib);
#ifdef WIN32
        if (adr1 && adr2)memcpy(adr2,adr1,size);else good=0;
#else
        if (adr1 && adr2)hmemcpy(adr2,adr1,size);else good=0;
#endif
        GlobalUnlock(dib);
        GlobalUnlock(mem);
        HOBJ2D hdib;
        if (good){ hdib=CreateDib2d(hsp,dib);
            POINT2D so,ss,vs;
            so.x=0;so.y=0;
            ss.x=-1;ss.y=-1;
            vs=ss;
            object2d=CreateBitmap2d(hsp,hdib,&so,&ss,&GetPastePoint(),&vs);
            AdjustZOrder(object2d);
            PostCreateObject2d(object2d);
        }
        CloseClipboard();
    }
};
BOOL TSchemeWindow::Cm_PasteRtf(){
    if(OpenClipboard(HWindow)){
        HGLOBAL mem=GetClipboardData(CF_RTF);
        {
            TMemoryStream st(mem);
            HOBJ2D htext=ReadRTF(hsp,&st);
            if(htext){
                object2d=CreateTextRaster2d(hsp,htext,&GetPastePoint(),&TPoint(-1,-1),&TPoint(0,0),0);
            }else{
                CloseClipboard();
                return FALSE;
            }
        }
        AdjustZOrder(object2d);
        PostCreateObject2d(object2d);
        SetState(sfbase);
        CloseClipboard();
        return TRUE;
    }
    return FALSE;
}
void TSchemeWindow::Cm_PasteTEXT(){
    if(OpenClipboard(HWindow))
    {HGLOBAL mem=GetClipboardData(CF_TEXT);
        char * adr = (char*)GlobalLock(mem);
        long size=GlobalSize(mem);
        size=min(2048,size);
        LOGTEXT lt;
        char* string=new char[((int)size)+1];
        lstrcpyn(string,adr,(int)size);
        GlobalUnlock(mem);
        lt.ltStringIndex=CreateString2d(hsp,string);
        delete string;
        _CreateText2d(lt);
        HOBJ2D htext=::CreateText2d(hsp,&lt,1);
        object2d=CreateTextRaster2d(hsp,htext,&GetPastePoint(),&TPoint(-1,-1),&TPoint(0,0),0);
        AdjustZOrder(object2d);
        PostCreateObject2d(object2d);
        SetState(sfbase);
        CloseClipboard();
    }
};
void TSchemeWindow::_CreateText2d(LOGTEXT&lt){
    HOBJ2D ho=GetCurrentText2d(hsp);
    if (GetCurrentText2d(hsp))
    {
        LOGTEXT _lt;
        if(GetLogText2d(hsp,ho,&_lt,1))
        {
            ho=lt.ltStringIndex;
            lt=_lt;
            lt.ltStringIndex=ho;
            return;
        }
    }
    LOGFONT2D lf;
    memset(&lf,0,sizeof(lf));
    lf.lfSize=sizeof(lf);
    lf.lfHeight=-12;
    LoadString(HResource,IDS_str120,lf.lfFaceName,sizeof(lf.lfFaceName));
    lt.ltFontIndex=CreateFont2d (hsp,&lf);
    lt.ltFgColor=0;
    HOBJ2D hb=GetBkBrush2d(hsp);
    if(hb){
        COLORREF rgb=GetBrushColor2d(hsp,hb);
        if(rgb==lt.ltFgColor)lt.ltFgColor=RGB(255,255,255);
    }
    lt.ltBgColor=0x01000000l;
};
POINT2D TSchemeWindow::GetPastePoint(){
    if(lastclickvalid)return lastpoint;
    GetNewImagePos();return _mousepos;
    /*
      RECT r;
      GetClientRect(HWindow,&r);
      POINT2D org;org.x=r.right;org.y=r.bottom;
     org.x/=2;org.y/=2;
      DPtoLP2d(hsp,&org,1);
     Snap(org);
 return org;*/
};

void TSchemeWindow::EvLButtonDblClk(WORD modKeys,int xPos,int yPos)
{
    POINT2D point2d;
    point2d.x=xPos;
    point2d.y=yPos;
    DPtoLP2d(hsp,&point2d,1);

    if (state < sfbase3d)
    {
        switch(state)
        {
            case sfbase:
            case sfrectbase:
            {
                BOOL shift = ScGetKeyState(VK_SHIFT);
                if(shift)
                {
                    HOBJ2D primary;
                    HOBJ2D obj2d = GetObjectFromPoint2d (hsp,&point2d,&primary);
                    if(!(object && obj2d == object && state==sfbase))
                    {
                        if(GrpPressProc(obj2d,primary))
                            return;
                    }
                }
            }break;
            case sfnewpolyline_1:
            {
                ReleaseCapture();
                SetState(sfbase);
                PostCreateObject2d(object2d);
                return;
            }
            case  sfzoom:return;
            case  sfwindrag:return;
        }
        SetState(sfbase);

        if (link)
        {
            OpenLinksDialog(link);
            return;
        }

        if (object2d)
        {
            switch (GetObjectType2d(hsp, object2d))
            {
                case otGROUP2D:;
                case otRGROUP2D:
                {
                    HOBJ2D o = GetSubObjectFromPoint2d(hsp,object2d,&point2d);
                    if (o != 0)
                    {
                        DeleteFrame();
                        o=(HOBJ2D)abs((INT16)o);
                        object2d = o;
                        __PrepareToDrag
                    }
                }break;

                default:
                    Cm_EditObject();
            }
            return;
        }

        if (object)
        {
            if (modKeys&MK_SHIFT || modKeys&MK_CONTROL)
            {
                TClass *_c=NULL;TObject* _o=NULL;
                if (_object)
                {
                    _o = _object->_GetObject(object);
                    if (_o)
                        _c = _o->_class;
                }
                else
                {
                    INT16 i = _class->GetChildById(object);
                    if (i > -1)
                        _c = _class->childs[i].type->GetClass();
                }
                if (_c || _o)
                {
                    if(modKeys & MK_SHIFT)
                        mainwindow->CreateSchemeWindow(_c->name, _o);

                    if(modKeys & MK_CONTROL)
                        mainwindow->CreateTextWindow(_c);
                }
            }
            else
                OpenClassDialog(object);
        }
    }
    else
    {
        // dblklk 3d
        switch (state){
            case sfbase3d:{
                if (object3d){
                    switch (GetObjectType3d(hsp3d,object3d)){
                        case otGROUP3D:{
                            HOBJ2D o=GetSubObject3dFromPoint2d(hsp,object2d,object3d,&point2d);
                            if (o!=0){
                                _DeleteEditFrame3d();
                                o=(HOBJ2D)abs((INT16)o);
                                object3d=o;
                                SetCurrentObject3d(hsp3d,object3d);
                                CreateEditFrame3d();
                            }else {object3d=0;}
                        }
                            break;
                        default:Object3dDialog(HWindow,hsp3d,object3d);
                    }
                }
            }break; // sfbase3d
            default:SetState(sfbase3d);
        }
    }
};

void TSchemeWindow::OpenLinksDialog(HOBJ2D obj)
{
    INT16 num;
    if (_class && obj && _class->GetLinkById(obj,&num))
        LinksDialog(HWindow,_class,hsp,obj);
    else
        link=0;
}

void TSchemeWindow::SetSpace(HSP2D _hsp)
{
    hsp=_hsp;
    object2d = object = NULL;
    link = 0;
    oldlink = 0;

    SetState(sfbase);
    DeleteFrame();
    SetCurrentObject2d(hsp, OID_NONE);

    POINT2D p;
    GetOrgSpace2d(hsp,&p);

    hscroll->SetPos(p.x,0);
    vscroll->SetPos(p.y,0);
    UpdateScrollers();

    grid = new TGrid(hsp);
}

BOOL TSchemeWindow::PressButton0(POINT2D& point2d,UINT16 flags,UINT modKeys)
{
    HOBJ2D _obj2d=GetObjectFromPoint2d (hsp,&point2d);

    if (link && (!readonly))
    {
        editpoint=GetLinePointFromPoint2d(hsp,link,&point2d);
        if(editpoint>-1 && flags&256){
            lastpoint=point2d;
            SetCapture();
            SetState(sfLDrag);
            INT16 num;
            plink=_class->GetLinkById(link,&num);
            return 0;
        }
    };
    if (((_obj2d==__frame)||(_obj2d==__objects) || (_obj2d==linkframe))&& _obj2d)return 0;

    if(((flags &1)==0)&&(GetObjectType2d(hsp,_obj2d)==otEDITFRAME2D))return 0;

    if (object2d==0 && _obj2d>=OID_RESERVED && object &&(!readonly)){
        if (flags&256){
            if (_class){
                int id=_class->GetChildById(object);
                if (id>-1){
                    if ((_class->childs[id].type->GetClass()->flags)&CF_SIZEABLE){
                        subobject=_obj2d;
                        SetCapture();
                        SetState(sfimgaesize1);
                    }}}
        }
        return 0;
    }
    int type = WhatObject(_obj2d);

    if ((_obj2d==0 && object)||(_obj2d==0 && object2d)||(type==3 && object))
    {
        SetCurrentObject2d(hsp,-1);
        DeleteEditFrame2d(hsp);
        DeleteRotateCenter2d(hsp);
        object=0;
        object2d=0;
    }
    if (_obj2d)
        switch (type)
        {

            case 3:
            {
                object=0;
                object2d=0;
                if (link>0 && link==_obj2d)
                {
                    // просто надавили на уже активную связь
                }
                else
                {
                    INT16 num;
                    if (_obj2d && ((plink = _class->GetLinkById(_obj2d,&num))!=0))
                    {
                        // Выяснили, что ткнули на связь
                        if (_obj2d != link)
                        {
                            link = _obj2d;
                            SetState(sfbase);
                            DeleteEditFrame2d(hsp);
                        }
                    }else link=0;
                }
            }break;
            case 1:
            case 2:
            {
                link = 0;
                object2d = 0;
                if (_obj2d != object)
                {
                    object = _obj2d;
                    CreateImageFrame();
                }
                if((flags&1)&&(!readonly))SetState(sfOdown1);
                lastpoint=point2d;
                GetObjectOrgSize2d(hsp,object,&delta,NULL);
                delta.x=point2d.x-delta.x;delta.y=point2d.y-delta.y;
            }break;
            case 0:{
                object2d =_GetObjectFromPoint(point2d);
                if (object2d)
                {
                    object=0;
                    link=0;
                    if ((flags &1)&&(!readonly)){
                        __PrepareToDrag
                    }else{ SetCurrentObject(object2d); }
                }else
                {if (GetCurrentObject2d(hsp)>0)
                        SetCurrentObject2d(hsp,-1);
                    DeleteFrame();
                }
            }break;
        }else{
        object2d=0;
        object=0;
        link=0;
    }
    SetText(NULL,0);
    CurrChanged();
    return 0;
};

void TSchemeWindow::DeleteAllEditItems(HSP2D _hsp){
    DeleteLinkFrame(_hsp);
    DeleteFrames(_hsp);
};

void TSchemeWindow::EvLButtonDown(WORD modKeys,int xPos,int yPos){
    POINT2D point2d;
    POINT2D _point;_point.x=xPos;_point.y=yPos;
    point2d=_point;
    DPtoLP2d(hsp,&point2d,1);
    if (state<sfbase3d){
        POINT2D _point2d=point2d;
        Snap(_point2d);
__restart:
        switch (state){
            case sfnames:break;
            case sftransprent:{
                VIDEOPIXEL vp;
                vp.hsp=hsp;
                vp.hframe=object2d;
                vp.point=point2d;

                if(aviaudioMessage(HWindow,WM_SCV_GETPIXEL,(LPARAM)&vp,0)){
                    SetState(sftransprent1);
                    SetCapture();
                    lastpoint.x=xPos;
                    FRAMEPROP fp;
                    tmpdata=0;
                    if (GetFrameProp2d(hsp,object2d,&fp)){
                        fp.color=vp.color;
                        tmpdata=(void*)(fp.sense);
                        SetFrameProp2d(hsp,object2d,&fp);
                    }
                }
            }break;
            case sfbase:{
                BOOL shift=ScGetKeyState(VK_SHIFT);
                if(shift)
                {
                    HOBJ2D primary;
                    HOBJ2D obj2d=GetObjectFromPoint2d (hsp,&point2d,&primary);
                    if(GrpPressProc(obj2d,primary))
                        break;
                }
                PressButton0(point2d,257,modKeys);
                if (object==0 && link==0 && object2d==0){
                    // **** Был _class в условии ( && _class)
                    SetState(sfrectselect);
                    SetCapture();
                    lastpoint=point2d;
                }
            } break;

            case sfLbase:
            {
                HOBJ2D obj2d=GetObjectFromPoint2d (hsp,&point2d);
                int type=WhatObject(obj2d);
                if (obj2d && (type==1||type==2))
                {
                    link=0;
                    SetState(sfLdown1);
                    object=obj2d;
                    DeleteEditFrame2d(hsp);
                    Snap(point2d);
                    lastpoint=point2d;
                }else SwithcToBaseMode();
            };break;
            case sfLClipBase:{
                HOBJ2D obj2d=GetObjectFromPoint2d (hsp,&point2d);
                int type=WhatObject(obj2d);
                if (obj2d && (type==1||type==2)){
                    link=0;
                    SetState(sfLClipdown1);
                    object=obj2d;
                    DeleteEditFrame2d(hsp);
                    Snap(point2d);
                    lastpoint=point2d;
                }else {SwithcToBaseMode();//DoneLinkPaste();
                }
            }break;
            case sfwindrag:{
                lastpoint.x=xPos;
                lastpoint.y=yPos;
                SetCapture();
                SetState(sfwindrag1);
            };break;
            case sfzoom:Zoom(xPos,yPos,TRUE);break;
            case sfrectbase:{
                HOBJ2D primary;
                HOBJ2D obj2d=GetObjectFromPoint2d (hsp,&point2d,&primary);

                BOOL shift=ScGetKeyState(VK_SHIFT);

                if(shift && GrpPressProc(obj2d,primary))
                    return;
                if (obj2d && (obj2d==__objects || obj2d==__frame || obj2d==__objects2d)){
                    SetState(sfrectdown);
                    SetCapture();
                    lastpoint=point2d;
                    return;
                }
                SetState(sfbase);goto __restart;
                break;}
            case sfHELP:{

                MSGBOX(HWindow,MAKEINTRESOURCE(IDS_str111),_MSG_ERROR,MB_OK);}break;

            case sfcalcorder:{
                HOBJ2D obj=GetObjectFromPoint2d (hsp,&point2d);
                if(WhatObject(obj)==1){
                    if(modKeys&MK_SHIFT){
                        char msg[256],cmd[256];
                        LoadString(HResource,IDS_str112,cmd,sizeof(cmd));
                        wsprintf(msg,cmd,1,_class->childcount);
                        char s[256];
                        wsprintf(s,"%d",substate+1);
                        if(InputBox(HWindow,"Calc order",msg,s)){
                            int co;
                            if((sscanf(s,"%d",&co)==1) && (co>0) && (co<=_class->childcount)){
                                substate=co-1;
                            }else return;
                        }else   return;
                    }
                    _class->SetObjectOrder(obj,(INT16)substate);
                    substate++;
                    if (substate>=_class->childcount)SetState(sfbase);else
                        UpdateCalcOrder();
                }
            }break;
            case sfL_delete:{
                INT16 p=GetLinePointFromPoint2d(hsp,link,&point2d);
                if (p!=0 && GetVectorNumPoints2d(hsp,link)!=p+1){
                    DelVectorPoint2d(hsp,link,p);
                    DeleteLinkFrame();oldlink=0;

                }

            }break;
            case sfL_add:{
                INT16 numpoints=GetVectorNumPoints2d(hsp,link);
                POINT2D * pn=new POINT2D[numpoints];
                GetVectorPoints2d(hsp,link,pn,numpoints);
                INT16 j=-1;
                for (INT16 i=0;i<numpoints-1;i++){
                    if (PointNearLine2d(&(pn[i]),&(pn[(INT16)(i+1)]),&point2d)){j=(INT16)(i+1);
                        break;}
                }
                //		  linkobj=0;
                if (j>-1){
                    DeleteLinkFrame();
                    oldlink=0;
                    lastpoint=point2d;
                    AddVectorPoint2d(hsp,link,j,&point2d);
                    SetState(sfLDrag);editpoint=j;
                }
                else {ReleaseCapture();SetState(sfbase);}
                delete pn;
            }break;
            case sfLnextpoint:{}break;
            case sfLedit:{}break;

                // Edit objects 2d
            case sfnewtext:{
                LOGTEXT lt;
                lt.ltStringIndex=CreateString2d(hsp,"sample text");
                _CreateText2d(lt);
                HOBJ2D htext=::CreateText2d(hsp,&lt,1);
                POINT2D _textsize;_textsize.x=_textsize.y=-1;
                POINT2D _textdelta;_textdelta.x=_textdelta.y=0;
                object2d=CreateTextRaster2d(hsp,htext,&_point2d,&_textsize,&_textdelta,0);
                AdjustZOrder(object2d);
                PostCreateObject2d(object2d);
                Cm_EditObject();
                SetState(sfbase);
            }break;
            case sfnewline:{
                POINT2D pp[2];pp[0]=_point2d;pp[1]=_point2d;
                object2d=CreatePolyline2d(hsp,GetDefaultPen(),0,pp,2);
                if (object2d){
                    AdjustZOrder(object2d);
                    editpoint=1;SetState(sfnewline1);SetCapture();
                }	break;}
            case sfnewarc:{
                POINT2D pp[1];
                pp[0]=_point2d;
                lastpoint=_point2d;
                HOBJ2D hp=GetDefaultPen();
                HOBJ2D hb=GetDefaultBrush();
                object2d=CreatePolyline2d(hsp,hp,hb,pp,1);
                if (object2d){
                    AdjustZOrder(object2d);
                    SetState(sfnewarc1);SetCapture();
                }
            }break;
            case sfnewrect:{
                POINT2D pp[5];
                lastpoint=_point2d;
                for(int i=0;i<5;i++)pp[i]=_point2d;
                HOBJ2D hp=GetDefaultPen();
                HOBJ2D hb=GetDefaultBrush();
                object2d=CreatePolyline2d(hsp,hp,hb,pp,5);
                if (object2d){
                    AdjustZOrder(object2d);
                    SetState(sfnewrect1);SetCapture();
                }
            }break;
            case sfnewcircle:{
                INT16 nn=SCGetScVar("points_in_circle",32);
                POINT2D * pp=new POINT2D[nn];
                for (INT16 i=0;i<nn;i++)pp[i]=_point2d;
                HOBJ2D hp=GetDefaultPen();
                HOBJ2D hb=GetDefaultBrush();
                object2d=CreatePolyline2d(hsp,hp,hb,pp,nn);delete pp;
                AdjustZOrder(object2d);
                SetState(sfnewcircle1);
                lastpoint=_point2d;
                SetCapture();break;
            }
            case sfnewroundrect:{
                POINT2D pp[5];
                for(INT16 i=0;i<5;i++)pp[i]=_point2d;
                HOBJ2D hp=GetDefaultPen();
                HOBJ2D hb=GetDefaultBrush();
                object2d=CreatePolyline2d(hsp,hp,
                                          hb,pp,5);lastpoint=_point2d;
                if (object2d){
                    AdjustZOrder(object2d);
                    SetState(sfnewroundrect1);SetCapture();
                }	break;}
            case sfpaste:{
                RECT r;
                GetClientRect(HWindow,&r);
                POINT __pp;__pp.x=xPos;__pp.y=yPos;
                if (PtInRect(&r,__pp)){
                    if (substate){
                    }else object2d=PasteFromClipboard2d(hsp,&_point2d,
                                                        PFC_SETCURRENT|PFC_SETFRAME|PFC_MOVEOBJECT|PFC_ALL);}
                SetState(sfbase);
                ReleaseCapture();
                break;}
            case sfeditline:
                editpoint=GetLinePointFromPoint2d(hsp,object2d,&point2d);
                if (editpoint>-1){
                    SetState(sfeditline_btndn);
                    lastpoint=point2d;
                }else LocalUndo(FALSE);
                break;
            case sfeditline_add:{
                INT16 numpoints=GetVectorNumPoints2d(hsp,object2d);
                POINT2D * pn=new POINT2D[numpoints];
                GetVectorPoints2d(hsp,object2d,pn,numpoints);
                INT16 j=-1;
                for (INT16 i=0;i<numpoints-1;i++){
                    if (PointNearLine2d(&(pn[i]),&(pn[(INT16)(i+1)]),&point2d)){j=(INT16)(i+1);
                        break;}
                }
                if (j==-1){
                    gr_float x=pn[0].x-_point2d.x,y=pn[0].y-_point2d.y;
                    gr_float d1=sqrt(x*x+y*y);
                    x=pn[numpoints-1].x-_point2d.x;y=pn[numpoints-1].y-_point2d.y;
                    gr_float d2=sqrt(x*x+y*y);
                    if (d1<10 || d2<10)
                        j=(d1<d2)?(INT16)0:numpoints;
                }
                ReleaseCapture();
                if (j>-1){lastpoint=_point2d;
                    AddVectorPoint2d(hsp,object2d,j,&_point2d);
                    SetState(sfeditline_btndn);editpoint=j;}
                else 	SetState(sfeditline);
                delete pn;
            }break;
            case sfeditline_delete:{
                SetState(sfeditline);
                INT16 p=GetLinePointFromPoint2d(hsp,object2d,&point2d);
                if (p>-1 && GetVectorNumPoints2d(hsp,object2d)>2)
                    DelVectorPoint2d(hsp,object2d,p);
                SetCursor(HResource,IDC_POINT);
                ReleaseCapture();}
                break;
            case sfnewpolyline_1:{
                AddVectorPoint2d(hsp,object2d,editpoint,&_point2d);
                editpoint++;
                SetState(sfnewpolyline1);
            }break;
            case sfnewpolyline:{
                POINT2D pp[2];pp[0]=_point2d;pp[1]=_point2d;
                HOBJ2D hp=GetDefaultPen();
                HOBJ2D hb=0;//GetDefaultBrush();
                object2d=CreatePolyline2d(hsp,hp,hb,pp,2);

                if (object2d){
                    lastpoint=point2d;
                    SetState(sfnewpolyline2);SetCapture();
                    editpoint=1;
                }
            }break;
            case sfnewrgngroup:
            case sfnewgroup:
            case sfaddtogroup:{
                HOBJ2D o=GetObjectFromPoint2d(hsp,&point2d);
                if (o && !WhatObject(o)){
                    HOBJ2D p=0,p1;p1=o;
                    INT16 ot;
                    do {
                        p1=GetObjectParent2d(hsp,p1);
                        if (p1)p=p1;
                    }while (p1);

                    if (p)o=p;
                    if (object2d!=0 && o==object2d)_Error(EM_ALREDY2D);
                    else{
                        ot=(INT16)(object2d?GetObjectType2d(hsp,object2d):
                                            ((state==sfnewrgngroup)?otRGROUP2D:otGROUP2D));
                        if (ot==otRGROUP2D && GetObjectType2d(hsp,o)!=otLINE2D){
                            _Error(EM_FIRSTMUSTL);
                            break;
                        }else{
                            if (ot==otRGROUP2D && object2d==0 && GetBrushObject2d(hsp,o)==0){
                                _Error(EM_MUSTBRUSH);
                                break;}else
                            {if (object2d==0){
                                    if (ot==otGROUP2D)object2d=CreateGroup2d(hsp,0,0);else
                                        object2d=CreateRgnGroup2d(hsp,0,0);
                                    if (object2d==0){SetState(sfbase);
                                        _Error(EM_GROUPCREATE);
                                        break; } }
                                int as;
                                if (ot==otGROUP2D)as=AddGroupItem2d(hsp,object2d,o);else
                                {RGNGROUPITEM rgni;
                                    rgni.Rop=RGN_OR;rgni.Object=o;
                                    as=AddRgnGroupItem2d(hsp,object2d,GetGroupItemsNum2d(hsp,object2d),
                                                         & rgni );
                                }
                                if(!as) _Error(EM_ADDTOGROUP);
                            } } }
                    if (state!=sfaddtogroup) SetState(sfaddtogroup);}else{
                    if (state!=sfaddtogroup){
                        substate=state;
                        SetCapture();
                        SetState(sfnewgrouprect);
                    }
                }
            }	break;
            case sfnewWinedit:
            case sfnewWincombo:
            case sfnewWincheck:
            case sfnewWinradio:
            case sfnewWinbutton:
            case sfnewWinlist:{
                substate=state;
                SetCapture();
                lastpoint=_point2d;
                SetState(sfWinwrect);
            }

        }
    }else{
        // down3d S T A R T
        if(InProjection(point2d)){
            switch(state){
                case sfbase3d:{
                    PressButton3d(point2d,1,modKeys);

                }break;
                case sfeditpoint3d:{
                    INT16 i= GetPoint3dFromPoint2d(hsp,object2d, object3d,&point2d,2);
                    if(i>-1){
                        editpoint=i;
                        POINT3D _base;
                        if(GetPoints3d( hsp3d,object3d,&base,editpoint,1))
                        {_base=base;
                            SetCursor(HResource,IDC_DRAGPOINT);
                            SCToolPopupVar(CM_MODIFY3D,10,1,(void*)editpoint);
                            SCToolPopupVar(CM_MODIFY3D,11,1,&base);
                            ::SetCapture(HWindow);
                            SetState(sfeditpoint3d1);

                            PushCrdSystem3d(hsp3d);
                            CRDSYSTEM crd;
                            crd.hObject=object3d;
                            crd.type=LOCAL_COORDINATE;
                            SetCrdSystem3d(hsp3d,&crd);
                            LP3dtoLP3d(hsp3d,&base,&base,1,LP3D_CURRENT,LP3D_WORLD);

                            crd.type=GetCrdType();
                            if(crd.type!=VIEW_COORDINATE)
                            {
                                int a=GetAxis3d();
                                submode3d=(INT16)a;
                                crd.hObject=hlocalcs?hlocalcs:object3d;
                                SetCrdSystem3d(hsp3d,&crd);
                                LP3dtoLP3d(hsp3d,&base,&base,1,LP3D_WORLD,LP3D_CURRENT);

                            }else submode3d=0;

                            double r;
                            tmpdata=0;
                            if(SCGetScVarD("vertex_area",r)){ // соседние точки
                                INT16 c=GetNumPoints3d(hsp3d,object3d);
                                POINT3D * points = new POINT3D[c];
                                if(!GetPoints3d(hsp3d,object3d,points,0,c))return;
                                if(r<0){
                                    POINT3D o,s;
                                    GetObjectOrgSize3d(hsp3d,object3d,&o,&s);
                                    double x=s.x;
                                    x=sqrt(s.y*s.y+s.x*s.x+s.z*s.z);
                                    r=x*(-r)/100;
                                }
                                size3d=r;
                                TCollection items(10,10);
                                for(int i=0;i<c;i++)if(i!=editpoint){
                                    double d=sqrt((points[i].x-_base.x)*(points[i].x-_base.x)+
                                                  (points[i].y-_base.y)*(points[i].y-_base.y)+
                                                  (points[i].z-_base.z)*(points[i].z-_base.z));
                                    if(d<r){items.Insert((pointer)i);
                                    }
                                }
                                if(items.count){
                                    BYTE * tmp=new BYTE[2+items.count*2+sizeof(POINT3D)*items.count];
                                    *((INT16*)tmp)=items.count;
                                    for(C_TYPE j=0;j<items.count;j++){
                                        INT16 k=(INT16)items.At(j);
                                        ((INT16*)tmp)[j+1]=k;
                                        ((POINT3D*)(tmp+(2+items.count*2)))[j]=points[k];
                                    }
                                    tmpdata=tmp;
                                    org3d=_base;
                                }
                                delete points;
                            }
                        }else SetCursor(0,IDC_ARROW);
                    }}break;
                case sfcamera3d:{
                    submode3d=GetRotate3dCursor(point2d);
                    if(submode3d){
                        SetState(sfscnbtndown3d);
                        lastpoint=point2d;
                    }
                }break;
                case sfnewgroup3d:
                case sfaddtogroup3d:{
                    HOBJ2D primary3d;
                    HOBJ2D o=GetObject3dFromPoint2d(hsp,object2d,&point2d,&primary3d);
                    if (o){
                        HOBJ2D p=0,p1;p1=o;
                        INT16 ot;
                        do {
                            p1=GetObjectParent3d(hsp3d,p1);
                            if (p1)p=p1;
                        }while (p1);

                        if (p)o=p;
                        if (object3d!=0 && o==object3d)_Error(EM_ALREDY2D);
                        else{

                            {if (object3d==0){
                                    object3d=CreateGroup2d((HSP2D)hsp3d,0,0);
                                    if (object3d==0){SetState(sfbase3d);
                                        _Error(EM_GROUPCREATE);
                                        break; } }
                                int as;

                                as=AddGroupItem2d((HSP2D)hsp3d,object3d,o);
                                if(!as) _Error(EM_ADDTOGROUP);
                            }  }
                        if (state!=sfaddtogroup3d) SetState(sfaddtogroup3d);

                    }else{
                        // Rect track
                    }
                }	break;
            }
        }else{
            HOBJ2D p;
            if(GetObjectFromPoint2d(hsp,&point2d,&p)&&
               GetObjectType2d(hsp,p)==otVIEW3D2D && Get3dSpace2d(hsp,p)==hsp3d){
                object2d=p;SetCurrentObject2d(hsp,object2d);
                PrepareNewProjection();
            }else{
                if (state==sfbase3d){
                    if (object3d<1){
                        DoneAddMode();
                        mode3d=0;
                        SetState(sfbase);hsp3d=0;
                        SetCurrentObject2d(hsp,object2d);
                        DeleteEditFrame2d(hsp);
                        CreateEditFrame2d();
                        DeleteRotateObj3d();
                    }else
                    {
                        _DeleteEditFrame3d();object3d=0;
                    }
                }
            }
        }
        // down3d  E N D
    }
};
void TSchemeWindow::CreateControl2d(POINT2D& org,POINT2D&size,int _state){
    char* _cn=NULL;
    char*_txt=NULL;
    DWORD dwStyle=0;
    DWORD ExdwStyle=0;
    INT16 id=100;
    INT16 nWidth=100;
    INT16 nHeight=14;

    DWORD lDlgBaseUnits=GetDialogBaseUnits();
    //nWidth=(x * LOWORD(lDlgBaseUnits)) / 4
    nHeight=(INT16)((nHeight * HIWORD(lDlgBaseUnits)) / 8);

    switch(_state){
        case sfnewWinedit:  _cn="EDIT";dwStyle=ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP;break;
        case sfnewWincombo: _cn="COMBOBOX";dwStyle=CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP;
            nHeight=200;break;
        case sfnewWincheck: _cn="BUTTON";_txt="CheckBox";dwStyle=BS_AUTOCHECKBOX | WS_TABSTOP;break;
        case sfnewWinradio: _cn="BUTTON";_txt="RadioButton";dwStyle=BS_AUTORADIOBUTTON	| WS_TABSTOP;break;
        case sfnewWinbutton:_cn="BUTTON";_txt="&Ok";dwStyle=BS_PUSHBUTTON | WS_TABSTOP	;break;
        case sfnewWinlist:  _cn="LISTBOX";dwStyle=WS_VSCROLL|LBS_NOINTEGRALHEIGHT;
            nHeight=200; break;
    }

    if (size.x>0)nWidth=size.x;
    if (size.x>0)nHeight=size.y;
    dwStyle|=WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS;
    object2d=CreateControlObject2d(hsp,
                                   _cn,	/* address of registered class name	*/
                                   _txt,	/* address of window text	*/
                                   dwStyle,	/* window style	*/
                                   org.x,	/* horizontal position of window	*/
                                   org.y,	/* vertical position of window	*/
                                   nWidth,	/* window width	*/
                                   nHeight,	/* window height	*/
                                   id,
                                   ExdwStyle);
    SetState(sfbase);
    if (object2d)PostCreateObject2d(object2d);
};

void TSchemeWindow::SetObjectPos(POINT2D p){
    GetObjectOrgSize2d(hsp,object,&delta,NULL);
    lastpoint=delta;
    delta.x=lastpoint.x-delta.x;delta.y=lastpoint.y-delta.y;
    lastpoint.x=0;lastpoint.y=0;
    TrackGroup(p);
};

void TSchemeWindow::TrackGroup(POINT2D& point2d){

    if(__objects || object){
        HOBJ2D hobj2d;
        INT16 count;
        HOBJ2D * items;
        if (__objects){
            hobj2d=__objects;
            count=GetGroupItemsNum2d(hsp,__objects);
            items=new HOBJ2D[count];
            GetGroupItems2d(hsp,__objects,items,count);
        }else {
            hobj2d=object;
            count=1;
            items=new HOBJ2D[1];
            items[0]=hobj2d;
        }
        POINT2D neworg,_delta;


        POINT2D temp;
        neworg=point2d;

        GetObjectOrgSize2d(hsp,hobj2d,&temp,NULL);
        SetRectOrg2d(point2d);
        GetObjectOrgSize2d(hsp,hobj2d,&neworg,NULL);

        if(neworg.x==temp.x && neworg.y==temp.y)return;
        _delta.x-=temp.x-neworg.x;
        _delta.y-=temp.y-neworg.y;

        for(int i=0;i<_class->linkcount;i++){
            HOBJ2D h1=_class->link[i].obj1Id,h2=_class->link[i].obj2Id;
            if (h1==0)h1=_class->link[i].varhandle;
            if (h2==0)h2=_class->link[i].varhandle;
            BOOL p1=0,p2=0;
            for(INT16 j=0;j<count;j++){
                if (h1==items[j])p1=1;
                if (h2==items[j])p2=1;
            }
            if (p1 || p2){
                HOBJ2D h=_class->link[i].handle;
                if (p1 && p2){
                    GetObjectOrgSize2d(hsp,h,&neworg,NULL);
                    neworg.x=neworg.x+_delta.x;neworg.y=neworg.y+_delta.y;
                    SetObjectOrg2d(hsp,h,&neworg);
                }else{
                    if (p1){
                        GetVectorPoint2d(hsp,h,0,&neworg);
                        neworg.x=neworg.x+_delta.x;neworg.y=neworg.y+_delta.y;
                        SetVectorPoint2d(hsp,h,0,&neworg);
                    }
                    if (p2){
                        INT16 n=GetVectorNumPoints2d(hsp,h);
                        GetVectorPoint2d(hsp,h,(INT16)(n-1),&neworg);
                        neworg.x=neworg.x+_delta.x;neworg.y=neworg.y+_delta.y;
                        SetVectorPoint2d(hsp,h,(INT16)(n-1),&neworg);
                    }}
            }
        }
        delete items;
    }else{

        SetRectOrg2d(point2d);

    }
};

void TSchemeWindow::EvLButtonUp(WORD,int xPos,int yPos)
{
    POINT2D point2d;
    point2d.x=xPos;point2d.y=yPos;

    DPtoLP2d(hsp,&point2d,1);
    if (state<sfbase3d){
        switch(state){
            case sftransprent:break;
            case sftransprent1:tmpdata=0;SetState(sftransprent);ReleaseCapture();break;
            case sfLClipBase:SetState(sfbase);//DoneLinkPaste();
                break;
            case sfLbase:break;
            case sfLClipTrack:
            case sfLTrack:ReleaseCapture();UnTrackLine(point2d,TRUE);break;

            case sfrectdrag:{
                SetCursor(0,IDC_ARROW);
                ReleaseCapture();
                SetState(sfrectbase);
                if(__frame)ShowObject2d(hsp,__frame);
                if(__objects2d){
                    HOBJ2D  h= GetObjectByName2d(hsp,__objects2d,"$@#_frames2d_$@#");
                    if(h)ShowObject2d(hsp,h);
                }
                if(_class)_class->UpdateCoordinates();
                AfterMove();
                UpdateScrollers();
            }break;
            case sfwindrag1:
            case sfwindrag:{
                ReleaseCapture();
                SetState(sfwindrag);
            }break;
            case sfrectdown:{
                ReleaseCapture();
                SetState(sfrectbase);
            }break;
            case sfzoom:
            case sfrectbase:break;
            case sfrectselect1:{
                //           Lock2d(hsp);

                DeleteEditFrame2d(hsp);
                ReleaseCapture();
                if(__objects2d==0 && _class)
                {
                    if(__frame){
                        INT16 c=GetGroupItemsNum2d(hsp,__frame);
                        if(c==1){
                            object=GetGroupItem2d(hsp,__objects,0);
                            DeleteFrames();
                            CreateImageFrame();
                        }
                    }
                }
                if (__frame || __objects2d) SetState(sfrectbase);else SetState(sfbase);
                //          UnLock2d(hsp);
            }break;
            case sfrectselect: ReleaseCapture();SetState(sfbase);break;
            case sfOTrack:{
                SetState(sfObase);
                ReleaseCapture();
                CreateImageFrame();
                _class->UpdateCoordinates();
                UpdateScrollers();
                AfterMove();
            }break;
            case sfnames:
            case sfcalcorder:break;
            case sfLDrag:ReleaseCapture();UnDrag(point2d);break;

                // Edit 2d objects
            case sfctrllbtndown2d:
            case sflbuttondown2d:SetState(sfbase);ReleaseCapture();

            case sfbase:if (object2d>=OID_RESERVED)object2d=GetCurrentObject2d(hsp);
                break;

            case sfnewpolyline2:
            case sfnewpolyline1:SetState(sfnewpolyline_1);break;
            case sfnewarc1:;
            case sfnewroundrect1:;
            case sfnewcircle1:;
            case sfnewrect1:;
            case sfnewline1:SetState(sfbase);
                ReleaseCapture();{
                POINT2D s;GetObjectOrgSize2d(hsp,object2d,NULL,&s);
                if (s.y>2 || s.x>2){PostCreateObject2d(object2d);}
                else
                    _DeleteObject2d();
            }
                break;
            case sfdrag:if (object2d<OID_RESERVED){
                    CreateEditFrame2d();
                    EndDragDrop2d(hsp,object2d);
                    UpdateScrollers();
                }else{
                    if (object2d>OID_RCENTER) UpdateFromFrame2d(hsp);
                    object2d=GetCurrentObject2d(hsp);
                }
                ReleaseCapture();
                SetState(sfbase);
                break;
            case sfnewrgngroup:
            case sfnewgroup:
            case sfaddtogroup:break;
            case sfeditline:break;
            case sfeditline_btndn:SetState(sfeditline);break;
            case sfeditline_drag:ReleaseCapture();
                SetCursor(HResource,IDC_POINT);
                SetState(sfeditline);break;
            case sfeditline_add:;
            case sfeditline_delete:
                SetState(sfeditline);
                SetCursor(HResource,IDC_POINT);
                ReleaseCapture();
                break;
            case sfWinwrect:ReleaseCapture();
                CreateControl2d(lastpoint,TPoint(-1,-1),substate);
                break;
            case sfWinwrect1:{
                POINT2D org,size;
                GetObjectOrgSize2d(hsp,OID_FRAME2d,&org,&size);
                DeleteEditFrame2d(hsp);
                ReleaseCapture();
                CreateControl2d(org,size,substate);
            };break;
            case sfnewgrouprect:ReleaseCapture();SetState(substate);break;
            case sfnewgrouprect1:{
                ReleaseCapture();
                POINT2D org,size;
                TRestoreCrd crd(hsp);
                GetObjectOrgSize2d(hsp,OID_FRAME2d,&org,&size);

                object2d = CreateRectGroup2d(org,size,substate==sfnewrgngroup);

                if (object2d)SetCurrentObject(object2d);else DeleteEditFrame2d(hsp);
                SetState(sfbase);
            }break;
            case sfimgaesize2:{
                UpdateFromFrame2d(hsp);
                INT16 count=GetGroupItemsNum2d(hsp,object);
                if (count==2){
                    POINT2D org,size;
                    GetObjectOrgSize2d(hsp,object,&org,&size);
                    HOBJ2D obj=GetGroupItem2d(hsp,object,0);
                    SetObjectOrg2d(hsp,obj,&org);
                    SetObjectSize2d(hsp,obj,&size);
                    INT16 id=_class->GetChildById(object);
                    _class->UpdateCoordinates();
                    if (id>-1){_class->childs[id].flags|=CHF_WASSIZED;}
                }
            }
            case sfimgaesize1:{
                ReleaseCapture();
                SetState(sfbase);
            }break;
            default:SetState(sfbase);
        }}
    else{
        //   up3d
        switch(state){
            case sfeditpoint3d1: PopCrdSystem3d(hsp3d);
                ReleaseCapture();SetState(sfeditpoint3d);
                if(tmpdata)delete tmpdata;tmpdata=NULL;
                break;
            case sfeditpoint3d:break;
            case sfdrag3d:{
                switch(mode3d){
                    case CM_3Drotate:{
                        delete tmpdata;
                        tmpdata=NULL;
                        PopCrdSystem3d(hsp3d);
                        ReleaseCapture();SetState(sfbase3d);
                    };
                    case CM_3Dmove:PopCrdSystem3d(hsp3d);
                    case CM_3Dscale:CreateEditFrame3d();ReleaseCapture();SetState(sfbase3d);break;
                    default:SetState(sfbase3d);break;
                };
            }break;

            case sfscndrag3d:{ ReleaseCapture();
                CAMERA3D cam;
                GetCamera3d(hsp3d,hcamera,&cam);
                cam.poRenderType=camera3d.poRenderType;
                lstrcpy(cam.poRenderName,camera3d.poRenderName);
                camera3d=cam;
                SetCamera3d(hsp3d,hcamera,&camera3d);
                SetState(sfcamera3d);
            }  break;

            case sfscnbtndown3d:SetState(sfcamera3d);break;
            case sflbuttondown3d:SetState(sfbase3d); break;
            case sfcamera3d:
            case sfnewgroup3d:
            case sfaddtogroup3d:break;
            default:SetState(sfbase3d);
        }
    }
};

void TSchemeWindow::PostCreateObject2d(HOBJ2D obj){
    object2d=obj;
    SetCurrentObject(object2d);
    int type=GetObjectType2d(hsp,object2d);
    if (type==otLINE2D){
        HOBJ2D pen=GetPenObject2d(hsp,object2d);
        HOBJ2D brush=GetBrushObject2d(hsp,object2d);
        if ((INT16)pen<1)pen=OID_NONE;if ((INT16)brush<1)brush=OID_NONE;
        SetCurrentPen2d(hsp,pen);
        SetCurrentBrush2d(hsp,brush);
    }
    UpdateScrollers();
    CreateEditFrame2d();
};
void TSchemeWindow::SwithcToBaseMode(){
    SetState(sfbase);
    PressControlButton(CM_SCOBJECT);
    SetState(sfbase);
};
HOBJ2D  TSchemeWindow::GetGRPPen(){
    HOBJ2D obj=GetGroupItem2d(hsp,__frame,0);
    if (obj){
        obj=GetGroupItem2d(hsp,obj,8);
        return GetPenObject2d(hsp,obj);
    }else return CreatePen2d(hsp,PS_DOT,0,RGB(0,255,0),R2_COPYPEN);

};
void TSchemeWindow::CreateLinkFrame(){
    if (link){
        TRestoreCrd crd(hsp);
        if(linkframe)DeleteLinkFrame(0);
        HOBJ2D dib=CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(MARKER_RED));
        POINT2D so,ss; ss.x=(-1);ss.y=(-1);
        POINT2D vs,vo;vs=ss;vo.x=0;vo.y=0;
        INT16 count=2;
        HOBJ2D items[3];
        GetVectorPoint2d(hsp,link,0,&so);
        so.x-=4;so.y-=4;
        POINT2D pp=so;
        items[0]=CreateBitmap2d(hsp,dib,&vo,&vs,&so,&ss);
        INT16 n=GetVectorNumPoints2d(hsp,link);
        GetVectorPoint2d(hsp,link,(INT16)(n-1),&so);
        so.x-=4;so.y-=4;

        dib=CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(MARKER_CYAN));
        if (n==2){
            pp.x=(pp.x+so.x)/2;pp.y=(pp.y+so.y)/2;
            items[2]=CreateBitmap2d(hsp,dib,&vo,&vs,&pp,&ss);count++;
        }

        items[1]=CreateBitmap2d(hsp,dib,&vo,&vs,&so,&ss);


        linkframe=CreateGroup2d(hsp,items,count);
        SetAttribute2d(hsp,linkframe,0x04,ATTRSET);
    }
};
void TSchemeWindow::DeleteLinkFrame(HSP2D _hsp){
    if (!_hsp)_hsp=hsp;
    if (linkframe){
        FullDeleteObject2d(_hsp,linkframe);
        if (hsp==_hsp)linkframe=0;
    }
};

void TSchemeWindow::CreateGRPFrame(HOBJ2D h,HOBJ2D dib,POINT2D& _org){
    AddGroupItem2d(hsp,__objects,h);
    //	 HOBJ2D items[9];
    POINT2D point[5];
    POINT2D _size;
    GetObjectOrgSize2d(hsp,h,NULL,&_size);
    for(INT16 i=0;i<5;i++)point[i]=_org;
    point[1].x+=_size.x;point[2].x+=_size.x;
    point[2].y+=_size.y;point[3].y+=_size.y;

    AddGroupItem2d(hsp,__frame,CreatePolyline2d(hsp,0,dib , point,5));

};

void TSchemeWindow::EvRButtonDown(WORD, int xPos, int yPos)
{
    POINT2D point2d;
    POINT point;
    switch(state)
    {
        case sfwindrag1:
        case sfnewarc1:
        case sfWinwrect1:
        case sfnewroundrect1:
        case sfnewcircle1:
        case sfnewpolyline2:
        case sfnewpolyline1:
        case sfnewrect1:
        case sfnewline1:
        case sfeditline_drag:
        case sfLTrack:
        case sfOTrack:
            return;
    }

    {
        POINT _point;
        GetCursorPos(&_point);
        ScreenToClient(HWindow,&_point);
        point2d.x=_point.x;
        point2d.y=_point.y;
        point=_point;
    }
    if (state==sfzoom)
    {
        Zoom(xPos,yPos,FALSE);
        return ;
    }

    if (state<sfbase3d)
    {
        DPtoLP2d(hsp,&point2d,1);
        _mousepos=point2d;
        lastpoint=point2d;
        lastclickvalid=1;

        if(state==sfrectbase)
        {
            HOBJ2D _obj2d = GetObjectFromPoint2d(hsp,&point2d);
            if(_obj2d == __frame || _obj2d == __objects2d)
            {
                ClientToScreen(HWindow,&point);

                HMENU hMenu = (__frame) ? ScLoadPopup(MENU_GROUPIMAGE) : CreatePopupMenu();

                if(_object && __frame)
                    ScAppendMenu(hMenu, MF_ENABLED , CM_SETVARrect ,IDS_MENU65);

                if(__objects2d)
                    ScAppendMenu(hMenu, MF_ENABLED , CM_STATEgroup ,IDS_MENU49);

                AddGotoMenu(hMenu,NULL);
                AddCopyCutMenu(hMenu);
                TSTRATUMMENU sm;
                sm.hWindow=HWindow;
                sm.hSpace=hsp;
                sm.hObject=__objects;
                sm.data1=(UINT32)_class;
                sm.data2=(UINT32)_object;
                sm.hType=SCMENU_GRPIMAJE;
                sm.hMenu=hMenu;
                TrackPopupMenu(point,&sm,TRUE);
                return;
            }
            else
            {
                SetState(sfbase);
            }
        }

        if (state!=sfbase)
        {
            BOOL lu=LocalUndo(TRUE) ;
            SetText(&point2d,0);
            if (lu)
            {
                SetState(sfbase);
                return;
            }
        }

        PressButton0(point2d,0,0);
        switch (state)
        {
            case  sfbase:
            {
                BOOL ms=1;
                if (object)
                {
                    ObjectMenu(object,point);
                    ms=0;
                };
                HOBJ2D obj = (object2d>=OID_RESERVED) ? GetCurrentObject2d(hsp) : object2d;

                if (obj && ms)
                {
                    _ObjectMenu(point,obj,TRUE);
                    ms=0;
                }

                if (state==sfbase3d && object3d>0 && object3d<OID_RESERVED && ms)
                {
                    _ObjectMenu(point,object3d,TRUE);
                    ms=0;
                }
                if(ms)
                    ObjectMenu(0,point);
            }break;

            case sfnames:
            case sfHELP:
            case sfcalcorder:
                SetState(sfbase);
                return;
        }
    }
    else
    {
        if (state!=sfbase3d)
        {
            BOOL lu=LocalUndo(TRUE) ;
        }
        DPtoLP2d(hsp,&point2d,1);
        PressButton3d(point2d,0,0);

        switch(state)
        {
            default:Object3dMenu(point);
        }
    }
};

void TSchemeWindow::Object3dMenu(POINT point){
    TSTRATUMMENU sm;
    sm.hWindow=HWindow;
    sm.hSpace=(HSP2D)hsp3d;
    sm.hObject=object3d;
    sm.data1=(UINT32)_class;
    sm.data2=(UINT32)_object;


    HMENU p[10];
    p[0]=CreatePopupMenu();
    if (object3d){
        sm.hType=SCMENU_OBJ3D;
        INT16 type=GetObjectType3d(hsp3d,object3d);
        ScAppendMenu(p[0], MF_ENABLED,CM_PROPERTIES,IDS_MENU29  );
        p[1]=CreatePopupMenu();
        Create3dViewMenu(p+1);
        ScAppendMenu(p[0], MF_ENABLED|MF_POPUP,(UINT)p[1],IDS_MENU59);

        AppendMenu(p[0], MF_SEPARATOR,0,"");
        ScAppendMenu(p[0], MF_ENABLED|MF_STRING|(mode3d==CM_3Dmove)?MF_CHECKED:MF_UNCHECKED,CM_3Dmove,IDS_MENU32);
        ScAppendMenu(p[0], MF_ENABLED|MF_STRING|(mode3d==CM_3Dscale)?MF_CHECKED:MF_UNCHECKED,CM_3Dscale,IDS_MENU33);
        ScAppendMenu(p[0], MF_ENABLED|MF_STRING|(mode3d==CM_3Drotate)?MF_CHECKED:MF_UNCHECKED,CM_3Drotate,IDS_MENU34);

        {
            AppendMenu(p[0], MF_SEPARATOR,0,"");
            HMENU hm=CreatePopupMenu();
            char s[64];
            if(!GetComboStr(CM_3Daxis,s,sizeof(s)))lstrcpy(s,"Y");
            for(int i=0;i<sizeof(axis_str)/sizeof(axis_str[0]);i++)
            { BOOL sel=lstrcmpi(axis_str[i],s)==0;
                int f=MF_ENABLED|MF_STRING|((sel)?MF_CHECKED:MF_UNCHECKED);
                if((i==4) || (i==1))f|=MF_MENUBARBREAK;
                AppendMenu(hm, f,CM_AXIS3Dmode+i,axis_str[i]);
            };
            AppendMenu(p[0], MF_POPUP,(UINT)hm,"Locked Axis");
        }
        BOOL bs=TRUE;
        if (type==otGROUP3D){
            if(bs){AppendMenu(p[0], MF_SEPARATOR,0,"");bs=FALSE;}
            ScAppendMenu(p[0], MF_ENABLED,CM_UNGROUP,IDS_MENU30);
        }

        if(GetObjectParent3d(hsp3d,object3d)){
            if(bs){AppendMenu(p[0], MF_SEPARATOR,0,"");bs=FALSE;}
            ScAppendMenu(p[0], MF_ENABLED,CM_DELFROMGROUP,IDS_MENU31);
        }
        if(addmode3d){

        }else{
            ScAppendMenu(p[0], MF_ENABLED|MF_STRING,CM_SAVEOBJECTAS,IDS_MENU46);

            if(type==otOBJECT3D){
                if(bs){AppendMenu(p[0], MF_SEPARATOR,0,"");bs=FALSE;}
                ScAppendMenu(p[0], MF_ENABLED|MF_STRING,CM_MODIFY3D,IDS_MENU60);
            }
        }
        SetDefaultMenuItem(p[0],CM_PROPERTIES);
    }else {Create3dViewMenu(p);
        sm.hType=SCMENU_VIEW3D;
    }
    AddCopyCutMenu(p[0]);
    sm.hMenu=p[0];
    ClientToScreen(HWindow,&point);
    TrackPopupMenu(point,&sm);
};

void   TSchemeWindow::GetTargetPoint3d(POINT3D&p)
{
    ::GetTargetPoint3d(camera3d,p);
};

void TSchemeWindow::DeleteRotateObj3d(){
    if(hrotate){
        FullDeleteObject2d(hsp,hrotate);
        hrotate=0;
    }
};
HSP2D LoadSpaceFromResource(HWND HWindow,char * text);
HOBJ2D TSchemeWindow::MergeObject2d(char*objects,char*objname,POINT2D _org2d){
    HOBJ2D obj=0;
    HSP2D _hsp=LoadSpaceFromResource(0,objects);
    if(hsp){
        HOBJ2D _h=GetObjectByName2d(_hsp,0,objname);
        HGLOBAL hg=CopyToMemory2d(_hsp,_h,NULL);
        DeleteSpace2d(_hsp);
        obj=PasteFromMemory2d(hsp,hg,&_org2d,PFC_MOVEOBJECT);
        GlobalFree(hg);
    }
    return obj;
};


HOBJ2D TSchemeWindow::CreateRotateObj3d(){
    POINT3D o3d;
    DeleteRotateObj3d();
    // GetTargetPoint3d(o3d);
    // LP3dtoLP2d(hsp,object2d,&_org2d,&o3d,1);
    POINT2D vo;
    vo.x=view_org.x+view_size.x/2;
    vo.y=view_org.y+view_size.y/2;
    hrotate=MergeObject2d("rotator3d","ROTOR",vo);
    POINT2D o,s;
    double size=min(view_size.x,view_size.y)*0.8;
    s.x=size;s.y=size;
    SetObjectSize2d(hsp,hrotate,&s);
    vo.x-=s.x/2;
    vo.y-=s.y/2;
    SetObjectOrg2d(hsp,hrotate,&vo);
    HOBJ2D ho=GetObjectByName2d(hsp,hrotate,"tmp");
    FullDeleteObject2d(hsp,ho);
    return hrotate;
};
int TSchemeWindow::GetRotate3dCursor(POINT2D&point2d){
    int  _submode3d=0;
    HOBJ2D prim;
    HOBJ2D _obj2d=GetObjectFromPoint2d (hsp,&point2d,&prim);
    if (_obj2d==hrotate){
        char s[64];
        GetObjectName2d(hsp,prim,s,sizeof(s));

        if (!lstrcmp(s,"R"))_submode3d=1;
        if (!lstrcmp(s,"L"))_submode3d=1;
        if (!lstrcmp(s,"U"))_submode3d=2;
        if (!lstrcmp(s,"D"))_submode3d=2;
        if (!lstrcmp(s,"Z"))_submode3d=3;

        if (!lstrcmp(s,"S"))_submode3d=4;
        if (!lstrcmp(s,"F"))_submode3d=5;
        if (!lstrcmp(s,"1"))_submode3d=6;
        if (!lstrcmp(s,"P"))_submode3d=7;//pan

    }
    return _submode3d;
};
void TSchemeWindow::DoneAddMode(BOOL newmode){
    switch(addmode3d){
        case 0:return;
        case AM3D_VERTEXES: SetAttribute3d(hsp3d,htmpframe,STATE_POINTS,ATTRRESET);
            //CreateEditFrame3d();
            SetState(sfbase3d);htmpframe=0;
            break;
        case AM3D_PRIMITIVES:break;
        case AM3D_TEXTURE:   {
            {;DeleteObject2d((HSP2D)hsp3d,htmpframe);htmpframe=0;
            }
        }break;
    }
    int _addmode3d=addmode3d;
    addmode3d=0;
    switch(_addmode3d){
        case AM3D_VERTEXES:
        case AM3D_PRIMITIVES:
        case AM3D_TEXTURE: hmain3d=0;
            if(!newmode)DestroyToolPopup(CM_MODIFY3D);
    }

};

BOOL TSchemeWindow::PressButton3d(POINT2D& point2d,UINT16 flags,UINT modKeys){
    if(flags&1){
        if(addmode3d == AM3D_PRIMITIVES){
            INT16 primitive=-1;
            HOBJ2D obj =
                    GetObjectItem3dbyPoint2d( hsp,object2d,object3d,&point2d,&primitive);
            if(primitive>-1){
                SCToolPopupVar(CM_MODIFY3D,50,1,(void*)primitive);
            }
        }
    }

    HOBJ2D _object3d;
    _object3d=_GetObject3dFromPoint(point2d);
    if(object3d && (_object3d!=object3d)){
        switch(addmode3d){
            case AM3D_TEXTURE:
                if(_object3d==htmpframe){_object3d=object3d;break;}
                if((htmpframe!=_object3d)&&(hmain3d!=_object3d))
                    DoneAddMode();
                break;
            default:DoneAddMode();
        }

    }
    object3d=_object3d;
    if (object3d){
        //		  object3d=_object3d;
        SetCurrentObject3d(hsp3d,object3d);
        CreateEditFrame3d();
    }else {
        _DeleteEditFrame3d();
    }

    if (flags&1){
        SetState(sflbuttondown3d);
        lastpoint=point2d;
    }
    return FALSE;
};

void TSchemeWindow::SetState(int _state)
{
    if(state>=sfbase3d)
    {
        switch(_state)
        {
            case sfcamera3d:PressControlButton(CM_3Dscnrotate);break;
            default: if(state==sfcamera3d)
                {
                    if(_state!=sfscnbtndown3d)
                    {
                        DeleteRotateObj3d();PressControlButton(CM_STATEbase);
                    }
                }
        }
    }
    if((_state==sfbase3d)&&(state!=sfbase)&&(mode3d==0))
        PressControlButton(CM_STATEbase);

    if(state>=sfbase3d && _state<sfbase3d)
    {
        _DeleteEditFrame3d();
        DeleteRotateObj3d();
    }

    //---------------------------------------------------
    //          Редактирование линии
    //---------------------------------------------------
    if ((_state<sfeditline_start|| sfeditline_end<_state)&&
        (state>=sfeditline_start&& sfeditline_end>=state))
    {
        PressControlButton(CM_EDITPOINTS,0);
        SetAttribute2d(hsp,object2d,2,ATTRRESET);
    };
    if ((state==sfzoom)||(state==sfHELP))PressControlButton(CM_ZOOM,0);
    if ((_state<sfRECTstart || _state>sfRECTend ) && (__frame || __objects2d)) DeleteFrames();
    if (state==sfnames && _state!=sfnames)Un_CalcOrder();
    if (state==sfcalcorder && _state!=sfcalcorder)Un_CalcOrder();
    if (oldlink>0 && link!=oldlink && oldlink)
    {
        DeleteLinkFrame();
        oldlink=0;
    }
    if (link>0 && link!=oldlink)
    {
        CreateLinkFrame();
        oldlink=link;
    }
    if((state>=sfLClipBase && state<=sfLClipTrack)&&((_state<sfLClipBase)||(_state>sfLClipTrack)))
    {
        DoneLinkPaste();
    }
    if (_state==sfaddtogroup && !(state ==sfnewgroup|| state==sfnewrgngroup))
    {
        SetCursor(HResource,IDC_ADDTOGROUP);
        PressControlButton(CM_STATEgroup);
    }

    if (_state==sfbase3d)
    {
        SetCursor(0,IDC_ARROW);
    }

    if (_state==sfbase)
    {
        SetCursor(0,IDC_ARROW);
        substate=0;
        if (_state!=state)
            PressControlButton(CM_STATEbase);
        SetText(NULL,0);
    };
    state=_state;
};

void TSchemeWindow::OpenClassDialog(HOBJ2D obj)
{
    if (obj)
    {
        int type=WhatObject(obj);
        if(type==2)
            VarPlaceDialog(HWindow,hsp,obj,_class);
        else
        {
            TClass *_c=NULL;TObject* _o=NULL;
            if (_object)
                _o=_object->_GetObject(obj);
            else
            {
                INT16 i=_class->GetChildById(obj);
                if (i>-1)_c=_class->childs[i].type->GetClass();
            }
            if (_c || _o)
            {
                if(lstrcmp(_c?_c->name:_o->_class->name,"EMPTY_CLASS")==0)
                {
                    MSGBOX(HWindow,MAKEINTRESOURCE(IDS_str225),_MSG_INFO,MB_ICONINFORMATION|MB_OK);
                }
                else
                    ClassDialog(HWindow,_c,_o,NULL,NULL,_class,obj);
            }
        }
    }
};

// eng
void CorrectClassMenu(HMENU  p,PClass _class)
{
    if(!(_class->flags&CF_SCHEME))
    {
        char s[256];
        LoadString(HResource,IDS_MENU61,s,sizeof(s));
        ModifyMenu(p,2,MF_BYPOSITION,GetMenuItemID(p,2),s);
    }
};

HMENU TSchemeWindow::CreateObjectMenu(PClass _class,PObject object){
    HMENU  p=0;
    if (_class){
        p = ScLoadPopup(MENU_IMAGE);
        CorrectClassMenu(p,_class);
        if (_class->varcount){
            HMENU  p1=CreatePopupMenu();
            ScAppendMenu(p,MF_POPUP,(int)p1,IDS_MENU5);
            for(INT16 i=0;i<_class->varcount;i++){
                if(!(_class->vars[i].flags&VF_LOCAL)){  //local
                    if (i>39)break;
                    char s[128];
                    lstrcpy(s,_class->vars[i].name);
                    lstrcat(s," : ");
                    lstrcat(s,_class->vars[i].classType->GetClassName());
                    if (object){
                        char ss[258];
                        char*ps;
                        ps=GetObjectVarA(object,i,ss);
                        if (ps==0)ps=ss;
                        lstrcat(s," = ");
                        int l=lstrlen(s);
                        int l1=lstrlen(ps);
                        lstrcpyn(s+l,ps,80-(l));
                        if ((l+l1)>80)lstrcat(s,"...");
                    }
                    AppendMenu(p1,MF_ENABLED,CM_VARITEM+i,s);
                } //local
            }
        }

        ScAppendMenu(p,MF_ENABLED,CM_OBJECTVARSAVE,IDS_MENU1);
        ScAppendMenu(p,MF_ENABLED,CM_OBJECTVARLOAD,IDS_MENU2);
        ScAppendMenu(p,MF_ENABLED,CM_OBJECTVARDEF,IDS_MENU3);
        if ((_class->GetClassListInfo()->flags&CLF_PROJECT)==0 || _class->helpfile){
            AppendMenu(p, MF_SEPARATOR,0,"");
            ScAppendMenu(p,MF_ENABLED,CM_HELPONIMAGE,IDS_MENU4);
        }
        AddGotoMenu(p,NULL);
        AddCopyCutMenu(p);
    }else{
        p = ScLoadPopup(MENU_PAGE);
        HOBJ2D bk=GetBkBrush2d(hsp);
        if(bk){
            bk=GetBrushDibIndex2d(hsp,bk);
            if(bk){
                ScAppendMenu(p,MF_ENABLED,CM_COPYBK,IDS_MENU70);
                ScAppendMenu(p,MF_ENABLED,CM_SAVEBK,IDS_MENU71);
                AppendMenu(p, MF_SEPARATOR,0,"");
            }
        }


        if (hardmode==SCHEME_EDITOR){

            char s[64];
            LoadString(HResource,IDS_MENU72,s,sizeof(s));
            InsertMenu(p,1,MF_BYPOSITION,CM_PAGEIMAGE_PROP,s);

            ScAppendMenu(p,MF_ENABLED,CM_INSERTOBJECT,IDS_MENU6);
            ScAppendMenu(p,MF_ENABLED,CM_CREATENEWOBJECT,IDS_MENU7);
            ScAppendMenu(p,MF_ENABLED,CM_NEWVARPOINT,IDS_MENU8);

            AppendMenu(p, MF_SEPARATOR,0,"");
        }
        ScAppendMenu(p,MF_ENABLED,CM_EDITPASTE,IDS_MENU9);
        ScAppendMenu(p,MF_ENABLED,CM_IMPORTOBJ,IDS_MENU10);
    }
    return p;
};

void TSchemeWindow::ObjectMenu(HOBJ2D obj, POINT point)
{
    HMENU  p=NULL;
    TSTRATUMMENU sm;
    sm.hWindow=HWindow;
    sm.hSpace=hsp;
    sm.data1=(UINT32)_class;
    sm.data2=(UINT32)_object;

    if (object>0 && obj>0)
    {
        int type=WhatObject(obj);
        if (type==2)
        {
            // Var Place Menu
            p = ScLoadPopup(MENU_VARPLACE);
            AddCopyCutMenu(p);
            sm.hType=SCMENU_VARPLACE;
        }
        if (type==1)
        {
            int id=_class->GetChildById(obj);
            PClass _cl=(id<0?NULL:_class->childs[id].type->GetClass());
            p=CreateObjectMenu(_cl,_object?_object->_GetObject(obj):NULL);
            sm.hType=SCMENU_IMAJE;
        }
        sm.hObject=obj;
    }else	p=CreateObjectMenu(NULL,NULL);
    if (link>0){
        sm.hType=SCMENU_LINK;
        sm.hObject=link;
        INT16 num;
        plink=_class->GetLinkById(link,&num);
        if (plink){
            p=ScLoadPopup(MENU_LINK);
            if (plink->count){
                AppendMenu(p, MF_SEPARATOR,0,"");
                HMENU  p1=CreatePopupMenu();
                ScAppendMenu(p,MF_POPUP,(int)p1,IDS_MENU16);
                for(INT16 i=0;i<plink->count;i++){
                    if (i>39)break;
                    char s[128];
                    wsprintf(s,"%s\t%s",plink->items[i].name1,plink->items[i].name2);
                    AppendMenu(p1,MF_ENABLED,CM_LINKITEM+i,s);
                }
            }

            AddCopyCutMenu(p);
        }}

    if (p){
        sm.hMenu=p;
        SetDefaultMenuItem(p,CM_PROPERTIES);
        ClientToScreen(HWindow,&point);
        TrackPopupMenu(point,&sm,TRUE);
    }
};
BOOL TSchemeWindow::GetBallonStr(BALLONSTRUCT *bs){
    if ((state==sfbase) && (hardmode==SCHEME_EDITOR))
    {
        POINT2D point2d;point2d.x=bs->bsPoint.x;point2d.y=bs->bsPoint.y;
        DPtoLP2d(hsp,&point2d,1);
        HOBJ2D _obj2d=GetObjectFromPoint2d (hsp,&point2d);
        if (_obj2d /*&& _obj2d!=object*/){
            int type=WhatObject(_obj2d);
            switch(type){
                case 1:{
                    char cmd[256];
                    LoadString(HResource,IDS_str113,cmd,sizeof(cmd));
                    INT16 id=_class->GetChildById(_obj2d);
                    if(_class->childs[id].absentclass){
                        wsprintf(bs->bsText,cmd ,_class->childs[id].absentclass);
                        lstrcat(bs->bsText," [ NOT FOUND ]");
                    }else{
                        PClass _c=_class->childs[id].type->GetClass();
                        wsprintf(bs->bsText,cmd ,_c->name);
                        if(_c->flags&CF_SCHEME){
                            lstrcat(bs->bsText," *");
                        }
                    }
                }return TRUE;
                case 2:LoadString(HResource,IDS_str191,bs->bsText,64);
                    return TRUE;
                case 3:
                    LoadString(HResource,IDS_str114,bs->bsText,32);
                    INT16 num;
                    PLinkInfo plink=_class->GetLinkById(_obj2d,&num);
                    if(plink->flags&LF_DISABLED){
                        lstrcat(bs->bsText," > Disabled < ");
                    }else{
                        int i=0;
                        if(plink->count){
                            do{
                                char s[128];
                                wsprintf(s," (%s•%s)",plink->items[i].name1,plink->items[i].name2);
                                i++;
                                if((lstrlen(bs->bsText)+lstrlen(s))>=bs->bsSize){
                                    if((lstrlen(bs->bsText)+3)<bs->bsSize)lstrcat(bs->bsText,"...");
                                    break;
                                }else {lstrcat(bs->bsText,s);}
                            }while(i<plink->count);};
                    }
                    return TRUE;
            }
        }}
    switch(state){
        case sfcamera3d:{
            POINT2D point2d;point2d.x=bs->bsPoint.x;point2d.y=bs->bsPoint.y;
            DPtoLP2d(hsp,&point2d,1);
            int sm3d=GetRotate3dCursor(point2d);
            if(sm3d){
                char * ps=NULL;
                LoadString(HResource,IDS_3DCURSOR+sm3d-1,bs->bsText,64);
                return TRUE;
            }
        }break;
        case sfwindrag1:{
            POINT2D pp;
            GetOrgSpace2d(hsp,&pp);
            sprintf(bs->bsText,"%g : %g",pp.x,pp.y);
            return TRUE;
        }break;
        case sfeditline_btndn:
        case sfeditline_drag:
        {
            POINT2D p;
            if(GetVectorPoint2d(hsp,object2d,editpoint,&p)){
                sprintf(bs->bsText,"%g , %g",p.x,p.y);
                return TRUE;
            }
        }break;
    }
    return FALSE;
}

void TSchemeWindow::EvMouseMove(WORD modKeys,int xPos,int yPos)
{
    lastclickvalid=0;
    POINT2D point2d;point2d.x=xPos;point2d.y=yPos;
    DPtoLP2d(hsp,&point2d,1);
    if (state<sfbase3d)
    {
        POINT2D _point2d=point2d;
        Snap(_point2d);
        switch(state)
        {
            case sfbase:
            {
                if (object2d)
                {
                    HOBJ2D o=GetFrameCursor2d(hsp,&point2d);
                    if (o)
                    {
                        if (RCenter()&&(o!=OID_RCENTER))
                            SetCursor(HResource,IDC_ROTATE);
                        else
                            SetCursor(HResource,IDC_RESIZE);
                    }
                    else
                        SetCursor(0,IDC_ARROW);
                }
                if (link)
                {
                    editpoint=GetLinePointFromPoint2d(hsp,link,&point2d);
                    if(editpoint>-1)
                        SetCursor(HResource,IDC_DRAGPOINT);
                    else
                        SetCursor(0,IDC_ARROW);
                };
                if (object && _class)
                {
                    INT16 id=_class->GetChildById(object);
                    if (id>-1)
                    {
                        if ((_class->childs[id].type->GetClass()->flags)&CF_SIZEABLE)
                        {
                            HOBJ2D o=GetFrameCursor2d(hsp,&point2d);
                            if (o)
                                SetCursor(HResource,IDC_RESIZE);
                            else
                                SetCursor(0,IDC_ARROW);
                        }
                    }
                }break;
            }
            case sftransprent1:
            {
                FRAMEPROP fp;
                if (GetFrameProp2d(hsp,object2d,&fp))
                {
                    double ns=(BYTE)tmpdata;
                    ns+=((double)(xPos-lastpoint.x))/10.0;
                    if(ns<0)ns=0;
                    if(ns>255)ns=255;
                    BYTE _ns=ns;
                    if(_ns!=fp.sense)
                    {
                        fp.sense=_ns;
                        SetFrameProp2d(hsp,object2d,&fp);
                    }
                }
            }break;
            case sftransprent:break;
            case sfwindrag:break;
            case sfwindrag1:
            {
                long x,y,nx,ny;
                x=hscroll->GetPos();
                y=vscroll->GetPos();
                nx=x-(xPos-lastpoint.x);
                ny=y-(yPos-lastpoint.y);
                ny=vscroll->ValidValue(ny);
                nx=hscroll->ValidValue(nx);

                if((nx!=x)||(ny!=y))
                {
                    hscroll->SetPos(nx,0);
                    vscroll->SetPos(ny,0);
                    ScrollWindow(HWindow,(int)(x-nx),(int)(y-ny),NULL,NULL);
                    POINT2D pp;
                    pp.x=nx;
                    pp.y=ny;
                    SetOrgSpace2d(hsp,&pp);
                    UpdateWindow(HWindow);
                    lastpoint.x=xPos;
                    lastpoint.y=yPos;
                }
            }break;
            case sfctrllbtndown2d:
                if((object2d<OID_RESERVED)&&
                   ((abs(lastpoint.x-_point2d.x)>1) || (abs(lastpoint.y-_point2d.y)>1)))
                {
                    Cm_DuplicateObj(&_point2d);
                }
                //no break
            case sflbuttondown2d:
                if ((abs(lastpoint.x-_point2d.x)>1) || (abs(lastpoint.y-_point2d.y)>1))
                {
                    int _state=state;
                    SetState(sfdrag);
                    if (object2d<OID_RESERVED)
                    {
                        DeleteFrame();
                        SetDragCursor(_state==sfctrllbtndown2d);
                    }
                    else
                    {
                        if (RCenter()&&(object2d!=OID_RCENTER))
                            SetCursor(HResource,IDC_ROTATE);
                        else
                            SetCursor(HResource,IDC_RESIZE);
                    }
                    BeginDragDrop2d(hsp,object2d);
                }
                else
                    break;
            case sfdrag:
            {
                POINT2D tp=point2d;
                tp.x-=delta.x;tp.y-=delta.y;
                Snap(tp);
                SetObjectOrg2d( hsp,object2d,&tp);
            }
                break;
            case sfnewgrouprect:{
                lastpoint=point2d;
                SetCurrentObject2d(hsp,OID_NONE);
                ::CreateEditFrame2d(hsp,&sizer,FRAME_NOOBJECT|FRAME_HASLINES|FRAME_DASHED);
                SetObjectOrg2d(hsp,OID_FRAME2d1,&lastpoint);
                lastpoint.x+=2;lastpoint.y+=2;
                SetObjectOrg2d(hsp, OID_FRAME2d3,&lastpoint);
                SetState(sfnewgrouprect1);
            };
            case sfnewgrouprect1:
                SetObjectOrg2d(hsp,OID_FRAME2d3,&point2d);break;

            case sfnewline1:SetVectorPoint2d(hsp,object2d,editpoint,&_point2d);	break;
            case sfWinwrect:{
                SetCurrentObject2d(hsp,OID_NONE);
                ::CreateEditFrame2d(hsp,&sizer,FRAME_NOOBJECT|FRAME_HASLINES|FRAME_DASHED);
                SetObjectOrg2d(hsp,OID_FRAME2d1,&lastpoint);
                lastpoint.x+=2;lastpoint.y+=2;
                SetObjectOrg2d(hsp, OID_FRAME2d3,&lastpoint);
                SetState(sfWinwrect1);
            };
            case sfWinwrect1:
                SetObjectOrg2d(hsp,OID_FRAME2d3,&point2d);break;
            case sfnewarc1:{
                double dx=point2d.x-lastpoint.x;
                double dy=point2d.y-lastpoint.y;
                double angle=fabs(GetAngleByXY2d(dx,dy)-GetAngleByXY2d(0,-1));

                dx=_point2d.x-lastpoint.x;
                dy=_point2d.y-lastpoint.y;

                double r=sqrt((dx*dx)+(dy*dy));
                if(r>0){
                    INT16 steps=max((angle*6.0*sqrt(r)),2);
                    POINT2D * p=new POINT2D[steps+1];
                    double d=angle/(double)steps;
                    p[0]=lastpoint;
                    for(INT16 i=0;i<steps;i++){
                        double a=d*i-M_PI/2.0;
                        p[i+1].x=r*cos(a)+lastpoint.x;
                        p[i+1].y=r*sin(a)+lastpoint.y;
                    }SetVectorPoints2d(hsp,object2d,p,steps+1);
                    delete p;
                }
            }break;
            case sfnewrect1:{POINT2D pp[5];GetVectorPoints2d(hsp,object2d,pp,5);
                pp[1].x=pp[2].x=_point2d.x;
                pp[2].y=pp[3].y=(modKeys&MK_SHIFT)?((_point2d.x-lastpoint.x)+pp[0].y):_point2d.y;
                SetVectorPoints2d(hsp,object2d,pp,5);
                break;}
            case sfnewcircle1:{
                INT16 numpoints=GetVectorNumPoints2d(hsp,object2d);
                POINT2D * p=new POINT2D[numpoints];
                POINT2D r;r.x=abs(_point2d.x-lastpoint.x);r.y=abs(_point2d.y-lastpoint.y);
                if(modKeys&MK_SHIFT)r.y=r.x;
                double d=(M_PI*2.0)/((double)(numpoints-1));
                for(INT16 i=0;i<numpoints;i++){
                    double a=d*i;
                    p[i].x=r.x*cos(a)+lastpoint.x;
                    p[i].y=r.y*sin(a)+lastpoint.y;
                }SetVectorPoints2d(hsp,object2d,p,numpoints);
                delete p;
                break; }
            case sfnewroundrect1:{
                if(modKeys&MK_SHIFT){_point2d.y=(_point2d.x-lastpoint.x)+lastpoint.y;}

                gr_float dx=fabs(_point2d.x-lastpoint.x),
                        dy=fabs(_point2d.y-lastpoint.y);
                dx=min(dx,dy);
                gr_float r=dx/5;
                gr_float x1=fabs(_point2d.x-lastpoint.x)-r; // fabs
                gr_float y1=fabs(_point2d.y-lastpoint.y)-r; // fabs
                INT16 pn=(INT16)(2+sqrt(r));
                INT16 nn=(INT16)(4*pn+5);
                POINT2D *pp=new POINT2D[nn];
                double da=M_PI_2/(pn);
                INT16 i;
                for(i=0;i<=pn;i++){
                    double a=da*i;
                    double x=r*cos(a),y=r*sin(a);
                    gr_float ix=x,iy=y;
                    pp[i]=TPoint(r-ix,r-iy);
                    pp[pn+i+1]=TPoint(x1+iy,r-ix);
                    pp[2*pn+i+2]=TPoint(x1+ix,y1+iy);
                    pp[3*pn+i+3]=TPoint(r-iy,y1+ix);
                }
                pp[0]=TPoint(0,r);
                pp[pn]=TPoint(r,0);
                pp[  pn+1]=TPoint(x1,0);
                pp[2*pn+1]=TPoint(x1+r,r);
                pp[2*pn+2]=TPoint(x1+r,y1);
                pp[3*pn+2]=TPoint(x1,y1+r);
                pp[3*pn+3]=TPoint(r,y1+r);
                pp[4*pn+3]=TPoint(0,y1);
                pp[nn-1]=pp[0];
                gr_float sx=sign(_point2d.x-lastpoint.x);
                gr_float sy=sign(_point2d.y-lastpoint.y);
                for(i=0;i<nn;i++){

                    if (sx)pp[i].x+=lastpoint.x;
                    else pp[i].x=lastpoint.x-pp[i].x;
                    if (sy)pp[i].y+=lastpoint.y;
                    else pp[i].y=lastpoint.y-pp[i].y;

                }
                SetVectorPoints2d(hsp,object2d,pp,nn);
                delete pp;
                break;}
            case sfLdown1:{
                SetCapture();
                SetState(sfLTrack);
                POINT2D pp[2];
                pp[0]=lastpoint;
                pp[1]=_point2d;
                link=CreatePolyline2d(hsp,
                                      CreatePen2d(hsp,PS_SOLID,0 ,RGB(255,0,0), R2_COPYPEN)
                                      ,0 ,pp,2);
                SetObjectLayer2d(hsp,link,24);
                editpoint=1;
            }break;
            case sfLClipTrack:
            case sfLTrack:{
                SetVectorPoint2d(hsp,link,editpoint,&_point2d);
            }break;
            case sfLClipdown1:{
                SetCapture();
                SetState(sfLClipTrack);
                POINT2D pp[2];
                pp[0]=lastpoint;
                pp[1]=_point2d;
                LINKCLIP*lc=(LINKCLIP*)tmpdata;
                link=CreatePolyline2d(hsp,
                                      CreatePen2d(hsp,PS_SOLID,lc->width ,lc->color, R2_COPYPEN)
                                      ,0 ,pp,2);
                SetObjectLayer2d(hsp,link,24);
                editpoint=1;
            };break;
            case sfrectdown:{
                POINT2D p=GetRectOrg2d();

                delta.x=lastpoint.x-p.x;
                delta.y=lastpoint.y-p.y;

                SetState(sfrectdrag);
                SetDragCursor(modKeys & MK_CONTROL);

                if ( modKeys & MK_CONTROL ){
                    POINT2D neworg;
                    neworg.x=point2d.x-delta.x;neworg.y=point2d.y-delta.y;
                    Cm_DuplicateObj(&neworg);
                }

                HideObject2d(hsp,__frame);
                if(__objects2d){
                    HOBJ2D  h= GetObjectByName2d(hsp,__objects2d,"$@#_frames2d_$@#");
                    if(h)HideObject2d(hsp,h);
                }
            };

            case sfrectdrag:{
                POINT2D neworg;
                neworg.x=point2d.x-delta.x;neworg.y=point2d.y-delta.y;
                Snap(neworg);
                TrackGroup(neworg);

            }break;

            case sfrectselect1:
                // Lock2d(hsp);
                SetObjectOrg2d(hsp,OID_FRAME2d3,&point2d);
                SelectRectProc();
                // UnLock2d(hsp);
                break;
            case sfrectselect:{
                if (fabs(point2d.x-lastpoint.x)>1 &&
                    fabs(point2d.y-lastpoint.y)>1){
                    SetCurrentObject2d(hsp,0);
                    ::CreateEditFrame2d(hsp,&sizer,FRAME_NOOBJECT|FRAME_HASLINES|FRAME_DASHED);
                    SetObjectOrg2d(hsp,OID_FRAME2d1,&lastpoint);
                    lastpoint.x+=2;lastpoint.y+=2;
                    SetObjectOrg2d(hsp, OID_FRAME2d3,&lastpoint);
                    SetState(sfrectselect1);
                }
            }break;
            case sfOdown1:
                if (fabs(point2d.x-lastpoint.x)>1 ||
                    fabs(point2d.y-lastpoint.y)>1){
                    if ( modKeys & MK_CONTROL ){
                        POINT2D neworg;
                        neworg.x=point2d.x-delta.x;neworg.y=point2d.y-delta.y;
                        Snap(neworg);
                        Cm_DuplicateObj(&neworg);
                    }
                    SetState(sfOTrack);
                    SetCapture();
                    DeleteEditFrame2d(hsp);
                    SetDragCursor(modKeys & MK_CONTROL);
                    /*
          GetObjectOrgSize2d(hsp,object,&delta,NULL);
          delta.x=lastpoint.x-delta.x;delta.y=lastpoint.y-delta.y;
        lastpoint.x=0;lastpoint.y=0;
      */
                }else break;
            case sfOTrack:{
                POINT2D neworg;
                neworg.x=point2d.x-delta.x;neworg.y=point2d.y-delta.y;
                Snap(neworg);
                TrackGroup(neworg);
            }break;
            case sfLClipBase:{

            }break;
            case sfLbase:{
            }break;
            case sfLDrag:{
                DeleteLinkFrame();
                SetVectorPoint2d(hsp,link,editpoint,&_point2d);
            };break;

            case sfnewWinedit:
            case sfnewWincombo:
            case sfnewWincheck:
            case sfnewWinradio:
            case sfnewWinbutton:
            case sfnewWinlist:
            case sfnames:
            case sfcalcorder:
            case sfHELP:
            case sfnewarc:
            case sfL_add:
            case sfL_delete:
            case sfrectbase:
            case sfzoom:
            case sfnewgroup:;
            case sfnewrgngroup:;
            case sfnewrect:;
            case sfnewcircle:;
            case sfnewpolyline:;
            case sfnewroundrect:;
            case sfnewprojection:;
            case sfnewtext:;
            case sfnewline:break;

            case sfnewpolyline1:
                SetVectorPoint2d(hsp,object2d,editpoint,&point2d);
                SetState(sfnewpolyline2);
                break;
            case sfnewpolyline2:{POINT2D pp;
                GetVectorPoint2d(hsp,object2d,(INT16)(editpoint-1),&pp);
                GetVectorPoint2d(hsp,object2d,editpoint,&lastpoint);
                if (lastpoint.x!=point2d.x || lastpoint.y!=point2d.y){
                    BOOL a=TRUE;
                    if  ( sign(lastpoint.x-pp.x)==sign(point2d.x-pp.x)&&
                          sign(lastpoint.y-pp.y)==sign(point2d.y-pp.y) ){
                        if (point2d.x==lastpoint.x)a=(pp.x!=point2d.x);else{
                            double k1=((double)(point2d.y-lastpoint.y))/((double)(point2d.x-lastpoint.x));
                            if ((pp.x-lastpoint.x)*k1+lastpoint.y==pp.y)a=FALSE;
                        }
                    }
                    if (a){editpoint=GetVectorNumPoints2d( hsp,object2d );
                        AddVectorPoint2d(hsp,object2d,editpoint,&point2d);
                    }
                    else SetVectorPoint2d(hsp,object2d,editpoint,&point2d);
                    lastpoint=point2d;
                }}
                break;

            case sfnewpolyline_1:
                SetVectorPoint2d(hsp,object2d,editpoint,&_point2d);
                break;
                //*------------ Л И Н И И -----------------------------*//
            case sfeditline:if(GetLinePointFromPoint2d(hsp,object2d,&point2d)>-1)
                    SetCursor(HResource,IDC_DRAGPOINT);else
                    SetCursor(HResource,IDC_POINT);
                break;
            case sfeditline_btndn:
                if((abs(lastpoint.x-_point2d.x)>1) || (abs(lastpoint.y-_point2d.y)>1)){
                    SetCapture();SetState(sfeditline_drag);
                    SetCursor(HResource,IDC_DRAGPOINT);}else break;
            case sfeditline_drag:SetVectorPoint2d(hsp,object2d,editpoint,&_point2d);break;

            case sfaddtogroup:
            case sfeditline_add:;
            case sfeditline_delete:break;
                //*------------ Л И Н И И -----------------------------*//
            case sfimgaesize1:SetState(sfimgaesize2);//no break
            case sfimgaesize2:SetObjectOrg2d(hsp,subobject,&_point2d);break;
            default:SetState(sfbase);
        }
    }else{
        // move3d
        switch(state){
            case sfbase3d:break;
            case sfeditpoint3d1:{
                POINT2D _pp;
                _pp.x=point2d.x;
                _pp.y=point2d.y;
                POINT3D _newpoint=base;

                if(NewMovePoint3d(_pp,_newpoint,FALSE)){
                    PushCrdSystem3d(hsp3d);
                    if(submode3d!=0){
                        LP3dtoLP3d(hsp3d,&_newpoint,&_newpoint,1,LP3D_CURRENT,LP3D_WORLD);
                    }
                    CRDSYSTEM crd;
                    crd.hObject=object3d;
                    crd.type=LOCAL_COORDINATE;
                    crd.hObject=object3d;
                    SetCrdSystem3d(hsp3d,&crd);
                    LP3dtoLP3d(hsp3d,&_newpoint,&_newpoint,1,LP3D_WORLD,LP3D_CURRENT);
                    PopCrdSystem3d(hsp3d);
                    if(tmpdata){
                        INT16 count=*((INT16*)tmpdata);
                        INT16 *nums=((INT16*)tmpdata)+1;
                        POINT3D*points=(POINT3D*)(((BYTE*)tmpdata)+(count+1)*2);
                        POINT3D*_points=new POINT3D[count];
                        for(INT16 i=0;i<count;i++){
                            double d=sqrt((points[i].x-org3d.x)*(points[i].x-org3d.x)+
                                          (points[i].y-org3d.y)*(points[i].y-org3d.y)+
                                          (points[i].z-org3d.z)*(points[i].z-org3d.z));
                            double k=sin((1-d/size3d)*M_PI_2);
                            _points[i].x=points[i].x+(_newpoint.x-org3d.x)*k;
                            _points[i].y=points[i].y+(_newpoint.y-org3d.y)*k;
                            _points[i].z=points[i].z+(_newpoint.z-org3d.z)*k;

                        }
                        SetPoints3di( hsp3d,object3d,_points,nums,count);
                        delete _points;
                    }
                    SetPoints3d( hsp3d,object3d,&_newpoint,editpoint,1);
                    SCToolPopupVar(CM_MODIFY3D,11,1,&_newpoint);
                }
            }break;
            case sfeditpoint3d:{
                INT16 i= GetPoint3dFromPoint2d(hsp,object2d, object3d,&point2d,2);
                if(i>-1)SetCursor(HResource,IDC_DRAGPOINT);else SetCursor(0,IDC_ARROW);
            }break;
            case sfcamera3d:{
                int sm3d=GetRotate3dCursor(point2d);
                switch(sm3d){
                    case 0:SetCursor(0,IDC_ARROW);break;
                    case 1:SetCursor(HResource,IDC_ROTATE3DH);break;
                    case 2:SetCursor(HResource,IDC_ROTATE3DV);break;
                    case 3:SetCursor(HResource,IDC_ROTATE);break;
                    case 4:SetCursor(HResource,IDC_ZOOMIN);break;
                    case 5:SetCursor(HResource,IDC_FOCUScrs);break;
                    case 6:SetCursor(HResource,IDC_DISTANCE);break;
                    case 7:SetCursor(HResource,IDC_DRAG);break;
                    default: SetCursor(0,IDC_ARROW);
                }
            }break;
            case sfscnbtndown3d:{
                substate=0;
                GetCamera3d(hsp3d,hcamera,&camera3d);
                SetCapture();
                SetState(sfscndrag3d);
            };
            case sfscndrag3d:{
                CAMERA3D cam=camera3d;
                LowQCamera(cam);
                // aroun Z
                switch(submode3d){
                    case 5:{//pan
                        if(cam.poFocus<0)cam.poFocus=1;
                        double dx=(lastpoint.x-point2d.x);
                        if (dx>0.0)dx=1.0+dx/100.0;else dx=1.0/(1.0+fabs(dx/100.0));
                        cam.poFocus*=dx;
                    };break;
                    case 4:{//scale
                        double dx=-(lastpoint.x-point2d.x);
                        if (dx>0.0)dx=1.0+dx/100.0;else dx=1.0/(1.0+fabs(dx/100.0));
                        TransformCamera3d(hsp,object2d,cam,10,dx);
                    }break;//
                    case 6:{
                        double dx=(lastpoint.y-point2d.y);
                        //if (dx>0.0)dx=1.0+dx/100.0;else dx=1.0/(1.0+fabs(dx/100.0));
                        TransformCamera3d(hsp,object2d,cam,6,dx);
                    }break;//
                    case 7:{// pan
                        delta.x=point2d.x-lastpoint.x;
                        delta.y=point2d.y-lastpoint.y;

                        POINT2D pos;
                        pos.x=view_size.x*cam.poOffset.x;
                        pos.y=view_size.y*cam.poOffset.y;

                        pos.x+=delta.x;
                        pos.y+=delta.y;

                        cam.poOffset.x=pos.x/view_size.x;
                        cam.poOffset.y=pos.y/view_size.y;
                    }break;
                    case 1:{ // поворот относительно Y
                        TransformCamera3d(hsp,object2d,cam,1,-M_PI*(point2d.x-lastpoint.x)/300.0);
                    }break;
                    case 2:{ // поворот относительно X
                        TransformCamera3d(hsp,object2d,cam,2,M_PI*(point2d.y-lastpoint.y)/300.0);
                    }break;
                    case 3:{ // поворот относительно Z
                        double centerx,centery;
                        centerx=view_org.x+view_size.x/2;
                        centery=view_org.y+view_size.y/2;
                        double dx=(point2d.x-centerx);
                        double dy=(point2d.y-centery);
                        double r;
                        r=GetAngleByXY2d(dx,dy);
                        dx=(lastpoint.x-centerx);
                        dy=(lastpoint.y-centery);
                        double a1=GetAngleByXY2d(dx,dy);;
                        r-=a1;
                        r=-r;
                        TransformCamera3d(hsp,object2d,cam,3,r);
                    };break;
                }
                SetCamera3d(hsp3d,hcamera,&cam);
            }break;

            case sflbuttondown3d:{
                if(modKeys&MK_CONTROL)
                    switch(mode3d){
                        case CM_3Dmove:
                        case CM_3Dscale:
                        case CM_3Drotate:{
                            Cm_DuplicateObj(NULL);
                        }
                    }

                switch(mode3d){
                    case CM_3Dmove:{
                        if(GetObjectBase3d(hsp3d,object3d,&base)){
                            POINT2D _org2d;
                            LP3dtoLP2d(hsp,object2d,&_org2d,&base,1);
                            delta.x=_org2d.x-lastpoint.x;
                            delta.y=_org2d.y-lastpoint.y;
                            //       _DeleteEditFrame3d();
                            SetCapture();
                            // Определение режима

                            PushCrdSystem3d(hsp3d);
                            CRDSYSTEM crd;
                            crd.type=GetCrdType();
                            crd.hObject=hlocalcs?hlocalcs:object3d;
                            SetCrdSystem3d(hsp3d,&crd);

                            if(crd.type!=VIEW_COORDINATE)
                            {
                                int a=GetAxis3d();
                                submode3d=(INT16)a;
                            }else submode3d=0;
                            SetCursor(HResource,IDC_DRAG);
                        }else return;
                    }break;
                    case CM_3Dscale:{
                        size3d=1.0;
                        _DeleteEditFrame3d();
                        SetCapture();
                    }break;
                    case CM_3Drotate:{
                        PushCrdSystem3d(hsp3d);
                        CRDSYSTEM crd;
                        crd.type=LOCAL_COORDINATE;
                        crd.hObject=hlocalcs?hlocalcs:object3d;

                        SetCrdSystem3d(hsp3d,&crd);
                        ROTATOR3D * r=new ROTATOR3D;
                        memset(r,0,sizeof(ROTATOR3D));
                        int a=GetAxis3d();
                        r->VctSize.x=10*((a&1)!=0);
                        r->VctSize.y=10*((a&2)!=0);
                        r->VctSize.z=10*((a&4)!=0);
                        tmpdata=r;
                        SetCapture();
                        break;
                    };
                    default:SetState(sfbase3d);return;
                } //switch
                SetState(sfdrag3d);
            };
            case sfdrag3d:{
                switch(mode3d){
                    case CM_3Dmove:{
                        POINT2D _pp;
                        _pp.x=point2d.x+delta.x;
                        _pp.y=point2d.y+delta.y;
                        POINT3D _newpoint;
                        if(NewMovePoint3d(_pp,_newpoint)){
                            SetObjectBase3d(hsp3d,GetObject3d(),&_newpoint);
                        }
                    };break;
                        // end object move3d
                    case CM_3Drotate:{
                        ROTATOR3D * r=(ROTATOR3D *)tmpdata;
                        double a=M_PI*((lastpoint.x-point2d.x)/((modKeys&MK_SHIFT )?100.0:5.0))/180.0;
                        ROTATOR3D _r=*r;
                        _r.Angle=r->Angle-a;
                        RotateObject3d(hsp3d,GetObject3d(),&_r);
                        r->Angle=a;
                        break;
                    };
                    case CM_3Dscale:{
                        double dx=(lastpoint.x-point2d.x);
                        if (dx<0.0)dx=1.0+fabs(dx)/100.0;else dx=1.0/(1.0+fabs(dx/100.0));
                        double d=dx/size3d;
                        if (d!=1.0){
                            gr_float m[4][4];
                            memset(m,0,sizeof(m));
                            m[0][0]=m[1][1]=m[2][2]=m[3][3]=1.0;

                            int a=GetAxis3d();
                            if((a&1)!=0)m[0][0]=d;
                            if((a&2)!=0)m[1][1]=d;
                            if((a&4)!=0)m[2][2]=d;

                            PushCrdSystem3d(hsp3d);
                            CRDSYSTEM crd;
                            crd.type=GetCrdType();
                            crd.hObject=hlocalcs?hlocalcs:GetObject3d();
                            SetCrdSystem3d(hsp3d,&crd);

                            TransformObject3d(hsp3d,GetObject3d(),&(m[0][0]));
                            PopCrdSystem3d(hsp3d);
                            size3d=dx;
                        }
                    };break;
                }}break;
            case sfnewgroup3d:
            case sfaddtogroup3d:break;

            default:SetState(sfbase3d);
        }
    }

    SetText(&point2d,0);
    // SetCoordinates(point2d);
};
void TSchemeWindow::LowQCamera(CAMERA3D&cam){
    cam.poRenderType&=~RENDER_TYPEMASK;
    cam.poRenderType|=1;
    lstrcpy(cam.poRenderName,"Default");
};
BOOL TSchemeWindow::AcceptDrop(DRAGDROPSTRUCT&data){
    if (_class){
        if (data.datatype==DATA_CLASS)return 1;
    }
    return 0;
};

BOOL TSchemeWindow::ProcessDrop(DRAGDROPSTRUCT&data)
{
    POINT2D _mousepos;
    _mousepos.x=data.CursorPos.x;
    _mousepos.y=data.CursorPos.y;
    DPtoLP2d(hsp,&_mousepos,1);
    if (data.datatype==DATA_CLASS)
    {
        char name[256];
        lstrcpy(name,((PClassListInfo)(data.data))->GetClassName());
        DeleteFrame();
        object=0;
        AddChildren(_class,name,&object,_mousepos.x,_mousepos.y);
        CreateImageFrame();
        UpdateScrollers();
        return 1;
    }
    return 0;
};

LRESULT TSchemeWindow::KeyUp(WPARAM key,LPARAM/* dwKeyData*/){

    switch(state){
        case sfeditline_add:if (key==VK_CONTROL)goto m1;
            ;break;
        case sfeditline_delete:if (key==VK_SHIFT){
m1:				SetState(sfeditline);
                SetCursor(HResource,IDC_POINT);
                ReleaseCapture();
            };return 0;
    };
    if (link>0)
        switch(key){
            case VK_SHIFT:
                if (state==sfL_delete){
                    SetState(sfbase);
                    ReleaseCapture();
                }; return 0;
            case VK_CONTROL:
                if (state==sfL_add){
                    SetState(sfbase);
                    ReleaseCapture();
                };return 0;
        }
    if ((link>0 || object>0 || __objects>0 || object2d || __objects2d) && (key==VK_DELETE)){EvCommand(CM_EDITDELETE,0,0);}
    return 0;
}

LRESULT TSchemeWindow::KeyDown(WPARAM wVkey,LPARAM dwKeyData){
    int repeat=LOWORD(dwKeyData);
    if ((wVkey==VK_ESCAPE) &&
        ((state==sfzoom) || (state==sfHELP) || (state==sfcalcorder)||(state==sfnames))
        ){SetState(sfbase);}
    if (_class){
        switch(wVkey){
            case VK_INSERT:if((ScGetKeyState(VK_SHIFT)==0) &&
                              (ScGetKeyState(VK_CONTROL)==0)&&
                              (ScGetKeyState(VK_MENU)==0)){
                    EvCommand(CM_INSERTOBJECTm,HWindow,0);
                }return 0;
            case VK_TAB:{SelectNext(ScGetKeyState(VK_SHIFT)); return 0;}
        }
        if ((link>0) &&(!readonly))
            switch(wVkey){
                case VK_SHIFT:
                    if (state==sfbase){
                        SetState(sfL_delete);
                        SetCursor(HResource,IDC_DELPOINT);
                        ReleaseCapture();
                    };return 0;
                case VK_CONTROL:
                    if (state==sfbase){
                        SetState(sfL_add);
                        SetCursor(HResource,IDC_ADDPOINT);
                        ReleaseCapture();
                    };return 0;
            }
    }
    if(state>=sfbase3d){
        switch(wVkey){
            case 'X':
            case 'Y':
            case 'Z':{
                int b=ScGetKeyState('X');
                if(ScGetKeyState('Y'))b|=2;
                if(ScGetKeyState('Z'))b|=4;
                SetAxis3d(b);
            }break;
        }
        return 0;
    }


    if ((object2d || object || __objects || __objects2d)){
        if( !readonly){
            switch(wVkey){
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN:break;
                default: goto m1;
            }

            HOBJ2D _hobj=0;
            if (object2d)_hobj=object2d;
            if (object)_hobj=object;

            POINT2D p,p_old,p_new;

            do{

                if(__objects || __objects2d){
                    p=GetRectOrg2d();
                }else  GetObjectOrgSize2d(hsp,_hobj,&p,NULL);
                p_old=p;
                do{
                    switch(wVkey){
                        case VK_LEFT:p.x--;break;
                        case VK_RIGHT:p.x++;break;
                        case VK_UP:p.y--;break;
                        case VK_DOWN:p.y++;break;
                    }
                    p_new=p;
                    Snap(p_new);
                }while ((p_new.x==p_old.x) && (p_new.y==p_old.y));
                POINT2D pf;

                if (object2d){
                    SetObjectOrg2d(hsp,_hobj,&p_new);
                }else{
                    lastpoint.x=delta.x=0;
                    lastpoint.y=delta.y=0;
                    TrackGroup(p_new);
                }
                repeat--;
            }while(repeat>0);
            if(!object2d && _class)
            {
                _class->UpdateCoordinates();
            }
        }
    }else{
        // перемещение экрана
        int dx=0,dy=0;
        switch(wVkey){
            case VK_LEFT:dx=-8;break;
            case VK_RIGHT:dx=8;break;
            case VK_UP:dy=-8;break;
            case VK_DOWN:dy=8;break;
            case VK_PRIOR:dy=-vscroll->GetPage();break;
            case VK_NEXT:dy=vscroll->GetPage();break;
        }
        if(dx || dy){
            long x,y,nx,ny;
            x=hscroll->GetPos();
            y=vscroll->GetPos();
            nx=x+dx;
            ny=y+dy;
            ny=vscroll->ValidValue(ny);
            nx=hscroll->ValidValue(nx);

            if((nx!=x)||(ny!=y)){
                hscroll->SetPos(nx,0);
                vscroll->SetPos(ny,0);
                ScrollWindow(HWindow,(int)(x-nx),(int)(y-ny),NULL,NULL);
                POINT2D pp;
                pp.x=nx;
                pp.y=ny;
                SetOrgSpace2d(hsp,&pp);
                UpdateWindow(HWindow);
            }
        }
    }
m1:
    switch(wVkey){

        case VK_RETURN:{
            EvCommand(CM_PROPERTIES,0,0);
        }break;
        case VK_SPACE:if(!readonly){
                if (object2d){
                    if (RCenter())DeleteRotateCenter2d(hsp);else{
                        TRestoreCrd crd(hsp);
                        POINT2D org,size;
                        GetObjectOrgSize2d(hsp,object2d,&org,&size);
                        CreateRotateCenter2d(hsp,&TPoint(org.x+size.x/2,org.y+size.y/2));
                    }
                }
            }break;
        case VK_ESCAPE:LocalUndo(FALSE);return 0;
        case VK_SHIFT:
            if (state==sfeditline){
                SetState(sfeditline_delete);
                SetCursor(HResource,IDC_DELPOINT);
                ReleaseCapture();
            };break;
        case VK_CONTROL:
            if (state==sfeditline){
                SetState(sfeditline_add);
                SetCursor(HResource,IDC_ADDPOINT);
                ReleaseCapture();
            };break;

        case VK_PRIOR:if(object2d && (!readonly))SetZOrder2d(hsp,object2d,(INT16)(GetZOrder2d(hsp,object2d)+1));break;
        case VK_NEXT :if(object2d && (!readonly))SetZOrder2d(hsp,object2d,(INT16)(GetZOrder2d(hsp,object2d)-1));break;

    }
    SetText(NULL,0);
    return 0;
};

INT16  TSchemeWindow::WhatObject(HOBJ2D obj)
{
    switch(hardmode)
    {
        case SCHEME_EDITOR:
            if (_class)
                return _WhatType(_class, hsp, obj);
            return 0;

        case IMAGE_EDITOR:
            break;
        case EDITOR:
            break;
    }
    return 0;
}

HOBJ2D  TSchemeWindow::GetDefaultPen(){
    HOBJ2D pen=GetCurrentPen2d(hsp);
    if (((INT16)pen)>0)return pen;
    return CreatePen2d(hsp,PS_SOLID,1,0,R2_COPYPEN);
};

HOBJ2D  TSchemeWindow::GetDefaultBrush(){
    HOBJ2D brush=GetCurrentBrush2d(hsp);
    if (brush>0)return brush;
    return  CreateBrush2d(hsp,BS_SOLID,0,RGB(192,192,192),0,R2_COPYPEN);
};

void TSchemeWindow::SelectNext(BOOL prev){
    if (_class && _class->linkcount && link){
        INT16 num=0;
        if (link>0){
            _class->GetLinkById(link,&num);
            if (num<0) return;
            if (prev){
                if (num>0)num--;else num=(INT16)(_class->linkcount-1);
            }else{
                if (num<_class->linkcount-1)num++;else num=0;
            }
            link=_class->link[num].handle;
        }else link=_class->link[0].handle;
        plink=_class->GetLinkById(link,&num);
        SetState(sfLbase);

        return ;
    };

    if (_class && _class->childcount){
        if (object>0){
            INT16 num=_class->GetChildById(object);
            if (num<0) return;
            if (prev){
                if (num>0)num--;else num=(INT16)(_class->childcount-1);
            }else{
                if (num<_class->childcount-1)num++;else num=0;
            }
            object=_class->childs[num].handle;
        }else object=_class->childs[0].handle;
        CreateImageFrame();
    }
};

HOBJ2D TSchemeWindow::_GetObjectFromPoint(POINT2D&point){
    HOBJ2D primary;
    HOBJ2D o=GetObjectFromPoint2d(hsp,&point,&primary);
    if (object2d>0 && o && o<OID_RESERVED){
        while ((primary!=object2d) && primary)
            primary=GetObjectParent2d(hsp,primary);
        if (primary==object2d)o=object2d;
    }
    return o;
};

/*-------------------------------------------------------------------*/
HOBJ2D TSchemeWindow::_GetObject3dFromPoint(POINT2D& point){
    HOBJ2D primary3d;
    HOBJ2D o=GetObject3dFromPoint2d(hsp,object2d,&point,&primary3d);
    if (object3d>0 && o && o<OID_RESERVED){
        while ((primary3d!=object3d) && primary3d)
            primary3d=GetObjectParent3d(hsp3d,primary3d);
        if (primary3d==object3d)o=object3d;
    }
    return o;
}
/*-------------------------------------------------------------------*/


void TSchemeWindow::Cm_EditObject(){
    Object2dDialog(HWindow,hsp,object2d,0,(hardmode==IMAGE_EDITOR)?_class:NULL);
    UpdateScrollers();
};
void TSchemeWindow::SetCurrentObject(HOBJ2D o){
    object2d=o;
    if (o<OID_RESERVED){
        if (GetCurrentObject2d(hsp)!=o){
            SetCurrentObject2d(hsp,o);
            if (RCenter())DeleteRotateCenter2d(hsp);}
        CreateEditFrame2d();
    }
};
void TSchemeWindow::DeleteFrame(){
    DeleteEditFrame2d(hsp);
    if (RCenter())DeleteRotateCenter2d(hsp);
};
void TSchemeWindow::_DeleteObject3d(){
    DeleteObject2d((HSP2D)hsp3d,object3d);
    object3d=0;
    SetCursor(0,IDC_ARROW);
};
void TSchemeWindow::_DeleteObject2d(){
    FullDeleteObject2d(hsp,object2d);object2d=0;
    SetCursor(0,IDC_ARROW);
};

void TSchemeWindow::GetRect2d(RECT & r,HOBJ2D obj){
    POINT2D org,size;
    GetObjectOrgSize2d(hsp,obj,&org,&size);
    r.left=org.x;
    r.top=org.y;
    r.right=org.x+size.x;
    r.bottom=org.y+size.y;

};
/*-------------------------------------------------------------------*/
void TSchemeWindow::SetReadOnly(BOOL r){
    readonly=r;
    SetSpaceParam2d(hsp,SP_SETREADONLY,readonly);
};
void TSchemeWindow::Cm_ActualSize()
{if (object){
        POINT2D o,s;
        if (!GetBitmapSrcRect(hsp,object,&o,&s))GetActualSize2d(hsp,object,&s);
        SetObjectSize2d(hsp,object,&s);
    }
};

void TSchemeWindow::Cm_Paste(){
    if(IsClipboardFormatAvailable(CF_2D)){
        SetState(sfbase);
        object2d=PasteFromClipboard2d(hsp,&GetPastePoint(),
                                      PFC_SETCURRENT|PFC_MOVEOBJECT|pasteflags);
        HOBJ2D  h= GetObjectByName2d(hsp,object2d,"$@#_frames2d_$@#");
        if(h){
            __objects2d=object2d;
            object2d=0;
            SetState(sfrectbase);
        }else{
            PostCreateObject2d(object2d);
        }
        ReleaseCapture();
    }
};
void TSchemeWindow::Cm_Cut(){
    if(!readonly){EvCommand(CM_EDITCOPY,0,0);EvCommand(CM_EDITDELETE,0,0);}
};
void TSchemeWindow::DoneLinkPaste(){
    if(tmpdata){
        LINKCLIP*lc=(LINKCLIP*)tmpdata;
        if(lc->plink){ _class->DeleteLink(lc->plink);delete lc->plink;}
        delete tmpdata;tmpdata=NULL;

    }
}
void TSchemeWindow::Cm_Copy(){
    if(__objects2d)CopyToClipboard2d(hsp,__objects2d);
    if(object2d){CopyToClipboard2d(hsp,object2d);}

    if(OpenClipboard(HWindow)){
        if(object2d){
            INT16 t=GetObjectType2d(hsp,object2d);

            switch(t){
                case otTEXT2D://SetClipboardData(CF_TEXT,NULL);
                    //SetClipboardData(CF_RTF,NULL);
                    RenderFormat(CF_RTF);
                    RenderFormat(CF_TEXT);
                    break;
                case otBITMAP2D:
                    //SetClipboardData(CF_DIB,NULL);
                    RenderFormat(CF_DIB);
                    break;

            }

        }
        lastinclipboard= GetClipboardData(CF_2D);
        CloseClipboard();

    }

};
BOOL TSchemeWindow::LocalUndo(BOOL){
    switch(state){

        case sfnewgroup3d:
        case sfaddtogroup3d:SetState(sfbase3d);break;

        case sfpaste:SetState(sfbase);ReleaseCapture();break;
        case sfLClipBase://DoneLinkPaste();break;
        case sftransprent:
        case sfLbase:
        case sfnewWinedit:
        case sfnewWincombo:
        case sfnewWincheck:
        case sfnewWinradio:
        case sfnewWinbutton:
        case sfnewWinlist:
        case sfwindrag:
        case sfzoom:
        case sfnewgroup:;
        case sfnewrgngroup:;
        case sfnewpolyline:;
        case sfnewarc:;
        case sfnewrect:;
        case sfnewcircle:;
        case sfnewroundrect:;
        case sfnewprojection:;
        case sfnewtext:;
        case sfnewline:SetState(sfbase);break;
        case sfnewpolyline_1:ReleaseCapture();SetState(sfbase);PostCreateObject2d(object2d);break;
        case sfeditline:{SetState(sfbase);
            SetCurrentObject(object2d);
            ;break;}
        case sfaddtogroup:SetCurrentObject(object2d);
            SetState(sfbase);break;
        default: return FALSE;
    }
    return TRUE;
};

HMENU TSchemeWindow::CreateView3dPopup(){

};
/*-------------------------------------------------------------------*/
int  TSchemeWindow::Create3dViewMenu(HMENU*hp){
    int i=1;
    HOBJ2D _hcamera=Get3dViewCamera3d(hsp,object2d);
    HSP3D _hsp3d=Get3dSpace2d(hsp,object2d);
    CAMERA3D camera3d;
    GetCamera3d(_hsp3d,_hcamera,&camera3d);
    hp[i]=ScLoadPopup(MENU_SUB3D);

    INT16 render=0;
    char r_name[256];
    while ((render=GetNextRenderDriver3d(render,r_name,sizeof(r_name)))!=0){
        int mode=MF_ENABLED;
        if (!lstrcmpi(r_name,camera3d.poRenderName))mode|=MF_CHECKED;
        char s[256];
        wsprintf(s,"%d %s",render,r_name);
        AppendMenu(hp[i],mode,CM_DRIVER3D+render,s);
    }
    ScAppendMenu(hp[0],MF_POPUP,(int)hp[i],IDS_MENU26);
    i++;
    i++;
    int mode=camera3d.poRenderType&RENDER_TYPEMASK;

    AppendMenu(hp[0], MF_SEPARATOR,0,"");
    char s[256];
    int j=IDS_RENDERMODES;
    while(LoadString(HResource,j,s,sizeof(s))){
        AppendMenu(hp[0], MF_ENABLED|(mode==j-IDS_RENDERMODES)?MF_CHECKED:0,CM_RENDER0+j-IDS_RENDERMODES,s);
        j++;
    }

    AppendMenu(hp[0], MF_SEPARATOR,0,"");
    ScAppendMenu(hp[0], MF_ENABLED|(camera3d.poRenderType&RENDER_USELIGHT)?MF_CHECKED:0,CM_RENDER_USELIGHT,IDS_MENU36);
    ScAppendMenu(hp[0], MF_ENABLED|(camera3d.poRenderType&RENDER_HAZE)?MF_CHECKED:0,CM_RENDER_HAZE,        IDS_MENU37);
    ScAppendMenu(hp[0], MF_ENABLED|(camera3d.poRenderType&RENDER_TEXTURED)?MF_CHECKED:0,CM_RENDER_TEXTURE, IDS_MENU38);
    ScAppendMenu(hp[0], MF_ENABLED|(camera3d.poRenderType&RENDER_SHADED)?MF_CHECKED:0,CM_RENDER_SHADED,    IDS_MENU39);
    ScAppendMenu(hp[0], MF_ENABLED|(camera3d.poRenderType&RENDER_ZSORT)?MF_CHECKED:0,CM_RENDER_ZSORT,      IDS_MENU40);
    ScAppendMenu(hp[0], MF_ENABLED|(camera3d.poRenderType&RENDER_ALLFACES)?MF_CHECKED:0,CM_RENDER_ALLFACES,IDS_MENU41);
    AppendMenu(hp[0], MF_SEPARATOR,0,"");


    hp[i]=CreatePopupMenu();
    HOBJ2D hcamera=0;
    INT16 k=0;
    while ((hcamera=GetNextObject3d(_hsp3d,hcamera))!=0)
    {
        if (GetObjectType3d(_hsp3d,hcamera)==otCAMERA3D){
            char s[128];
            char cmd[128];
            char ss[256];
            GetObjectName3d(_hsp3d,hcamera,s,sizeof(s));

            if (!lstrlen(s))LoadString(HResource,IDS_str115,s,sizeof(s));
            LoadString(HResource,IDS_str116,cmd,sizeof(cmd));
            wsprintf(ss,cmd,s);
            AppendMenu(hp[i], MF_ENABLED|((hcamera==_hcamera)?MF_CHECKED:0),CM_CAMERA+k,ss);
            k++;
        }
    }
    ScAppendMenu(hp[0],MF_POPUP,(int)hp[i],IDS_MENU42 );i++;
    i++;
};
void TWindow::AddCopyCutMenu(HMENU m){
    BOOL e1=!IsCommandDisable(CM_EDITCUT);
    BOOL e2=!IsCommandDisable(CM_EDITCOPY);
    BOOL e3=!IsCommandDisable(CM_EDITPASTE);
    if(e1 || e2 || e3){
        AppendMenu(m, MF_SEPARATOR,0,"");
        if(e1) ScAppendMenu(m,MF_ENABLED,CM_EDITCUT  , IDS_MENU68 );
        if(e2) ScAppendMenu(m,MF_ENABLED,CM_EDITCOPY , IDS_MENU67 );
        if(e3) ScAppendMenu(m,MF_ENABLED,CM_EDITPASTE, IDS_MENU9  );
    }
};

void TSchemeWindow::_ObjectMenu(POINT point,HOBJ2D obj,int /*mouse*/){
    BOOL _info=1;

    HMENU hp[5];
    memset(hp,0,sizeof(hp));
    hp[0]=CreatePopupMenu();int i=1;
    if (hsp3d){

    }else{
        int t=GetObjectType2d(hsp,obj);

        ScAppendMenu(hp[0], MF_ENABLED , CM_OBJECTEDIT , IDS_MENU29 );

        if (t==otVIEW3D2D){
            ScAppendMenu(hp[0], MF_ENABLED,CM_EDITVIEW3D, IDS_MENU43 );
            i+=Create3dViewMenu(hp);

        }

#ifdef CAN_EDIT_BITMAP
        if (t==otBITMAP2D || t==otDOUBLEBITMAP2D){
            ScAppendMenu(hp[0], MF_ENABLED,CM_EDITBITMAP,  IDS_MENU44 );
            if(t!=otBITMAP2D){
                ScAppendMenu(hp[0], MF_ENABLED,CM_COPYASBITMAP,IDS_MENU45 );
            }
            ScAppendMenu(hp[0], MF_ENABLED,CM_SAVEOBJECT,  IDS_MENU46 );
        }
#endif


        if (t==otLINE2D){
            ScAppendMenu(hp[0], MF_ENABLED,CM_EDITPOINTS,IDS_MENU47 );
        }

        if(GetObjectParent2d(hsp,object2d)){

            ScAppendMenu(hp[0], MF_ENABLED,CM_DELFROMGROUP,IDS_MENU48  );
        }

        if (t==otGROUP2D || t==otRGROUP2D){

            ScAppendMenu(hp[0], MF_ENABLED,CM_UNGROUP,IDS_MENU30  );
        }
        if (t==otLINE2D || t==otRGROUP2D){
            AppendMenu(hp[0], MF_SEPARATOR,0,"");
            int mode;
            if (t==otLINE2D)mode=GetRgnCreateMode(hsp,obj);
            else mode=GetRgnCreateMode(hsp,GetGroupItem2d(hsp,obj,0));

            hp[i]=CreatePopupMenu();
            AppendMenu(hp[i], MF_ENABLED |IsChecked(mode,ALTERNATE)
                       ,CM_POLYFILLMODEALTERNATE,"&Alternate");
            AppendMenu(hp[i], MF_ENABLED | IsChecked(mode,WINDING),CM_POLYFILLMODEWINDING,"&Winding");

            ScAppendMenu(hp[0],MF_POPUP,(int)hp[i],IDS_MENU50 );
            i++;

            HOBJ2D p=GetObjectParent2d(hsp,obj);
            if (t==otRGROUP2D||(t==otLINE2D && p && GetObjectType2d(hsp,p)==otRGROUP2D)){
                int enable=0;//Запрещено если поледняя в RGN group
                RGNGROUPITEM  item;mode=0;
                if (t==otRGROUP2D){GetRgnGroupItem2d(hsp,obj,0,&item);
                    mode=item.Rop;
                }else{
                    INT16 n=GetGroupItemsNum2d(hsp,p);
                    for(INT16 i=0;i<n;i++){
                        if (GetRgnGroupItem2d(hsp,p,i,&item)){
                            if (obj==item.Object){  mode=item.Rop;enable=(i==n-1)?MF_GRAYED:0;
                                i=n;}
                        }}}
                hp[i]=CreatePopupMenu();
                AppendMenu(hp[i], MF_ENABLED | IsChecked(mode,RGN_AND),CM_POLYCOMBINEMODE_AND,"&AND");
                AppendMenu(hp[i], MF_ENABLED| IsChecked(mode,RGN_OR),CM_POLYCOMBINEMODE_OR,"&OR");
                AppendMenu(hp[i], MF_ENABLED| IsChecked(mode,RGN_XOR),CM_POLYCOMBINEMODE_XOR,"&XOR");
                AppendMenu(hp[i], MF_ENABLED| IsChecked(mode,RGN_DIFF),CM_POLYCOMBINEMODE_DIFF,"&DIFF");

                ScAppendMenu(hp[0],MF_POPUP|enable,(int)hp[i],IDS_MENU51);
                i++;
            }
            //	AppendMenu(hp[0], MF_SEPARATOR,0,"");
        }
        if (IsVideoFrame2d(hsp,obj)){

            ScAppendMenu(hp[0], MF_ENABLED,CM_AVIPLAY,  IDS_MENU58 );
            ScAppendMenu(hp[0], MF_ENABLED,CM_AVISTART, IDS_MENU52 );
            ScAppendMenu(hp[0], MF_ENABLED,CM_AVIEND,   IDS_MENU53 );
            ScAppendMenu(hp[0], MF_ENABLED,CM_AVIPAUSE, IDS_MENU54 );
            ScAppendMenu(hp[0], MF_ENABLED,CM_AVIRESUME,IDS_MENU55 );
            ScAppendMenu(hp[0], MF_ENABLED,CM_AVISTOP,  IDS_MENU56 );

            AppendMenu(hp[0], MF_SEPARATOR,0,"");

        }
        if ((t!=otRGROUP2D)&&(t!=otGROUP2D)){
            ScAppendMenu(hp[0], MF_POPUP|MF_ENABLED,(UINT)ScLoadPopup(MENU_ZORDER),IDS_MENU69);
        }

    }

    //	ScAppendMenu(hp[0], MF_ENABLED,CM_HIDEOBJECT,IDS_MENU57);
    AddGotoMenu(hp[0],NULL);
    AddCopyCutMenu(hp[0]);
    TSTRATUMMENU sm;
    sm.hWindow=HWindow;
    sm.hSpace =hsp;
    sm.hObject=obj;
    sm.hType=SCMENU_OBJECT2D;
    sm.hMenu=hp[0];
    ClientToScreen(HWindow,&point);
    SetDefaultMenuItem(hp[0],CM_OBJECTEDIT);
    TrackPopupMenu(point,&sm,TRUE);
};

void TSchemeWindow::Cm_Poly(WPARAM p){
    HOBJ2D obj=(IsReserved(object2d))?GetCurrentObject2d(hsp):object2d;
    INT16 t=GetObjectType2d(hsp,obj);
    if (t==otLINE2D || t==otRGROUP2D){
        RGNGROUPITEM  item;
        INT16 mode;
        switch (p){
            case CM_POLYFILLMODEALTERNATE:mode=ALTERNATE;goto m1;
            case CM_POLYFILLMODEWINDING:mode=WINDING;
m1:if(t==otLINE2D) SetRgnCreateMode(hsp,obj,mode);else{
                    INT16 n=GetGroupItemsNum2d(hsp,obj);
                    for(INT16 i=0;i<n;i++){
                        GetRgnGroupItem2d(hsp,obj,i,&item);
                        SetRgnCreateMode(hsp,item.Object,mode);
                    }	 }
                break;
            case CM_POLYCOMBINEMODE_AND:mode=RGN_AND;goto m2;
            case CM_POLYCOMBINEMODE_OR:mode=RGN_OR;goto m2;
            case CM_POLYCOMBINEMODE_XOR:mode=RGN_XOR;goto m2;
            case CM_POLYCOMBINEMODE_DIFF:mode=RGN_DIFF;
m2:if(t==otRGROUP2D){ // вся группа регионов
                    INT16 n=GetGroupItemsNum2d(hsp,obj);
                    for(INT16 i=0;i<n;i++){
                        GetRgnGroupItem2d(hsp,obj,i,&item);
                        item.Rop=mode;
                        SetRgnGroupItem2d(hsp,obj,i,&item);
                    }}else{// один элемент группы
                    HOBJ2D p=GetObjectParent2d(hsp,obj);
                    if (p){ INT16 n=GetGroupItemsNum2d(hsp,p);
                        for(INT16 i=0;i<n;i++){
                            if (GetRgnGroupItem2d(hsp,p,i,&item))
                                if (obj==item.Object){
                                    item.Rop=mode;
                                    SetRgnGroupItem2d(hsp,p,i,&item);return;}
                        }
                    } }
                break;
        }}};

void TSchemeWindow::Cm_DuplicateObj(POINT2D * neworg){
    if(state<sfbase3d){
        TRestoreCrd crd(hsp);
        POINT2D org;
        if (neworg)org = (*neworg);else{
            if(__objects || __objects2d || object || __objects2d)org=GetRectOrg2d();
            else {
                HOBJ2D obj=object2d;
                GetObjectOrgSize2d(hsp,object2d,&org,NULL);
            };
            org.x+=10;
            org.y+=10;
        };
        if (object2d){
            long size;
            HGLOBAL block=CopyToMemory2d(hsp,object2d,&size);
            object2d=PasteFromMemory2d(hsp,block,&org,PFC_ALL|PFC_SETCURRENT|PFC_MOVEOBJECT);
            GlobalFree(block);
        }else {
            POINT2D _odl_objs,_odl_objs2d;

            if(object || __objects)
            {
                HOBJ2D h=object | __objects; // или то или это
                GetObjectOrgSize2d(hsp,h,&_odl_objs,NULL);
            }
            HGLOBAL hg=(object || __objects)?_Copy():NULL;
            HGLOBAL _obj2d=NULL;

            if(__objects2d){
                _obj2d=CopyToMemory2d(hsp,__objects2d,NULL);
                GetObjectOrgSize2d(hsp,__objects2d,&_odl_objs2d,NULL);
            };
            HOBJ2D _objs=0;
            if(_obj2d){
                _objs=PasteFromMemory2d(hsp,_obj2d,&_odl_objs2d,PFC_ALL|PFC_MOVEOBJECT);
                GlobalFree(_obj2d);
            }
            if (hg){
                _Paste(hg,&_odl_objs,__frame?TRUE:FALSE);
                GlobalFree(hg);
            }else DeleteFrames(hsp);
            __objects2d=_objs;


            SetRectOrg2d(org);

        }  }else{
        if(object3d){
            long size;
            _DeleteEditFrame3d();
            HGLOBAL block=CopyToMemory3d(hsp3d,object3d,&size);
            POINT3D org;memset(&org,0,sizeof(org));
            object3d=PasteFromMemory3d(hsp3d,block,neworg?&org:NULL);
            GlobalFree(block);
            CreateEditFrame3d();
        }
    }
};


void TSchemeWindow::GetObjectName(HOBJ2D obj,char * s){
    static INT16 last=-1;
    static char l[60];
    if (obj){
        INT16 o=GetObjectType2d3d(((INT16)hsp3d)?(HSPACE)hsp3d:(HSPACE)hsp,obj);
        if (last!=o){
            GetObjectTypeName2d(o,l,sizeof(l));
            last=o;
        }
        lstrcat(s,l);
        if (hsp3d){
            if (o==otGROUP3D){INT16 so=GetGroupItemsNum2d((HSP2D)hsp3d,obj);
                wsprintf(s+lstrlen(s),"(%d objects)",so);
            }
        }else{
            if (o==otGROUP2D || o==otRGROUP2D){
                INT16 so=GetGroupItemsNum2d(hsp,obj);
                char cmd[256];
                LoadString(HResource,IDS_str117,cmd,sizeof(cmd));
                wsprintf(s+lstrlen(s),cmd   ,so);
            }HOBJ2D p=GetObjectParent2d(hsp,obj);
            if (p){INT16 n=IsGroupContainObject2d(hsp,p,obj);
                char cmd[256];
                LoadString(HResource,IDS_str118,cmd,sizeof(cmd));
                wsprintf(s+lstrlen(s),cmd,n);
            }}
    }
};
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
void TSchemeWindow::SetText(POINT2D* tp,int){
    static char txt[512];txt[0]='\0';
    if (state<sfbase3d)
        switch (state){
            //  case sfbase:;break;
            case sftransprent:
            case sftransprent1:{
                FRAMEPROP fp;
                if (GetFrameProp2d(hsp,object2d,&fp)){
                    tmpdata=(void*)fp.sense;
                    double p=100.0*((double)fp.sense)/255.0;
                    sprintf(txt,"Transparent color rgb(%d,%d,%d),%g %%",
                            GetRValue(fp.color),
                            GetGValue(fp.color),
                            GetBValue(fp.color),p);
                }}break;
            case sfLClipBase:if(tmpdata){
                    LINKCLIP*lc=(LINKCLIP*)tmpdata;
                    char cmd[128];
                    LoadString(HResource,IDS_str129,cmd,sizeof(cmd));
                    wsprintf(txt,cmd,lc->obj1 );
                }break;
            case sfLClipdown1:
            case sfLClipTrack:if(tmpdata){
                    LINKCLIP*lc=(LINKCLIP*)tmpdata;
                    char cmd[128];
                    LoadString(HResource,IDS_str130,cmd,sizeof(cmd));
                    wsprintf(txt,cmd,lc->obj2
                             );
                }break;
            case sfnewtext   :;
            case sfnewline   :;
            case sfnewline1  :;
            case sfnewarc:;
            case sfnewarc1:;
            case sfnewrect   :;
            case sfnewcircle:;
            case sfnewroundrect:;
            case sfnewroundrect1:;
            case sfnewrect1  :;
            case sfnewpolyline :;
            case sfnewpolyline1 :;
            case sfnewpolyline2 :;
            case sfnewgroup     :;
            case sfnewrgngroup  :;  break;
            case sfnewcircle1:if(tp){
                    POINT2D r=TPoint(tp->x-lastpoint.x,tp->y-lastpoint.y);
                    r.x=fabs(r.x);r.y=fabs(r.y);
                    int numpoints=GetVectorNumPoints2d(hsp,object2d);
                    char cmd[128];
                    LoadString(HResource,IDS_str131,cmd,sizeof(cmd));
                    sprintf(txt,cmd, lastpoint.x,lastpoint.y,r.x,r.y,numpoints);
                    break;}
            case sfaddtogroup   :GetObjectName(object,txt);;
                break;
            default:
                if (object2d){
                    HOBJ2D _obj=object2d;

                    if(IsReserved(_obj))
                    {
                        if(_obj!=OID_RCENTER)
                        {
                            _obj=OID_FRAME2d;
                        }
                    }else GetObjectName(GetCurrentObject2d(hsp),txt);


                    POINT2D o,s;
                    GetObjectOrgSize2d(hsp,_obj,&o,&s);
                    double a=GetObjectAngle2d(hsp,_obj);

                    if(object2d!=OID_RCENTER){
                        char static cmd[60]="";
                        if(!cmd[0])LoadString(HResource,IDS_str212,cmd,sizeof(cmd));
                        sprintf(txt+lstrlen(txt),cmd,

                                o.x,o.y,s.x,s.y);}else
                        sprintf(txt+lstrlen(txt)," Center (%g,%g)",o.x,o.y);
                    int rc=RCenter();
                    if ((a!=0.0||rc) && object2d!=OID_RCENTER){
                        a*=180.0/3.1415926536;
                        if (a<0)a+=360.0;
                        int d1=a,d2=abs((int(a*10))%10);
                        sprintf(txt+lstrlen(txt)," Angle: %d.%d",d1,d2);
                    }
                }else{
                    SetImageText(txt);
                };
        }else{

        switch (state){
            //  case sflookfor2object:lstrcpy(txt,"Выберите второй объект ,");break;
            case sfdrag3d:{
                switch(mode3d){
                    case CM_3Dmove:{goto m1;}
                    case CM_3Drotate:{
                        ROTATOR3D * r=(ROTATOR3D *)tmpdata;
                        sprintf(txt+lstrlen(txt)," Angle: %g",r->Angle*180.0/M_PI);
                    }break;
                }
            }break;
m1:
            default:GetObjectName(object3d,txt);
                POINT3D o,s;GetObjectOrgSize3d(hsp3d,object3d,NULL,&s),
                        GetObjectBase3d(hsp3d,object3d,&o);
                sprintf(txt+lstrlen(txt)," Origin:(%g,%g,%g) Size:(%g,%g,%g)",
                        o.x,o.y,o.z,s.x,s.y,s.z);
        };

    }
    SCSetStatusText(1,txt);
    if(tp)SetCoordinates(*tp);
};
void TSchemeWindow::Cm_Names(){
    SetFocus(HWindow);
    SetCursor(0,IDC_ARROW);
    DeleteFrame();
    object=0;object2d=0;
    SetCurrentObject2d(hsp,0);
    substate=0;
    LOGFONT2D lf;
    memset(&lf,0,sizeof(lf));
    lf.lfSize=sizeof(lf);
    lf.lfHeight=14;
    LoadString(HResource,IDS_str120,lf.lfFaceName,sizeof(lf.lfFaceName));
    HOBJ2D hfont=CreateFont2d (hsp,&lf);
    for(int i=0;i<_class->childcount;i++){

        PClass _c=_class->childs[i].type->GetClass();
        LOGTEXT lt;
        lt.ltFgColor=RGB(0,0,0);
        lt.ltBgColor=(_c && _c->flags&CF_SCHEME)?RGB(145,255,200):RGB(255,255,196);
        lt.ltFontIndex=hfont;
        char s[256];
        wsprintf(s," %s ",_class->childs[i].type->GetClassName());
        lt.ltStringIndex=CreateString2d(hsp,s);
        HOBJ2D htext=::CreateText2d(hsp,&lt,1);
        POINT2D origin,size,sizet;
        GetObjectOrgSize2d(hsp,_class->childs[i].handle,&origin,&size);

        origin.y+=size.y;
        HOBJ2D htr=CreateTextRaster2d (hsp,htext,
                                       &origin,&TPoint(-1,-1),&TPoint(0,0),0);
        GetObjectOrgSize2d(hsp,htr,NULL,&sizet);
        origin.x-=(sizet.x-size.x)/2;
        SetObjectOrg2d(hsp,htr,&origin);
        AddGroupItem2d(hsp,_class->childs[i].handle,htr);
    }
};
void TSchemeWindow::Cm_CalcOrder(){
    SetCursor(HResource,IDC_ORDER);
    SetFocus(HWindow);
    DeleteFrame();
    substate=0;
    LOGFONT2D lf;
    memset(&lf,0,sizeof(lf));
    lf.lfSize=sizeof(lf);
    lf.lfHeight=20;
    // lstrcpy(lf.lfFaceName,_arial);
    LoadString(HResource,IDS_str120,lf.lfFaceName,sizeof(lf.lfFaceName));
    HOBJ2D hfont=CreateFont2d (hsp,&lf);

    for(int i=0;i<_class->childcount;i++){
        HOBJ2D handle=_class->childs[i].handle;
        char s[8];
        int I=i+1;
        wsprintf(s," %d ",I);
        LOGTEXT lt;
        lt.ltFgColor=RGB(255,255,255);
        lt.ltBgColor=RGB(255,0,0);
        lt.ltFontIndex=hfont;
        lt.ltStringIndex=CreateString2d(hsp,s);
        HOBJ2D htext=CreateText2d(hsp,&lt,1);
        POINT2D origin;
        GetObjectOrgSize2d(hsp,handle,&origin,NULL);
        HOBJ2D htr=CreateTextRaster2d (hsp,htext,
                                       &origin,&TPoint(-1,-1),&TPoint(0,0),0);
        AddGroupItem2d(hsp,handle,htr);
    }
    char cmd[128];
    LoadString(HResource,IDS_str132,cmd,sizeof(cmd));

    SCSetStatusText(1,cmd );
};

void TSchemeWindow::UpdateCalcOrder(){
    for(int i=0;i<_class->childcount;i++){
        HOBJ2D handle=_class->childs[i].handle;
        INT16 num=GetGroupItemsNum2d(hsp,handle);
        HOBJ2D obj=GetGroupItem2d(hsp,handle,(INT16)(num-1));
        HOBJ2D htext=GetTextObject2d(hsp,obj);
        LOGTEXT lt;
        GetLogText2d(hsp,htext,&lt,1);
        char so[8];
        int I=i+1;
        wsprintf(so," %d ",I);
        char s[11];
        GetLogString2d(hsp,lt.ltStringIndex,s,10);
        if (lstrcmp(so,s)){
            SetLogString2d(hsp,lt.ltStringIndex,so);
            POINT2D p;
            GetActualSize2d(hsp,obj,&p);
            SetObjectSize2d(hsp,obj,&p);
        }
    }
    char text[80];
    int I=substate+1;
    char cmd[128];
    LoadString(HResource,IDS_str133,cmd,sizeof(cmd));

    wsprintf(text,cmd,I);
    SCSetStatusText(1,text);
};

void TSchemeWindow::Un_CalcOrder(){
    HOBJ2D hfont=0;
    for(INT16 i=0;i<_class->childcount;i++){
        HOBJ2D handle=_class->childs[i].handle;
        INT16 num=GetGroupItemsNum2d(hsp,handle);
        HOBJ2D obj=GetGroupItem2d(hsp,handle,(INT16)(num-1));
        HOBJ2D htext=GetTextObject2d(hsp,obj);
        LOGTEXT lt;
        GetLogText2d(hsp,htext,&lt,1);
        hfont=lt.ltFontIndex;
        DeleteObject2d(hsp,obj);
        DeleteText2d(hsp,htext);
        DeleteString2d(hsp,lt.ltStringIndex);
    }
    DeleteFont2d(hsp,hfont);
};

void TSchemeWindow::SetImageText(char *text){
    const sizeoftext=255;
    *text=0;
    if(__objects || __objects2d){
        LoadString(HResource,IDS_str73,text,sizeoftext);
        return;
    }

    if (_class && (link || object || __objects)){
        if (object){
            int type=WhatObject(object);
            if (type==1){
                PObject po=GetActiveObject();
                if (po){

                    LoadString(HResource,IDS_str152,text,sizeoftext);
                    lstrcat(text,po->_class->name);
                    if (po->_class->varcount){
                        wsprintf(text+lstrlen(text)," %d var(s)",po->_class->varcount);
                    }
                    if (po->_class->childcount){
                        char cmd[64];
                        LoadString(HResource,IDS_str153,cmd,sizeof(cmd));
                        wsprintf(text+lstrlen(text),cmd,po->_class->childcount);
                    }
                }
            }
            if (type==2){
                LoadString(HResource,IDS_str191,text,64);
            }
        }
        if (link){
            INT16 num;
            PLinkInfo plink=_class->GetLinkById(link,&num);

            LoadString(HResource,IDS_str154,text,sizeoftext);
            if (plink){
                for(INT16 i=0;i<plink->count;i++){
                    if (lstrlen(text)>60){lstrcat(text," ...");break;}
                    if (i!=0)lstrcat(text,",");
                    lstrcat(text,plink->items[i].name1);
                    lstrcat(text,"<->");
                    lstrcat(text,plink->items[i].name2);
                }
            }
        }
    }else
        LoadString(HResource,IDS_str72,text,sizeoftext);

};

void TSchemeWindow::Snap(POINT2D&p){
    if (grid && grid->use){
        gr_float x=p.x-grid->OffsetX,y=p.y-grid->OffsetY;
        x/=grid->StepX;y/=grid->StepY;
        x+=0.5;y+=0.5;
        x=floor(x);y=floor(y);
        x*=grid->StepX;y*=grid->StepY;
        x=x+grid->OffsetX;
        y=y+grid->OffsetY;
        p.x=x;p.y=y;
    }
};

void TSchemeWindow::SetClass(PClass _c, PObject po)
{
    if (_path)
    {
        delete _path;
        _path = 0;
        pathcount = 0;
    }

    _class = _c;
    _object = po;

    if (_object)
    {
        project = po->GetProject();
        pathcount = SCGetFullObjectName(_object, 0, 0);
        _path = new HOBJ2D[pathcount];
        SCGetFullObjectName(_object, _path, pathcount);
    }
};

void TSchemeWindow::UpdateObject()
{
    if(project==0)return;
    if (_object){
        _object=SCGetObjectByFullName(project,_path,pathcount);
    }
};
void TSchemeWindow::ProjectClose(TProject*p){
    if(project==p){
        _object=0;
        if (_path){
            delete _path;_path=0;pathcount=0;
        }
    }
};
void TSchemeWindow::Cm_Zoom(){
    SetState(sfzoom);
    SetCursor(HResource,IDC_ZOOMIN);
};
void TSchemeWindow::AdjustZOrder(HOBJ2D object2d){
    if (_class && _class->childcount){
        INT16 zo,z;
        for(INT16 i=0;i<_class->childcount;i++){
            HOBJ2D obj=_class->childs[i].handle;
            HOBJ2D _obj=GetGroupItem2d(hsp,obj,0);
            if (_obj)zo=GetZOrder2d(hsp,_obj);
            if (i==0)z=zo;else { if (zo<z)z=zo; }
        }
        SetZOrder2d(hsp,object2d,z);
    }
};


void TSchemeWindow::GetSpaceInfo(char *s){
    if (_class){
        char cmd[128];
        LoadString(HResource,(scheme==_class->scheme)?IDS_str156:IDS_str157,cmd,sizeof(cmd));
        sprintf(s,cmd,_class->name);
    }else{
        LoadString(HResource,IDS_str155,s,128);
    }
};

BOOL FullDeleteObject2d(HSP2D hsp,HOBJ2D obj){
    if (obj)
        switch (GetObjectType2d(hsp,obj)){
            case otLINE2D:{
                HOBJ2D hp,hb;
                hp=GetPenObject2d(hsp,obj);hb=GetBrushObject2d(hsp,obj);
                DeleteObject2d(hsp,obj);
                if (hp && GetToolRef2d(hsp,PEN2D,hp)==0)DeletePen2d(hsp,hp);
                FullDeleteTool2d(hsp,BRUSH2D,hb);
            }break;
            case otBITMAP2D:
            {
                HOBJ2D hdib=GetDibObject2d(hsp,obj);
                DeleteObject2d(hsp,obj);
                if(GetToolRef2d(hsp,DIB2D,hdib)==0)
                    DeleteDib2d(hsp,hdib);
            }break;
            case otDOUBLEBITMAP2D:{
                HOBJ2D hddib=GetDoubleDibObject2d(hsp,obj);
                DeleteObject2d(hsp,obj);
                if (GetToolRef2d(hsp,DOUBLEDIB2D,hddib)==0)DeleteDoubleDib2d(hsp,hddib);
            }break;
            case otTEXT2D:{
                HOBJ2D htext=GetTextObject2d(hsp,obj);
                DeleteObject2d(hsp,obj);
                //
                FullDeleteTool2d(hsp,TEXT2D,htext);
            }break;
            case otVIEW3D2D:{
                HSP3D hsp3d=Get3dSpace2d(hsp,obj);
                DeleteObject2d(hsp,obj);
                if (GetUsageSpace3d(hsp3d)==0)DeleteSpace3d(hsp3d);
            }break;

            case otGROUP2D:
            case otRGROUP2D:{
                INT16 count=GetGroupItemsNum2d(hsp,obj);
                if(count){
                    HOBJ2D *items=new HOBJ2D[count];
                    GetGroupItems2d(hsp, obj, items,count);
                    for(INT16 i=0;i<count;i++)FullDeleteObject2d(hsp,items[i]);
                    delete items;
                }
                DeleteObject2d(hsp,obj);
                break;
            }
            default:DeleteObject2d(hsp,obj);break;
        }
    return 0;

};


void TSchemeWindow::Cm_HideObject(){
    if (object2d && (MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG17),_MSG_WARNING,MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON2)==IDYES))
    {
        HideObject2d(hsp,object2d);
    }
}

BOOL TSchemeWindow::SetAxis3d(int b){
    const char *ps[]={"X","Y","X-Y","Z","Z-X","Z-Y","X-Y-Z"};
    if(b<8 && b>0){
        b--;
        return SetComboStr(CM_3Daxis,(char*)ps[b]);
    }
    return FALSE;
};

int TSchemeWindow::GetAxis3d(){
    char s[64];
    int a=0;
    if(!GetComboStr(CM_3Daxis,s,sizeof(s)))lstrcpy(s,"Y");
    char *ps=s;
    while (*ps){
        switch(*ps){
            case 'X':a|=1;break;
            case 'Y':a|=2;break;
            case 'Z':a|=4;break;
        }ps++;
    }
    return a;
};
int  TSchemeWindow::GetCrdType(){
    char s[64];
    int a=0;
    if(GetComboStr(CM_3Dsystem,s,sizeof(s))){
        if(!lstrcmpi("Local",s))return LOCAL_COORDINATE;
        if(!lstrcmpi("View",s))return  VIEW_COORDINATE;
        if(!lstrcmpi("World",s))return WORLD_COORDINATE;
        if(!lstrcmpi("Parent",s))return PARENT_COORDINATE;
    }
    return 0;
};
void  TSchemeWindow::SetDragCursor(BOOL ctrl){
    if(SCGetScVar("editor2d_opt",0)&2){
        if(ctrl)  SetCursor(HResource,IDC_DRAGADD);else SetCursor(HResource,IDC_DRAG);
    }else SetCursor(HResource,IDC_EMPTY);

};
void  TSchemeWindow::SetCapture(){
    ::SetCapture(HWindow);
    captured=TRUE;
};
void  TSchemeWindow::ReleaseCapture(){
    captured=FALSE;
    ::ReleaseCapture();
};
void  TSchemeWindow::CreateEditFrame2d(){
    ::CreateEditFrame2d(hsp,&sizer,FRAME_HASPOINTS);
};
//INT16 TSchemeWindow::mode3d=CM_3Dmove;

#endif // runtime

void    TSpaceWindow::UpdateScaleBox(){
    POINT2D _m,_d;
    GetScaleSpace2d(hsp,&_m,&_d);
    double s=_m.x;s/=((double)_d.x);
    SetScaleBox(s);
};

void TSpaceWindow::SetCoordinates(POINT2D& p){
    char pos[40];
    sprintf(pos,"%g:%g",p.x,p.y);
    SCSetStatusText(2,pos);
};

void TSpaceWindow::Zoom(int xPos,int yPos,BOOL in,POINT2D * newmul,POINT2D * newdiv){
    TRestoreCrd crd(hsp);
    POINT2D _mul,_div;
    GetScaleSpace2d(hsp,&_mul,&_div);
    double d=((double)_mul.x)/((double)_div.x);
    if (in)d*=2.0; else d/=2.0;
    d*=100;
    _div=TPoint(100,100);_mul=TPoint(d,d);
    POINT p;p.x=xPos;p.y=yPos;
    POINT2D lp,dp;
    PntToPnt(lp,p);
    DPtoLP2d(hsp,&lp,1);
    if(newmul){
        _mul=*newmul;
        _div=*newdiv;
    }
    SetScaleSpace2d(hsp,&_mul,&_div);
    UpdateScrollers();
    dp=lp;
    LPtoDP2d(hsp,&dp,1);

    POINT2D org;
    GetOrgSpace2d(hsp,&org);

    org.x-=(p.x-dp.x);
    org.y-=(p.y-dp.y);

    SetOrgSpace2d(hsp,&org);
    if(hscroll)hscroll->SetPos(org.x,0);
    if(vscroll)vscroll->SetPos(org.y,0);
    UpdateScrollers();
    UpdateScaleBox();
};

void  GetTargetPoint3d(CAMERA3D&camera3d,POINT3D&p){
    if (camera3d.poFlags&CAMERA_TARGET){
        p.x=camera3d.poDirection.x;
        p.y=camera3d.poDirection.y;
        p.z=camera3d.poDirection.z;
    }else{
        p.x=camera3d.poDirection.x+camera3d.poOrg.x;
        p.y=camera3d.poDirection.y+camera3d.poOrg.y;
        p.z=camera3d.poDirection.z+camera3d.poOrg.z;
    }
};
void  SetTargetPoint3d(CAMERA3D&camera3d,POINT3D&p){
    if (camera3d.poFlags&CAMERA_TARGET){
        camera3d.poDirection.x=p.x;
        camera3d.poDirection.y=p.y;
        camera3d.poDirection.z=p.z;
    }else{
        camera3d.poDirection.x=p.x-camera3d.poOrg.x;
        camera3d.poDirection.y=p.y-camera3d.poOrg.y;
        camera3d.poDirection.z=p.z-camera3d.poOrg.z;
    }
};


BOOL  TransformCamera3d(HSP2D hsp,HOBJ2D object2d,CAMERA3D&cam,int mode,double f1,double f2,double f3){
    switch(mode){
        case 1:{ //XX
            ROTATOR3D r;
            r.Angle=f1;
            GetTargetPoint3d(cam,r.VctOrg);
            POINT3D org=r.VctOrg;
            TransformPoint3d(hsp,object2d,&org,1,TRANSFORM_WORLD_to_VIEW);
            org.y-=100.0;
            TransformPoint3d(hsp,object2d,&org,1,TRANSFORM_VIEW_to_WORLD);
            POINT3D org3d=org;
            r.VctSize.x=org3d.x-r.VctOrg.x;
            r.VctSize.y=org3d.y-r.VctOrg.y;
            r.VctSize.z=org3d.z-r.VctOrg.z;
            RotatePoint3d(&r,&(cam.poOrg),1);
            RotatePoint3d(&r,&(cam.poUp),1);
        }break;
        case 2:{ //YY
            ROTATOR3D r;
            r.Angle=f1;
            GetTargetPoint3d(cam,r.VctOrg);
            POINT3D org3d;
            POINT3D org=r.VctOrg;
            TransformPoint3d(hsp,object2d,&org,1,TRANSFORM_WORLD_to_VIEW);
            org.x-=100.0;
            TransformPoint3d(hsp,object2d,&org,1,TRANSFORM_VIEW_to_WORLD);
            org3d=org;
            r.VctSize.x=org3d.x-r.VctOrg.x;
            r.VctSize.y=org3d.y-r.VctOrg.y;
            r.VctSize.z=org3d.z-r.VctOrg.z;
            RotatePoint3d(&r,&(cam.poOrg),1);
            RotatePoint3d(&r,&(cam.poUp),1);
        }break;
        case 3:{
            ROTATOR3D r;
            r.Angle=f1;
            r.VctOrg=cam.poOrg;
            if (cam.poFlags&CAMERA_TARGET){
                r.VctSize.x=cam.poDirection.x-cam.poOrg.x;
                r.VctSize.y=cam.poDirection.y-cam.poOrg.y;
                r.VctSize.z=cam.poDirection.z-cam.poOrg.z;
            }else r.VctSize=cam.poDirection;
            RotatePoint3d(&r,&(cam.poUp),1);
        }break;
        case 6:{
            POINT3D p1,p2;
            p1=cam.poOrg;
            GetTargetPoint3d(cam,p2);
            POINT3D d;
            d.x=p1.x-p2.x;
            d.y=p1.y-p2.y;
            d.z=p1.z-p2.z;
            double r=sqrt(d.x*d.x+d.y*d.y+d.z*d.z);
            if(r){
                d.x/=r;d.y/=r;d.z/=r;
                d.x*=f1;d.y*=f1;d.z*=f1;
                cam.poOrg.x+=d.x;
                cam.poOrg.y+=d.y;
                cam.poOrg.z+=d.z;
                if (cam.poFlags&CAMERA_TARGET)
                {
                    cam.poDirection.x+=d.x;
                    cam.poDirection.y+=d.y;
                    cam.poDirection.z+=d.z;
                }
            }
        }break;
        case 10:{
            cam.poExtent.x*=f1;
            cam.poExtent.y*=f1;
            //     cam.poExtent.z/=f1;
        }break;
        case 31:
        case 30:{ //XX - направление взгляда
            ROTATOR3D r;
            r.Angle=f1;
            POINT3D _p;
            GetTargetPoint3d(cam,_p);
            r.VctOrg=cam.poOrg;
            POINT3D org=r.VctOrg;
            TransformPoint3d(hsp,object2d,&org,1,TRANSFORM_WORLD_to_VIEW);
            if(mode==31){
                org.x-=100.0;
            }else {org.y-=100.0;}
            TransformPoint3d(hsp,object2d,&org,1,TRANSFORM_VIEW_to_WORLD);
            POINT3D org3d=org;
            r.VctSize.x=org3d.x-r.VctOrg.x;
            r.VctSize.y=org3d.y-r.VctOrg.y;
            r.VctSize.z=org3d.z-r.VctOrg.z;
            RotatePoint3d(&r,&(_p),1);
            RotatePoint3d(&r,&(cam.poUp),1);
            SetTargetPoint3d(cam,_p);
            //cam.poDirection=_p;
        }break;
        case 42://pan Y
        case 41://pan X
        case 40://pan All
        {
            POINT2D view_size;
            GetObjectOrgSize2d(hsp,object2d,NULL,&view_size);
            double x=cam.poOffset.x*view_size.x;
            double y=cam.poOffset.y*view_size.y;
            switch(mode){
                case 40:x-=f1;y-=f2;break;
                case 41:x-=f1;break;
                case 42:y-=f1;break;
            };

            cam.poOffset.x=x/view_size.x;
            cam.poOffset.y=y/view_size.y;

            /*
      POINT3D o[3];
      o[0]=cam.poOrg;o[1]=cam.poUp;
      GetTargetPoint3d(cam,o[2]);
      TransformPoint3d(hsp,object2d,o,3,TRANSFORM_WORLD_to_VIEW);
      double dx,dy;
       dx=dy=0;
      switch(mode){
       case 40:dx=f1;dy=f2;break;
       case 41:dx=f1;break;
       case 42:dy=f1;break;
      }
      for(int i=0;i<3;i++){
       o[i].x+=dx;o[i].y+=dy;
      }
      TransformPoint3d(hsp,object2d,o,3,TRANSFORM_VIEW_to_WORLD);
      cam.poOrg=o[0];cam.poUp=o[1];
      SetTargetPoint3d(cam,o[2]);
 */
        }break;
        default:return FALSE;
    }
    return TRUE;
};

BOOL   FullDeleteTool2d(HSP2D hsp,TOOL_TYPE t,HOBJ2D obj){
    switch(t){
        case TEXT2D: if (GetToolRef2d(hsp,TEXT2D,obj)==0){
                INT16 count=GetTextNumItems2d(hsp,obj);
                LOGTEXT * Text=NULL;
                if (count){
                    Text=new LOGTEXT[count];
                    GetLogText2d(hsp,obj,Text ,count);
                }
                DeleteText2d(hsp,obj);
                if (count){
                    for(int i=0;i<count;i++){
                        HOBJ2D hstr=Text[i].ltStringIndex;
                        HOBJ2D hfnt=Text[i].ltFontIndex;
                        if (GetToolRef2d(hsp,STRING2D,hstr)==0)DeleteString2d(hsp,hstr);
                        if (GetToolRef2d(hsp,FONT2D,hstr)==0)DeleteFont2d(hsp,hfnt);
                    }
                    if(Text)delete Text;
                }
                return TRUE;
            }break;
        case BRUSH2D:if (obj && GetToolRef2d(hsp,BRUSH2D,obj)==0){
                HOBJ2D hdib=GetBrushDibIndex2d(hsp,obj);
                DeleteBrush2d(hsp,obj);
                if (GetToolRef2d(hsp,DIB2D,hdib)==0)DeleteDib2d(hsp,hdib);
            }break;
        case PEN2D:
        case DIB2D:
        case FONT2D:
        case STRING2D:
        case DOUBLEDIB2D:
        case SPACE3D:if(GetToolRef2d(hsp,t,obj)==0)DeleteTool2d(hsp,t,obj);return TRUE;
    }
    return FALSE;
};

