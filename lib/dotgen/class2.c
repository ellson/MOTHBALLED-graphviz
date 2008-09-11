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


/* classify edges for mincross/nodepos/splines, using given ranks */

#include "dot.h"

static node_t*
label_vnode(graph_t * g, edge_t * orig)
{
    node_t *v;
    pointf dimen;

    dimen = ED_label(orig)->dimen;
    v = virtual_node(g);
    ND_label(v) = ED_label(orig);
    ND_lw(v) = GD_nodesep(v->graph->root);
    if (!ED_label_ontop(orig)) {
	if (GD_flip(g->root)) {
	    ND_ht(v) = dimen.x;
	    ND_rw(v) = dimen.y;
	} else {
	    ND_ht(v) = dimen.y;
	    ND_rw(v) = dimen.x;
	}
    }
    return v;
}

static void 
incr_width(graph_t * g, node_t * v)
{
    int width = GD_nodesep(g) / 2;
    ND_lw(v) += width;
    ND_rw(v) += width;
}

static node_t*
plain_vnode(graph_t * g, edge_t * orig)
{
    node_t *v;
    orig = orig;
    v = virtual_node(g);
    incr_width(g, v);
    return v;
}

static node_t*
leader_of(graph_t * g, node_t * v)
{
    graph_t *clust;
    node_t *rv;

    if (ND_ranktype(v) != CLUSTER) {
	/*assert(v == UF_find(v));  could be leaf, so comment out */
	rv = UF_find(v);
    } else {
	clust = ND_clust(v);
	rv = GD_rankleader(clust)[ND_rank(v)];
    }
    return rv;
}

/* make_chain:
 * Create chain of dummy nodes for edge orig.
 */
static void 
make_chain(graph_t * g, node_t * from, node_t * to, edge_t * orig)
{
    int r, label_rank;
    node_t *u, *v;
    edge_t *e;

    u = from;
    if (ED_label(orig))
	label_rank = (ND_rank(from) + ND_rank(to)) / 2;
    else
	label_rank = -1;
    assert(ED_to_virt(orig) == NULL);
    for (r = ND_rank(from) + 1; r <= ND_rank(to); r++) {
	if (r < ND_rank(to)) {
	    if (r == label_rank)
		v = label_vnode(g, orig);
	    else
		v = plain_vnode(g, orig);
	    ND_rank(v) = r;
	} else
	    v = to;
	e = virtual_edge(u, v, orig);
	virtual_weight(e);
	u = v;
    }
    assert(ED_to_virt(orig) != NULL);
}

static void 
interclrep(graph_t * g, edge_t * e)
{
    node_t *t, *h;
    edge_t *ve;

    t = leader_of(g, e->tail);
    h = leader_of(g, e->head);
    if (ND_rank(t) > ND_rank(h)) {
	node_t *t0 = t;
	t = h;
	h = t0;
    }
    if (ND_clust(t) != ND_clust(h)) {
	if ((ve = find_fast_edge(t, h))) {
	    merge_chain(g, e, ve, TRUE);
	    return;
	}
	if (ND_rank(t) == ND_rank(h))
	    return;
	make_chain(g, t, h, e);

	/* mark as cluster edge */
	for (ve = ED_to_virt(e); ve && (ND_rank(ve->head) <= ND_rank(h));
	     ve = ND_out(ve->head).list[0])
	    ED_edge_type(ve) = CLUSTER_EDGE;
    }
    /* else ignore intra-cluster edges at this point */
}

static int 
is_cluster_edge(edge_t * e)
{
    return ((ND_ranktype(e->tail) == CLUSTER)
	    || (ND_ranktype(e->head) == CLUSTER));
}

void merge_chain(graph_t * g, edge_t * e, edge_t * f, int flag)
{
    edge_t *rep;
    int lastrank = MAX(ND_rank(e->tail), ND_rank(e->head));

    assert(ED_to_virt(e) == NULL);
    ED_to_virt(e) = f;
    rep = f;
    do {
	/* interclust multi-edges are not counted now */
	if (flag)
	    ED_count(rep) += ED_count(e);
	ED_xpenalty(rep) += ED_xpenalty(e);
	ED_weight(rep) += ED_weight(e);
	if (ND_rank(rep->head) == lastrank)
	    break;
	incr_width(g, rep->head);
	rep = ND_out(rep->head).list[0];
    } while (rep);
}

