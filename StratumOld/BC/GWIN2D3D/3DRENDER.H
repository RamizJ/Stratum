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
 float x,y,z;       // - координаты после видового и перспективного преобразования
 float __x,__y;     // без перспективы, если она есть.
 INT32   _x,_y,_z;  // - теже значения, целочисленные
 union{
 COLOR3D color;     // Цвет в точке если нужен
 INT32 c;           // Коэффицент С (от 0 до 65535)
 };
 INT16   count; 
}_TPOINT3D;

/*
 Связанны следующими соотношениями:
 _x = x*256.0
 _y = y*256.0
 _z = z*256.0
*/

typedef struct tag_TPRIMITIVE3D{
 COLOR3D  color;      // - цвет плоскости
 INT32    a,b,c,d;    // - коэффициенты плоскости
 long     maxz;       // - max значение Z координаты (*256)
 long 	  minz;		  // - min значение Z координаты (*256)

 struct{
  unsigned  visible:1;   //- примитив видим т.е. повернут видимой стороной
  unsigned  wireframe:1; //- примитив является фреймом
 }flags;              // - различные атрибуты.
}_TPRIMITIVE3D;  //- характеристики трехмерного примитива

// ! Если примитив не вмидим то все аттрибуты неверны(имеют произвольное значение)
	 
typedef struct tagVTABLE3d{
#ifdef WIN32
UINT32 __c1;
#else
UINT16 __c1;
#endif
}dVTABLE3d; // dVTABLE3d - Указатель на таблицу виртуальных методов 



typedef struct tagTITEMS2d{
 dVTABLE3d Vtable;  //reserved
 INT16 count;   // - количество элементов в списке
 INT16 delta,limit;
 void ** items; // - массив из указателей на элементы.
}TITEMS2d;  //-  Это коллекция


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
 void* owner;          // - указатель на группу
 WORD options;         // - флаги
 DWORD PTR;            // - пользовательские данные
 char * name;          // - имя объекта
 HOBJ2D _HANDLE;       // - дескриптор объекта
 TITEMS2d * dataitems; // - дополнительные данные
 POINT3D   origin;     // - координата точки привязки (мировые координаты)
 POINT3D   size;       // - размер объекта            (мировые координаты)
 INT16     pointcount; // - количество вершин в объекте
 POINT3D * points;     // - массив вершин объекта
 TITEMSprim *primitives; // - коллекция примитивов
 WORD   flag;          // - флаги
 void * space3d;       // - указатель на трехмерное пространство
}TIntObject3d;  //- Внутренний объект

typedef struct tag_OBJECT3D{
 TIntObject3d*object;  // - указатель на "внутренний" объект
 POINT3Di org;         // - точка привязки объекта (в видовых координатах)
 POINT3Di size;        // - размеры объекта        (в видовых координатах)
 UINT16 objglags;      // - копия флагов реального объекта:
 UINT32   flags;       // - флаги объекта:
 TITEMSpoint *points;     // - коллекция из вершин. Каждый элемент типа _TPOINT3D
 TITEMS_prim *primitives; // - коллекция из примитивов. Каждый элемент типа _TPRIMITIVE3D
}_OBJECT3D; //- объект в видовой системе координат

// Флаги  для  _OBJECT3D:

#define O3D_NOTINRECT      1 // - объект не попадает в прямоугольник обновления
#define O3D_FULLINRECT     2 // - объект полностью попадает в прямоугольник обновления
#define O3D_CALCALLFACES   4
#define O3D_ZFARCONFLICT  16 // - объект конфликтует с дальней отсекающей плоскостью
#define O3D_ZNEARCONFLICT 32 // - объект конфликтует с ближней отсекающей плоскостью
#define O3D_ZNOTINRECT    64 // - объект полностью выпадает из Z области.

struct TMATERIAL3D{
 UINT32    flags;
union{
 COLOR3D   ambient;
 COLOR3D   color;
};
 COLOR3D   diffuse;
 COLOR3D   specular;
 COLOR3D   emittance;
 gr_float  shine;
 gr_float  transparency;
 char    * name ;
 LPBITMAPINFOHEADER bitmap;
};


typedef struct tagPAINTSTRUCT3D{
 HDC       hcolorDc;      // - контекст устройства
 HDC       hmonoDc;       // - контекст монохромного устройства(маска).
 RECT      srcPaint;      // - прямоугольник нуждающийся в обновлении
 RECT      srcPaint1;
 POINT2D   offset;        // - center offset
 CAMERA3D* camera;        // - Указатель на камеру
 _OBJECT3D** _objects;    // - Массив из указателей на объекты
 LIGHT3D**   lights;      // - Массив из указателей на источники света
 TIntObject3d**objects3d; // - Массив из указателей на внутренние объекты
 INT16     objcount;      // - количество объектов
 INT16     lightcount;    // - количество источников света
 BYTE      zclipvalid;    // - можно производить клип по Z координатам
}PAINTSTRUCT3D;   //структура передается внешней функции отрисовки



#define RM3D_PAINT         1
#define RM3D_GETCAPS       2
#define RM3D_GETDRAWPROC   3

#define RM3D_INITDRIVER    10
// Посылается при загрузке драйвера
#define RM3D_DONEDRIVER    11
// Посылается при выгрузке драйвера
#define RM3D_GETDRIVERINFO 12
// первый указатель на буфер, второй его размер
// Если по 0 то вертается размер под буфер

#define RM3D_GETVERSION   13
#define RM3D_DRIVERSETUP  14
// Первый параметр HWND
// Второй параметр HGLOBAL


#define caps3d_facecolor    1
#define caps3d_vertexcolor  2 // return 1 - calc color / bit 2 calc C 
#define caps3d_faceabcd     3
#define caps3d_vertexc      4
#define caps3d_version      5
#define caps3d_drawonDIB    6
#define caps3d_drawonDC     7
#define caps3d_minrect      8 // Желательно примменять минимизацию области рисования

#define caps3d_points       9
#define caps3d_faces        10
#define caps3d_bounds       11


#endif
