#ifndef GRAPHICSITEMTYPE_H
#define GRAPHICSITEMTYPE_H

#include <stspace_global.h>

enum class GraphicsItemType : int
{
    Polyline = 1,
    RichText,
    Pixmap,
    Group,
    ClassObject,
    Link
};

//STSPACESHARED_EXPORT int operator + (int value, const GraphicsItemType& type)
//{
//    return value + static_cast<int>(type);
//}

//int operator + (const GraphicsItemType& type, int value)
//{
//    return value + static_cast<int>(type);
//}

#endif // GRAPHICSITEMTYPE_H
