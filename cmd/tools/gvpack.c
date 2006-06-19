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
 * Written by Emden R. Gansner
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

#include <assert.h>
#include <render.h>
#include <neatoprocs.h>
#include <ingraphs.h>
#include <pack.h>
#include <assert.h>

#include "builddate.h"

extern Agdict_t *agdictof(void *);

/* gvpack:
 * Input consists of graphs in dot format.
 * The graphs should have pos, width and height set for nodes, 
 * bb set for clusters, and, optionally, spline info for edges.
 * The graphs are packed geometrically and combined
 * into a single output graph, ready to be sent to neato -s -n2.
 *  -m <i> specifies the margin, in points, about each graph.
 */
char *Info[] = {
    "gvpack",			/* Program */
    VERSION,			/* Version */
    BUILDDATE			/* Build Date */
};

typedef struct {
    Dtlink_t link;
    char *name;
    char *value;
} attr_t;

typedef struct {
    Dtlink_t link;
    char *name;
    int cnt;
} pair_t;

static int margin = 8;		/* Default margin in packing */
static bool doSplines = TRUE;	/* Use edges in packing */
static pack_mode packMode = l_clust;	/* Default packing mode - use clusters */
static int verbose = 0;
static char **myFiles = 0;
static int nGraphs = 0;		/* Guess as to no. of graphs */
static FILE *outfp;		/* output; stdout by default */
static int kind = -1;		/* type of graph; -1 = undefined */
static int G_cnt;		/* No. of -G arguments */
static int G_sz;		/* Storage size for -G arguments */
static attr_t *G_args;		/* Storage for -G arguments */

#define NEWNODE(n) ((node_t*)ND_alg(n))
#define DOPACK     (packMode != l_undef)

static char *useString =
    "Usage: gvpack [-gnuv?] [-m<margin>] [-o<outf>] <files>\n\
  -n          - use node granularity\n\
  -g          - use graph granularity\n\
  -G<n>=<v>   - attach name/value attribute to output graph\n\
  -m<n>       - set margin to <n> points\n\
  -o<outfile> - write output to <outfile>\n\
  -u          - no packing; just combine graphs\n\
  -v          - verbose\n\
  -?          - print usage\n\
If no files are specified, stdin is used\n";

static void usage(int v)
{
    printf(useString);
    exit(v);
}

static FILE *openFile(char *name, char *mode)
{
    FILE *fp;
    char *modestr;

    fp = fopen(name, mode);
    if (!fp) {
	if (*mode == 'r')
	    modestr = "reading";
	else
	    modestr = "writing";
	fprintf(stderr, "gvpack: could not open file %s for %s\n",
		name, modestr);
	exit(1);
    }
    return (fp);
}

#define G_CHUNK 10

/* setNameValue:
 * If arg is a name-value pair, add it to the list
 * and return 0; otherwise, return 1.
 */
static int setNameValue(char *arg)
{
    char *p;
    char *rhs = "true";

    if ((p = strchr(arg, '='))) {
	*p++ = '\0';
	rhs = p;
    }
    if (G_cnt >= G_sz) {
	G_sz += G_CHUNK;
	G_args = RALLOC(G_sz, G_args, attr_t);
    }
    G_args[G_cnt].name = arg;
    G_args[G_cnt].value = rhs;
    G_cnt++;

    return 0;
}

/* setInt:
 * If arg is an integer, value is stored in v
 * and functions returns 0; otherwise, returns 1.
 */
static int setInt(int *v, char *arg)
{
    char *p;
    int i;

    i = (int) strtol(arg, &p, 10);
    if (p == arg) {
	fprintf(stderr, "Error: bad value in flag -%s - ignored\n",
		arg - 1);
	return 1;
    }
    *v = i;
    return 0;
}

/* init:
 */
