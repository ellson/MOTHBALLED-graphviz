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
 * The polygon may have concavities.
 * Doesn't work with twisted polygons.
 * From O'Rourke book (via erg@research.att.com)
 */

#include <stdlib.h>
#include <vispath.h>
#include <pathutil.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static Ppoint_t subpt(Ppoint_t p, Ppoint_t q)
{
    Ppoint_t rv;
    rv.x = p.x - q.x;
    rv.y = p.y - q.y;
    return rv;
}

int in_poly(Ppoly_t argpoly, Ppoint_t q)
{
    int i, i1;			/* point index; i1 = i-1 mod n */
    double x;			/* x intersection of e with ray */
    int crossings = 0;		/* 2 * number of edge/ray crossings */
    Ppoly_t poly;		/* original O'Rourke code overwrites the arg polygon! */
    Ppoint_t *P;
    int n;

    /* Shift so that q is the origin. */
    poly = copypoly(argpoly);
    P = poly.ps;
    n = poly.pn;
    for (i = 0; i < n; i++)
	poly.ps[i] = subpt(poly.ps[i], q);

    /* For each edge e=(i-1,i), see if crosses ray. */
    for (i = 0; i < n; i++) {
	i1 = (i + n - 1) % n;

	/* if edge is horizontal, test to see if the point is on it */
	if ((P[i].y == 0) && (P[i1].y == 0)) {
	    if ((P[i].x * P[i1].x) < 0)
		return TRUE;
	    else
		continue;
	}
	/* if e straddles the x-axis... */
	if (((P[i].y >= 0) && (P[i1].y <= 0)) ||
	    ((P[i1].y >= 0) && (P[i].y <= 0))) {
	    /* e straddles ray, so compute intersection with ray. */
	    x = (P[i].x * P[i1].y - P[i1].x * P[i].y)
		/ (double) (P[i1].y - P[i].y);

	    /* if intersect at origin, we've found intersection */
	    if (x == 0)
		return TRUE;

	    /* crosses ray if strictly positive intersection. */
	    if (x > 0) {
		if (P[i].y == 0) {
		    if (P[(i - 1 + n) % n].y * P[(1 + i) % n].y < 0) {
			/* count half a crossing */
			crossings++;
		    } else if (P[i].y * P[(2 + i) % n].y < 0) {
			/* count half a crossing */
			crossings++;
		    }
		} else {
		    /* count a full crossing */
		    crossings += 2;
		}
	    }
	}
    }

    freepoly(poly);

    /* q inside if an odd number of crossings. */
    if ((crossings % 4) >= 2)
	return TRUE;
    else
	return FALSE;
}
