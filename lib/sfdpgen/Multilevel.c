#include "general.h"
#include "SparseMatrix.h"
#include "Multilevel.h"


Multilevel_control Multilevel_control_new(){
  Multilevel_control ctrl;

  ctrl = MALLOC(sizeof(struct Multilevel_control_struct));
  ctrl->minsize = 4;
  ctrl->min_coarsen_factor = 0.75;
  ctrl->maxlevel = 1<<30;
  ctrl->randomize = TRUE;
  ctrl->coarsen_scheme = COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_CLUSTER_PERNODE_LEAVES_FIRST;
  ctrl->coarsen_scheme = COARSEN_HYBRID;
  return ctrl;
}

void Multilevel_control_delete(Multilevel_control ctrl){
  FREE(ctrl);
}

static Multilevel Multilevel_init(SparseMatrix A, real *node_weights){
  Multilevel grid;
  if (!A) return NULL;
  assert(A->m == A->n);
  grid = MALLOC(sizeof(struct Multilevel_struct));
  grid->level = 0;
  grid->n = A->n;
  grid->A = A;
  grid->P = NULL;
  grid->R = NULL;
  grid->node_weights = node_weights;
  grid->next = NULL;
  grid->prev = NULL;
  grid->delete_top_level_A = FALSE;
  return grid;
}

void Multilevel_delete(Multilevel grid){
  if (!grid) return;
  if (grid->A){
    if (grid->level == 0) {
      if (grid->delete_top_level_A) SparseMatrix_delete(grid->A);
    } else {
      SparseMatrix_delete(grid->A);
    }
  }
  SparseMatrix_delete(grid->P);
  SparseMatrix_delete(grid->R);
  if (grid->node_weights && grid->level > 0) FREE(grid->node_weights);
  Multilevel_delete(grid->next);
  FREE(grid);
}

static void maximal_independent_vertex_set(SparseMatrix A, int randomize, int **vset, int *nvset, int *nzc){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *vset = MALLOC(sizeof(int)*m);
  for (i = 0; i < m; i++) (*vset)[i] = MAX_IND_VTX_SET_0;
  *nvset = 0;
  *nzc = 0;

  if (!randomize){
    for (i = 0; i < m; i++){
      if ((*vset)[i] == MAX_IND_VTX_SET_0){
	(*vset)[i] = (*nvset)++;
	for (j = ia[i]; j < ia[i+1]; j++){
	  if (i == ja[j]) continue;
	  (*vset)[ja[j]] = MAX_IND_VTX_SET_U;
	  (*nzc)++;
	}
      }
    }
  } else {
    p = random_permutation(m);
    for (ii = 0; ii < m; ii++){
      i = p[ii];
      if ((*vset)[i] == MAX_IND_VTX_SET_0){
	(*vset)[i] = (*nvset)++;
	for (j = ia[i]; j < ia[i+1]; j++){
	  if (i == ja[j]) continue;
	  (*vset)[ja[j]] = MAX_IND_VTX_SET_U;
	  (*nzc)++;
	}
      }
    }
    FREE(p);
  }
  (*nzc) += *nvset;
}


static void maximal_independent_edge_set(SparseMatrix A, int randomize, int **matching, int *nmatch){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *matching = MALLOC(sizeof(int)*m);
  for (i = 0; i < m; i++) (*matching)[i] = i;
  *nmatch = n;

  if (!randomize){
    for (i = 0; i < m; i++){
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
	  (*matching)[ja[j]] = i;
	  (*matching)[i] = ja[j];
	  (*nmatch)--;
	}
      }
    }
  } else {
    p = random_permutation(m);
    for (ii = 0; ii < m; ii++){
      i = p[ii];
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
	  (*matching)[ja[j]] = i;
	  (*matching)[i] = ja[j];
	  (*nmatch)--;
	}
      }
    }
    FREE(p);
  }
}



