#include <gwin2d3d\gwintype.h>
#include "_object.h"
#include "matrix.h"
#include "rotator3.h"
#include "mem.h"
#include "math.h"


TMatrix::TMatrix(const POINT3D&p){
memset(this,0,sizeof(TMatrix));
 a[0][0]=p.x;
 a[1][1]=p.y;
 a[2][2]=p.z;
 a[3][3]=1.0;
};

void TMatrix::Set(){
 memset(this,0,sizeof(TMatrix));
 a[0][0]=a[1][1]=a[2][2]=a[3][3]=1.0;
};

void TMatrix::Mul( const TMatrix &_a, const TMatrix & _b){
  for(int i=0; i<4; i++) {
	for(int j=0; j<4; j++) {
	    a[i][j] = (
             _a.a[i][0] * _b.a[0][j] +
		       _a.a[i][1] * _b.a[1][j] +
		       _a.a[i][2] * _b.a[2][j] +
		       _a.a[i][3] * _b.a[3][j]);
	} }
};

void TMatrix::Mul(POINT3D&dst, const POINT3D&src){
     dst.x=src.x*a[0][0]+src.y*a[1][0]+src.z*a[2][0]+a[3][0];
     dst.y=src.x*a[0][1]+src.y*a[1][1]+src.z*a[2][1]+a[3][1];
     dst.z=src.x*a[0][2]+src.y*a[1][2]+src.z*a[2][2]+a[3][2];
};

void TMatrix::_Mul(POINT3D&dst, const POINT3D&src){
     dst.x=src.x*a[0][0]+src.y*a[1][0]+src.z*a[2][0];
     dst.y=src.x*a[0][1]+src.y*a[1][1]+src.z*a[2][1];
     dst.z=src.x*a[0][2]+src.y*a[1][2]+src.z*a[2][2];
};

void TMatrix::PreMul(const TMatrix & m1){
 TMatrix tmp;
 tmp.Mul(*this,m1);
 Set(tmp);
};

