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

#ifndef GV_LOGIC_H
#define GV_LOGIC_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined HAVE_STDBOOL_H && ! defined __cplusplus
#include <stdbool.h>
#endif

#ifndef NOT
#define NOT(v) (!(v))
#endif

#if 0
/* HAVE_BOOL only tested the CC compiler, we know C++ must define bool */
#if ! defined HAVE_BOOL && ! defined __cplusplus
typedef unsigned char bool;
#define false 0
#define true NOT(false)
#endif
#endif

#ifndef FALSE
#define	FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif

#ifndef NOTUSED
#define NOTUSED(var) (void) var
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef NIL
#define NIL(type) ((type)0)
#endif

    extern unsigned char mapbool(char *);
#ifdef __cplusplus
}
#endif

#endif

