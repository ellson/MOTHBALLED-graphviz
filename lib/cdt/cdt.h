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

#ifndef _CDT_H
#define _CDT_H		1

/*	Public interface for the dictionary library
**
**      Written by Kiem-Phong Vo (05/25/96)
*/

#define CDT_VERSION	19991101L

#define Void_t		void
#define _ARG_(x)	x
#ifndef NIL
#define NIL(type) ((type)0)
#endif

#include <string.h>

#if _PACKAGE_ast
#  include	<ast_std.h>
#elif _BLD_cdt
#  include	"ast_common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _dtlink_s Dtlink_t;
    typedef struct _dthold_s Dthold_t;
    typedef struct _dtdisc_s Dtdisc_t;
    typedef struct _dtmethod_s Dtmethod_t;
    typedef struct _dtdata_s Dtdata_t;
    typedef struct _dt_s Dt_t;
    typedef struct _dt_s Dict_t;	/* for libdict compatibility */
    typedef struct _dtstat_s Dtstat_t;
    typedef Void_t *(*Dtsearch_f) _ARG_((Dt_t *, Void_t *, int));
    typedef Void_t *(*Dtmake_f) _ARG_((Dt_t *, Void_t *, Dtdisc_t *));
    typedef void (*Dtfree_f) _ARG_((Dt_t *, Void_t *, Dtdisc_t *));
    typedef int (*Dtcompar_f)
	_ARG_((Dt_t *, Void_t *, Void_t *, Dtdisc_t *));
    typedef unsigned int (*Dthash_f) _ARG_((Dt_t *, Void_t *, Dtdisc_t *));
    typedef Void_t *(*Dtmemory_f)
	_ARG_((Dt_t *, Void_t *, size_t, Dtdisc_t *));
    typedef int (*Dtevent_f) _ARG_((Dt_t *, int, Void_t *, Dtdisc_t *));

    struct _dtlink_s {
	Dtlink_t *right;	/* right child          */
	union {
	    unsigned int _hash;	/* hash value           */
	    Dtlink_t *_left;	/* left child           */
	} hl;
    };

/* private structure to hold an object */
    struct _dthold_s {
	Dtlink_t hdr;		/* header               */
	Void_t *obj;		/* user object          */
    };

/* method to manipulate dictionary structure */
    struct _dtmethod_s {
	Dtsearch_f searchf;	/* search function     */
	int type;		/* type of operation    */
    };

/* stuff that may be in shared memory */
    struct _dtdata_s {
	int type;		/* type of dictionary                   */
	Dtlink_t *here;		/* finger to last search element        */
	union {
	    Dtlink_t **_htab;	/* hash table                           */
	    Dtlink_t *_head;	/* linked list                          */
	} hh;
	int ntab;		/* number of hash slots                 */
	int size;		/* number of objects                    */
	int loop;		/* number of nested loops               */
    };

/* structure to hold methods that manipulate an object */
    struct _dtdisc_s {
	int key;		/* where the key begins in an object    */
	int size;		/* key size and type                    */
	int link;		/* offset to Dtlink_t field             */
	Dtmake_f makef;		/* object constructor                   */
	Dtfree_f freef;		/* object destructor                    */
	Dtcompar_f comparf;	/* to compare two objects               */
	Dthash_f hashf;		/* to compute hash value of an object   */
	Dtmemory_f memoryf;	/* to allocate/free memory              */
	Dtevent_f eventf;	/* to process events                    */
    };

/* the dictionary structure itself */
    struct _dt_s {
	Dtsearch_f searchf;	/* search function                      */
	Dtdisc_t *disc;		/* method to manipulate objs            */
	Dtdata_t *data;		/* sharable data                        */
	Dtmemory_f memoryf;	/* function to alloc/free memory        */
	Dtmethod_t *meth;	/* dictionary method                    */
	int type;		/* type information                     */
	int nview;		/* number of parent view dictionaries   */
	Dt_t *view;		/* next on viewpath                     */
	Dt_t *walk;		/* dictionary being walked              */
    };

/* structure to get status of a dictionary */
    struct _dtstat_s {
	int dt_meth;		/* method type                          */
	int dt_size;		/* number of elements                   */
	int dt_n;		/* number of chains or levels           */
	int dt_max;		/* max size of a chain or a level       */
	int *dt_count;		/* counts of chains or levels by size   */
    };

/* supported storage methods */
#define DT_SET		0000001	/* set with unique elements             */
#define DT_BAG		0000002	/* multiset                             */
#define DT_OSET		0000004	/* ordered set (self-adjusting tree)    */
#define DT_OBAG		0000010	/* ordered multiset                     */
#define DT_LIST		0000020	/* linked list                          */
#define DT_STACK	0000040	/* stack                                */
#define DT_QUEUE	0000100	/* queue                                */
#define DT_METHODS	0000177	/* all currently supported methods      */

/* asserts to dtdisc() */
#define DT_SAMECMP	0000001	/* compare methods equivalent           */
#define DT_SAMEHASH	0000002	/* hash methods equivalent              */

