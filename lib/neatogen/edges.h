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

#ifdef __cplusplus
extern "C" {
#endif



#ifndef EDGES_H
#define EDGES_H

#include "site.h"

    typedef struct Edge {
	double a, b, c;		/* edge on line ax + by = c */
	Site *ep[2];		/* endpoints (vertices) of edge; initially NULL */
	Site *reg[2];		/* sites forming edge */
	int edgenbr;
    } Edge;

#define le 0
#define re 1

    extern double pxmin, pxmax, pymin, pymax;	/* clipping window */
    extern void edgeinit(void);
    extern void endpoint(Edge *, int, Site *);
    extern void clip_line(Edge * e);
    extern Edge *bisect(Site *, Site *);

#endif

#ifdef __cplusplus
}
#endif
