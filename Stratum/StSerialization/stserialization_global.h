#ifndef STSERIALIZATION_GLOBAL_H
#define STSERIALIZATION_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(STSERIALIZATION_LIBRARY)
#  define STSERIALIZATIONSHARED_EXPORT Q_DECL_EXPORT
#else
#  define STSERIALIZATIONSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // STSERIALIZATION_GLOBAL_H
