/*
Copyright (c) 1996 TST
Example  of using graphic library for windows

Module Name
	 facemake.cpp
*/

//#define NOGDI            // GDI APIs and definitions
//#define NOUSER           // USER APIs and definitions
#define NOSOUND          // Sound APIs and definitions
#define NOCOMM           // Comm driver APIs and definitions
#define NODRIVERS        // Installable driver APIs and definitions
#define OEMRESOURCE      // OEM Resource values
#define NONLS            // All NLS defines and routines
#define NOSERVICE        // All Service Controller routines, SERVICE_ equates, etc.
#define NOKANJI          // Kanji support stuff.
#define NOMINMAX         // min() and max() macros
#define NOLOGERROR       // LogError() and related definitions
#define NOPROFILER       // Profiler APIs
//#define NOMEMMGR         // Local and global memory management
#define NOLFILEIO        //_l* file I/O routines
#define NOOPENFILE       // OpenFile and related definitions
#define NORESOURCE       // Resource management
#define NOATOM           // Atom management
#define NOLANGUAGE       // Character test routines
//#define NOLSTRING        // lstr* string management routines
#define NODBCS           // Double-byte character set routines
#define NOKEYBOARDINFO   // Keyboard driver routines
#define NOGDICAPMASKS    // GDI device capability constants
//#define NOCOLOR          // COLOR_* color values
//#define NOGDIOBJ         // GDI pens, brushes, fonts
#define NODRAWTEXT       // DrawText() and related definitions
#define NOTEXTMETRIC     // TEXTMETRIC and related APIs
#define NOSCALABLEFONT   // Truetype scalable font support
#define NOBITMAP         // Bitmap support
//#define NORASTEROPS      // GDI Raster operation definitions
#define NOMETAFILE       // Metafile support
#define NOSYSMETRICS     // GetSystemMetrics() and related SM_* definitions
#define NOSYSTEMPARAMSINFO //SystemParametersInfo() and SPI_* definitions
#define NOMSG            // APIs and definitions that use MSG structure
#define NOWINSTYLES      // Window style definitions
#define NOWINOFFSETS     // Get/SetWindowWord/Long offset definitions
#define NOSHOWWINDOW     // ShowWindow and related definitions
#define NODEFERWINDOWPOS  // DeferWindowPos and related definitions
#define NOVIRTUALKEYCODES // VK_* virtual key codes
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

#include "_g_def.h"

#include "facemake.h"
#include "facemake.rh"
#include <math.h>
#include <mem.h>

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

HINSTANCE extern hInstance;

