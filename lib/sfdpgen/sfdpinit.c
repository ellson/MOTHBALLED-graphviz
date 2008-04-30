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
#include <neatoprocs.h>
#include <adjust.h>
#include <pack.h>
#include <assert.h>
#include <ctype.h>
#include <spring_electrical.h>
#include <overlap.h>

#ifdef DEBUG
double _statistics[10];
#endif

static void sfdp_init_node(node_t * n)
{
    common_init_node(n);
    ND_pos(n) = N_NEW(GD_ndim(n->graph), double);
    gv_nodesize(n, GD_flip(n->graph));
}

static void sfdp_init_edge(edge_t * e)
{
    common_init_edge(e);
}

static void sfdp_init_node_edge(graph_t * g)
{
    node_t *n;
    edge_t *e;
    int nnodes = agnnodes(g);
    attrsym_t *N_pos = agfindattr(g->proto->n, "pos");

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	sfdp_init_node(n);
	user_pos(N_pos, NULL, n, nnodes);
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    sfdp_init_edge(e);
    }
}

static void sfdp_init_graph(Agraph_t * g)
{
    setEdgeType(g, ET_LINE);
    g->u.ndim = late_int(g, agfindattr(g, "dim"), 2, 2);
    Ndim = g->u.ndim = MIN(g->u.ndim, MAXDIM);

    sfdp_init_node_edge(g);
}

/* getPos:
 */
static real *getPos(Agraph_t * g)
{
    Agnode_t *n;
    real *pos = N_NEW(Ndim * agnnodes(g), real);
    int ix, i;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	i = ND_id(n);
	if (hasPos(n)) {
	    for (ix = 0; ix < Ndim; ix++) {
		pos[i * Ndim + ix] = ND_pos(n)[ix];
	    }
	}
    }

    return pos;
}

/* getSizes:
 * Set up array of half sizes in inches.
 */
static real *getSizes(Agraph_t * g, pointf pad)
{
    Agnode_t *n;
    real *sizes = N_GNEW(2 * agnnodes(g), real);
    int i;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	i = ND_id(n);
	sizes[i * 2] = ND_width(n) * .5 + pad.x;
	sizes[i * 2 + 1] = ND_height(n) * .5 + pad.y;
    }

    return sizes;
}

/* makeMatrix:
 * Assumes g is connected and simple, i.e., we can have a->b and b->a
 * but not a->b and a->b
 */
static SparseMatrix *makeMatrix(Agraph_t * g, int dim)
{
    SparseMatrix *A = 0;
    Agnode_t *n;
    Agedge_t *e;
    Agsym_t *sym;
    int nnodes;
    int nedges;
    int i, row;
    int *I;
    int *J;
    real *val;
    real v;
    int type = MATRIX_TYPE_REAL;

    if (!g)
	return NULL;
    nnodes = agnnodes(g);
    nedges = agnedges(g);

    /* Assign node ids */
    i = 0;
    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	ND_id(n) = i++;

    I = N_GNEW(nedges, int);
    J = N_GNEW(nedges, int);
    val = N_GNEW(nedges, real);

    sym = agfindattr(g->proto->e, "wt");
    i = 0;
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	row = ND_id(n);
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    I[i] = row;
	    J[i] = ND_id(e->head);
	    if (!sym || (sscanf(agxget(e, sym->index), "%lf", &v) != 1))
		v = 1;
	    val[i] = v;
	    i++;
	}
    }

    A = SparseMatrix_from_coordinate_arrays(nedges, nnodes, nnodes, I, J,
					    val, type);

    free(I);
    free(J);
    free(val);

    return A;
}

int
fdpAdjust (graph_t* g)
{
    SparseMatrix *A = makeMatrix(g, Ndim);
    real *sizes;
    real *pos = N_NEW(Ndim * agnnodes(g), real);
    Agnode_t *n;
    int flag, i;
    expand_t sep = sepFactor(g);
    pointf pad;

    if (sep.doAdd) {
	pad.x = PS2INCH(sep.x);
	pad.y = PS2INCH(sep.y);
    } else {
	pad.x = PS2INCH(DFLT_MARGIN);
	pad.y = PS2INCH(DFLT_MARGIN);
    }
    sizes = getSizes(g, pad);

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	real* npos = pos + (Ndim * ND_id(n));
	for (i = 0; i < Ndim; i++) {
	    npos[i] = ND_pos(n)[i];
	}
    }

    flag = remove_overlap(Ndim, A, pos, sizes, 3);

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	real *npos = pos + (Ndim * ND_id(n));
	for (i = 0; i < Ndim; i++) {
	    ND_pos(n)[i] = npos[i];
	}
    }

    free(sizes);
    free(pos);
    SparseMatrix_delete (A);

    return flag;
}

