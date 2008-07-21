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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>

#ifdef USE_CGRAPH
#include <stdlib.h>
#include <cgraph.h>

typedef struct {
    Agrec_t h;
    char cc_subg;
} Agraphinfo_t;

typedef struct {
    Agrec_t h;
    char mark;
    Agobj_t* ptr;
} Agnodeinfo_t;

#define GD_cc_subg(g)  (((Agraphinfo_t*)(g->base.data))->cc_subg)
#define ND_mark(n)  (((Agnodeinfo_t*)(n->base.data))->mark)
#define ND_ptr(n)  (((Agnodeinfo_t*)(n->base.data))->ptr)
#define ND_dn(n)  ((Agnode_t*)ND_ptr(n))
#define ND_clust(n)  ((Agraph_t*)ND_ptr(n))
#define agfindnode(G,N) (agnode(G, N, 0))
#else
typedef struct {
    char cc_subg;
} Agraphinfo_t;
typedef struct {
    char mark;
    union {
	struct Agnode_t *dn;
	struct Agraph_t *clust;
    } ptr;
} Agnodeinfo_t;
typedef char Agedgeinfo_t;

#define GD_cc_subg(g) (g)->u.cc_subg
#define ND_mark(n) (n)->u.mark
#define ND_ptr(n) (n)->u.ptr
#include <graph.h>
#define agnameof(x) (agobjkind(x) == AGNODE ? ((Agnode_t*)(x))->name : ((Agraph_t*)(x))->name)
#define agtail(e) ((e)->tail)
#define aghead(e) ((e)->head)
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

#ifdef HAVE_UNISTD_H
#include	<unistd.h>
#endif
#include <string.h>
#include <ingraphs.h>

  /* internals of libgraph */
#define TAG_NODE            1

#define INTERNAL 0
#define EXTERNAL 1
#define SILENT   2
#define EXTRACT  3

char **Files;
int verbose;
int printMode = INTERNAL;
int useClusters = 0;
int doAll = 1;			/* induce subgraphs */
char *suffix = 0;
char *outfile = 0;
char *path = 0;
int sufcnt = 0;
int x_index = -1;
char *x_node;

static char *useString =
    "Usage: ccomps [-svnCx?] [-X[#]v] [-o<out template>] <files>\n\
  -s - silent\n\
  -x - external\n\
  -X - extract component\n\
  -C - use clusters\n\
  -n - do not induce subgraphs\n\
  -v - verbose\n\
  -o - output file template\n\
  -? - print usage\n\
If no files are specified, stdin is used\n";

static void usage(int v)
{
    printf(useString);
    exit(v);
}

static void split(char *name)
{
    char *sfx = 0;
    int size;

    sfx = strrchr(name, '.');
    if (sfx) {
	suffix = sfx + 1;
	size = sfx - name;
	path = (char *) malloc(size + 1);
	strncpy(path, name, size);
	*(path + size) = '\0';
    } else {
	path = name;
    }
}

/* isCluster:
 * Return true if graph is a cluster
 */
static int isCluster(Agraph_t * g)
{
    return (strncmp(agnameof(g), "cluster", 7) == 0);
}

static void init(int argc, char *argv[])
{
    int c;

#ifndef USE_CGRAPH
    aginit();
#endif
    while ((c = getopt(argc, argv, ":o:xCX:nsv?")) != -1) {
	switch (c) {
	case 'o':
	    outfile = optarg;
	    split(outfile);
	    break;
	case 'C':
	    useClusters = 1;
	    break;
	case 'n':
	    doAll = 0;
	    break;
	case 'x':
	    printMode = EXTERNAL;
	    break;
	case 's':
	    printMode = SILENT;
	    break;
	case 'X':
	    if (*optarg == '#') {
		char *p = optarg + 1;
		if (isdigit(*p)) {
		    x_index = atoi(p);
		    printMode = EXTRACT;
		} else
		    fprintf(stderr,
			    "ccomps: number expected in -X%s flag - ignored\n",
			    optarg);
	    } else {
		x_node = optarg;
		printMode = EXTRACT;
	    }
	    break;
	case 'v':
	    verbose = 1;
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr,
			"ccomps: option -%c unrecognized - ignored\n", optopt);
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (argc)
	Files = argv;
}

static int dfs(Agraph_t * g, Agnode_t * n, Agraph_t * out, int cnt)
{
    Agedge_t *e;
    Agnode_t *other;

    ND_mark(n) = 1;
    cnt++;
#ifdef USE_CGRAPH
    agsubnode(out, n, 1);
#else
    aginsert(out, n);
#endif
    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	if ((other = agtail(e)) == n)
	    other = aghead(e);
	if (ND_mark(other) == 0)
	    cnt = dfs(g, other, out, cnt);
    }
    return cnt;
}

