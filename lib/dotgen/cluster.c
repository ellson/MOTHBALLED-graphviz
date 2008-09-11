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


#include "dot.h"

static node_t*
map_interclust_node(node_t * n)
{
    node_t *rv;

    if ((ND_clust(n) == NULL) || (ND_clust(n)->u.expanded))
	rv = n;
    else
	rv = ND_clust(n)->u.rankleader[ND_rank(n)];
    return rv;
}

/* make d slots starting at position pos (where 1 already exists) */
static void 
make_slots(graph_t * root, int r, int pos, int d)
{
    int i;
    node_t *v, **vlist;
    vlist = ND_rank(root)[r].v;
    if (d <= 0) {
	for (i = pos - d + 1; i < ND_rank(root)[r].n; i++) {
	    v = vlist[i];
	    ND_order(v) = i + d - 1;
	    vlist[ND_order(v)] = v;
	}
	for (i = ND_rank(root)[r].n + d - 1; i < ND_rank(root)[r].n; i++)
	    vlist[i] = NULL;
    } else {
/*assert(ND_rank(root)[r].n + d - 1 <= ND_rank(root)[r].an);*/
	for (i = ND_rank(root)[r].n - 1; i > pos; i--) {
	    v = vlist[i];
	    ND_order(v) = i + d - 1;
	    vlist[ND_order(v)] = v;
	}
	for (i = pos + 1; i < pos + d; i++)
	    vlist[i] = NULL;
    }
    ND_rank(root)[r].n += d - 1;
}

static node_t* 
clone_vn(graph_t * g, node_t * vn)
{
    node_t *rv;
    int r;

    r = ND_rank(vn);
    make_slots(g, r, ND_order(vn), 2);
    rv = virtual_node(g);
    ND_lw(rv) = ND_lw(vn);
    ND_rw(rv) = ND_rw(vn);
    ND_rank(rv) = ND_rank(vn);
    ND_order(rv) = ND_order(vn) + 1;
    GD_rank(g)[r].v[ND_order(rv)] = rv;
    return rv;
}

static void 
map_path(node_t * from, node_t * to, edge_t * orig, edge_t * ve, int type)
{
    int r;
    node_t *u, *v;
    edge_t *e;

    assert(ND_rank(from) < ND_rank(to));

    if ((ve->tail == from) && (ve->head == to))
	return;

    if (ED_count(ve) > 1) {
	ED_to_virt(orig) = NULL;
	if (ND_rank(to) - ND_rank(from) == 1) {
	    if ((e = find_fast_edge(from, to)) && (ports_eq(orig, e))) {
		merge_oneway(orig, e);
		if ((ND_node_type(from) == NORMAL)
		    && (ND_node_type(to) == NORMAL))
		    other_edge(orig);
		return;
	    }
	}
	u = from;
	for (r = ND_rank(from); r < ND_rank(to); r++) {
	    if (r < ND_rank(to) - 1)
		v = clone_vn(from->graph, ve->head);
	    else
		v = to;
	    e = virtual_edge(u, v, orig);
	    ED_edge_type(e) = type;
	    u = v;
	    ED_count(ve)--;
	    ve = ND_out(ve->head).list[0];
	}
    } else {
	if (ND_rank(to) - ND_rank(from) == 1) {
	    if ((ve = find_fast_edge(from, to)) && (ports_eq(orig, ve))) {
		/*ED_to_orig(ve) = orig; */
		ED_to_virt(orig) = ve;
		ED_edge_type(ve) = type;
		ED_count(ve)++;
		if ((ND_node_type(from) == NORMAL)
		    && (ND_node_type(to) == NORMAL))
		    other_edge(orig);
	    } else {
		ED_to_virt(orig) = NULL;
		ve = virtual_edge(from, to, orig);
		ED_edge_type(ve) = type;
	    }
	}
	if (ND_rank(to) - ND_rank(from) > 1) {
	    e = ve;
	    if (ve->tail != from) {
		ED_to_virt(orig) = NULL;
		e = ED_to_virt(orig) = virtual_edge(from, ve->head, orig);
		delete_fast_edge(ve);
	    } else
		e = ve;
	    while (ND_rank(e->head) != ND_rank(to))
		e = ND_out(e->head).list[0];
	    if (e->head != to) {
		ve = e;
		e = virtual_edge(e->tail, to, orig);
		ED_edge_type(e) = type;
		delete_fast_edge(ve);
	    }
	}
    }
}

static void 
make_interclust_chain(graph_t * g, node_t * from, node_t * to, edge_t * orig)
{
    int newtype;
    node_t *u, *v;

    u = map_interclust_node(from);
    v = map_interclust_node(to);
    if ((u == from) && (v == to))
	newtype = VIRTUAL;
    else
	newtype = CLUSTER_EDGE;
    map_path(u, v, orig, ED_to_virt(orig), newtype);
}

