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

/*	List, Stack, Queue.
**
**	Written by Kiem-Phong Vo (05/25/96)
*/

#if __STD_C
static Void_t *dtlist(reg Dt_t * dt, reg Void_t * obj, reg int type)
#else
static Void_t *dtlist(dt, obj, type)
reg Dt_t *dt;
reg Void_t *obj;
reg int type;
#endif
{
    reg int lk, sz, ky;
    reg Dtcompar_f cmpf;
    reg Dtdisc_t *disc;
    reg Dtlink_t *r, *t;
    reg Void_t *key, *k;

    INITDISC(dt, disc, ky, sz, lk, cmpf);

    UNFLATTEN(dt);

    if (!obj) {
	if (type & (DT_LAST | DT_FIRST)) {
	    if ((r = dt->data->head)) {
		if (type & DT_LAST)
		    r = r->left;
		dt->data->here = r;
	    }
	    return r ? OBJ(r, lk) : NIL(Void_t *);
	} else if (type & (DT_DELETE | DT_DETACH)) {
	    if ((dt->data->type & DT_LIST) || !(r = dt->data->head))
		return NIL(Void_t *);
	    else
		goto dt_delete;
	} else if (type & DT_CLEAR) {
	    if (disc->freef || disc->link < 0) {
		for (r = dt->data->head; r; r = t) {
		    t = r->right;
		    if (disc->freef)
			(*disc->freef) (dt, OBJ(r, lk), disc);
		    if (disc->link < 0)
			(*dt->memoryf) (dt, (Void_t *) r, 0, disc);
		}
	    }
	    dt->data->head = dt->data->here = NIL(Dtlink_t *);
	    dt->data->size = 0;
	    return NIL(Void_t *);
	} else
	    return NIL(Void_t *);
    }

    if (type & (DT_INSERT | DT_ATTACH)) {
	if (disc->makef && (type & DT_INSERT) &&
	    !(obj = (*disc->makef) (dt, obj, disc)))
	    return NIL(Void_t *);
	if (lk >= 0)
	    r = ELT(obj, lk);
	else {
	    r = (Dtlink_t *) (*dt->memoryf)
		(dt, NIL(Void_t *), sizeof(Dthold_t), disc);
	    if (r)
		((Dthold_t *) r)->obj = obj;
	    else {
		if (disc->makef && disc->freef && (type & DT_INSERT))
		    (*disc->freef) (dt, obj, disc);
		return NIL(Void_t *);
	    }
	}

	if (dt->data->type & DT_LIST) {
	    if ((t = dt->data->here) && t != dt->data->head) {
		r->left = t->left;
		t->left->right = r;
		r->right = t;
		t->left = r;
	    } else
		goto dt_stack;
	} else if (dt->data->type & DT_STACK) {
	  dt_stack:
	    r->right = t = dt->data->head;
	    if (t) {
		r->left = t->left;
		t->left = r;
	    } else
		r->left = r;
	    dt->data->head = r;
	} else {		/* if(dt->data->type&DT_QUEUE) */
	    if ((t = dt->data->head)) {
		t->left->right = r;
		r->left = t->left;
		t->left = r;
	    } else {
		dt->data->head = r;
		r->left = r;
	    }
	    r->right = NIL(Dtlink_t *);
	}

	if (dt->data->size >= 0)
	    dt->data->size += 1;

	dt->data->here = r;
	return OBJ(r, lk);
    }

    if ((type & DT_MATCH) || !(r = dt->data->here) || OBJ(r, lk) != obj) {
	key = (type & DT_MATCH) ? obj : KEY(obj, ky, sz);
	for (r = dt->data->head; r; r = r->right) {
	    k = OBJ(r, lk);
	    k = KEY(k, ky, sz);
	    if (CMP(dt, key, k, disc, cmpf, sz) == 0)
		break;
	}
    }

    if (!r)
	return NIL(Void_t *);

    if (type & (DT_DELETE | DT_DETACH)) {
      dt_delete:
	if (r->right)
	    r->right->left = r->left;
	if (r == (t = dt->data->head)) {
	    dt->data->head = r->right;
	    if (dt->data->head)
		dt->data->head->left = t->left;
	} else {
	    r->left->right = r->right;
	    if (r == t->left)
		t->left = r->left;
	}

	dt->data->here = r == dt->data->here ? r->right : NIL(Dtlink_t *);
	dt->data->size -= 1;

	obj = OBJ(r, lk);
	if (disc->freef && (type & DT_DELETE))
	    (*disc->freef) (dt, obj, disc);
	if (disc->link < 0)
	    (*dt->memoryf) (dt, (Void_t *) r, 0, disc);
	return obj;
    } else if (type & DT_NEXT)
	r = r->right;
    else if (type & DT_PREV)
	r = r == dt->data->head ? NIL(Dtlink_t *) : r->left;

    dt->data->here = r;
    return r ? OBJ(r, lk) : NIL(Void_t *);
}

#ifndef KPVDEL			/* to be remove next round */
#define static
#endif
static Dtmethod_t _Dtlist = { dtlist, DT_LIST };
static Dtmethod_t _Dtstack = { dtlist, DT_STACK };
static Dtmethod_t _Dtqueue = { dtlist, DT_QUEUE };

__DEFINE__(Dtmethod_t *, Dtlist, &_Dtlist);
__DEFINE__(Dtmethod_t *, Dtstack, &_Dtstack);
__DEFINE__(Dtmethod_t *, Dtqueue, &_Dtqueue);
