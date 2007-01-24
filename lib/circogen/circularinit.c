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
 * Circular layout. Biconnected components are put on circles.
 * block-cutnode tree is done recursively, with children placed
 * about parent block.
 * Based on:
 *   Six and Tollis, "A Framework for Circular Drawings of
 * Networks", GD '99, LNCS 1731, pp. 107-116;
 *   Six and Tollis, "Circular Drawings of Biconnected Graphs",
 * Proc. ALENEX '99, pp 57-73.
 *   Kaufmann and Wiese, "Maintaining the Mental Map for Circular
 * Drawings", GD '02, LNCS 2528, pp. 12-22.
 */

#include    "circular.h"
#include    "adjust.h"
#include    "pack.h"
#include    "neatoprocs.h"
#include    <string.h>

static void circular_init_node(node_t * n)
{
    common_init_node(n);

    neato_nodesize(n, GD_flip(n->graph));
    ND_pos(n) = N_NEW(GD_ndim(n->graph), double);
}

static void circular_init_edge(edge_t * e)
{
    common_init_edge(e);

    ED_factor(e) = late_double(e, E_weight, 1.0, 0.0);
}


static void circular_init_node_edge(graph_t * g)
{
    node_t *n;
    edge_t *e;
    int i = 0;
    ndata* alg = N_NEW(agnnodes(g), ndata);

    GD_neato_nlist(g) = N_NEW(agnnodes(g) + 1, node_t *);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	ND_alg(n) = alg + i;
	GD_neato_nlist(g)[i++] = n;
	circular_init_node(n);
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    circular_init_edge(e);
	}
    }
}


void circo_init_graph(graph_t * g)
{
    setEdgeType (g, ET_LINE);
    /* GD_ndim(g) = late_int(g,agfindattr(g,"dim"),2,2); */
    Ndim = GD_ndim(g) = 2;	/* The algorithm only makes sense in 2D */
    circular_init_node_edge(g);
}

/* makeDerivedNode:
 * Make a node in the derived graph, with the given name.
 * orig points to what it represents, either a real node or
 * a cluster. Copy size info from original node; needed for
 * adjustNodes and packSubgraphs.
 */
static node_t *makeDerivedNode(graph_t * dg, char *name, int isNode,
			       void *orig)
{
    node_t *n = agnode(dg, name);
    ND_alg(n) = (void *) NEW(cdata);
    if (isNode) {
	ND_pos(n) = N_NEW(Ndim, double);
	ND_xsize(n) = ND_xsize((node_t *) orig);
	ND_ysize(n) = ND_ysize((node_t *) orig);
	ORIGN(n) = (node_t *) orig;
    } else
	ORIGG(n) = (graph_t *) orig;
    return n;
}

/* circomps:
 * Construct a strict, undirected graph with no loops from g.
 * Construct the connected components with the provision that all
 * nodes in a block subgraph are considered connected.
 * Return array of components with number of components in cnt.
 * Each component has its blocks as subgraphs.
 * FIX: Check that blocks are disjoint.
 */
Agraph_t **circomps(Agraph_t * g, int *cnt)
{
    int c_cnt;
    Agraph_t **ccs;
    Agraph_t *dg;
    Agnode_t *n, *v, *dt, *dh;
    Agedge_t *e;
    Agraph_t *sg;
    int i;
    Agedge_t *ep;
    Agnode_t *p;
#ifdef USER_BLOCKS
    Agraph_t *ssg, *ssgl, *subg;
    Agnode_t *t;
    Agedge_t *me;
#endif

    dg = agopen("derived", AGFLAG_STRICT);
#ifdef USER_BLOCKS
    sg = g->meta_node->graph;
    for (me = agfstout(sg, g->meta_node); me; me = agnxtout(sg, me)) {
	subg = agusergraph(me->head);

	if (strncmp(subg->name, "block", 5) != 0)
	    continue;

	if (agnnodes(subg) == 0)
	    continue;

	n = makeDerivedNode(dg, subg->name, 0, subg);
	for (v = agfstnode(subg); v; v = agnxtnode(subg, v)) {
	    DNODE(v) = n;
	}
    }
#endif

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	if (DNODE(v))
	    continue;
	n = makeDerivedNode(dg, v->name, 1, v);
	DNODE(v) = n;
    }

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
	    dt = DNODE(e->tail);
	    dh = DNODE(e->head);
	    if (dt != dh)
		agedge(dg, dt, dh);
	}
    }

    ccs = ccomps(dg, &c_cnt, 0);

    /* replace block nodes with block contents */
    for (i = 0; i < c_cnt; i++) {
	sg = ccs[i];

#ifdef USER_BLOCKS
	for (n = agfstnode(sg); n; n = agnxtnode(sg, n)) {
	    /* Expand block nodes, and create block subgraph in sg */
	    if (agobjkind(ORIGN(n)) != AGNODE) {
		ssg = ORIGG(n);
		free(ND_alg(n));
		agdelete(n->graph, n);
		ssgl = agsubg(sg, ssg->name);
		for (t = agfstnode(ssg); t; t = agnxtnode(ssg, t)) {
		    p = makeDerivedNode(dg, t->name, 1, t);
		    DNODE(t) = p;
		    aginsert(ssgl, p);
		}
	    }
	}
#endif

	/* add edges: since sg is a union of components, all edges
	 * of any node should be added, except loops.
	 */
	for (n = agfstnode(sg); n; n = agnxtnode(sg, n)) {
	    p = ORIGN(n);
	    for (e = agfstout(g, p); e; e = agnxtout(g, e)) {
		/* n = DNODE(e->tail); by construction since e->tail == p */
		dh = DNODE(e->head);
		if (n != dh) {
		    ep = agedge(dg, n, dh);
		    aginsert(sg, ep);
		}
	    }
	}
    }

    /* Finally, add edge data to edges */
    for (n = agfstnode(dg); n; n = agnxtnode(dg, n)) {
	for (e = agfstout(dg, n); e; e = agnxtout(dg, e)) {
	    ED_alg(e) = NEW(edata);
	}
    }

    *cnt = c_cnt;
    return ccs;
}

