#include <gwin2d3d\gwintype.h>
#include <gwin2d3d\3drender.h>
#include "polygon.h"
#include "_g_def.h"
#include "..\msgcrk.h"
#include "zbuff.rh"
//#define  USE_DRAWDIB

BITMAPINFOHEADER _info_mono={sizeof(_info_mono),
                             1, // Width
                             1, // Height
                             1,
                             1,
                             0,//biCompression;
                             0,300,300, //biX(Y)PelsPerMeter;
                             0,0};
COLORREF _info_mono_rgb[2]={RGB(0,0,0),RGB(255,255,255)};

HINSTANCE hInstance=0;
#ifdef USE_DRAWDIB
#include <vfw\drawdib.h>
HDRAWDIB hdrawdib;
#endif


BOOL InitZBuffer(){
#ifdef USE_DRAWDIB
    hdrawdib= DrawDibOpen();
#endif
    return TRUE;
};

void DoneZBuffer(){
#ifdef USE_DRAWDIB
    DrawDibClose(hdrawdib);
#endif
};
COLORREF GetGDIColor(COLORREF _color){
    if(_color&0x02000000L){
        _color=GetSysColor((int)((0xfffful)&_color));
    }
    return _color;
}

long ScanBytes(int pixWidth, int bitsPixel)
{
    return (((long)pixWidth*bitsPixel+31) / 32) * 4;
}

long _minx,_miny,_maxx,_maxy; // поверхности
long minx,miny,maxx,maxy;     // конкретного примитива

// 1 - clip
// 2 - shaded
// 4 - textured
void DrawPrimitive3d(PRIMITIVE3D *primitive3d,_TPRIMITIVE3D*_primitive,BYTE flags_)
{
    ps.count=primitive3d->count;
    ps.nums=primitive3d->points;
    if(_primitive->flags.wireframe){

        BOOL noclip=(flags_&1)?((minx>=_minx)&&(miny>=_miny)&&(maxx<=_maxx)&&(maxy<=_maxy)):0;
        int flags=(noclip?0:2)|((primitive3d->flags&PRIMITIVE_POLYGON)?1:0);
        if((primitive3d->flags&(PRIMITIVE_POLYGON|PRIMITIVE_CLOSELINE))==PRIMITIVE_CLOSELINE){
            flags|=1;
        }
        // Если чертится линиями
        DrawPolyline(0xfffffful&(_primitive->color.rgb),flags);
    }else{
        if((flags_&4) && primitive3d->flags&PRIMITIVE_TEXTURED && primitive3d->material){
            tDrawSolidPolygon(_primitive,primitive3d,flags_);//_primitive->color
        }else{
            if((flags_&2) &&(!(primitive3d->flags&PRIMITIVE_NOSHADE)))
                DrawShadedPolygon(primitive3d->color);
            else {
                DrawSolidPolygon(_primitive->color);
            }
        }}
}

