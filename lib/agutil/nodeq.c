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
