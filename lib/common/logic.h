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

typedef unsigned char boolean;

#define NOT(v) (!(v))
#ifndef FALSE
#define	FALSE 0
#endif
#ifndef TRUE
#define TRUE NOT(FALSE)
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

#ifdef __cplusplus
}
#endif

#endif

