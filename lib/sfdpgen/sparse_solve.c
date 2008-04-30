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

#include "sparse_solve.h"
#include "memory.h"
#include "logic.h"
#include "math.h"
#include "assert.h"
#include "string.h"

static real *vector_subtract_to(int n, real * x, real * y)
{
    /* y = x-y */
    int i;
    for (i = 0; i < n; i++)
	y[i] = x[i] - y[i];
    return y;
}

static real *vector_saxpy(int n, real * x, real * y, real beta)
{
    /* y = x+beta*y */
    int i;
    for (i = 0; i < n; i++)
	y[i] = x[i] + beta * y[i];
    return y;
}

static real *vector_saxpy2(int n, real * x, real * y, real beta)
{
    /* x = x+beta*y */
    int i;
    for (i = 0; i < n; i++)
	x[i] = x[i] + beta * y[i];
    return x;
}

static real vector_product(int n, real *x, real *y){
  real res = 0;
  int i;
  for (i = 0; i < n; i++) res += x[i]*y[i];
  return res;
}

real *Operator_matmul_apply(Operator * o, real * x, real * y)
{
    SparseMatrix *A = (SparseMatrix *) o->data;
    SparseMatrix_multiply_vector(A, x, &y, FALSE);
    return y;
}

Operator *Operator_matmul_new(SparseMatrix * A)
{
    Operator *o = GNEW(Operator);
    o->data = (void *) A;
    o->Operator_apply = Operator_matmul_apply;
    return o;
}

void Operator_matmul_delete(Operator * o)
{
}

real *Operator_diag_precon_apply(Operator * o, real * x, real * y)
{
    int i, m;
    real *diag = (real *) o->data;
    m = (int) diag[0];
    diag++;
    for (i = 0; i < m; i++)
	y[i] = x[i] * diag[i];
    return y;
}

Operator *Operator_diag_precon_new(SparseMatrix * A)
{
    Operator *o;
    real *diag;
    int i, j, m = A->m, *ia = A->ia, *ja = A->ja;
    real *a = (real *) A->a;

    assert(A->type == MATRIX_TYPE_REAL);

    assert(a);

    o = GNEW(Operator);
    o->data = N_GNEW(A->m + 1, real);
    diag = (real *) o->data;

    diag[0] = m;
    diag++;
    for (i = 0; i < m; i++) {
	diag[i] = 1.;
	for (j = ia[i]; j < ia[i + 1]; j++) {
	    if (i == ja[j] && ABS(a[j]) > 0)
		diag[i] = 1. / a[j];
	}
    }

    o->Operator_apply = Operator_diag_precon_apply;

    return o;
}

void Operator_diag_precon_delete(Operator * o)
{
    free(o->data);
}

static real conjugate_gradient(Operator * A, Operator * precon, int n, real * x,
			real * rhs, real tol, int maxit, int *flag)
{
    real *z, *r, *p, *q, res = 10 * tol, alpha;
    real rho = 1.0e20, rho_old = 1, res0, beta;
    op_apply_fn Ax = A->Operator_apply;
    op_apply_fn Minvx = precon->Operator_apply;
    int iter = 0;

    z = N_GNEW(n, real);
    r = N_GNEW(n, real);
    p = N_GNEW(n, real);
    q = N_GNEW(n, real);

    r = Ax(A, x, r);
    r = vector_subtract_to(n, rhs, r);

    res0 = res = sqrt(vector_product(n, r, r)) / n;
#ifdef DEBUG_PRINT
    if (Verbose && 0) {
	fprintf(stderr, "   cg iter = %d, residual = %g\n", iter, res);
    }
#endif

    while ((iter++) < maxit && res > tol * res0) {
	z = Minvx(precon, r, z);
	rho = vector_product(n, r, z);

	if (iter > 1) {
	    beta = rho / rho_old;
	    p = vector_saxpy(n, z, p, beta);
	} else {
	    memcpy(p, z, sizeof(real) * n);
	}

	q = Ax(A, p, q);

	alpha = rho / vector_product(n, p, q);

	x = vector_saxpy2(n, x, p, alpha);
	r = vector_saxpy2(n, r, q, -alpha);

	res = sqrt(vector_product(n, r, r)) / n;

#ifdef DEBUG_PRINT
	if (Verbose && 0) {
	    fprintf(stderr, "   cg iter = %d, residual = %g\n", iter, res);
	}
#endif



	rho_old = rho;
    }
#ifdef DEBUG
    _statistics[0] += iter - 1;
#endif

#ifdef DEBUG_PRINT
    if (Verbose && 0) {
	fprintf(stderr, "   cg iter = %d, residual = %g\n", iter, res);
    }
#endif
    return res;
}

real SparseMatrix_solve(SparseMatrix * A, int dim, real * x0, real * rhs,
			real tol, int maxit, int method, int *flag)
{
    Operator *Ax;
    Operator *precond;
    real *x, *b, res = 0;
    int n = A->m, k, i;

    *flag = 0;

    switch (method) {
    case SOLVE_METHOD_CG:
	Ax = Operator_matmul_new(A);
	precond = Operator_diag_precon_new(A);

	x = N_GNEW(n, real);
	b = N_GNEW(n, real);
	for (k = 0; k < dim; k++) {
	    for (i = 0; i < n; i++) {
		x[i] = x0[i * dim + k];
		b[i] = rhs[i * dim + k];
	    }

	    res +=
		conjugate_gradient(Ax, precond, n, x, b, tol, maxit, flag);
	    for (i = 0; i < n; i++) {
		rhs[i * dim + k] = x[i];
	    }
	}
	Operator_matmul_delete(Ax);
	Operator_diag_precon_delete(precond);
	free(x);
	free(b);

	break;
    default:
	assert(0);
	break;

    }
    return res;
}

