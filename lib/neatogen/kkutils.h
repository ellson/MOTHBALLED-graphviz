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

#ifdef __cplusplus
extern "C" {
#endif



#ifndef KKUTILS_H_
#define KKUTILS_H_

#include "defs.h"

#ifdef __cplusplus

    inline double distance_kD(double **coords, int dim, int i, int j) {
	/* compute a k-D Euclidean distance between 'coords[*][i]' and 'coords[*][j]' */
	double sum = 0;
	for (int k = 0; k < dim; k++) {
	    sum +=
		(coords[k][i] - coords[k][j]) * (coords[k][i] -
						 coords[k][j]);
	} return sqrt(sum);
    }
    void compute_apsp(vtx_data * graph, int n, DistType ** (&Dij));
    void compute_apsp_artifical_weights(vtx_data * graph, int n,
					DistType ** (&Dij));

    void quicksort_place(double *place, int *ordering, int first,
			 int last);
    void free_graph(vtx_data * (&graph));
#else
    extern void fill_neighbors_vec_unweighted(vtx_data *, int vtx,
					      int *vtx_vec);
    extern int common_neighbors(vtx_data *, int v, int u, int *);
    extern void empty_neighbors_vec(vtx_data * graph, int vtx,
				    int *vtx_vec);
    extern DistType **compute_apsp(vtx_data *, int);
    extern DistType **compute_apsp_artifical_weights(vtx_data *, int);
    extern double distance_kD(double **, int, int, int);
    extern void quicksort_place(double *, int *, int, int);
    extern void compute_new_weights(vtx_data * graph, int n);
    extern void restore_old_weights(vtx_data * graph, int n,
				    float *old_weights);
#endif

#endif

#ifdef __cplusplus
}
#endif
