#include <gwin2d3d\gwintype.h>
#include <gwin2d3d\3drender.h>
#include <math.h>
#include "polygon.h"
#define s_65536uL 256l
#define s_16 8

//#define PutPixel #include "_putpixel.h"
long ScanBytes(int pixWidth, int bitsPixel);
//  *zptr=z;



//ps.txtInfo->biHeight-
//  *_curptr=*bmp;
//*_curptr=((*_curptr)&(0xff000000ul))| ((*bmp)&0xfffffful);

POLYGONstruct ps; // Почти ка HDC :)

void ZbPutPixel(_TPOINT3D *p1){
long z=p1->_z;
 if(ps.checknearclip && z>ps.znearclip)return;
BYTE r,g,b;
 r=p1->color.c.r;
 g=p1->color.c.g;
 b=p1->color.c.b;
long x=p1->_x/256;
long y=p1->_y/256;

union{
 long _HUGE * zptr;
 BYTE _HUGE * _zptr;
};
BYTE _HUGE * curptr=ps.Buffer+((long)((ps.Height-1)-(y-ps.miny)))*((long)ps.perline)+((long)(x-ps.minx)*3L);
zptr=(long _HUGE *)(((BYTE _HUGE *)ps.Zbuffer)+((long)(y-ps.miny))*((long)ps.Zperline)+((long)(x-ps.minx)*4L));
//PutPixel
#include "_putpxl.h"
};


typedef struct tagEDGE3D{
 _TPOINT3D* p1;
 _TPOINT3D* p2;
 tagEDGE3D * next;
 long z1,z2,dz;
 long yfromtop;
 long _dy;
 long dy;
 long y; // Координата y
 long x,dx;
 long c_r,c_g,c_b;
 long d_r,d_g,d_b;
// для текстуры
 long q,s; // координаты текстуры
 long d_q,d_s;
}EDGE3D;

void _edge_calc(EDGE3D*_e)
{
   _e->y+=s_65536uL;
   _e->x+=_e->dx;
   _e->c_r+=_e->d_r;
   _e->c_g+=_e->d_g;
   _e->c_b+=_e->d_b;
   _e->yfromtop++;
}


void _edge_calc_noshade(EDGE3D*_e)
{
   _e->y+=s_65536uL;
   _e->x+=_e->dx;
   _e->yfromtop++;
}


BOOL DrawSolidPolygon(COLOR3D  color){
//return TRUE;
#include "_polygon.h"
};


BOOL tSubDraw(int _cc){
#define TEXTURED_POLYGON
#define TEXTURE_MODE 1
#include "_polygon.h"
#undef TEXTURE_MODE
#undef TEXTURED_POLYGON
};
BOOL tSubDraw1(COLOR3D color){
#define TEXTURED_POLYGON
#define SHADED_POLYGON
#include "_polygon.h"
#undef TEXTURED_POLYGON
#undef SHADED_POLYGON
};
BOOL tSubDraw_256(int _cc){
#define TEXTURED_POLYGON
#define TEXTURE_MODE 1
#define TEXTURE_256
#include "_polygon.h"
#undef TEXTURE_256
#undef TEXTURE_MODE
#undef TEXTURED_POLYGON
};
BOOL tSubDraw1_256(COLOR3D color){
#define TEXTURED_POLYGON
#define SHADED_POLYGON
#define TEXTURE_256
#include "_polygon.h"
#undef TEXTURE_256
#undef TEXTURED_POLYGON
#undef SHADED_POLYGON
};

BOOL tSubDraw2(){
#define TEXTURED_POLYGON
#include "_polygon.h"
#undef TEXTURED_POLYGON
};
BOOL tSubDraw2_256(){
#define TEXTURED_POLYGON
#define TEXTURE_256
#include "_polygon.h"
#undef TEXTURE_256
#undef TEXTURED_POLYGON
};