static void init(int argc, char *argv[])
{
    int c;

    aginit();
    while ((c = getopt(argc, argv, ":ngvum:o:G:")) != -1) {
	switch (c) {
	case 'n':
	    packMode = l_node;
	    break;
	case 'g':
	    packMode = l_graph;
	    break;
	case 'm':
	    setInt(&margin, optarg);
	    break;
	case 'o':
	    outfp = openFile(optarg, "w");
	    break;
	case 'u':
	    packMode = l_undef;
	    break;
	case 'G':
	    if (*optarg)
		setNameValue(optarg);
	    else
		fprintf(stderr,
			"gvpack: option -G missing argument - ignored\n");
	    break;
	case 'v':
	    verbose = 1;
	    Verbose = 1;
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else
		fprintf(stderr,
			"gvpack: option -%c unrecognized - ignored\n", optopt);
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (argc) {
	myFiles = argv;
	nGraphs = argc;		/* guess one graph per file */
    } else
	nGraphs = 10;		/* initial guess as to no. of graphs */
    if (!outfp)
	outfp = stdout;		/* stdout the default */
}

/* init_node_edge:
 * initialize node and edge attributes
 */
static void init_node_edge(Agraph_t * g)
{
    node_t *n;
    edge_t *e;
    int nG = agnnodes(g);
    attrsym_t *N_pos = agfindattr(g->proto->n, "pos");
    attrsym_t *N_pin = agfindattr(g->proto->n, "pin");

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	neato_init_node(n);
	user_pos(N_pos, N_pin, n, nG);	/* set user position if given */
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    common_init_edge(e);
    }
}

/* init_graph:
 * Initialize attributes. We always do the minimum required by
 * libcommon. If fill is true, we use init_nop (neato -n) to
 * read in attributes relevant to the layout.
 */
static void init_graph(Agraph_t * g, bool fill)
{
    int d;

    graph_init(g, FALSE);
    d = late_int(g, agfindattr(g, "dim"), 2, 2);
    if (d != 2) {
	fprintf(stderr, "Error: graph %s has dim = %d (!= 2)\n", g->name,
		d);
	exit(1);
    }
    Ndim = GD_ndim(g) = 2;
    init_node_edge(g);
    if (fill && init_nop(g, 0)) {
	exit(1);
    }
}

/* cloneAttrs:
 * Copy all attributes from old object to new. Assume
 * attributes have been initialized.
 */
static void cloneAttrs(void *old, void *new)
{
    int j;
    Agsym_t *a;
    Agdict_t *dict = agdictof(old);

    for (j = 0; j < dtsize(dict->dict); j++) {
	a = dict->list[j];
	agset(new, a->name, agxget(old, a->index));
    }
}

/* cloneEdge:
 * Note that here, and in cloneNode and cloneCluster,
 * we do a shallow copy. We thus assume that attributes
 * are not disturbed. In particular, we assume they are
 * not deallocated.
 */
static void cloneEdge(Agedge_t * old, Agedge_t * new)
{
    cloneAttrs(old, new);
    ED_spl(new) = ED_spl(old);
    ED_label(new) = ED_label(old);
    ED_head_label(new) = ED_head_label(old);
    ED_tail_label(new) = ED_tail_label(old);
}

/* cloneNode:
 */
static void cloneNode(Agnode_t * old, Agnode_t * new)
{
    cloneAttrs(old, new);
    ND_coord_i(new).x = POINTS(ND_pos(old)[0]);
    ND_coord_i(new).y = POINTS(ND_pos(old)[1]);
    ND_height(new) = ND_height(old);
    ND_ht_i(new) = ND_ht_i(old);
    ND_width(new) = ND_width(old);
    ND_lw_i(new) = ND_lw_i(old);
    ND_rw_i(new) = ND_rw_i(old);
    ND_shape(new) = ND_shape(old);
    ND_shape_info(new) = ND_shape_info(old);
}

/* cloneCluster:
 */
static void cloneCluster(Agraph_t * old, Agraph_t * new)
{
    /* string attributes were cloned as subgraphs */
    GD_label(new) = GD_label(old);
    GD_bb(new) = GD_bb(old);
}

/* freef:
 * Generic free function for dictionaries.
 */
static void freef(Dt_t * dt, Void_t * obj, Dtdisc_t * disc)
{
    free(obj);
}

static Dtdisc_t attrdisc = {
    offsetof(attr_t, name),	/* key */
    -1,				/* size */
    offsetof(attr_t, link),	/* link */
    (Dtmake_f) 0,
    (Dtfree_f) freef,
    (Dtcompar_f) 0,		/* use strcmp */
    (Dthash_f) 0,
    (Dtmemory_f) 0,
    (Dtevent_f) 0
};

/* fillDict:
 * Fill newdict with all the name-value attributes of
 * objp. If the attribute has already been defined and
 * has a different default, set default to "".
 */
static void fillDict(Dt_t * newdict, void *objp)
{
    int j;
    Agsym_t *a;
    Agdict_t *olddict = agdictof(objp);
    char *name;
    char *value;
    attr_t *rv;

    for (j = 0; j < dtsize(olddict->dict); j++) {
	a = olddict->list[j];
	name = a->name;
	value = agxget(objp, a->index);
	rv = (attr_t *) dtmatch(newdict, name);
	if (!rv) {
	    rv = NEW(attr_t);
	    rv->name = name;
	    rv->value = value;
	    dtinsert(newdict, rv);
	} else if (strcmp(value, rv->value))
	    rv->value = "";
    }
}

/* fillGraph:
 * Use all the name-value entries in the dictionary d to define
 * to define universal node/edge/graph attributes for g.
 */
static void
fillGraph(Agraph_t * g, Dt_t * d,
	  Agsym_t * (*setf) (Agraph_t *, char *, char *))
{
    attr_t *av;
    for (av = (attr_t *) dtflatten(d); av;
	 av = (attr_t *) dtlink(d, (Dtlink_t *) av)) {
	setf(g, av->name, av->value);
    }
}

/* initAttrs:
 * Initialize the attributes of root as the union of the
 * attributes in the graphs gs.
 */
static void initAttrs(Agraph_t * root, Agraph_t ** gs, int cnt)
{
    Agraph_t *g;
    Dt_t *n_attrs;
    Dt_t *e_attrs;
    Dt_t *g_attrs;
    int i;

    n_attrs = dtopen(&attrdisc, Dtoset);
    e_attrs = dtopen(&attrdisc, Dtoset);
    g_attrs = dtopen(&attrdisc, Dtoset);

    for (i = 0; i < cnt; i++) {
	g = gs[i];
	fillDict(g_attrs, g);
	fillDict(n_attrs, g->proto->n);
	fillDict(e_attrs, g->proto->e);
    }

    fillGraph(root, g_attrs, agraphattr);
    fillGraph(root, n_attrs, agnodeattr);
    fillGraph(root, e_attrs, agedgeattr);

    dtclose(n_attrs);
    dtclose(e_attrs);
    dtclose(g_attrs);
}

/* cloneGraphAttr:
 */
static void cloneGraphAttr(Agraph_t * g, Agraph_t * ng)
{
    cloneAttrs(g, ng);
    cloneAttrs(g->proto->n, ng->proto->n);
    cloneAttrs(g->proto->e, ng->proto->e);
}

#ifdef UNIMPL
/* redoBB:
 * If "bb" attribute is valid, translate to reflect graph's
 * new packed position.
 */
static void redoBB(Agraph_t * g, char *s, Agsym_t * G_bb, point delta)
{
    box bb;
    char buf[100];

    if (sscanf(s, "%d,%d,%d,%d", &bb.LL.x, &bb.LL.y, &bb.UR.x, &bb.UR.y) ==
	4) {
	bb.LL.x += delta.x;
	bb.LL.y += delta.y;
	bb.UR.x += delta.x;
	bb.UR.y += delta.y;
	sprintf(buf, "%d,%d,%d,%d", bb.LL.x, bb.LL.y, bb.UR.x, bb.UR.y);
	agxset(g, G_bb->index, buf);
    }
}
#endif

/* xName:
 * Create a name for an object in the new graph using the
 * dictionary names and the old name. If the old name has not
 * been used, use it and add it to names. If it has been used,
 * create a new name using the old name and a number.
 */
static char *xName(Dt_t * names, char *oldname)
{
    char name[BUFSIZ];
    char *namep;
    pair_t *p;

    p = (pair_t *) dtmatch(names, oldname);
    if (p) {
	p->cnt++;
	sprintf(name, "%s_gv%d", oldname, p->cnt);
	namep = name;
    } else {
	p = NEW(pair_t);
	p->name = oldname;
	dtinsert(names, p);
	namep = oldname;
    }
    return namep;
}

#define MARK(e) (ED_alg(e) = e)
#define MARKED(e) (ED_alg(e))
#define ISCLUSTER(g) (!strncmp(g->name,"cluster",7))
#define SETCLUST(g,h) (GD_alg(g) = h)
#define GETCLUST(g) ((Agraph_t*)GD_alg(g))

/* cloneSubg:
 * Create a copy of g in ng, copying attributes, inserting nodes
 * and adding edges.
 */
static void
cloneSubg(Agraph_t * g, Agraph_t * ng, Agsym_t * G_bb, Dt_t * gnames)
{
    graph_t *mg;
    edge_t *me;
    node_t *mn;
    node_t *n;
    node_t *nn;
    edge_t *e;
    edge_t *ne;
    node_t *nt;
    node_t *nh;
    Agraph_t *subg;
    Agraph_t *nsubg;

    cloneGraphAttr(g, ng);
    if (DOPACK)
	agxset(ng, G_bb->index, "");	/* Unset all subgraph bb */

    /* clone subgraphs */
    mg = g->meta_node->graph;
    for (me = agfstout(mg, g->meta_node); me; me = agnxtout(mg, me)) {
	mn = me->head;
	subg = agusergraph(mn);
	nsubg = agsubg(ng, xName(gnames, subg->name));
	cloneSubg(subg, nsubg, G_bb, gnames);
	/* if subgraphs are clusters, point to the new 
	 * one so we can find it later.
	 */
	if (ISCLUSTER(subg))
	    SETCLUST(subg, nsubg);
    }

    /* add remaining nodes */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	nn = NEWNODE(n);
	if (!agfindnode(ng, nn->name)) {
	    aginsert(ng, nn);
	}
    }

    /* add remaining edges. libgraph doesn't provide a way to find
     * multiedges, so we add edges bottom up, marking edges when added.
     */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (MARKED(e))
		continue;
	    nt = NEWNODE(e->tail);
	    nh = NEWNODE(e->head);
	    ne = agedge(ng, nt, nh);
	    cloneEdge(e, ne);
	    MARK(e);
	}
    }
}

