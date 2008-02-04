#include "general.h"
#include "SparseMatrix.h"
#include "spring_electrical.h"
#include "post_process.h"
#include "overlap.h"
#include "call_tri.h"

static void ideal_distance_avoid_overlap(int dim, real pad, SparseMatrix A, real *x, real *width, real *ideal_distance, real *tmax){
  /*  if (x1>x2 && y1 > y2) we want either x1 + t (x1-x2) - x2 > pad + (width1+width2), or y1 + t (y1-y2) - y2 > pad + (height1+height2),
      hence t = MAX(expandmin, MIN(expandmax, (pad + width1+width2)/(x1-x2) - 1, (pad + height1+height2)/(y1-y2) - 1)), and
      new ideal distance = (1+t) old_distance. t can be negative sometimes.
      The result ideal distance is set to negative if the edge needs shrinking
  */
  int i, j, jj;
  int *ia = A->ia, *ja = A->ja;
  real dist, dx, dy, wx, wy, t;
  real expandmax = 1.5, expandmin = 1;

  *tmax = 0;
  assert(SparseMatrix_is_symmetric(A, FALSE));
  for (i = 0; i < A->m; i++){
    for (j = ia[i]; j < ia[i+1]; j++){
      jj = ja[j];
      if (jj == i) continue;
      dist = distance(x, dim, i, jj);
      dx = ABS(x[i*dim] - x[jj*dim]);
      dy = ABS(x[i*dim+1] - x[jj*dim+1]);
      wx = pad + width[i*dim]+width[jj*dim];
      wy = pad + width[i*dim+1]+width[jj*dim+1];
      if (dx < MACHINEACC*wx && dy < MACHINEACC*wy){
	ideal_distance[j] = sqrt(wx*wx+wy*wy);
	*tmax = 2;
      } else {
	if (dx < MACHINEACC*wx){
	  t = wy/dy;
	} else if (dy < MACHINEACC*wy){
	  t = wx/dx;
	} else {
	  t = MIN(wx/dx, wy/dy);
	}
	if (t > 1) t = MAX(t, 1.001);/* no point in things like t = 1.00000001 as this slow down convergence */
	*tmax = MAX(*tmax, t);
	t = MIN(expandmax, t);
	t = MAX(expandmin, t);
	if (t > 1) {
	  ideal_distance[j] = t*dist;
	} else {
	  ideal_distance[j] = -t*dist;
	}
      }

    }
  }
  return;
}



/* ============================== label overlap smoother ==================*/


OverlapSmoother OverlapSmoother_new(SparseMatrix A, int dim, real lambda0, real *x, real *width, int include_original_graph,
				    real *max_overlap){
  OverlapSmoother sm;
  int i, j, k, m = A->m, *iw, *jw, *id, *jd, jdiag;
  SparseMatrix B;
  real *lambda, *d, *w, diag_d, diag_w, dist, pad = 0;

  assert(SparseMatrix_is_symmetric(A, FALSE));

  sm = MALLOC(sizeof(struct OverlapSmoother_struct));
  lambda = sm->lambda = MALLOC(sizeof(real)*m);
  for (i = 0; i < m; i++) sm->lambda[i] = lambda0;
  
  if (m > 2){
    B= call_tri(m, dim, x);
  } else {
    B = SparseMatrix_copy(A);
  }

  if (include_original_graph){
    sm->Lw = SparseMatrix_add(A, B);
    SparseMatrix_delete(B);
  } else {
    sm->Lw = B;
  }
  sm->Lwd = SparseMatrix_copy(sm->Lw);

  {FILE *fp;
  fp = fopen("/tmp/111","w");
  export_embedding(fp, dim, sm->Lwd, x);
  fclose(fp);}


  if (!(sm->Lw) || !(sm->Lwd)) {
    OverlapSmoother_delete(sm);
    return NULL;
  }

  assert((sm->Lwd)->type == MATRIX_TYPE_REAL);
  
  ideal_distance_avoid_overlap(dim, pad, sm->Lwd, x, width, (real*) (sm->Lwd->a), max_overlap);

  /* no overlap at all! */
  if (*max_overlap < 1){
    for (i = 0; i < dim*m; i++) {
      x[i] *= (*max_overlap);
    }
    goto RETURN;
  }

  iw = sm->Lw->ia; jw = sm->Lw->ja;
  id = sm->Lwd->ia; jd = sm->Lwd->ja;
  w = (real*) sm->Lw->a; d = (real*) sm->Lwd->a;

  for (i = 0; i < m; i++){
    diag_d = diag_w = 0;
    jdiag = -1;
    for (j = iw[i]; j < iw[i+1]; j++){
      k = jw[j];
      if (k == i){
	jdiag = j;
	continue;
      }
      if (d[j] > 0){/* those edges that needs expansion */
	w[j] = 100/d[j]/d[j];
      } else {/* those that needs shrinking is set to negative in ideal_distance_avoid_overlap */
	w[j] = 1/d[j]/d[j];
	d[j] = -d[j];
      }
      dist = d[j];
      diag_w += w[j];
      d[j] = w[j]*dist;
      diag_d += d[j];

    }

    lambda[i] *= (-diag_w);/* alternatively don't do that then we have a constant penalty term scaled by lambda0 */

    assert(jdiag >= 0);
    w[jdiag] = -diag_w + lambda[i];
    d[jdiag] = -diag_d;
  }
 RETURN:
  return sm;
}

void OverlapSmoother_delete(OverlapSmoother sm){

  StressMajorizationSmoother_delete(sm);

}

void OverlapSmoother_smooth(OverlapSmoother sm, int dim, real *x){

  StressMajorizationSmoother_smooth(sm, dim, x);
  {FILE *fp;
  fp = fopen("/tmp/222","w");
  export_embedding(fp, dim, sm->Lwd, x);
  fclose(fp);}

}

/*================================= end OverlapSmoother =============*/

static void scale_to_edge_length(int dim, SparseMatrix A, real *x, real avg_label_size){
  real dist;
  int i;

  dist = average_edge_length(A, dim, x);
  fprintf(stderr,"avg edge len=%f avg_label-size= %f\n", dist, avg_label_size);


  dist = avg_label_size/MAX(dist, MACHINEACC);

  for (i = 0; i < dim*A->m; i++) x[i] *= dist;
}

void remove_overlap(int dim, SparseMatrix A, real *x, real *label_sizes, int ntry, int *flag){
  real lambda = 0.00;
  OverlapSmoother sm;
  int include_original_graph = 0, i;
  real avg_label_size;
  real max_overlap = FALSE;

  if (!label_sizes) return;

  avg_label_size = 0;
  for (i = 0; i < A->m; i++) avg_label_size += label_sizes[i*dim]+label_sizes[i*dim+1];
  avg_label_size /= A->m;

  scale_to_edge_length(dim, A, x,4*avg_label_size);

  *flag = 0;

  for (i = 0; i < ntry; i++){
    sm = OverlapSmoother_new(A, dim, lambda, x, label_sizes, include_original_graph, &max_overlap); 
    fprintf(stderr, "try %d, tmax = %f\n", i, max_overlap);

    if (max_overlap <= 1){
      OverlapSmoother_delete(sm);
      break;
    }
    OverlapSmoother_smooth(sm, dim, x);
    OverlapSmoother_delete(sm);
  }

}
