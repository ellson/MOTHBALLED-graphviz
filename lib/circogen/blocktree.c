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

static int min_value(int x, int y)
{
    if (x < y)
	return x;
    return y;
}

static void addNode(block_t * bp, Agnode_t * n)
{
    aginsert(bp->sub_graph, n);
    SET_BCDONE(n);
    BLOCK(n) = bp;
}

static Agraph_t *makeBlockGraph(Agraph_t * g, circ_state * state)
{
    char name[SMALLBUF];
    Agraph_t *subg;

    sprintf(name, "_block_%d", state->blockCount++);
    subg = agsubg(g, name);
    return subg;
}

static block_t *makeBlock(Agraph_t * g, circ_state * state)
{
    Agraph_t *subg = makeBlockGraph(g, state);
    block_t *bp = mkBlock(subg);

    return bp;
}

/* dfs:
 *
 * Current scheme adds articulation point to first non-trivial child
 * block. If none exists, it will be added to its parent's block, if
 * non-trivial, or else given its own block.
 *
 * FIX:
 * This should be modified to:
 *  - allow user to specify which block gets a node, perhaps on per-node basis.
 *  - if an articulation point is not used in one of its non-trivial blocks,
 *    dummy edges should be added to preserve biconnectivity
 *  - turn on user-supplied blocks.
 *
 */
static void dfs(Agraph_t * g, Agnode_t * n, circ_state * state, int isRoot)
{
    Agedge_t *e;
    Agnode_t *curtop;

    LOWVAL(n) = VAL(n) = state->orderCount++;

    stackPush(state->bcstack, n);

    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	Agnode_t *neighbor = e->head;
	if (neighbor == n)
	    neighbor = e->tail;

	if (neighbor == PARENT(n))
	    continue;

	if (VAL(neighbor)) {
	    LOWVAL(n) = min_value(LOWVAL(n), VAL(neighbor));
	    continue;
	}
	if (!stackCheck(state->bcstack, n)) {
	    stackPush(state->bcstack, n);
	}

	PARENT(neighbor) = n;
	curtop = top(state->bcstack);
	dfs(g, neighbor, state, 0);

	LOWVAL(n) = min_value(LOWVAL(n), LOWVAL(neighbor));
	if (LOWVAL(neighbor) >= VAL(n)) {
	    block_t *block = NULL;
	    Agnode_t *np;
	    if (top(state->bcstack) != curtop)
		do {
		    np = stackPop(state->bcstack);
		    if (!BCDONE(np)) {
			if (!block)
			    block = makeBlock(g, state);
			addNode(block, np);
		    }
		} while (np != n);
	    if (block) {	/* If block != NULL, it's not empty */
		if (isRoot && (BLOCK(n) == block))
		    insertBlock(&state->bl, block);
		else
		    appendBlock(&state->bl, block);
	    }
	    if ((LOWVAL(n) < VAL(n)) && (!stackCheck(state->bcstack, n))) {
		stackPush(state->bcstack, n);
	    }
	}
    }
    if ((LOWVAL(n) == VAL(n)) && !BCDONE(n)) {
	block_t *block = makeBlock(g, state);
	stackPop(state->bcstack);
	addNode(block, n);
	if (isRoot)
	    insertBlock(&state->bl, block);
	else
	    appendBlock(&state->bl, block);
    }
}

#ifdef USER_BLOCKS
/* findUnvisited:
 * Look for unvisited node n in visited block (i.e., some nodes in
 * the block have been visited) with neighbor not in block. Note
 * that therefore neighbor is unvisited. Set neighbor's parent to n
 * and return neighbor.
 * Guaranteed blp is non-empty.
 * 
 */
static Agnode_t *findUnvisited(blocklist_t * blp)
{
    Agnode_t *retn = NULL;
  FIX:finish Agnode_t * n;
    Agnode_t *newn;
    graph_t *clust_subg;
    edge_t *e;
    block_t *bp;
    block_t *prev = NULL;

    for (bp = blp->first; prev != blp->last; bp = bp->next) {
	prev = bp;
	clust = bp->sub_graph;

	if (DONE(bp))
	    continue;
	if (PARTIAL(bp)) {
	    for (n = agfstnode(clust); n; n = agnxtnode(clust, n)) {
		if (!VISITED(n)) {
		    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
			newn = e->head;
			if (newn == n)
			    newn = e->tail;
			if ((BLOCK(newn) != bp)) {
			    retn = newn;
			    return;
			}
		    }
		    /* mark n visited */
		}
	    }
	    /* mark bp done */
	} else {
	}
    }
    return retn;
}
#endif

/* find_blocks:
 */
static void find_blocks(Agraph_t * g, circ_state * state)
{
    Agnode_t *n;
    Agnode_t *root = NULL;
    block_t *rootBlock = NULL;
    blocklist_t ublks;
#ifdef USER_BLOCKS
    graph_t *clust_subg;
    graph_t *mg;
    edge_t *me;
    node_t *mm;
    int isRoot;
#endif

    initBlocklist(&ublks);

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
#ifdef USER_BLOCKS
    /* process clusters first */
    /* by construction, all subgraphs are blocks and are non-empty */
    mm = g->meta_node;
    mg = mm->graph;
    for (me = agfstout(mg, mm); me; me = agnxtout(mg, me)) {
	block_t *block;

	clust_subg = agusergraph(me->head);

	isRoot = 0;
	block = mkBlock(clust_subg);
	/* block = makeBlock(g, state); */
	for (n = agfstnode(clust_subg); n; n = agnxtnode(clust_subg, n)) {
	    if (!BCDONE(n)) {	/* test not necessary if blocks disjoint */
		SET_BCDONE(n);
		BLOCK(n) = block;
		if (n == root)
		    isRoot = 1;
	    }
	}
	if (isRoot) {
	    /* Assume blocks are disjoint, so don't check if rootBlock is
	     * already assigned.
	     */
	    rootBlock = block;
	    insertBlock(&state->bl, block);
	} else {
	    appendBlock(&state->bl, block);
	}
    }
    ublks.first = state->bl.first;
    ublks.last = state->bl.last;
#endif

    if (!root)
	root = agfstnode(g);
    dfs(g, root, state, !rootBlock);

#ifdef USER_BLOCKS
    /* If g has user-supplied blocks, it may be disconnected.
     * We then fall into the following ugly loop.
     * We are guaranteed !VISITED(n) and PARENT(n) has been
     * set to a visited node.
     */
    if (ublks.first) {
	while (n = findUnvisited(&ublks)) {
	    dfs(g, n, state, 0);
	}
    }
#endif
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