static void maximal_independent_edge_set_heavest_edge_pernode(SparseMatrix A, int randomize, int **matching, int *nmatch){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  real *a, amax = 0;
  int first = TRUE, jamax = 0;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *matching = MALLOC(sizeof(int)*m);
  for (i = 0; i < m; i++) (*matching)[i] = i;
  *nmatch = n;

  assert(SparseMatrix_is_symmetric(A, FALSE));
  assert(A->type == MATRIX_TYPE_REAL);

  a = (real*) A->a;
  if (!randomize){
    for (i = 0; i < m; i++){
      first = TRUE;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
	  if (first) {
	    amax = a[j];
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j] > amax){
	      amax = a[j];
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  (*matching)[jamax] = i;
	  (*matching)[i] = jamax;
	  (*nmatch)--;
      }
    }
  } else {
    p = random_permutation(m);
    for (ii = 0; ii < m; ii++){
      i = p[ii];
      if ((*matching)[i] != i) continue;
      first = TRUE;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
	  if (first) {
	    amax = a[j];
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j] > amax){
	      amax = a[j];
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  (*matching)[jamax] = i;
	  (*matching)[i] = jamax;
	  (*nmatch)--;
      }
    }
    FREE(p);
  }
}





#define node_degree(i) (ia[(i)+1] - ia[(i)])

static void maximal_independent_edge_set_heavest_edge_pernode_leaves_first(SparseMatrix A, int randomize, int **cluster, int **clusterp, int *ncluster){
  int i, ii, j, *ia, *ja, m, n, *p = NULL, q;
  real *a, amax = 0;
  int first = TRUE, jamax = 0;
  int *matched, nz, ncmax = 0, nz0, nzz,k ;
  enum {UNMATCHED = -2, MATCHED = -1};

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *cluster = MALLOC(sizeof(int)*m);
  *clusterp = MALLOC(sizeof(int)*(m+1));
  matched = MALLOC(sizeof(int)*m);

  for (i = 0; i < m; i++) matched[i] = i;

  assert(SparseMatrix_is_symmetric(A, FALSE));
  assert(A->type == MATRIX_TYPE_REAL);

  *ncluster = 0;
  (*clusterp)[0] = 0;
  nz = 0;
  a = (real*) A->a;
  if (!randomize){
    for (i = 0; i < m; i++){
      if (matched[i] == MATCHED || node_degree(i) != 1) continue;
      q = ja[ia[i]];
      assert(matched[q] != MATCHED);
      matched[q] = MATCHED;
      (*cluster)[nz++] = q;
      for (j = ia[q]; j < ia[q+1]; j++){
	if (q == ja[j]) continue;
	if (node_degree(ja[j]) == 1){
	  matched[ja[j]] = MATCHED;
	  (*cluster)[nz++] = ja[j];
	}
      }
      ncmax = MAX(ncmax, nz - (*clusterp)[*ncluster]);
      nz0 = (*clusterp)[*ncluster];
      if (nz - nz0 <= MAX_CLUSTER_SIZE){
	(*clusterp)[++(*ncluster)] = nz;
      } else {
	(*clusterp)[++(*ncluster)] = ++nz0;	
	nzz = nz0;
	for (k = nz0; k < nz && nzz < nz; k++){
	  nzz += MAX_CLUSTER_SIZE - 1;
	  nzz = MIN(nz, nzz);
	  (*clusterp)[++(*ncluster)] = nzz;
	}
      }

    }
 #ifdef DEBUG_print
   if (Verbose)
     fprintf(stderr, "%d leaves and parents for %d clusters, largest cluster = %d\n",nz, *ncluster, ncmax);
#endif
    for (i = 0; i < m; i++){
      first = TRUE;
      if (matched[i] == MATCHED) continue;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
	  if (first) {
	    amax = a[j];
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j] > amax){
	      amax = a[j];
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  matched[jamax] = MATCHED;
	  matched[i] = MATCHED;
	  (*cluster)[nz++] = i;
	  (*cluster)[nz++] = jamax;
	  (*clusterp)[++(*ncluster)] = nz;
      }
    }

    /* dan yi dian, wu ban */
    for (i = 0; i < m; i++){
      if (matched[i] == i){
	(*cluster)[nz++] = i;
	(*clusterp)[++(*ncluster)] = nz;
      }
    }
    assert(nz == n);
    
  } else {
    p = random_permutation(m);
    for (ii = 0; ii < m; ii++){
      i = p[ii];
      if (matched[i] == MATCHED || node_degree(i) != 1) continue;
      q = ja[ia[i]];
      assert(matched[q] != MATCHED);
      matched[q] = MATCHED;
      (*cluster)[nz++] = q;
      for (j = ia[q]; j < ia[q+1]; j++){
	if (q == ja[j]) continue;
	if (node_degree(ja[j]) == 1){
	  matched[ja[j]] = MATCHED;
	  (*cluster)[nz++] = ja[j];
	}
      }
      ncmax = MAX(ncmax, nz - (*clusterp)[*ncluster]);
      nz0 = (*clusterp)[*ncluster];
      if (nz - nz0 <= MAX_CLUSTER_SIZE){
	(*clusterp)[++(*ncluster)] = nz;
      } else {
	(*clusterp)[++(*ncluster)] = ++nz0;	
	nzz = nz0;
	for (k = nz0; k < nz && nzz < nz; k++){
	  nzz += MAX_CLUSTER_SIZE - 1;
	  nzz = MIN(nz, nzz);
	  (*clusterp)[++(*ncluster)] = nzz;
	}
      }
    }

 #ifdef DEBUG_print
    if (Verbose)
      fprintf(stderr, "%d leaves and parents for %d clusters, largest cluster = %d\n",nz, *ncluster, ncmax);
#endif
    for (ii = 0; ii < m; ii++){
      i = p[ii];
      first = TRUE;
      if (matched[i] == MATCHED) continue;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
	  if (first) {
	    amax = a[j];
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j] > amax){
	      amax = a[j];
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  matched[jamax] = MATCHED;
	  matched[i] = MATCHED;
	  (*cluster)[nz++] = i;
	  (*cluster)[nz++] = jamax;
	  (*clusterp)[++(*ncluster)] = nz;
      }
    }

    /* dan yi dian, wu ban */
    for (i = 0; i < m; i++){
      if (matched[i] == i){
	(*cluster)[nz++] = i;
	(*clusterp)[++(*ncluster)] = nz;
      }
    }

    FREE(p);
  }

  FREE(matched);
}



