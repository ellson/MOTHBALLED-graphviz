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


/* Lefteris Koutsofios - AT&T Bell Laboratories */

#ifndef _COMMON_H
#define _COMMON_H

/* some config and conversion definitions from graphviz distribution */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef MSWIN32
#define FEATURE_WIN32
#define FEATURE_MS
#else
#ifndef FEATURE_GTK
#define FEATURE_X11
#endif
#endif
#ifdef HAVECS
#define FEATURE_CS
#endif
#ifdef HAVENETSCAPE
#define FEATURE_NETSCAPE
#endif
#ifdef HAVEGMAP
#define FEATURE_GMAP
#define FEATURE_MINTSIZE
#endif
#ifdef HAVEDOT
#define FEATURE_DOT
#endif
#ifdef GNU
#define FEATURE_GNU
#endif
#ifdef HAVERUSAGE
#define FEATURE_RUSAGE
#endif
/* */

#ifdef FEATURE_CS
#include <ast.h>
#else
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#endif

#include <math.h>
#include <stdio.h>
#include <setjmp.h>
#include <ctype.h>

#ifdef FEATURE_WIN32
#include <windows.h>
#include <commdlg.h>
#endif
#ifdef FEATURE_MS
#include <malloc.h>
#endif

#define POS __FILE__, __LINE__

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#ifndef L_SUCCESS
#define L_SUCCESS 1
#define L_FAILURE 0
#endif

#define CHARSRC 0
#define FILESRC 1

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef REALSTRCMP
#define Strcmp(s1, s2) ( \
    *(s1) == *(s2) ? ( \
        (*s1) ? strcmp ((s1) + 1, (s2) + 1) : 0 \
    ) : (*(s1) < *(s2) ? -1 : 1) \
)
#else
#define Strcmp(s1, s2) strcmp ((s1), (s2))
#endif

    extern int warnflag;
    extern char *leftypath, *leftyoptions, *shellpath;
    extern jmp_buf exitljbuf;
    extern int idlerunmode;
    extern fd_set inputfds;

    int init(char *);
    void term(void);
    char *buildpath(char *, int);
    char *buildcommand(char *, char *, int, int, char *);
    void warning(char *, int, char *, char *, ...);
    void panic(char *, int, char *, char *, ...);
    void panic2(char *, int, char *, char *, ...);
#endif				/* _COMMON_H */

#ifdef __cplusplus
}
#endif
