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
 * Break cycles in a directed graph by depth-first search.
 */

#include "dot.h"

void reverse_edge(edge_t * e)
{
    edge_t *f;

    delete_fast_edge(e);
    if ((f = find_fast_edge(e->head, e->tail)))
	merge_oneway(e, f);
    else
	virtual_edge(e->head, e->tail, e);
}

static void 
dfs(node_t * n)
{
    int i;
    edge_t *e;
    node_t *w;

    if (ND_mark(n))
	return;
    ND_mark(n) = TRUE;
    ND_onstack(n) = TRUE;
    for (i = 0; (e = ND_out(n).list[i]); i++) {
	w = e->head;
	if (ND_onstack(w)) {
	    reverse_edge(e);
	    i--;
	} else {
	    if (ND_mark(w) == FALSE)
		dfs(w);
	}
    }
    ND_onstack(n) = FALSE;
}


void acyclic(graph_t * g)
{
    int c;
    node_t *n;

    for (c = 0; c < GD_comp(g).size; c++) {
	GD_nlist(g) = GD_comp(g).list[c];
	for (n = GD_nlist(g); n; n = ND_next(n))
	    ND_mark(n) = FALSE;
	for (n = GD_nlist(g); n; n = ND_next(n))
	    dfs(n);
    }
}

