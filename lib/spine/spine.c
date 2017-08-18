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

typedef int (*qsort_cmpf) (const void *, const void *);

#define MIN(a,b)	((a)<(b)?(a):(b))
#define MAX(a,b)	((a)>(b)?(a):(b))

void* mcalloc (size_t cnt, size_t sz)
{
	void* p = calloc(cnt, sz);
	/* fprintf(stderr, "alloc %lu bytes %p\n", cnt*sz, p); */
	return p;
}
 
#if 0
static float ewt(Agedge_t * e)
{
	return ED_wt(e);
}
#endif

static int cmpe(void *v0, void *v1)
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

static void
reweightEdge (Agedge_t* e, Dt_t* nbr0, Dt_t* nbr1, Agedge_t*** nbrs, int verbose)
{
	Agnode_t* tail = agtail(e);
	Agnode_t* head = aghead(e);
	Agedge_t** tail_edgelist = nbrs[ND_id(tail)];
	long int len0 = nbrs[ND_id(tail)+1] - tail_edgelist;
	Agedge_t** head_edgelist = nbrs[ND_id(head)];
	long int len1 = nbrs[ND_id(head)+1] - head_edgelist;
	long int minlen = MIN(len0, len1);
	long int i;
	long int maxi = 0;
	double wt, maxwt = 0;
	double common_cnt = 0;
	double union_cnt = 0;
	Agnode_t* n0;
	Agnode_t* n1;
	double oldwt;

	clearSubset(nbr0);
	clearSubset(nbr1);
		
	if (verbose > 1)
		oldwt = ED_wt(e);

#ifdef TEST_JACCARD
    Dt_t* nbr00 = mkSubset();
	Dt_t* nbr11 = mkSubset();
    double wt0;
#endif
	for (i = 0; i < minlen; i++) {
		n0 = (*tail_edgelist++)->node;
		n1 = (*head_edgelist++)->node;
		assert(n0 != tail);
		assert(n1 != head);
#ifdef TEST_JACCARD
		addSubset(nbr00, n0);
		addSubset(nbr11, n1);
#endif
/* fprintf (stderr, "add %s  %s\n", agnameof(n0), agnameof(n1)); */
		if (n0 != n1) {
			if (inSubset(nbr1, n0)) common_cnt++;
			else {
				addSubset (nbr0, n0);
				union_cnt++;
			}
			if (inSubset(nbr0, n1)) common_cnt++;
            else {
				addSubset (nbr1, n1);
				union_cnt++;
			}
		}
		else {
			common_cnt++;
			union_cnt++;
		}
		wt = common_cnt/union_cnt;
#ifdef TEST_JACCARD
		wt0 = ((double)intersect_size(nbr00,nbr11))/union_size(nbr00,nbr11);
		assert(wt == wt0);
#endif
		if (wt > maxwt) {
			maxi = i;
			maxwt = wt;
		}
	}
	if (len0 > minlen) {
		for (; i < len0; i++) {
			n0 = (*tail_edgelist++)->node;
/* fprintf (stderr, "add %s  -\n", agnameof(n0)); */
#ifdef TEST_JACCARD
			addSubset(nbr00, n0);
#endif
			if (inSubset(nbr1, n0)) common_cnt++;
			else union_cnt++;
			wt = common_cnt/union_cnt;
#ifdef TEST_JACCARD
			wt0 = ((double)intersect_size(nbr00,nbr11))/union_size(nbr00,nbr11);
			assert(wt == wt0);
#endif
			if (wt > maxwt) {
				maxi = i;
				maxwt = wt;
			}
		}
	}
	else if (len1 > minlen) {
		for (; i < len1; i++) {
			n1 = (*head_edgelist++)->node;
/* fprintf (stderr, "add - %s\n", agnameof(n1)); */
#ifdef TEST_JACCARD
			addSubset(nbr11, n1);
#endif
			if (inSubset(nbr0, n1)) common_cnt++;
			else union_cnt++;
			wt = common_cnt/union_cnt;
#ifdef TEST_JACCARD
			wt0 = ((double)intersect_size(nbr00,nbr11))/union_size(nbr00,nbr11);
			assert(wt == wt0);
#endif
			if (wt > maxwt) {
				maxi = i;
				maxwt = wt;
			}
		}
	}
	
	ED_wt(e) = maxwt;
	if (verbose > 1)
		fprintf(stderr, "%s : %s %f %ld/%ld %f\n",
			agnameof(tail), agnameof(head), maxwt, maxi, MAX(len0, len1), oldwt);
#ifdef TEST_JACCARD
    closeSubset(nbr00);
    closeSubset(nbr11);
#endif
}

