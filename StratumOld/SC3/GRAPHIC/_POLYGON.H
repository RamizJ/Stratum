// Template to draw polygon
// #define SHADED_POLYGON рисуется SHADED по Гуро


 if (ps.count==0 || ps.count>MAX_IN_PRIMITIVE)return FALSE;

#if !defined(SHADED_POLYGON)
BYTE r,g,b;
#if !defined TEXTURED_POLYGON
r=color.c.r;
g=color.c.g;
b=color.c.b;
#endif
#endif


 EDGE3D e[MAX_IN_PRIMITIVE];
 EDGE3D* _list;
 BYTE k=0;

 for(INT16 i=0;i<ps.count;i++){
  EDGE3D* _e=e+k;
  _e->p1=ps.points[ps.nums[i]];

	  if (i==ps.count-1){
		_e->p2=ps.points[ps.nums[0]];  }else
	  {
		_e->p2=ps.points[ps.nums[i+1]];}

  if (_e->p1->_y!=_e->p2->_y){
#ifdef TEXTURED_POLYGON
BYTE swaped=FALSE;
#endif
  if (_e->p1->_y>_e->p2->_y){ _e->p1=_e->p2;_e->p2=ps.points[ps.nums[i]];
#ifdef TEXTURED_POLYGON
   swaped=TRUE;
#endif
   }


  long _dy=((_e->p2->_y)>>8)-((_e->p1->_y)>>8);
// Просчет всех параметров

 if (_dy){
 _e->dy=_dy<<8;
 _e->_dy=_dy;
 _e->y=0;
 _e->x=(_e->p1->_x)*256;
 _e->dx=((_e->p2->_x-_e->p1->_x)*256)/(_dy);

// Сейчас цвета
#ifdef SHADED_POLYGON
 _e->c_r=_e->p1->c*color.c.r;
 _e->c_g=_e->p1->c*color.c.g;
 _e->c_b=_e->p1->c*color.c.b;

 _e->d_r=_e->p2->c*color.c.r;
 _e->d_g=_e->p2->c*color.c.g;
 _e->d_b=_e->p2->c*color.c.b;

 _e->d_r=(_e->d_r-_e->c_r)/(_dy);
 _e->d_g=(_e->d_g-_e->c_g)/(_dy);
 _e->d_b=(_e->d_b-_e->c_b)/(_dy);
#endif

#ifdef TEXTURED_POLYGON
{
 long Q=ps.txtPoints[i].x*(ps.txtInfo->biWidth-1);
 long S=ps.txtPoints[i].y*(ps.txtInfo->biHeight-1);
 long _Q,_S;
	 if (i==ps.count-1){
	_Q=ps.txtPoints[0].x*(ps.txtInfo->biWidth-1);
	_S=ps.txtPoints[0].y*(ps.txtInfo->biHeight-1);
		  }else
	  {
	_Q=ps.txtPoints[i+1].x*(ps.txtInfo->biWidth-1);
	_S=ps.txtPoints[i+1].y*(ps.txtInfo->biHeight-1);
	}
  if(swaped){
  long b=Q;Q=_Q;_Q=b;
	   b=S;S=_S;_S=b;
  }
 _e->q=Q<<16;_e->s=S<<16;
 _e->d_q = ((_Q-Q)<<16)/_dy;
 _e->d_s = ((_S-S)<<16)/_dy;
}
#endif

 // Z buffer
 _e->z1=_e->p1->_z;
 _e->z2=_e->p2->_z;

 _e->dz=(_e->z2-_e->z1);

 _e->yfromtop=0;
 // Z buffer
  k++;
  }}
 }

 if (k<2)return FALSE;

_list=&(e[0]);
_list->next=0;
 i=1;
 while (i<k){
 EDGE3D**__list=&_list;
 EDGE3D*_e=e+i;
  while ((*__list)&&((*__list)->p1->_y<(_e->p1)->_y)) __list=&((*__list)->next);

  if(*__list){
   _e->next=*__list;
   *__list=_e;
  }else{
   _e->next=NULL;
   *__list=_e;
   }
 i++;
 }

 EDGE3D*_e1=_list;_list=_list->next;
 EDGE3D*_e2=_list;_list=_list->next;

// long y=_e1->p1->_y;
 long _y;
 if (_e2->p1->_y<_e2->p1->_y)_y=_e2->p1->_y>>s_16;else _y=_e1->p1->_y>>s_16;

long psminx=ps.minx;
long psminy=ps.miny;
long psmaxx=ps.maxx;
long psmaxy=ps.maxy;

#ifdef TEXTURED_POLYGON
 long Q,S;
