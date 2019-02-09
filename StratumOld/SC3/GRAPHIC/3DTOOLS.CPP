/*
Copyright (c) 1995  Virtual World
Module Name:
	 3dtools.h
Author:
	 Alexander Shelemekhov
Description:
 .3ds file import routine
*/

#include <math.h>
#include <mem.h>
#include <gwin2d3d\gwinfunc.h>
#include "3dsbin.h"
#include "_g_def.h"
#ifdef WIN32
#define TO_OPENREAD    OF_READ|OF_SHARE_DENY_WRITE
#else
#define TO_OPENREAD    READ|OF_SHARE_DENY_WRITE
#endif
#define  FROM_CURRENT 1
#define  FROM_END     2

HOBJ2D WINAPI MakeFace3d(HSP3D hsp3d,HOBJ2D ,POINT3D * points,INT16 count,ROTATOR3D * r,
	POINT3D * delta,INT16 asteps,COLOR3D color,MATERIAL3D *m,
	POINT3D *org,UINT32 flags);

HFILE hfile;
HSP3D _hsp3d;
HOBJ2D _hobj3d;
HOBJ2D _hgroup;
UINT16  version;
COLOR3D bkcolor;
BOOL    setbk;

class TMAT3D{
public:
 TMAT3D*left;
 TMAT3D*right;
 char name[128];
 COLOR3D ambient,specular,diffuse;
 BOOL twosided;
 TMAT3D*GetMat(char*);
 TMAT3D(char*);
 TMAT3D();
 ~TMAT3D();
 void Insert(TMAT3D*);
};

void TMAT3D::Insert(TMAT3D*m){
int i=lstrcmpi(name,m->name);
 if (i<0){
  if (left)left->Insert(m);else left=m;
  return;
 }
 if (i>0){
  if (right)right->Insert(m);else right=m;
  return;
 }
 delete m;
};
TMAT3D*TMAT3D::GetMat(char*s){
int i=lstrcmpi(name,s);
 switch(i){
  case -1:if (left)return left->GetMat(s);return NULL;
  case 0:return this;
  case 1:if (right)return right->GetMat(s);return NULL;
 }
 return NULL;
};
TMAT3D::TMAT3D(){
 memset(this,0,sizeof(TMAT3D));
 diffuse.rgb=RGB(255,255,255);
};

TMAT3D::TMAT3D(char*s){
 memset(this,0,sizeof(TMAT3D));
 lstrcpy(name,s);
};
TMAT3D::~TMAT3D(){
if(left)delete left;
if(right)delete right;
};

TMAT3D*material=0;
TMAT3D*defmat=0;

void Insertmaterial(TMAT3D* m){
if (material)material->Insert(m);
else material=m;
};

TMAT3D*GetMaterial(char*s){
 TMAT3D* m=0;
 if (material)m=material->GetMat(s);
 if (!m)m=defmat;
 return m;
};


WORD   ReadWord(){WORD w; _lread(hfile,&w,2);return w;};
char   ReadChar(){ char c; _lread(hfile,&c,1); return c;};
UINT32 ReadLong(){ UINT32 u; _lread(hfile,&u,4); return u;};
float  ReadFloat(){ float f; _lread(hfile,&f,4); return f;};
BOOL   ReadPOINT3D(POINT3D*p){
			p->x=ReadFloat();
			p->y=ReadFloat();
			p->z=ReadFloat();
   };
typedef struct tagCHUNK{
 UINT16 ID;    // Chunk ID
 UINT32 next;  // Pointer to next;
}CHUNK;


typedef struct tagFACE3D{
 INT16 vertex1, vertex2, vertex3;
 INT16 flags;
}FACE3D;

BOOL Read3DSChunk(CHUNK&ch){
BOOL ret=(_lread(hfile,&ch,sizeof(CHUNK))==sizeof(CHUNK));

if ((ch.ID==0x4f10)||
	(ch.ID==0x4f00))
	{
	int i=0;i++;
   }
return ret;
}
void SkipToNextChunk (CHUNK &Chunk) {
	_llseek ( hfile, Chunk.next-6, FROM_CURRENT );
  }
long GetPos()
  {return _llseek(hfile,0,1);
  }

long GetSize(){
 long p=GetPos();
 long l=_llseek(hfile,0,2);
 _llseek(hfile,p,0);
 return l;
};

