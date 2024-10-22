/*
Copyright (c) 1996 TST
graphic library for windows support file

Module Name
	 facemake.h
*/
#ifndef  __FACEMAKE_H_
#include <gwin2d3d\tpoint3d.h>
#include <gwin2d3d\gwinfunc.h>

#define MF_ALLVISIBLE 1
#define MF_NOSHADEDE  2

class TFaceMaker{
  BOOL close; // figure closed
int pin1,     //  point count in first line / ����� ����� � ������ ������
	pin2,     // point count in next line / � ����������� �������
	steps,
	pall,     // num points / ����� ����� � ����������� �����������
	primall,   //num triangles / ����� �������������
	fullrect, // ����� ������������� ��������� � �����
	fulltri,  // ����� ����������� ��������� � �����
	tflag;    //flag � ������������
int * pf;     // ����� ��� ����� (<0 - ����� ������ � ������ ������,
				  //-32767
				  // n >0 ������ �� ����� (n-1) � points)
	int rez;   // result / ��������� ���������� ��������
	HOBJ2D object;
public:
	TPoint3d *_points; //creating points / ����������� �����
	PRIMITIVE3D * prim;//creating triangles / ����������� ������������
	INT16       * pointnums;
	int         numnum;
	TPoint3d org,size;

 TFaceMaker(TPoint3d * points,int count,ROTATOR3D * r,
						 TPoint3d * delta,int asteps,COLOR3D color,int flags
						 );
 TFaceMaker(double height,double radius,COLORREF c);//
~TFaceMaker();
 HOBJ2D    CreateObject(HSP3D h3);
 void   CalcSize();
 void   Move(TPoint3d);
 void  _Move(TPoint3d,int);
 int    Result(){return rez;}
 int    GetObject(){return object;};
 void   SetObject(HOBJ2D ho){object=ho;};
private:
 int    GetPoint(int x,int y);
};
#define __FACEMAKE_H_
#endif