/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/


#include <ctype.h>
#include <setjmp.h>
#include "render.h"
#include "pack.h"

static jmp_buf jbuf;

#define MARKED(stk,n) ((stk)->markfn(n,-1))
#define MARK(stk,n)   ((stk)->markfn(n,1))
#define UNMARK(stk,n) ((stk)->markfn(n,0))

typedef struct blk_t {
    Agnode_t **data;
    Agnode_t **endp;
    struct blk_t *prev;
    struct blk_t *next;
} blk_t;

typedef struct {
    blk_t *fstblk;
    blk_t *curblk;
    Agnode_t **curp;
    void (*actionfn) (Agnode_t *, void *);
    int (*markfn) (Agnode_t *, int);
} stk_t;

#define INITBUF 1024
#define BIGBUF 1000000

static void initStk(stk_t* sp, blk_t* bp, Agnode_t** base, void (*actionfn) (Agnode_t *, void *),
     int (*markfn) (Agnode_t *, int))
{
    bp->data = base;
    bp->endp = bp->data + INITBUF;
    bp->prev = bp->next = NULL;
    sp->curblk = sp->fstblk = bp;
    sp->curp = sp->curblk->data;
    sp->actionfn = actionfn;
    sp->markfn = markfn;
}

static void freeBlk (blk_t* bp)
{
    free (bp->data);
    free (bp);
}

static void freeStk (stk_t* sp)
{
    blk_t* bp;
    blk_t* nxtbp;

    for (bp = sp->fstblk->next; bp; bp = nxtbp) {
	nxtbp = bp->next;
	freeBlk (bp);
    }
}

static void push(stk_t* sp, Agnode_t * np)
{
    if (sp->curp == sp->curblk->endp) {
	if (sp->curblk->next == NULL) {
	    blk_t *bp = GNEW(blk_t);
	    if (bp == 0) {
		agerr(AGERR, "gc: Out of memory\n");
		longjmp(jbuf, 1);
	    }
	    bp->prev = sp->curblk;
	    bp->next = NULL;
	    bp->data = N_GNEW(BIGBUF, Agnode_t *);
	    if (bp->data == 0) {
		agerr(AGERR, "gc: Out of memory\n");
		longjmp(jbuf, 1);
	    }
	    bp->endp = bp->data + BIGBUF;
	    sp->curblk->next = bp;
	}
	sp->curblk = sp->curblk->next;
	sp->curp = sp->curblk->data;
    }
    MARK(sp,np);
    *sp->curp++ = np;
}

static Agnode_t *pop(stk_t* sp)
{
    if (sp->curp == sp->curblk->data) {
	if (sp->curblk == sp->fstblk)
	    return 0;
	sp->curblk = sp->curblk->prev;
	sp->curp = sp->curblk->endp;
    }
    sp->curp--;
    return *sp->curp;
}


static int dfs(Agraph_t * g, Agnode_t * n, void *state, stk_t* stk)
{
    Agedge_t *e;
    Agnode_t *other;
    int cnt = 0;

    push (stk, n);
    while ((n = pop(stk))) {
	cnt++;
	if (stk->actionfn) stk->actionfn(n, state);
        for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	    if ((other = agtail(e)) == n)
		other = aghead(e);
            if (!MARKED(stk,other))
                push(stk, other);
        }
    }
    return cnt;
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
    agsubnode((Agraph_t *) state,n,1);
}

/* markFn:
 */
static int markFn (Agnode_t* n, int v)
{
    int ret;
    if (v < 0) return ND_mark(n);
    ret = ND_mark(n);
    ND_mark(n) = v;
    return ret;
}

/* setPrefix:
 */
static char*
setPrefix (char* pfx, int* lenp, char* buf, int buflen)
{
    int len;
    char* name;

    if (!pfx || !isLegal(pfx)) {
        pfx = "_cc_";
    }
    len = strlen(pfx);
    if (len + 25 <= buflen)
        name = buf;
    else {
        if (!(name = (char *) gmalloc(len + 25))) return NULL;
    }
    strcpy(name, pfx);
    *lenp = len;
    return name;
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
 * Return NULL on error or if graph is empty.
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
    stk_t stk;
    blk_t blk;
    Agnode_t* base[INITBUF];
    int error = 0;

    if (agnnodes(g) == 0) {
	*ncc = 0;
	return 0;
    }
    name = setPrefix (pfx, &len, buffer, SMALLBUF);

    ccs = N_GNEW(bnd, Agraph_t *);

    initStk (&stk, &blk, base, insertFn, markFn);
    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	UNMARK(&stk,n);

    if (setjmp(jbuf)) {
	error = 1;
	goto packerror;
    }
    /* Component with pinned nodes */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARKED(&stk,n) || !isPinned(n))
	    continue;
	if (!out) {
	    sprintf(name + len, "%d", c_cnt);
	    out = agsubg(g, name,1);
	    agbindrec(out, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);	//node custom data
	    ccs[c_cnt] = out;
	    c_cnt++;
	    pin = TRUE;
	}
	dfs (g, n, out, &stk);
    }

    /* Remaining nodes */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARKED(&stk,n))
	    continue;
	sprintf(name + len, "%d", c_cnt);
	out = agsubg(g, name,1);
	agbindrec(out, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);	//node custom data
	dfs(g, n, out, &stk);
	if (c_cnt == bnd) {
	    bnd *= 2;
	    ccs = RALLOC(bnd, ccs, Agraph_t *);
	}
	ccs[c_cnt] = out;
	c_cnt++;
    }