/* nodeInduce:
 * Using the edge set of eg, add to g any edges
 * with both endpoints in g.
 */
static int nodeInduce(Agraph_t * g, Agraph_t * eg)
{
    Agnode_t *n;
    Agedge_t *e;
    int e_cnt = 0;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(eg, n); e; e = agnxtout(eg, e)) {
#ifdef USE_CGRAPH
	    if (agsubnode(g, aghead(e), 0)) {
		agsubedge(g, e, 1);
#else
	    if (agcontains(g, aghead(e))) {
		aginsert(g, e);
#endif
		e_cnt++;
	    }
	}
    }
    return e_cnt;
}

static char *getName(void)
{
    char *name;
    static char *buf = 0;

    if (sufcnt == 0)
	name = outfile;
    else {
	if (!buf)
	    buf = (char *) malloc(strlen(outfile) + 20);	/* enough to handle '_number' */
	if (suffix)
	    sprintf(buf, "%s_%d.%s", path, sufcnt, suffix);
	else
	    sprintf(buf, "%s_%d", path, sufcnt);
	name = buf;
    }
    sufcnt++;
    return name;
}

static void gwrite(Agraph_t * g)
{
    FILE *outf;
    char *name;

    if (!outfile) {
	agwrite(g, stdout);
	fflush(stdout);
    } else {
	name = getName();
	outf = fopen(name, "w");
	if (!outf) {
	    fprintf(stderr, "Could not open %s for writing\n", name);
	    perror("ccomps");
	}
	agwrite(g, outf);
	fflush(outf);
	fclose(outf);
    }
}

/* getBuf
 * Return pointer to buffer containing at least n bytes.
 * Non-reentrant.
 */
static char *getBuf(int n)
{
    static int len = 0;
    static char *buf = 0;
    int sz;

    if (n > len) {
	sz = n + 100;
	if (len == 0)
	    buf = (char *) malloc(sz);
	else
	    buf = (char *) realloc(buf, sz);
	len = sz;
    }
    return buf;
}

/* projectG:
 * If any nodes of subg are in g, create a subgraph of g
 * and fill it with all nodes of subg in g and their induced
 * edges in subg. Copy the attributes of subg to g. Return the subgraph.
 * If not, return null.
 */
static Agraph_t *projectG(Agraph_t * subg, Agraph_t * g, char *pfx,
			  int pfxlen, int inCluster)
{
    Agraph_t *proj = 0;
    Agnode_t *n;
    Agnode_t *m;
    char *name;

    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {
	if ((m = agfindnode(g, agnameof(n)))) {
	    if (proj == 0) {
		name = getBuf(strlen(agnameof(subg)) + pfxlen + 2);
		sprintf(name, "%s_%s", agnameof(subg), pfx);
#ifdef USE_CGRAPH
		proj = agsubg(g, name, 1);
#else
		proj = agsubg(g, name);
#endif
	    }
#ifdef USE_CGRAPH
	    agsubnode(proj, m, 1);
#else
	    aginsert(proj, m);
#endif
	}
    }
    if (!proj && inCluster) {
	name = getBuf(strlen(agnameof(subg)) + pfxlen + 2);
	sprintf(name, "%s_%s", agnameof(subg), pfx);
#ifdef USE_CGRAPH
	proj = agsubg(g, name, 1);
#else
	proj = agsubg(g, name);
#endif
    }
    if (proj) {
	nodeInduce(proj, subg);
	agcopyattr(subg, proj);
    }

    return proj;
}

/* subgInduce:
 * Project subgraphs of root graph on subgraph.
 * If non-empty, add to subgraph.
 */
#ifdef USE_CGRAPH
static void
subgInduce(Agraph_t * root, Agraph_t * g, char *pfx, int pfxlen,
	   int inCluster)
{
    Agraph_t *subg;
    Agraph_t *proj;
    int in_cluster;

/* fprintf (stderr, "subgInduce %s inCluster %d\n", agnameof(root), inCluster); */
    for (subg = agfstsubg(root); subg; subg = agnxtsubg(subg)) {
	if (GD_cc_subg(subg))
	    continue;
	if ((proj = projectG(subg, g, pfx, pfxlen, inCluster))) {
	    in_cluster = inCluster || (useClusters && isCluster(subg));
	    subgInduce(subg, proj, pfx, pfxlen, in_cluster);
	}
    }
}

