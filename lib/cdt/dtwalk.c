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

/*	Walk a dictionary and all dictionaries viewed through it.
**	userf:	user function
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if __STD_C
int dtwalk(reg Dt_t * dt, int (*userf) (Dt_t *, Void_t *, Void_t *),
	   Void_t * data)
#else
int dtwalk(dt, userf, data)
reg Dt_t *dt;
int (*userf) ();
Void_t *data;
#endif
{
    reg Void_t *obj, *next;
    reg Dt_t *walk;
    reg int rv;

    for (obj = dtfirst(dt); obj;) {
	if (!(walk = dt->walk))
	    walk = dt;
	next = dtnext(dt, obj);
	if ((rv = (*userf) (walk, obj, data)) < 0)
	    return rv;
	obj = next;
    }
    return 0;
}