int mergeable(edge_t * e, edge_t * f)
{
    if (e && f && (e->tail == f->tail) && (e->head == f->head) &&
	(ED_label(e) == ED_label(f)) && ports_eq(e, f))
	return TRUE;
    return FALSE;
}

void class2(graph_t * g)
{
    int c;
    node_t *n, *t, *h;
    edge_t *e, *prev, *opp;

    GD_nlist(g) = NULL;

    GD_n_nodes(g) = 0;		/* new */

    mark_clusters(g);
    for (c = 1; c <= GD_n_cluster(g); c++)
	build_skeleton(g, GD_clust(g)[c]);
    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (ND_weight_class(e->head) <= 2)
		ND_weight_class(e->head)++;
	    if (ND_weight_class(e->tail) <= 2)
		ND_weight_class(e->tail)++;
	}

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if ((ND_clust(n) == NULL) && (n == UF_find(n))) {
	    fast_node(g, n);
	    GD_n_nodes(g)++;
	}
	prev = NULL;
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {

	    /* already processed */
	    if (ED_to_virt(e)) {
		prev = e;
		continue;
	    }

	    /* edges involving sub-clusters of g */
	    if (is_cluster_edge(e)) {
		/* following is new cluster multi-edge code */
		if (mergeable(prev, e)) {
		    if (ED_to_virt(prev)) {
			merge_chain(g, e, ED_to_virt(prev), FALSE);
			other_edge(e);
		    } else if (ND_rank(e->tail) == ND_rank(e->head)) {
			merge_oneway(e, prev);
			other_edge(e);
		    }
		    /* else is an intra-cluster edge */
		    continue;
		}
		interclrep(g, e);
		prev = e;
		continue;
	    }
	    /* merge multi-edges */
	    if (prev && (e->tail == prev->tail) && (e->head == prev->head)) {
		if (ND_rank(e->tail) == ND_rank(e->head)) {
		    merge_oneway(e, prev);
		    other_edge(e);
		    continue;
		}
		if ((ED_label(e) == NULL) && (ED_label(prev) == NULL)
		    && ports_eq(e, prev)) {
		    if (Concentrate)
			ED_edge_type(e) = IGNORED;
		    else {
			merge_chain(g, e, ED_to_virt(prev), TRUE);
			other_edge(e);
		    }
		    continue;
		}
		/* parallel edges with different labels fall through here */
	    }

	    /* self edges */
	    if (e->tail == e->head) {
		other_edge(e);
		prev = e;
		continue;
	    }

	    t = UF_find(e->tail);
	    h = UF_find(e->head);

	    /* non-leader leaf nodes */
	    if ((e->tail != t) || (e->head != h)) {
		/* FIX need to merge stuff */
		continue;
	    }


	    /* flat edges */
	    if (ND_rank(e->tail) == ND_rank(e->head)) {
		flat_edge(g, e);
		prev = e;
		continue;
	    }

	    /* forward edges */
	    if (ND_rank(e->head) > ND_rank(e->tail)) {
		make_chain(g, e->tail, e->head, e);
		prev = e;
		continue;
	    }

	    /* backward edges */
	    else {
		/*other_edge(e); */
		/* avoid when opp==e in undirected graph */
		if ((opp = agfindedge(g, e->head, e->tail)) && (opp != e)) {
		    /* shadows a forward edge */
		    if (ED_to_virt(opp) == NULL)
			make_chain(g, opp->tail, opp->head, opp);
		    if ((ED_label(e) == NULL) && (ED_label(opp) == NULL)
			&& ports_eq(e, opp)) {
			if (Concentrate) {
			    ED_edge_type(e) = IGNORED;
			    ED_conc_opp_flag(opp) = TRUE;
			} else {	/* see above.  this is getting out of hand */
			    other_edge(e);
			    merge_chain(g, e, ED_to_virt(opp), TRUE);
			}
			continue;
		    }
		}
		make_chain(g, e->head, e->tail, e);
		prev = e;
	    }
	}
    }
    /* since decompose() is not called on subgraphs */
    if (g != g->root) {
	GD_comp(g).list = ALLOC(1, GD_comp(g).list, node_t *);
	GD_comp(g).list[0] = GD_nlist(g);
    }
}

