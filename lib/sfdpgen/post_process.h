struct StressMajorizationSmoother_struct {
  SparseMatrix Lw;
  SparseMatrix Lwd;
  real* lambda;
};

typedef struct StressMajorizationSmoother_struct *StressMajorizationSmoother;

void StressMajorizationSmoother_delete(StressMajorizationSmoother sm);

enum {IDEAL_GRAPH_DIST, IDEAL_AVG_DIST, IDEAL_POWER_DIST};
StressMajorizationSmoother StressMajorizationSmoother_new(SparseMatrix A, int dim, real lambda, real *x, int ideal_dist_scheme);

void StressMajorizationSmoother_smooth(StressMajorizationSmoother sm, int dim, real *x, int maxit);
/*-------------------- triangle/neirhborhood graph based smoother ------------------- */
typedef  StressMajorizationSmoother TriangleSmoother;

#define TriangleSmoother_struct StressMajorizationSmoother_struct

void TriangleSmoother_delete(TriangleSmoother sm);

TriangleSmoother TriangleSmoother_new(SparseMatrix A, int dim, real lambda, real *x, int use_triangularization);

void TriangleSmoother_smooth(TriangleSmoother sm, int dim, real *x);



/*------------------ spring and spring-electrical based smoother */

struct SpringSmoother_struct {
  SparseMatrix D;
  spring_electrical_control ctrl;
};

typedef struct SpringSmoother_struct *SpringSmoother;

SpringSmoother SpringSmoother_new(SparseMatrix A, int dim, spring_electrical_control ctrl, real *x);

void SpringSmoother_delete(SpringSmoother sm);

void SpringSmoother_smooth(SpringSmoother sm, SparseMatrix A, real *node_weights, int dim, real *x);
/*------------------------------------------------------------------*/

void post_process_smoothing(int dim, SparseMatrix A, spring_electrical_control ctrl, real *node_weights, real *x, int *flag);
