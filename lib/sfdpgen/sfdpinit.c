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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#ifdef HAVE_VALUES_H
#include <values.h>
#endif
#endif
#include <sfdp.h>
#include <neato.h>
#include <adjust.h>
#include <pack.h>
#include <assert.h>
#include <ctype.h>
#include <spring_electrical.h>
#include <overlap.h>

#ifdef DEBUG
double _statistics[10];
#endif

static void sfdp_init_edge(edge_t * e)
{
    common_init_edge(e);
}

static void sfdp_init_node_edge(graph_t * g)
{
    node_t *n;
    edge_t *e;
#if 0
    int nnodes = agnnodes(g);
    attrsym_t *N_pos = agfindnodeattr(g, "pos");
#endif

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	neato_init_node(n);
#if 0
   FIX so that user positions works with multiscale
	user_pos(N_pos, NULL, n, nnodes); 
#endif
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    sfdp_init_edge(e);
    }
}

static void sfdp_init_graph(Agraph_t * g)
{
    int outdim;
    int temp;

    setEdgeType(g, ET_LINE);
    outdim = late_int(g, agfindgraphattr(g, "dimen"), 2, 2);
    GD_ndim(agroot(g)) = late_int(g, agfindgraphattr(g, "dim"), outdim, 2);
    Ndim = GD_ndim(agroot(g)) = MIN(GD_ndim(agroot(g)), MAXDIM);
    GD_odim(agroot(g)) = MIN(outdim, Ndim);
    sfdp_init_node_edge(g);
}

/* getPos:
 */
static real *getPos(Agraph_t * g, spring_electrical_control ctrl)
{
    Agnode_t *n;
    real *pos = N_NEW(Ndim * agnnodes(g), real);
    int ix, i;

    if (agfindnodeattr(g, "pos") == NULL)
	return pos;

    ctrl->posSet = N_NEW(agnnodes(g), char);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	i = ND_id(n);
	if (hasPos(n)) {
	    for (ix = 0; ix < Ndim; ix++) {
		pos[i * Ndim + ix] = ND_pos(n)[ix];
	    }
	    ctrl->posSet[i] = 1;
	}
	else
	    ctrl->posSet[i] = 0;
    }

    return pos;
}

static void sfdpLayout(graph_t * g, spring_electrical_control ctrl,
		       pointf pad)
{
    real *sizes;
    real *pos;
    Agnode_t *n;
    int flag, i;

    SparseMatrix A = makeMatrix(g, Ndim);
    if (ctrl->overlap >= 0)
	sizes = getSizes(g, pad);
    else
	sizes = NULL;
    pos = getPos(g, ctrl);

    multilevel_spring_electrical_embedding(Ndim, A, ctrl, NULL, sizes, pos, &flag);

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	real *npos = pos + (Ndim * ND_id(n));
	for (i = 0; i < Ndim; i++) {
	    ND_pos(n)[i] = npos[i];
	}
    }

    free(sizes);
    free(pos);
    free(ctrl->posSet);
    SparseMatrix_delete (A);
}

static int
late_smooth (graph_t* g, Agsym_t* sym, int dflt)
{
    char* s;
    int v;
    int rv;

    if (!sym) return dflt;
#ifdef WITH_CGRAPH
    s = agxget (g, sym);
#else
    s = agxget (g, sym->index);
#endif
    if (isdigit(*s)) {
#if (HAVE_GTS || HAVE_TRIANGLE)
	if ((v = atoi (s)) <= SMOOTHING_RNG)
#else
	if ((v = atoi (s)) <= SMOOTHING_SPRING)
#endif
	    rv = v;
	else
	    rv = dflt;
    }
    else if (isalpha(*s)) {
	if (!strcasecmp(s, "avg_dist"))
	    rv = SMOOTHING_STRESS_MAJORIZATION_AVG_DIST;
	else if (!strcasecmp(s, "graph_dist"))
	    rv = SMOOTHING_STRESS_MAJORIZATION_GRAPH_DIST;
	else if (!strcasecmp(s, "none"))
	    rv = SMOOTHING_NONE;
	else if (!strcasecmp(s, "power_dist"))
	    rv = SMOOTHING_STRESS_MAJORIZATION_POWER_DIST;
#if (HAVE_GTS || HAVE_TRIANGLE)
	else if (!strcasecmp(s, "rng"))
	    rv = SMOOTHING_RNG;
#endif
	else if (!strcasecmp(s, "spring"))
	    rv = SMOOTHING_SPRING;
#if (HAVE_GTS || HAVE_TRIANGLE)
	else if (!strcasecmp(s, "triangle"))
	    rv = SMOOTHING_TRIANGLE;
#endif
	else
	    rv = dflt;
    }
    else
	rv = dflt;
    return rv;
}

