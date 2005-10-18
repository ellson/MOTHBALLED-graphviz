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
 *	build edge_t concentrators for parallel edges with a common endpoint
 */

#include	"dot.h"


#define		UP		0
#define		DOWN	1

static bool samedir(edge_t * e, edge_t * f)
{
    edge_t *e0, *f0;

    for (e0 = e; ED_edge_type(e0) != NORMAL; e0 = ED_to_orig(e0));
    for (f0 = f; ED_edge_type(f0) != NORMAL; f0 = ED_to_orig(f0));
    if (ED_conc_opp_flag(e0))
	return FALSE;
    if (ED_conc_opp_flag(f0))
	return FALSE;
    return ((ND_rank(f0->tail) - ND_rank(f0->head))
	    * (ND_rank(e0->tail) - ND_rank(e0->head)) > 0);
}

static bool downcandidate(node_t * v)
{
    return ((ND_node_type(v) == VIRTUAL) && (ND_in(v).size == 1)
	    && (ND_out(v).size == 1) && (ND_label(v) == NULL));
}

static bool bothdowncandidates(node_t * u, node_t * v)
{
    edge_t *e, *f;
    e = ND_in(u).list[0];
    f = ND_in(v).list[0];
    if (downcandidate(v) && (e->tail == f->tail)) {
	return samedir(e, f)
	    && (portcmp(ED_tail_port(e), ED_tail_port(f)) == 0);
    }
    return FALSE;
}

static bool upcandidate(node_t * v)
{
    return ((ND_node_type(v) == VIRTUAL) && (ND_out(v).size == 1)
	    && (ND_in(v).size == 1) && (ND_label(v) == NULL));
}

static bool bothupcandidates(node_t * u, node_t * v)
{
    edge_t *e, *f;
    e = ND_out(u).list[0];
    f = ND_out(v).list[0];
    if (upcandidate(v) && (e->head == f->head)) {
	return samedir(e, f)
	    && (portcmp(ED_head_port(e), ED_head_port(f)) == 0);
    }
    return FALSE;
}

static void mergevirtual(graph_t * g, int r, int lpos, int rpos, int dir)
{
    int i, k;
    node_t *left, *right;
    edge_t *e, *f, *e0;

    left = GD_rank(g)[r].v[lpos];
    /* merge all right nodes into the leftmost one */
    for (i = lpos + 1; i <= rpos; i++) {
	right = GD_rank(g)[r].v[i];
	if (dir == DOWN) {
	    while ((e = ND_out(right).list[0])) {
		for (k = 0; (f = ND_out(left).list[k]); k++)
		    if (f->head == e->head)
			break;
		if (f == NULL)
		    f = virtual_edge(left, e->head, e);
		while ((e0 = ND_in(right).list[0])) {
		    merge_oneway(e0, f);
		    /*ED_weight(f) += ED_weight(e0); */
		    delete_fast_edge(e0);
		}
		delete_fast_edge(e);
	    }
	} else {
	    while ((e = ND_in(right).list[0])) {
		for (k = 0; (f = ND_in(left).list[k]); k++)
		    if (f->tail == e->tail)
			break;
		if (f == NULL)
		    f = virtual_edge(e->tail, left, e);
		while ((e0 = ND_out(right).list[0])) {
		    merge_oneway(e0, f);
		    delete_fast_edge(e0);
		}
		delete_fast_edge(e);
	    }
	}
	assert(ND_in(right).size + ND_out(right).size == 0);
	delete_fast_node(g, right);
    }
    k = lpos + 1;
    i = rpos + 1;
    while (i < GD_rank(g)[r].n) {
	node_t *n;
	n = GD_rank(g)[r].v[k] = GD_rank(g)[r].v[i];
	ND_order(n) = k;
	k++;
	i++;
    }
    GD_rank(g)[r].n = k;
    GD_rank(g)[r].v[k] = NULL;
}

static void infuse(graph_t * g, node_t * n)
{
    node_t *lead;

    lead = GD_rankleader(g)[ND_rank(n)];
    if ((lead == NULL) || (ND_order(lead) > ND_order(n)))
	GD_rankleader(g)[ND_rank(n)] = n;
}

static void rebuild_vlists(graph_t * g)
{
    int c, i, r, maxi;
    node_t *n, *lead;
    edge_t *e, *rep;

    for (r = GD_minrank(g); r <= GD_maxrank(g); r++)
	GD_rankleader(g)[r] = NULL;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	infuse(g, n);
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    for (rep = e; ED_to_virt(rep); rep = ED_to_virt(rep));
	    while (ND_rank(rep->head) < ND_rank(e->head)) {
		infuse(g, rep->head);
		rep = ND_out(rep->head).list[0];
	    }
	}
    }

    for (r = GD_minrank(g); r <= GD_maxrank(g); r++) {
	lead = GD_rankleader(g)[r];
	if (ND_rank(g->root)[r].v[ND_order(lead)] != lead)
	    abort();
	GD_rank(g)[r].v =
	    ND_rank(g->root)[r].v + GD_rankleader(g)[r]->u.order;
	maxi = -1;
	for (i = 0; i < GD_rank(g)[r].n; i++) {
	    if ((n = GD_rank(g)[r].v[i]) == NULL)
		break;
	    if (ND_node_type(n) == NORMAL) {
		if (agcontains(g, n))
		    maxi = i;
		else
		    break;
	    } else {
		edge_t *e;
		for (e = ND_in(n).list[0]; e && ED_to_orig(e);
		     e = ED_to_orig(e));
		if (e && (agcontains(g, e->tail))
		    && agcontains(g, e->head))
		    maxi = i;
	    }
	}
	if (maxi == -1)
	    agerr(AGWARN, "degenerate concentrated rank %s,%d\n", g->name,
		  r);
	GD_rank(g)[r].n = maxi + 1;
    }

    for (c = 1; c <= GD_n_cluster(g); c++)
	rebuild_vlists(GD_clust(g)[c]);
}

void dot_concentrate(graph_t * g)
{
    int c, r, leftpos, rightpos;
    node_t *left, *right;

    if (GD_maxrank(g) - GD_minrank(g) <= 1)
	return;
    /* this is the downward looking pass. r is a candidate rank. */
    for (r = 1; GD_rank(g)[r + 1].n; r++) {
	for (leftpos = 0; leftpos < GD_rank(g)[r].n; leftpos++) {
	    left = GD_rank(g)[r].v[leftpos];
	    if (downcandidate(left) == FALSE)
		continue;
	    for (rightpos = leftpos + 1; rightpos < GD_rank(g)[r].n;
		 rightpos++) {
		right = GD_rank(g)[r].v[rightpos];
		if (bothdowncandidates(left, right) == FALSE)
		    break;
	    }
	    if (rightpos - leftpos > 1)
		mergevirtual(g, r, leftpos, rightpos - 1, DOWN);
	}
    }
    /* this is the corresponding upward pass */
    while (r > 0) {
	for (leftpos = 0; leftpos < GD_rank(g)[r].n; leftpos++) {
	    left = GD_rank(g)[r].v[leftpos];
	    if (upcandidate(left) == FALSE)
		continue;
	    for (rightpos = leftpos + 1; rightpos < GD_rank(g)[r].n;
		 rightpos++) {
		right = GD_rank(g)[r].v[rightpos];
		if (bothupcandidates(left, right) == FALSE)
		    break;
	    }
	    if (rightpos - leftpos > 1)
		mergevirtual(g, r, leftpos, rightpos - 1, UP);
	}
	r--;
    }
    for (c = 1; c <= GD_n_cluster(g); c++)
	rebuild_vlists(GD_clust(g)[c]);
}