static void maximal_independent_edge_set_heavest_edge_pernode_supernodes_first(SparseMatrix A, int randomize, int **cluster, int **clusterp, int *ncluster){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  real *a, amax = 0;
  int first = TRUE, jamax = 0;
  int *matched, nz, nz0;
  enum {UNMATCHED = -2, MATCHED = -1};
  int  nsuper, *super = NULL, *superp = NULL;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *cluster = MALLOC(sizeof(int)*m);
  *clusterp = MALLOC(sizeof(int)*(m+1));
  matched = MALLOC(sizeof(int)*m);

  for (i = 0; i < m; i++) matched[i] = i;

  assert(SparseMatrix_is_symmetric(A, FALSE));
  assert(A->type == MATRIX_TYPE_REAL);

  SparseMatrix_decompose_to_supervariables(A, &nsuper, &super, &superp);

  *ncluster = 0;
  (*clusterp)[0] = 0;
  nz = 0;
  a = (real*) A->a;

  for (i = 0; i < nsuper; i++){
    if (superp[i+1] - superp[i] <= 1) continue;
    nz0 = (*clusterp)[*ncluster];
    for (j = superp[i]; j < superp[i+1]; j++){
      matched[super[j]] = MATCHED;
      (*cluster)[nz++] = super[j];
      if (nz - nz0 >= MAX_CLUSTER_SIZE){
	(*clusterp)[++(*ncluster)] = nz;
	nz0 = nz;
      }
    }
    if (nz > nz0) (*clusterp)[++(*ncluster)] = nz;
  }

  if (!randomize){
    for (i = 0; i < m; i++){
      first = TRUE;
      if (matched[i] == MATCHED) continue;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
	  if (first) {
	    amax = a[j];
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j] > amax){
	      amax = a[j];
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  matched[jamax] = MATCHED;
	  matched[i] = MATCHED;
	  (*cluster)[nz++] = i;
	  (*cluster)[nz++] = jamax;
	  (*clusterp)[++(*ncluster)] = nz;
      }
    }

    /* dan yi dian, wu ban */
    for (i = 0; i < m; i++){
      if (matched[i] == i){
	(*cluster)[nz++] = i;
	(*clusterp)[++(*ncluster)] = nz;
      }
    }
    assert(nz == n);
    
  } else {
    p = random_permutation(m);
    for (ii = 0; ii < m; ii++){
      i = p[ii];
      first = TRUE;
      if (matched[i] == MATCHED) continue;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
	  if (first) {
	    amax = a[j];
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j] > amax){
	      amax = a[j];
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  matched[jamax] = MATCHED;
	  matched[i] = MATCHED;
	  (*cluster)[nz++] = i;
	  (*cluster)[nz++] = jamax;
	  (*clusterp)[++(*ncluster)] = nz;
      }
    }

    /* dan yi dian, wu ban */
    for (i = 0; i < m; i++){
      if (matched[i] == i){
	(*cluster)[nz++] = i;
	(*clusterp)[++(*ncluster)] = nz;
      }
    }
    FREE(p);

  }

  FREE(super);

  FREE(superp);

  FREE(matched);
}

