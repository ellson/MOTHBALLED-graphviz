
typedef  StressMajorizationSmoother OverlapSmoother;

#define OverlapSmoother_struct StressMajorizationSmoother_struct

void OverlapSmoother_delete(OverlapSmoother sm);

OverlapSmoother OverlapSmoother_new(SparseMatrix A, int dim, real lambda0, real *x, real *width, int include_original_graph, int neighborhood_only,
				    real *max_overlap, real *min_overlap);

void OverlapSmoother_smooth(OverlapSmoother sm, int dim, real *x);


void remove_overlap(int dim, SparseMatrix A, real *x, real *label_sizes, int ntry, int *flag);
