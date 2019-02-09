typedef long int32;
class TPoint3d{
 public:
 double x,y,z;
};

class TTriangle3d{
 public:
 uint16 t1,t2,t3;
 BYTE flags;
 COLORREF color;
};
class TLine3d{
public:
 uint16 t1,t2
 BYTE flags;
 COLORREF color;
};

class _tpoint3d{
 int32 x,y,z;
 POINT GetPOINT();
};

class _triangle3d{
 COLORREF color;
};


class TObject3d{
 TPoint3d    *points;
 TRiangles3d *triangles;
 TLine3d     *lines;
};