#endif
 while (_e1 && _e2){

 if (_y>psmaxy)return TRUE; // clip maxy

// Чертим линию

 if (_y>=psminy)     // clip miny
  {
// drawline
  if (_e1->x>_e2->x){ //swap
   EDGE3D*__ee=_e1;
   _e1=_e2;_e2=__ee;
  }

  int x1=_e1->x>>16;
  int x2=_e2->x>>16;
  int maxx=x2;
  int minx=x1;
  if (minx<psminx)minx=psminx;  //clip
  if (x2>psmaxx)maxx=psmaxx;    //clip

#ifdef SHADED_POLYGON
  long dr,dg,db;
  union{long r;
  _COLOR3D _cr;};
  union{long g;
  _COLOR3D _cg;};
  union{long b;
  _COLOR3D _cb;};

   r=_e1->c_r;
   g=_e1->c_g;
   b=_e1->c_b;
#endif

  int dx=x2-x1;

BYTE _HUGE * curptr;
long _HUGE * zptr;

 curptr=ps.Buffer+((long)((ps.Height-1)-(_y-psminy)))*((long)ps.perline)+((long)(minx-psminx)*3L);
 zptr=(long _HUGE *)(((BYTE _HUGE *)ps.Zbuffer)+((long)(_y-psminy))*((long)ps.Zperline)+((long)(minx-psminx)*4L));

  long z1=_e1->z1+ _e1->dz*_e1->yfromtop/_e1->_dy;
  long z2=_e2->z1+ _e2->dz*_e2->yfromtop/_e2->_dy;
  long z;
  z=z1;
  if ((x1<=maxx) && (x2>=minx)){ //clip X


#ifdef TEXTURED_POLYGON
 Q=_e1->q;
 S=_e1->s;
#endif


  if (ps.checknearclip){ //clip Z весьма кривой
   BYTE _byte=z1>ps.znearclip;if(z2>ps.znearclip)_byte|=2;
	if(!_byte)goto clipzok;
	 if(_byte!=3){
if(dx){

#ifdef SHADED_POLYGON
   dr=(_e2->c_r-r)/dx;dg=(_e2->c_g-g)/dx; db=(_e2->c_b-b)/dx;
#endif
#ifdef TEXTURED_POLYGON
 long d_Q,d_S;
 {
 long Q1=_e2->q;
 long S1=_e2->s;
 d_Q=(Q1-Q)/dx;
 d_S=(S1-S)/dx;
 }
#endif

  long dz=z2-z1;//  old z
  long xx=x1;
  long dxx=(x2-xx);

   while (TRUE){
	if (x1>=minx){  // clip minx
if(z<ps.znearclip){
#include "_putpxl.h"
 }else{
 curptr+=3;
// if(dz>0)break;
 }
	 zptr++;
	 }              //clip minx
	x1++;
	if (x1>maxx)break; //clip
#ifdef SHADED_POLYGON
   r+=dr;g+=dg;b+=db;
#endif
#ifdef TEXTURED_POLYGON
Q+=d_Q;
S+=d_S;
#endif
	z=z1+dz*(x1-xx)/dxx;// old z
   }
	}}
   }else{
clipzok:
  if (dx){
#ifdef SHADED_POLYGON
   dr=(_e2->c_r-r)/dx;dg=(_e2->c_g-g)/dx; db=(_e2->c_b-b)/dx;
#endif
#ifdef TEXTURED_POLYGON
 long d_Q,d_S;
 {
 long Q1=_e2->q;
 long S1=_e2->s;
 d_Q=(Q1-Q)/dx;
 d_S=(S1-S)/dx;
 }
#endif

  long dz=z2-z1;//  old z
  long xx=x1;
  long dxx=(x2-xx);

   while (TRUE){
	if (x1>=minx){  // clip minx
#include "_putpxl.h"
	 zptr++;
	 }              //clip minx
	x1++;
	if (x1>maxx)break; //clip
#ifdef SHADED_POLYGON
	r+=dr;g+=dg;b+=db;
#endif
#ifdef TEXTURED_POLYGON
   Q+=d_Q;S+=d_S;
#endif

	z=z1+dz*(x1-xx)/dxx;// old z
   }
   }else{
#include "_putpxl.h"
		}
	} // кривой клип по Z
   } // clip  X
  } // clip  min Y
  // drawline
//  y+=s_65536uL;
  _y++;

  if ((_e1->p2->_y>>8)<_y){
	  _e1=_list;
	  if(!_e1)break;
	  if(_list)_list=_list->next;
	 }
#ifdef TEXTURED_POLYGON
_e1->q+=_e1->d_q;
_e1->s+=_e1->d_s;
#endif

#ifdef SHADED_POLYGON
 _edge_calc(_e1);
#else
 _edge_calc_noshade(_e1);
#endif

// EDGE3D*__e2=_e2;
  if ((_e2->p2->_y>>8)<_y){
#ifdef TEXTURED_POLYGON
  _e2->s+=0;
#endif
  _e2=_list;
  if(!_e2)break;
  if(_list)_list=_list->next;
  }
#ifdef TEXTURED_POLYGON
_e2->q+=_e2->d_q;
_e2->s+=_e2->d_s;
#endif
 

#ifdef SHADED_POLYGON
 _edge_calc(_e2);
#else
 _edge_calc_noshade(_e2);
#endif

 }
 return TRUE;