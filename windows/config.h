/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GVIZ_CONFIG
#define GVIZ_CONFIG

/* Define if you have the cbrt function.  */
/* #define HAVE_CBRT 1 */

/* Define if you have the setenv function.  */
/* #define HAVE_SETENV 1 */

/* Define if errno is declared in system include files.  */
#define HAVE_ERRNO_DECL 1

/* Specify internal structure of FILE. Pick one. */
/* This is used to determine if more characters are waiting in the stream. */
/* Define if FILE has _r field */
/* #define HAVE_FILE_R 1 */
/* Define if FILE has _IO_read_end field */
/* #define HAVE_FILE_IO_READ_END 1 */
/* Define if FILE has _cnt field */
#define HAVE_FILE_CNT 1
/* Define if FILE has next field */
#define HAVE_FILE_NEXT 1

/* Define if intptr_t is declared */
/* #define HAVE_INTPTR_T 1 */

/* Define to 1 if you have the <inttypes.h> header file. */
/* #define HAVE_INTTYPES_H 1 */

/* Define if you have the vsnprintf function.  */
/* #define HAVE_VSNPRINTF 1 */

/* Define if you have the setmode function.  */
#define HAVE_SETMODE 1

/* Define if you have the iconv function.  */
/* #define HAVE_ICONV 1 */

/* Define if <iconv.h> defines iconv_t. */
/* #define HAVE_ICONV_T_DEF 1 */

/* Define if you have the lsqrt function.  */
#define HAVE_LSQRT 1

/* Define if you have the drand48 function.  */
/* #define HAVE_DRAND48 1 */

/* Define if you have the feenableexcept function.  */
/* #define HAVE_FEENABLEEXCEPT 1 */

/* Define if you have the lrand48 function.  */
#define HAVE_LRAND48 1

/* Define if you have the srand48 function.  */
/* #define HAVE_SRAND48 1 */

/* Define if you have the sincos function.  */
/* #define HAVE_SINCOS 1 */

/* Define if you have the strcasecmp function.  */
/* #define HAVE_STRCASECMP 1 */

/* Define if you have the strncasecmp function.  */
/* #define HAVE_STRNCASECMP 1 */

/* Define if you have the strerror function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have struct dioattr in <sys/fcntl.h>. */
/* #define HAVE_STRUCT_DIOATTR 1 */

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fenv.h> header file.  */
/* #define HAVE_FENV_H 1 */

/* Define if you have the <fpu_control.h> header file.  */
/* #define HAVE_FPU_CONTROL_H 1 */

/* Define to 1 if you have the <ft2build.h> header file. */
#define HAVE_FT2BUILD_H 1

/* Define if the GD library has the GD_FREETYPE feature */
#define HAVE_GD_FREETYPE 1

/* Define if the GD library has the GD_GIF feature */
#define HAVE_GD_GIF 1

/* Define if the GD library has the GD_JPEG feature */
#define HAVE_GD_JPEG 1

/* Define if the GD library has the GD_PNG feature */
#define HAVE_GD_PNG 1

/* Define if you have the <iconv.h> header file.  */
/* #define HAVE_ICONV_H 1 */

/* Define if you have the <sys/fpu.h> header file.  */
/* #define HAVE_SYS_FPU_H 1 */

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
/* #define HAVE_STDINT_H 1 */

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the <dirent.h> header file.  */
/* #define HAVE_DIRENT_H 1 */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #define HAVE_DLFCN_H 1 */

/* Define if you have the <getopt.h> header file.  */
/* #define HAVE_GETOPT_H 1 */

/* Define if you have the <ndir.h> header file.  */
#define HAVE_NDIR_H 1

/* Define if you have the <search.h> header file.  */
#define HAVE_SEARCH_H 1

/* Define if you have the <strings.h> header file.  */
/* #define HAVE_STRINGS_H 1 */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the <sys/ndir.h> header file.  */
/* #define HAVE_SYS_NDIR_H 1 */

/* Define to 1 if you have the <sys/select.h> header file. */
/* #define HAVE_SYS_SELECT_H 1 */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <termios.h> header file.  */
/* #define HAVE_TERMIOS_H 1 */

/* Define if you have the <unistd.h> header file.  */
/* #define HAVE_UNISTD_H 1 */

/* Define if you have the <values.h> header file.  */
/* #define HAVE_VALUES_H 1 */

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <X11/Intrinsic.h> header file. */
/* #define HAVE_X11_INTRINSIC_H 1 */

/* Define to 1 if you have the <X11/Xaw/Text.h> header file. */
/* #define HAVE_X11_XAW_TEXT_H 1 */

/* Define if you have the jpeg library (-ljpeg).  */
#define	HAVE_LIBJPEG    1

/* Define to 1 if you have the `m' library (-lm). */
/* #define HAVE_LIBM 1 */

/* Define if you have the png library (-lpng).  */
#define HAVE_LIBPNG     1

/* Define if you have the freetype library (-lfreetype),  */
#define	HAVE_LIBFREETYPE   1

/* Define if you have the XAW library */
/* #define HAVE_LIBXAW 1 */

/* Define if you have the z library (-lz).  */
#define HAVE_LIBZ       1

/* Define to 1 if you have the `__freadable' function. */
/* #define HAVE___FREADABLE 1 */

/* Define if you have the expat library (-lexpat). */
#define HAVE_LIBEXPAT       1

/* Define if you have the xpm library (-lXpm).  */
/* At present, the xpm library isn't really used */
/* #define HAVE_LIBXPM       1 */

#ifndef DEFAULT_FONTPATH
# ifdef _UWIN
#  define DEFAULT_FONTPATH    "/win/fonts"
#  define PATHSEPARATOR ":"
#  else
#  ifdef MSWIN32
#   define DEFAULT_FONTPATH    "C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
#   define PATHSEPARATOR ";"
#  else
#   define DEFAULT_FONTPATH    "/usr/share/ttf:/usr/local/share/ttf:/usr/share/fonts/ttf:/usr/local/share/fonts/ttf:/usr/lib/fonts:/usr/local/lib/fonts:/usr/lib/fonts/ttf:/usr/local/lib/fonts/ttf:/usr/common/share/fonts/ttf:/usr/common/graphviz/lib/fonts/ttf:/windows/fonts:/dos/windows/fonts:/usr/add-on/share/ttf:/usr/openwin/lib/X11/fonts/TrueType:."
#   define PATHSEPARATOR ":"
#  endif
# endif
#endif

/* No X display on Windows */
#define X_DISPLAY_MISSING 1

/* Define to 1 if you have the ANSI C header files. */
/* <stdlib.h> <stdarg.h> <string.h> <float.h> */
#define STDC_HEADERS 1

/* Set to 0 if you don't want the old codegens */
#define ENABLE_CODEGENS 1

/* Set to 1 if you want the new gvrender codegens */
#define ENABLE_GVRENDER 0

/* Define if you want the gd renderers (png, gif, etc ) */
#define GD_RENDER 1

/* Default DPI. */
#define DEFAULT_DPI 96

#endif

#ifdef __cplusplus
}
#endif
