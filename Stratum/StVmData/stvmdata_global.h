#ifndef STVMDATA_GLOBAL_H
#define STVMDATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STVMDATA_LIBRARY)
#  define STVMDATASHARED_EXPORT Q_DECL_EXPORT
#else
#  define STVMDATASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // STVMDATA_GLOBAL_H
