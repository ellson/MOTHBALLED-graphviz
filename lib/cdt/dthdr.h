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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DTHDR_H
#define _DTHDR_H	1
#ifndef _BLD_cdt
#define _BLD_cdt	1
#endif

/*	Internal definitions for libcdt.
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if _PACKAGE_ast
#include	<ast.h>
#endif

#include	<cdt.h>

/* short-hand notations */
#define NIL(t)	((t)0)
#define reg	register
#define uint	unsigned int
#define left	hl._left
#define hash	hl._hash
#define htab	hh._htab
#define head	hh._head

/* this must be disjoint from DT_METHODS */
#define DT_FLATTEN	010000	/* dictionary already flattened */
#define DT_WALK		020000	/* hash table being walked      */

/* hash start size and load factor */
#define HSLOT		(32)
#define HRESIZE(n)	((n) << 1)
#define HLOAD(s)	((s) << 1)
#define HINDEX(n,h)	((h)&((n)-1))

#define UNFLATTEN(dt) \
		((dt->data->type&DT_FLATTEN) ? dtrestore(dt,NIL(Dtlink_t*)) : 0)

/* the pointer to the actual object */
#define INITDISC(dt,d,ky,sz,lk,cmpf) \
		(d = dt->disc, ky = d->key, sz = d->size, lk = d->link, cmpf = d->comparf)
#define ELT(o,lk)	((Dtlink_t*)((char*)(o) + lk) )
#define OBJ(e,lk)	(lk < 0 ? ((Dthold_t*)(e))->obj : (Void_t*)((char*)(e) - lk) )
#define KEY(o,ky,sz)	((Void_t*)(sz < 0 ? *((char**)((char*)(o) + ky)) : \
					    ((char*)(o) + ky) ) )

/* compare and hash functions */
#define CMP(dt,k1,k2,d,cmpf,sz) \
		(cmpf ? (*cmpf)(dt,k1,k2,d) : \
		 	 sz <= 0 ? strcmp(k1,k2) : memcmp(k1,k2,sz) )
#define HASH(dt,k,d,sz)	(d->hashf ? (*d->hashf)(dt,k,d) : dtstrhash(0,k,sz) )

/* tree rotation functions */
#define RROTATE(x,y)	((x)->left = (y)->right, (y)->right = (x), (x) = (y))
#define LROTATE(x,y)	((x)->right = (y)->left, (y)->left  = (x), (x) = (y))
#define RLINK(r,x)	((r) = (r)->left  = (x) )
#define LLINK(l,x)	((l) = (l)->right = (x) )

#if !_PACKAGE_ast
    _BEGIN_EXTERNS_ extern Void_t *malloc _ARG_((size_t));
    extern Void_t *realloc _ARG_((Void_t *, size_t));
    extern void free _ARG_((Void_t *));
    extern int memcmp _ARG_((const Void_t *, const Void_t *, size_t));
    extern int strcmp _ARG_((const char *, const char *));
     _END_EXTERNS_
#endif
#endif				/* _DTHDR_H */
#ifdef __cplusplus
}
#endif