static void
subGInduce(Agraph_t* g, Agraph_t * out)
{
    subgInduce(g, out, agnameof(out), strlen(agnameof(out)), 0);
}
#else
static void
subgInduce(Agraph_t * root, Agraph_t * g, char *pfx, int pfxlen,
	   int inCluster)
{
    Agraph_t *mg;
    Agedge_t *me;
    Agnode_t *mn;
    Agraph_t *subg;
    Agraph_t *proj;
    int in_cluster;

/* fprintf (stderr, "subgInduce %s inCluster %d\n", root->name, inCluster); */
    mg = root->meta_node->graph;
    for (me = agfstout(mg, root->meta_node); me; me = agnxtout(mg, me)) {
	mn = me->head;
	subg = agusergraph(mn);
	if (GD_cc_subg(subg))
	    continue;
	if ((proj = projectG(subg, g, pfx, pfxlen, inCluster))) {
	    in_cluster = inCluster || (useClusters && isCluster(subg));
	    subgInduce(subg, proj, pfx, pfxlen, in_cluster);
	}
    }
}

#endif

#define PFX1 "%s_cc"
#define PFX2 "%s_cc_%ld"

/* deriveGraph:
 * Create derived graph dg of g where nodes correspond to cluster or
 * top-level nodes and there is an edge in dg if there is an edge in g
 * between any nodes in the respective clusters.
 */
#ifdef USE_CGRAPH
static Agraph_t *deriveGraph(Agraph_t * g)
{
    Agraph_t *dg;
    Agnode_t *dn;
    Agnode_t *n;
    Agraph_t *subg;

    dg = agopen("dg", Agstrictundirected, (Agdisc_t *) 0);

    for (subg = agfstsubg(g); subg; subg = agnxtsubg(subg)) {
	if (!strncmp(agnameof(subg), "cluster", 7)) {
	    dn = agnode(dg, agnameof(subg), 1);
	    agbindrec (dn, "nodeinfo", sizeof(Agnodeinfo_t), TRUE);
	    ND_ptr(dn) = (Agobj_t*)subg;
	    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {
		ND_ptr(n) = (Agobj_t*)dn;
	    }
	}
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_dn(n))
	    continue;
	dn = agnode(dg, agnameof(n), 1);
	agbindrec (dn, "nodeinfo", sizeof(Agnodeinfo_t), TRUE);
	ND_ptr(dn) = (Agobj_t*)n;
	ND_ptr(n) = (Agobj_t*)dn;
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	Agedge_t *e;
	Agnode_t *hd;
	Agnode_t *tl = ND_dn(n);
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    hd = ND_dn(aghead(e));
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
#else
static Agraph_t *deriveGraph(Agraph_t * g)
{
    Agraph_t *mg;
    Agedge_t *me;
    Agnode_t *mn;
    Agraph_t *dg;
    Agnode_t *dn;
    Agnode_t *n;
    Agraph_t *subg;

    dg = agopen("dg", AGRAPHSTRICT);

    mg = g->meta_node->graph;
    for (me = agfstout(mg, g->meta_node); me; me = agnxtout(mg, me)) {
	mn = me->head;
	subg = agusergraph(mn);
	if (!strncmp(subg->name, "cluster", 7)) {
	    dn = agnode(dg, subg->name);
	    ND_ptr(dn).clust = subg;
	    for (n = agfstnode(subg); n; n = agnxtnode(subg, n)) {
		ND_ptr(n).dn = dn;
	    }
	}
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_ptr(n).dn)
	    continue;
	dn = agnode(dg, agnameof(n));
	ND_ptr(dn).clust = (Agraph_t *) n;
	ND_ptr(n).dn = dn;
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	Agedge_t *e;
	Agnode_t *hd;
	Agnode_t *tl = ND_ptr(n).dn;
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    hd = ND_ptr(aghead(e)).dn;
	    if (hd == tl)
		continue;
	    if (hd > tl)
		agedge(dg, tl, hd);
	    else
		agedge(dg, hd, tl);
	}
    }

    return dg;
}
#endif

/* unionNodes:
 * Add all nodes in cluster nodes of dg to g
 */
