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

#define STANDALONE
#include "general.h"
#include "SparseMatrix.h"
#include "clustering.h"

Multilevel_Modularity_Clustering Multilevel_Modularity_Clustering_init(SparseMatrix A, int level){
  Multilevel_Modularity_Clustering grid;
  int n = A->n, i, j;

  assert(A->type == MATRIX_TYPE_REAL);
  assert(SparseMatrix_is_symmetric(A, FALSE));

  if (!A) return NULL;
  assert(A->m == n);
  grid = MALLOC(sizeof(struct Multilevel_Modularity_Clustering_struct));
  grid->level = level;
  grid->n = n;
  grid->A = A;
  grid->P = NULL;
  grid->R = NULL;
  grid->next = NULL;
  grid->prev = NULL;
  grid->delete_top_level_A = FALSE;
  grid->matching = MALLOC(sizeof(real)*(n));
  grid->deg = NULL;

  if (level == 0){
    real modularity = 0;
    int *ia = A->ia, *ja = A->ja, n = A->n;
    real deg_total = 0;
    real *deg, *a = (real*) (A->a);
    real *indeg;

    grid->deg_total = 0.;
    grid->deg = MALLOC(sizeof(real)*(n));
    deg = grid->deg;

    indeg = MALLOC(sizeof(real)*n);
    for (i = 0; i < n; i++){
      deg[i] = 0;
      indeg[i] = 0.;
      for (j = ia[i]; j < ia[i+1]; j++){
	deg[i] += a[j];
	if (ja[j] == i) indeg[i] = a[j];
      }
      deg_total += deg[i];
    }
    if (deg_total == 0) deg_total = 1;
    for (i = 0; i < n; i++){
      modularity += (indeg[i] - deg[i]*deg[i]/deg_total)/deg_total;
    }
    grid->deg_total = deg_total;
    grid->deg = deg;
    grid->modularity = modularity;
    FREE(indeg);
  }


  return grid;
} 

void Multilevel_Modularity_Clustering_delete(Multilevel_Modularity_Clustering grid){
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
  FREE(grid->matching);
  FREE(grid->deg);

  Multilevel_Modularity_Clustering_delete(grid->next);
  FREE(grid);
}

