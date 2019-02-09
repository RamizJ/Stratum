/*
Copyright (c) 1995  Virtual World
Module Name:
			space3d.cpp
Author:
			Alexander Shelemekhov
*/
//#define NOKERNEL         // KERNEL APIs and definitions
//#define NOGDI             GDI APIs and definitions
//#define NOUSER           // USER APIs and definitions
#define NOSOUND          // Sound APIs and definitions
#define NOCOMM           // Comm driver APIs and definitions
//#define NODRIVERS        // Installable driver APIs and definitions
//#define OEMRESOURCE      // OEM Resource values
#define NONLS            // All NLS defines and routines
#define NOSERVICE        // All Service Controller routines, SERVICE_ equates, etc.
#define NOKANJI          // Kanji support stuff.
#define NOMINMAX         // min() and max() macros
#define NOLOGERROR       // LogError() and related definitions
#define NOPROFILER       // Profiler APIs
#define NOMEMMGR         // Local and global memory management
#define NOLFILEIO        //_l* file I/O routines
#define NOOPENFILE       // OpenFile and related definitions
#define NORESOURCE       // Resource management
#define NOATOM           // Atom management
#define NOLANGUAGE       // Character test routines
#define NOLSTRING        // lstr* string management routines
#define NODBCS           // Double-byte character set routines
#define NOKEYBOARDINFO   // Keyboard driver routines
#define NOGDICAPMASKS    // GDI device capability constants
//#define NOCOLOR          // COLOR_* color values
//#define NOGDIOBJ         // GDI pens, brushes, fonts
#define NODRAWTEXT       // DrawText() and related definitions
#define NOTEXTMETRIC     // TEXTMETRIC and related APIs
#define NOSCALABLEFONT   // Truetype scalable font support
//#define NOBITMAP         // Bitmap support
//#define NORASTEROPS      // GDI Raster operation definitions
#define NOMETAFILE       // Metafile support
#define NOSYSMETRICS     // GetSystemMetrics() and related SM_* definitions
#define NOSYSTEMPARAMSINFO //SystemParametersInfo() and SPI_* definitions
#define NOMSG            // APIs and definitions that use MSG structure
#define NOWINSTYLES      // Window style definitions
#define NOWINOFFSETS     // Get/SetWindowWord/Long offset definitions
#define NOSHOWWINDOW     // ShowWindow and related definitions
#define NODEFERWINDOWPOS  // DeferWindowPos and related definitions
//#define NOVIRTUALKEYCODES // VK_* virtual key codes
#define NOKEYSTATES       // MK_* message key state flags
#define NOWH              // SetWindowsHook and related WH_* definitions
#define NOMENUS           // Menu APIs
#define NOSCROLL          // Scrolling APIs and scroll bar control
#define NOCLIPBOARD       // Clipboard APIs and definitions
#define NOICONS           // IDI_* icon IDs
//#define NOMB              // MessageBox and related definitions
#define NOSYSCOMMANDS     // WM_SYSCOMMAND SC_* definitions
#define NOMDI             // MDI support
#define NOCTLMGR          // Control management and controls
#define NOWINMESSAGES     // WM_* window messages
#define NOHELP            // Help support

#include "windows.h"
#include "space3d.h"
#include "scan_typ.h"
#define MAXD 8
#define tfcutz tfvisible
#define tfmaxz tfcyber

const long Zbuf_initial=(-1l<<31);
static long maskintc=0x7fffffffl;
                   //0x0fdfffffl;
//0xff800000ul;
BITMAPINFOHEADER _info_true={sizeof(_info_true),
			1024/*dx*/,1,1,24,0,1024*3/*dx*3*/,300,300,0,0};
BITMAPINFOHEADER _info_mono={sizeof(_info_mono),
			1024/*dx*/,1,1,1,0,1024*3/*dx*3*/,300,300,0,0};
static RGBQUAD _info_mono_rgb[2]={{0,0,0},{255,255,255}};
//_info_mono_rgb должно идти за _info_mono

