/*
Copyright (c) 1996 TST
graphic library for windows support file

Module Name
	 tpoint.h
*/

#ifndef __TPOINT3D__H_
#ifndef __GRAPHIC_DLL_2D_3D_
#include <gwin2d3d\graphic.h>
#endif
class TPoint3d : public tagPOINT3D {
public:
	TPoint3d( const TPoint3d& p );
	TPoint3d( const POINT3D& p );
	TPoint3d( double _x = 0 , double _y = 0, double _z = 0 );
	operator POINT3D &() const;
	BOOL operator == ( const TPoint3d& p )  const;
	BOOL operator != ( const TPoint3d& p )  const;
	TPoint3d& operator += ( const TPoint3d& s );
	TPoint3d& operator -= ( const TPoint3d& s );
	TPoint3d& operator /= (const double i);
	TPoint3d& operator *= (const double i);
	TPoint3d& operator = ( const TPoint3d& sz ) const;
	};
#define __TPOINT3D__H_
#endif
