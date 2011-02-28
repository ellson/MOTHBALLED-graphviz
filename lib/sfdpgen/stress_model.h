#ifndef STRESS_MODEL_H
#define STRESS_MODEL_H

void stress_model(int dim, SparseMatrix A, real **x, int maxit, real tol, int *flag);

#endif
