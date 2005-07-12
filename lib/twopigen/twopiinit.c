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
 * Written by Emden R. Gansner
 * Derived from Graham Wills' algorithm described in GD'97.
 */

#include    "circle.h"
#include    "adjust.h"
#include    "pack.h"
#include    "neatoprocs.h"


void twopi_nodesize(node_t * n, boolean flip)
{
    int w;

    w = ND_xsize(n) = POINTS(ND_width(n));
    ND_lw_i(n) = ND_rw_i(n) = w / 2;
    ND_ht_i(n) = ND_ysize(n) = POINTS(ND_height(n));
}

static void twopi_init_node(node_t * n)
{
    common_init_node(n);

    twopi_nodesize(n, GD_flip(n->graph));
    ND_pos(n) = ALLOC(GD_ndim(n->graph), 0, double);
    ND_alg(n) = (void *) NEW(rdata);
}

static void twopi_init_edge(edge_t * e)
{
    common_init_edge(e);

    ED_factor(e) = late_double(e, E_weight, 1.0, 0.0);
}

static void twopi_init_node_edge(graph_t * g)
{
    node_t *n;
    edge_t *e;
    int i = 0;

    GD_neato_nlist(g) = N_NEW(agnnodes(g) + 1, node_t *);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	GD_neato_nlist(g)[i++] = n;
	twopi_init_node(n);
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    twopi_init_edge(e);
	}
    }
}

void twopi_init_graph(graph_t * g)
{
    /* GD_ndim(g) = late_int(g,agfindattr(g,"dim"),2,2); */
    Ndim = GD_ndim(g) = 2;	/* The algorithm only makes sense in 2D */
    twopi_init_node_edge(g);
}

/* twopi_layout:
 */
void twopi_layout(Agraph_t * g)
{
    Agnode_t *ctr = 0;
    char *s;

    twopi_init_graph(g);
    s = agget(g, "root");
    if (s && (*s != '\0')) {
	ctr = agfindnode(g, s);
	if (!ctr) {
	    agerr(AGWARN, "specified root node \"%s\" was not found.", s);
	    agerr(AGPREV, "Using default calculation for root node\n");
	}
    }
    if (agnnodes(g)) {
	Agraph_t **ccs;
	Agraph_t *sg;
	Agnode_t *c = NULL;
	int ncc;
	int i;

	ccs = ccomps(g, &ncc, 0);
	if (ncc == 1) {
	    circleLayout(g, ctr);
	    adjustNodes(g);
	    spline_edges(g);
	} else {
	    pack_info pinfo;
	    pack_mode pmode = getPackMode(g, l_node);

	    for (i = 0; i < ncc; i++) {
		sg = ccs[i];
		if (ctr && agcontains(sg, ctr))
		    c = ctr;
		else
		    c = 0;
		nodeInduce(sg);
		circleLayout(sg, c);
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
    }
    dotneato_postprocess(g, twopi_nodesize);

}

static void twopi_cleanup_node(node_t * n)
{
    free(ND_alg(n));
    if (ND_shape(n))
	ND_shape(n)->fns->freefn(n);
    free_label(ND_label(n));
    memset(&(n->u), 0, sizeof(Agnodeinfo_t));
}

static void twopi_free_splines(edge_t * e)
{
    int i;
    if (ED_spl(e)) {
	for (i = 0; i < ED_spl(e)->size; i++)
	    free(ED_spl(e)->list[i].list);
	free(ED_spl(e)->list);
	free(ED_spl(e));
    }
    ED_spl(e) = NULL;
}

static void twopi_cleanup_edge(edge_t * e)
{
    twopi_free_splines(e);
    free_label(ED_label(e));
    memset(&(e->u), 0, sizeof(Agedgeinfo_t));
}

static void twopi_cleanup_graph(graph_t * g)
{
    free(GD_neato_nlist(g));
}

void twopi_cleanup(graph_t * g)
{
    node_t *n;
    edge_t *e;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    twopi_cleanup_edge(e);
	}
	twopi_cleanup_node(n);
    }
    twopi_cleanup_graph(g);
}