/* cloneClusterTree:
 * Given old and new subgraphs which are corresponding
 * clusters, recursively create the subtree of clusters
 * under ng using the subtree of clusters under g.
 */
static void cloneClusterTree(Agraph_t * g, Agraph_t * ng)
{
    int i;

    cloneCluster(g, ng);

    if (GD_n_cluster(g)) {
	GD_n_cluster(ng) = GD_n_cluster(g);
	GD_clust(ng) = N_NEW(1 + GD_n_cluster(g), Agraph_t *);
	for (i = 1; i <= GD_n_cluster(g); i++) {
	    Agraph_t *c = GETCLUST(GD_clust(g)[i]);
	    GD_clust(ng)[i] = c;
	    cloneClusterTree(GD_clust(g)[i], c);
	}
    }
}

static Dtdisc_t pairdisc = {
    offsetof(pair_t, name),	/* key */
    -1,				/* size */
    offsetof(attr_t, link),	/* link */
    (Dtmake_f) 0,
    (Dtfree_f) freef,
    (Dtcompar_f) 0,		/* use strcmp */
    (Dthash_f) 0,
    (Dtmemory_f) 0,
    (Dtevent_f) 0
};

/* cloneGraph:
 * Create and return a new graph which is the logical union
 * of the graphs gs. 
 */
