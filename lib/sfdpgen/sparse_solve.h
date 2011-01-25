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


#ifndef SPARSE_SOLVER_H
#define SPARSE_SOLVER_H

#include "SparseMatrix.h"

enum {SOLVE_METHOD_CG};


typedef struct Operator_struct *Operator;

struct Operator_struct {
  void *data;
  real* (*Operator_apply)(Operator o, real *in, real *out);
};

real SparseMatrix_solve(SparseMatrix A, int dim, real *x0, real *rhs, real tol, int maxit, int method, int *flag);

#endif
 
