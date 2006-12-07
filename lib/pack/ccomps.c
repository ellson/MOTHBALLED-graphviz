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


#include <render.h>
#include <pack.h>
#include <ctype.h>

#define MARKED(n) ((n)->u.mark)
#define MARK(n) ((n)->u.mark = 1)
#define UNMARK(n) ((n)->u.mark = 0)

typedef void (*dfsfn) (Agnode_t *, void *);

static void dfs(Agraph_t * g, Agnode_t * n, dfsfn action, void *state)
{
    Agedge_t *e;
    Agnode_t *other;

    MARK(n);
    action(n, state);
    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	if ((other = e->tail) == n)
	    other = e->head;
	if (!MARKED(other))
	    dfs(g, other, action, state);
    }
}

static int isLegal(char *p)
{
    unsigned char c;

    while ((c = *(unsigned char *) p++)) {
	if ((c != '_') && !isalnum(c))
	    return 0;
    }

    return 1;
}

/* insertFn:
 */
static void insertFn(Agnode_t * n, void *state)
{
    aginsert((Agraph_t *) state, n);
}

/* pccomps:
 * Return an array of subgraphs consisting of the connected 
 * components of graph g. The number of components is returned in ncc. 
 * All pinned nodes are in one component.
 * If pfx is non-null and a legal graph name, we use it as the prefix
 * for the name of the subgraphs created. If not, a simple default is used.
 * If pinned is non-null, *pinned set to 1 if pinned nodes found
 * and the first component is the one containing the pinned nodes.
 * Note that the component subgraphs do not contain any edges. These must
 * be obtained from the root graph.
 */
Agraph_t **pccomps(Agraph_t * g, int *ncc, char *pfx, boolean * pinned)
{
    int c_cnt = 0;
    char buffer[SMALLBUF];
    char *name;
    Agraph_t *out = 0;
    Agnode_t *n;
    Agraph_t **ccs;
    int len;
    int bnd = 10;
    boolean pin = FALSE;

    if (agnnodes(g) == 0) {
	*ncc = 0;
	return 0;
    }
    if (!pfx || !isLegal(pfx)) {
	pfx = "_cc_";
    }
    len = strlen(pfx);
    if (len + 25 <= SMALLBUF)
	name = buffer;
    else
	name = (char *) gmalloc(len + 25);
    strcpy(name, pfx);

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	UNMARK(n);

    ccs = N_GNEW(bnd, Agraph_t *);

    /* Component with pinned nodes */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARKED(n) || !isPinned(n))
	    continue;
	if (!out) {
	    sprintf(name + len, "%d", c_cnt);
	    out = agsubg(g, name);
	    ccs[c_cnt] = out;
	    c_cnt++;
	    pin = TRUE;
	}
	dfs(g, n, insertFn, out);
    }

    /* Remaining nodes */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARKED(n))
	    continue;
	sprintf(name + len, "%d", c_cnt);
	out = agsubg(g, name);
	dfs(g, n, insertFn, out);
	if (c_cnt == bnd) {
	    bnd *= 2;
	    ccs = RALLOC(bnd, ccs, Agraph_t *);
	}
	ccs[c_cnt] = out;
	c_cnt++;
    }

    ccs = RALLOC(c_cnt, ccs, Agraph_t *);
    if (name != buffer)
	free(name);
    *ncc = c_cnt;
    *pinned = pin;
    return ccs;
}

/* ccomps:
 * Return an array of subgraphs consisting of the connected
 * components of graph g. The number of components is returned in ncc.
 * If pfx is non-null and a legal graph name, we use it as the prefix
 * for the name of the subgraphs created. If not, a simple default is used.
 * Note that the component subgraphs do not contain any edges. These must
 * be obtained from the root graph.
 */
Agraph_t **ccomps(Agraph_t * g, int *ncc, char *pfx)
{
    int c_cnt = 0;
    char buffer[SMALLBUF];
    char *name;
    Agraph_t *out;
    Agnode_t *n;
    Agraph_t **ccs;
    int len;
    int bnd = 10;

    if (agnnodes(g) == 0) {
	*ncc = 0;
	return 0;
    }
    if (!pfx || !isLegal(pfx)) {
	pfx = "_cc_";
    }
    len = strlen(pfx);
    if (len + 25 <= SMALLBUF)
	name = buffer;
    else
	name = (char *) gmalloc(len + 25);
    strcpy(name, pfx);

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	UNMARK(n);

    ccs = N_GNEW(bnd, Agraph_t *);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARKED(n))
	    continue;
	sprintf(name + len, "%d", c_cnt);
	out = agsubg(g, name);
	dfs(g, n, insertFn, out);
	if (c_cnt == bnd) {
	    bnd *= 2;
	    ccs = RALLOC(bnd, ccs, Agraph_t *);
	}
	ccs[c_cnt] = out;
	c_cnt++;
    }
    ccs = RALLOC(c_cnt, ccs, Agraph_t *);
    if (name != buffer)
	free(name);
    *ncc = c_cnt;
    return ccs;
}

/* cntFn:
 */
static void cntFn(Agnode_t * n, void *s)
{
    *(int *) s += 1;
}

/* isConnected:
 * Returns true if the graph is connected.
 */
int isConnected(Agraph_t * g)
{
    Agnode_t *n;
    int ret = 1;
    int cnt = 0;

    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	UNMARK(n);

    n = agfstnode(g);
    if (n) {
	dfs(g, n, cntFn, &cnt);
	if (cnt != agnnodes(g))
	    ret = 0;
    }
    return ret;
}

/* nodeInduce:
 * Given a subgraph, adds all edges in the root graph both of whose
 * endpoints are in the subgraph.
 * If g is a connected component, this will be all edges attached to
 * any node in g.
 * Returns the number of edges added.
 */
int nodeInduce(Agraph_t * g)
{
    Agnode_t *n;
    Agraph_t *root = g->root;
    Agedge_t *e;
    int e_cnt = 0;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(root, n); e; e = agnxtout(root, e)) {
	    if (agcontains(g, e->head)) {	/* test will always be true */
		aginsert(g, e);	/* for connected component  */
		e_cnt++;
	    }
	}
    }
    return e_cnt;
}