BOOL _EXPORT DrawSpace3d_Zbuffer(PAINTSTRUCT3D*PS){

    // FillArea(PS);

    ps.minx=PS->srcPaint.left;
    ps.miny=PS->srcPaint.top;
    ps.maxx=PS->srcPaint.right;
    ps.maxy=PS->srcPaint.bottom;
    ps.hdc=PS->hcolorDc;
    ps.Width=(ps.maxx-ps.minx)+1;
    ps.Height=(ps.maxy-ps.miny)+1;
    if ((ps.Width<=0)||(ps.Height<0)){
        return 1;
    }

    ps.perline=ScanBytes(ps.Width,24);
    long size=((long)ps.perline)*(long)ps.Height;
    ps.Zperline=ps.Width*4L;

    long zsize=(long)ps.Zperline*(long)ps.Height;

    HGLOBAL buffer;
    COLORREF BKcolor=GetGDIColor(PS->camera->poBackground);
    if(PS->hmonoDc)BKcolor=0;

    if (BKcolor){
        buffer=GlobalAlloc(GMEM_MOVEABLE,size);
    }else buffer=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,size);

    HGLOBAL Zbuffer=GlobalAlloc(GMEM_MOVEABLE,zsize);

    zsize/=4;

    ps.Buffer=(BYTE*)GlobalLock(buffer);
    ps.Zbuffer=(long*)GlobalLock(Zbuffer);

    BYTE zclip=PS->zclipvalid;

    // Заполнение фона   S T A R T

    if (BKcolor){
        long _color=RGB(GetBValue(BKcolor),GetGValue(BKcolor),GetRValue(BKcolor));
        for(int i=0;i<ps.Height;i++){
            BYTE _HUGE *ptr=((BYTE _HUGE *)ps.Buffer)+((long)ps.perline)*((long)i);
            int last=ps.Width;
            last--;
            for(int j=0;j<last;j++){
                *((long _HUGE *)ptr)=_color;
                ptr+=3;
            }
            ptr[0]=GetBValue(BKcolor);
            ptr[1]=GetGValue(BKcolor);
            ptr[2]=GetRValue(BKcolor);
        }
    }
    // Заполнение фона   E N D


    // Заполнение Z буфера
    UINT32  fmask=0x80000000l;
    {
        if(zclip){
            ps.znearclip=PS->camera->poNear_Clip*(-256.0);
            double fm=(PS->camera->poFar_Clip)*(-256.0);
            if (((long)0x80000000l)<fm)fmask=fm;

        }
        for(long kk=0;kk<zsize;kk++)ps.Zbuffer[kk]=fmask;
    }
    BOOL shaded=(PS->camera->poRenderType&RENDER_SHADED)!=0;
    BOOL textured=(PS->camera->poRenderType&RENDER_TEXTURED)!=0;
    BYTE drawpixel=(PS->camera->poRenderType&RENDER_TYPEMASK)==RENDER_POINTS;

    BYTE _subflag2=(shaded?2:0)|(textured?4:0);

    for(INT16 i=0;i<PS->objcount;i++){
        _OBJECT3D * obj=PS->_objects[i];
        if (!(obj->flags&O3D_NOTINRECT)){
            ps.checknearclip=obj->flags&O3D_ZNEARCONFLICT;
            TIntObject3d*object3d=PS->objects3d[i];


            BYTE _subflag=_subflag2;
            if(shaded){
                if(object3d->options&STATE_NOSHADED)_subflag&=(~2);
            }
            ps.points=(_TPOINT3D**)obj->points->items;
            if(obj->objglags&STATE_POINTS){
                _TPOINT3D __tp[2];
                long _d=256; // чтобы наверху
                for(INT16 i=0;i<obj->points->count;i++){
                    __tp[1]=__tp[0]=*ps.points[i];
                    __tp[0]._z+=_d;__tp[1]._z+=_d;
                    __tp[0]._x-=512;__tp[1]._x+=512;
                    _DrawSolidLine(__tp,__tp+1,RGB(255,255,255),NULL);
                    __tp[0]._x=__tp[1]._x=ps.points[i]->_x;
                    __tp[0]._y-=512;__tp[1]._y+=512;
                    _DrawSolidLine(__tp,__tp+1,RGB(255,255,255),NULL);
                }
            }

            _TPRIMITIVE3D ** _primitives=obj->primitives->items;
            PRIMITIVE3D ** primitives3d=object3d->primitives->items;

            if (obj->flags&O3D_FULLINRECT){
                // Объект полностью видим

                if(drawpixel){ // Ставим точки
                    for(INT16 i=0;i<obj->points->count;i++)ZbPutPixel(ps.points[i]);

                }else{
                    for(INT16 j=0;j<obj->primitives->count;j++){
                        if (_primitives[j]->flags.visible)
                        { //***
                            DrawPrimitive3d(primitives3d[j],_primitives[j],_subflag);
                        }
                    }}
                // Объект полностью видим  END
            }else{
                // Объект видим частично
                // Поэтому перед черчением проверям прямоугольные оболочки
                //  PS->srcPaint;

                _minx=256L*PS->srcPaint.left;
                _maxx=256L*PS->srcPaint.right;
                _miny=256L*PS->srcPaint.top;
                _maxy=256L*PS->srcPaint.bottom;

                if(drawpixel){ // Ставим точки
                    for(INT16 i=0;i<obj->points->count;i++){
                        _TPOINT3D * _tp=ps.points[i];
                        if ((_tp->_x >=_minx && _tp->_x <=_maxx )&&
                            (_tp->_y >=_miny && _tp->_y <=_maxy ))ZbPutPixel(_tp);
                    }
                }else
                {
                    for(INT16 j=0;j<obj->primitives->count;j++){
                        _TPRIMITIVE3D*_primitive=_primitives[j];
                        PRIMITIVE3D *primitive3d=primitives3d[j];
                        if (_primitive->flags.visible)
                        {
                            // Прямоугольная оболочка
                            INT16 *nums=primitive3d->points;
                            minx=maxx=ps.points[nums[0]]->_x;
                            miny=maxy=ps.points[nums[0]]->_y;
                            for(int k=1;k<primitive3d->count;k++)
                            {long x=ps.points[nums[k]]->_x;
                                long y=ps.points[nums[k]]->_y;

                                if (x>maxx)maxx=x;else {if (x<minx)minx=x;}
                                if (y>maxy)maxy=y;else {if (y<miny)miny=y;}
                            }
                            // Прямоугольная оболочка подсчет окончен

                            if(!((minx>_maxx)||(miny>_maxy)||(maxx<_minx)||(maxy<_miny)))  // тест этой оболочки
                            {
                                DrawPrimitive3d(primitive3d,_primitive,_subflag|1);
                            }
                        } //_primitive->flags.visible
                    } //for
                }
                // Объект полностью видим частично END
            }
            ;}
    }

    BITMAPINFOHEADER _info_true={sizeof(_info_true),
                                 ps.Width,
                                 ps.Height,
                                 1,
                                 24,
                                 0,//biCompression;
                                 size,
                                 300,300, //biX(Y)PelsPerMeter;
                                 0,0};
