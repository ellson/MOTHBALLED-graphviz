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


#include "matrix_ops.h"
#include "pca.h"
#include "closest.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int num_pairs = 4;

void
PCA_alloc(DistType ** coords, int dim, int n, double **new_coords,
	  int new_dim)
{
    double **DD = NULL;		/* dim*dim matrix: coords*coords^T */
    double sum;
    int i, j, k;
    double **eigs = NULL;
    double *evals = NULL;
    double *storage_ptr;

    eigs = N_GNEW(new_dim, double *);
    for (i = 0; i < new_dim; i++)
	eigs[i] = N_GNEW(dim, double);
    evals = N_GNEW(new_dim, double);

    DD = N_GNEW(dim, double *);
    storage_ptr = N_GNEW(dim * dim, double);
    for (i = 0; i < dim; i++) {
	DD[i] = storage_ptr;
	storage_ptr += dim;
    }

    for (i = 0; i < dim; i++) {
	for (j = 0; j <= i; j++) {
	    /* compute coords[i]*coords[j] */
	    sum = 0;
	    for (k = 0; k < n; k++) {
		sum += coords[i][k] * coords[j][k];
	    }
	    DD[i][j] = DD[j][i] = sum;
	}
    }

    power_iteration(DD, dim, new_dim, eigs, evals, true);

    for (j = 0; j < new_dim; j++) {
	for (i = 0; i < n; i++) {
	    sum = 0;
	    for (k = 0; k < dim; k++) {
		sum += coords[k][i] * eigs[j][k];
	    }
	    new_coords[j][i] = sum;
	}
    }

    for (i = 0; i < new_dim; i++)
	free(eigs[i]);
    free(eigs);
    free(evals);
    free(DD[0]);
    free(DD);
}

bool
iterativePCA_1D(double **coords, int dim, int n, double *new_direction)
{
    vtx_data *laplacian;
    float **mat1 = NULL;
    double **mat = NULL;
    double eval;

    /* Given that first projection of 'coords' is 'coords[0]'
       compute another projection direction 'new_direction'
       that scatters points that are close in 'coords[0]'
     */

    /* find the nodes that were close in 'coords[0]' */
    /* and construct appropriate Laplacian */
    closest_pairs2graph(coords[0], n, num_pairs * n, &laplacian);

    /* Compute coords*Lap*coords^T */
    mult_sparse_dense_mat_transpose(laplacian, coords, n, dim, &mat1);
    mult_dense_mat_d(coords, mat1, dim, n, dim, &mat);
    free(mat1[0]);
    free(mat1);

    /* Compute direction */
    return power_iteration(mat, dim, 1, &new_direction, &eval, true);
/* ?? When is mat freed? */
}

#ifdef UNUSED

double dist(double **coords, int dim, int p1, int p2)
{
    int i;
    double sum = 0;

    for (i = 0; i < dim; i++) {
	sum +=
	    (coords[i][p1] - coords[i][p2]) * (coords[i][p1] -
					       coords[i][p2]);
    }
    return sqrt(sum);
}


void weight_laplacian(double **X, int n, int dim, vtx_data * laplacian)
{
    int i, j, neighbor;

    int *edges;
    float *ewgts;
    for (i = 0; i < n; i++) {
	edges = laplacian[i].edges;
	ewgts = laplacian[i].ewgts;
	*ewgts = 0;
	for (j = 1; j < laplacian[i].nedges; j++) {
	    neighbor = edges[j];
	    *ewgts -= ewgts[j] =
		float (-1.0 / (dist(X, dim, i, neighbor) + 1e-10));
	}
    }
}

#endif
