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


#include	"vmhdr.h"
/*
 * return a copy of s using vmalloc
 */

char *vmstrdup(Vmalloc_t * v, register const char *s)
{
    register char *t;
    register int n;

    return ((t =
	     vmalloc(v, n =
		     strlen(s) + 1)) ? (char *) memcpy(t, s,
						       n) : (char *) 0);
}
