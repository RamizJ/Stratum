/*
Copyright (c) 1995  Virtual World
Module Name:
	 3drender.h
Author:
	 Alexander Shelemekhov
*/
#ifndef _3D_GR_RENDER_
#define _3D_GR_RENDER_
typedef struct tag_TPOINT3D{
 gr_float x,y,z;    // - ���������� ����� �������� � �������������� ��������������
 INT32   _x,_y,_z;  // - ���� ��������, �������������
 union{
 COLOR3D color;     // ���� � ����� ���� �����
 INT32 c;           // ���������� � (�� 0 �� 65535)
 };
 INT16   count; 
}_TPOINT3D;

/*
 �������� ���������� �������������:
 _x = x*256.0
 _y = y*256.0
 _z = z*256.0
*/

typedef struct tag_TPRIMITIVE3D{
 COLOR3D  color;      // - ���� ���������
 INT32    a,b,c,d;    // - ������������ ���������
 long     maxz;       // - max �������� Z ���������� (*256)
 long 	  minz;		  // - min �������� Z ���������� (*256)

 struct{
  unsigned  visible:1;   //- �������� ����� �.�. �������� ������� ��������
  unsigned  wireframe:1; //- �������� �������� �������
 }flags;              // - ��������� ��������.
}_TPRIMITIVE3D;  //- �������������� ����������� ���������

// ! ���� �������� �� ������ �� ��� ��������� �������(����� ������������ ��������)
	 
typedef struct tagVTABLE3d{
#ifdef WIN32
UINT32 __c1;
#else
UINT16 __c1;
#endif
}dVTABLE3d; // dVTABLE3d - ��������� �� ������� ����������� ������� 



typedef struct tagTITEMS2d{
 dVTABLE3d Vtable;  //reserved
 INT16 count;   // - ���������� ��������� � ������
 INT16 delta,limit;
 void ** items; // - ������ �� ���������� �� ��������.
}TITEMS2d;  //-  ��� ���������


typedef struct tagTITEMSprim{
 dVTABLE3d Vtable;
 INT16 count;  
 INT16 delta,limit;
 PRIMITIVE3D ** items;
}TITEMSprim; 


typedef struct tagTITEMS_prim{
 dVTABLE3d Vtable;
 INT16 count;  
 INT16 delta,limit;
 _TPRIMITIVE3D ** items;
}TITEMS_prim; 

typedef struct tagTITEMSpoint{
 dVTABLE3d Vtable;
 INT16 count;  
 INT16 delta,limit;
 _TPOINT3D ** items;
}TITEMSpoint; 


typedef struct tagTIntObject3d
{
 dVTABLE3d    Vtable;  //reserved
 void* owner;          // - ��������� �� ������
 WORD options;         // - �����
 DWORD PTR;            // - ���������������� ������
 char * name;          // - ��� �������
 HOBJ2D _HANDLE;       // - ���������� �������
 TITEMS2d * dataitems; // - �������������� ������
 POINT3D   origin;     // - ���������� ����� �������� (������� ����������)
 POINT3D   size;       // - ������ �������            (������� ����������)
 INT16     pointcount; // - ���������� ������ � �������
 POINT3D * points;     // - ������ ������ �������
 TITEMSprim *primitives; // - ��������� ����������
 WORD   flag;          // - �����
 void * space3d;       // - ��������� �� ���������� ������������
}TIntObject3d;  //- ���������� ������

typedef struct tag_OBJECT3D{
 TIntObject3d*object;  // - ��������� �� "����������" ������
 POINT3Di org;         // - ����� �������� ������� (� ������� �����������)
 POINT3Di size;        // - ������� �������        (� ������� �����������)
 UINT16 objglags;      // - ����� ������ ��������� �������:
 UINT32   flags;       // - ����� �������:
 TITEMSpoint *points;     // - ��������� �� ������. ������ ������� ���� _TPOINT3D
 TITEMS_prim *primitives; // - ��������� �� ����������. ������ ������� ���� _TPRIMITIVE3D
}_OBJECT3D; //- ������ � ������� ������� ���������

// �����  ���  _OBJECT3D:

#define O3D_NOTINRECT      1 // - ������ �� �������� � ������������� ����������
#define O3D_FULLINRECT     2 // - ������ ��������� �������� � ������������� ����������
#define O3D_CALCALLFACES   4
#define O3D_ZFARCONFLICT  16 // - ������ ����������� � ������� ���������� ����������
#define O3D_ZNEARCONFLICT 32 // - ������ ����������� � ������� ���������� ����������
#define O3D_ZNOTINRECT    64 // - ������ ��������� �������� �� Z �������.

typedef struct tagPAINTSTRUCT3D{
 HDC       hcolorDc;      // - �������� ����������
 HDC       hmonoDc;       // - �������� ������������ ����������(�����).
 RECT      srcPaint;      // - ������������� ����������� � ����������
 RECT      srcPaint1;
 POINT2D   offset;        // - center offset
 CAMERA3D* camera;        // - ��������� �� ������
 _OBJECT3D** _objects;    // - ������ �� ���������� �� �������
 LIGHT3D**   lights;      // - ������ �� ���������� �� ��������� �����
 TIntObject3d**objects3d; // - ������ �� ���������� �� ���������� �������
 INT16     objcount;      // - ���������� ��������
 INT16     lightcount;    // - ���������� ���������� �����
 BYTE      zclipvalid;    // - ����� ����������� ���� �� Z �����������
}PAINTSTRUCT3D;   //��������� ���������� ������� ������� ���������



#define RM3D_PAINT         1
#define RM3D_GETCAPS       2
#define RM3D_GETDRAWPROC   3

#define RM3D_INITDRIVER    10
// ���������� ��� �������� ��������
#define RM3D_DONEDRIVER    11
// ���������� ��� �������� ��������
#define RM3D_GETDRIVERINFO 12
// ������ ��������� �� �����, ������ ��� ������
// ���� �� 0 �� ��������� ������ ��� �����

#define RM3D_GETVERSION   13
#define RM3D_DRIVERSETUP  14
// ������ �������� HWND
// ������ �������� HGLOBAL


#define caps3d_facecolor    1
#define caps3d_vertexcolor  2 // return 1 - calc color / bit 2 calc C 
#define caps3d_faceabcd     3
#define caps3d_vertexc      4
#define caps3d_version      5
#define caps3d_drawonDIB    6
#define caps3d_drawonDC     7
#define caps3d_minrect      8 // ���������� ���������� ����������� ������ ���������

#define caps3d_points       9
#define caps3d_faces        10
#define caps3d_bounds       11

#endif