packerror:
    freeStk (&stk);
    if (name != buffer)
	free(name);
    if (error) {
	int i;
	*ncc = 0;
	for (i=0; i < c_cnt; i++) {
	    agclose (ccs[i]);
	}
	free (ccs);
	ccs = NULL;
    }
    else {
	ccs = RALLOC(c_cnt, ccs, Agraph_t *);
	*ncc = c_cnt;
	*pinned = pin;
    }
    return ccs;
}

/* ccomps:
 * Return an array of subgraphs consisting of the connected
 * components of graph g. The number of components is returned in ncc.
 * If pfx is non-null and a legal graph name, we use it as the prefix
 * for the name of the subgraphs created. If not, a simple default is used.
 * Note that the component subgraphs do not contain any edges. These must
 * be obtained from the root graph.
 * Returns NULL on error or if graph is empty.
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
    stk_t stk;
    blk_t blk;
    Agnode_t* base[INITBUF];

    if (agnnodes(g) == 0) {
	*ncc = 0;
	return 0;
    }
    name = setPrefix (pfx, &len, buffer, SMALLBUF);

    ccs = N_GNEW(bnd, Agraph_t *);
    initStk (&stk, &blk, base, insertFn, markFn);
    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	UNMARK(&stk,n);

    if (setjmp(jbuf)) {
	freeStk (&stk);
	free (ccs);
	if (name != buffer)
	    free(name);
	*ncc = 0;
	return NULL;
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (MARKED(&stk,n))
	    continue;
	sprintf(name + len, "%d", c_cnt);
	out = agsubg(g, name,1);
	agbindrec(out, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);	//node custom data
	dfs(g, n, out, &stk);
	if (c_cnt == bnd) {
	    bnd *= 2;
	    ccs = RALLOC(bnd, ccs, Agraph_t *);
	}
	ccs[c_cnt] = out;
	c_cnt++;
    }
    freeStk (&stk);
    ccs = RALLOC(c_cnt, ccs, Agraph_t *);
    if (name != buffer)
	free(name);
    *ncc = c_cnt;
    return ccs;
}

typedef struct {
    Agrec_t h;
    char cc_subg;   /* true iff subgraph corresponds to a component */
} ccgraphinfo_t;

typedef struct {
    Agrec_t h;
    char mark;
    union {
	Agraph_t* g;
	Agnode_t* n;
	void*     v;
    } ptr;
} ccgnodeinfo_t;

#define GRECNAME "ccgraphinfo"
#define NRECNAME "ccgnodeinfo"
#define GD_cc_subg(g)  (((ccgraphinfo_t*)aggetrec(g, GRECNAME, FALSE))->cc_subg)
#ifdef DEBUG
Agnode_t*
dnodeOf (Agnode_t* v)
{
  ccgnodeinfo_t* ip = (ccgnodeinfo_t*)aggetrec(v, NRECNAME, FALSE);
  if (ip)
    return ip->ptr.n;
  fprintf (stderr, "nodeinfo undefined\n");
  return 0;
}
void
dnodeSet (Agnode_t* v, Agnode_t* n)
{
  ((ccgnodeinfo_t*)aggetrec(v, NRECNAME, FALSE))->ptr.n = n;
}
#else
#define dnodeOf(v)  (((ccgnodeinfo_t*)aggetrec(v, NRECNAME, FALSE))->ptr.n)
#define dnodeSet(v,w) (((ccgnodeinfo_t*)aggetrec(v, NRECNAME, FALSE))->ptr.n=w)
#endif

