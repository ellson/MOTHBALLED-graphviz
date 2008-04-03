#ifndef SPRING_ELECTRICAL_H
#define SPRING_ELECTRICAL_H

enum {ERROR_NOT_SQUARE_MATRIX = -100};

/* a flag to indicate that p should be set auto */
#define AUTOP -1.0001234

#define MINDIST 1.e-15

enum {SMOOTHING_NONE, SMOOTHING_STRESS_MAJORIZATION_GRAPH_DIST, SMOOTHING_STRESS_MAJORIZATION_AVG_DIST, SMOOTHING_STRESS_MAJORIZATION_POWER_DIST, SMOOTHING_SPRING, SMOOTHING_TRIANGLE, SMOOTHING_RNG};
struct spring_electrical_control_struct {
  real p;/*a negativve number default to -1. repulsive force = dist^p */
  int random_start;/* whether to apply SE from a random layout, or from exisiting layout */
  real K;/* the natural distance. If K < 0, K will be set to the average distance of an edge */
  real C;/* another parameter. f_a(i,j) = C*dist(i,j)^2/K * d_ij, f_r(i,j) = K^(3-p)/dist(i,j)^(-p). By default C = 0.2. */
  int multilevels;/* if <=1, single level */
  int quadtree_size;/* cut off size above which quadtree approximation is used */
  int max_qtree_level;/* max level of quadtree */
  real bh;/* Barnes-Hutt constant, if width(snode)/dist[i,snode] < bh, treat snode as a supernode. default 0.2*/
  real tol;/* minimum different between two subsequence config before terminating. ||x-xold|| < tol */
  int maxiter;
  real cool;/* default 0.9 */
  real step;/* initial step size */
  int adaptive_cooling;
  int random_seed;
  int beautify_leaves;
  int use_node_weights;
  int smoothing;
  int overlap;
};

typedef struct  spring_electrical_control_struct  *spring_electrical_control; 

spring_electrical_control spring_electrical_control_new();

void spring_electrical_embedding(int dim, SparseMatrix A0, spring_electrical_control ctrl, real *node_weights, real *x, int *flag);

void multilevel_spring_electrical_embedding(int dim, SparseMatrix A0, spring_electrical_control ctrl0, real *node_weights, real *label_sizes, real *x, int *flag);

void export_embedding(FILE *fp, int dim, SparseMatrix A, real *x, real *width);
void spring_electrical_control_delete(spring_electrical_control ctrl);
void print_matrix(real *x, int n, int dim);

real distance(real *x, int dim, int i, int j);
real distance_cropped(real *x, int dim, int i, int j);
real average_edge_length(SparseMatrix A, int dim, real *coord);

void spring_electrical_spring_embedding(int dim, SparseMatrix A, SparseMatrix D, spring_electrical_control ctrl, real *node_weights, real *x, int *flag);

#endif