void TMatrix::Set(const TMatrix &m){
     memcpy(a,m.a,sizeof(TMatrix));
};
void TMatrix::Invert(const TMatrix &m ){
    double pos, neg, t;
    Set();
    // Calculate the determinant of submatrix A and determine if the
    // the matrix is singular as limited by the double precision
    // floating-point data representation.

    pos = neg = 0.0;
    t =  m.a[0][0] * m.a[1][1] * m.a[2][2];
    if (t >= 0.0) pos += t; else neg += t;

    t =  m.a[1][0] * m.a[2][1] * m.a[0][2];
    if (t >= 0.0) pos += t; else neg += t;

    t =  m.a[2][0] * m.a[0][1] * m.a[1][2];
    if (t >= 0.0) pos += t; else neg += t;

    t = -m.a[2][0] * m.a[1][1] * m.a[0][2];
    if (t >= 0.0) pos += t; else neg += t;

    t = -m.a[1][0] * m.a[0][1] * m.a[2][2];
    if (t >= 0.0) pos += t; else neg += t;

    t = -m.a[0][0] * m.a[2][1] * m.a[1][2];
    if (t >= 0.0) pos += t; else neg += t;

    double det = pos + neg;

    // Is the submatrix A singular?
    if (fabs(det) < ((pos - neg) * 1.0e-12)) {
	Set();
	return;
    }

    // Calculate inverse(A) = adj(A) / det(A)

    double det_1 = 1.0 / det;
    a[0][0] = (  (m.a[1][1]*m.a[2][2] - m.a[2][1]*m.a[1][2] )*det_1);
    a[0][1] = (- (m.a[0][1]*m.a[2][2] - m.a[2][1]*m.a[0][2] )*det_1);
    a[0][2] = (  (m.a[0][1]*m.a[1][2] - m.a[1][1]*m.a[0][2] )*det_1);
    a[1][0] = (- (m.a[1][0]*m.a[2][2] - m.a[2][0]*m.a[1][2] )*det_1);
    a[1][1] = (  (m.a[0][0]*m.a[2][2] - m.a[2][0]*m.a[0][2] )*det_1);
    a[1][2] = (- (m.a[0][0]*m.a[1][2] - m.a[1][0]*m.a[0][2] )*det_1);
    a[2][0] = (  (m.a[1][0]*m.a[2][1] - m.a[2][0]*m.a[1][1] )*det_1);
    a[2][1] = (- (m.a[0][0]*m.a[2][1] - m.a[2][0]*m.a[0][1] )*det_1);
    a[2][2] = (  (m.a[0][0]*m.a[1][1] - m.a[1][0]*m.a[0][1] )*det_1);

    /* Calculate -C * inverse(A) */
    a[3][0] = - (m.a[3][0] * a[0][0] +
		 m.a[3][1] * a[1][0] +
		 m.a[3][2] * a[2][0] );

    a[3][1] = - (m.a[3][0] * a[0][1] +
		 m.a[3][1] * a[1][1] +
		 m.a[3][2] * a[2][1] );


    a[3][2] = - (m.a[3][0] * a[0][2] +
		 m.a[3][1] * a[1][2] +
		 m.a[3][2] * a[2][2] );



/*
   TMatrix tmp = source;
   Set();

   for (int i = 0; i < 4; i++) {
	int ind = i;
	double val = tmp.a[i][i];

	// Find largest value for this row.
	for (int j = i + 1; j < 4; j++) {
	    if (fabs(tmp.a[j][i]) > fabs(val)) {
		ind = j;
		val = tmp.a[j][i];
	    }
	}

	// A row of zeros -> the matrix has no inverse.
	if (val == 0.0F) {  Set();	 return;}

	// Move the largest value for the row to the diagonal axis, if
	// it isn't already there.

	if (ind != i) {
	    for (int row = 0; row < 4; row++) {
		double val2;

		val2 = a[i][row];
		a[i][row] = a[ind][row];
		a[ind][row] = val2;

		val2 = tmp.a[i][row];
		tmp.a[i][row] = tmp.a[ind][row];
		tmp.a[ind][row] = val2;
	    }
	}

	// Scale the row so that the entry on the diagonal axis becomes 1.


	double ival = 1.0/val;
	for ( j = 0 ; j < 4 ; j++) {
	    tmp.a[j][i] *= ival;
	    a[j][i] *= ival;
	}

	// Add multiples of the scaled row to elminate nonzero entries
	// in the current axis column.

	for ( j = 0 ; j < 4 ; j++) {
	    if (j == i)
		continue;
	    val = tmp.a[i][j];
	    for (int k = 0; k < 4; k++) {
		tmp.a[k][j] -= tmp.a[k][i] * val;
		a[k][j] -= a[k][i] * val;
	    }
	}
    }
*/
}
 // Matrix2

 TMatrix2::TMatrix2(){
   a.Set();
   bmustcalc=TRUE;
 };
 void TMatrix2::Set(const TMatrix&m){
   memcpy(&a,&m,sizeof(a));
   bmustcalc=TRUE;
 };

 void TMatrix2::Rotate(const TRotator3d&r){
  a.PreMul(r.r);
  bmustcalc=TRUE;
 };
 void TMatrix2::ReSet(){
  a.Set();
  bmustcalc=TRUE;
 };
 void TMatrix2::Mul(TMatrix&m){
  a.PreMul(m);
  bmustcalc=TRUE;
 };
 void TMatrix2::MoveRel(const POINT3D&p){
  a.a[3][0]+=p.x;
  a.a[3][1]+=p.y;
  a.a[3][2]+=p.x;
  bmustcalc=TRUE;
 };
 void TMatrix2::MoveTo(const POINT3D&p){
  a.a[3][0]=p.x;
  a.a[3][1]=p.y;
  a.a[3][2]=p.z;
  bmustcalc=TRUE;
 };

 void TMatrix2::Size(const POINT3D&s){
   TMatrix m(s);
   a.PreMul(m);
   bmustcalc=TRUE;
 };

 void TMatrix2::Load(TStream&st)
 {
  st.Read(&a,sizeof(a));
  bmustcalc=TRUE;
 };

 void TMatrix2::Store(TStream&st){
  st.Write(&a,sizeof(a));
 };
 void TMatrix2::CalcObr(){
  if(bmustcalc){
   b.Invert(a);
   bmustcalc=FALSE;
  }
 };
 TMatrix*TMatrix2::GetB(){
   CalcObr();
   return &b;
 };