#define ptrOf(np)  (((ccgnodeinfo_t*)((np)->base.data))->ptr.v)
#define nodeOf(np)  (((ccgnodeinfo_t*)((np)->base.data))->ptr.n)
#define clustOf(np)  (((ccgnodeinfo_t*)((np)->base.data))->ptr.g)
#define clMark(n) (((ccgnodeinfo_t*)(n->base.data))->mark)

/* isCluster:
 * Return true if graph is a cluster
 */
#define isCluster(g) (strncmp(agnameof(g), "cluster", 7) == 0)

/* deriveClusters:
 * Construct nodes in derived graph corresponding top-level clusters.
 * Since a cluster might be wrapped in a subgraph, we need to traverse
 * down into the tree of subgraphs
 */
static void deriveClusters(Agraph_t* dg, Agraph_t * g)
{
    Agraph_t *subg;
    Agnode_t *dn;
    Agnode_t *n;

    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	if (isCluster(subg)) {
	    dn = agnode(dg, agnameof(subg), 1);
	    agbindrec (dn, NRECNAME, sizeof(ccgnodeinfo_t), TRUE);
	    clustOf(dn) = subg;
	    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {
		if (dnodeOf(n)) {
		   fprintf (stderr, "Error: node \"%s\" belongs to two non-nested clusters \"%s\" and \"%s\"\n",
			agnameof (n), agnameof(subg), agnameof(dnodeOf(n)));  
		}
		dnodeSet(n,dn);
	    }
	}
	else {
	    deriveClusters (dg, subg);
	}
    }
}

/* deriveGraph:
 * Create derived graph dg of g where nodes correspond to top-level nodes 
 * or clusters, and there is an edge in dg if there is an edge in g
 * between any nodes in the respective clusters.
 */
static Agraph_t *deriveGraph(Agraph_t * g)
{
    Agraph_t *dg;
    Agnode_t *dn;
    Agnode_t *n;

    dg = agopen("dg", Agstrictundirected, (Agdisc_t *) 0);

    deriveClusters (dg, g);

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (dnodeOf(n))
	    continue;
	dn = agnode(dg, agnameof(n), 1);
	agbindrec (dn, NRECNAME, sizeof(ccgnodeinfo_t), TRUE);
	nodeOf(dn) = n;
	dnodeSet(n,dn);
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	Agedge_t *e;
	Agnode_t *hd;
	Agnode_t *tl = dnodeOf(n);
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    hd = aghead(e);
	    hd = dnodeOf(hd);
	    if (hd == tl)
		continue;
	    if (hd > tl)
		agedge(dg, tl, hd, 0, 1);
	    else
		agedge(dg, hd, tl, 0, 1);
	}
    }

    return dg;
}

/* unionNodes:
 * Add all nodes in cluster nodes of dg to g
 */
static void unionNodes(Agraph_t * dg, Agraph_t * g)
{
    Agnode_t *n;
    Agnode_t *dn;
    Agraph_t *clust;

    for (dn = agfstnode(dg); dn; dn = agnxtnode(dg, dn)) {
	if (AGTYPE(ptrOf(dn)) == AGNODE) {
	    agsubnode(g, nodeOf(dn), 1);
	} else {
	    clust = clustOf(dn);
	    for (n = agfstnode(clust); n; n = agnxtnode(clust, n))
		agsubnode(g, n, 1);
	}
    }
}

/* clMarkFn:
 */
static int clMarkFn (Agnode_t* n, int v)
{
    int ret;
    if (v < 0) return clMark(n);
    ret = clMark(n);
    clMark(n) = v;
    return ret;
}

/* node_induce:
 * Using the edge set of eg, add to g any edges
 * with both endpoints in g.
 * Returns the number of edges added.
 */
int node_induce(Agraph_t * g, Agraph_t* eg)
{
    Agnode_t *n;
    Agedge_t *e;
    int e_cnt = 0;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(eg, n); e; e = agnxtout(eg, e)) {
	    if (agsubnode(g, aghead(e),0)) {
		agsubedge(g,e,1);
		e_cnt++;
	    }
	}
    }
    return e_cnt;
}

/* projectG:
 * If any nodes of subg are in g, create a subgraph of g
 * and fill it with all nodes of subg in g and their induced
 * edges in subg. Copy the attributes of subg to g. Return the subgraph.
 * If not, return null.
 */
static Agraph_t *projectG(Agraph_t * subg, Agraph_t * g, int inCluster)
{
    Agraph_t *proj = 0;
    Agnode_t *n;
    Agnode_t *m;

    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {
	if ((m = agfindnode(g, agnameof(n)))) {
	    if (proj == 0) {
		proj = agsubg(g, agnameof(subg), 1);
	    }
	    agsubnode(proj, m, 1);
	}
    }
    if (!proj && inCluster) {
	proj = agsubg(g, agnameof(subg), 1);
    }
    if (proj) {
	node_induce(proj, subg);
	agcopyattr(subg, proj);
    }

    return proj;
}

