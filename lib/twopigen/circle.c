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


#include    "circle.h"
#define DEF_RANKSEP 1.00
#define UNSET 10.00

/* dfs to set distance from a particular leaf.
 * Note that termination is implicit in the test
 * for reduced number of steps. Proof?
 */
static void setNStepsToLeaf(Agraph_t * g, Agnode_t * n, Agnode_t * prev)
{
    Agnode_t *next;
    Agedge_t *ep;
    int nsteps = SLEAF(n) + 1;

    for (ep = agfstedge(g, n); ep; ep = agnxtedge(g, ep, n)) {
	if ((next = ep->tail) == n)
	    next = ep->head;

	if (prev == next)
	    continue;

	if (nsteps < SLEAF(next)) {	/* handles loops and multiedges */
	    SLEAF(next) = nsteps;
	    setNStepsToLeaf(g, next, n);
	}
    }
}

/* isLeaf:
 * Return true if n is a leaf node.
 */
static int isLeaf(Agraph_t * g, Agnode_t * n)
{
    Agedge_t *ep;
    Agnode_t *neighp = 0;
    Agnode_t *np;

    for (ep = agfstedge(g, n); ep; ep = agnxtedge(g, ep, n)) {
	if ((np = ep->tail) == n)
	    np = ep->head;
	if (n == np)
	    continue;		/* loop */
	if (neighp) {
	    if (neighp != np)
		return 0;	/* two different neighbors */
	} else
	    neighp = np;
    }
    return 1;
}

static void initLayout(Agraph_t * g)
{
    Agnode_t *n;
    int nnodes = agnnodes(g);
    int INF = nnodes * nnodes;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	/* STSIZE(n) = 0; */
	/* NCHILD(n) = 0; */
	SCENTER(n) = INF;
	THETA(n) = UNSET;	/* marks theta as unset, since 0 <= theta <= 2PI */
	if (isLeaf(g, n))
	    SLEAF(n) = 0;
	else
	    SLEAF(n) = INF;
    }
}

/*
 * Working recursively in from each leaf node (ie, each node
 * with nStepsToLeaf == 0; see initLayout), set the
 * minimum value of nStepsToLeaf for each node.  Using
 * that information, assign some node to be the centerNode.
*/
static Agnode_t *findCenterNode(Agraph_t * g)
{
    Agnode_t *n;
    Agnode_t *center = NULL;
    int maxNStepsToLeaf = 0;

    /* With just 1 or 2 nodes, return anything. */
    if (agnnodes(g) <= 2)
	return (agfstnode(g));

    /* dfs from each leaf node */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (SLEAF(n) == 0)
	    setNStepsToLeaf(g, n, 0);
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (SLEAF(n) > maxNStepsToLeaf) {
	    maxNStepsToLeaf = SLEAF(n);
	    center = n;
	}
    }
    return center;
}

/* dfs to set distance from center
 * Note that termination is implicit in the test
 * for reduced number of steps. Proof?
 */
static void setNStepsToCenter(Agraph_t * g, Agnode_t * n, Agnode_t * prev)
{
    Agnode_t *next;
    Agedge_t *ep;
    int nsteps = SCENTER(n) + 1;

    for (ep = agfstedge(g, n); ep; ep = agnxtedge(g, ep, n)) {
	if ((next = ep->tail) == n)
	    next = ep->head;

	if (prev == next)
	    continue;

	if (nsteps < SCENTER(next)) {	/* handles loops and multiedges */
	    SCENTER(next) = nsteps;
	    if (SPARENT(next))
		NCHILD(SPARENT(next))--;
	    SPARENT(next) = n;
	    NCHILD(n)++;
	    setNStepsToCenter(g, next, n);
	}
    }
}


/*
 * Work out from the center and determine the value of
 * nStepsToCenter and parent node for each node.
 */
static int setParentNodes(Agraph_t * sg, Agnode_t * center)
{
    Agnode_t *n;
    int maxn = 0;

    SCENTER(center) = 0;
    SPARENT(center) = 0;
    setNStepsToCenter(sg, center, 0);

    /* find the maximum number of steps from the center */
    for (n = agfstnode(sg); n; n = agnxtnode(sg, n)) {
	if (SCENTER(n) > maxn) {
	    maxn = SCENTER(n);
	}
    }
    return maxn;
}

/* Sets each node's subtreeSize, which counts the number of 
 * leaves in subtree rooted at the node.
 * At present, this is done bottom-up.
 */
