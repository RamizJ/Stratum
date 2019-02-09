#ifndef STDATA_GLOBAL_H
#define STDATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STDATA_LIBRARY)
#  define STDATASHARED_EXPORT Q_DECL_EXPORT
#else
#  define STDATASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // STDATA_GLOBAL_H