/* 
 * attach and install edges between clusters.
 * essentially, class2() for interclust edges.
 */
void interclexp(graph_t * subg)
{
    graph_t *g;
    node_t *n;
    edge_t *e, *prev;

    g = subg->root;
    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {

	/* N.B. n may be in a sub-cluster of subg */
	prev = NULL;
	for (e = agfstedge(subg->root, n); e;
	     e = agnxtedge(subg->root, e, n)) {
	    if (agcontains(subg, e))
		continue;

	    /* short/flat multi edges */
	    if (mergeable(prev, e)) {
		if (ND_rank(e->tail) == ND_rank(e->head))
		    ED_to_virt(e) = prev;
		else
		    ED_to_virt(e) = NULL;
		if (ED_to_virt(prev) == NULL)
		    continue;	/* internal edge */
		merge_chain(subg, e, ED_to_virt(prev), FALSE);
		safe_other_edge(e);
		continue;
	    }

	    /* flat edges */
	    if (ND_rank(e->tail) == ND_rank(e->head)) {
		edge_t* fe;
		if ((fe = find_flat_edge(e->tail, e->head)) == NULL) {
		    flat_edge(g, e);
		    prev = e;
		} else if (e != fe) {
		    safe_other_edge(e);
		    if (!ED_to_virt(e)) merge_oneway(e, fe);
		}
		continue;
	    }

	    assert(ED_to_virt(e) != NULL);

	    /* forward edges */
	    if (ND_rank(e->head) > ND_rank(e->tail)) {
		make_interclust_chain(g, e->tail, e->head, e);
		prev = e;
		continue;
	    }

	    /* backward edges */
	    else {
/*
I think that make_interclust_chain should create call other_edge(e) anyway 
				if (agcontains(subg,e->tail)
					&& agfindedge(subg->root,e->head,e->tail)) other_edge(e);
*/
		make_interclust_chain(g, e->head, e->tail, e);
		prev = e;
	    }
	}
    }
}

static void 
merge_ranks(graph_t * subg)
{
    int i, d, r, pos, ipos;
    node_t *v;
    graph_t *root;

    root = subg->root;
    if (GD_minrank(subg) > 0)
	ND_rank(root)[GD_minrank(subg) - 1].valid = FALSE;
    for (r = GD_minrank(subg); r <= GD_maxrank(subg); r++) {
	d = GD_rank(subg)[r].n;
	ipos = pos = GD_rankleader(subg)[r]->u.order;
	make_slots(root, r, pos, d);
	for (i = 0; i < GD_rank(subg)[r].n; i++) {
	    v = ND_rank(root)[r].v[pos] = GD_rank(subg)[r].v[i];
	    ND_order(v) = pos++;
	    v->graph = subg->root;
	    delete_fast_node(subg, v);
	    fast_node(subg->root, v);
	    GD_n_nodes(subg->root)++;
	}
	GD_rank(subg)[r].v = ND_rank(root)[r].v + ipos;
	ND_rank(root)[r].valid = FALSE;
    }
    if (r < GD_maxrank(root))
	GD_rank(root)[r].valid = FALSE;
    GD_expanded(subg) = TRUE;
}

static void 
remove_rankleaders(graph_t * g)
{
    int r;
    node_t *v;
    edge_t *e;

    for (r = GD_minrank(g); r <= GD_maxrank(g); r++) {
	v = GD_rankleader(g)[r];

	/* remove the entire chain */
	while ((e = ND_out(v).list[0]))
	    delete_fast_edge(e);
	while ((e = ND_in(v).list[0]))
	    delete_fast_edge(e);
	delete_fast_node(g->root, v);
	GD_rankleader(g)[r] = NULL;
    }
}

/* delete virtual nodes of a cluster, and install real nodes or sub-clusters */
void expand_cluster(graph_t * subg)
{
    /* build internal structure of the cluster */
    class2(subg);
    GD_comp(subg).size = 1;
    GD_comp(subg).list[0] = GD_nlist(subg);
    allocate_ranks(subg);
    build_ranks(subg, 0);
    merge_ranks(subg);

    /* build external structure of the cluster */
    interclexp(subg);
    remove_rankleaders(subg);
}

