/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

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
