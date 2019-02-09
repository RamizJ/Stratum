#if !defined(_MATRIXES_CLASSES_)
#define _MATRIXES_CLASSES_

class TMatrix{
public:
  double a[4][4];

  TMatrix(){};
  TMatrix(const POINT3D&);

  void PreMul(const TMatrix & ); // ceбя * m результат в себя

  void Mul( const TMatrix &, const TMatrix & ); // результат в себя
  void Mul(POINT3D&dst, const POINT3D&src);     // результат в dst
  void _Mul(POINT3D&dst, const POINT3D&src);    // результат в dst
  void Invert(const TMatrix &);                 // результат в себя

  void Set();
  void Set(const TMatrix&);

  double inline Get(int i,int j){return a[i-1][j-1];};
  double inline Put(int i,int j,double d){return a[i-1][j-1]=d;};
};
class TRotator3d;

class TMatrix2{
public:
 TMatrix a,b;
 BOOL bmustcalc;
 TMatrix2();
 void ReSet();
 void Set(const TMatrix&);
 void Rotate(const TRotator3d&);
 void MoveRel(const POINT3D&);
 void MoveTo(const POINT3D&);
 void Size(const POINT3D&);

 void Mul(TMatrix&);


 void Store(TStream&);
 void Load(TStream&);
 void CalcObr();
 TMatrix*GetB();
};

#endif