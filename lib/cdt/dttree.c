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

/*	Ordered set/multiset
**	dt:	dictionary being searched
**	obj:	the object to look for.
**	type:	search type.
**
**      Written by Kiem-Phong Vo (5/25/96)
*/

#if __STD_C
static Void_t *dttree(Dt_t * dt, Void_t * obj, int type)
#else
static Void_t *dttree(dt, obj, type)
Dt_t *dt;
Void_t *obj;
int type;
#endif
{
    reg Dtlink_t *root, *t;
    reg int cmp, lk, sz, ky;
    reg Void_t *k, *key;
    reg Dtcompar_f cmpf;
    reg Dtdisc_t *disc;
    reg Dtlink_t *l, *r, *me;
    Dtlink_t link;

    me = 0;
    UNFLATTEN(dt);
    INITDISC(dt, disc, ky, sz, lk, cmpf);

    root = dt->data->here;
    if (!obj) {
	if (!root || !(type & (DT_CLEAR | DT_FIRST | DT_LAST)))
	    return NIL(Void_t *);

	if (type & DT_CLEAR) {	/* delete all objects */
	    if (disc->freef || disc->link < 0) {
		do {
		    while ((t = root->left))
			RROTATE(root, t);
		    t = root->right;
		    if (disc->freef)
			(*disc->freef) (dt, OBJ(root, lk), disc);
		    if (disc->link < 0)
			(*dt->memoryf) (dt, (Void_t *) root, 0, disc);
		} while ((root = t));
	    }

	    dt->data->size = 0;
	    dt->data->here = NIL(Dtlink_t *);
	    return NIL(Void_t *);
	} else {		/* computing largest/smallest element */
	    if (type & DT_LAST) {
		while ((t = root->right))
		    LROTATE(root, t);
	    } else {		/* type&DT_FIRST */
		while ((t = root->left))
		    RROTATE(root, t);
	    }

	    dt->data->here = root;
	    return OBJ(root, lk);
	}
    }

    /* note that link.right is LEFT tree and link.left is RIGHT tree */
    l = r = &link;

    if (type & (DT_MATCH | DT_SEARCH | DT_INSERT | DT_ATTACH)) {
	key = (type & DT_MATCH) ? obj : KEY(obj, ky, sz);
	if (root)
	    goto do_search;
    } else if (type & DT_RENEW) {
	me = (Dtlink_t *) obj;
	obj = OBJ(me, lk);
	key = KEY(obj, ky, sz);
	if (root)
	    goto do_search;
    } else if (root && OBJ(root, lk) != obj) {
	key = KEY(obj, ky, sz);
      do_search:
	while (1) {
	    k = OBJ(root, lk);
	    k = KEY(k, ky, sz);
	    if ((cmp = CMP(dt, key, k, disc, cmpf, sz)) == 0)
		break;
	    else if (cmp < 0) {
		if ((t = root->left)) {
		    k = OBJ(t, lk);
		    k = KEY(k, ky, sz);
		    if ((cmp = CMP(dt, key, k, disc, cmpf, sz)) < 0) {
			RROTATE(root, t);
			RLINK(r, root);
			if (!(root = root->left))
			    break;
		    } else if (cmp == 0) {
			RROTATE(root, t);
			break;
		    } else {	/* if(cmp > 0) */
			LLINK(l, t);
			RLINK(r, root);
			if (!(root = t->right))
			    break;
		    }
		} else {
		    RLINK(r, root);
		    root = NIL(Dtlink_t *);
		    break;
		}
	    } else {		/* if(cmp > 0) */
		if ((t = root->right)) {
		    k = OBJ(t, lk);
		    k = KEY(k, ky, sz);
		    if ((cmp = CMP(dt, key, k, disc, cmpf, sz)) > 0) {
			LROTATE(root, t);
			LLINK(l, root);
			if (!(root = root->right))
			    break;
		    } else if (cmp == 0) {
			LROTATE(root, t);
			break;
		    } else {	/* if(cmp < 0) */
			RLINK(r, t);
			LLINK(l, root);
			if (!(root = t->left))
			    break;
		    }
		} else {
		    LLINK(l, root);
		    root = NIL(Dtlink_t *);
		    break;
		}
	    }
	}
    }

    if (root) {			/* found it, now isolate it */
	l->right = root->left;
	r->left = root->right;

	if (type & (DT_SEARCH | DT_MATCH)) {
	  has_root:
	    root->left = link.right;
	    root->right = link.left;
	    if ((dt->meth->type & DT_OBAG)
		&& (type & (DT_SEARCH | DT_MATCH))) {
		key = OBJ(root, lk);
		key = KEY(key, ky, sz);
		while ((t = root->left)) {
		    k = OBJ(t, lk);
		    k = KEY(k, ky, sz);
		    if (CMP(dt, key, k, disc, cmpf, sz) != 0)
			break;
		    RROTATE(root, t);
		}
	    }
	    dt->data->here = root;
	    return OBJ(root, lk);
	} else if (type & DT_NEXT) {
	    root->left = link.right;
	    root->right = NIL(Dtlink_t *);
	    link.right = root;
	  dt_next:
	    if ((root = link.left)) {
		while ((t = root->left))
		    RROTATE(root, t);
		link.left = root->right;
		goto has_root;
	    } else
		goto no_root;
	} else if (type & DT_PREV) {
	    root->right = link.left;
	    root->left = NIL(Dtlink_t *);
	    link.left = root;
	  dt_prev:
	    if ((root = link.right)) {
		while ((t = root->right))
		    LROTATE(root, t);
		link.right = root->left;
		goto has_root;
	    } else
		goto no_root;
	} else if (type & (DT_DELETE | DT_DETACH)) {
	    obj = OBJ(root, lk);
	    if (disc->freef && (type & DT_DELETE))
		(*disc->freef) (dt, obj, disc);
	    if (disc->link < 0)
		(*dt->memoryf) (dt, (Void_t *) root, 0, disc);
	    if ((dt->data->size -= 1) < 0)
		dt->data->size = -1;
	    goto no_root;
	} else if (type & (DT_INSERT | DT_ATTACH)) {
	    if (dt->meth->type & DT_OSET)
		goto has_root;
	    else {
		root->left = NIL(Dtlink_t *);
		root->right = link.left;
		link.left = root;
		goto dt_insert;
	    }
	} else if (type & DT_RENEW) {	/* a duplicate */
	    if (dt->meth->type & DT_OSET) {
		if (disc->freef)
		    (*disc->freef) (dt, obj, disc);
		if (disc->link < 0)
		    (*dt->memoryf) (dt, (Void_t *) me, 0, disc);
	    } else {
		me->left = NIL(Dtlink_t *);
		me->right = link.left;
		link.left = me;
		dt->data->size += 1;
	    }
	    goto has_root;
	}
    } else {			/* not found, finish up LEFT and RIGHT trees */
	r->left = NIL(Dtlink_t *);
	l->right = NIL(Dtlink_t *);

	if (type & (DT_SEARCH | DT_MATCH)) {
	  no_root:
	    while ((t = r->left))
		r = t;
	    r->left = link.right;
	    dt->data->here = link.left;
	    return (type & DT_DELETE) ? obj : NIL(Void_t *);
	} else if (type & (DT_INSERT | DT_ATTACH)) {
	  dt_insert:
	    if (disc->makef && (type & DT_INSERT))
		obj = (*disc->makef) (dt, obj, disc);
	    if (obj) {
		if (lk >= 0)
		    root = ELT(obj, lk);
		else {
		    root = (Dtlink_t *) (*dt->memoryf)
			(dt, NIL(Void_t *), sizeof(Dthold_t), disc);
		    if (root)
			((Dthold_t *) root)->obj = obj;
		    else if (disc->makef && disc->freef &&
			     (type & DT_INSERT))
			(*disc->freef) (dt, obj, disc);
		}
	    }
	    if (root) {
		if (dt->data->size >= 0)
		    dt->data->size += 1;
		goto has_root;
	    } else
		goto no_root;
	} else if (type & DT_NEXT)
	    goto dt_next;
	else if (type & DT_PREV)
	    goto dt_prev;
	else if (type & DT_RENEW) {
	    root = me;
	    dt->data->size += 1;
	    goto has_root;
	} else {		/*if(type&DT_DELETE) */
	    obj = NIL(Void_t *);
	    goto no_root;
	}
    }

    return NIL(Void_t *);
}

/* make this method available */
static Dtmethod_t _Dtoset = { dttree, DT_OSET };
static Dtmethod_t _Dtobag = { dttree, DT_OBAG };

__DEFINE__(Dtmethod_t *, Dtoset, &_Dtoset);
__DEFINE__(Dtmethod_t *, Dtobag, &_Dtobag);

#ifndef KPVDEL			/* backward compatibility - delete next time around */
Dtmethod_t _Dttree = { dttree, DT_OSET };

__DEFINE__(Dtmethod_t *, Dtorder, &_Dttree);
__DEFINE__(Dtmethod_t *, Dttree, &_Dttree);
#endif
