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


/*
 * operations on the fast internal graph.
 */

static edge_t *ffe(node_t * u, elist uL, node_t * v, elist vL)
{
    int i;
    edge_t *e;

    if ((uL.size > 0) && (vL.size > 0)) {
	if (uL.size < vL.size) {
	    for (i = 0; (e = uL.list[i]); i++)
		if (e->head == v)
		    break;
	} else {
	    for (i = 0; (e = vL.list[i]); i++)
		if (e->tail == u)
		    break;
	}
    } else
	e = 0;
    return e;
}

edge_t *find_fast_edge(node_t * u, node_t * v)
{
    return ffe(u, ND_out(u), v, ND_in(v));
}

static node_t*
find_fast_node(graph_t * g, node_t * n)
{
    node_t *v;
    for (v = GD_nlist(g); v; v = ND_next(v))
	if (v == n)
	    break;
    return v;
}

edge_t *find_flat_edge(node_t * u, node_t * v)
{
    return ffe(u, ND_flat_out(u), v, ND_flat_in(v));
}

/* safe_list_append - append e to list L only if e not already a member */
static void 
safe_list_append(edge_t * e, elist * L)
{
    int i;

    for (i = 0; i < L->size; i++)
	if (e == L->list[i])
	    return;
    elist_append(e, (*L));
}

edge_t *fast_edge(edge_t * e)
{
#ifdef DEBUG
    int i;
    edge_t *f;
    for (i = 0; (f = ND_out(e->tail).list[i]); i++) {
	if (e == f) {
	    fprintf(stderr, "duplicate fast edge\n");
	    return;
	}
	assert(e->head != f->head);
    }
    for (i = 0; (f = ND_in(e->head).list[i]); i++) {
	if (e == f) {
	    fprintf(stderr, "duplicate fast edge\n");
	    return;
	}
	assert(e->tail != f->tail);
    }
#endif
    elist_append(e, ND_out(e->tail));
    elist_append(e, ND_in(e->head));
    return e;
}

/* zapinlist - remove e from list and fill hole with last member of list */
void zapinlist(elist * L, edge_t * e)
{
    int i;

    for (i = 0; i < L->size; i++) {
	if (L->list[i] == e) {
	    L->size--;
	    L->list[i] = L->list[L->size];
	    L->list[L->size] = NULL;
	    break;
	}
    }
}

/* disconnects e from graph */
void delete_fast_edge(edge_t * e)
{
    assert(e != NULL);
    zapinlist(&(ND_out(e->tail)), e);
    zapinlist(&(ND_in(e->head)), e);
}

static void 
safe_delete_fast_edge(edge_t * e)
{
    int i;
    edge_t *f;

    assert(e != NULL);
    for (i = 0; (f = ND_out(e->tail).list[i]); i++)
	if (f == e)
	    zapinlist(&(ND_out(e->tail)), e);
    for (i = 0; (f = ND_in(e->head).list[i]); i++)
	if (f == e)
	    zapinlist(&(ND_in(e->head)), e);
}

void other_edge(edge_t * e)
{
    elist_append(e, ND_other(e->tail));
}

void safe_other_edge(edge_t * e)
{
    safe_list_append(e, &(ND_other(e->tail)));
}

#ifdef OBSOLETET
static void 
delete_other_edge(edge_t * e)
{
    assert(e != NULL);
    zapinlist(&(ND_other(e->tail)), e);
}
#endif

/* orig might be an input edge, reverse of an input edge, or virtual edge */
edge_t *new_virtual_edge(node_t * u, node_t * v, edge_t * orig)
{
    edge_t *e;

    e = NEW(edge_t);
    e->tail = u;
    e->head = v;
    ED_edge_type(e) = VIRTUAL;

    if (orig) {
	e->id = orig->id;
	ED_count(e) = ED_count(orig);
	ED_xpenalty(e) = ED_xpenalty(orig);
	ED_weight(e) = ED_weight(orig);
	ED_minlen(e) = ED_minlen(orig);
	if (e->tail == orig->tail)
	    ED_tail_port(e) = ED_tail_port(orig);
	else if (e->tail == orig->head)
	    ED_tail_port(e) = ED_head_port(orig);
	if (e->head == orig->head)
	    ED_head_port(e) = ED_head_port(orig);
	else if (e->head == orig->tail)
	    ED_head_port(e) = ED_tail_port(orig);

	if (ED_to_virt(orig) == NULL)
	    ED_to_virt(orig) = e;
	ED_to_orig(e) = orig;
    } else
	ED_minlen(e) = ED_count(e) = ED_xpenalty(e) = ED_weight(e) = 1;
    return e;
}

edge_t *virtual_edge(node_t * u, node_t * v, edge_t * orig)
{
    return fast_edge(new_virtual_edge(u, v, orig));
}

void fast_node(graph_t * g, Agnode_t * n)
{

#ifdef DEBUG
    assert(find_fast_node(g, n) == NULL);
#endif
    ND_next(n) = GD_nlist(g);
    if (ND_next(n))
	ND_next(n)->u.prev = n;
    GD_nlist(g) = n;
    ND_prev(n) = NULL;
    assert(n != ND_next(n));
}