/* closeDerivedGraph:
 */
static void closeDerivedGraph(graph_t * g)
{
    node_t *n;
    edge_t *e;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    free(ED_alg(e));
	}
	free(ND_alg(n));
	free(ND_pos(n));
    }
    agclose(g);
}

/* copyPosns:
 * Copy position of nodes in given subgraph of derived graph
 * to corresponding node in original graph.
 * FIX: consider assigning from n directly to ORIG(n).
 */
static void copyPosns(graph_t * g)
{
    node_t *n;
    node_t *v;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	v = ORIGN(n);
	ND_pos(v)[0] = ND_pos(n)[0];
	ND_pos(v)[1] = ND_pos(n)[1];
    }
}

/* circoLayout:
 */
void circoLayout(Agraph_t * g)
{
    Agraph_t **ccs;
    Agraph_t *sg;
    int ncc;
    int i;

    if (agnnodes(g)) {
	ccs = circomps(g, &ncc);

	if (ncc == 1) {
	    circularLayout(ccs[0]);
	    copyPosns(ccs[0]);
	    adjustNodes(g);
	} else {
	    Agraph_t *dg = ccs[0]->root;
	    pack_info pinfo;
	    pack_mode pmode = getPackMode(g, l_node);

	    for (i = 0; i < ncc; i++) {
		sg = ccs[i];
		circularLayout(sg);
		adjustNodes(sg);
	    }
	    pinfo.margin = getPack(g, CL_OFFSET, CL_OFFSET);
	    /* FIX: splines have not been calculated for dg
	     * To use, either do splines in dg and copy to g, or
	     * construct components of g from ccs and use that in packing.
	     */
	    pinfo.doSplines = 1;
	    pinfo.mode = pmode;
	    pinfo.fixed = 0;
	    packSubgraphs(ncc, ccs, dg, &pinfo);
	    for (i = 0; i < ncc; i++)
		copyPosns(ccs[i]);
	}
    }
}

/* circo_layout:
 */
void circo_layout(Agraph_t * g)
{
    if (agnnodes(g) == 0) return;
    circo_init_graph(g);
    circoLayout(g);
    free(ND_alg(agfstnode(g)));
    spline_edges(g);
    dotneato_postprocess(g);
}

static void circular_cleanup_node(node_t * n)
{
    free(ND_pos(n));
    if (ND_shape(n))
	ND_shape(n)->fns->freefn(n);
    free_label(ND_label(n));
    memset(&(n->u), 0, sizeof(Agnodeinfo_t));
}

static void circular_free_splines(edge_t * e)
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

static void circular_cleanup_edge(edge_t * e)
{
    circular_free_splines(e);
    free_label(ED_label(e));
    memset(&(e->u), 0, sizeof(Agedgeinfo_t));
}

void circo_cleanup(graph_t * g)
{
    node_t *n;
    edge_t *e;

    n = agfstnode(g);
    if (n == NULL)
	return;			/* g is empty */

    closeDerivedGraph(DNODE(n)->graph);	/* delete derived graph */

    for (; n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    circular_cleanup_edge(e);
	}
	circular_cleanup_node(n);
    }
    free(GD_neato_nlist(g));
    if (g != g->root) memset(&(g->u), 0, sizeof(Agraphinfo_t));
}
