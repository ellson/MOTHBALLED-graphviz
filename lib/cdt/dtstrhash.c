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

#include	"dthdr.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/*	Hashing a string
**
**	Written by Kiem-Phong Vo (05/22/96)
*/
#if __STD_C
uint dtstrhash(reg uint h, Void_t * args, reg int n)
#else
uint dtstrhash(h, args, n)
reg uint h;
Void_t *args;
reg int n;
#endif
{
    reg unsigned char *s = (unsigned char *) args;

    if (n <= 0) {
	for (; (n = *s) != 0; ++s)
	    h = dtcharhash(h, n);
    } else {
	reg unsigned char *ends;
	for (ends = s + n; s < ends; ++s) {
	    n = *s;
	    h = dtcharhash(h, n);
	}
    }

    return h;
}