static int
late_quadtree_scheme (graph_t* g, Agsym_t* sym, int dflt)
{
    char* s;
    int v;
    int rv;

    if (!sym) return dflt;
#ifdef WITH_CGRAPH
    s = agxget (g, sym);
#else
    s = agxget (g, sym->index);
#endif
    if (isdigit(*s)) {
      if ((v = atoi (s)) <= QUAD_TREE_FAST && v >= QUAD_TREE_NONE){
	rv = v;
      }	else {
	rv = dflt;
      }
    } else if (isalpha(*s)) {
      if (!strcasecmp(s, "none") || !strcasecmp(s, "false") ){
	rv = QUAD_TREE_NONE;
      } else if (!strcasecmp(s, "normal") || !strcasecmp(s, "true") || !strcasecmp(s, "yes")){
	rv = QUAD_TREE_NORMAL;
      } else if (!strcasecmp(s, "fast")){
	rv = QUAD_TREE_FAST;
      }	else {
	rv = dflt;
      }
    } else {
      rv = dflt;
    }
    return rv;
}


/* tuneControl:
 * Use user values to reset control
 * 
 * Possible parameters:
 *   ctrl->use_node_weights
 */
static void
tuneControl (graph_t* g, spring_electrical_control ctrl)
{
    long seed;
    int init;

    seed = ctrl->random_seed;
    init = setSeed (g, INIT_RANDOM, &seed);
    if (init != INIT_RANDOM) {
        agerr(AGWARN, "sfdp only supports start=random\n");
    }
    ctrl->random_seed = seed;

    ctrl->K = late_double(g, agfindgraphattr(g, "K"), -1.0, 0.0);
    ctrl->p = -1.0*late_double(g, agfindgraphattr(g, "repulsiveforce"), -AUTOP, 0.0);
    ctrl->multilevels = late_int(g, agfindgraphattr(g, "levels"), INT_MAX, 0);
    ctrl->smoothing = late_smooth(g, agfindgraphattr(g, "smoothing"), SMOOTHING_NONE);
    ctrl->tscheme = late_quadtree_scheme(g, agfindgraphattr(g, "quadtree"), QUAD_TREE_NORMAL);
}

void sfdp_layout(graph_t * g)
{
    int doAdjust;
    adjust_data am;
	sfdp_init_graph(g);
    doAdjust = (Ndim == 2);

    if (agnnodes(g)) {
	Agraph_t **ccs;
	Agraph_t *sg;
	int ncc;
	int i;
	expand_t sep;
	pointf pad;
	spring_electrical_control ctrl = spring_electrical_control_new();

	tuneControl (g, ctrl);
#if (HAVE_GTS || HAVE_TRIANGLE)
	graphAdjustMode(g, &am, "prism0");
#else
	graphAdjustMode(g, &am, 0);
#endif

	if ((am.mode == AM_PRISM) && doAdjust) {
	    doAdjust = 0;  /* overlap removal done in sfpd */
	    ctrl->overlap = am.value;
    	    ctrl->initial_scaling = am.scaling;
	    sep = sepFactor(g);
	    if (sep.doAdd) {
		pad.x = PS2INCH(sep.x);
		pad.y = PS2INCH(sep.y);
	    } else {
		pad.x = PS2INCH(DFLT_MARGIN);
		pad.y = PS2INCH(DFLT_MARGIN);
	    }
	}
	else {
   		/* Turn off overlap removal in sfdp if prism not used */
	    ctrl->overlap = -1;
	}

	ccs = ccomps(g, &ncc, 0);
	if (ncc == 1) {
	    sfdpLayout(g, ctrl, pad);
	    if (doAdjust) removeOverlapWith(g, &am);
	    spline_edges(g);
	} else {
	    pack_info pinfo;
	    getPackInfo(g, l_node, CL_OFFSET, &pinfo);
	    pinfo.doSplines = 1;

	    for (i = 0; i < ncc; i++) {
		sg = ccs[i];
		nodeInduce(sg);
		sfdpLayout(sg, ctrl, pad);
		if (doAdjust) removeOverlapWith(sg, &am);
		setEdgeType(sg, ET_LINE);
		spline_edges(sg);
	    }
	    packSubgraphs(ncc, ccs, g, &pinfo);
	}
	for (i = 0; i < ncc; i++) {
	    agdelete(g, ccs[i]);
	}
	free(ccs);
	spring_electrical_control_delete(ctrl);
    }

    dotneato_postprocess(g);
}

static void sfdp_cleanup_graph(graph_t * g)
{
}

void sfdp_cleanup(graph_t * g)
{
    node_t *n;
    edge_t *e;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    gv_cleanup_edge(e);
	}
	gv_cleanup_node(n);
    }
    sfdp_cleanup_graph(g);
}
 
