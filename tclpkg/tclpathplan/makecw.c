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
 * Force the vertices of a polygon to be in CW order.
 * 
 * Works for polygons with concavities.
 * Does not work for twisted polygons.
 *
 * ellson@graphviz.org    October 2nd, 1996
 */

#include <pathutil.h>

void make_CW(Ppoly_t * poly)
{
    int i, j, n;
    Ppoint_t *P;
    Ppoint_t tP;
    double area = 0.0;

    P = poly->ps;
    n = poly->pn;
    /* points or lines don't have a rotation */
    if (n > 2) {
	/* check CW or CCW by computing (twice the) area of poly */
	for (i = 1; i < n - 1; i++) {
	    area += area2(P[0], P[i + 1], P[i]);
	}
	/* if the area is -ve then the rotation needs to be reversed */
	/* the starting point is left unchanged */
	if (area < 0.0) {
	    for (i = 1, j = n - 1; i < 1 + n / 2; i++, j--) {
		tP = P[i];
		P[i] = P[j];
		P[j] = tP;
	    }
	}
    }
}