static void setSubtreeSize(Agraph_t * g)
{
    Agnode_t *n;
    Agnode_t *parent;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (NCHILD(n) > 0)
	    continue;
	STSIZE(n)++;
	parent = SPARENT(n);
	while (parent) {
	    STSIZE(parent)++;
	    parent = SPARENT(parent);
	}
    }
}

static void setChildSubtreeSpans(Agraph_t * g, Agnode_t * n)
{
    Agedge_t *ep;
    Agnode_t *next;
    double ratio;

    ratio = SPAN(n) / STSIZE(n);
    for (ep = agfstedge(g, n); ep; ep = agnxtedge(g, ep, n)) {
	if ((next = ep->tail) == n)
	    next = ep->head;
	if (SPARENT(next) != n)
	    continue;		/* handles loops */

	if (SPAN(next) != 0.0)
	    continue;		/* multiedges */
	(SPAN(next) = ratio * STSIZE(next));

	if (NCHILD(next) > 0) {
	    setChildSubtreeSpans(g, next);
	}
    }
}

static void setSubtreeSpans(Agraph_t * sg, Agnode_t * center)
{
    SPAN(center) = 2 * PI;
    setChildSubtreeSpans(sg, center);
}

 /* Set the node positions for the 2nd and later rings. */
static void setChildPositions(Agraph_t * sg, Agnode_t * n)
{
    Agnode_t *next;
    Agedge_t *ep;
    double theta;		/* theta is the lower boundary radius of the fan */

    if (SPARENT(n) == 0)	/* center */
	theta = 0;
    else
	theta = THETA(n) - SPAN(n) / 2;

    for (ep = agfstedge(sg, n); ep; ep = agnxtedge(sg, ep, n)) {
	if ((next = ep->tail) == n)
	    next = ep->head;
	if (SPARENT(next) != n)
	    continue;		/* handles loops */
	if (THETA(next) != UNSET)
	    continue;		/* handles multiedges */

	THETA(next) = theta + SPAN(next) / 2.0;
	theta += SPAN(next);

	if (NCHILD(next) > 0)
	    setChildPositions(sg, next);
    }
}

static void setPositions(Agraph_t * sg, Agnode_t * center)
{
    THETA(center) = 0;
    setChildPositions(sg, center);
}

static void setAbsolutePos(Agraph_t * g)
{
    char *p;
    Agnode_t *n;
    double xf;
    double hyp;

    p = late_string(g, agfindattr(g->root, "ranksep"), NULL);
    if (p) {
	if (sscanf(p, "%lf", &xf) == 0)
	    xf = DEF_RANKSEP;
	else {
	    if (xf < MIN_RANKSEP)
		xf = MIN_RANKSEP;
	}
    } else
	xf = DEF_RANKSEP;
    if (Verbose)
	fprintf(stderr, "Rank separation = %f\n", xf);

    /* Convert circular to cartesian coordinates */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	hyp = xf * (SCENTER(n));
	ND_pos(n)[0] = hyp * cos(THETA(n));
	ND_pos(n)[1] = hyp * sin(THETA(n));
    }
}

#if 0				/* not used */
static void dumpGraph(Agraph_t * g)
{
    Agnode_t *n;
    char *p;

    fprintf(stderr,
	    "     :  leaf  stsz nkids  cntr parent   span  theta\n");
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (SPARENT(n))
	    p = SPARENT(n)->name;
	else
	    p = "<C>";
	fprintf(stderr, "%4s :%6d%6d%6d%6d%7s%7.3f%7.3f%8.3f%8.3f\n",
		n->name, SLEAF(n), STSIZE(n), NCHILD(n),
		SCENTER(n), p, SPAN(n), THETA(n), ND_pos(n)[0],
		ND_pos(n)[1]);
    }
}
#endif

/* circleLayout:
 *  We assume sg is is connected and non-empty.
 *  Also, if center != 0, we are guaranteed that center is
 *  in the graph.
 */
void circleLayout(Agraph_t * sg, Agnode_t * center)
{
    /* int maxNStepsToCenter; */

    if (agnnodes(sg) == 1) {
	Agnode_t *n = agfstnode(sg);
	ND_pos(n)[0] = 0;
	ND_pos(n)[1] = 0;
	return;
    }

    initLayout(sg);

    if (!center)
	center = findCenterNode(sg);
    if (Verbose)
	fprintf(stderr, "Center = %s\n", center->name);

    /* maxNStepsToCenter = setParentNodes(sg,center); */
    setParentNodes(sg, center);

    setSubtreeSize(sg);

    setSubtreeSpans(sg, center);

    setPositions(sg, center);

    setAbsolutePos(sg);
    /* dumpGraph (sg); */
}