BOOL tDrawSolidPolygon(_TPRIMITIVE3D*_primitive,PRIMITIVE3D*p,BYTE flags_){
if(p->texturePOINTS){

 ps.txtPoints=p->texturePOINTS;
//return TRUE;
 TMATERIAL3D*_m=r3dGetMaterialTexture(p);
 if(!_m)return NULL;
  ps.txtInfo=_m->bitmap;
 if(!ps.txtInfo){
  // нет текстуры
  if((flags_&2) &&(!(p->flags&PRIMITIVE_NOSHADE))){
   DrawShadedPolygon(p->color);
  }else{
  DrawSolidPolygon(p->color);
  }
  return TRUE;
 }
 ps.txtperline=ScanBytes(ps.txtInfo->biWidth,_m->bitmap->biBitCount);
 ps.txtBits=((char*)ps.txtInfo)+sizeof(BITMAPINFOHEADER);
 ps.txtWidth_1=ps.txtInfo->biWidth-1;
 ps.txtHeight_1=ps.txtInfo->biHeight-1;
 ps.txtWidth=ps.txtInfo->biWidth;
 ps.txtHeight=ps.txtInfo->biHeight;
 if(_m->bitmap->biBitCount==8)
   {
     ps.txtBits+=1024;
   }
 if(_m->flags&TF_CONSTCOLOR){
// без затемнения при поворотах

{

if(_m->bitmap->biBitCount==8){
 tSubDraw2_256();
}else{
 tSubDraw2();
}
}
 }else{
if((flags_&2) &&(!(p->flags&PRIMITIVE_NOSHADE))){
 // shaded
   COLOR3D c;
 c.rgb=RGB(255,255,255);
 if(_m->bitmap->biBitCount==8)tSubDraw1_256(c);else tSubDraw1(c);

}else{
// с затемнением на поворотах
 if(_m->bitmap->biBitCount==8)tSubDraw_256(_primitive->c>>8);
      else tSubDraw(_primitive->c>>8);
}
}
}else{
    COLOR3D c;
  c.rgb=RGB(255,255,255);
  DrawSolidPolygon(c);
}
return FALSE;
};

/*
  Черчение shaded полигона
*/


BOOL DrawShadedPolygon(COLOR3D color){
//return TRUE;
#define SHADED_POLYGON
#include "_polygon.h"
}

BOOL DrawSolidLine(_TPOINT3D *p1,_TPOINT3D *p2,COLORREF color,BREZENHEM *brezenhem){

BYTE r,g,b;
 r=GetRValue(color);
 g=GetGValue(color);
 b=GetBValue(color);

long x=p1->_x/256;
long y=p1->_y/256;
long dx=p2->_x/256-x;
long dy=p2->_y/256-y;

if (dx==0 && dy==0)return FALSE;
long z=p1->_z,z2=p2->_z;
long dz=p2->_z-p1->_z;


long denom,t,e;
long delta_1,delta_2,delta_Z1,delta_Z2;
{
	long xinc=3,yinc=-ps.perline,zxinc=4,zyinc=ps.Zperline;
	if (dx<0){xinc=-3;dx=-dx;zxinc=-4;}
	if (dy<0){yinc=ps.perline;dy=-dy;zyinc=-ps.Zperline;}
	if (dy>dx){long aux=dx;dx=dy;dy=aux;
	// Вытянутее по у
	 delta_1=xinc;delta_Z1=zxinc;
	 delta_2=yinc;delta_Z2=zyinc;
	}else{
	 delta_1=yinc;delta_Z1=zyinc;
	 delta_2=xinc;delta_Z2=zxinc;
	}
}
if (brezenhem){
 denom=brezenhem->denom;
 t=brezenhem->t;
 e=brezenhem->e;
}else{
 denom=dx<<1;
 t=dy<<1;
 e=-dx;
}
BYTE _HUGE * curptr=ps.Buffer+((long)((ps.Height-1)-(y-ps.miny)))*((long)ps.perline)+((long)(x-ps.minx)*3L);
union{
long _HUGE * zptr;
BYTE _HUGE * _zptr;
};
zptr=(long _HUGE *)(((BYTE _HUGE *)ps.Zbuffer)+((long)(y-ps.miny))*((long)ps.Zperline)+((long)(x-ps.minx)*4L));

long _dx=dx;
 dx;
 while(dx-->0)
	{ if(*zptr<z)
		{ //  SetPixel(dc,x,y,color);
		  BYTE _HUGE * _curptr=curptr;
		  *_curptr=b;_curptr++;
		  *_curptr=g;_curptr++;
		  *_curptr=r;
		  *zptr=z;
		 } // Set Pixel End
	 if ((e+=t)>0)
	  {
	   curptr+=delta_1;
	   _zptr+=delta_Z1;
       e-=denom;
      }
      curptr+=delta_2;
      _zptr+=delta_Z2;
      z=z2-dz*dx/_dx;
    }
return TRUE;
};

