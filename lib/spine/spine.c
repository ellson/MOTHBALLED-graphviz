/* vim:set shiftwidth=4 ts=4: */

#include <spinehdr.h>
#include <subset.h>
#include <quad.h>
#include "union_find.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef MAIN
#include <getopt.h>
#include "ingraphs.h"

typedef struct {
	FILE *outfp;
	char **Files;
	float sparse_ratio;
	int verbose;
} opts_t;
#endif

#if 0
static float ewt(Agedge_t * e)
{
	return ED_wt(e);
}
#endif

static void doEdge(Agraph_t * g, Agnode_t * v, Agnode_t * w, int *quadcnt)
{
	Agedge_t *e = agedge(g, v, w, 0, 0);
	if (!e)
		e = agedge(g, w, v, 0, 0);
	if (!e) {
		fprintf(stderr, "Could not find edge %s--%s\n", agnameof(v),
				agnameof(w));
		exit(1);
	}
	quadcnt[ED_id(e)] += 1;
}

static void
recordQuads(Agnode_t * v, Agnode_t * w, Dt_t * subset, void *state)
{
	Dtlink_t *link0;
	Dtlink_t *link1;
	Agnode_t *u0;
	Agnode_t *u1;
	int *quadcnt = (int *) state;
	Agraph_t *g = agroot(v);

	for (link0 = dtflatten(subset); link0; link0 = dtlink(subset, link0)) {
		u0 = (Agnode_t *) (((ptritem *) dtobj(subset, link0))->v);
		for (link1 = dtlink(subset, link0); link1;
			 link1 = dtlink(subset, link1)) {
			u1 = (Agnode_t *) (((ptritem *) dtobj(subset, link1))->v);
			doEdge(g, v, u0, quadcnt);
			doEdge(g, w, u0, quadcnt);
			doEdge(g, v, u1, quadcnt);
			doEdge(g, w, u1, quadcnt);
		}
	}
}

static void setEdgeWeights(Agraph_t * g)
{
	int *quadcnt = N_NEW(agnedges(g), int);
	int *que = N_NEW(agnnodes(g), int);
	Agnode_t *n;
	/* Agnode_t *v; */
	Agedge_t *e;
	int wt;

	genQuads(g, recordQuads, quadcnt);
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		wt = 0;
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			if (aghead(e) == n)
				continue;
			wt += quadcnt[ED_id(e)];
		}
		for (e = agfstin(g, n); e; e = agnxtin(g, e)) {
			if (agtail(e) == n)
				continue;
			wt += quadcnt[ED_id(e)];
		}
		que[ND_id(n)] = wt;
#if 0
		fprintf(stderr, " %s quad %d\n", agnameof(n), que[ND_id(n)]);
#endif
	}

	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			int quadv = quadcnt[ED_id(e)];
			if (quadv)
				ED_wt(e) =
					quadv /
					sqrt((double) (que[ND_id(n)] * que[ND_id(aghead(e))]));
			else
				ED_wt(e) = 0;
#if 0
			fprintf(stderr, " %s--%s quadcnt %d wt %f\n",
					agnameof(n), agnameof(aghead(e)), quadcnt[ED_id(e)],
					ED_wt(e));
#endif
		}
	}

	free(quadcnt);
	free(que);
}

/* Return number in range [0,nedges] */
static int computeIndex(int nedges, float s)
{
	int r = ceilf(nedges * (1 - s));
	return r;
}

static int doBucket(Agedge_t ** edgelist, int idx, Dt_t * M)
{
	Agedge_t *e;
	float weight = ED_wt(edgelist[idx]);

	while ((e = edgelist[idx]) && (ED_wt(e) == weight)) {
		idx++;
		if (UF_find(agtail(e)) != UF_find(aghead(e)))
			addSubset(M, e);
	}

	return idx;
}