static void setEdgeWeights(Agraph_t * g, int verbose)
{
	int *quadcnt = N_NEW((size_t)agnedges(g), int);
	int ncnt = agnnodes(g);
	int *que = N_NEW((size_t)ncnt, int);
	Agnode_t *n;
	Agedge_t *e;
	int iwt;
	int edgecnt = 0; 
	long int i = 0;

    /* Count the number q(u,v) of quadrilaterals each edge is in.
     * This is stored in quadcnt, indexed by the edges index. 
     */
	genQuads(g, recordQuads, quadcnt);

    /* Weight node v by q(v), the sum of the q(u,v) for all neighbors v of u.
     * This is stored in que, indexed by the node's index.
     */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		ND_id(n) = i++;
		iwt = 0;
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			iwt += quadcnt[ED_id(e)];
			edgecnt++;
		}
		for (e = agfstin(g, n); e; e = agnxtin(g, e)) {
			iwt += quadcnt[ED_id(e)];
		}
		que[ND_id(n)] = iwt;
#if 0
		fprintf(stderr, " %s quad %d\n", agnameof(n), que[ND_id(n)]);
#endif
	}

    /* Assign each edge a normalized initial weight Q(u,v) which is q(u,v)/sqrt(q(u)q(v)).
     * This is stored in ED_wt.
     */
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
					jD_wt(e));
#endif
		}
	}

	free(quadcnt);
	free(que);

	Agedge_t** edges = N_NEW((size_t)(2*edgecnt+1),Agedge_t*);
	Agedge_t*** nbrs = N_NEW((size_t)ncnt+1, Agedge_t**);
	Agedge_t** edgelist = edges;
	size_t degree;
    /* For each node, sort its edges by Q(u,v). */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		degree = 0;
		nbrs[ND_id(n)] = edgelist;
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			degree++;
			*edgelist++ = e;
		}
		for (e = agfstin(g, n); e; e = agnxtin(g, e)) {
			degree++;
			*edgelist++ = e;
		}
		qsort(nbrs[ND_id(n)], degree, sizeof(Agedge_t **), (qsort_cmpf)cmpe);
/*
		fprintf (stderr, "sort %s(%d) degree %lu %d %p\n", 
			agnameof(n), ND_id(n), degree, agdegree(g,n,1,1), nbrs[ND_id(n)]);
*/
	}
	nbrs[ncnt] = edgelist;

#if 0
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		edgelist = nbrs[ND_id(n)];
		long int len0 = nbrs[ND_id(n)+1] - edgelist;
		fprintf (stderr, "%s len %lu degree %d\n", agnameof(n), len0, agdegree(g,n,1,1));
		for (i = 0; i < len0; i++) {
			e = *edgelist++;
			fprintf (stderr, "%s %s %lf\n",
				agnameof(agtail(e)), agnameof(aghead(e)), ED_wt(e));
		}
	}
#endif

    /* Finally, for each edge (u,v), compute the Jaccard coefficient of the union of
     * k neighbors of u and v, using the sorted edges. The final edge weight is the
     * maximum Jaccard coefficient.
     */
	Dt_t* nbr0 = mkSubset();
	Dt_t* nbr1 = mkSubset();
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		edgelist = nbrs[ND_id(n)];
		long int len0 = nbrs[ND_id(n)+1] - edgelist;
		for (i = 0; i < len0; i++) {
			e = *edgelist++;
			if (AGTYPE(e)==AGINEDGE) continue;
			reweightEdge (e, nbr0, nbr1, nbrs, verbose);
		}
	}
	closeSubset(nbr0);
	closeSubset(nbr1);
	free(edges);
	free(nbrs);
}

/* Return number in range [0,nedges] */
static size_t computeIndex(size_t nedges, float s)
{
	size_t r = ceilf(nedges * (1 - s));
	return r;
}

static size_t doBucket(Agedge_t ** edgelist, size_t idx, Dt_t * M)
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