static void unionNodes(Agraph_t * dg, Agraph_t * g)
{
    Agnode_t *n;
    Agnode_t *dn;
    Agraph_t *clust;

    for (dn = agfstnode(dg); dn; dn = agnxtnode(dg, dn)) {
#ifdef USE_CGRAPH
	if (AGTYPE(ND_ptr(dn)) == AGNODE) {
	    agsubnode(g, ND_dn(dn), 1);
	} else {
	    clust = ND_clust(dn);
	    for (n = agfstnode(clust); n; n = agnxtnode(clust, n))
		agsubnode(g, n, 1);
	}
#else
	clust = ND_ptr(dn).clust;
	if (clust->tag == TAG_NODE) {
	    n = (Agnode_t *) clust;
	    aginsert(g, n);
	} else {
	    for (n = agfstnode(clust); n; n = agnxtnode(clust, n))
		aginsert(g, n);
	}
#endif
    }
}

/* processClusters:
 * Return 0 if graph is connected.
 */
static int processClusters(Agraph_t * g)
{
    Agraph_t *dg;
    long n_cnt, c_cnt, e_cnt;
    char *name;
    Agraph_t *out;
    Agnode_t *n;
    Agraph_t *dout;
    Agnode_t *dn;

    dg = deriveGraph(g);

    if (x_node) {
	n = agfindnode(g, x_node);
	if (!n) {
	    fprintf(stderr, "ccomps: node %s not found in graph %s\n",
		    x_node, agnameof(g));
	    return 1;
	}
	name = getBuf(sizeof(PFX1) + strlen(agnameof(g)));
	sprintf(name, PFX1, agnameof(g));
#ifdef USE_CGRAPH
	dout = agsubg(dg, name, 1);
	out = agsubg(g, name, 1);
	aginit(out, AGRAPH, "graphinfo", sizeof(Agraphinfo_t), TRUE);
#else
	dout = agsubg(dg, name);
	out = agsubg(g, name);
#endif
	GD_cc_subg(out) = 1;
#ifdef USE_CGRAPH
	dn = ND_dn(n);
#else
	dn = ND_ptr(n).dn;
#endif
	n_cnt = dfs(dg, dn, dout, 0);
	unionNodes(dout, out);
	e_cnt = nodeInduce(out, out->root);
	if (doAll)
#ifdef USE_CGRAPH
	    subGInduce(g, out);
#else
	    subgInduce(g, out, out->name, strlen(out->name), 0);
#endif
	gwrite(out);
	if (verbose)
	    fprintf(stderr, " %7ld nodes %7ld edges\n", n_cnt, e_cnt);
	return 0;
    }

    c_cnt = 0;
    for (dn = agfstnode(dg); dn; dn = agnxtnode(dg, dn)) {
	if (ND_mark(dn))
	    continue;
	name = getBuf(sizeof(PFX2) + strlen(agnameof(g)) + 32);
	sprintf(name, "%s_component_%ld", agnameof(g), c_cnt);
#ifdef USE_CGRAPH
	dout = agsubg(dg, name, 1);
	out = agsubg(g, name, 1);
	aginit(out, AGRAPH, "graphinfo", sizeof(Agraphinfo_t), TRUE);
#else
	dout = agsubg(dg, name);
	out = agsubg(g, name);
#endif
	GD_cc_subg(out) = 1;
	n_cnt = dfs(dg, dn, dout, 0);
	unionNodes(dout, out);
	e_cnt = nodeInduce(out, out->root);
	if (printMode == EXTERNAL) {
	    if (doAll)
#ifdef USE_CGRAPH
		subGInduce(g, out);
#else
		subgInduce(g, out, out->name, strlen(out->name), 0);
#endif
	    gwrite(out);
	} else if (printMode == EXTRACT) {
	    if (x_index == c_cnt) {
		if (doAll)
#ifdef USE_CGRAPH
		    subGInduce(g, out);
#else
		    subgInduce(g, out, out->name, strlen(out->name), 0);
#endif
		gwrite(out);
		return 0;
	    }
	}
	if (printMode != INTERNAL)
	    agdelete(g, out);
	agdelete(dg, dout);
	if (verbose)
	    fprintf(stderr, "(%4ld) %7ld nodes %7ld edges\n",
		    c_cnt, n_cnt, e_cnt);
	c_cnt++;
    }
    if (printMode == EXTRACT) {
	fprintf(stderr,
		"ccomps: component %d not found in graph %s - ignored\n",
		x_index, agnameof(g));
	return 1;
    }

    if (printMode == INTERNAL)
	gwrite(g);

    if (verbose)
	fprintf(stderr, "       %7d nodes %7d edges %7ld components %s\n",
		agnnodes(g), agnedges(g), c_cnt, agnameof(g));

    agclose(dg);

    return (c_cnt ? 1 : 0);
}

