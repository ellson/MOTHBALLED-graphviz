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


#ifndef STRESS_H
#define STRESS_H

#include "defs.h"

#define tolerance_cg 1e-3

#define DFLT_ITERATIONS 200

#define DFLT_TOLERANCE 1e-4

    /* some possible values for 'num_pivots_stress' */
#define num_pivots_stress 40
#define num_pivots_smart_ini   0
#define num_pivots_no_ini   50

    /* relevant when using sparse distance matrix
     * when optimizing within subspace it can be set to 0
     * otherwise, recommended value is above zero (usually around 3-6)
     * some possible values for 'neighborhood_radius'
     */
#define neighborhood_radius_unrestricted  4
#define neighborhood_radius_subspace 0


    /* Full dense stress optimization (equivalent to Kamada-Kawai's energy) */
    /* Slowest and most accurate optimization */
    extern int stress_majorization_kD_mkernel(vtx_data * graph,	/* Input graph in sparse representation */
					      int n,	/* Number of nodes */
					      int nedges_graph,	/* Number of edges */
					      double **coords,	/* coordinates of nodes (output layout)  */
					      int dim,	/* dimemsionality of layout */
					      int smart_ini,	/* smart initialization */
					      int model,	/* model */
					      int maxi	/* max iterations */
	);

extern float *compute_apsp_packed(vtx_data * graph, int n);
extern float *compute_apsp_artifical_weights_packed(vtx_data * graph, int n);
extern float* circuitModel(vtx_data * graph, int nG);
extern int initLayout(vtx_data * graph, int n, int dim, double **coords);

#endif

#ifdef __cplusplus
}
#endif