static int scomp(const void *s1, const void *s2){
  real *ss1, *ss2;
  ss1 = (real*) s1;
  ss2 = (real*) s2;

  if ((ss1)[1] > (ss2)[1]){
    return -1;
  } else if ((ss1)[1] < (ss2)[1]){
    return 1;
  }
  return 0;
}

static void maximal_independent_edge_set_heavest_cluster_pernode_leaves_first(SparseMatrix A, int csize, 
									      int randomize, int **cluster, int **clusterp, int *ncluster){
  int i, ii, j, *ia, *ja, m, n, *p = NULL, q, iv, ncmax;
  real *a;
  int *matched, nz,  nz0, nzz,k, nv;
  enum {UNMATCHED = -2, MATCHED = -1};
  real *vlist;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *cluster = MALLOC(sizeof(int)*m);
  *clusterp = MALLOC(sizeof(int)*(m+1));
  matched = MALLOC(sizeof(int)*m);
  vlist = MALLOC(sizeof(real)*2*m);

  for (i = 0; i < m; i++) matched[i] = i;

  assert(SparseMatrix_is_symmetric(A, FALSE));
  assert(A->type == MATRIX_TYPE_REAL);

  *ncluster = 0;
  (*clusterp)[0] = 0;
  nz = 0;
  a = (real*) A->a;

  p = random_permutation(m);
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if (matched[i] == MATCHED || node_degree(i) != 1) continue;
    q = ja[ia[i]];
    assert(matched[q] != MATCHED);
    matched[q] = MATCHED;
    (*cluster)[nz++] = q;
    for (j = ia[q]; j < ia[q+1]; j++){
      if (q == ja[j]) continue;
      if (node_degree(ja[j]) == 1){
	matched[ja[j]] = MATCHED;
	(*cluster)[nz++] = ja[j];
      }
    }
    ncmax = MAX(ncmax, nz - (*clusterp)[*ncluster]);
    nz0 = (*clusterp)[*ncluster];
    if (nz - nz0 <= MAX_CLUSTER_SIZE){
      (*clusterp)[++(*ncluster)] = nz;
    } else {
      (*clusterp)[++(*ncluster)] = ++nz0;	
      nzz = nz0;
      for (k = nz0; k < nz && nzz < nz; k++){
	nzz += MAX_CLUSTER_SIZE - 1;
	nzz = MIN(nz, nzz);
	(*clusterp)[++(*ncluster)] = nzz;
      }
    }
  }
  
  for (ii = 0; ii < m; ii++){
    i = p[ii];
    if (matched[i] == MATCHED) continue;
    nv = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      if (matched[ja[j]] != MATCHED && matched[i] != MATCHED){
	vlist[2*nv] = ja[j];
	vlist[2*nv+1] = a[j];
	nv++;
      }
    }
    if (nv > 0){
      qsort(vlist, nv, sizeof(real)*2, scomp);
      for (j = 0; j < MIN(csize - 1, nv); j++){
	iv = (int) vlist[2*j];
	matched[iv] = MATCHED;
	(*cluster)[nz++] = iv;
      }
      matched[i] = MATCHED;
      (*cluster)[nz++] = i;
      (*clusterp)[++(*ncluster)] = nz;
    }
  }
  
  /* dan yi dian, wu ban */
  for (i = 0; i < m; i++){
    if (matched[i] == i){
      (*cluster)[nz++] = i;
      (*clusterp)[++(*ncluster)] = nz;
    }
  }
  FREE(p);


  FREE(matched);
}
static void maximal_independent_edge_set_heavest_edge_pernode_scaled(SparseMatrix A, int randomize, int **matching, int *nmatch){
  int i, ii, j, *ia, *ja, m, n, *p = NULL;
  real *a, amax = 0;
  int first = TRUE, jamax = 0;

  assert(A);
  assert(SparseMatrix_known_strucural_symmetric(A));
  ia = A->ia;
  ja = A->ja;
  m = A->m;
  n = A->n;
  assert(n == m);
  *matching = MALLOC(sizeof(int)*m);
  for (i = 0; i < m; i++) (*matching)[i] = i;
  *nmatch = n;

  assert(SparseMatrix_is_symmetric(A, FALSE));
  assert(A->type == MATRIX_TYPE_REAL);

  a = (real*) A->a;
  if (!randomize){
    for (i = 0; i < m; i++){
      first = TRUE;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
	  if (first) {
	    amax = a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]);
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]) > amax){
	      amax = a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]);
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  (*matching)[jamax] = i;
	  (*matching)[i] = jamax;
	  (*nmatch)--;
      }
    }
  } else {
    p = random_permutation(m);
    for (ii = 0; ii < m; ii++){
      i = p[ii];
      if ((*matching)[i] != i) continue;
      first = TRUE;
      for (j = ia[i]; j < ia[i+1]; j++){
	if (i == ja[j]) continue;
	if ((*matching)[ja[j]] == ja[j] && (*matching)[i] == i){
	  if (first) {
	    amax = a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]);
	    jamax = ja[j];
	    first = FALSE;
	  } else {
	    if (a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]) > amax){
	      amax = a[j]/(ia[i+1]-ia[i])/(ia[ja[j]+1]-ia[ja[j]]);
	      jamax = ja[j];
	    }
	  }
	}
      }
      if (!first){
	  (*matching)[jamax] = i;
	  (*matching)[i] = jamax;
	  (*nmatch)--;
      }
    }
    FREE(p);
  }
}

