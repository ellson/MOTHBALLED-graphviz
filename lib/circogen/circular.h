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

#ifndef CIRCULAR_H
#define CIRCULAR_H

#include <render.h>
#include <block.h>
#include <stack.h>

typedef struct {
    blocklist_t bl;
    int orderCount;
    int blockCount;
    nstack_t *bcstack;
    attrsym_t *N_artpos;
    attrsym_t *N_root;
    char *rootname;
    double min_dist;
} circ_state;

typedef struct {
    Agnode_t *dnode;
} ndata;

/* Extra node data used for layout:
 * Pass O: build derived graph
 * Pass 1: construct blocks
 * Pass 2: construct block tree
 * Pass 3: layout block
 *      a:  construct block skeleton
 *      b:  construct skeleton spanning tree
 *      c:  construct circular list of nodes
 * Pass 4: connect blocks
 */
typedef struct {
    union {			/* Pointer to node/cluster in original graph */
	Agraph_t *g;
	Agnode_t *np;
    } orig;
    int flags;
    node_t *parent;		/* parent in block-cutpoint traversal (1,2,4) */
    block_t *block;		/* Block containing node (1,2,3,4) */
    union {
	struct {		/* Pass  1 */
	    node_t *next;	/* used for stack */
	    int val;
	    int low_val;
	} bc;
	node_t *clone;		/* Cloned node (3a) */
	struct {		/* Spanning tree and longest path (3b) */
	    node_t *tparent;	/* Parent in tree */
	    node_t *first;	/* Leaf on longest path from node */
	    node_t *second;	/* Leaf on 2nd longest path from node */
	    int fdist;		/* Length of longest path from node */
	    int sdist;		/* Length of 2nd longest path from node */
	} t;
	struct {
	    int pos;		/* Index of node in block circle (3c,4) */
	    double psi;		/* Offset angle of children (4) */
	} f;
    } u;
} cdata;

typedef struct {
    int order;
} edata;

#define NDATA(n) ((ndata*)((n)->u.alg))
#define DNODE(n)	(NDATA(n)->dnode)

#define EDGEDATA(e)  ((edata*)((e)->u.alg))
#define EDGEORDER(e) (EDGEDATA(e)->order)

#define DATA(n) ((cdata*)((n)->u.alg))
#define ORIGG(n)	(DATA(n)->orig.g)
#define ORIGN(n)	(DATA(n)->orig.np)
#define FLAGS(n) (DATA(n)->flags)
#define PARENT(n) (DATA(n)->parent)
#define BLOCK(n) (DATA(n)->block)
#define NEXT(n) (DATA(n)->u.bc.next)
#define VAL(n) (DATA(n)->u.bc.val)
#define LOWVAL(n)	 (DATA(n)->u.bc.low_val)
#define CLONE(n) (DATA(n)->u.clone)
#define TPARENT(n)	(DATA(n)->u.t.tparent)
#define LEAFONE(n)	(DATA(n)->u.t.first)
#define LEAFTWO(n)	(DATA(n)->u.t.second)
#define DISTONE(n)	(DATA(n)->u.t.fdist)
#define DISTTWO(n)	(DATA(n)->u.t.sdist)
#define POSITION(n) (DATA(n)->u.f.pos)
#define PSI(n) (DATA(n)->u.f.psi)

#define VISITED_F   (1 << 0)
#define BCDONE_F    (1 << 1)
#define ONSTACK_F   (1 << 2)
#define PARENT_F    (1 << 3)
#define PATH_F      (1 << 4)
#define NEIGHBOR_F  (1 << 5)

#define VISITED(n) (FLAGS(n)&VISITED_F)
#define BCDONE(n)	 (FLAGS(n)&BCDONE_F)
#define ONSTACK(n)	 (FLAGS(n)&ONSTACK_F)
#define ISPARENT(n)	 (FLAGS(n)&PARENT_F)
#define ONPATH(n)	 (FLAGS(n)&PATH_F)
#define NEIGHBOR(n)	 (FLAGS(n)&NEIGHBOR_F)

#define SET_VISITED(n) (FLAGS(n) |= VISITED_F)
#define SET_BCDONE(n)	 (FLAGS(n) |= BCDONE_F)
#define SET_ONSTACK(n)	 (FLAGS(n) |= ONSTACK_F)
#define SET_PARENT(n)	 (FLAGS(n) |= PARENT_F)
#define SET_ONPATH(n)	 (FLAGS(n) |= PATH_F)
#define SET_NEIGHBOR(n)	 (FLAGS(n) |= NEIGHBOR_F)

#define UNSET_VISITED(n) (FLAGS(n) &= ~VISITED_F)
#define UNSET_BCDONE(n)	 (FLAGS(n) &= ~BCDONE_F)
#define UNSET_ONSTACK(n)	 (FLAGS(n) &= ~ONSTACK_F)
#define UNSET_NEIGHBOR(n)	 (FLAGS(n) &= ~NEIGHBOR_F)

#define DEGREE(n) (ND_order(n))

#include <circo.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
    extern void prData(Agnode_t * n, int pass);
#endif

    extern void circularLayout(Agraph_t * sg);

#ifdef __cplusplus
}
#endif
#endif
