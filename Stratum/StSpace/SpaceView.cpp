#include "SpaceView.h"

#include <QGLFormat>
#include <QOpenGLWidget>
#include <qevent.h>

namespace StSpace {

SpaceView::SpaceView(QWidget* parent) :
    QGraphicsView(parent),
    m_isScrollingEnabled(true)
{
//    QOpenGLWidget* glWidget = new QOpenGLWidget(this);
//    QSurfaceFormat format = glWidget->format();
//    format.setSamples(4);
//    glWidget->setFormat(format);
//    setViewport(glWidget);
}

SpaceView::~SpaceView()
{}

bool SpaceView::setHorizontalScrollRange(double min, double max)
{
    if(min < max && horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
    {
        m_horizontalScrollRangeMin = min;
        m_horizontalScrollRangeMax = max;

        QRectF rect = sceneRect();
        rect.setLeft(min);
        rect.setRight(max);
        setSceneRect(rect);
        return true;
    }
    return false;
}

bool SpaceView::setVertivalScrollRange(double min, double max)
{
    if(min < max && horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
    {
        m_verticalScrollRangeMin = min;
        m_verticalScrollRangeMax = max;

        QRectF rect = sceneRect();
        rect.setTop(min);
        rect.setBottom(max);
        setSceneRect(rect);
        return true;
    }
    return false;
}

void SpaceView::scrollContentsBy(int dx, int dy)
{
    if(m_isScrollingEnabled)
        QGraphicsView::scrollContentsBy(dx, dy);
}

//void SpaceView::resizeEvent(QResizeEvent* event)
//{
//}

}
