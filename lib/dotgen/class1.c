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
 * Classify edges for rank assignment phase to
 * create temporary edges.
 */

#include "dot.h"


int nonconstraint_edge(edge_t * e)
{
    char *constr;

    if (E_constr && (constr = agxget(e, E_constr->index))) {
	if (constr[0] && mapbool(constr) == FALSE)
	    return TRUE;
    }
    return FALSE;
}

static void 
interclust1(graph_t * g, node_t * t, node_t * h, edge_t * e)
{
    node_t *v, *t0, *h0;
    int offset, t_len, h_len, t_rank, h_rank;
    edge_t *rt, *rh;

    if (ND_clust(e->tail))
	t_rank = ND_rank(e->tail) - ND_rank(GD_leader(ND_clust(e->tail)));
    else
	t_rank = 0;
    if (ND_clust(e->head))
	h_rank = ND_rank(e->head) - ND_rank(GD_leader(ND_clust(e->head)));
    else
	h_rank = 0;
    offset = ED_minlen(e) + t_rank - h_rank;
    if (offset > 0) {
	t_len = 0;
	h_len = offset;
    } else {
	t_len = -offset;
	h_len = 0;
    }

    v = virtual_node(g);
    ND_node_type(v) = SLACKNODE;
    t0 = UF_find(t);
    h0 = UF_find(h);
    rt = make_aux_edge(v, t0, t_len, CL_BACK * ED_weight(e));
    rh = make_aux_edge(v, h0, h_len, ED_weight(e));
    ED_to_orig(rt) = ED_to_orig(rh) = e;
}
void class1(graph_t * g)
{
    node_t *n, *t, *h;
    edge_t *e, *rep;

    mark_clusters(g);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {

	    /* skip edges already processed */
	    if (ED_to_virt(e))
		continue;

	    /* skip edges that we want to ignore in this phase */
	    if (nonconstraint_edge(e))
		continue;

	    t = UF_find(e->tail);
	    h = UF_find(e->head);

	    /* skip self, flat, and intra-cluster edges */
	    if (t == h)
		continue;


	    /* inter-cluster edges require special treatment */
	    if (ND_clust(t) || ND_clust(h)) {
		interclust1(g, e->tail, e->head, e);
		continue;
	    }

	    if ((rep = find_fast_edge(t, h)))
		merge_oneway(e, rep);
	    else
		virtual_edge(t, h, e);

#ifdef NOTDEF
	    if ((t == e->tail) && (h == e->head)) {
		if (rep = find_fast_edge(t, h))
		    merge_oneway(e, rep);
		else
		    virtual_edge(t, h, e);
	    } else {
		f = agfindedge(g, t, h);
		if (f && (ED_to_virt(f) == NULL))
		    rep = virtual_edge(t, h, f);
		else
		    rep = find_fast_edge(t, h);
		if (rep)
		    merge_oneway(e, rep);
		else
		    virtual_edge(t, h, e);
	    }
#endif
	}
    }
}

