/*
Copyright (c) 1996 TST
graphic library for windows support file

Module Name
	 facemake.h
*/
#ifndef  __FACEMAKE_H_
#include <gwin2d3d\tpoint3d.h>
class TFaceMaker{
  BOOL close; // figure closed
int pin1,     //  point count in first line / ����� ����� � ������ ������
	 pin2,     // point count in next line / � ����������� �������
	 steps,
	 pall,     // num points / ����� ����� � ����������� �����������
	 triall,   //num triangles / ����� �������������
	 fullrect, // ����� ������ ��������� (�.�. 2 ������������)
	 tflag;    //flag � ������������
int * pf;     // ����� ��� ����� (<0 - ����� ������ � ������ ������,
				  //-32767
				  // n >0 ������ �� ����� (n-1) � points)
	int rez;   // result / ��������� ���������� ��������
	int object;
public:
	TPoint3d *_points; //creating points / ����������� �����
	TRIANGLE3D * _tri;//creating triangles / ����������� ������������
	TPoint3d org,size;

 TFaceMaker(TPoint3d * points,int count,ROTATOR3D * r,
						 TPoint3d * delta,int asteps,COLORREF color,int flags,
						 const HINSTANCE hinstance);
 TFaceMaker(double height,double radius,COLORREF c);//
~TFaceMaker();
 int    CreateObject(HSP3D h3);
 void   CalcSize();
 void   Move(TPoint3d);
 void  _Move(TPoint3d,int);
 int    Result(){return rez;}
 int    GetObject(){return object;};
private:
 int    GetPoint(int x,int y);
};
#define __FACEMAKE_H_
#endif