void Add_triangle(_triangle**base,_triangle* t){
while (*base){
_triangle*b=*base;
if (b->maxy<t->maxy)base=(&(b->max));else{
 if (b->maxy>t->maxy)base=(&(b->min));else{
  t->equal=b->equal;b->equal=t;return;}
  }
}
*base=t;
};
void Add_line(_line**base,_line* l){
while (*base){
_line*b=*base;
if (b->maxy<l->maxy)base=(&(b->max));else{
 if (b->maxy>l->maxy)base=(&(b->min));else{
  l->equal=b->equal;b->equal=l;return;}
  }
}
*base=l;
};
_triangle * _list;
_triangle **__list;
_triangle * _edges;
_line * _linelist;
_line **__linelist;
_line * _lineedges;

long *zbuf;// Z буфер для одной строки.
TColor * scrbuf;//буфер экрана для одной строки.
BYTE * andmask=NULL;
void _line::GetK(int l){
if (l<maxy){
 int d=l-maxy;
 leftZ+=delta_Zy*d;
 xleft+=delta_Xl*d;
// int _xl=(_newxl-xleft)>>16;
// if (_xl){leftZ+=delta_Zx*_xl;}
 int miny;
  if (t1->iy<t2->iy)miny=t1->iy;else miny=t2->iy;
  maxy=l-miny;
}else maxy=abs((int)(t1->iy-t2->iy));
};
void _triangle::GetFirstK(){
PPoint3d top=NULL,p1,p2;
BYTE le;
switch(ExFlag.top){
 case 1:top=t1;p1=t2;p2=t3;le=2;
		 break;
 case 2:top=t2;p1=t1;p2=t3;le=3;
		 break;
 case 3:top=t3;p1=t1;p2=t2;le=1;
		 break;
}
int topy=top->iy;
if (p2->iy==maxy){PPoint3d pp=p1;p1=p2;p2=pp;goto m1;}
if (p1->iy==maxy){// \/
m1:
 if (p2->iy==top->iy){
 delta_Xl=0;delta_Xr=0;
 long mi,ma;
 if (t1->ix>t2->ix){ma=t1->ix;mi=t2->ix;}else{ma=t2->ix;mi=t1->ix;}
 if (t3->ix<mi){mi=t3->ix;}else{ if(t3->ix>ma){ma=t3->ix;} }
 xleft=mi<<16;xright=ma<<16;maxy=1;
 }else{
 delta_Xl=((p2->ix-p1->ix)<<16)/(p1->iy-p2->iy);
 delta_Xr=((p2->ix-top->ix)<<16)/(top->iy-p2->iy);
 xleft=p1->ix<<16;xright=top->ix<<16;
 maxy=topy-p2->iy;
 if (xleft>xright){long b=xright;xright=xleft;xleft=b;
	  b=delta_Xr;delta_Xr=delta_Xl;delta_Xl=b;
	  leftZ=top->iz;
	  }else leftZ=p1->iz;}
 ExFlag.last_edge=0;
}else{
 xleft=(top->ix)<<16;xright=xleft;
 delta_Xl=((p1->ix-top->ix)<<16)/(top->iy-p1->iy);
 delta_Xr=((p2->ix-top->ix)<<16)/(top->iy-p2->iy);
 if (p1->iy==p2->iy){ // /\
 maxy=topy-p2->iy;
 leftZ=top->iz;
  if (delta_Xl>delta_Xr){long b=delta_Xl;delta_Xl=delta_Xr;delta_Xr=b;}
 ExFlag.last_edge=0;
 }else{
 if (delta_Xl>delta_Xr){long register b=delta_Xl;delta_Xl=delta_Xr;delta_Xr=b;
 PPoint3d pp=p2;p2=p1;p1=pp;
 }
 leftZ=top->iz;
 if (p1->iy<p2->iy){ExFlag.left_end=0;}
 else {p2=p1;ExFlag.left_end=1;}
 ExFlag.last_edge=le;
 maxy=topy-p2->iy;
 }}
};
void _triangle::GetSubFirstK(int l){
PPoint3d top=NULL;
switch(ExFlag.top){
 case 1:top=t1;break;
 case 2:top=t2;break;
 case 3:top=t3;break;
 default: return;
}
if (l<top->iy){
int dl=top->iy-l;
if (dl>maxy){// если после ребра
long oldxl=xleft;
xleft+=delta_Xl*(maxy-1);
xright+=delta_Xr*(maxy-1);
dl-=maxy;
if (!(ExFlag.left_end)){
leftZ-=delta_Zy*(maxy-1);
int _xl=HIWORD(xleft)-HIWORD(oldxl);
if (_xl){leftZ+=delta_Zx*_xl;}
}
GetSecondK();
}
long xl=xleft;
xleft+=delta_Xl*dl;
xright+=delta_Xr*dl;
leftZ-=delta_Zy*dl;
int _xl=HIWORD(xleft)-HIWORD(xl);
 if (_xl){leftZ+=delta_Zx*_xl;}
maxy-=dl;
}
};
void _triangle::GetSecondK(){
PPoint3d pp,pm;
if (t1->iy<t2->iy){pp=t1;pm=t2;}else {pp=t2;pm=t1;}
if (t3->iy<pp->iy){pm=pp;pp=t3;}else{
if (pm->iy>t3->iy)pm=t3;
}
if (ExFlag.left_end)
		  {xright+=delta_Xr;xleft=(pm->ix)<<16;
			delta_Xl=((pp->ix-pm->ix)<<16)/(pm->iy-pp->iy);
			leftZ=pm->iz;
		  }else
		  {long xl=xleft;
			xleft+=delta_Xl;xright=(pm->ix)<<16;
			delta_Xr=((pp->ix-pm->ix)<<16)/(pm->iy-pp->iy);
			leftZ-=delta_Zy;
			int _xl=HIWORD(xleft)-HIWORD(xl);
			if (_xl){leftZ+=delta_Zx*_xl;}
		  }
maxy=pm->iy-pp->iy;
ExFlag.last_edge=0;
};
//*---------------------------------------------------*
void far AddToEdgesList(){
_triangle * _l=_list;
while (_l){_l->GetFirstK();_l=_l->equal;}
__list=&(_list->equal);
while ((*__list)!=NULL)__list=&((*__list)->equal);
*__list=_edges;
_edges=_list;
_list=_list->max;
_edges->max=NULL;_edges->min=NULL;
};

