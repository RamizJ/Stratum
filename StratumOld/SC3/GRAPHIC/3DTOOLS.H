#ifdef _3D_TOOLS_STATIC_
extern "C"{
HOBJ2D WINAPI Import3DS(HSP3D,char*filename);
/*
  Импортируются объекты из 3ds файла  filename
  Все первичные объединяются в группу 'Main_Group'
*/
HOBJ2D WINAPI Import3DS_2(HSP2D,char*filename);


HOBJ2D WINAPI MakeFace3d(HSP3D hsp3d,HOBJ2D ,POINT3D * points,INT16 count,ROTATOR3D * r,
	POINT3D * delta,INT16 asteps,COLOR3D color,MATERIAL3D *m,
	POINT3D *org,UINT32 flags);

#define MF3D_MOVE_CENTER   256

HOBJ2D WINAPI MakeCylinder3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R1,double R2,double L,INT16 c1,INT16 c2);
HOBJ2D WINAPI MakeSphere3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R,INT16 c1,INT16 c2);
HOBJ2D WINAPI MakeTore3d(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double r1,double r2,INT16 pcount,INT16 c2);
HOBJ2D WINAPI MakeBar3d(HSP3D hsp3d,COLOR3D color,double sizex,double sizey,double sizez);
HOBJ2D WINAPI MakeGrid3d(HSP3D hsp3d,double size,int count,COLORREF color);

};
#else

extern HOBJ2D  (WINAPI * Import3DS)(HSP3D,char*filename);
extern BOOL (WINAPI * MakeFace3d)(HSP3D hsp3d,HOBJ2D ,POINT3D * points,INT16 count,ROTATOR3D * r,
	POINT3D * delta,INT16 asteps,COLOR3D color,MATERIAL3D *m,
	POINT3D *org,UINT32 flags);

extern HOBJ2D (WINAPI * MakeCylinder3d)(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R1,double R2,double L,INT16 c1,INT16 c2);
extern HOBJ2D (WINAPI * MakeSphere3d)(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R,INT16 c1,INT16 c2);
extern HOBJ2D (WINAPI * MakeTore3d)(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double r1,double r2,INT16 pcount,INT16 c2);
extern HOBJ2D (WINAPI * MakeBar3d)(HSP3D hsp3d,COLOR3D color,double sizex,double sizey,double sizez);
extern HOBJ2D (WINAPI * MakeGrid3d)(HSP3D hsp3d,double size,int count,COLORREF color);



#endif

extern HINSTANCE  hinst3d;
BOOL InitTools3d();
BOOL Done3dTools();
