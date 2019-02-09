class  TPoint2d : public tagPOINT2D {
  public:
    // Constructors
    TPoint2d() {}
    TPoint2d(gr_float _x,gr_float _y) {x = _x; y = _y;}
    TPoint2d(const POINT far& point) {x = point.x; y = point.y;}
    TPoint2d(const TPoint2d far& point) {x = point.x; y = point.y;}
    TPoint2d(const SIZE far& size) {x = size.cx; y = size.cy;}
    TPoint2d(UINT32 dw) {x = INT16(LOWORD(dw)); y = INT16(HIWORD(dw));}
    // Information functions/operators
    bool       operator ==(const TPoint2d& other) const;
    bool       operator !=(const TPoint2d& other) const;
};

