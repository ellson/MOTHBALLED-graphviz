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


#include <circular.h>
#include	<block.h>
#include	<assert.h>

void initBlocklist(blocklist_t * bl)
{
    bl->first = NULL;
    bl->last = NULL;
}

/*
void
cleanBlocklist(blocklist_t* sp)
{
	block_t*  bp;
	block_t*  temp;

    if (!sp) return;
	for(bp = sp->first; bp; bp = temp) {
		temp = bp->next;
		freeBlock(bp);
	}
}
*/

block_t *mkBlock(Agraph_t * g)
{
    block_t *sn;

    sn = NEW(block_t);
    initBlocklist(&sn->children);
    sn->sub_graph = g;
    return sn;
}

void freeBlock(block_t * sp)
{
    if (!sp)
	return;
    freeNodelist(sp->circle_list);
    free(sp);
}

/* appendBlock:
 * add block at end
 */
void appendBlock(blocklist_t * bl, block_t * bp)
{
    bp->next = NULL;
    if (bl->last) {
	bl->last->next = bp;
	bl->last = bp;
    } else {
	bl->first = bp;
	bl->last = bp;
    }
}

/* insertBlock:
 * add block at beginning
 */
void insertBlock(blocklist_t * bl, block_t * bp)
{
    if (bl->first) {
	bp->next = bl->first;
	bl->first = bp;
    } else {
	bl->first = bp;
	bl->last = bp;
    }
}

#ifdef DEBUG
void printBlocklist(blocklist_t * snl)
{
    block_t *bp;
    for (bp = snl->first; bp; bp = bp->next) {
	Agnode_t *n;
	char *p;
	Agraph_t *g = bp->sub_graph;
	fprintf(stderr, "block=%s\n", g->name);
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    Agedge_t *e;
	    if (PARENT(n))
		p = PARENT(n)->name;
	    else
		p = "<nil>";
	    fprintf(stderr, "  %s (%d %s)\n", n->name, VAL(n), p);
	    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
		fprintf(stderr, "    %s--%s\n", e->tail->name,
			e->head->name);
	    }
	}
    }
}
#endif
