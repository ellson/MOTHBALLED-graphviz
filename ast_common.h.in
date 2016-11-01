#ifndef _AST_COMMON_H
#define _AST_COMMON_H   1

#include "config.h"

#ifdef HAVE_SYS_TYPES_H
#  define _sys_types 1
#endif

#ifdef HAVE_SYS_TYPES_H
#  define _sys_types 1
#endif

#ifdef HAVE_SYS_SOCKET_H
#  define _sys_socket 1
#endif

#ifdef HAVE_SYS_STAT_H
#  define _sys_stat 1
#endif

#ifdef HAVE_STDARG_H
#  define _hdr_stdarg 1
#endif

#ifdef HAVE_VARARGS_H
#  define _hdr_varargs 1
#endif

/* FIXME - need proper config test for these */
#define _typ_ssize_t    1	/* ssize_t is a type */
#define _typ_long_double        1	/* long double is a type */
/* */

/* extern symbols must be protected against C++ name mangling */
#ifndef _BEGIN_EXTERNS_
#  if defined(__cplusplus) || defined(c_plusplus)
#    define _BEGIN_EXTERNS_ extern "C" {
#    define _END_EXTERNS_   }
#  else
#    define _BEGIN_EXTERNS_
#    define _END_EXTERNS_
#  endif
#endif /*_BEGIN_EXTERNS_*/

/* dynamic linked library external scope handling */
#undef extern
#if defined(_dll_import) && !defined(__EXPORT__) && _DLL_BLD
#define __EXPORT__      __declspec(dllexport)
#endif
#if defined(_dll_import) && !defined(__IMPORT__)
#define __IMPORT__      __declspec(dllimport)
#endif
#if !defined(_astimport)
#if defined(__IMPORT__) && _DLL_BLD
#define _astimport      __IMPORT__
#else
#define _astimport      extern
#endif
#endif /*_astimport*/

#ifndef _AST_STD_H
#       if defined(_hdr_stdarg)
#       include <stdarg.h>
#       endif
#       if defined(_hdr_varargs)
#       include <varargs.h>
#       endif
#       if defined(_sys_types)
#       include <sys/types.h>
#       endif
#endif
#if !_typ_ssize_t
#       define _typ_ssize_t     1
typedef int ssize_t;
#endif

/* FIXME - need proper configure tests for these */
#define _ast_int1_t             char
#define _ast_int2_t             short
#define _ast_int4_t             int
#define _ast_int8_t             long long
#define _ast_intmax_t           _ast_int8_t
#define _ast_intswap            7

#define _ast_flt4_t             float
#define _ast_flt8_t             double
#define _ast_flt12_t            long double
#define _ast_fltmax_t           _ast_flt12_t
/* */

#ifndef va_listref
#define va_listref(p) (p)	/* pass va_list to varargs function */
#define va_listval(p) (p)	/* retrieve va_list from va_arg(ap,va_listarg) */
#define va_listarg va_list	/* va_arg() va_list type */
#ifndef va_copy
#define va_copy(to,fr) ((to)=(fr))	/* copy va_list fr -> to */
#endif
#undef  _ast_va_list
#ifdef  va_start
#define _ast_va_list va_list
#else
#define _ast_va_list void*	/* va_list that avoids #include */
#endif
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#ifdef HAVE_VALUES_H
#include <values.h>
#endif
#endif

#ifndef MAXINT
#define MAXINT INT_MAX
#endif

#endif				/*AST_COMMON_H */