static void Multilevel_coarsen(SparseMatrix A, SparseMatrix *cA, real *node_wgt, real **cnode_wgt,
			       SparseMatrix *P, SparseMatrix *R, Multilevel_control ctrl, int *coarsen_scheme_used){
  int *matching = NULL, nmatch, nc, nzc, n, i;
  int *irn = NULL, *jcn = NULL, *ia = NULL, *ja = NULL;
  real *val = NULL;
  SparseMatrix B = NULL;
  int *vset = NULL, nvset, ncov, j;
  int *cluster, *clusterp, ncluster;

  assert(A->m == A->n);
  *cA = NULL;
  *P = NULL;
  *R = NULL;
  n = A->m;

  *coarsen_scheme_used = ctrl->coarsen_scheme;

  switch (ctrl->coarsen_scheme){
  case COARSEN_HYBRID:
#ifdef DEBUG_PRINT
    if (Verbose)
      fprintf(stderr, "hybrid scheme, try COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_LEAVES_FIRST first\n");
#endif
    *coarsen_scheme_used = ctrl->coarsen_scheme =  COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_LEAVES_FIRST;
    Multilevel_coarsen(A, cA, node_wgt, cnode_wgt, P, R, ctrl, coarsen_scheme_used);

    if (!(*cA)) {
#ifdef DEBUG_PRINT
      if (Verbose)
        fprintf(stderr, "switching to COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_SUPERNODES_FIRST\n");
#endif
      *coarsen_scheme_used = ctrl->coarsen_scheme =  COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_SUPERNODES_FIRST;
      Multilevel_coarsen(A, cA, node_wgt, cnode_wgt, P, R, ctrl, coarsen_scheme_used);
    }

    if (!(*cA)) {
#ifdef DEBUG_PRINT
      if (Verbose)
        fprintf(stderr, "switching to COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_CLUSTER_PERNODE_LEAVES_FIRST\n");
#endif
      *coarsen_scheme_used = ctrl->coarsen_scheme =  COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_CLUSTER_PERNODE_LEAVES_FIRST;
      Multilevel_coarsen(A, cA, node_wgt, cnode_wgt, P, R, ctrl, coarsen_scheme_used);
    }

    if (!(*cA)) {
#ifdef DEBUG_PRINT
     if (Verbose)
        fprintf(stderr, "switching to COARSEN_INDEPENDENT_VERTEX_SET\n");
#endif
      *coarsen_scheme_used = ctrl->coarsen_scheme = COARSEN_INDEPENDENT_VERTEX_SET;
      Multilevel_coarsen(A, cA, node_wgt, cnode_wgt, P, R, ctrl, coarsen_scheme_used);
    }


    if (!(*cA)) {
#ifdef DEBUG_PRINT
      if (Verbose)
        fprintf(stderr, "switching to COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE\n");
#endif
      *coarsen_scheme_used = ctrl->coarsen_scheme = COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE;
      Multilevel_coarsen(A, cA, node_wgt, cnode_wgt, P, R, ctrl, coarsen_scheme_used);
    }
    ctrl->coarsen_scheme = COARSEN_HYBRID;
    break;
  case  COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_SUPERNODES_FIRST:
  case  COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_CLUSTER_PERNODE_LEAVES_FIRST:
  case COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_LEAVES_FIRST:
    if (ctrl->coarsen_scheme == COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_LEAVES_FIRST) {
      maximal_independent_edge_set_heavest_edge_pernode_leaves_first(A, ctrl->randomize, &cluster, &clusterp, &ncluster);
    } else if (ctrl->coarsen_scheme == COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_SUPERNODES_FIRST) {
      maximal_independent_edge_set_heavest_edge_pernode_supernodes_first(A, ctrl->randomize, &cluster, &clusterp, &ncluster);
    } else {
      maximal_independent_edge_set_heavest_cluster_pernode_leaves_first(A, 4, ctrl->randomize, &cluster, &clusterp, &ncluster);
    }
    assert(ncluster <= n);
    nc = ncluster;
    if (nc > ctrl->min_coarsen_factor*n || nc < ctrl->minsize) {
#ifdef DEBUG_PRINT
      if (Verbose)
        fprintf(stderr, "nc = %d, nf = %d, minsz = %d, coarsen_factor = %f coarsening stops\n",nc, n, ctrl->minsize, ctrl->min_coarsen_factor);
#endif
      goto RETURN;
    }
    irn = MALLOC(sizeof(int)*n);
    jcn = MALLOC(sizeof(int)*n);
    val = MALLOC(sizeof(real)*n);
    nzc = 0; 
    for (i = 0; i < ncluster; i++){
      for (j = clusterp[i]; j < clusterp[i+1]; j++){
	assert(clusterp[i+1] > clusterp[i]);
	irn[nzc] = cluster[j];
	jcn[nzc] = i;
	val[nzc++] = 1.;
     }
    }
    assert(nzc == n);
    *P = SparseMatrix_from_coordinate_arrays(nzc, n, nc, irn, jcn, (void *) val, MATRIX_TYPE_REAL);
    *R = SparseMatrix_transpose(*P);
    B = SparseMatrix_multiply(*R, A);
    *cA = SparseMatrix_multiply(B, *P); 
    SparseMatrix_multiply_vector(*R, node_wgt, cnode_wgt, FALSE);
    *R = SparseMatrix_divide_row_by_degree(*R);
    SparseMatrix_set_symmetric(*cA);
    SparseMatrix_set_pattern_symmetric(*cA);
    *cA = SparseMatrix_remove_diagonal(*cA);
    break;
  case COARSEN_INDEPENDENT_EDGE_SET:
    maximal_independent_edge_set(A, ctrl->randomize, &matching, &nmatch);
  case COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE:
    if (ctrl->coarsen_scheme == COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE) 
      maximal_independent_edge_set_heavest_edge_pernode(A, ctrl->randomize, &matching, &nmatch);
  case COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_DEGREE_SCALED:
    if (ctrl->coarsen_scheme == COARSEN_INDEPENDENT_EDGE_SET_HEAVEST_EDGE_PERNODE_DEGREE_SCALED) 
      maximal_independent_edge_set_heavest_edge_pernode_scaled(A, ctrl->randomize, &matching, &nmatch);
    nc = nmatch;
    if (nc > ctrl->min_coarsen_factor*n || nc < ctrl->minsize) {
#ifdef DEBUG_PRINT
      if (Verbose)
        fprintf(stderr, "nc = %d, nf = %d, minsz = %d, coarsen_factor = %f coarsening stops\n",nc, n, ctrl->minsize, ctrl->min_coarsen_factor);
#endif
      goto RETURN;
    }
    irn = MALLOC(sizeof(int)*n);
    jcn = MALLOC(sizeof(int)*n);
    val = MALLOC(sizeof(real)*n);
    nzc = 0; nc = 0;
    for (i = 0; i < n; i++){
      if (matching[i] >= 0){
	if (matching[i] == i){
	  irn[nzc] = i;
	  jcn[nzc] = nc;
	  val[nzc++] = 1.;
	} else {
	  irn[nzc] = i;
	  jcn[nzc] = nc;
	  val[nzc++] = 1;
	  irn[nzc] = matching[i];
	  jcn[nzc] = nc;
	  val[nzc++] = 1;
	  matching[matching[i]] = -1;
	}
	nc++;
	matching[i] = -1;
      }
    }
    assert(nc == nmatch);
    assert(nzc == n);
    *P = SparseMatrix_from_coordinate_arrays(nzc, n, nc, irn, jcn, (void *) val, MATRIX_TYPE_REAL);
    *R = SparseMatrix_transpose(*P);
    B = SparseMatrix_multiply(*R, A);
    *cA = SparseMatrix_multiply(B, *P); 
    SparseMatrix_multiply_vector(*R, node_wgt, cnode_wgt, FALSE);
    *R = SparseMatrix_divide_row_by_degree(*R);
    SparseMatrix_set_symmetric(*cA);
    SparseMatrix_set_pattern_symmetric(*cA);
    *cA = SparseMatrix_remove_diagonal(*cA);
    break;
  case COARSEN_INDEPENDENT_VERTEX_SET:
    maximal_independent_vertex_set(A, ctrl->randomize, &vset, &nvset, &nzc);
    ia = A->ia;
    ja = A->ja;
    nc = nvset;
    if (nc > ctrl->min_coarsen_factor*n || nc < ctrl->minsize) {
#ifdef DEBUG_PRINT
      if (Verbose)
        fprintf(stderr, "nc = %d, nf = %d, minsz = %d, coarsen_factor = %f coarsening stops\n",nc, n, ctrl->minsize, ctrl->min_coarsen_factor);
#endif
      goto RETURN;
    }
    irn = MALLOC(sizeof(int)*nzc);
    jcn = MALLOC(sizeof(int)*nzc);
    val = MALLOC(sizeof(real)*nzc);
    nzc = 0; 
    for (i = 0; i < n; i++){
      if (vset[i] == MAX_IND_VTX_SET_U){
	ncov = 0;
	for (j = ia[i]; j < ia[i+1]; j++){
	  if (vset[ja[j]] >= MAX_IND_VTX_SET_C){
	    ncov++;
	  }
	}
	assert(ncov > 0);
	for (j = ia[i]; j < ia[i+1]; j++){
	  if (vset[ja[j]] >= MAX_IND_VTX_SET_C){
	    irn[nzc] = i;
	    jcn[nzc] = vset[ja[j]];
	    val[nzc++] = 1./(double) ncov;
	  }
	}
      } else {
	assert(vset[i] >= MAX_IND_VTX_SET_C);
	irn[nzc] = i;
	jcn[nzc] = vset[i];
	val[nzc++] = 1.;
      }
    }

    *P = SparseMatrix_from_coordinate_arrays(nzc, n, nc, irn, jcn, (void *) val, MATRIX_TYPE_REAL);
    *R = SparseMatrix_transpose(*P);
    B = SparseMatrix_multiply(*R, A);
    *cA = SparseMatrix_multiply(B, *P); 
    SparseMatrix_multiply_vector(*R, node_wgt, cnode_wgt, FALSE);
    SparseMatrix_set_symmetric(*cA);
    SparseMatrix_set_pattern_symmetric(*cA);
    *cA = SparseMatrix_remove_diagonal(*cA);
    break;
  default:
    goto RETURN;
  }
 RETURN:
  if (matching) FREE(matching);
  if (vset) FREE(vset);
  if (irn) FREE(irn);
  if (jcn) FREE(jcn);
  if (val) FREE(val);
  if (B) SparseMatrix_delete(B);
}

