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

#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <spring_electrical.h>

struct StressMajorizationSmoother_s {
    SparseMatrix Lw;
    SparseMatrix Lwd;
    real *lambda;
};

typedef struct StressMajorizationSmoother_s* StressMajorizationSmoother;

void StressMajorizationSmoother_delete(StressMajorizationSmoother  sm);

enum { IDEAL_GRAPH_DIST, IDEAL_AVG_DIST, IDEAL_POWER_DIST };
StressMajorizationSmoother StressMajorizationSmoother_new(SparseMatrix 
							   A, int dim,
							   real lambda,
							   real * x,
							   int
							   ideal_dist_scheme);

void StressMajorizationSmoother_smooth(StressMajorizationSmoother  sm,
				       int dim, real * x, int maxit);
/*-------------------- triangle/neirhborhood graph based smoother ------------------- */
typedef StressMajorizationSmoother TriangleSmoother;

void TriangleSmoother_delete(TriangleSmoother  sm);

TriangleSmoother TriangleSmoother_new(SparseMatrix  A, int dim,
				       real lambda, real * x,
				       int use_triangularization);

void TriangleSmoother_smooth(TriangleSmoother  sm, int dim, real * x);



/*------------------ spring and spring-electrical based smoother */

struct SpringSmoother_s {
    SparseMatrix D;
    spring_electrical_control ctrl;
};
typedef struct SpringSmoother_s* SpringSmoother;

SpringSmoother SpringSmoother_new(SparseMatrix  A, int dim,
				   spring_electrical_control  ctrl,
				   real * x);

void SpringSmoother_delete(SpringSmoother  sm);

void SpringSmoother_smooth(SpringSmoother  sm, SparseMatrix  A,
			   real * node_weights, int dim, real * x);
/*------------------------------------------------------------------*/

int post_process_smoothing(int dim, SparseMatrix  A,
			    spring_electrical_control  ctrl,
			    real * node_weights, real * x);

#endif
