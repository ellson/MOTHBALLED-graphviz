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

/*
 * This code was (mostly) written by Ken Turkowski, who said:
 *
 * Oh, that. I wrote it in college the first time. It's open source - I think I
 * posted it after seeing so many people solve equations by inverting matrices
 * by computing minors naïvely.
 * -Ken
 *
 * The views represented here are mine and are not necessarily shared by
 * my employer.
   	Ken Turkowski			turk@apple.com
	Immersive Media Technologist 	http://www.worldserver.com/turk/
	Apple Computer, Inc.
	1 Infinite Loop, MS 302-3VR
	Cupertino, CA 95014
 */

/* Matinv() inverts the matrix A using LU decomposition.  Arguments:
 *	A    - the (n x n) matrix to be inverted
 *	Ainv - the (n x n) inverted matrix
 *	n    - the order of the matrices A and Ainv
 */

#if _PACKAGE_ast
#include <ast.h>
#else
#endif
#include <stdlib.h>
#include <render.h>
extern int lu_decompose(double **a, int n);
extern void lu_solve(double *x, double *b, int n);

int matinv(double **A, double **Ainv, int n)
{
    register int i, j;
    double *b, temp;

    /* Decompose matrix into L and U triangular matrices */
    if (lu_decompose(A, n) == 0)
	return (0);		/* Singular */

    /* Invert matrix by solving n simultaneous equations n times */
    b = N_NEW(n, double);
    for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++)
	    b[j] = 0.0;
	b[i] = 1.0;
	lu_solve(Ainv[i], b, n);	/* Into a row of Ainv: fix later */
    }
    free(b);

    /* Transpose matrix */
    for (i = 0; i < n; i++) {
	for (j = 0; j < i; j++) {
	    temp = Ainv[i][j];
	    Ainv[i][j] = Ainv[j][i];
	    Ainv[j][i] = temp;
	}
    }

    return (1);
}
