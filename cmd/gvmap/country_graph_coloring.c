/* $Id$Revision: */
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

#define STANDALONE
#include "SparseMatrix.h"
#include "country_graph_coloring.h"
#include "power.h"

static void get_local_12_norm(int n, int i, int *ia, int *ja, int *p, real *norm){
  int j;
  norm[0] = n; norm[1] = 0;
  for (j = ia[i]; j < ia[i+1]; j++){
    if (ja[j] == i) continue;
    norm[0] = MIN(norm[0], ABS(p[i] - p[ja[j]]));
    norm[1] += ABS(p[i] - p[ja[j]]);
  }
}
static void get_12_norm(int n, int *ia, int *ja, int *p, real *norm){
  int i, j;
  norm[0] = n; norm[1] = 0;
  for (i = 0; i < n; i++){
    for (j = ia[i]; j < ia[i+1]; j++){
      if (ja[j] == i) continue;
      norm[0] = MIN(norm[0], ABS(p[i] - p[ja[j]]));
      norm[1] += ABS(p[i] - p[ja[j]]);
    }
  }
}

void country_graph_coloring(int seed, SparseMatrix A, int **p, real *norm_1){
  int n = A->m, i, j, jj;
  SparseMatrix L, A2;
  int *ia = A->ia, *ja = A->ja;
  int a = -1.;
  real nrow;
  real *v = NULL;
  real norm1[2], norm2[2], norm11[2], norm22[2], norm = n;
  real pi, pj;
  int improved = TRUE;


  assert(A->m == A->n);
  A2 = SparseMatrix_symmetrize(A, TRUE);
  ia = A2->ia; ja = A2->ja;

  /* Laplacian */
  L = SparseMatrix_new(n, n, 1, MATRIX_TYPE_REAL, FORMAT_COORD);
  for (i = 0; i < n; i++){
    nrow = 0.;
    for (j = ia[i]; j < ia[i+1]; j++){
      jj = ja[j];
      if (jj != i){
	nrow ++;
	L = SparseMatrix_coordinate_form_add_entries(L, 1, &i, &jj, &a);
      }
    }
    L = SparseMatrix_coordinate_form_add_entries(L, 1, &i, &i, &nrow);
  }
  L = SparseMatrix_from_coordinate_format(L);

  /* largest eigen vector */
  {
    int maxit = 100;
    real tol = 0.00001;
    power_method(L, seed, maxit, tol, &v);
  }

  vector_ordering(n, v, p, TRUE);

  /* swapping */
  while (improved){
    improved = FALSE; norm = n;
    for (i = 0; i < n; i++){
      get_local_12_norm(n, i, ia, ja, *p, norm1);
      for (j = 0; j < n; j++){
	if (j == i) continue;
	get_local_12_norm(n, j, ia, ja, *p, norm2);
	norm = MIN(norm, norm2[0]);
	pi = (*p)[i]; pj = (*p)[j];
	(*p)[i] = pj;
	(*p)[j] = pi;
	get_local_12_norm(n, i, ia, ja, *p, norm11);
	get_local_12_norm(n, j, ia, ja, *p, norm22);
	if (MIN(norm11[0],norm22[0]) > MIN(norm1[0],norm2[0])){
	  //	    ||
	  //(MIN(norm11[0],norm22[0]) == MIN(norm1[0],norm2[0]) && norm11[1]+norm22[1] > norm1[1]+norm2[1])) {
	  improved = TRUE;
	  norm1[0] = norm11[0];
	  norm1[1] = norm11[1];
	  continue;
	}
	(*p)[i] = pi;
	(*p)[j] = pj;
      }
    }
    if (Verbose) {
      get_12_norm(n, ia, ja, *p, norm1);
      fprintf(stderr, "norm = %f", norm);
      fprintf(stderr, "norm1 = %f, norm2 = %f\n", norm1[0], norm1[1]);
    }
  }
  get_12_norm(n, ia, ja, *p, norm1);

  *norm_1 = norm1[0];
  if (A2 != A) SparseMatrix_delete(A2);
  SparseMatrix_delete(L);
}



