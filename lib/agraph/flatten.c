/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


#include "aghdr.h"

void agflatten_elist(Dict_t * d, Dtlink_t ** lptr)
{
    dtrestore(d, *lptr);
    (void) dtflatten(d);
    *lptr = dtextract(d);
}

void agflatten_edges(Agraph_t * g, Agnode_t * n)
{

    union {
    	Dtlink_t **dtlink;
    	Agedge_t **agedge;
	} out, in;

    out.agedge = & (n->out);
    in.agedge = & (n->in);
    agflatten_elist(g->e_seq, out.dtlink);
    agflatten_elist(g->e_seq, in.dtlink);

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
