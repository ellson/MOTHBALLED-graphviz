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


/* fdpinit.c:
 * Written by Emden R. Gansner
 *
 * Mostly boilerplate initialization and cleanup code.
 */

/* uses PRIVATE interface */
#define FDP_PRIVATE 1

#include    "tlayout.h"
#include    "neatoprocs.h"
#include    "agxbuf.h"

static void initialPositions(graph_t * g)
{
    int i;
    node_t *np;
    attrsym_t *possym;
    attrsym_t *pinsym;
    double *pvec;
    char *p;
    char c;

    possym = agfindattr(g->proto->n, "pos");
    if (!possym)
	return;
    pinsym = agfindattr(g->proto->n, "pin");
    for (i = 0; (np = GD_neato_nlist(g)[i]); i++) {
	p = agxget(np, possym->index);
	if (p[0]) {
	    pvec = ND_pos(np);
	    c = '\0';
	    if (sscanf(p, "%lf,%lf%c", pvec, pvec + 1, &c) >= 2) {
		if (PSinputscale > 0.0) {
		    int i;
		    for (i = 0; i < NDIM; i++)
			pvec[i] = pvec[i] / PSinputscale;
		}
		ND_pinned(np) = P_SET;
		if ((c == '!')
		    || (pinsym && mapbool(agxget(np, pinsym->index))))
		    ND_pinned(np) = P_PIN;
	    } else
		fprintf(stderr,
			"Warning: node %s, position %s, expected two floats\n",
			np->name, p);
	}
    }
}

static void fdp_initNode(node_t * n)
{
    neato_init_node(n);
}

/* init_edge:
 */
static void init_edge(edge_t * e, attrsym_t * E_len)
{
    ED_factor(e) = late_double(e, E_weight, 1.0, 0.0);
    ED_dist(e) = late_double(e, E_len, fdp_parms.K, 0.0);

    common_init_edge(e);
}

void fdp_init_node_edge(graph_t * g)
{
    attrsym_t *E_len;
    node_t *n;
    edge_t *e;
    int nn = agnnodes(g);
    int i;
    ndata* alg = N_NEW(nn, ndata);

    processClusterEdges(g);

    GD_neato_nlist(g) = N_NEW(nn + 1, node_t *);

    for (i = 0, n = agfstnode(g); n; n = agnxtnode(g, n)) {
	fdp_initNode (n);
	ND_alg(n) = alg + i;
	GD_neato_nlist(g)[i] = n;
	ND_id(n) = i++;
    }

    E_len = agfindattr(g->proto->e, "len");
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    init_edge(e, E_len);
	}
    }
    initialPositions(g);

}

static void cleanup_subgs(graph_t * g)
{
    graph_t *mg;
    edge_t *me;
    node_t *mn;
    graph_t *subg;

    mg = g->meta_node->graph;
    for (me = agfstout(mg, g->meta_node); me; me = agnxtout(mg, me)) {
	mn = me->head;
	subg = agusergraph(mn);
	free_label(GD_label(subg));
	if (GD_alg(subg)) {
	    free(PORTS(subg));
	    free(GD_alg(subg));
	}
	cleanup_subgs(subg);
    }
}

static void fdp_cleanup_graph(graph_t * g)
{
    cleanup_subgs(g);
    free(GD_neato_nlist(g));
    free(GD_alg(g));
    if (g != g->root) memset(&(g->u), 0, sizeof(Agraphinfo_t));
}

void fdp_cleanup(graph_t * g)
{
    node_t *n;
    edge_t *e;

    n = agfstnode(g);
    free(ND_alg(n));
    for (; n; n = agnxtnode(g, n)) {
	for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	    gv_cleanup_edge(e);
	}
	gv_cleanup_node(n);
    }
    fdp_cleanup_graph(g);
}