#ifdef USE_DRAWDIB
    DrawDibDraw(hdrawdib,PS->hcolorDc,
                PS->srcPaint.left,PS->srcPaint.top,
                ps.Width,ps.Height,
                (BITMAPINFOHEADER*)&_info_true,ps.Buffer,
                0,0, // SRC ORG
                ps.Width,ps.Height, // SRC SIZE
                DDF_BACKGROUNDPAL|DDF_NOTKEYFRAME
                //DDF_HALFTONE
                );
#else
    SetDIBitsToDevice(PS->hcolorDc,PS->srcPaint.left,PS->srcPaint.top,
                      ps.Width,ps.Height,
                      0,0,0,ps.Height,ps.Buffer,(BITMAPINFO*)&_info_true,DIB_RGB_COLORS);
#endif
    if(PS->hmonoDc){
        _info_mono.biWidth=ps.Width;
        BYTE buf [1024];
        if(ScanBytes(ps.Width,1)<=sizeof(buf)){

            for(int i=0;i<ps.Height;i++){
                long *_zb=((long*)ps.Zbuffer)+ps.Width*i;
                char *_b=buf;
                BYTE b=0;
                BYTE clk=0;
                for(int j=0;j<ps.Width;j++){
                    b<<=1;
                    if(*_zb==fmask)b|=1;
                    _zb++;
                    clk++;
                    if(clk==8){ clk=0;*_b=b;_b++;  }
                }
                if(clk){ b<<=8-clk;*_b=b; }
                //  SetPixel(PS->hmonoDc,PS->srcPaint.top+i,PS->srcPaint.top+i,RGB(255,255,255));
                b=SetDIBitsToDevice(PS->hmonoDc,PS->srcPaint.left,PS->srcPaint.top+i,
                                    ps.Width,1,
                                    0,0,0,1,buf,(BITMAPINFO*)&_info_mono,DIB_RGB_COLORS);
            }
        }}
    GlobalUnlock(buffer);
    GlobalFree(buffer);
    GlobalUnlock(Zbuffer);
    GlobalFree(Zbuffer);
    return TRUE;
};
BOOL CALLBACK DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_INITDIALOG:{
            SetDlgItemText(hwnd,IDC_INFO,"Z buffer driver\r\n======================================\r\nCapabilties and Main Featuers:\r\n • Software Z buffer (optimized for Intel PENTIUM®)\r\n • Filled polygons;\r\n • Smoothed polygons;\r\n • Polylines;\r\n • Z buffer depth - 32 bit;\r\n • Color resolution - 24 bit;\r\n • Shading - Guro;\r\n • Perspective correction texturing;\r\n • Version 1.04\r\nCopyright 1996-1997 Alexander A. Shelemekhov\r\n for Stratum 2000©");
            return TRUE;
        };
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDOK:
                case IDCANCEL:EndDialog(hwnd,1);return TRUE;

            }

        }break;

    }
    return FALSE;
}

