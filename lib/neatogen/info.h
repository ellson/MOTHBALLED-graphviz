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



#ifndef INFO_H
#define INFO_H

#include "voronoi.h"
#include "poly.h"
#include "graph.h"

    typedef struct ptitem {	/* Point list */
	struct ptitem *next;
	Point p;
    } PtItem;

    typedef struct {		/* Info concerning site */
	Agnode_t *node;		/* libgraph node */
	Site site;		/* site used by voronoi code */
	int overlaps;		/* true if node overlaps other nodes */
	Poly poly;		/* polygon at node */
	PtItem *verts;		/* sorted list of vertices of */
	/* voronoi polygon */
    } Info_t;

    extern Info_t *nodeInfo;	/* Array of node info */

    extern void infoinit(void);
    /* Insert vertex into sorted list */
    extern void addVertex(Site *, double, double);
#endif

#ifdef __cplusplus
}
#endif