long ChkReadChunk (long pos,CHUNK &chunk) {
	 long _pos=GetPos();
	 long delta=(pos+(chunk.next))-_pos;
	 if (delta){
	  _llseek ( hfile,delta, FROM_CURRENT );
	  return GetPos();
	}
	return _pos;
  }
BOOL ReadName(char*temp_name){
 int count=0;
 while ((temp_name[count]=ReadChar()) && (count<100))count++;
 if (!count)lstrcpy(temp_name,"Default Name");
 return count<100;
};

class HIERARCHY3D{
public:
 char name[128];
 int  id;
 HOBJ2D object;
 HIERARCHY3D * next;
 HIERARCHY3D(){
  ReadName(name) ;
  ReadWord();
  ReadWord();
  id=ReadWord();
  object=GetObjectByName2d((HSP2D)_hsp3d,0,name);
  next=NULL;
 };
};

COLOR3D ReadRGBColor(){
 COLOR3D c;
 c.c.r=ReadFloat()*255.0;
 c.c.g=ReadFloat()*255.0;
 c.c.b=ReadFloat()*255.0;
 c.c.a=0;
 return c;
};
COLOR3D ReadTrueColor(){
 COLOR3D c;
 c.c.r=ReadChar();
 c.c.g=ReadChar();
 c.c.b=ReadChar();
 c.c.a=0;
 return c;
};

