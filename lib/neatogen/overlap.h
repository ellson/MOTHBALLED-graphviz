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

#ifndef OVERLAP_H
#define OVERLAP_H

#ifdef SMOOTHER
#include "post_process.h"

typedef  StressMajorizationSmoother OverlapSmoother;

#define OverlapSmoother_struct StressMajorizationSmoother_struct

void OverlapSmoother_delete(OverlapSmoother sm);

OverlapSmoother OverlapSmoother_new(SparseMatrix A, int m, int dim, real lambda0, real *x, real *width, int include_original_graph, int neighborhood_only,
				    real *max_overlap, real *min_overlap, int shrink);

void OverlapSmoother_smooth(OverlapSmoother sm, int dim, real *x);

#else
#include "SparseMatrix.h"
#endif

void remove_overlap(int dim, SparseMatrix A, int m, real *x, real *label_sizes, int ntry, real initial_scaling, int *flag);
#endif
