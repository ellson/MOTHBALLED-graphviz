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


#include "blocktree.h"

static void addNode(block_t * bp, Agnode_t * n)
{
#ifndef WITH_CGRAPH
    aginsert(bp->sub_graph, n);
#else /* WITH_CGRAPH */
    agsubnode(bp->sub_graph, n,1);
#endif /* WITH_CGRAPH */
    SET_BCDONE(n);
    BLOCK(n) = bp;
}

static Agraph_t *makeBlockGraph(Agraph_t * g, circ_state * state)
{
    char name[SMALLBUF];
    Agraph_t *subg;

    sprintf(name, "_block_%d", state->blockCount++);
#ifndef WITH_CGRAPH
    subg = agsubg(g, name);
#else /* WITH_CGRAPH */
    subg = agsubg(g, name,1);
    agbindrec(subg, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);	//node custom data
#endif /* WITH_CGRAPH */
    return subg;
}

static block_t *makeBlock(Agraph_t * g, circ_state * state)
{
    Agraph_t *subg = makeBlockGraph(g, state);
    block_t *bp = mkBlock(subg);

    return bp;
}

typedef struct {
    Agedge_t *top;
    int sz;
} estack;

static void
push (estack* s, Agedge_t* e)
{
    ENEXT(e) = s->top;
    s->top = e;
    s->sz += 1;
}

static Agedge_t*
pop (estack* s)
{
    Agedge_t *top = s->top;

    if (top) {
	assert(s->sz > 0);
	s->top = ENEXT(top);
	s->sz -= 1;
    } else {
	assert(0);
    }

    return top;
}


static void dfs(Agraph_t * g, Agnode_t * u, circ_state * state, int isRoot, estack* stk)
{
    Agedge_t *e;
    Agnode_t *v;

    LOWVAL(u) = VAL(u) = state->orderCount++;
    for (e = agfstedge(g, u); e; e = agnxtedge(g, e, u)) {
	v = aghead (e);
	if (v == u) {
            v = agtail(e);
	    if (!EDGEORDER(e)) EDGEORDER(e) = -1;
	}
	else {
	    if (!EDGEORDER(e)) EDGEORDER(e) = 1;
	}

        if (VAL(v) == 0) {
	    PARENT(v) = u;
            push(stk, e);
            dfs(g, v, state, 0, stk);
            LOWVAL(u) = MIN(LOWVAL(u), LOWVAL(v));
            if (LOWVAL(v) >= VAL(u)) {       /* u is an articulation point */
		block_t *block = NULL;
		Agnode_t *np;
		Agedge_t *ep;
                do {
                    ep = pop(stk);
		    if (EDGEORDER(ep) == 1)
			np = aghead (ep);
		    else
			np = agtail (ep);
		    if (!BLOCK(np)) {
			if (!block)
			    block = makeBlock(g, state);
			addNode(block, np);
		    }
                } while (ep != e);
		if (block) {	/* If block != NULL, it's not empty */
		    if (blockSize (block) > 1)
			addNode(block, u);
		    if (isRoot && (BLOCK(u) == block))
			insertBlock(&state->bl, block);
		    else
			appendBlock(&state->bl, block);
		}
            }
        } else if (PARENT(u) != v) {
            LOWVAL(u) = MIN(LOWVAL(u), VAL(v));
        }
    }
    if (isRoot && !BLOCK(u)) {
	block_t *block = makeBlock(g, state);
	addNode(block, u);
	insertBlock(&state->bl, block);
    }
}


/* find_blocks:
 */
static void find_blocks(Agraph_t * g, circ_state * state)
{
    Agnode_t *n;
    Agnode_t *root = NULL;
    estack stk;

    /*      check to see if there is a node which is set to be the root
     */
    if (state->rootname) {
	root = agfindnode(g, state->rootname);
    }
    if (!root && state->N_root) {
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    if (late_bool(ORIGN(n), state->N_root, 0)) {
		root = n;
		break;
	    }
	}
    }

    if (!root)
	root = agfstnode(g);
    if (Verbose)
	fprintf (stderr, "root = %s\n", root->name);
    stk.sz = 0;
    stk.top = NULL;
    dfs(g, root, state, 1, &stk);

}

/* create_block_tree:
 * Construct block tree by peeling nodes from block list in state.
 * When done, return root. The block list is empty
 * FIX: use largest block as root
 */
block_t *createBlocktree(Agraph_t * g, circ_state * state)
{
    block_t *bp;
    block_t *next;
    block_t *root;
    int min;
    /* int        ordercnt; */

    find_blocks(g, state);

    bp = state->bl.first;	/* if root chosen, will be first */
    /* Otherwise, just pick first as root */
    root = bp;

    /* Find node with minimum VAL value to find parent block */
    /* FIX: Should be some way to avoid search below.               */
    /* ordercnt = state->orderCount;  */
    for (bp = bp->next; bp; bp = next) {
	Agnode_t *n;
	Agnode_t *parent;
	Agnode_t *child;
	Agraph_t *subg = bp->sub_graph;

	child = n = agfstnode(subg);

	min = VAL(n);
	parent = PARENT(n);
	for (n = agnxtnode(subg, n); n; n = agnxtnode(subg, n)) {
	    if (VAL(n) < min) {
		child = n;
		min = VAL(n);
		parent = PARENT(n);
	    }
	}
	SET_PARENT(parent);
	CHILD(bp) = child;
	next = bp->next;	/* save next since list insertion destroys it */
	appendBlock(&(BLOCK(parent)->children), bp);
    }
    initBlocklist(&state->bl);	/* zero out list */
    return root;
}

void freeBlocktree(block_t * bp)
{
    block_t *child;
    block_t *next;

    for (child = bp->children.first; child; child = next) {
	next = child->next;
	freeBlocktree(child);
    }

    freeBlock(bp);
}

#ifdef DEBUG
static void indent(int i)
{
    while (i--)
	fputs("  ", stderr);
}

void print_blocktree(block_t * sn, int depth)
{
    block_t *child;
    Agnode_t *n;
    Agraph_t *g;

    indent(depth);
    g = sn->sub_graph;
    fprintf(stderr, "%s:", g->name);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	fprintf(stderr, " %s", n->name);
    }
    fputs("\n", stderr);

    depth++;
    for (child = sn->children.first; child; child = child->next) {
	print_blocktree(child, depth);
    }
}

#endif
