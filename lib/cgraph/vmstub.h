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

#ifndef _VMSTUB_H
#define _VMSTUB_H
#include <stdlib.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
typedef void Vmalloc_t;
#define vmalloc(heap,size) malloc(size)
#define vmopen(x,y,z) (Vmalloc_t*)(0)
#define vmclose(x) while (0)
#define vmresize(heap,ptr,size,oktomoveflag)  realloc((ptr),(size))
#define vmfree(heap,ptr) free(ptr)
#ifndef EXTERN
#define EXTERN extern
#endif
EXTERN void *Vmregion;
#endif
