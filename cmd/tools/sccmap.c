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


/*
 * Written by Stephen North
 * Updated by Emden Gansner
 */

/*
 * This is a filter that reads a graph, searches for strongly
 * connected components, and writes each as a separate graph
 * along with a map of the components.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include	<unistd.h>
#endif
#include <agraph.h>
#include <ingraphs.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

#define INF ((unsigned int)(-1))

typedef struct Agraphinfo_t {
    Agrec_t h;
    Agnode_t *rep;
} Agraphinfo_t;

typedef struct Agnodeinfo_t {
    Agrec_t h;
    unsigned int val;
    Agraph_t *scc;
} Agnodeinfo_t;

#ifdef INLINE
#define getrep(g)  (((Agraphinfo_t*)(g->base.data))->rep)
#define setrep(g,rep)  (getrep(g) = rep)
#define getscc(n)    (((Agnodeinfo_t*)((n)->base.data))->scc)
#define setscc(n,sub)    (getscc(n) = sub)
#define getval(n)    (((Agnodeinfo_t*)((n)->base.data))->val)
#define setval(n,newval)    (getval(n) = newval)
#else
static Agnode_t *getrep(Agraph_t * g)
{
    return (((Agraphinfo_t *) (g->base.data))->rep);
}
static void setrep(Agraph_t * g, Agnode_t * rep)
{
    ((Agraphinfo_t *) (g->base.data))->rep = rep;
}
static Agraph_t *getscc(Agnode_t * n)
{
    return (((Agnodeinfo_t *) (n->base.data))->scc);
}
static void setscc(Agnode_t * n, Agraph_t * scc)
{
    ((Agnodeinfo_t *) (n->base.data))->scc = scc;
}
static int getval(Agnode_t * n)
{
    return (((Agnodeinfo_t *) (n->base.data))->val);
}
static void setval(Agnode_t * n, int v)
{
    ((Agnodeinfo_t *) (n->base.data))->val = v;
}
#endif

/********* stack ***********/
typedef struct {
    Agnode_t **data;
    Agnode_t **ptr;
} Stack;

static void initStack(Stack * sp, int sz)
{
    sp->data = (Agnode_t **) malloc(sz * sizeof(Agnode_t *));
    sp->ptr = sp->data;
}

static void freeStack(Stack * sp)
{
    free(sp->data);
}

#ifdef INLINE
#define push(sp,n) (*((sp)->ptr++) = n)
#define top(sp) (*((sp)->ptr - 1))
#define pop(sp) (*(--((sp)->ptr)))
#else
static void push(Stack * sp, Agnode_t * n)
{
    *(sp->ptr++) = n;
}

static Agnode_t *top(Stack * sp)
{
    return *(sp->ptr - 1);
}

static Agnode_t *pop(Stack * sp)
{
    sp->ptr--;
    return *(sp->ptr);
}
#endif


/********* end stack ***********/

typedef struct {
    int Comp;
    int ID;
    int N_nodes_in_nontriv_SCC;
} sccstate;

int wantDegenerateComp;
int Silent;
int Verbose;
char *CmdName;
char **Files;

static void nodeInduce(Agraph_t * g)
{
    Agnode_t *n, *rootn;
    Agedge_t *e;

    for (n = agfstnode(g); n; n = agnxtnode(n)) {
	rootn = agsubnode(agroot(g), n, FALSE);
	for (e = agfstout(rootn); e; e = agnxtout(e)) {
	    if (agsubnode(g, aghead(e), FALSE))
		agsubedge(g, e, TRUE);
	    else {
		if (getscc(aghead(e)) && getscc(agtail(e)))
		    agedge(getrep(getscc(agtail(e))),
			   getrep(getscc(aghead(e))), NIL(char *), TRUE);
	    }
	}
    }
}

static int visit(Agnode_t * n, Agraph_t * map, Stack * sp, sccstate * st)
{
    unsigned int m, min;
    Agnode_t *t;
    Agraph_t *subg;
    Agedge_t *e;

    min = ++(st->ID);
    setval(n, min);
    push(sp, n);

    for (e = agfstout(n); e; e = agnxtout(e)) {
	t = aghead(e);
	if (getval(t) == 0)
	    m = visit(t, map, sp, st);
	else
	    m = getval(t);
	if (m < min)
	    min = m;
    }

    if (getval(n) == min) {
	if (!wantDegenerateComp && (top(sp) == n)) {
	    setval(n, INF);
	    pop(sp);
	} else {
	    char name[32];
	    Agraph_t *G = agraphof(n);;
	    sprintf(name, "cluster_%d", (st->Comp)++);
	    subg = agsubg(G, name, TRUE);
	    agbindrec(subg, "scc_graph", sizeof(Agraphinfo_t), TRUE);
	    setrep(subg, agnode(map, name, TRUE));
	    do {
		t = pop(sp);
		agsubnode(subg, t, TRUE);
		setval(t, INF);
		setscc(t, subg);
		st->N_nodes_in_nontriv_SCC++;
	    } while (t != n);
	    nodeInduce(subg);
	    if (!Silent)
		agwrite(subg, stdout);
	}
    }
    return min;
}