void far AddToLineEdgesList(int y){
_line * _l=_linelist;
while (_l){_l->GetK(y);
			  _l=_l->equal;}
__linelist=&(_linelist->equal);
while ((*__linelist)!=NULL)__linelist=&((*__linelist)->equal);
*__linelist=_lineedges;
_lineedges=_linelist;
_linelist=_linelist->max;
_lineedges->max=NULL;_lineedges->min=NULL;
};

void near _GetList(_triangle*base){
if (base->max)_GetList(base->max);
*__list=base;
__list=&(base->max);base->max=NULL;
if (base->min)_GetList(base->min);
base->min=NULL;
}

void near _GetLineList(_line*base){
if (base->max)_GetLineList(base->max);
*__linelist=base;
__linelist=&(base->max);base->max=NULL;
if (base->min)_GetLineList(base->min);
base->min=NULL;
}
inline BOOL int_intersect(int a,int b,int c,int d){
 if (a<=c)return b>c;else return d>a;
  };
void TSpace3d::ViewScanInter(){
PreScan();
 if (_list==NULL)return;

};
void ScanLinesZmain(int y,int updaterect_bottom,
int updaterect_left,int deltax,int zbufsize,int scrbufsize,long _cut_z,
HDC dcmono,HDC dc){

while(y>updaterect_bottom){
 if (_list && _list->maxy==y)AddToEdgesList();//tri
 if (_linelist && _linelist->maxy==y)AddToLineEdgesList(y);//line
 if (_edges||_lineedges){
int i,j;
_triangle * t=_edges,* _lt=NULL;
_line * l=_lineedges,* _ll3=NULL;
for(i=0;i<zbufsize;i++){zbuf[i]=Zbuf_initial;}
for(i=0;i<scrbufsize;i++)((long*)scrbuf)[i]=0l;
TColor color;
// для линий
while (l){
int i=HIWORD(l->xleft);
long _newxl=l->xleft+l->delta_Xl;
int end =HIWORD(_newxl);
if (i>end){int b=end;end=i;i=b;}
end=min(deltax-1,end-updaterect_left);
if (end<0)goto end_oflineout;
 long Z;
 if (i<updaterect_left)
 {Z=l->leftZ+l->delta_Zx*(updaterect_left-i);i=0;}else
  {Z=l->leftZ;i-=updaterect_left;}
 if (i<0)i=0;
 color.b=GetBValue(l->color);
 color.g=GetGValue(l->color);
 color.r=GetRValue(l->color);
//*---  Вывод сканирующей строки для линии ---*//
 while (i<=end){
  if (Z>zbuf[i]){zbuf[i]=Z;scrbuf[i]=color;}
  Z=Z+=l->delta_Zx;
 i++;}
end_oflineout:
if((--(l->maxy))>0){l->xleft=_newxl;
 l->leftZ+=l->delta_Zy;
 //int _xl=(_newxl-l->xleft)>>16;
 //if (_xl){l->leftZ+=l->delta_Zx*_xl;}
 }else
  {if (_ll3)_ll3->equal=l->equal;else _lineedges=l->equal;
			l=l->equal;
			continue;// линия удаляется
		  }
  _ll3=l;l=l->equal;
}
// для треугольников
while (t){
 int i=HIWORD(t->xleft);
 int end = min(deltax-1,(t->xright>>16)-updaterect_left);
 if (end<0)goto end_ofout;
 long Z;
 if (i<updaterect_left)
 {Z=t->leftZ+t->delta_Zx*(updaterect_left-i);i=0;}else
  {Z=t->leftZ;i-=updaterect_left;}
 if (i<0)i=0;
 //TColor color=&((TColor*)((void*)(((char*)&(t->outcolor))+1)));
 color.b=GetBValue(t->outcolor);
 color.g=GetGValue(t->outcolor);
 color.r=GetRValue(t->outcolor);
 //*---  Вывод сканирующей строки для треугольника ---*//
 if (t->delta_Zx){//если Z меняется

 if(t->flag&tfmaxz){
 while (i<=end){
	 if ((Z>zbuf[i])&&(Z<_cut_z)){

		 if (Z>t->max_Z)zbuf[i]=t->max_Z; else zbuf[i]=Z;
		 scrbuf[i]=color;
	  }
	 Z=Z+=t->delta_Zx;i++;
  }
  }else {

 if (t->flag&tfcutz){
 while (i<=end){
  if ((Z>zbuf[i])&&(Z<_cut_z)){zbuf[i]=Z;scrbuf[i]=color;}
  Z=Z+=t->delta_Zx;i++;}}
 else{
 while (i<=end){
  if (Z>zbuf[i]){zbuf[i]=Z;scrbuf[i]=color;}
  Z=Z+=t->delta_Zx;
 i++;}}
 }
 }else{
if (Z<_cut_z)
while (i<=end){if (Z>zbuf[i]){zbuf[i]=Z;scrbuf[i]=color;}i++;}}
 //*--------  Вывод сканирующей строки ---------------*//
end_ofout:
 if((--(t->maxy))>0){t->xright+=t->delta_Xr;
 long xl=t->xleft;t->xleft+=t->delta_Xl;
// xl=t->xleft-xl;
 t->leftZ-=t->delta_Zy;
 int _xl=HIWORD(t->xleft)-HIWORD(xl);
 if (_xl){t->leftZ+=t->delta_Zx*_xl;}
 }else
  {if (t->ExFlag.last_edge)t->GetSecondK();else
		  {
			if (_lt)
			  _lt->equal=t->equal;
			  else _edges=t->equal;
			t=t->equal;
			continue;// треугольник удаляется
		  }
  }_lt=t; t=t->equal;
 }

if (andmask){

BYTE m=0;int j=0,k=0;
for(i=0;i<deltax;i++){m<<=1;if (zbuf[i]==Zbuf_initial)m|=0x01u;j++;
  if(j==8){andmask[k]=m;m=0;j=0;k++;}
}if (j){andmask[k]=m<<(8-j);}
//FillAndMask(zbuf,andmask,Zbuf_initial,dx);
SetDIBitsToDevice(dcmono,updaterect_left,y,deltax,1,
 0,0,0,1,andmask,(BITMAPINFO*)&_info_mono,DIB_RGB_COLORS);
}
SetDIBitsToDevice(dc,updaterect_left,y,deltax,1,
 0,0,0,1,scrbuf,(BITMAPINFO*)&_info_true,DIB_RGB_COLORS);

}
 y--;
}}

