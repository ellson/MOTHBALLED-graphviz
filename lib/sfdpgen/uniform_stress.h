/*
#ifndef UniformStress_H
#define UniformStress_H
*/

typedef StressMajorizationSmoother UniformStressSmoother;

#define UniformStressSmoother_struct StressMajorizationSmoother_struct

void UniformStressSmoother_delete(UniformStressSmoother sm);

UniformStressSmoother UniformStressSmoother_new(int dim, SparseMatrix A, real *x, real alpha, real M, int *flag);

void uniform_stress(int dim, SparseMatrix A, real *x, int *flag);

/*
#endif
*/