/* subgInduce:
 * Project subgraphs of root graph on subgraph.
 * If non-empty, add to subgraph.
 */
static void
subgInduce(Agraph_t * root, Agraph_t * g, int inCluster)
{
    Agraph_t *subg;
    Agraph_t *proj;
    int in_cluster;

/* fprintf (stderr, "subgInduce %s inCluster %d\n", agnameof(root), inCluster); */
    for (subg = agfstsubg(root); subg; subg = agnxtsubg(subg)) {
	if (GD_cc_subg(subg))
	    continue;
	if ((proj = projectG(subg, g, inCluster))) {
	    in_cluster = (inCluster || isCluster(subg));
	    subgInduce(subg, proj, in_cluster);
	}
    }
}

static void
subGInduce(Agraph_t* g, Agraph_t * out)
{
    subgInduce(g, out, 0);
}

Agraph_t **cccomps(Agraph_t * g, int *ncc, char *pfx)
{
    Agraph_t *dg;
    long n_cnt, c_cnt, e_cnt;
    char *name;
    Agraph_t *out;
    Agraph_t *dout;
    Agnode_t *dn;
    char buffer[SMALLBUF];
    Agraph_t **ccs;
    stk_t stk;
    blk_t blk;
    Agnode_t* base[INITBUF];
    int len, sz = sizeof(ccgraphinfo_t);

    if (agnnodes(g) == 0) {
	*ncc = 0;
	return 0;
    }
    
    /* Bind ccgraphinfo to graph and all subgraphs */
    aginit(g, AGRAPH, GRECNAME, -sz, FALSE);

    /* Bind ccgraphinfo to graph and all subgraphs */
    aginit(g, AGNODE, NRECNAME, sizeof(ccgnodeinfo_t), FALSE);

    name = setPrefix (pfx, &len, buffer, SMALLBUF);

    dg = deriveGraph(g);

    ccs = N_GNEW(agnnodes(dg), Agraph_t *);
    initStk (&stk, &blk, base, insertFn, clMarkFn);

    c_cnt = 0;
    for (dn = agfstnode(dg); dn; dn = agnxtnode(dg, dn)) {
	if (MARKED(&stk,dn))
	    continue;
	sprintf(name + len, "%ld", c_cnt);
	dout = agsubg(dg, name, 1);
	out = agsubg(g, name, 1);
	agbindrec(out, GRECNAME, sizeof(ccgraphinfo_t), FALSE);
	GD_cc_subg(out) = 1;
	n_cnt = dfs(dg, dn, dout, &stk);
	unionNodes(dout, out);
	e_cnt = nodeInduce(out);
	subGInduce(g, out);
	ccs[c_cnt] = out;
	agdelete(dg, dout);
	if (Verbose)
	    fprintf(stderr, "(%4ld) %7ld nodes %7ld edges\n",
		    c_cnt, n_cnt, e_cnt);
	c_cnt++;
    }

    if (Verbose)
	fprintf(stderr, "       %7d nodes %7d edges %7ld components %s\n",
	    agnnodes(g), agnedges(g), c_cnt, agnameof(g));

    agclose(dg);
    agclean (g, AGRAPH, GRECNAME);
    agclean (g, AGNODE, NRECNAME);
    freeStk (&stk);
    ccs = RALLOC(c_cnt, ccs, Agraph_t *);
    if (name != buffer)
	free(name);
    *ncc = c_cnt;
    return ccs;
}

/* isConnected:
 * Returns 1 if the graph is connected.
 * Returns 0 if the graph is not connected.
 * Returns -1 if the graph is error.
 */
int isConnected(Agraph_t * g)
{
    Agnode_t *n;
    int ret = 1;
    int cnt = 0;
    stk_t stk;
    blk_t blk;
    Agnode_t* base[INITBUF];

    if (agnnodes(g) == 0)
	return 1;

    initStk (&stk, &blk, base, NULL, markFn);
    for (n = agfstnode(g); n; n = agnxtnode(g, n))
	UNMARK(&stk,n);

    if (setjmp(jbuf)) {
	freeStk (&stk);
	return -1;
    }

    n = agfstnode(g);
    cnt = dfs(g, agfstnode(g), NULL, &stk);
    if (cnt != agnnodes(g))
	ret = 0;
    freeStk (&stk);
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
    return node_induce (g, g->root);
}