BOOL ReadFacesChunk(long size,INT16&facecount,PRIMITIVE3D**_prim,INT16**_nums){
 CHUNK chunk;
   PRIMITIVE3D*prim;
   INT16* nums;
		   facecount=ReadWord();// point count;
		   if (facecount){
			FACE3D*faces3d=new FACE3D[facecount];
			_lread(hfile,faces3d,facecount*sizeof(FACE3D));
			nums=new INT16[facecount*3];
			prim=new PRIMITIVE3D[facecount];

             *_prim=prim;
             *_nums=nums;

			memset(prim,0,sizeof(PRIMITIVE3D)*facecount);
            char name[256];
			for(int i=0;i<facecount;i++){
			  nums[(i*3)]   = faces3d[i].vertex1;
			  nums[(i*3)+1] = faces3d[i].vertex2;
			  nums[(i*3)+2] = faces3d[i].vertex3;
			  prim[i].count=3;
			  prim[i].points=nums +(i*3);
			  prim[i].flags=PRIMITIVE_POLYGON;//|PRIMITIVE_ALWAYS;
			  prim[i].color.rgb=RGB(255,255,255);
			};

			delete faces3d;
			}


 long  CurP=GetPos();
  while (CurP<size)
   {
	 if (!Read3DSChunk(chunk))return FALSE;

		switch (chunk.ID)
		  {
        case TRI_SMOOTH:{
         int n=(chunk.next-GetPos());
         // дальше идет массив из long[n].
         // каждый указывает на номер примитива в группе.
        }break;
        case TRI_MATERIAL:{
                 char name[256];
                 ReadName(name);
                 TMAT3D *m=GetMaterial(name);
                 INT16 c=ReadWord();
                 if (c){
                  INT16 * items=new INT16[c];
                  _lread(hfile,items,c*2);
                 if(m){
                   for(int i=0;i<c;i++){
                       prim[items[i]].color=m->diffuse;
                       if(m->twosided)prim[items[i]].flags|=PRIMITIVE_ALWAYS;
                      }
                    }
                  delete items;
                 }
                } break;
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
  return TRUE;
};

HOBJ2D ReadPrimary3ds(long size){
 CHUNK chunk;
 HOBJ2D object3d=CreateObject3d(_hsp3d);
 if (_hobj3d){
  if(!_hgroup){_hgroup=CreateGroup2d((HSP2D)_hsp3d,0,0);
	AddGroupItem2d((HSP2D)_hsp3d,_hgroup,_hobj3d);
   }
   AddGroupItem2d((HSP2D)_hsp3d,_hgroup,object3d);
  }else{
	_hobj3d=object3d;
  }
 long  CurP=GetPos();
 INT16 pointcount=0;
 INT16 *nums=NULL;
 INT16 facecount=0;
 INT16 visible=TRUE;
 POINT3D*points=NULL;
 PRIMITIVE3D *prim=NULL;

 float matrix[4][3];
 BOOL  matrixloaded=0;

 BOOL  c4170=FALSE;
 float x_tiling, y_tiling;
 float icon_x, icon_y, icon_z;
 float _matrix[4][3];
 float scaling, plan_icon_w, plan_icon_h, cyl_icon_h;
 INT16 maptype=0;


 int color=0;
 BOOL ok=TRUE;
 while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)){
        ok=FALSE;
        break;
     }
		switch (chunk.ID)
		  {
		   case TRI_FACEL1:
             if(!ReadFacesChunk(CurP+chunk.next,facecount,&prim,&nums)){
             ok=FALSE;
             break;
             }
		   break;
		   case TRI_VERTEXL:{
		   pointcount=ReadWord();// point count;
         if(pointcount<32001){
			if (pointcount){
			 points=new POINT3D[pointcount];
			 for(INT16 i=0;i<pointcount;i++)ReadPOINT3D(points+i);
			}}else{
         	MessageBox(0,"Scene to complex!\n To many vertexes in ONE object","Import 3DS file",MB_TASKMODAL|MB_OK|MB_ICONSTOP);
            return 0;
         }
		   }break;
//		   case TRI_VERTEXOPTIONS:
//		   case TRI_MAPPINGCOORS:
//		   case TRI_MAPPINGSTANDARD:
		   case TRI_LOCAL:{
			 _lread(hfile,matrix,sizeof(matrix));
			 matrixloaded=1;
		   }break;
		   case TRI_MESHCOLOR:{
			color=ReadChar();
		   }break;
         case 0x4170:{
         maptype=ReadWord();
         c4170=TRUE;
         x_tiling=ReadFloat();
         y_tiling=ReadFloat();
         icon_x=ReadFloat();icon_y=ReadFloat(); icon_z=ReadFloat();
         _lread(hfile,_matrix,sizeof(_matrix));

         scaling=ReadFloat();
         plan_icon_w=ReadFloat();
         plan_icon_h=ReadFloat();
         cyl_icon_h=ReadFloat();
         }break;
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
	   if (ok)CurP=ChkReadChunk (CurP,chunk);else break;
	} // End switch

  BOOL rez;
  if (ok && points){
   if(matrixloaded ){

   double m[4][4];
   double a[4][4];

   double _e[4][4];


   memset(m,0,sizeof(m));

   {
   for(int i=0;i<4;i++)
    for(int j=0;j<3;j++) m[i][j]=matrix[i][j];
    m[3][3]=1.0;
    __INV_MAT4(&m,&a);
    __MUL_MAT4(&m,&a,&_e);

   }

   for(INT16 i=0;i<pointcount;i++){
	 POINT3D *p=points+i;

	 double x,y,z;

	 x=p->x*a[0][0]+p->y*a[1][0]+p->z*a[2][0]+a[3][0];
	 y=p->x*a[0][1]+p->y*a[1][1]+p->z*a[2][1]+a[3][1];
	 z=p->x*a[0][2]+p->y*a[1][2]+p->z*a[2][2]+a[3][2];

	 p->x=x;
	 p->y=y;
	 p->z=z;

	}
    SetMatrix3d(_hsp3d,object3d,m);
   }

   rez=AddPoints3d(_hsp3d,object3d,points,pointcount);
   rez=CreatePrimitives3d(_hsp3d,object3d,prim,facecount);
  }

  if (points)delete points;
  if (prim)delete prim;
 if (ok) return object3d;
 DeleteObject2d((HSP2D)_hsp3d,object3d);
 return 0;
}
HOBJ2D ReadCamera3ds(long size){
 CHUNK chunk;

	CAMERA3D camera;
    MakeDefaultCamera(&camera,0);
    camera.poSize=sizeof(camera);

    camera.poFlags=CAMERA_TARGET;

    camera.poOrg.x=ReadFloat();
    camera.poOrg.y=ReadFloat();
    camera.poOrg.z=ReadFloat();

    POINT3D target;
    target.x=ReadFloat();
    target.y=ReadFloat();
    target.z=ReadFloat();

    camera.poDirection.x=target.x;
    camera.poDirection.y=target.y;
    camera.poDirection.z=target.z;

     double angle=ReadFloat();
     double lens=ReadFloat();
    camera.poFocus=-1;
    camera.poBackground=bkcolor.rgb;
    camera.poOffset.x=0.5;
    camera.poOffset.y=0.5;
    camera.poExtent.x=camera.poExtent.y=100.0;
    camera.poExtent.z=1.0;

 long  CurP=GetPos();

 while (CurP<size)
{
	 if (!Read3DSChunk(chunk)) return FALSE;
		switch (chunk.ID)
		  {
		   case CAM_UNKNWN02:{
			camera.poFar_Clip=ReadFloat();
			camera.poNear_Clip=ReadFloat();
		   }break;
			default :
			 SkipToNextChunk (chunk );
			 break;  
		  } // End switch		  
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch

  HOBJ2D object3d=CreateCamera3d(_hsp3d,&camera);

  return object3d;
};

HOBJ2D ReadLight3ds(long size){
 CHUNK chunk;

	LIGHT3D light;
	  memset(&light,0,sizeof(light));
	  light.lSize=sizeof(LIGHT3D);
	  light.lOrg.x=ReadFloat();
	  light.lOrg.y=ReadFloat();
	  light.lOrg.z=ReadFloat();
	BOOL off=FALSE;
 long  CurP=GetPos();

 while (CurP<size)
{
	 if (!Read3DSChunk(chunk)) return FALSE;
		switch (chunk.ID)
		  {
		   case COL_TRU:{
			light.lColor=ReadTrueColor();
		   }break;
		   case COL_RGB:{
			light.lColor=ReadRGBColor();
		   }break;
		   case LIT_OFF:{
			off=FALSE;
		   }break;
		   case LIT_SPOT:{
			light.lType=LIGHT_SPOT;
			light.lDir.x=ReadFloat();
			light.lDir.y=ReadFloat();
			light.lDir.z=ReadFloat();
			float HotSpot=ReadFloat();
			float FallOff=ReadFloat();
		   }break;
		   case LIT_UNKNWN01:{

		   }
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch

  HOBJ2D object3d=CreateLight3d(_hsp3d,&light);
  if (off)HideObject3d(_hsp3d,object3d);

  return object3d;
};

COLOR3D ReadColorChunk(long size){
 CHUNK chunk;
 long  CurP=GetPos();
 COLOR3D color;
 color.rgb=0;
  while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)) return color;

		switch (chunk.ID)
		  {
        case COL_RGB :color=ReadRGBColor();

                      break;
        case COL_TRU :color=ReadTrueColor();

                      break;
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
  return color;
};


BOOL ReadBackGround(long size){
 CHUNK chunk;
 long  CurP=GetPos();
  while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)) return FALSE;

		switch (chunk.ID)
		  {
        case COL_RGB :bkcolor=ReadRGBColor();
                      setbk=TRUE;
                      break;
        case COL_TRU :bkcolor=ReadTrueColor();
                      setbk=TRUE;
                      break;
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
  return TRUE;
};

BOOL ReadMaterial(long size){
 CHUNK chunk;
 char name[256]="";
 COLOR3D ambient,diffuse,specular;
 BOOL two_sided=FALSE;

 long  CurP=GetPos();
  while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)) return FALSE;

		switch (chunk.ID)
		  { case MAT_NAME01: ReadName(name);break;
            case MAT_AMBIENT:  ambient  = ReadColorChunk(CurP+chunk.next);break;
            case MAT_DIFFUSE:  diffuse  = ReadColorChunk(CurP+chunk.next);break;
            case MAT_SPECULAR: specular = ReadColorChunk(CurP+chunk.next);break;
            case MAT_TWO_SIDE: two_sided=TRUE;break;
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
   if (lstrlen(name)){
    TMAT3D *m=new TMAT3D(name);
     m->ambient=ambient;
     m->diffuse=diffuse;
     m->specular=specular;
     m->twosided=two_sided;
     Insertmaterial(m);

   }
  return TRUE;
};



