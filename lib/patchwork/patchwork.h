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

#ifndef PATCHWORK_H
#define PATCHWORK_H

#include "render.h"
#include "../fdpgen/fdp.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
	int nStepsToLeaf;
	int subtreeSize;
	int nChildren;
	int nStepsToCenter;
	node_t *parent;
	double span;
	double theta;
    } rdata;

#define RDATA(n) ((rdata*)((n)->u.alg))
#define SLEAF(n) (RDATA(n)->nStepsToLeaf)
#define STSIZE(n) (RDATA(n)->subtreeSize)
#define NCHILD(n) (RDATA(n)->nChildren)
#define SCENTER(n) (RDATA(n)->nStepsToCenter)
#define SPARENT(n) (RDATA(n)->parent)
#define SPAN(n) (RDATA(n)->span)
#define THETA(n) (RDATA(n)->theta)

extern void patchwork_init_graph(graph_t * g);
extern void twopi_layout(Agraph_t * g);
extern void twopi_cleanup(Agraph_t * g);

#ifdef __cplusplus
}
#endif
#endif