Multilevel_Modularity_Clustering Multilevel_Modularity_Clustering_establish(Multilevel_Modularity_Clustering grid, int maxcluster){
  int *matching = grid->matching;
  SparseMatrix A = grid->A;
  int n = grid->n, level = grid->level, nc = 0;
  real modularity = 0;
  int *ia = A->ia, *ja = A->ja;
  real *a;
  real *deg = grid->deg;
  real *deg_new;
  int i, j, jj, jc, jmax;
  real inv_deg_total = 1./(grid->deg_total);
  real *deg_inter, gain;
  int *mask;
  real maxgain;
  real total_gain = 0;
  modularity = grid->modularity;

  deg_new = MALLOC(sizeof(real)*n);
  deg_inter = MALLOC(sizeof(real)*n);
  mask = MALLOC(sizeof(int)*n);
  for (i = 0; i < n; i++) mask[i] = -1;

  assert(n == A->n);
  for (i = 0; i < n; i++) matching[i] = UNMATCHED;

  /* gain in merging node i into cluster j is
     deg(i,j)/deg_total - 2*deg(i)*deg(j)/deg_total^2
  */
  a = (real*) A->a;
  for (i = 0; i < n; i++){
    if (matching[i] != UNMATCHED) continue;
    /* accumulate connections between i and clusters */
    for (j = ia[i]; j < ia[i+1]; j++){
      jj = ja[j];
      if (jj == i) continue;
      if ((jc=matching[jj]) != UNMATCHED){
	if (mask[jc] != i) {
	  mask[jc] = i;
	  deg_inter[jc] = a[j];
	} else {
	  deg_inter[jc] += a[j];
	}
      }
    }

    maxgain = 0;
    jmax = -1;
    for (j = ia[i]; j < ia[i+1]; j++){
      jj = ja[j];
      if (jj == i) continue;
      if ((jc=matching[jj]) == UNMATCHED){
	/* the first 2 is due to the fact that deg_iter gives edge weight from i to jj, but there are also edges from jj to i */
	gain = (2*a[j] - 2*deg[i]*deg[jj]*inv_deg_total)*inv_deg_total;
      } else {
	if (deg_inter[jc] > 0){
	  /* the first 2 is due to the fact that deg_iter gives edge weight from i to jc, but there are also edges from jc to i */
	  gain = (2*deg_inter[jc] - 2*deg[i]*deg_new[jc]*inv_deg_total)*inv_deg_total;
	  //	  printf("mod = %f deg_inter[jc] =%f, deg[i] = %f, deg_new[jc]=%f, gain = %f\n",modularity, deg_inter[jc],deg[i],deg_new[jc],gain);
	  deg_inter[jc] = -1; /* so that we do not redo the calulation when we hit another neighbor in cluster jc */
	} else {
	  gain = -1;
	}
      }
      if (j == ia[i] || gain > maxgain){
	maxgain = gain;
	jmax = jj;
      }

    }

    /* now merge i and jmax */
    if (maxgain > 0 || (nc >= 1 && nc > maxcluster)){
      total_gain += maxgain;
      jc = matching[jmax];
      if (jc == UNMATCHED){
	//     printf("maxgain=%f, merge %d, %d\n",maxgain, i, jmax);
	matching[i] = matching[jmax] = nc;
	deg_new[nc] = deg[i] + deg[jmax];
	nc++;
      } else {
	//	printf("maxgain=%f, merge with existing cluster %d, %d\n",maxgain, i, jc);
	deg_new[jc] += deg[i];
	matching[i] = jc;
      }
    } else {
      assert(maxgain <= 0);
      matching[i] = nc;
      deg_new[nc] = deg[i];
      nc++;
    }

  }

  if (Verbose) fprintf(stderr,"modularity = %f new modularity = %f level = %d, n = %d, nc = %d, gain = %g\n", modularity, modularity + total_gain, 
		       level, n, nc, total_gain);

  if (nc >= 1 && (total_gain > 0 || nc < n)){
    /* now set up restriction and prolongation operator */
    SparseMatrix P, R, R0, B, cA;
    real one = 1.;
    Multilevel_Modularity_Clustering cgrid;

    R0 = SparseMatrix_new(nc, n, 1, MATRIX_TYPE_REAL, FORMAT_COORD);
    for (i = 0; i < n; i++){
      jj = matching[i];
      SparseMatrix_coordinate_form_add_entries(R0, 1, &jj, &i, &one);
    }
    R = SparseMatrix_from_coordinate_format(R0);
    SparseMatrix_delete(R0);
    P = SparseMatrix_transpose(R);
    B = SparseMatrix_multiply(R, A);
    if (!B) goto RETURN;
    cA = SparseMatrix_multiply(B, P); 
    if (!cA) goto RETURN;
    SparseMatrix_delete(B);
    grid->P = P;
    grid->R = R;
    level++;
    cgrid = Multilevel_Modularity_Clustering_init(cA, level); 
    deg_new = REALLOC(deg_new, nc*sizeof(real));
    cgrid->deg = deg_new;
    cgrid->modularity = grid->modularity + total_gain;
    cgrid->deg_total = grid->deg_total;
    cgrid = Multilevel_Modularity_Clustering_establish(cgrid, maxcluster);
    grid->next = cgrid;
    cgrid->prev = grid;
  } else {
    /* no more improvement, stop and final clustering found */
    for (i = 0; i < n; i++) matching[i] = i;
    FREE(deg_new);
  }

 RETURN:
  FREE(deg_inter);
  FREE(mask);
  return grid;
}