static Agraph_t *cloneGraph(Agraph_t ** gs, int cnt)
{
    Agraph_t *root;
    Agraph_t *g;
    Agraph_t *subg;
    Agnode_t *n;
    Agnode_t *np;
    int i;
    Dt_t *gnames;		/* dict of used subgraph names */
    Dt_t *nnames;		/* dict of used node names */
    Agsym_t *G_bb;
    Agsym_t *rv;
    bool doWarn = TRUE;

    if (verbose)
	fprintf(stderr, "Creating clone graph\n");
    root = agopen("root", kind);
    initAttrs(root, gs, cnt);
    G_bb = agfindattr(root, "bb");
    if (DOPACK) assert(G_bb);

    /* add command-line attributes */
    for (i = 0; i < G_cnt; i++) {
	rv = agfindattr(root, G_args[i].name);
	if (rv)
	    agxset(root, rv->index, G_args[i].value);
	else
	    agraphattr(root, G_args[i].name, G_args[i].value);
    }

    /* do common initialization. This will handle root's label. */
    init_graph(root, FALSE);
    State = GVSPLINES;

    gnames = dtopen(&pairdisc, Dtoset);
    nnames = dtopen(&pairdisc, Dtoset);
    for (i = 0; i < cnt; i++) {
	g = gs[i];
	if (verbose)
	    fprintf(stderr, "Cloning graph %s\n", g->name);
	GD_n_cluster(root) += GD_n_cluster(g);

	/* Clone nodes, checking for node name conflicts */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    if (doWarn && agfindnode(root, n->name)) {
		fprintf(stderr,
			"Warning: node %s in graph[%d] %s already defined\n",
			n->name, i, g->name);
		fprintf(stderr, "Some nodes will be renamed.\n");
		doWarn = FALSE;
	    }
	    np = agnode(root, xName(nnames, n->name));
	    ND_alg(n) = np;
	    cloneNode(n, np);
	}

	/* wrap the clone of g in a subgraph of root */
	subg = agsubg(root, xName(gnames, g->name));
	cloneSubg(g, subg, G_bb, gnames);
    }
    dtclose(gnames);
    dtclose(nnames);

    /* set up cluster tree */
    if (GD_n_cluster(root)) {
	int j, idx;
	GD_clust(root) = N_NEW(1 + GD_n_cluster(root), graph_t *);

	idx = 1;
	for (i = 0; i < cnt; i++) {
	    g = gs[i];
	    for (j = 1; j <= GD_n_cluster(g); j++) {
		Agraph_t *c = GETCLUST(GD_clust(g)[j]);
		GD_clust(root)[idx++] = c;
		cloneClusterTree(GD_clust(g)[j], c);
	    }
	}
    }

    return root;
}