void print_padding(int n){
  int i;
  for (i = 0; i < n; i++) fputs (" ", stderr);
}
static Multilevel Multilevel_establish(Multilevel grid, Multilevel_control ctrl){
  Multilevel cgrid;
  int coarsen_scheme_used;
  real *cnode_weights = NULL;
  SparseMatrix P, R, A, cA;

#ifdef DEBUG_PRINT
  if (Verbose) {
    print_padding(grid->level);
    fprintf(stderr, "level -- %d, n = %d, nz = %d nz/n = %f\n", grid->level, grid->n, grid->A->nz, grid->A->nz/(double) grid->n);
  }
#endif
  A = grid->A;
  if (grid->level >= ctrl->maxlevel - 1) {
#ifdef DEBUG_PRINT
  if (Verbose) {
    print_padding(grid->level);
    fprintf(stderr, " maxlevel reached, coarsening stops\n");
  }
#endif
    return grid;
  }
  Multilevel_coarsen(A, &cA, grid->node_weights, &cnode_weights, &P, &R, ctrl, &coarsen_scheme_used);
  if (!cA) return grid;

  cgrid = Multilevel_init(cA, cnode_weights);
  grid->next = cgrid;
  cgrid->coarsen_scheme_used = coarsen_scheme_used;
  cgrid->level = grid->level + 1;
  cgrid->n = cA->m;
  cgrid->A = cA;
  cgrid->P = P;
  grid->R = R;
  cgrid->prev = grid;
  cgrid = Multilevel_establish(cgrid, ctrl);
  return grid;
  
}

Multilevel Multilevel_new(SparseMatrix A0, real *node_weights, Multilevel_control ctrl){
  Multilevel grid;
  SparseMatrix A = A0;

  if (!SparseMatrix_is_symmetric(A, FALSE) || A->type != MATRIX_TYPE_REAL){
    A = SparseMatrix_get_real_adjacency_matrix_symmetrized(A);
  }
  grid = Multilevel_init(A, node_weights);
  grid = Multilevel_establish(grid, ctrl);
  if (A != A0) grid->delete_top_level_A = TRUE;/* be sure to clean up later */
  return grid;
}


Multilevel Multilevel_get_coarsest(Multilevel grid){
  while (grid->next){
    grid = grid->next;
  }
  return grid;
}

