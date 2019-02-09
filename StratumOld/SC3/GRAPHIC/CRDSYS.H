
class TRansform{
 public:
 TMatrix2 matrix;

 void ResetCrd();
 BOOL SetCrd(CRDSYSTEM*);
 BOOL GetCrd(CRDSYSTEM*);
 BOOL PushCrd();
 BOOL PopCrd();

 LPtoWP(POINT3D*src,POINT3D*dst,int);
 LStoWS(POINT3D*src,POINT3D*dst,int);
 WLPtoLP(POINT3D*src,POINT3D*dst,int);
 WStoLS(POINT3D*src,POINT3D*dst,int);
};