// Clipping
int  _Code_(long x,long y){
 return (((x<ps.minx)<<3)|((x>ps.maxx)<<2)|((y<ps.miny)<<1)|(y>ps.maxy));
}

BOOL _DrawSolidLine(_TPOINT3D *p1,_TPOINT3D *p2,COLORREF color,BREZENHEM *brezenhem){


long x=p1->_x/256;
long y=p1->_y/256;
long x2=p2->_x/256;
long y2=p2->_y/256;
long dx=x2-x;
long dy=y2-y;

int c1=_Code_(x,y);
int c2=_Code_(x2,y2);
if (!(c1|c2)){
 return DrawSolidLine(p1,p2,color);
}
if (c1&c2)return FALSE; // невидима

BYTE r,g,b;
 r=GetRValue(color);
 g=GetGValue(color);
 b=GetBValue(color);

BOOL vl=0;
if (dx==0 && dy==0)return FALSE;
long z=p1->_z,z2=p2->_z;
long dz=p2->_z-p1->_z;


long denom,t,e;
long delta_1,delta_2,delta_Z1,delta_Z2;
    long xinc=3,yinc=-ps.perline,zxinc=4,zyinc=ps.Zperline;
{

    if (dx<0){xinc=-3;dx=-dx;zxinc=-4;}
    if (dy<0){yinc=ps.perline;dy=-dy;zyinc=-ps.Zperline;}
	if (dy>dx){long aux=dx;dx=dy;dy=aux;
    // Вытянутее по у
     vl=1;
     delta_1=xinc;delta_Z1=zxinc;
     delta_2=yinc;delta_Z2=zyinc;
    }else{
     delta_1=yinc;delta_Z1=zyinc;
     delta_2=xinc;delta_Z2=zxinc;
    }
}

 if (xinc>0)xinc=1;else xinc=-1;
 if (yinc<0)yinc=1;else yinc=-1;

 denom=dx<<1;
 t=dy<<1;
 e=-dx;

BYTE _HUGE * curptr;
union{
long _HUGE * zptr;
BYTE _HUGE * _zptr;
};


long _dx=dx;
 dx;
 BOOL enter=0;
 while(dx-->0)
    {
	int c=_Code_(x,y);
    if (!c){
     if (!enter){
    curptr=ps.Buffer+((long)((ps.Height-1)-(y-ps.miny)))*((long)ps.perline)+((long)(x-ps.minx)*3L);
    zptr=(long _HUGE *)(((BYTE _HUGE *)ps.Zbuffer)+((long)(y-ps.miny))*((long)ps.Zperline)+((long)(x-ps.minx)*4L));
    enter=TRUE;
   }
  }else{
  if (enter){return FALSE;}
  }
    if(enter && *zptr<z)
        { //  SetPixel(dc,x,y,color);
          BYTE _HUGE * _curptr=curptr;
          *_curptr=b;_curptr++;
          *_curptr=g;_curptr++;
          *_curptr=r;
          *zptr=z;
         } // Set Pixel End
     if ((e+=t)>0)
      {
       curptr+=delta_1;
       _zptr+=delta_Z1;
       e-=denom;
       if(vl)x+=xinc;else y+=yinc;
      }
       if(vl)y+=yinc;else x+=xinc;
      curptr+=delta_2;
      _zptr+=delta_Z2;
      z=z2-dz*dx/_dx;
    }
return TRUE;
};