static int walker(Agedge_t * e, Agraph_t * sg)
{
	UF_union(agtail(e), aghead(e));
	agsubedge(sg, e, 1);
	return 0;
}

static Agraph_t *findUMST(Agraph_t * g, Agedge_t ** edgelist, int nedges)
{
	Agraph_t *sg = agsubg(g, "UMST", 1);
	Dt_t *M = mkSubset();

	int i = 0;
	while (i < nedges) {
		i = doBucket(edgelist, i, M);
		/* for each edge in M, add to sg, and union nodes */
		walkSubset(M, (walkfn) walker, sg);
		if (i < nedges)
			clearSubset(M);
	}
	closeSubset(M);
	return sg;
}

static int cmpe(const void *v0, const void *v1)
{
	Agedge_t *e0 = *(Agedge_t **) v0;
	Agedge_t *e1 = *(Agedge_t **) v1;

	if (ED_wt(e0) > ED_wt(e1))
		return -1;
	else if (ED_wt(e0) < ED_wt(e1))
		return 1;
	else
		return 0;
}


void genSpine(Agraph_t * g, float sparse_ratio, int verbose)
{
	Agraph_t *sg_union;
	Agnode_t *n;
	Agedge_t *e;
	Agedge_t **edgelist;
	int i, index;
	int nedges = agnedges(g);
	float threshhold;

	if (verbose)
		fprintf(stderr, "Graph %s %d nodes %d edges:\n", agnameof(g),
				agnnodes(g), agnedges(g));
	aginit(g, AGNODE, "nodeinfo", sizeof(nodeinfo_t), 1);
	aginit(g, AGEDGE, "edgeinfo", sizeof(edgeinfo_t), 1);

	int eidx = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			ED_id(e) = eidx++;
		}
	}
	if (verbose)
		fprintf(stderr, "setEdgeWeights\n");
	setEdgeWeights(g);

	/* sort edges by non-increasing weight */
	if (verbose)
		fprintf(stderr, "sorting\n");
	edgelist = N_NEW(nedges + 1, Agedge_t *);	/* NULL at end */
	i = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			edgelist[i++] = e;
		}
	}
	qsort(edgelist, nedges, sizeof(Agedge_t *), cmpe);
#if 0
	float curwt = -1;
	int cnt = 0;
	for (i = 0; i <= nedges; i++) {
		e = edgelist[i];
		if (e && (ED_wt(e) == curwt))
			cnt++;
		else {
			if (cnt)
				fprintf(stderr, "%f %d\n", curwt, cnt);
			if (e) {
				cnt = 1;
				curwt = ED_wt(e);
			}
		}
	}
#endif
	if (verbose)
		fprintf(stderr, "heaviest wt %f least wt %f\n", ED_wt(edgelist[0]),
				ED_wt(edgelist[nedges - 1]));

	/* compute UMST */
	sg_union = findUMST(g, edgelist, nedges);
	int umst_size = agnedges(sg_union);
	if (verbose)
		fprintf(stderr, " union of maximum spanning trees: %d edges\n",
				umst_size);

	/* Find index of the |E|(1-sparse_ratio)th edge */
	index = computeIndex(nedges, sparse_ratio);
	if (verbose)
		fprintf(stderr, " index %d out of %d\n", index, nedges);

	/* set of edges with weights above threshhold */
	/* Add all edges with wt >= wt(edgelist[index]) */
	/* As edgelist is sorted, first index edges */
	int extra_edges = 0;
	for (i = 1; i <= index; i++) {
		e = edgelist[i - 1];
		if (verbose) {
			if (agsubedge(sg_union, e, 0) == NULL)
				extra_edges++;
		}
		agsubedge(sg_union, e, 1);
	}

	/* Add any additional edges with same weight as e */
	if (index) {
		threshhold = ED_wt(e);
		for (; i <= nedges; i++) {
			e = edgelist[i - 1];
			if (ED_wt(e) >= threshhold) {
				if (verbose) {
					if (agsubedge(sg_union, e, 0) == NULL)
						extra_edges++;
				}
				agsubedge(sg_union, e, 1);
			} else
				break;
		}
	}
	if (verbose)
		fprintf(stderr,
				"number of extra edges not in UMST %d total edges %d\n",
				extra_edges, agnedges(sg_union));

	/* layout graph sg_union */
	Agsym_t *sym = agattr(g, AGEDGE, "weight", "0");
	int ncnt = 0;
	int ecnt = 0;
	for (n = agfstnode(sg_union); n; n = agnxtnode(sg_union, n)) {
		ncnt++;
		for (e = agfstout(sg_union, n); e; e = agnxtout(sg_union, e)) {
			ecnt++;
			agxset(e, sym, "1");
		}
	}
	fprintf(stderr, "ncnt %d ecnt %d\n", ncnt, ecnt);
}

