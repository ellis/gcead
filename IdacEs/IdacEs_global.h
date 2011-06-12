#ifndef IDACES_GLOBAL_H
#define IDACES_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IDACES_LIBRARY)
#  define IDACESSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IDACESSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IDACES_GLOBAL_H