Multilevel_Modularity_Clustering Multilevel_Modularity_Clustering_new(SparseMatrix A0, int maxcluster){
  /* maxcluster is used to specify the maximum number of cluster desired, e.g., maxcluster=10 means that a maximum of 10 clusters
     is desired. this may not always be realized, and modularity may be low when this is specified. Default: maxcluster = 0 */
  Multilevel_Modularity_Clustering grid;
  SparseMatrix A = A0;

  if (maxcluster <= 0) maxcluster = A->m;
  if (!SparseMatrix_is_symmetric(A, FALSE) || A->type != MATRIX_TYPE_REAL){
    A = SparseMatrix_get_real_adjacency_matrix_symmetrized(A);
  }
  grid = Multilevel_Modularity_Clustering_init(A, 0);

  grid = Multilevel_Modularity_Clustering_establish(grid, maxcluster);

  if (A != A0) grid->delete_top_level_A = TRUE;/* be sure to clean up later */
  return grid;
}


static void hierachical_modularity_clustering(SparseMatrix A, int maxcluster,
					      int *nclusters, int **assignment, real *modularity, int *flag){
  /* find a clustering of vertices by maximize modularity
     A: symmetric square matrix n x n. If real value, value will be used as edges weights, otherwise edge weights are considered as 1.
     maxcluster: used to specify the maximum number of cluster desired, e.g., maxcluster=10 means that a maximum of 10 clusters
     .   is desired. this may not always be realized, and modularity may be low when this is specified. Default: maxcluster = 0 
     nclusters: on output the number of clusters
     assignment: dimension n. Node i is assigned to cluster "assignment[i]". 0 <= assignment < nclusters
   */

  Multilevel_Modularity_Clustering grid, cgrid;
  int *matching, i;
  SparseMatrix P;
  real *u;
  assert(A->m == A->n);

  *modularity = 0.;

  *flag = 0;

  grid = Multilevel_Modularity_Clustering_new(A, maxcluster);

  /* find coarsest */
  cgrid = grid;
  while (cgrid->next){
    cgrid = cgrid->next;
  }

  /* project clustering up */
  u =  MALLOC(sizeof(real)*cgrid->n);
  for (i = 0; i < cgrid->n; i++) u[i] = (real) (cgrid->matching)[i];
  *nclusters = cgrid->n;
  *modularity = cgrid->modularity;

  while (cgrid->prev){
    real *v = NULL;
    P = cgrid->prev->P;
    SparseMatrix_multiply_vector(P, u, &v, FALSE);
    FREE(u);
    u = v;
    cgrid = cgrid->prev;
  }

  if (*assignment){
    matching = *assignment; 
  } else {
    matching = MALLOC(sizeof(int)*(grid->n));
    *assignment = matching;
  }
  for (i = 0; i < grid->n; i++) (matching)[i] = (int) u[i];
  FREE(u);

  Multilevel_Modularity_Clustering_delete(grid);
  
}



void modularity_clustering(SparseMatrix A, int inplace, int maxcluster, int use_value,
			   int *nclusters, int **assignment, real *modularity, int *flag){
  /* find a clustering of vertices by maximize modularity
     A: symmetric square matrix n x n. If real value, value will be used as edges weights, otherwise edge weights are considered as 1.
     inplace: whether A can e modified. If true, A will be modified by removing diagonal.
     maxcluster: used to specify the maximum number of cluster desired, e.g., maxcluster=10 means that a maximum of 10 clusters
     .   is desired. this may not always be realized, and modularity may be low when this is specified. Default: maxcluster = 0 
     nclusters: on output the number of clusters
     assignment: dimension n. Node i is assigned to cluster "assignment[i]". 0 <= assignment < nclusters
   */
  SparseMatrix B;

  *flag = 0;
  
  assert(A->m == A->n);

  B = SparseMatrix_symmetrize(A, FALSE);

  if (!inplace && B == A) {
    B = SparseMatrix_copy(A);
  }

  B = SparseMatrix_remove_diagonal(B);

  if (B->type != MATRIX_TYPE_REAL || !use_value) B = SparseMatrix_set_entries_to_real_one(B);

  hierachical_modularity_clustering(B, maxcluster, nclusters, assignment, modularity, flag);

  if (B != A) SparseMatrix_delete(B);

}