BOOL ReadObjects3ds(long size){
 CHUNK chunk;
 HOBJ2D object3d=0;
 BOOL hidden=0;
 char temp_name [100];
 ReadName(temp_name);

 long  CurP=GetPos();

 while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)) return FALSE;

		switch (chunk.ID)
		  {
		   case OBJ_TRIMESH:{
			   if (!(object3d=ReadPrimary3ds(CurP+chunk.next)))return FALSE;
			   }
				break;
		   case OBJ_CAMERA:{
			   if (!(object3d=ReadCamera3ds(CurP+chunk.next)))return FALSE;
			}break;
		   case OBJ_LIGHT:{
			   if (!(object3d=ReadLight3ds(CurP+chunk.next)))return FALSE;
			}break;
		   case OBJ_HIDDEN:hidden=TRUE; break;
		   case OBJ_UNKNWN02:
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
   if(object3d){
   SetObjectName3d(_hsp3d,object3d,temp_name);
   if(hidden)HideObject3d(_hsp3d,object3d);
   }
  return TRUE;
};

BOOL ReadEdit3ds(long size){
 CHUNK chunk;
 long  CurP=GetPos();
 while (CurP<size)
   {
	 Read3DSChunk(chunk);

		switch (chunk.ID)
		  {
           case EDIT_MATERIAL:
   			   ReadMaterial(CurP+chunk.next);
               break;
           case EDIT_BACKGR:
   			   ReadBackGround(CurP+chunk.next);
               break;
		   case EDIT_OBJECT:
			   ReadObjects3ds(CurP+chunk.next);
			   break;
			default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
  return TRUE;
}
BOOL ReadHierarchy(long size){
 long  CurP=GetPos();
 HIERARCHY3D *root=NULL;
 HIERARCHY3D *_curp=NULL;
  while (CurP<size){
   HIERARCHY3D *item=new HIERARCHY3D();
   if (!root){
	root=item;
   }else _curp->next=item;
	_curp=item;
   CurP=GetPos();
  }

  while (root){
	_curp=root;
	root=root->next;
	delete _curp;
  }
 return FALSE;
};


BOOL ReadKeyfObj3ds(long size){
 CHUNK chunk;
 long  CurP=GetPos();
 while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)) return FALSE;
		switch (chunk.ID)
		  {
		  case EDIT_OBJECT:
			   ReadObjects3ds(CurP+chunk.next);       //???
			   break;
		   default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
  return TRUE;
}





BOOL ReadKeyf3ds(long size){
 CHUNK chunk;
 long  CurP=GetPos();
 while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)) return FALSE;

		switch (chunk.ID)
		  {
		   case 0xB010U:{
			ReadHierarchy(CurP+chunk.next);
		   };break;
/*
		   case KEYF_OBJDES:{
			ReadKeyfObj3ds(CurP+chunk.next);
		   }break;
*/
		   // 1 сюда
		   default :
			 SkipToNextChunk (chunk );
			 break;
		  } // End switch
		 CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
  return TRUE;
}
BOOL ReadMainChunk(long size){
 CHUNK chunk;
 long  CurP=GetPos();
 while (CurP<size)
   {
	 if (!Read3DSChunk(chunk)) return FALSE;

		switch (chunk.ID)
		  {
		// case FILE_VERSION:{  version=ReadWord(); }break;
		 case EDIT3DS:if (!ReadEdit3ds(CurP+chunk.next))return FALSE;
					 break;
		 case KEYF3DS:if (!ReadKeyf3ds(CurP+chunk.next))return FALSE;
					 break;
			default :
			  SkipToNextChunk (chunk );
			  break;
		  } // End switch
		CurP=ChkReadChunk (CurP,chunk);
	  } // End switch
  return TRUE;
};

