#include "general.h"
#include "SparseMatrix.h"
#include "spring_electrical.h"
#include "post_process.h"
#include "stress_model.h"

void stress_model(int dim, SparseMatrix B, real **x, int maxit_sm, real tol, int *flag){
  int m;
  SparseStressMajorizationSmoother sm;
  real lambda = 0;
  /*int maxit_sm = 1000, i; tol = 0.001*/
  int i;
  SparseMatrix A = B;

  if (!SparseMatrix_is_symmetric(A, FALSE) || A->type != MATRIX_TYPE_REAL){
    if (A->type == MATRIX_TYPE_REAL){
      A = SparseMatrix_symmetrize(A, FALSE);
      A = SparseMatrix_remove_diagonal(A);
    } else {
      A = SparseMatrix_get_real_adjacency_matrix_symmetrized(A);
    } 
  }
  A = SparseMatrix_remove_diagonal(A);

  *flag = 0;
  m = A->m;
  if (!x) {
    *x = MALLOC(sizeof(real)*m*dim);
    srand(123);
    for (i = 0; i < dim*m; i++) (*x)[i] = drand();
  }

  sm = SparseStressMajorizationSmoother_new(A, dim, lambda, *x, WEIGHTING_SCHEME_NONE);/* do not under weight the long distances */

  if (!sm) {
    *flag = -1;
    goto RETURN;
  }


  SparseStressMajorizationSmoother_smooth(sm, dim, *x, maxit_sm, 0.001);
  for (i = 0; i < dim*m; i++) {
    (*x)[i] /= sm->scaling;
  }
  SparseStressMajorizationSmoother_delete(sm);

 RETURN:
  if (A != B) SparseMatrix_delete(A);

}
