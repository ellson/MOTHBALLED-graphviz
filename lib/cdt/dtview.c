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

#include	"dthdr.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/*	Set a view path from dict to view.
**
**	Written by Kiem-Phong Vo (5/25/96)
*/


#if __STD_C
static Void_t *dtvsearch(Dt_t * dt, reg Void_t * obj, reg int type)
#else
static Void_t *dtvsearch(dt, obj, type)
Dt_t *dt;
reg Void_t *obj;
reg int type;
#endif
{
    reg Dt_t *d, *p;
    reg Void_t *o;
    reg Dtdisc_t *disc;
    reg Dtlink_t *here;

    /* these operations only happen at the top level */
    if (type & (DT_INSERT | DT_DELETE | DT_CLEAR | DT_RENEW))
	return (*(dt->meth->searchf)) (dt, obj, type);

    if (!obj && !(type & (DT_FIRST | DT_LAST)))
	return NIL(Void_t *);

    if (type & (DT_MATCH | DT_SEARCH | DT_FIRST | DT_LAST)) {
	for (d = dt; d; d = d->view) {
	    if ((o = (*(d->meth->searchf)) (d, obj, type))) {
		dt->walk = d;
		return o;
	    }
	}

	dt->walk = NIL(Dt_t *);
	return NIL(Void_t *);
    }

    /* must be (DT_NEXT|DT_PREV) */
    if (!dt->walk || !(here = dt->walk->data->here) ||
	obj != OBJ(here, dt->walk->disc->link)) {
	for (d = dt; d; d = d->view) {
	    if ((o = (*(d->meth->searchf)) (d, obj, DT_SEARCH))) {
		dt->walk = d;
		goto do_adj;
	    }
	}

	dt->walk = NIL(Dt_t *);
	return NIL(Void_t *);
    }

  do_adj:for (d = dt->walk, o = (*(d->meth->searchf)) (d, obj, type);;)
    {
	while (o) {
	    disc = d->disc;
	    here = (d->meth->type & (DT_SET | DT_BAG)) ?
		d->data->here : NIL(Dtlink_t *);

	    for (p = dt;; p = p->view) {
		reg Dtdisc_t *dc;

		if (p == d)	/* this object is uncovered */
		    return o;

		/* see if it is covered */
		if (here && (p->meth->type & (DT_SET | DT_BAG)) &&
		    (disc == (dc = p->disc) ||
		     (disc->key == dc->key && disc->size == dc->size &&
		      disc->link == dc->link && disc->hashf == dc->hashf)))
		{
		    if ((*(p->meth->searchf)) (p, here, DT_VSEARCH))
			break;
		} else {
		    if ((*(p->meth->searchf)) (p, o, DT_SEARCH))
			break;
		}
	    }

	    o = (*(d->meth->searchf)) (d, o, type);
	}

	if (!(d = dt->walk = d->view))
	    return NIL(Void_t *);

	if (type & DT_NEXT)
	    o = (*(d->meth->searchf)) (d, NIL(Void_t *), DT_FIRST);
	else			/* if(type&DT_PREV) */
	    o = (*(d->meth->searchf)) (d, NIL(Void_t *), DT_LAST);
    }
}

#if __STD_C
Dt_t *dtview(reg Dt_t * dt, reg Dt_t * view)
#else
Dt_t *dtview(dt, view)
reg Dt_t *dt;
reg Dt_t *view;
#endif
{
    reg Dt_t *d;

    UNFLATTEN(dt);
    if (view)
	UNFLATTEN(view);

    /* make sure there won't be a cycle */
    for (d = view; d; d = d->view)
	if (d == dt)
	    return NIL(Dt_t *);

    /* no more viewing lower dictionary */
    if ((d = dt->view))
	d->nview -= 1;
    dt->view = dt->walk = NIL(Dt_t *);

    if (!view) {
	dt->searchf = dt->meth->searchf;
	return d;
    }

    /* ok */
    dt->view = view;
    dt->searchf = dtvsearch;
    view->nview += 1;

    return view;
}