static int label(Agnode_t * n, int nodecnt, int *edgecnt)
{
    Agedge_t *e;

    setval(n, 1);
    nodecnt++;
    for (e = agfstedge(n); e; e = agnxtedge(e, n)) {
	(*edgecnt) += 1;
	if (e->node == n)
	    e = agopp(e);
	if (!getval(e->node))
	    nodecnt = label(e->node, nodecnt, edgecnt);
    }
    return nodecnt;
}

static int
countComponents(Agraph_t * g, int *max_degree, float *nontree_frac)
{
    int nc = 0;
    int sum_edges = 0;
    int sum_nontree = 0;
    int deg;
    int n_edges;
    int n_nodes;
    Agnode_t *n;

    for (n = agfstnode(g); n; n = agnxtnode(n)) {
	if (!getval(n)) {
	    nc++;
	    n_edges = 0;
	    n_nodes = label(n, 0, &n_edges);
	    sum_edges += n_edges;
	    sum_nontree += (n_edges - n_nodes + 1);
	}
    }
    if (max_degree) {
	int maxd = 0;
	for (n = agfstnode(g); n; n = agnxtnode(n)) {
	    deg = agdegree(n, TRUE, TRUE);
	    if (maxd < deg)
		maxd = deg;
	    setval(n, 0);
	}
	*max_degree = maxd;
    }
    if (nontree_frac) {
	if (sum_edges > 0)
	    *nontree_frac = (float) sum_nontree / (float) sum_edges;
	else
	    *nontree_frac = 0.0;
    }
    return nc;
}

static void process(Agraph_t * G)
{
    Agnode_t *n;
    Agraph_t *map;
    int nc = 0;
    float nontree_frac = 0;
    int Maxdegree = 0;
    Stack stack;
    sccstate state;

    aginit(G, AGRAPH, "scc_graph", sizeof(Agraphinfo_t), TRUE);
    aginit(G, AGNODE, "scc_node", sizeof(Agnodeinfo_t), TRUE);
    state.Comp = state.ID = 0;
    state.N_nodes_in_nontriv_SCC = 0;

    if (Verbose)
	nc = countComponents(G, &Maxdegree, &nontree_frac);

    initStack(&stack, agnnodes(G) + 1);
    map = agopen("scc_map", Agdirected, (Agdisc_t *) 0);
    for (n = agfstnode(G); n; n = agnxtnode(n))
	if (getval(n) == 0)
	    visit(n, map, &stack, &state);
    freeStack(&stack);
    if (!Silent)
	agwrite(map, stdout);
    agclose(map);

    if (Verbose)
	fprintf(stderr, "%d %d %d %d %.4f %d %.4f\n",
		agnnodes(G), agnedges(G), nc, state.Comp,
		state.N_nodes_in_nontriv_SCC / (double) agnnodes(G),
		Maxdegree, nontree_frac);
    else
	fprintf(stderr, "%d nodes, %d edges, %d strong components\n",
		agnnodes(G), agnedges(G), state.Comp);

}

static char *useString = "Usage: %s [-sdv?] <files>\n\
  -s - silent\n\
  -d - allow degenerate components\n\
  -v - verbose\n\
  -? - print usage\n\
If no files are specified, stdin is used\n";

static void usage(int v)
{
    printf(useString, CmdName);
    exit(v);
}

static void scanArgs(int argc, char **argv)
{
    int c;

    CmdName = argv[0];
    while ((c = getopt(argc, argv, ":sdv")) != EOF) {
	switch (c) {
	case 's':
	    Silent = 1;
	    break;
	case 'd':
	    wantDegenerateComp = 1;
	    break;
	case 'v':
	    Verbose = 1;
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr, "%s: option -%c unrecognized - ignored\n",
			CmdName, c);
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (argc)
	Files = argv;
}

static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}

int main(int argc, char **argv)
{
    Agraph_t *g;
    ingraph_state ig;

    scanArgs(argc, argv);
    newIngraph(&ig, Files, gread);

    while ((g = nextGraph(&ig)) != 0) {
	if (agisdirected(g))
	    process(g);
	else
	    fprintf(stderr, "Graph %s in %s is undirected - ignored\n",
		    agnameof(g), fileName(&ig));
	agclose(g);
    }

    return 0;
}