int PointOnLine(TPoint3d *p1,TPoint3d *p2,TPoint3d *p){
 double d1=(p2->x-p1->x)*(p->y-p1->y)-
		  (p->x-p1->x)*(p2->y-p1->y);
	double d2=(p2->x-p1->x)*(p->z-p1->z)-
		  (p->x-p1->x)*(p2->z-p1->z);
	double dd1=fabs(d1);
	double dd2=fabs(d2);
	return((dd1<(1e-8))&&(dd2<(1e-8)));
}
int TFaceMaker::GetPoint(int y,int x){
int k=0,n=0,i;
if (pf[x]<0){
if (pf[x]!=-32767)return (abs(pf[x])-1);
for(i=0;i<x;i++)if (pf[i]<=0)k++;
return k;
}
//*---------------------
if (y==0 ||(close && y==steps-1))
{ // в первой строке
  for(i=0;i<x;i++)if (pf[i]<=0)k++;
}else{
for( i=0;i<x;i++)if (pf[i]==0)k++;
n=(y-1)*pin2+pin1;
}
 if (pf[x]==0)n+=k;else
  n+=(pf[x]-1);
 return n;
};
TFaceMaker::TFaceMaker(TPoint3d * points,int count,ROTATOR3D * r,
	TPoint3d * delta,int asteps,COLOR3D color,int flags
	){
	 rez=0;
fullrect=0;
 object=0;
primall=0;
_points=NULL;
prim=NULL;
steps=asteps;
size=TPoint3d(0,0,0);
org=TPoint3d(0,0,0);
TPoint3d point1,point2;
int k=0,j,i;
INT16 kk=0;
if (steps<2){rez=fmnopoints;goto quit;}
if (r){if (r->Angle==0.0)r=NULL;}
if (delta){if ((*delta)==TPoint3d(0.0,0.0,0.0))delta=NULL;}
{
double a=2.0*M_PI;
close=((delta==NULL) && r && (fabs(r->Angle)==a));
}

if (r){
  point1=r->VctOrg;point2=r->VctSize;
  point2+=point1;
}

tflag=0;
/*
if (close){
if ((PointOnLine(&point1,&point2,&(points[0])) &&
	 PointOnLine(&point1,&point2,&(points[count-1])))||
	 (points[0]==points[count-1])
	)tflag=0;
	}
*/
pf=new int[count];

for(i=0;i<count;i++)pf[i]=0;
for(i=0;i<count;i++){
 if ((delta==NULL) && (r!=NULL)){
	if(PointOnLine(&point1,&point2,points+i))
	{
	 pf[i]=-32767;
	 }
	}
	for(j=i+1;j<count;j++){
	  if (pf[j]==0 && (points[i]==points[j])
	  ) pf[j]=i+1;
	}
 }
pin1=pin2=0;
for(i=0;i<count;i++){
 if(pf[i]==0)pin2++;
 if(pf[i]<=0)pin1++;
 if (i!=count-1){if (pf[i]>=0 && pf[i+1]>=0)fullrect++;}
}

for(i=0;i<count-1;i++)if (pf[i]<0 && pf[i+1]<0)
	 {
	   rez=fmrotator_invalid;goto quit;
	 }
pall=pin1+pin2*(steps-(1+(close?1:0)));
if (pall>(65520l/sizeof(POINT3D))){rez=fmtomanypoints;goto quit;}
_points=new TPoint3d[pall];

// Генерация точек   *** Н А Ч А Л О ***

 for(i=0;i<count;i++)if(pf[i]<=0){_points[k]=points[i];k++;}
 {
ROTATOR3D rr;
if (r){rr=*r;}
int _steps=steps-(close?1:0);
for(i=1;i<_steps;i++)
  {
   int k1=k;
  for(j=0;j<count;j++)if (pf[j]==0)_points[k++]=points[j];
   if(r){
	   rr.Angle=r->Angle*i/((double)(steps-1));
	   RotatePoint3d(&rr,_points+k1,k-k1);
	 }
   if (delta){
	 double d=((double)i)/((double)(steps-1));
	 double dx=delta->x*d,dy=delta->y*d,dz=delta->z*d;
	 for(j=k1;j<k;j++){
		 _points[j].x+=dx;
		 _points[j].y+=dy;
		 _points[j].z+=dz;
	   }
	}
   }
 }

if (k!=pall){rez=fmpoints_invalid;goto quit;}
// Генерация точек   *** К О Н Е Ц ***
k=0;

fulltri=(count -(fullrect+1));
numnum=(fullrect*4+fulltri*3)*(steps-1);
pointnums=new INT16[numnum];

primall=(fullrect+fulltri)*(steps-1);


if (primall>min(65520l/sizeof(PRIMITIVE3D),16380))
							{rez=fmtomanyprimitives;goto quit;}
prim=new PRIMITIVE3D[primall];
memset(prim,0,sizeof(PRIMITIVE3D)*primall);

for(i=0;i<steps-1;i++)
 for(j=0;j<count-1;j++){
  prim[k].color=color;
  prim[k].count=3;
  prim[k].points=pointnums+kk;
  prim[k].flags=PRIMITIVE_POLYGON;
  if(flags&MF_ALLVISIBLE)prim[k].flags|=PRIMITIVE_ALWAYS;
  if(flags&MF_NOSHADEDE)prim[k].flags|=PRIMITIVE_NOSHADE;
  kk+=3;
   BOOL t1=(pf[j]>=0),t2=(pf[j+1]>=0);
   if (t1){
	if (t2){
	 prim[k].count=4;
	 kk+=1;
	 prim[k].points[3]=GetPoint((i+1),j);
	 prim[k].points[2]=GetPoint(i,j);
	 prim[k].points[1]=GetPoint(i,j+1);
	 prim[k].points[0]=GetPoint(i+1,j+1);

	}else{
	 prim[k].points[2]=GetPoint((i+1),j);
	 prim[k].points[1]=GetPoint(i,j);
	 prim[k].points[0]=GetPoint(i,j+1);
	}
   }else
   {
   if (t2){
	 prim[k].points[2]=GetPoint((i),j+1);
	 prim[k].points[1]=GetPoint((i+1),j+1);
	 prim[k].points[0]=GetPoint((i+1),j);

   }
  }
 k++;
 }
if (k!=primall){rez=fmprimitives_invalid;goto quit;}
rez=0;
quit:
delete pf;
pf=NULL;
if (rez!=0){
char s[256];
lstrcpy(s,"");
LoadString(hInstance,rez,s,sizeof(s));
MessageBox(0,s,"Error while making Face" ,MB_TASKMODAL|MB_OK|MB_ICONHAND);}
return;
};

