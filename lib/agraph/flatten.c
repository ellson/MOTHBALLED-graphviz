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


#include "aghdr.h"

void agflatten_elist(Dict_t * d, Dtlink_t ** lptr)
{
    dtrestore(d, *lptr);
    (void) dtflatten(d);
    *lptr = dtextract(d);
}

void agflatten_edges(Agraph_t * g, Agnode_t * n)
{
    Agedge_t **tmp;

    tmp = &(n->out); /* avoiding - "dereferencing type-punned pointer will break strict-aliasing rules" */

    agflatten_elist(g->e_seq, (Dtlink_t **) tmp);
    tmp = &(n->in);
    agflatten_elist(g->e_seq, (Dtlink_t **) tmp);
}

void agflatten(Agraph_t * g, int flag)
{
    Agnode_t *n;

    if (flag) {
	if (g->desc.flatlock == FALSE) {
	    dtflatten(g->n_seq);
	    g->desc.flatlock = TRUE;
	    for (n = agfstnode(g); n; n = agnxtnode(n))
		agflatten_edges(g, n);
	}
    } else {
	if (g->desc.flatlock) {
	    g->desc.flatlock = FALSE;
	}
    }
}

void agnotflat(Agraph_t * g)
{
    if (g->desc.flatlock)
	agerror(AGERROR_FLAT, "");
}
