#if !defined(TRANSFORMERS_3DH_)
#define TRANSFORMERS_3DH_

class TRotator3d
{
public:
	ROTATOR3D rotator;
	TMatrix r;
  TRotator3d(ROTATOR3D* aaa);
 	void rotate(POINT3D*,int count);
	//double r11,r12,r13,r21,r22,r23,r31,r32,r33,r41,r42,r43;
};

#endif