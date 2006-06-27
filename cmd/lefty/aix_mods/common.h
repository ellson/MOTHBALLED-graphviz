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
#ifdef HAVECS
#include <ast.h>
#else
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <sys/select.h>
#include <math.h>
#include <stdio.h>
#include <setjmp.h>
#include <ctype.h>

#ifdef MSWIN32
#include <windows.h>
#include <commdlg.h>
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

/*#define M_PI 3.14159265358979323846 */

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
    void panic1(char *, int, char *, char *, ...);
    void panic2(char *, int, char *, char *, ...);
#endif				/* _COMMON_H */

#ifdef __cplusplus
}
#endif