/* types of search */
#define DT_INSERT	0000001	/* insert object if not found           */
#define DT_DELETE	0000002	/* delete object if found               */
#define DT_SEARCH	0000004	/* look for an object                   */
#define DT_NEXT		0000010	/* look for next element                */
#define DT_PREV		0000020	/* find previous element                */
#define DT_RENEW	0000040	/* renewing an object                   */
#define DT_CLEAR	0000100	/* clearing all objects                 */
#define DT_FIRST	0000200	/* get first object                     */
#define DT_LAST		0000400	/* get last object                      */
#define DT_MATCH	0001000	/* find object matching key             */
#define DT_VSEARCH	0002000	/* search using internal representation */
#define DT_ATTACH	0004000	/* attach an object to the dictionary   */
#define DT_DETACH	0010000	/* attach an object to the dictionary   */

/* events */
#define DT_OPEN		1	/* a dictionary is being opened         */
#define DT_CLOSE	2	/* a dictionary is being closed         */
#define DT_DISC		3	/* discipline is about to be changed    */
#define DT_METH		4	/* method is about to be changed        */

#if _BLD_cdt && defined(__EXPORT__)
#define extern	__EXPORT__
#endif
#if !_BLD_cdt && defined(__IMPORT__)
#define extern	__IMPORT__
#endif
     extern Dtmethod_t *Dtset;
    extern Dtmethod_t *Dtbag;
    extern Dtmethod_t *Dtoset;
    extern Dtmethod_t *Dtobag;
    extern Dtmethod_t *Dtlist;
    extern Dtmethod_t *Dtstack;
    extern Dtmethod_t *Dtqueue;

/* compatibility stuff; will go away */
#ifndef KPVDEL
    extern Dtmethod_t *Dtorder;
    extern Dtmethod_t *Dttree;
    extern Dtmethod_t *Dthash;
    extern Dtmethod_t _Dttree;
    extern Dtmethod_t _Dthash;
    extern Dtmethod_t _Dtlist;
    extern Dtmethod_t _Dtqueue;
    extern Dtmethod_t _Dtstack;
#endif

#undef extern
#if _BLD_cdt && defined(__EXPORT__)
#define extern	__EXPORT__
#endif
#if !_BLD_cdt && defined(__IMPORT__) && defined(__EXPORT__)
#define extern	__IMPORT__
#endif
     extern Dt_t *dtopen _ARG_((Dtdisc_t *, Dtmethod_t *));
    extern int dtclose _ARG_((Dt_t *));
    extern Dt_t *dtview _ARG_((Dt_t *, Dt_t *));
    extern Dtdisc_t *dtdisc _ARG_((Dt_t * dt, Dtdisc_t *, int));
    extern Dtmethod_t *dtmethod _ARG_((Dt_t *, Dtmethod_t *));

    extern Dtlink_t *dtflatten _ARG_((Dt_t *));
    extern Dtlink_t *dtextract _ARG_((Dt_t *));
    extern int dtrestore _ARG_((Dt_t *, Dtlink_t *));

    extern int dtwalk
	_ARG_((Dt_t *, int (*)(Dt_t *, Void_t *, Void_t *), Void_t *));

    extern Void_t *dtrenew _ARG_((Dt_t *, Void_t *));

    extern int dtsize _ARG_((Dt_t *));
    extern int dtstat _ARG_((Dt_t *, Dtstat_t *, int));
    extern unsigned int dtstrhash _ARG_((unsigned int, Void_t *, int));

#undef extern

#define _DT_(d)		((Dt_t*)(d))
#define dtvnext(d)	(_DT_(d)->view)
#define dtvcount(d)	(_DT_(d)->nview)
#define dtvhere(d)	(_DT_(d)->walk)
#define dtlink(d,e)	(((Dtlink_t*)(e))->right)
#define dtobj(d,e)	((_DT_(d)->disc->link < 0) ? (((Dthold_t*)(e))->obj) : \
				(Void_t*)((char*)(e) - _DT_(d)->disc->link) )
#define dtfinger(d)	(_DT_(d)->data->here ? dtobj((d),_DT_(d)->data->here) : \
				(Void_t*)(0) )
#define dtfirst(d)	(*(_DT_(d)->searchf))((d),(Void_t*)(0),DT_FIRST)
#define dtnext(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_NEXT)
#define dtlast(d)	(*(_DT_(d)->searchf))((d),(Void_t*)(0),DT_LAST)
#define dtprev(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_PREV)
#define dtsearch(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_SEARCH)
#define dtmatch(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_MATCH)
#define dtinsert(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_INSERT)
#define dtdelete(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_DELETE)
#define dtattach(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_ATTACH)
#define dtdetach(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_DETACH)
#define dtclear(d)	(*(_DT_(d)->searchf))((d),(Void_t*)(0),DT_CLEAR)
/* A linear congruential hash: h*17 + c + 97531 */
#define dtcharhash(h,c)	((((unsigned int)(h))<<4) + ((unsigned int)(h)) + \
			 ((unsigned char)(c)) + 97531 )
#ifdef __cplusplus
}
#endif
#endif				/* _CDT_H */
