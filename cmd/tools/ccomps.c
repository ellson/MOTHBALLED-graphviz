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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

#include <graph.h>
#ifdef HAVE_UNISTD_H
#include	<unistd.h>
#endif
#include <string.h>
#include <ingraphs.h>

  /* internals of libgraph */
#define TAG_NODE            1
extern Agdict_t *agdictof(void *);

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
    return (strncmp(g->name, "cluster", 7) == 0);
}

static void init(int argc, char *argv[])
{
    int c;

    aginit();
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
    aginsert(out, n);
    for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	if ((other = e->tail) == n)
	    other = e->head;
	if (ND_mark(other) == 0)
	    cnt = dfs(g, other, out, cnt);
    }
    return cnt;
}

static int nodeInduce(Agraph_t * g, Agraph_t * eg)
{
    Agnode_t *n;
    Agedge_t *e;
    int e_cnt = 0;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(eg, n); e; e = agnxtout(eg, e)) {
	    if (agcontains(g, e->head)) {
		aginsert(g, e);
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

/* copyAttr:
 * Copy the attributes of g2 to g1.
 */
static void copyAttr(Agraph_t * g1, Agraph_t * g2)
{
    Agdict_t *dict;
    Agsym_t *a;
    int i, n;

    dict = agdictof(g2);
    n = dtsize(dict->dict);
    for (i = 0; i < n; i++) {
	a = dict->list[i];
	agxset(g1, a->index, agxget(g2, a->index));
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
	if ((m = agfindnode(g, n->name))) {
	    if (proj == 0) {
		name = getBuf(strlen(subg->name) + pfxlen + 2);
		sprintf(name, "%s_%s", subg->name, pfx);
		proj = agsubg(g, name);
	    }
	    aginsert(proj, m);
	}
    }
    if (!proj && inCluster) {
	name = getBuf(strlen(subg->name) + pfxlen + 2);
	sprintf(name, "%s_%s", subg->name, pfx);
	proj = agsubg(g, name);
    }
    if (proj) {
	nodeInduce(proj, subg);
	copyAttr(proj, subg);
    }

    return proj;
}

/* subgInduce:
 * Project subgraphs of root graph on subgraph.
 * If non-empty, add to subgraph.
 */
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

#define PFX1 "%s_cc"
#define PFX2 "%s_cc_%ld"

/* deriveGraph:
 * Create derived graph dg of g where nodes correspond to cluster or
 * top-level nodes and there is an edge in dg if there is an edge in g
 * between any nodes in the respective clusters.
 */
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
	dn = agnode(dg, n->name);
	ND_ptr(dn).clust = (Agraph_t *) n;
	ND_ptr(n).dn = dn;
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	Agedge_t *e;
	Agnode_t *hd;
	Agnode_t *tl = ND_ptr(n).dn;
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    hd = ND_ptr(e->head).dn;
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

/* unionNodes:
 * Add all nodes in cluster nodes of dg to g
 */
static void unionNodes(Agraph_t * dg, Agraph_t * g)
{
    Agnode_t *n;
    Agnode_t *dn;
    Agraph_t *clust;

    for (dn = agfstnode(dg); dn; dn = agnxtnode(dg, dn)) {
	clust = ND_ptr(dn).clust;
	if (clust->tag == TAG_NODE) {
	    n = (Agnode_t *) clust;
	    aginsert(g, n);
	} else {
	    for (n = agfstnode(clust); n; n = agnxtnode(clust, n))
		aginsert(g, n);
	}
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
		    x_node, g->name);
	    return 1;
	}
	name = getBuf(sizeof(PFX1) + strlen(g->name));
	sprintf(name, PFX1, g->name);
	dout = agsubg(dg, name);
	out = agsubg(g, name);
	GD_cc_subg(out) = 1;
	dn = ND_ptr(n).dn;
	n_cnt = dfs(dg, dn, dout, 0);
	unionNodes(dout, out);
	e_cnt = nodeInduce(out, out->root);
	if (doAll)
	    subgInduce(g, out, out->name, strlen(out->name), 0);
	gwrite(out);
	if (verbose)
	    fprintf(stderr, " %7ld nodes %7ld edges\n", n_cnt, e_cnt);
	return 0;
    }

    c_cnt = 0;
    for (dn = agfstnode(dg); dn; dn = agnxtnode(dg, dn)) {
	if (ND_mark(dn))
	    continue;
	name = getBuf(sizeof(PFX2) + strlen(g->name) + 32);
	sprintf(name, "%s_component_%ld", g->name, c_cnt);
	dout = agsubg(dg, name);
	out = agsubg(g, name);
	GD_cc_subg(out) = 1;
	n_cnt = dfs(dg, dn, dout, 0);
	unionNodes(dout, out);
	e_cnt = nodeInduce(out, out->root);
	if (printMode == EXTERNAL) {
	    if (doAll)
		subgInduce(g, out, out->name, strlen(out->name), 0);
	    gwrite(out);
	} else if (printMode == EXTRACT) {
	    if (x_index == c_cnt) {
		if (doAll)
		    subgInduce(g, out, out->name, strlen(out->name), 0);
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
		x_index, g->name);
	return 1;
    }

    if (printMode == INTERNAL)
	gwrite(g);

    if (verbose)
	fprintf(stderr, "       %7d nodes %7d edges %7ld components %s\n",
		agnnodes(g), agnedges(g), c_cnt, g->name);

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

    if (useClusters)
	return processClusters(g);

    if (x_node) {
	n = agfindnode(g, x_node);
	if (!n) {
	    fprintf(stderr,
		    "ccomps: node %s not found in graph %s - ignored\n",
		    x_node, g->name);
	    return 1;
	}
	name = getBuf(sizeof(PFX1) + strlen(g->name));
	sprintf(name, PFX1, g->name);
	out = agsubg(g, name);
	GD_cc_subg(out) = 1;
	n_cnt = dfs(g, n, out, 0);
	e_cnt = nodeInduce(out, out->root);
	if (doAll)
	    subgInduce(g, out, out->name, strlen(out->name), 0);
	gwrite(out);
	if (verbose)
	    fprintf(stderr, " %7ld nodes %7ld edges\n", n_cnt, e_cnt);
	return 0;
    }

    c_cnt = 0;
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_mark(n))
	    continue;
	name = getBuf(sizeof(PFX2) + strlen(g->name) + 32);
	sprintf(name, "%s_component_%ld", g->name, c_cnt);
	out = agsubg(g, name);
	GD_cc_subg(out) = 1;
	n_cnt = dfs(g, n, out, 0);
	e_cnt = nodeInduce(out, out->root);
	if (printMode == EXTERNAL) {
	    if (doAll)
		subgInduce(g, out, out->name, strlen(out->name), 0);
	    gwrite(out);
	} else if (printMode == EXTRACT) {
	    if (x_index == c_cnt) {
		if (doAll)
		    subgInduce(g, out, out->name, strlen(out->name), 0);
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
		x_index, g->name);
	return 1;
    }

    if (printMode == INTERNAL)
	gwrite(g);

    if (verbose)
	fprintf(stderr, "       %7d nodes %7d edges %7ld components %s\n",
		agnnodes(g), agnedges(g), c_cnt, g->name);
    return (c_cnt ? 1 : 0);
}

int main(int argc, char *argv[])
{
    Agraph_t *g;
    ingraph_state ig;
    int r = 0;

    init(argc, argv);
    newIngraph(&ig, Files, agread);

    while ((g = nextGraph(&ig)) != 0) {
	r += process(g);
	agclose(g);
    }

    return r;
}
