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

#ifndef FDP_H
#define FDP_H

#include <render.h>

#ifdef FDP_PRIVATE

#define NDIM     2

typedef struct bport_s {
    edge_t *e;
    node_t *n;
    double alpha;
} bport_t;

/* gdata is attached to the root graph, each cluster graph, 
 * and to each derived graph.
 * Graphs also use "builtin" fields:
 *   n_cluster, clust - to record clusters  
 */
typedef struct {
    bport_t *ports;		/* boundary ports. 0-terminated */
    int nports;			/* no. of ports */
    boxf bb;			/* bounding box of graph */
    int flags;
    int level;			/* depth in graph hierarchy */
    graph_t *parent;		/* smallest containing cluster */
#ifdef DEBUG
    graph_t *orig;		/* original of derived graph */
#endif
} gdata;

#define GDATA(g)    ((gdata*)(GD_alg(g)))
#define BB(g)       (GDATA(g)->bb)
#define PORTS(g)    (GDATA(g)->ports)
#define NPORTS(g)   (GDATA(g)->nports)
#define LEVEL(g)    (GDATA(g)->level)
#define GPARENT(g)  (GDATA(g)->parent)
#ifdef DEBUG
#define GORIG(g)    (GDATA(g)->orig)
#endif

/* ndata is attached to nodes in real graphs.
 * Real nodes also use "builtin" fields:
 *   pos   - position information
 *   width,height     - node dimensions
 *   xsize,ysize      - node dimensions in points
 */
typedef struct {
    node_t *dn;			/* points to corresponding derived node,
				 * which may represent the node or its
				 * containing cluster. */
    graph_t *parent;		/* smallest containing cluster */
} ndata;

#define NDATA(n) ((ndata*)(ND_alg(n)))
#define DNODE(n) (NDATA(n)->dn)
#define PARENT(n) (NDATA(n)->parent)

/* dndata is attached to nodes in derived graphs.
 * Derived nodes also use "builtin" fields:
 *   clust - for cluster nodes, points to cluster in real graph.
 *   pos   - position information
 *   width,height     - node dimensions
 */
typedef struct {
    int deg;			/* degree of node */
    int wdeg;			/* weighted degree of node */
    node_t *dn;			/* If derived node is not a cluster, */
    /* dn points real node. */
    double disp[NDIM];		/* incremental displacement */
} dndata;

#define DNDATA(n) ((dndata*)(ND_alg(n)))
#define DISP(n) (DNDATA(n)->disp)
#define ANODE(n) (DNDATA(n)->dn)
#define DEG(n) (DNDATA(n)->deg)
#define WDEG(n) (DNDATA(n)->wdeg)
#define IS_PORT(n) (!ANODE(n) && !ND_clust(n))

#endif				/*  FDP_PRIVATE */

#ifdef __cplusplus
extern "C" {
#endif

    extern void fdp_layout(Agraph_t * g);
    extern void fdp_nodesize(node_t *, bool);
    extern void fdp_init_graph(Agraph_t * g);
    extern void fdp_init_node_edge(Agraph_t * g);
    extern void fdp_cleanup(Agraph_t * g);

#ifdef __cplusplus
}
#endif
#endif
