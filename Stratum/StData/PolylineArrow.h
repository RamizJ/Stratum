#ifndef POLYLINEARROW_H
#define POLYLINEARROW_H

#include <stdata_global.h>

namespace StData {

class STDATASHARED_EXPORT PolylineArrow
{
public:
    PolylineArrow();

    double startLength() const;
    void setStartLength(double startLength);

    double startAngle() const;
    void setStartAngle(double startAngle);

    double startDistance() const;
    void setStartDistance(double startDistance);

    double endLength() const;
    void setEndLength(double endLength);

    double endAngle() const;
    void setEndAngle(double endAngle);

    double endDistance() const;
    void setEndDistance(double endDistance);

    short flags() const;
    void setFlags(short flags);

    bool isStartFill() const;
    bool isEndFill() const;

private:
    double m_startLength;
    double m_startAngle;
    double m_startDistance;

    double m_endLength;
    double m_endAngle;
    double m_endDistance;
    short m_flags;
};

}

#endif // POLYLINEARROW_H
