#ifndef _SYSCONF_H_
#define _SYSCONF_H_

#cmakedefine DEFUMASK @DEFUMASK@
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_STDDEF_H 1
#cmakedefine HAVE_BACKTRACE 1
#cmakedefine HAVE_GETHOSTBYNAME 1
#cmakedefine HAVE_GETTIMEOFDAY 1
#cmakedefine HAVE_SETGRENT 1
#cmakedefine HAVE_STRCASECMP 1
#cmakedefine HAVE_STRICMP 1
#cmakedefine HAVE_STRINGS_H 1
#cmakedefine HAVE_STRLCAT 1
#cmakedefine HAVE_STRLCPY 1
#cmakedefine HAVE_SYS_SELECT_H 1
#cmakedefine HAVE_UMASK 1
#cmakedefine HAVE_VA_LIST_AS_ARRAY 1
// Temporary, change elsewhere to be SERVICES_DIR/modules/
#define MODULE_PATH "@SERVICES_DIR@/modules/"
#cmakedefine RUNGROUP "@RUNGROUP@"
#cmakedefine SERVICES_BIN "@SERVICES_BIN@"
#cmakedefine SERVICES_DIR "@SERVICES_DIR@"

// Temporary, remove from here later as well as elsewhere in the code
#define DL_PREFIX ""

#cmakedefine HAVE_INT16_T 1
#cmakedefine HAVE_UINT16_T 1
#cmakedefine HAVE_U_INT16_T 1
#cmakedefine HAVE_INT32_T 1
#cmakedefine HAVE_UINT32_T 1
#cmakedefine HAVE_U_INT32_T 1

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_STDDEF_H
# include <stddef.h>
#endif

#ifdef HAVE_INT16_T
typedef int16_t int16;
#else
typedef short int16;
#endif

#ifdef HAVE_UINT16_T
typedef uint16_t uint16;
#else
# ifdef HAVE_U_INT16_T
typedef u_int16_t uint16;
# else
typedef unsigned short uint16;
# endif
#endif

#ifdef HAVE_INT32_T
typedef int32_t int32;
#else
typedef long int32;
#endif

#ifdef HAVE_UINT32_T
typedef uint32_t uint32;
#else
# ifdef HAVE_U_INT32_T
typedef u_int32_t uint32;
# else
typedef unsigned long uint32;
# endif
#endif

#endif