void fast_nodeapp(node_t * u, node_t * v)
{
    assert(u != v);
    assert(ND_next(v) == NULL);
    ND_next(v) = ND_next(u);
    if (ND_next(u))
	ND_next(u)->u.prev = v;
    ND_prev(v) = u;
    ND_next(u) = v;
}

void delete_fast_node(graph_t * g, node_t * n)
{
    assert(find_fast_node(g, n));
    if (ND_next(n))
	ND_next(n)->u.prev = ND_prev(n);
    if (ND_prev(n))
	ND_prev(n)->u.next = ND_next(n);
    else
	GD_nlist(g) = ND_next(n);
}

node_t *virtual_node(graph_t * g)
{
    node_t *n;

    n = NEW(node_t);
    n->name = "virtual";
    n->graph = g;
    ND_node_type(n) = VIRTUAL;
    ND_lw_i(n) = ND_rw_i(n) = 1;
    ND_ht_i(n) = 1;
    ND_UF_size(n) = 1;
    alloc_elist(4, ND_in(n));
    alloc_elist(4, ND_out(n));
    fast_node(g, n);
    GD_n_nodes(g)++;
    return n;
}

void flat_edge(graph_t * g, edge_t * e)
{
    elist_append(e, ND_flat_out(e->tail));
    elist_append(e, ND_flat_in(e->head));
    GD_has_flat_edges(g->root) = GD_has_flat_edges(g) = TRUE;
}

void delete_flat_edge(edge_t * e)
{
    assert(e != NULL);
    if (ED_to_orig(e) && ED_to_virt(ED_to_orig(e)) == e)
	ED_to_virt(ED_to_orig(e)) = NULL;
    zapinlist(&(ND_flat_out(e->tail)), e);
    zapinlist(&(ND_flat_in(e->head)), e);
}

#ifdef DEBUG
static char *NAME(node_t * n)
{
    static char buf[20];
    if (ND_node_type(n) == NORMAL)
	return n->name;
    sprintf(buf, "V%x", n);
    return buf;
}

void fastgr(graph_t * g)
{
    int i, j;
    node_t *n, *w;
    edge_t *e, *f;

    for (n = GD_nlist(g); n; n = ND_next(n)) {
	fprintf(stderr, "%s %d: (", NAME(n), ND_rank(n));
	for (i = 0; e = ND_out(n).list[i]; i++) {
	    fprintf(stderr, " %s:%d", NAME(e->head), ED_count(e));
	    w = e->head;
	    if (g == g->root) {
		for (j = 0; f = ND_in(w).list[j]; j++)
		    if (e == f)
			break;
		assert(f != NULL);
	    }
	}
	fprintf(stderr, " ) (");
	for (i = 0; e = ND_in(n).list[i]; i++) {
	    fprintf(stderr, " %s:%d", NAME(e->tail), ED_count(e));
	    w = e->tail;
	    if (g == g->root) {
		for (j = 0; f = ND_out(w).list[j]; j++)
		    if (e == f)
			break;
		assert(f != NULL);
	    }
	}
	fprintf(stderr, " )\n");
    }
}
#endif

static void 
basic_merge(edge_t * e, edge_t * rep)
{
    if (ED_minlen(rep) < ED_minlen(e))
	ED_minlen(rep) = ED_minlen(e);
    while (rep) {
	ED_count(rep) += ED_count(e);
	ED_xpenalty(rep) += ED_xpenalty(e);
	ED_weight(rep) += ED_weight(e);
	rep = ED_to_virt(rep);
    }
}

void 
merge_oneway(edge_t * e, edge_t * rep)
{
    if (rep == ED_to_virt(e)) {
	agerr(AGWARN, "merge_oneway glitch\n");
	return;
    }
    assert(ED_to_virt(e) == NULL);
    ED_to_virt(e) = rep;
    basic_merge(e, rep);
}

static void 
unrep(edge_t * rep, edge_t * e)
{
    ED_count(rep) -= ED_count(e);
    ED_xpenalty(rep) -= ED_xpenalty(e);
    ED_weight(rep) -= ED_weight(e);
}

void unmerge_oneway(edge_t * e)
{
    edge_t *rep, *nextrep;
    for (rep = ED_to_virt(e); rep; rep = nextrep) {
	unrep(rep, e);
	nextrep = ED_to_virt(rep);
	if (ED_count(rep) == 0)
	    safe_delete_fast_edge(rep);	/* free(rep)? */

	/* unmerge from a virtual edge chain */
	while ((ED_edge_type(rep) == VIRTUAL)
	       && (ND_node_type(rep->head) == VIRTUAL)
	       && (ND_out(rep->head).size == 1)) {
	    rep = ND_out(rep->head).list[0];
	    unrep(rep, e);
	}
    }
    ED_to_virt(e) = NULL;
}

#ifdef OBSOLETET
static int 
is_fast_node(graph_t * g, node_t * v)
{
    node_t *n;

    for (n = GD_nlist(g); n; n = ND_next(n))
	if (v == n)
	    return TRUE;
    return FALSE;
}
#endif