HOBJ2D TFaceMaker::CreateObject(HSP3D h3){
if(rez==0){
  if(!object)object=CreateObject3d(h3);
  AddPoints3d(h3,object,_points,pall);
  CreatePrimitives3d(h3,object,prim,primall);
  return object;
 }
 return 0;
}
void TFaceMaker::CalcSize(){
for(int i=0;i<pall;i++){
if (i==0){
org=_points[i];size=_points[i];
}else{
org.x=min(_points[i].x,org.x);
org.y=min(_points[i].y,org.y);
org.z=min(_points[i].z,org.z);
size.x=max(_points[i].x,size.x);
size.y=max(_points[i].y,size.y);
size.z=max(_points[i].z,size.z);
}}
size-=org;
};
void TFaceMaker::Move(TPoint3d d){
TPoint3d _d=d;
_d-=org;
for(int i=0;i<pall;i++)_points[i]+=_d;
};
TFaceMaker::~TFaceMaker(){
if (_points)delete _points;
if (prim)delete prim;
}
//Создает пирамиду с треуголным основанием
TFaceMaker::TFaceMaker(double height,double radius,COLORREF c){
/*
 rez=0;
 object=0;
 double a=radius/sqrt(3.0);
 pall=4;
 _points=new TPoint3d[pall];
 TRIANGLE3D t[4];
 _points[0]=TPoint3d(0,height,0);
 _points[1]=TPoint3d(a,0,0);
 _points[2]=TPoint3d(-a/2,0,-a*sqrt(3)/2.0);
 _points[3]=TPoint3d(-a/2,0,a*sqrt(3)/2.0);
 triall=4;
 _tri=new TRIANGLE3D[triall];
 _tri[0].a=0;_tri[0].b=3;_tri[0].c=1;
 _tri[1].a=0;_tri[1].b=2;_tri[1].c=3;
 _tri[2].a=0;_tri[2].b=1;_tri[2].c=2;
 _tri[3].a=3;_tri[3].b=2;_tri[3].c=1;
 for(int i=0;i<triall;i++)
 {_tri[i].color=c;_tri[i].flag=TF_EDGES;}
 */
}
void TFaceMaker::_Move(TPoint3d o,int c){
CalcSize();
if (c){
TPoint3d d=size;
d/=2.0;o-=d;
} Move(o);
};

HOBJ2D _EXPORT MakeFace3d(HSP3D hsp3d,HOBJ2D oldobj,POINT3D * points,INT16 count,ROTATOR3D * r,
	POINT3D * delta,INT16 asteps,COLOR3D color,MATERIAL3D *,
	POINT3D *org,UINT32 flags){

 TFaceMaker fm((TPoint3d*)points,count,r,(TPoint3d*)delta,asteps,color,flags);
 fm.SetObject(oldobj);
 HOBJ2D obj3d=fm.CreateObject(hsp3d);
  if (org){
  fm._Move(*(TPoint3d*)org,((flags&256)!=0));
  }

 return obj3d;
 };


