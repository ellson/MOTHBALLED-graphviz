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


#ifndef SPARSE_SOLVER_H
#define SPARSE_SOLVER_H

#include "SparseMatrix.h"

enum { SOLVE_METHOD_CG };

typedef struct Operator_s* Operator;

typedef real *(*op_apply_fn) (Operator o, real * in, real * out);

struct Operator_s {
    void *data;
    op_apply_fn Operator_apply;
};

real SparseMatrix_solve(SparseMatrix A, int dim, real * x0, real * rhs,
			real tol, int maxit, int method, int *flag);

#endif