#ifdef MAIN

static FILE *openFile(char *cmd, char *name, char *mode)
{
	FILE *fp;
	char *modestr;

	fp = fopen(name, mode);
	if (!fp) {
		if (*mode == 'r')
			modestr = "reading";
		else
			modestr = "writing";
		fprintf(stderr, "%s: could not open file %s for %s\n",
				cmd, name, modestr);
		exit(1);
	}
	return fp;
}

static int setFloat(char *s, float *v)
{
	char *endp;
	float f = strtof(s, &endp);
	if (s == endp) {
		fprintf(stderr, "Option value \"%s\" must be a float\n", s);
		return 1;
	}
	if ((f < 0) || (f > 1)) {
		fprintf(stderr, "Option value \"%s\" must be in the range [0,1]\n",
				s);
		return 1;
	}
	*v = f;
	return 0;

}

static char *Usage = "Usage: %s [-?] [options]\n\
 -r<n>         : sparsification ratio [0,1] (0.5) \n\
 -o<outfile>   : put output in <outfile> (stdout)\n\
 -v            : verbose mode \n\
 -?            : print usage\n";

static void usage(char *cmd, int v)
{
	fprintf(v ? stderr : stdout, Usage, cmd);
	exit(v);
}

static void doOpts(int argc, char *argv[], opts_t * op)
{
	int c;
	char *cmd = argv[0];

	op->outfp = NULL;
	op->Files = NULL;
	op->verbose = 0;
	op->sparse_ratio = 0.5;

	opterr = 0;
	while ((c = getopt(argc, argv, "o:r:v")) != -1) {
		switch (c) {
		case 'o':
			op->outfp = openFile(cmd, optarg, "w");
			break;
		case 'v':
			op->verbose = 1;
			break;
		case 'r':
			if (setFloat(optarg, &(op->sparse_ratio))) {
				fprintf(stderr, "%s: bad value for flag -%c - ignored\n",
						cmd, c);
			}
			break;
		case '?':
			if (optopt == '?')
				usage(cmd, 0);
			else
				fprintf(stderr, "%s: option -%c unrecognized - ignored\n",
						cmd, optopt);
			break;
		}
	}
	argv += optind;
	argc -= optind;

	if (argc)
		op->Files = argv;
	if (op->outfp == NULL)
		op->outfp = stdout;
	if (op->verbose)
		fprintf(stderr, "sparse ratio = %f\n", op->sparse_ratio);
}

static Agraph_t *gread(FILE * fp)
{
	return agread(fp, (Agdisc_t *) 0);
}

int main(int argc, char *argv[])
{
	opts_t opts;
	ingraph_state ig;
	Agraph_t *g;

	doOpts(argc, argv, &opts);
	newIngraph(&ig, opts.Files, gread);
	while ((g = nextGraph(&ig)) != 0) {
		genSpine(g, ops.sparse_ratio, ops.verbose);
		agwrite(g, ops.outfp);
		agclose(g);
	}

	return 0;
}
#endif
