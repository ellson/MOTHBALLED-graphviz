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
#include "conjgrad.h"
/* #include <math.h> */
#include <stdlib.h>


/*************************
** C.G. method - SPARSE  *
*************************/

void conjugate_gradient
    (vtx_data * A, double *x, double *b, int n, double tol,
     int max_iterations) {
    /* Solves Ax=b using Conjugate-Gradients method */
    /* 'x' and 'b' are orthogonalized against 1 */

    int i;

    double alpha, beta, r_r, r_r_new, p_Ap;
    double *r = N_GNEW(n, double);
    double *p = N_GNEW(n, double);
    double *Ap = N_GNEW(n, double);
    double *Ax = N_GNEW(n, double);
    double *alphap = N_GNEW(n, double);

    double *orth_b = N_GNEW(n, double);
    copy_vector(n, b, orth_b);
    orthog1(n, orth_b);
    orthog1(n, x);
    right_mult_with_vector(A, n, x, Ax);
    vectors_subtraction(n, orth_b, Ax, r);
    copy_vector(n, r, p);
    r_r = vectors_inner_product(n, r, r);

    for (i = 0; i < max_iterations && max_abs(n, r) > tol; i++) {
	right_mult_with_vector(A, n, p, Ap);
	p_Ap = vectors_inner_product(n, p, Ap);
	if (p_Ap == 0)
	    break;		/*exit(1); */
	alpha = r_r / p_Ap;

	/* derive new x: */
	vectors_scalar_mult(n, p, alpha, alphap);
	vectors_addition(n, x, alphap, x);

	/* compute values for next iteration: */
	if (i < max_iterations - 1) {	/* not last iteration */
	    vectors_scalar_mult(n, Ap, alpha, Ap);
	    vectors_subtraction(n, r, Ap, r);	/* fast computation of r, the residual */

	    /* Alternaive accurate, but slow, computation of the residual - r */
	    /* right_mult_with_vector(A, n, x, Ax); */
	    /* vectors_subtraction(n,b,Ax,r); */

	    r_r_new = vectors_inner_product(n, r, r);
	    if (r_r == 0)
		exit(1);
	    beta = r_r_new / r_r;
	    r_r = r_r_new;
	    vectors_scalar_mult(n, p, beta, p);
	    vectors_addition(n, r, p, p);
	}
    }

    free(r);
    free(p);
    free(Ap);
    free(Ax);
    free(alphap);
    free(orth_b);

}


/****************************
** C.G. method - DENSE      *
****************************/

void conjugate_gradient_f
    (float **A, double *x, double *b, int n, double tol,
     int max_iterations, boolean ortho1) {
    /* Solves Ax=b using Conjugate-Gradients method */
    /* 'x' and 'b' are orthogonalized against 1 if 'ortho1=true' */

    int i;

    double alpha, beta, r_r, r_r_new, p_Ap;
    double *r = N_GNEW(n, double);
    double *p = N_GNEW(n, double);
    double *Ap = N_GNEW(n, double);
    double *Ax = N_GNEW(n, double);
    double *alphap = N_GNEW(n, double);

    double *orth_b = N_GNEW(n, double);
    copy_vector(n, b, orth_b);
    if (ortho1) {
	orthog1(n, orth_b);
	orthog1(n, x);
    }
    right_mult_with_vector_f(A, n, x, Ax);
    vectors_subtraction(n, orth_b, Ax, r);
    copy_vector(n, r, p);
    r_r = vectors_inner_product(n, r, r);

    for (i = 0; i < max_iterations && max_abs(n, r) > tol; i++) {
	right_mult_with_vector_f(A, n, p, Ap);
	p_Ap = vectors_inner_product(n, p, Ap);
	if (p_Ap == 0)
	    break;		/*exit(1); */
	alpha = r_r / p_Ap;

	/* derive new x: */
	vectors_scalar_mult(n, p, alpha, alphap);
	vectors_addition(n, x, alphap, x);

	/* compute values for next iteration: */
	if (i < max_iterations - 1) {	/* not last iteration */
	    vectors_scalar_mult(n, Ap, alpha, Ap);
	    vectors_subtraction(n, r, Ap, r);	/* fast computation of r, the residual */

	    /* Alternaive accurate, but slow, computation of the residual - r */
	    /* right_mult_with_vector(A, n, x, Ax); */
	    /* vectors_subtraction(n,b,Ax,r); */

	    r_r_new = vectors_inner_product(n, r, r);
	    if (r_r == 0)
		exit(1);
	    beta = r_r_new / r_r;
	    r_r = r_r_new;
	    vectors_scalar_mult(n, p, beta, p);
	    vectors_addition(n, r, p, p);
	}
    }

    free(r);
    free(p);
    free(Ap);
    free(Ax);
    free(alphap);
    free(orth_b);

}

void
conjugate_gradient_mkernel(float *A, float *x, float *b, int n,
			   double tol, int max_iterations)
{
    /* Solves Ax=b using Conjugate-Gradients method */
    /* A is a packed symmetric matrix */
    /* matrux A is "packed" (only upper triangular portion exists, row-major); */

    int i;

    double alpha, beta, r_r, r_r_new, p_Ap;
    float *r = N_NEW(n, float);
    float *p = N_NEW(n, float);
    float *Ap = N_NEW(n, float);
    float *Ax = N_NEW(n, float);

    /* centering x and b  */
    orthog1f(n, x);
    orthog1f(n, b);

    right_mult_with_vector_ff(A, n, x, Ax);
    /* centering Ax */
    orthog1f(n, Ax);


    vectors_substractionf(n, b, Ax, r);
    copy_vectorf(n, r, p);

    r_r = vectors_inner_productf(n, r, r);

    for (i = 0; i < max_iterations && max_absf(n, r) > tol; i++) {
	orthog1f(n, p);
	orthog1f(n, x);
	orthog1f(n, r);

	right_mult_with_vector_ff(A, n, p, Ap);
	/* centering Ap */
	orthog1f(n, Ap);

	p_Ap = vectors_inner_productf(n, p, Ap);
	if (p_Ap == 0)
	    break;
	alpha = r_r / p_Ap;

	/* derive new x: */
	vectors_mult_additionf(n, x, (float) alpha, p);

	/* compute values for next iteration: */
	if (i < max_iterations - 1) {	/* not last iteration */
	    vectors_mult_additionf(n, r, (float) -alpha, Ap);


	    r_r_new = vectors_inner_productf(n, r, r);

	    if (r_r == 0)
		exit(1);
	    beta = r_r_new / r_r;
	    r_r = r_r_new;

	    vectors_scalar_multf(n, p, (float) beta, p);

	    vectors_additionf(n, r, p, p);
	}
    }

    free(r);
    free(p);
    free(Ap);
    free(Ax);
}
