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



#ifndef _PACK_H
#define _PACK_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "graph.h"

/* Type indicating granularity and method 
 *  l_undef  - unspecified
 *  l_node   - polyomino using nodes and edges
 *  l_clust  - polyomino using nodes and edges and top-level clusters
 *             (assumes ND_clust(n) unused by application)
 *  l_graph  - polyomino using graph bounding box
 *  l_hull   - polyomino using convex hull (unimplemented)
 *  l_tile   - tiling using graph bounding box (unimplemented)
 *  l_bisect - alternate bisection using graph bounding box (unimplemented)
 */
    typedef enum { l_undef, l_clust, l_node, l_graph } pack_mode;

    typedef struct {
#ifdef UNIMPLEMENTED
	float aspect;		/* desired aspect ratio */
#endif
	unsigned int margin;	/* margin left around objects, in points */
	int doSplines;		/* use splines in constructing graph shape */
	pack_mode mode;		/* granularity and method */
	bool *fixed;		/* fixed[i] == true implies g[i] should not be moved */
    } pack_info;

    extern point *putGraphs(int, Agraph_t **, Agraph_t *, pack_info *);
    extern int shiftGraphs(int, Agraph_t **, point *, Agraph_t *, int);
    extern int packGraphs(int, Agraph_t **, Agraph_t *, pack_info *);
    extern int packSubgraphs(int, Agraph_t **, Agraph_t *, pack_info *);
    extern pack_mode getPackMode(Agraph_t * g, pack_mode dflt);
    extern int getPack(Agraph_t *, int not_def, int dflt);

    extern int isConnected(Agraph_t *);
    extern Agraph_t **ccomps(Agraph_t *, int *, char *);
    extern Agraph_t **pccomps(Agraph_t *, int *, char *, bool *);
    extern int nodeInduce(Agraph_t *);

#ifdef __cplusplus
}
#endif
#endif
