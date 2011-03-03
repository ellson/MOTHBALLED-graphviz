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

#include "SparseMatrix.h"
/* #include "matrix_market.h" */
void power_method(SparseMatrix A, int random_seed, int maxit, real tol, real **eigv){
  /* find the largest eigenvector of a matrix A. Result in eigv. if eigv == NULL; memory will be allocated.
     maxium of maxit iterations will be done, and tol is the convergence criterion

     This converges only if the largest eigenvector/value is real and the next largest eigenvalue separate from teh largest one

   */
  int n;
  real *v, *u;
  int iter = 0;
  real res, unorm;
  int i;
  assert(A->m == A->n);
  assert(A->type = MATRIX_TYPE_REAL || A->type == MATRIX_TYPE_INTEGER);

  n = A->m;
  if (!(*eigv)) *eigv = MALLOC(sizeof(real)*n);
  u = MALLOC(sizeof(real)*n);

  srand(random_seed);

  for (i = 0; i < n; i++) (*eigv)[i] = drand();

  res = vector_product(n, *eigv, *eigv);
  if (res > 0) res =  1/res;
  for (i = 0; i < n; i++) (*eigv)[i] = (*eigv)[i]*res;
  
  v = *eigv;
  
  do {
    SparseMatrix_multiply_vector(A, v, &u, FALSE);

    unorm = vector_product(n, u, u);/* ||u||^2 */
    unorm = sqrt(unorm);
    if (unorm > 0) unorm = 1/unorm;
    res = 0.;
    for (i = 0; i < n; i++) {
      u[i] = u[i]*unorm;
      res = res + (u[i] - v[i])*(u[i] - v[i]);
      v[i] = u[i];
    }
   
    /* 
    printf("=== %d === %f\n",iter,res);
    printf("{");
    {int j;
      for (j = 0; j < MIN(10,n); j++){
      if (j == n-1){
	printf("%f",v[j]);
      } else {
	printf("%f,",v[j]);
      }
    }
    printf("\n");
    }
    */

  } while (res/n > tol*tol && iter++ < maxit);
  
  
}

/*
main(){
  real *v = NULL;
  int i;
  int n;

  SparseMatrix A;

  A = SparseMatrix_import_matrix_market(stdin, FORMAT_CSR);
  n = A->m;
  
  power_method(A, 123, 100, 0.00001, &v);

  printf("{");
  for (i = 0; i < n; i++){
    if (i == n-1){
      printf("%f",v[i]);
    } else {
      printf("%f,",v[i]);
    }
  }
  printf("}\n");
}
*/
