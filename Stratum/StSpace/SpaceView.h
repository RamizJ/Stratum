#ifndef SPACE2DVIEW_H
#define SPACE2DVIEW_H

#include "stspace_global.h"
#include <QGraphicsView>

namespace StSpace {

class STSPACESHARED_EXPORT SpaceView : public QGraphicsView
{
public:
    SpaceView(QWidget* parent = 0);
    ~SpaceView();

    bool scrollingEnabled() const { return m_isScrollingEnabled; }
    void setScrollingEnabled(bool isScrollingEnabled) { m_isScrollingEnabled = isScrollingEnabled; }

    bool setHorizontalScrollRange(double min, double max);
    bool setVertivalScrollRange(double min, double max);


protected:
    void scrollContentsBy(int dx, int dy);
//    void resizeEvent(QResizeEvent* event);

private:
    bool m_isScrollingEnabled;
    double m_horizontalScrollRangeMin, m_horizontalScrollRangeMax;
    double m_verticalScrollRangeMin, m_verticalScrollRangeMax;
};

}

#endif // SPACE2DVIEW_H
