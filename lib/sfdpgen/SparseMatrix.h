struct {
  int m; /* row dimension */
  int n; /* column dimension */
  int nz; /* the current length of ja and a (if exists) allocated. The actual length used is ia[m] */
  int type; /* whether it is real/complex matrix, or pattern only */
  int *ia; /* row pointer */
  int *ja; /* column indices. 1-based */
  real *a; /* entry values. If NULL, pattern matrix */
} SparseMatrix_struct;

typedef SparseMatrix_struct* SparseMatrix;