/* this function marks every node in <g> with its top-level cluster under <g> */
void mark_clusters(graph_t * g)
{
    int c;
    node_t *n, *vn;
    edge_t *orig, *e;
    graph_t *clust;

    /* remove sub-clusters below this level */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_ranktype(n) == CLUSTER)
	    UF_singleton(n);
	ND_clust(n) = NULL;
    }

    for (c = 1; c <= GD_n_cluster(g); c++) {
	clust = GD_clust(g)[c];
	for (n = agfstnode(clust); n; n = agnxtnode(clust, n)) {
	    if (ND_ranktype(n) != NORMAL) {
		agerr(AGWARN,
		      "%s was already in a rankset, ignored in cluster %s\n",
		      n->name, g->name);
		continue;
	    }
	    UF_setname(n, GD_leader(clust));
	    ND_clust(n) = clust;
	    ND_ranktype(n) = CLUSTER;

	    /* here we mark the vnodes of edges in the cluster */
	    for (orig = agfstout(clust, n); orig;
		 orig = agnxtout(clust, orig)) {
		if ((e = ED_to_virt(orig))) {
		    while (e && (vn = e->head)->u.node_type == VIRTUAL) {
			ND_clust(vn) = clust;
			e = ND_out(e->head).list[0];
			/* trouble if concentrators and clusters are mixed */
		    }
		}
	    }
	}
    }
}

void build_skeleton(graph_t * g, graph_t * subg)
{
    int r;
    node_t *v, *prev, *rl;
    edge_t *e;

    prev = NULL;
    GD_rankleader(subg) = N_NEW(GD_maxrank(subg) + 2, node_t *);
    for (r = GD_minrank(subg); r <= GD_maxrank(subg); r++) {
	v = GD_rankleader(subg)[r] = virtual_node(g);
	ND_rank(v) = r;
	ND_ranktype(v) = CLUSTER;
	ND_clust(v) = subg;
	if (prev) {
	    e = virtual_edge(prev, v, NULL);
	    ED_xpenalty(e) *= CL_CROSS;
	}
	prev = v;
    }

    /* set the counts on virtual edges of the cluster skeleton */
    for (v = agfstnode(subg); v; v = agnxtnode(subg, v)) {
	rl = GD_rankleader(subg)[ND_rank(v)];
	ND_UF_size(rl)++;
	for (e = agfstout(subg, v); e; e = agnxtout(subg, e)) {
	    for (r = ND_rank(e->tail); r < ND_rank(e->head); r++) {
		ED_count(ND_out(rl).list[0])++;
	    }
	}
    }
    for (r = GD_minrank(subg); r <= GD_maxrank(subg); r++) {
	rl = GD_rankleader(subg)[r];
	if (ND_UF_size(rl) > 1)
	    ND_UF_size(rl)--;
    }
}

void install_cluster(graph_t * g, node_t * n, int pass, nodequeue * q)
{
    int r;
    graph_t *clust;

    clust = ND_clust(n);
    if (GD_installed(clust) != pass + 1) {
	for (r = GD_minrank(clust); r <= GD_maxrank(clust); r++)
	    install_in_rank(g, GD_rankleader(clust)[r]);
	for (r = GD_minrank(clust); r <= GD_maxrank(clust); r++)
	    enqueue_neighbors(q, GD_rankleader(clust)[r], pass);
	GD_installed(clust) = pass + 1;
    }
}

static void mark_lowcluster_basic(Agraph_t * g);
void mark_lowclusters(Agraph_t * root)
{
    Agnode_t *n, *vn;
    Agedge_t *orig, *e;

    /* first, zap any previous cluster labelings */
    for (n = agfstnode(root); n; n = agnxtnode(root, n)) {
	ND_clust(n) = NULL;
	for (orig = agfstout(root, n); orig; orig = agnxtout(root, orig)) {
	    if ((e = ED_to_virt(orig))) {
		while (e && (vn = e->head)->u.node_type == VIRTUAL) {
		    ND_clust(vn) = NULL;
		    e = ND_out(e->head).list[0];
		}
	    }
	}
    }

    /* do the recursion */
    mark_lowcluster_basic(root);
}

static void mark_lowcluster_basic(Agraph_t * g)
{
    Agraph_t *clust;
    Agnode_t *n, *vn;
    Agedge_t *orig, *e;
    int c;

    for (c = 1; c <= GD_n_cluster(g); c++) {
	clust = GD_clust(g)[c];
	mark_lowcluster_basic(clust);
    }
    /* see what belongs to this graph that wasn't already marked */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_clust(n) == NULL)
	    ND_clust(n) = g;
	for (orig = agfstout(g, n); orig; orig = agnxtout(g, orig)) {
	    if ((e = ED_to_virt(orig))) {
		while (e && (vn = e->head)->u.node_type == VIRTUAL) {
		    if (ND_clust(vn) == NULL)
			ND_clust(vn) = g;
		    e = ND_out(e->head).list[0];
		}
	    }
	}
    }
}
