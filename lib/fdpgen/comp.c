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


/* comp.c:
 * Written by Emden R. Gansner
 *
 * Support for "connected components". Components are either connected
 * or have a port node or have a pinned node.
 *
 */

/* use PRIVATE interface */
#define FDP_PRIVATE 1

#include <fdp.h>
#include <comp.h>
#include <pack.h>
#include <assert.h>

#define MARK(n) (marks[ND_id(n)])

static void dfs(Agraph_t * g, Agnode_t * n, Agraph_t * out, char *marks)
{
    Agedge_t *e;
    Agnode_t *other;

    MARK(n) = 1;
    aginsert(out, n);
    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	if ((other = e->tail) == n)
	    other = e->head;
	if (!MARK(other))
	    dfs(g, other, out, marks);
    }
}

/* findCComp:
 * Finds generalized connected components of graph g.
 * This merges all components containing a port node or a pinned node.
 * Assumes nodes have unique id's in range [0,agnnodes(g)-1].
 * Components are stored as subgraphs of g, with name sg_<i>.
 * Returns 0-terminated array of components.
 * If cnt is non-0, count of components is stored there.
 * If pinned is non-0, *pinned is set to 1 if there are pinned nodes.
 * Note that if ports and/or pinned nodes exists, they will all be
 * in the first component returned by findCComp.
 */
static int C_cnt = 0;
graph_t **findCComp(graph_t * g, int *cnt, int *pinned)
{
    node_t *n;
    graph_t *subg;
    char name[128];
    int c_cnt = 0;
    char *marks;
    bport_t *pp;
    graph_t **comps;
    graph_t **cp;
    graph_t *mg;
    edge_t *me;
    node_t *mn;
    int pinflag = 0;

/* fprintf (stderr, "comps of %s starting at %d \n", g->name, c_cnt); */
    marks = N_NEW(agnnodes(g), char);	/* freed below */

    /* Create component based on port nodes */
    subg = 0;
    if ((pp = PORTS(g))) {
	sprintf(name, "cc%s_%d", g->name, c_cnt++ + C_cnt);
	subg = agsubg(g, name);
	GD_alg(subg) = (void *) NEW(gdata);
	PORTS(subg) = pp;
	NPORTS(subg) = NPORTS(g);
	for (; pp->n; pp++) {
	    if (MARK(pp->n))
		continue;
	    dfs(g, pp->n, subg, marks);
	}
    }

    /* Create/extend component based on pinned nodes */
    /* Note that ports cannot be pinned */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARK(n))
	    continue;
	if (ND_pinned(n) != P_PIN)
	    continue;
	if (!subg) {
	    sprintf(name, "cc%s_%d", g->name, c_cnt++ + C_cnt);
	    subg = agsubg(g, name);
	    GD_alg(subg) = (void *) NEW(gdata);
	}
	pinflag = 1;
	dfs(g, n, subg, marks);
    }
    if (subg)
	nodeInduce(subg);

    /* Pick up remaining components */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARK(n))
	    continue;
	sprintf(name, "cc%s+%d", g->name, c_cnt++ + C_cnt);
	subg = agsubg(g, name);
	GD_alg(subg) = (void *) NEW(gdata);
	dfs(g, n, subg, marks);
	nodeInduce(subg);
    }
    free(marks);
    C_cnt += c_cnt;

    if (cnt)
	*cnt = c_cnt;
    if (pinned)
	*pinned = pinflag;
    /* freed in layout */
    comps = cp = N_NEW(c_cnt + 1, graph_t *);
    mg = g->meta_node->graph;
    for (me = agfstout(mg, g->meta_node); me; me = agnxtout(mg, me)) {
	mn = me->head;
	*cp++ = agusergraph(mn);
	c_cnt--;
    }
    assert(c_cnt == 0);
    *cp = 0;

    return comps;
}