BOOL AdjustObject(HSP3D hsp3d,HOBJ2D obj){
 POINT3D org,size;
 if(GetObjectOrgSize3d(hsp3d,obj,&org,&size)){
 org.x=-size.x/2.0;
 org.y=-size.y/2.0;
 org.z=-size.z/2.0;
 return SetObjectOrg3d(hsp3d,obj,&org);}
 return 0;
}

HOBJ2D _EXPORT Import3DS(HSP3D hsp3d,char*filename){

hfile=_lopen(filename,TO_OPENREAD);
 if (hfile>0){
  bkcolor.rgb=0;
  setbk=0;

  _hsp3d=0;
  _hgroup=_hobj3d=0;
  CHUNK ch;
  if (Read3DSChunk(ch)){
   if (ch.ID==MAIN3DS){

	{
	  _hsp3d=hsp3d;
	  version=0;
      defmat=new TMAT3D();
	  ReadMainChunk(GetSize());
      delete defmat;
      defmat=0;
	}
    }}
   _lclose(hfile);
   _hobj3d=_hgroup?_hgroup:_hobj3d;
//	AdjustObject(hsp3d,_hobj3d);
    if(material)delete material;
    material=NULL;
	return _hobj3d;
   }
   else{
	MessageBox(0,"aa","aaa",MB_OK|MB_ICONSTOP);
   }
return 0;

};

