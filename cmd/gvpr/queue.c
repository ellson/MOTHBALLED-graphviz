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
 * Queue implementation using cdt
 *
 */

#include <queue.h>
#include <ast.h>

typedef struct {
    Dtlink_t link;
    void *np;
} nsitem;

static Void_t *makef(Dt_t * d, nsitem * obj, Dtdisc_t * disc)
{
    nsitem *p;

    p = oldof(0, nsitem, 1, 0);
    p->np = obj->np;
    return p;
}

static void freef(Dt_t * d, nsitem * obj, Dtdisc_t * disc)
{
    free(obj);
}

static Dtdisc_t ndisc = {
    offsetof(nsitem, np),
    sizeof(void *),
    offsetof(nsitem, link),
    (Dtmake_f) makef,
    (Dtfree_f) freef,
    0,
    0,
    0,
    0
};

queue *mkQ(Dtmethod_t * meth)
{
    queue *nq;

    nq = dtopen(&ndisc, meth);
    return nq;
}

void push(queue * nq, void *n)
{
    nsitem obj;

    obj.np = n;
    dtinsert(nq, &obj);
}

void *pop(queue * nq, int delete)
{
    nsitem *obj;
    void *n;

    obj = dtfirst(nq);
    if (obj) {
	n = obj->np;
	if (delete)
	    dtdelete(nq, 0);
	return n;
    } else
	return 0;
}

void freeQ(queue * nq)
{
    dtclose(nq);
}
