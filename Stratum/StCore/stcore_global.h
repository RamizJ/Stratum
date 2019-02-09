#ifndef STCORE_GLOBAL_H
#define STCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STCORE_LIBRARY)
#  define STCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define STCORESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // STCORE_GLOBAL_H
