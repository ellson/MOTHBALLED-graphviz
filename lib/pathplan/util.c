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


#include <assert.h>
#include <pathutil.h>
#include <stdlib.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif

Ppoly_t copypoly(Ppoly_t argpoly)
{
    Ppoly_t rv;
    int i;

    rv.pn = argpoly.pn;
    rv.ps = malloc(sizeof(Ppoint_t) * argpoly.pn);
    for (i = 0; i < argpoly.pn; i++)
	rv.ps[i] = argpoly.ps[i];
    return rv;
}

void freepoly(Ppoly_t argpoly)
{
    free(argpoly.ps);
}

int Ppolybarriers(Ppoly_t ** polys, int npolys, Pedge_t ** barriers,
		  int *n_barriers)
{
    Ppoly_t pp;
    int i, j, k, n, b;
    Pedge_t *bar;

    n = 0;
    for (i = 0; i < npolys; i++)
	n = n + polys[i]->pn;

    bar = malloc(n * sizeof(Pedge_t));

    b = 0;
    for (i = 0; i < npolys; i++) {
	pp = *polys[i];
	for (j = 0; j < pp.pn; j++) {
	    k = j + 1;
	    if (k >= pp.pn)
		k = 0;
	    bar[b].a = pp.ps[j];
	    bar[b].b = pp.ps[k];
	    b++;
	}
    }
    assert(b == n);
    *barriers = bar;
    *n_barriers = n;
    return 1;
}
