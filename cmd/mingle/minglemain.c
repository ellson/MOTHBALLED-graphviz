
/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32 /*dependencies*/
    #pragma comment( lib, "cgraph.lib" )
    #pragma comment( lib, "gvc.lib" )
    #pragma comment( lib, "ingraphs.lib" )
    #pragma comment( lib, "sparse.lib" )
    #pragma comment( lib, "sfdp.lib" )
    #pragma comment( lib, "minglelib.lib" )
    #pragma comment( lib, "neatogen.lib" )
    #pragma comment( lib, "rbtree.lib" )
    #pragma comment( lib, "common.lib" )
#endif   /* not WIN32_DLL */

#include <cgraph.h>
#include <ingraphs.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "compat_getopt.h"
#endif

#include "DotIO.h"
#include "edge_bundling.h"
#include "nearest_neighbor_graph.h"

typedef struct {
	int outer_iter;
	int method; 
	int compatibility_method;
	real K;
	char* fmt;
	int nneighbors;
	int max_recursion;
	real angle_param;
	real angle;
} opts_t;

static char *fname;
static FILE *outfile;

static char* use_msg =
"Usage: mingle <options> <file>\n\
    -a t - max. turning angle [0-180] (40)\n\
    -c i - compatability measure; 0 : distance, 1: full (default)\n\
    -i iter: number of outer iterations/subdivisions (4)\n\
    -k k - number of neighbors in the nearest neighbor graph of edges (10)\n\
    -K k - the force constant\n\
    -m method - method used. 0 (force directed), 1 (agglomerative ink saving, default), 2 (cluster+ink saving)\n\
    -o fname - write output to file fname (stdout)\n\
    -p t - balance for avoiding sharp angles\n\
           The larger the t, the more sharp angles are allowed\n\
    -r R - max. recursion level with agglomerative ink saving method (100)\n\
    -T t - output format\n\
    -v - verbose\n";

static void
usage (int eval)
{
	fputs (use_msg, stderr);
	if (eval >= 0) exit (eval);
}

static void init(int argc, char *argv[], opts_t* opts)
{
	unsigned int c;
	char* CmdName = argv[0];

    opterr = 0;
	opts->outer_iter = 4;
#ifdef HAVE_ANN
	opts->method = METHOD_INK_AGGLOMERATE;
#else
	opts->method = METHOD_FD;
#endif
	opts->compatibility_method = COMPATIBILITY_FULL;
	opts->K = -1;
	opts->fmt = "gv";
	opts->nneighbors = 10;
	opts->max_recursion = 100;
	opts->angle_param = -1;
	opts->angle = 40/180*M_PI;

	while ((c = getopt(argc, argv, ":a:c:i:k:K:m:o:p:r:T:v")) != -1) {
		switch (c) {
		case 'v':
			Verbose = 1;
			break;
		case ':':
			fprintf(stderr, "%s: option -%c missing argument\n", CmdName, optopt);
			usage(1);
			break;
		case '?':
			if (optopt == '?')
				usage(0);
			else
				fprintf(stderr, "%s: option -%c unrecognized - ignored\n", CmdName, optopt);
			break;
		}
    }
    argv += optind;
    argc -= optind;

    if (argc > 0)
		Files = argv;
    outfile = stdout;
}

static int
bundle (Agraph_t* g, opts_t* opts)
{
	real *x = NULL;
	real *label_sizes = NULL;
	int n_edge_label_nodes;
	int dim = 2;
	SparseMatrix A;
	SparseMatrix B;
	pedge* edges;
    real *xx, eps = 0.;
    int nz = 0;
    int *ia, *ja, i, j;
	int rv = 0;

    initDotIO(g);
	A = SparseMatrix_import_dot(g, dim, &label_sizes, &x, &n_edge_label_nodes, NULL, FORMAT_CSR, NULL);
	if (!A){
		fprintf (stderr, "Error: could not convert graph %s (%s) into matrix\n", agnameof(g), fname);
		return 1;
    }
    if (x == NULL) {
		fprintf (stderr, "Error: graph %s (%s) has missing \"pos\" information\n", agnameof(g), fname);
		return 1;
    }

	A = SparseMatrix_symmetrize(A, TRUE);
	ia = A->ia; ja = A->ja;
	nz = A->nz;
	xx = MALLOC(sizeof(real)*nz*4);
	nz = 0;
	dim = 4;
	for (i = 0; i < A->m; i++){
		for (j = ia[i]; j < ia[i+1]; j++){
			if (ja[j] > i){
				xx[nz*dim] = x[i*2];
				xx[nz*dim+1] = x[i*2+1];
				xx[nz*dim+2] = x[ja[j]*2];
				xx[nz*dim+3] = x[ja[j]*2+1];
				nz++;
			}
		}
	}
	if (Verbose)
		fprintf(stderr,"n = %d nz = %d\n",A->m, nz);

	B = nearest_neighbor_graph(nz, MIN(opts->nneighbors, nz), dim, xx, eps);

	SparseMatrix_delete(A);
	A = B;
	FREE(x);
	x = xx;

	dim = 2;

	edges = edge_bundling(A, 2, x, opts->outer_iter, opts->K, opts->method, opts->nneighbors, opts->compatibility_method, opts->max_recursion, opts->angle_param, opts->angle, 0);
	
    pedge_export_gv(stdout, A->m, edges);
	return rv;
}

static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}

int main(int argc, char *argv[])
{
	Agraph_t *g;
	Agraph_t *prev = NULL;
	ingraph_state ig;
	int rv = 0;
	opts_t opts;

	init(argc, argv, &opts);
	newIngraph(&ig, Files, gread);

	while ((g = nextGraph(&ig)) != 0) {
		if (prev)
		    agclose(prev);
		prev = g;
		fname = fileName(&ig);
		if (Verbose)
		    fprintf(stderr, "Process graph %s in file %s\n", agnameof(g),
			    fname);
		rv |= bundle (g, &opts);
	}

	return rv;
}
