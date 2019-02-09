#include "PolylineArrow.h"

#define IS_START_ARROW_FILL 2
#define IS_END_ARROW_FILL 1

namespace StData {

PolylineArrow::PolylineArrow() :
    m_startAngle(0.0),
    m_startDistance(0.0),
    m_startLength(0.0),
    m_endAngle(0.0),
    m_endDistance(0.0),
    m_endLength(0.0)
{}

double PolylineArrow::startLength() const
{
    return m_startLength;
}

void PolylineArrow::setStartLength(double startLength)
{
    m_startLength = startLength;
}

double PolylineArrow::startAngle() const
{
    return m_startAngle;
}

void PolylineArrow::setStartAngle(double startAngle)
{
    m_startAngle = startAngle;
}

double PolylineArrow::startDistance() const
{
    return m_startDistance;
}

void PolylineArrow::setStartDistance(double startDistance)
{
    m_startDistance = startDistance;
}

double PolylineArrow::endLength() const
{
    return m_endLength;
}

void PolylineArrow::setEndLength(double endLength)
{
    m_endLength = endLength;
}

double PolylineArrow::endAngle() const
{
    return m_endAngle;
}

void PolylineArrow::setEndAngle(double endAngle)
{
    m_endAngle = endAngle;
}

double PolylineArrow::endDistance() const
{
    return m_endDistance;
}

void PolylineArrow::setEndDistance(double endDistance)
{
    m_endDistance = endDistance;
}

short PolylineArrow::flags() const
{
    return m_flags;
}

void PolylineArrow::setFlags(short flags)
{
    m_flags = flags;
}

bool PolylineArrow::isStartFill() const
{
    return (m_flags & IS_START_ARROW_FILL) != 0;
}

bool PolylineArrow::isEndFill() const
{
    return (m_flags & IS_END_ARROW_FILL) != 0;
}

}
