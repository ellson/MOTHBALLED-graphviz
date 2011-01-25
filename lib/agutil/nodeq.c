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

#include <agraph.h>
#include <agutil.h>

/* node Nqueue maintenance */

Nqueue *Nqueue_new(Agraph_t * g)
{
    Nqueue *q;
    int sz;

    q = agnew(g, Nqueue);
    sz = agnnodes(g);
    if (sz <= 1)
	sz = 2;
    q->head = q->tail = q->store = agnnew(g, sz, Agnode_t *);
    q->limit = q->store + sz;
    return q;
}

void Nqueue_free(Agraph_t * g, Nqueue * q)
{
    agfree(g, q->store);
    agfree(g, q);
}

void Nqueue_insert(Nqueue * q, Agnode_t * n)
{
    *(q->tail++) = n;
    if (q->tail >= q->limit)
	q->tail = q->store;
}

Agnode_t *Nqueue_remove(Nqueue * q)
{
    Agnode_t *n;
    if (q->head == q->tail)
	n = NIL(Agnode_t *);
    else {
	n = *(q->head++);
	if (q->head >= q->limit)
	    q->head = q->store;
    }
    return n;
}