static void sfdpLayout(graph_t * g, spring_electrical_control * ctrl,
		       pointf pad)
{
    real *sizes;
    real *pos;
    Agnode_t *n;
    int flag, i;

    SparseMatrix *A = makeMatrix(g, Ndim);
    if (ctrl->overlap)
	sizes = getSizes(g, pad);
    else
	sizes = NULL;
    pos = getPos(g);

    flag = multilevel_spring_electrical_embedding(Ndim, A, ctrl, NULL, sizes, pos);

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	real *npos = pos + (Ndim * ND_id(n));
	for (i = 0; i < Ndim; i++) {
	    ND_pos(n)[i] = npos[i];
	}
    }

    free(sizes);
    free(pos);
    SparseMatrix_delete (A);
}

static smooth_t
late_smooth (graph_t* g, Agsym_t* sym, smooth_t dflt)
{
    char* s;
    int v;
    smooth_t rv;

    if (!sym) return dflt;
    s = agxget (g, sym->index);
    if (isdigit(*s)) {
#if HAVE_GTS
	if ((v = atoi (s)) <= SMOOTHING_RNG)
#else
	if ((v = atoi (s)) <= SMOOTHING_SPRING)
#endif
	    rv = (smooth_t)v;
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
#if HAVE_GTS
	else if (!strcasecmp(s, "rng"))
	    rv = SMOOTHING_RNG;
#endif
	else if (!strcasecmp(s, "spring"))
	    rv = SMOOTHING_SPRING;
#if HAVE_GTS
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

/* tuneControl:
 * Use user values to reset control
 * 
 * Possible parameters:
 *   ctrl->use_node_weights
 */
static void
tuneControl (graph_t* g, spring_electrical_control* ctrl)
{
    ctrl->p = -1.0*late_double(g, agfindattr(g, "K"), -AUTOP, 0.0);
    ctrl->multilevels = late_int(g, agfindattr(g, "maxiter"), INT_MAX, 0);
    ctrl->smoothing = late_smooth(g, agfindattr(g, "smooth"), SMOOTHING_NONE);
}

void sfdp_layout(graph_t * g)
{
    sfdp_init_graph(g);

    if (agnnodes(g)) {
	Agraph_t **ccs;
	Agraph_t *sg;
	int ncc;
	int i;
	expand_t sep;
	pointf pad;
	spring_electrical_control *ctrl = spring_electrical_control_new();

	tuneControl (g, ctrl);

#if HAVE_GTS
	if (!agget(g, "overlap") || (graphAdjustMode(g)->mode == AM_FDP)) {
	    ctrl->overlap = 3;  /* FIX: User control? */
	    sep = sepFactor(g);
	    if (sep.doAdd) {
		pad.x = PS2INCH(sep.x);
		pad.y = PS2INCH(sep.y);
	    } else {
		pad.x = PS2INCH(DFLT_MARGIN);
		pad.y = PS2INCH(DFLT_MARGIN);
	    }
	}
#endif

	ccs = ccomps(g, &ncc, 0);
	if (ncc == 1) {
	    sfdpLayout(g, ctrl, pad);
	    adjustNodes(g);
	    spline_edges(g);
	} else {
	    pack_info pinfo;
	    pack_mode pmode = getPackMode(g, l_node);

	    for (i = 0; i < ncc; i++) {
		sg = ccs[i];
		nodeInduce(sg);
		sfdpLayout(g, ctrl, pad);
		adjustNodes(sg);
	    }
	    spline_edges(g);
	    pinfo.margin = getPack(g, CL_OFFSET, CL_OFFSET);
	    pinfo.doSplines = 1;
	    pinfo.mode = pmode;
	    pinfo.fixed = 0;
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