HINSTANCE hInstance=0;

#if defined(WIN32)
//#pragma argsused
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,
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
	return TRUE;// Indicate that the DLL was initialized successfully.
 }
 case DLL_PROCESS_DETACH:{
   return TRUE;
 }
}
 return FALSE;
#else
 if ( wHeapSize != 0 ) UnlockData( 0 );
	 ::hInstance=hInstance;
#endif
}

#if !defined(WIN32)
int CALLBACK WEP(int nExitType){
//if (nExitType==WEP_FREE_DLL)
 return 1;
}
#endif



extern "C" HOBJ2D _EXPORT MakeGrid3d(HSP3D hsp3d,double sizex,int countx,double sizey,int county,COLORREF color){
 if(countx<1 || county<1)return 0;
 int ps=(countx+1)*(county+1);
 int pc=(countx+1)+(county+1);
 POINT3D* p=new POINT3D[ps];
 memset(p,0,sizeof(POINT3D)*ps);
 PRIMITIVE3D* prim= new PRIMITIVE3D[pc];
 memset(prim,0,sizeof(PRIMITIVE3D)*pc);
 INT16 *nums=new INT16[2*ps];
 for(INT16 j=0;j<county+1;j++)
  for(INT16 i=0;i<countx+1;i++)
   {
    int n=j*(countx+1)+i;
    p[n].y=(sizey)*j/county -sizey/2.0;
    p[n].x=(sizex)*i/countx -sizex/2.0;
   }

INT16 *_n=nums;
for(j=0;j<county+1;j++){
 prim[j].points=_n;
 prim[j].flags=PRIMITIVE_POLYLINE;
 prim[j].color.rgb=color;
 prim[j].count=countx+1;
 for(INT16 i=0;i<countx+1;i++){*_n=j*(countx+1)+i;_n++;}
}

for(j=0;j<countx+1;j++){
 INT16 n=j+county+1;
 prim[n].points=_n;
 prim[n].flags=PRIMITIVE_POLYLINE;
 prim[n].color.rgb=color;
 prim[n].count=county+1;

 for(INT16 i=0;i<county+1;i++){*_n=i*(countx+1)+j;_n++;}

}
HOBJ2D obj=CreateObject3d(hsp3d);
 AddPoints3d(hsp3d,obj,p,ps);
 CreatePrimitives3d(hsp3d,obj,prim,pc);
delete p;
delete nums;
delete prim;
return obj;
};

HOBJ2D _EXPORT MakeBar3d(HSP3D hsp3d,COLOR3D color,double sizex,double sizey,double sizez){
HOBJ2D obj=CreateObject3d(hsp3d);
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
for(int i=0;i<6;i++){
p[i].flags=PRIMITIVE_POLYGON|PRIMITIVE_NOSHADE;//|PRIMITIVE_ALWAYS;
p[i].color=color;
p[i].points=npoints+i*4;
p[i].count=4;
}
 CreatePrimitives3d(hsp3d,obj,p,6);
 return obj;
};

HOBJ2D _EXPORT MakeToreEx3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double r1,double r2,double sp,INT16 pcount,INT16 c2){
 POINT3D* pp=new POINT3D[pcount];

	for(INT16 i=0;i<pcount;i++){
	   double a=((double)i/(pcount-1))*M_PI*2.0+sp;
	   pp[i].x=r2+r1*cos(a);
	   pp[i].z=r1*sin(a);
	   pp[i].y=0;
	}

 pp[pcount-1].z=pp[0].z;
 ROTATOR3D r;
 memset(&r,0,sizeof(r));
 r.VctSize.z=r2;
 r.Angle=M_PI*2.0;
 HOBJ2D obj3d=MakeFace3d(hsp3d,old,pp,pcount,&r,
	NULL,c2,color,NULL,
	NULL,0);

 delete pp;
 return obj3d;

};

HOBJ2D _EXPORT MakeTore3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double r1,double r2,INT16 pcount,INT16 c2){
  return MakeToreEx3d(hsp3d,old,color,r1,r2,0,pcount,c2);
};

