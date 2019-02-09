/*
Copyright (c) 1996 TST
Example of graphic library for windows

Module Name
	 tpoint3d.cpp
*/

#include <gwin2d3d\gwintype.h>
#include <gwin2d3d\tpoint3d.h>
#define _THIS ((POINT3D*)(&x))
#define _this ((TPoint3d*)(&x))
        TPoint3d::TPoint3d( const TPoint3d& p ) {
                (*(tagPOINT3D *)&x) =*(const tagPOINT3D *)&p;
        }
        TPoint3d::TPoint3d( const POINT3D& p ) {
                (*(tagPOINT3D *)&x) =*(const tagPOINT3D *)&p;
        }
        TPoint3d::TPoint3d( double _x , double _y, double _z) {
                x =_x; y =_y; z =_z;
        }
 //       TPoint3d::operator POINT3D &() const { return *_THIS; }
        BOOL TPoint3d::operator == ( const TPoint3d& p )  const{
                return ((x == p.x) && (y == p.y) && (z == p.z));
        }
        BOOL TPoint3d::operator != ( const TPoint3d& p )  const{
                return ((x != p.x) || (y != p.y) || (z != p.z));
        }
        TPoint3d& TPoint3d::operator += ( const TPoint3d& s ){
                return x += s.x, y += s.y, z += s.z, *_this;
        }
        TPoint3d& TPoint3d::operator -= ( const TPoint3d& s ){
                return x -= s.x, y -= s.y, z -= s.z, *_this;
        }
        TPoint3d& TPoint3d::operator /= (const double i){
                return x /=i, y /=i, z /=i, *_this;
        }
        TPoint3d& TPoint3d::operator *= (const double i){
                return x *=i, y *=i, z *=i, *_this;
        }
        TPoint3d& TPoint3d::operator = ( const TPoint3d& sz )  const{
                return *(tagPOINT3D *)_THIS = *(const tagPOINT3D*)&sz,*_this;
        }

