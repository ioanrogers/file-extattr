#ifndef EXTATTR_OS_H
#define EXTATTR_OS_H

/* OS detection */
#include <sys/param.h>

#ifdef BSD
#define EXTATTR_BSD
#endif

#ifdef __APPLE__
#define EXTATTR_MACOSX
#endif

#if defined(sun) || defined(__sun)
#if defined(__SVR4) || defined(__svr4__)
#define EXTATTR_SOLARIS
#endif
#endif

/* Include appropriate header for this OS, defaulting to Linux-style */
#if defined(EXTATTR_BSD)
#include "extattr_bsd.h"
#elif defined(EXTATTR_MACOSX)
#include "extattr_macosx.h"
#elif defined(EXTATTR_SOLARIS)
#include "extattr_solaris.h"
#else
#include "extattr_linux.h"
#endif

#endif