HOBJ2D _EXPORT MakeSphere3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R,INT16 c1,INT16 c2){
 if(c1<3 || c2<3)return 0;
 POINT3D* pp=new POINT3D[c1];
	for(INT16 i=0;i<c1;i++){
	   double a=((double)i/(c1-1))*M_PI;
	   pp[i].y=0;
	   pp[i].x=R*sin(a);
	   pp[i].z=R*cos(a);
	}

 pp[c1-1].x=pp[0].x;

 ROTATOR3D r;
 memset(&r,0,sizeof(r));
 r.VctSize.z=R;
 r.Angle=-M_PI*2.0;

 HOBJ2D obj3d=MakeFace3d(hsp3d,old,pp,c1,&r,
	NULL,c2,color,NULL,
	NULL,0);

 delete pp;
 return obj3d;
}

extern "C" HOBJ2D _EXPORT CreateCamera3dEx(HSP3D hsp3d,char*driver,
   long type,long poFlags,COLORREF poBackground,double poExtent,double perspective,
   double _ux,double _uy,double _uz,
   double _x,double _y,double _z
   ){
 CAMERA3D c;
 memset(&c,0,sizeof(c));
 MakeDefaultCamera(&c,0);
 lstrcpyn(c.poRenderName,driver,sizeof(c.poRenderName));
 c.poRenderType=type;
 c.poFlags=poFlags|CAMERA_TARGET;
 c.poBackground=poBackground;
 c.poExtent.x=c.poExtent.y=poExtent;
  if(perspective!=0.0){
  c.poPerspective=PERSPECTIVE_YES;
  c.poFocus=perspective;
 }else{
   c.poPerspective=PERSPECTIVE_NO;
 }
 c.poUp.y=_ux;c.poUp.x=_uy;c.poUp.z=_uz;
 c.poOrg.y=_y;c.poOrg.x=_x;c.poOrg.z=_z;
 return CreateCamera3d(hsp3d,&c);
};

extern "C" HOBJ2D _EXPORT MakeTube3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R1,double R2,double L,INT16 c1,INT16 c2){
 if(R1<0 || R2<0)return 0;
 if(c2<1)c2=1;
 if(R2<R1){double r=R1;R1=R2;R2=r;}

 INT16 pcount  = (c2+1)*2+1;
 POINT3D* pp   =new POINT3D[pcount];
 memset(pp,0,sizeof(POINT3D)*(pcount));
 int _cc=c2+1;

 for(int i=0;i<_cc;i++){
  pp[i].x=R2;
  pp[i+_cc].x=R1;
  pp[i+_cc].z = -(pp[i].z=(double)(L*i)/((double)c2)-L/2);
 };

 pp[pcount-1]=pp[0];

 ROTATOR3D r;
 memset(&r,0,sizeof(r));
 r.VctSize.z=10;
 r.Angle=M_PI*2.0;

 HOBJ2D obj3d=MakeFace3d(hsp3d,old,pp,pcount,&r,
	NULL,c1,color,NULL,
	NULL,0);

 delete pp;
 return obj3d;
}

extern "C" HOBJ2D _EXPORT MakeCylinder3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R1,double R2,double L,INT16 c1,INT16 c2){
 if(!(R1>0 || R2>0))return 0;
 if(c2<0)c2=0;
 INT16 pcount = c2+4;
 INT16 start=1,_c2=c2;
 if (R1==0 || R2==0)pcount--;

 POINT3D* pp=new POINT3D[pcount];
 memset(pp,0,sizeof(POINT3D)*(pcount));

 pp[0].z=-L/2;
 pp[pcount-1].z=L/2;

 if(R1>0){pp[1].z=pp[0].z;pp[1].x=R1;_c2++;}else start--;
 if(R2>0){pp[pcount-2].z=pp[pcount-1].z;pp[pcount-2].x=R2;_c2++;}

 int _cc=c2+1.0;
 for(int i=0;i<_cc;i++){
  pp[i+start].x= R1 + (R2-R1)*i/(_cc);
  pp[i+start].z=L*i/(_cc)-L/2;
 };


 ROTATOR3D r;
 memset(&r,0,sizeof(r));
 r.VctSize.z=10;
 r.Angle=M_PI*2.0;

 HOBJ2D obj3d=MakeFace3d(hsp3d,old,pp,pcount,&r,
	NULL,c1,color,NULL,
	NULL,0);

 delete pp;
 return obj3d;
}
