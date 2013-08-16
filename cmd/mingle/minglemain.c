
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
    #pragma comment( lib, "cdt.lib" )
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

typedef enum {
	FMT_GV,
	FMT_SIMPLE,
} fmt_t;

typedef struct {
	int outer_iter;
	int method; 
	int compatibility_method;
	real K;
	fmt_t fmt;
	int nneighbors;
	int max_recursion;
	real angle_param;
	real angle;
} opts_t;

static char *fname;
static FILE *outfile;

static FILE *openFile(char *name, char *mode, char* cmd)
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
		exit(-1);
	}
	return (fp);
}

static char* use_msg =
#ifdef HAVE_ANN
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
    -T fmt - output format: gv (default) or simple\n\
    -v - verbose\n";
#else
"Usage: mingle <options> <file>\n\
    -a t - max. turning angle [0-180] (40)\n\
    -c i - compatability measure; 0 : distance, 1: full (default)\n\
    -i iter: number of outer iterations/subdivisions (4)\n\
    -k k - number of neighbors in the nearest neighbor graph of edges (10)\n\
    -K k - the force constant\n\
    -m method - method used. 0 (force directed, default), 2 (cluster+ink saving)\n\
    -o fname - write output to file fname (stdout)\n\
    -p t - balance for avoiding sharp angles\n\
           The larger the t, the more sharp angles are allowed\n\
    -r R - max. recursion level with agglomerative ink saving method (100)\n\
    -T fmt - output format: gv (default) or simple\n\
    -v - verbose\n";
#endif

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
	real s;
    int i;

    opterr = 0;
	opts->outer_iter = 4;
#ifdef HAVE_ANN
	opts->method = METHOD_INK_AGGLOMERATE;
#else
	opts->method = METHOD_FD;
#endif
	opts->compatibility_method = COMPATIBILITY_FULL;
	opts->K = -1;
	opts->fmt = FMT_GV;
	opts->nneighbors = 10;
	opts->max_recursion = 100;
	opts->angle_param = -1;
	opts->angle = 40.0/180.0*M_PI;

	while ((c = getopt(argc, argv, ":a:c:i:k:K:m:o:p:r:T:v:")) != -1) {
		switch (c) {
		case 'a':
            if ((sscanf(optarg,"%lf",&s) > 0) && (s >= 0))
				opts->angle =  M_PI*s/180;
			else 
				fprintf (stderr, "-a arg %s must be positive real - ignored\n", optarg); 
			break;
		case 'c':
            if ((sscanf(optarg,"%d",&i) > 0) && (0 <= i) && (i <= COMPATIBILITY_FULL))
				opts->compatibility_method =  i;
			else 
				fprintf (stderr, "-c arg %s must be an integer in [0,%d] - ignored\n", optarg, COMPATIBILITY_FULL); 
			break;
		case 'i':
            if ((sscanf(optarg,"%d",&i) > 0) && (i >= 0))
				opts->outer_iter =  i;
			else 
				fprintf (stderr, "-i arg %s must be a non-negative integer - ignored\n", optarg); 
			break;
		case 'k':
            if ((sscanf(optarg,"%d",&i) > 0) && (i >= 2))
				opts->nneighbors =  i;
			else 
				fprintf (stderr, "-k arg %s must be an integer >= 2 - ignored\n", optarg); 
			break;
		case 'K':
            if ((sscanf(optarg,"%lf",&s) > 0) && (s > 0))
				opts->K =  s;
			else 
				fprintf (stderr, "-K arg %s must be positive real - ignored\n", optarg); 
			break;
		case 'm':
            if ((sscanf(optarg,"%d",&i) > 0) && (0 <= i) && (i <= METHOD_INK)
#ifndef HAVE_ANN
				&& (i != METHOD_INK_AGGLOMERATE)
#endif 
               )
				opts->method =  i;
			else 
				fprintf (stderr, "-k arg %s must be an integer >= 2 - ignored\n", optarg); 
			break;
		case 'o':
			outfile = openFile(optarg, "w", CmdName);
			break;
		case 'p':
            if ((sscanf(optarg,"%lf",&s) > 0))
				opts->angle_param =  s;
			else 
				fprintf (stderr, "-p arg %s must be real - ignored\n", optarg); 
			break;
		case 'r':
            if ((sscanf(optarg,"%d",&i) > 0) && (i >= 0))
				opts->max_recursion =  i;
			else 
				fprintf (stderr, "-r arg %s must be a non-negative integer - ignored\n", optarg); 
			break;
		case 'T':
			if ((*optarg == 'g') && ((*(optarg+1) == 'v'))) 
				opts->fmt = FMT_GV;
			else if ((*optarg == 's') && (!strcmp(optarg+1,"imple"))) 
				opts->fmt = FMT_SIMPLE;
			else
				fprintf (stderr, "-T arg %s must be \"gv\" or \"simple\" - ignored\n", optarg); 
			break;
		case 'v':
			Verbose = 1;
            if ((sscanf(optarg,"%d",&i) > 0) && (i >= 0))
				Verbose =  i;
			else 
				fprintf (stderr, "-v arg %s must be a non-negative integer - ignored\n", optarg); 
			break;
		case ':':
			if (optopt == 'v')
				Verbose = 1;
			else {
				fprintf(stderr, "%s: option -%c missing argument\n", CmdName, optopt);
				usage(1);
			}
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
    if (!outfile) outfile = stdout;
    if (Verbose) {
       fprintf (stderr, "Mingle params:\n");
       fprintf (stderr, "  outer_iter = %d\n", opts->outer_iter);
       fprintf (stderr, "  method = %d\n", opts->method);
       fprintf (stderr, "  compatibility_method = %d\n", opts->compatibility_method);
       fprintf (stderr, "  K = %.02f\n", opts->K);
       fprintf (stderr, "  fmt = %s\n", (opts->fmt?"simple":"gv"));
       fprintf (stderr, "  nneighbors = %d\n", opts->nneighbors);
       fprintf (stderr, "  max_recursion = %d\n", opts->max_recursion);
       fprintf (stderr, "  angle_param = %.02f\n", opts->angle_param);
       fprintf (stderr, "  angle = %.02f\n", 180*opts->angle/M_PI);
    }
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
		agerr (AGERR, "Error: could not convert graph %s (%s) into matrix\n", agnameof(g), fname);
		return 1;
    }
    if (x == NULL) {
		agerr (AGPREV, " in file %s\n",  fname);
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
	
	if (opts->fmt == FMT_GV)
    	pedge_export_gv(outfile, A->m, edges);   /* FIX */
    else
    	pedge_export_gv(outfile, A->m, edges);
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