/* process:
 * Return 0 if graph is connected.
 */
static int process(Agraph_t * g)
{
    long n_cnt, c_cnt, e_cnt;
    char *name;
    Agraph_t *out;
    Agnode_t *n;

#ifdef USE_CGRAPH
    aginit(g, AGNODE, "nodeinfo", sizeof(Agnodeinfo_t), TRUE);
    aginit(g, AGRAPH, "graphinfo", sizeof(Agraphinfo_t), TRUE);
#endif

    if (useClusters)
	return processClusters(g);

    if (x_node) {
	n = agfindnode(g, x_node);
	if (!n) {
	    fprintf(stderr,
		    "ccomps: node %s not found in graph %s - ignored\n",
		    x_node, agnameof(g));
	    return 1;
	}
	name = getBuf(sizeof(PFX1) + strlen(agnameof(g)));
	sprintf(name, PFX1, agnameof(g));
#ifdef USE_CGRAPH
	out = agsubg(g, name, 1);
	aginit(out, AGRAPH, "graphinfo", sizeof(Agraphinfo_t), TRUE);
#else
	out = agsubg(g, name);
#endif
	GD_cc_subg(out) = 1;
	n_cnt = dfs(g, n, out, 0);
	e_cnt = nodeInduce(out, out->root);
	if (doAll)
#ifdef USE_CGRAPH
	    subGInduce(g, out);
#else
	    subgInduce(g, out, out->name, strlen(out->name), 0);
#endif
	gwrite(out);
	if (verbose)
	    fprintf(stderr, " %7ld nodes %7ld edges\n", n_cnt, e_cnt);
	return 0;
    }

    c_cnt = 0;
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_mark(n))
	    continue;
	name = getBuf(sizeof(PFX2) + strlen(agnameof(g)) + 32);
	sprintf(name, "%s_component_%ld", agnameof(g), c_cnt);
#ifdef USE_CGRAPH
	out = agsubg(g, name, 1);
	aginit(out, AGRAPH, "graphinfo", sizeof(Agraphinfo_t), TRUE);
#else
	out = agsubg(g, name);
#endif
	GD_cc_subg(out) = 1;
	n_cnt = dfs(g, n, out, 0);
	e_cnt = nodeInduce(out, out->root);
	if (printMode == EXTERNAL) {
	    if (doAll)
#ifdef USE_CGRAPH
		subGInduce(g, out);
#else
		subgInduce(g, out, out->name, strlen(out->name), 0);
#endif
	    gwrite(out);
	} else if (printMode == EXTRACT) {
	    if (x_index == c_cnt) {
		if (doAll)
#ifdef USE_CGRAPH
		    subGInduce(g, out);
#else
		    subgInduce(g, out, out->name, strlen(out->name), 0);
#endif
		gwrite(out);
		return 0;
	    }
	}
	if (printMode != INTERNAL)
	    agdelete(g, out);
	if (verbose)
	    fprintf(stderr, "(%4ld) %7ld nodes %7ld edges\n",
		    c_cnt, n_cnt, e_cnt);
	c_cnt++;
    }
    if (printMode == EXTRACT) {
	fprintf(stderr,
		"ccomps: component %d not found in graph %s - ignored\n",
		x_index, agnameof(g));
	return 1;
    }

    if (printMode == INTERNAL)
	gwrite(g);

    if (verbose)
	fprintf(stderr, "       %7d nodes %7d edges %7ld components %s\n",
		agnnodes(g), agnedges(g), c_cnt, agnameof(g));
    return (c_cnt > 1);
}

#ifdef USE_CGRAPH
static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}
#endif

int main(int argc, char *argv[])
{
    Agraph_t *g;
    ingraph_state ig;
    int r = 0;

    init(argc, argv);
#ifdef USE_CGRAPH
    newIngraph(&ig, Files, gread);
#else
    newIngraph(&ig, Files, agread);
#endif

    while ((g = nextGraph(&ig)) != 0) {
	r += process(g);
	agclose(g);
    }

    return r;
}
