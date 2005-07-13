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

#include "digcola.h"
#ifdef DIGCOLA
#include "matrix_ops.h"
#include "conjgrad.h"

static void
construct_b(vtx_data* graph, int n, double *b) 
{
	/* construct a vector - b s.t. -b[i]=\sum_j -w_{ij}*\delta_{ij}
	 * (the "balance vector")
	 * Note that we build -b and not b, since our matrix is not the 
	 * real laplacian L, but its negation: -L. 
	 * So instead of solving Lx=b, we will solve -Lx=-b
     */
	int i,j;

	double b_i = 0;
	
	for (i=0; i<n; i++) {
		b_i = 0;
		if (graph[0].edists==NULL) {
			continue;
		}
		for (j=1; j<graph[i].nedges; j++) { /* skip the self loop */
			b_i += graph[i].ewgts[j]*graph[i].edists[j];
		}
		b[i] = b_i;
	}
}

#define hierarchy_cg_tol 1e-3

void 
compute_y_coords(vtx_data* graph, int n, double* y_coords, int max_iterations)
{
	/* Find y coords of a directed graph by solving L*x = b */
	int i,j;	
	double* b = N_NEW(n, double);
    double tol = hierarchy_cg_tol;
	int nedges = 0;
	float* uniform_weights;
	float* old_ewgts = graph[0].ewgts;
	
	construct_b(graph, n, b);

	init_vec_orth1(n, y_coords);

	for (i=0; i<n; i++) {
		nedges += graph[i].nedges;
	}
	
	/* replace original edge weights (which are lengths) with uniform weights */
	/* for computing the optimal arrangement */
	uniform_weights = N_GNEW(nedges,float);
	for (i=0; i<n; i++) {
		graph[i].ewgts = uniform_weights;
		uniform_weights[0] = (float)-(graph[i].nedges-1);
		for (j=1; j<graph[i].nedges; j++) {
			uniform_weights[j] = 1;
		}
		uniform_weights += graph[i].nedges;
	}
	
	conjugate_gradient(graph, y_coords, b, n, tol, max_iterations);

	/* restore original edge weights */
	free (graph[0].ewgts);	
	for (i=0; i<n; i++) {
		graph[i].ewgts = old_ewgts;
		old_ewgts += graph[i].nedges;
	}

	free(b);
}

#endif /* DIGCOLA */