INT32 _EXPORT ZBuffer3dMessage(INT16 msg,INT32 lp1,INT32 lp2){
    switch(msg){
        case RM3D_INITDRIVER:{
            HINSTANCE hi=(HINSTANCE)lp1;
            (FARPROC)r3dGetMaterialTexture=GetProcAddress(hi,"r3dGetMaterialTexture");
        };break;
        case RM3D_PAINT      :{
            DrawSpace3d_Zbuffer((PAINTSTRUCT3D*)lp2);
            return TRUE;
        }break;
        case RM3D_DRIVERSETUP:{
            HWND parent=(HWND)lp1;
            //MessageBox(parent,"Отрисовывает полигоны и линии используя Z буфер глубиной 32 бита. Цветовое разрешение 24 бита. Полигоны могут быть сглажены (like Гуро)\n\nCopyright 1996-1997 Alexander Shelemekhov\n for Statum Computer",
            //"Z buffer driver info",MB_OK|MB_ICONINFORMATION);
            DialogBox(hInstance,"DIALOG_ABOUT",parent,DialogProc);
        }return 0;
        case RM3D_GETVERSION:return 0x005;
        case RM3D_GETDRIVERINFO:{
            char info[]="Отрисовывает полигоны и линии используя Z буфер глубиной 32 бита. Цветовое разрешение 24 бита. Полигоны сглаживаются по алгоритму Гуро";
            if (lp1 && lp2){
                lstrcpyn((char*)lp1,info,lp2);
                return TRUE;
            }else return lstrlen(info);
        }break;
        case RM3D_GETCAPS    :{
            CAMERA3D*camera=(CAMERA3D*)lp2;
            int type=camera->poRenderType&RENDER_TYPEMASK;
            switch(lp1){
                case  caps3d_facecolor  :return 1;
                case  caps3d_vertexcolor: if (type==RENDER_POINTS)return 1;
                    if((camera->poRenderType&RENDER_SHADED))return 2;
                case  caps3d_faceabcd   :return 0;
                case  caps3d_vertexc    :return 0;
                case  caps3d_version    :return 0x100;
                case  caps3d_drawonDIB  :return 0;
                case  caps3d_drawonDC   :return TRUE;
                case  caps3d_minrect:   return TRUE;
                case  caps3d_points     :
                case  caps3d_faces      :return TRUE;
                case  caps3d_bounds     :return FALSE;

                default:return FALSE;
            }
        }break;
        case RM3D_GETDRAWPROC:{
            CAMERA3D*camera=(CAMERA3D*)lp2;
            int type=camera->poRenderType&RENDER_TYPEMASK;
            if(type==RENDER_BOUNDED)return 0;
            return (INT32)DrawSpace3d_Zbuffer;


        }break;
    }
    return 0;
};
//#ifdef USE_DRAWDIB

#if defined(WIN32)
//#pragma argsused
BOOL _EXPORT DllEntryPoint( HINSTANCE hinstDll,
                            DWORD fdwRreason,
                            LPVOID plvReserved)
#else /* not flat model  */
int FAR _pascal LibMain( HINSTANCE hInstance,
                         WORD ,
                         WORD wHeapSize,
                         LPSTR  )
#endif /* __FLAT */
{
#ifdef  WIN32

    switch(fdwRreason){
        case DLL_PROCESS_ATTACH:{
            ::hInstance=hinstDll;
            return InitZBuffer();// Indicate that the DLL was initialized successfully.
        }
        case DLL_PROCESS_DETACH:{
            //   exiting=1;
            DoneZBuffer();
            return TRUE;
        }
    }
    return TRUE;
#else
    if ( wHeapSize != 0 ) UnlockData( 0 );
    ::hInstance=hInstance;
    return InitZBuffer();// Indicate that the DLL was initialized successfully.
#endif

}

#if !defined(WIN32)
int CALLBACK WEP(int nExitType){
    //if (nExitType==WEP_FREE_DLL)
    DoneZBuffer();
    return 1;
}
#endif
//#endif

