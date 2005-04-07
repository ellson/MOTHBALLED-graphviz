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

/* 
 * in_poly
 * 
 * Test if a point is inside a polygon.
 * The polygon must be convex with vertices in CW order.
 */

#include <stdlib.h>
#include <vispath.h>
#include <pathutil.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int in_poly(Ppoly_t poly, Ppoint_t q)
{
    int i, i1;			/* point index; i1 = i-1 mod n */
    int n;
    Ppoint_t *P;

    P = poly.ps;
    n = poly.pn;
    for (i = 0; i < n; i++) {
	i1 = (i + n - 1) % n;
	if (wind(P[i1],P[i],q) == 1) return FALSE;
    }
    return TRUE;
}