/*
BOOL Clip(POLYGONstruct&ps,_TPOINT3D *p1,_TPOINT3D *p2,BREZENHEM&brezenhem){
long x1,y1,x2,y2,_x1,_y1,_x2,_y2;
 _x1=x1=p1->_x/256;
 _y1=y1=p1->_y/256;
 _x2=x2=p2->_x/256;
 _y2=y2=p2->_y/256;
 int _c1;
 int c1=_c1=_Code_(ps,x1,y1),c2=_Code_(ps,x2,y2);
 long dx,dy;
  while(c1|c2)
   {if (c1&c2)return FALSE; // невидима
    dx=x2-x1;dy=y2-y1;
  if (c1)
  {
    if(x1<ps.minx){y1+=(ps.minx-x1)*dy/dx;x1=ps.minx;}else
    if(x1>ps.maxx){y1+=(ps.maxx-x1)*dy/dx;x1=ps.maxx;}else
    if(y1<ps.miny){x1+=(ps.miny-y1)*dx/dy;y1=ps.miny;}else
    if(y1>ps.maxy){x1+=(ps.maxy-y1)*dx/dy;y1=ps.maxy;}
    c1=_Code_(ps,x1,y1);
   }else{
    if(x2<ps.minx){y2+=(ps.minx-x2)*dy/dx;x2=ps.minx;}else
    if(x2>ps.maxx){y2+=(ps.maxx-x2)*dy/dx;x2=ps.maxx;}else
    if(y2<ps.miny){x2+=(ps.miny-y2)*dx/dy;y2=ps.miny;}else
    if(y2>ps.maxy){x2+=(ps.maxy-y2)*dx/dy;y2=ps.maxy;}
    c2=_Code_(ps,x2,y2);
   }
 }
long z1=p1->_z, z2=p2->_z,_z1,_z2;
 long _dx=_x2-_x1;
 if (_dx){
  _z1=z1+(x1-_x1)*(z2-z1)/_dx;
  _z2=z2+(_x2-x2)*(z1-z2)/_dx;
 }else{
  long _dy=_y2-_y1;
  _z1=z1+(y1-_y1)*(z2-z1)/_dy;
  _z2=z2+(_y2-y2)*(z1-z2)/_dy;
 }
 p1->_z=_z1;p1->_x=x1*256;p1->_y=y1*256;
 p2->_z=_z2;p2->_x=x2*256;p2->_y=y2*256;


 dx=abs(_x2-_x1);dy=abs(_y2-_y1);
 BOOL vl=0;
 int dd=abs(_x1-x1);

 if (dy>dx){long aux=dx;dx=dy;dy=aux; vl=TRUE;dd=abs(_y1-y1);}
 brezenhem.denom=dx;// <<1
 brezenhem.t=dy;// <<1
 brezenhem.e=-dx;


 if (dd){
 while (dd-->0)
   if ((brezenhem.e+=brezenhem.t)>0)
     {
       brezenhem.e-=brezenhem.denom;
     }
 }
return TRUE;
}*/

BOOL DrawPolyline(COLORREF color,BYTE flags){
  INT16 end=ps.count;
  if (!(flags&1))end--;
    for(INT16 k=0;k<end;k++){
      INT16 k2=k+1;
      if ((k2==end)&&(flags&1))k2=0;
    if (flags&2){
     _TPOINT3D p1=*ps.points[ps.nums[k]];
     _TPOINT3D p2=*ps.points[ps.nums[k2]];
    // BREZENHEM brezenhem;

       _DrawSolidLine(&p1,&p2,color,NULL);

    }else
     DrawSolidLine(ps.points[ps.nums[k]],
             ps.points[ps.nums[k2]] ,color);
  }
  return TRUE;
};

PMATERIAL3D (gWINAPI *r3dGetMaterialTexture)(PRIMITIVE3D*)=NULL;