static Agraph_t *findUMST(Agraph_t * g, Agedge_t ** edgelist, size_t nedges)
{
	Agraph_t *sg = agsubg(g, "UMST", 1);
	Dt_t *M = mkSubset();

	size_t i = 0;
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

/* Remove loops and multiedges. */
static void cleanGraph (Agraph_t * g, int verbose)
{
	Agnode_t *n;
	Agnode_t *head;
	Agedge_t *e;
	Agedge_t *nexte;
	Agedge_t *inexte;
	Agedge_t *backe;
	Agedge_t *preve = NULL;
    int loopcnt = 0;
    int multicnt = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		preve = NULL;
		for (e = agfstout(g, n); e; e = nexte) {
			head = aghead(e);
			nexte = agnxtout(g, e);
			if (head == n) {
				agdelete(g, e);
				preve = NULL;
				loopcnt++;
			}
			else if (preve && (head == aghead(preve))) {
				agdelete(g, e);
				multicnt++;
			}
			else {
				preve = e;
				if ((backe = agedge(g, head, n, 0, 0))) {
					while (backe && (agtail(backe) == head)) {
						multicnt++;
						inexte = agnxtin(g, backe);	
						agdelete(g, backe);
						backe = inexte;
					}
				}
			
			}
		}
	}
	if (verbose)
		fprintf (stderr, "cleanGraph: %d loops %d multiedges removed\n", loopcnt, multicnt);
}

void genSpine(Agraph_t * g, float sparse_ratio, int verbose)
{
	Agraph_t *sg_union;
	Agnode_t *n;
	Agedge_t *e;
	Agedge_t **edgelist;
	size_t i, index;
	size_t nedges;
	float threshold;

	cleanGraph (g, verbose);
	nedges = agnedges(g);
	if (verbose)
		fprintf(stderr, "Graph %s %d nodes %lu edges:\n", agnameof(g),
				agnnodes(g), nedges);
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
	setEdgeWeights(g, verbose);

	/* sort edges by non-increasing weight */
	if (verbose)
		fprintf(stderr, "sorting\n");
	edgelist = N_NEW((size_t)(nedges + 1), Agedge_t *);	/* NULL at end */
	i = 0;
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
			edgelist[i++] = e;
		}
	}
	qsort(edgelist, nedges, sizeof(Agedge_t *), (qsort_cmpf)cmpe);
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
		fprintf(stderr, " index %lu out of %lu\n", index, nedges);

	/* set of edges with weights above threshold */
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
		threshold = ED_wt(e);
		for (; i <= nedges; i++) {
			e = edgelist[i - 1];
			if (ED_wt(e) >= threshold) {
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
	Agsym_t *sym = agattr(g, AGEDGE, "spine", "0");
	int ncnt = 0;
	int ecnt = 0;
	for (n = agfstnode(sg_union); n; n = agnxtnode(sg_union, n)) {
		ncnt++;
		for (e = agfstout(sg_union, n); e; e = agnxtout(sg_union, e)) {
			ecnt++;
			agxset(e, sym, "1");
		}
	}
	if (verbose)
		fprintf(stderr, "final ncnt %d ecnt %d\n", ncnt, ecnt);
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

static int setInt(char *s, int *v)
{
	char *endp;
	long int l = strtol(s, char &endp, 10);

	if (s == endp) {
		fprintf(stderr, "Option value \"%s\" must be an integer\n", s);
		return 1;
	}
	if (l < 0) {
		fprintf(stderr, "Option value \"%s\" must be non-negative.\n", s);
		return 1;
	}
	*v = (int)l;
	return 0;

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
 -o<outfile>   : put output in <outfile> (stderr)\n\
 -v[<val>]     : verbose mode \n\
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
	while ((c = getopt(argc, argv, "o:r:v::")) != -1) {
		switch (c) {
		case 'o':
			op->outfp = openFile(cmd, optarg, "w");
			break;
		case 'v':
			if (optarg)
				if (setInt(optarg, &(op->verbose))) {
					fprintf(stderr, "%s: bad value for flag -%c - ignored\n",
							cmd, c);
				}
			}
			else
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
		default:
			break;
		}
	}
	argv += optind;
	argc -= optind;

	if (argc)
		op->Files = argv;
	if (op->outfp == NULL)
		op->outfp = stderr;
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
		genSpine(g, opts.sparse_ratio, opts.verbose);
		agwrite(g, opts.outfp);
		agclose(g);
	}

	return 0;
}
#endif
