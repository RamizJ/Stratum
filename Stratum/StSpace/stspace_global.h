#ifndef STSPACE_GLOBAL_H
#define STSPACE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STSPACE_LIBRARY)
#  define STSPACESHARED_EXPORT Q_DECL_EXPORT
#else
#  define STSPACESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // STSPACE_GLOBAL_H