void TSpace3d::ViewScanZbuff(){
int dx=updaterect.right-updaterect.left;
if (dx>(65500u/sizeof(TColor)))
  {_Error(EM_tobiglinebuffer);dx=(65500u/sizeof(TColor));}
	PreScan();
  if (_list==NULL&&_linelist==NULL)return;
_edges=NULL;
if (dcmono){
_info_mono.biWidth=dx;
_info_mono.biSizeImage=(dx/8)+2;
andmask=new BYTE[_info_true.biSizeImage];
}
int zbufsize=dx+2;
int scrbufsize=(dx+2)*3/4+1;
zbuf=new long[zbufsize];
scrbuf =(TColor*) new long[scrbufsize];
 _info_true.biWidth=dx;
 _info_true.biSizeImage=dx*3;

int y=updaterect.top;
while(_list &&_list->maxy>y)AddToEdgesList();//tri
{_triangle * t=_edges;while (t){t->GetSubFirstK(y);t=t->equal;}}
while(_linelist &&_linelist->maxy>y)AddToLineEdgesList(y);//line
//-- Уже пошла обработка для всех строк
ScanLinesZmain(updaterect.top,updaterect.bottom,
updaterect.left,dx,zbufsize,scrbufsize,_cut_z,dcmono,dc);
delete zbuf;
delete scrbuf;
if (andmask)delete andmask;
_done:
};
void TSpace3d::PreScan(){
_list=NULL;
_linelist=NULL;
_lineedges=NULL;
_edges=NULL;
if (sizeof(_triangle)>sizeof(TTriangle))return;
long _cut_z=cut_z*65536ul;//*tofloat;
_triangle * base=NULL;
_line * linebase=NULL;
for(int i=0;i<primary->count;i++){
PObject3d po=(PObject3d)primary->At(i);
int point_count=po->points->count;
if ((!(po->options&1))&&(point_count)){
PPoint3d pp=(PPoint3d)po->points->_At(0);
pp->IntPerspective();
int minx=(int)(pp->ix),miny=(int)(pp->iy),maxx=minx,maxy=miny;
long minz=pp->iz,maxz=minz;
for(int j=1;j<point_count;j++){
PPoint3d pp=(PPoint3d)po->points->_At(j);
pp->IntPerspective();
if (minx>(int)(pp->ix))minx=(int)pp->ix;else
			 {if(maxx<(int)(pp->ix))maxx=(int)pp->ix;}
if (miny>(int)(pp->iy))miny=(int)pp->iy;else
			 {if(maxy<(int)(pp->iy))maxy=(int)pp->iy;}
if (pp->iz<minz)minz=pp->iz;else
			 {if(pp->iz>maxz)maxz=pp->iz;}
}// нашли прямоугольную оболочку объекта
int testx,testy;//2 - целиком внутри ,1 - пересекает
if (int_intersect(minx,maxx,updaterect.left,updaterect.right)){
  if (minx>=updaterect.left && maxx<=updaterect.right)testx=2;else
  testx=1;}else testx=0;
if (int_intersect(miny,maxy,updaterect.bottom,updaterect.top)){
if (miny>=updaterect.bottom && maxy<=updaterect.top)testy=2;else
 testy=1;}else testy=0;
if ((_cut_z>minz)&&testx&&testy){
 BOOL always=maxz>_cut_z;//треугольники всегда видимы
 if (po->triangles->count){
 int triangle_count=po->triangles->count;
 for(j=0;j<triangle_count;j++){
  _triangle  *t=(_triangle*)po->triangles->_At(j);
  PPoint3d t1=t->t1,t2=t->t2,t3=t->t3;
  double c=(t1->_x*(t2->_y-t3->_y)-t2->_x*(t1->_y-t3->_y)+
			 t3->_x*(t1->_y - t2->_y));
  if ((c>0.0)|| ((t->flag & tfalways)!=0)||always ){
  t->flag&=tfmask;
		  if (always){
		  if (t1->iz>_cut_z&&t2->iz>_cut_z&&t3->iz>_cut_z)continue;
		  if (t1->iz>_cut_z||t2->iz>_cut_z||t3->iz>_cut_z)t->flag|=tfcutz;
		  }
double t2_t3z =(t2->_z-t3->_z);
double t1_t3z =(t1->_z-t3->_z);
double t1_t2z =(t1->_z-t2->_z);

		  double a=t1->_y*t2_t3z - t2->_y*t1_t3z + t3->_y*t1_t2z;
		  double b=-(t1->_x*t2_t3z-t2->_x*t1_t3z + t3->_x*t1_t2z);
		  double r=sqrt(a*a+b*b+c*c);
			if (r>0.0){
		  long intc=MAXLONG*(fabs(c/r));

		  t->delta_Zx=(-a/c)*65536ul;
		  t->delta_Zy=(-b/c)*65536ul;

		  if (intc<maskintc){
			 double mz=t1->_z;
			  if (mz<t2->_z)mz=t2->_z;
			  if (mz<t3->_z)mz=t3->_z;
			  t->max_Z=mz*65536ul;
			  t->flag|=tfmaxz;
		  }
		  if (testx!=2){
			minx=(int)t1->ix;maxx=(int)t1->ix;
			if (minx>(int)t2->ix)minx=(int)t2->ix;else{
			 if (maxx<(int)t2->ix)maxx=(int)t2->ix;}
			if (minx>(int)t3->ix)minx=(int)t3->ix;else{
			 if (maxx<(int)t3->ix)maxx=(int)t3->ix;}
			 if (maxx<=updaterect.left||minx>=updaterect.right)continue;
			}//здесь отбросили треугольник если он не подходит по X.
			BYTE top;
			if (t1->iy<t2->iy){miny=t1->iy;maxy=t2->iy;top=2;}else
					  {miny=t2->iy;maxy=t1->iy;top=1;}
			if (miny>(int)t3->iy)miny=(int)t3->iy;else{
			 if (maxy<(int)t3->iy){maxy=(int)t3->iy;top=3;}}
			 if (maxy<=updaterect.bottom||miny>=updaterect.top)continue;
			 t->maxy=maxy;//t->deltay=maxy-miny;
			 t->ExFlag.top=top;
			//раз долшли до сюда , то значит треугольник видим.
			t->CalcColor(intc);
// if (palette)outcolor=PALETTEINDEX(GetNearestPaletteIndex(pal,outcolor));
 //Вычислили цвет отрисовки
		  t->min=t->max=t->equal=NULL;
		  Add_triangle(&base,t);
 }}
}}//для треугольников закончили
// Теперь для линий
if (po->lines->count){
for(j=0;j<po->lines->count;j++){
_line * l=(_line*)po->lines->At(j);
  if (always){ // тест по Z координате
		if (l->t1->iz>_cut_z&&l->t2->iz>_cut_z)continue;
		if (l->t1->iz>_cut_z||l->t2->iz>_cut_z){
		//Сразу режем линию


		}
	}
  if (testx!=2){ // тест по X координате
	int minx,maxx;
	if (l->t1->ix<l->t2->ix){minx=(int)l->t1->ix;maxx=(int)l->t2->ix;}
	 else {minx=(int)l->t2->ix;maxx=(int)l->t1->ix;}
	 if (maxx<=updaterect.left||minx>=updaterect.right)continue;
  }
  PPoint3d p1,p2;//p1->iy>p2->iy
//  (p1)-----
//				 ------
//						 ------
//								 ----(p2)
  if (l->t1->iy>l->t2->iy){p1=l->t1;p2=l->t2;}else{p1=l->t2;p2=l->t1;}
  if (((int)p1->iy)<updaterect.bottom||((int)p2->iy)>updaterect.top)continue;
  l->min=l->max=l->equal=NULL;
  l->xleft=(p1->ix<<16);
  l->leftZ=p1->iz;
  l->maxy=(int)p1->iy;
  if (p1->iy-p2->iy){
	 l->delta_Xl=((p2->ix-p1->ix)<<16)/(p1->iy-p2->iy);
	 l->delta_Zy=65536.0*((p2->_z-p1->_z)/(p1->_y-p2->_y));
	 if (p1->ix!=p2->ix)
	 l->delta_Zx=65536.0*((p2->_z-p1->_z)/(p1->_x-p2->_x));else l->delta_Zx=0;
	}else
			 {l->delta_Xl=(p2->ix-p1->ix)<<16;l->delta_Zx=0;l->delta_Zy=0;}
	Add_line(&linebase,l);
  }}
}}
};
// Закончили сортировку треугольников и линий для всех объектов
if (base!=NULL){
_list=NULL;
__list=&(_list);
_GetList(base);
}
if (linebase!=NULL){
_linelist=NULL;
__linelist=&(_linelist);
_GetLineList(linebase);
}


/* Получлил список
_list->T[].equal-->T[].equal->...T[].equal==NULL
			 .max
					  |
					  V
			 T[].equal-->T[].equal->...T[].equal==NULL
*/
};