/* readGraphs:
 * Read input, parse the graphs, use init_nop (neato -n) to
 * read in all attributes need for layout.
 * Return the list of graphs. If cp != NULL, set it to the number
 * of graphs read.
 * We keep track of the types of graphs read. They all must be
 * either directed or undirected. If all graphs are strict, the
 * combined graph will be strict; other, the combined graph will
 * be non-strict.
 */
static Agraph_t **readGraphs(int *cp)
{
    Agraph_t *g;
    Agraph_t **gs = 0;
    ingraph_state ig;
    int cnt = 0;
    int sz = 0;

    /* set various state values */
    PSinputscale = POINTS_PER_INCH;
    Nop = 2;

    newIngraph(&ig, myFiles, agread);
    while ((g = nextGraph(&ig)) != 0) {
	if (verbose)
	    fprintf(stderr, "Reading graph %s\n", g->name);
	if (cnt >= sz) {
	    sz += nGraphs;
	    gs = ALLOC(sz, gs, Agraph_t *);
	}
	if (kind == -1)
	    kind = g->kind;
	else if ((kind & AGFLAG_DIRECTED) != AG_IS_DIRECTED(g)) {
	    fprintf(stderr,
		    "Error: all graphs must be directed or undirected\n");
	    exit(1);
	} else if (!AG_IS_STRICT(g))
	    kind = g->kind;
	init_graph(g, DOPACK);
	gs[cnt++] = g;
    }

    gs = RALLOC(cnt, gs, Agraph_t *);
    if (cp)
	*cp = cnt;
    return gs;
}

/* compBB:
 * Compute the bounding box containing the graphs.
 * We can just use the bounding boxes of the graphs.
 */
box compBB(Agraph_t ** gs, int cnt)
{
    box bb, bb2;
    int i;

    bb = GD_bb(gs[0]);

    for (i = 1; i < cnt; i++) {
	bb2 = GD_bb(gs[i]);
	bb.LL.x = MIN(bb.LL.x, bb2.LL.x);
	bb.LL.y = MIN(bb.LL.y, bb2.LL.y);
	bb.UR.x = MAX(bb.UR.x, bb2.UR.x);
	bb.UR.y = MAX(bb.UR.y, bb2.UR.y);
    }

    return bb;
}

#ifdef DEBUG
void dump(Agraph_t * g)
{
    node_t *v;
    edge_t *e;

    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	fprintf(stderr, "%s\n", v->name);
	for (e = agfstout(g, v); e; e = agnxtout(g, e)) {
	    fprintf(stderr, "  %s -- %s\n", e->tail->name, e->head->name);
	}
    }
}

void dumps(Agraph_t * g)
{
    graph_t *subg;
    graph_t *mg;
    edge_t *me;
    node_t *mn;

    mg = g->meta_node->graph;
    for (me = agfstout(mg, g->meta_node); me; me = agnxtout(mg, me)) {
	mn = me->head;
	subg = agusergraph(mn);
	dump(subg);
	fprintf(stderr, "====\n");
    }
}
#endif

int main(int argc, char *argv[])
{
    Agraph_t **gs;
    Agraph_t *g;
    int cnt;
    pack_info pinfo;
    box bb;

    init(argc, argv);

    gs = readGraphs(&cnt);
    if (cnt == 0)
	exit(0);

    /* pack graphs */
    if (DOPACK) {
	pinfo.margin = margin;
	pinfo.doSplines = doSplines;
	pinfo.mode = packMode;
	pinfo.fixed = 0;
	if (packGraphs(cnt, gs, 0, &pinfo)) {
	    fprintf(stderr, "gvpack: packing of graphs failed.\n");
	    exit(1);
	}
    }

    /* create union graph and copy attributes */
    g = cloneGraph(gs, cnt);

    /* compute new top-level bb and set */
    if (DOPACK) {
	bb = compBB(gs, cnt);
	GD_bb(g) = bb;
	dotneato_postprocess(g);
	attach_attrs(g);
    }
    agwrite(g, outfp);
    exit(0);
}
