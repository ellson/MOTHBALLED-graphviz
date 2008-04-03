enum {SOLVE_METHOD_CG};


typedef struct Operator_struct *Operator;

struct Operator_struct {
  void *data;
  real* (*Operator_apply)(Operator o, real *in, real *out);
};

real conjugate_gradient(Operator A, Operator precon, int n, real *x0, real *rhs, real tol, int maxit, int *flag);

real SparseMatrix_solve(SparseMatrix A, int dim, real *x0, real *rhs, real tol, int maxit, int method, int *flag);
