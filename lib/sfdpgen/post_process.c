#include "general.h"
#include "SparseMatrix.h"
#include "spring_electrical.h"

#include "post_process.h"
#include "sparse_solve.h"
#include "call_tri.h"
#include <time.h>

#define node_degree(i) (ia[(i)+1] - ia[(i)])



SparseMatrix ideal_distance_matrix(SparseMatrix A, int dim, real *x){
  /* find the ideal distance between edges, either 1, or |N[i] \Union N[j]| - |N[i] \Intersection N[j]|
   */
  SparseMatrix D;
  int *ia, *ja, i, j, k, l, nz;
  real *d;
  int *mask = NULL;
  real len, di, sum, sumd;

  assert(SparseMatrix_is_symmetric(A, FALSE));

  D = SparseMatrix_copy(A);
  ia = D->ia;
  ja = D->ja;
  if (D->type != MATRIX_TYPE_REAL){
    FREE(D->a);
    D->type = MATRIX_TYPE_REAL;
    D->a = MALLOC(sizeof(real)*D->nz);
  }
  d = (real*) D->a;

  mask = MALLOC(sizeof(int)*D->m);
  for (i = 0; i < D->m; i++) mask[i] = -1;

  for (i = 0; i < D->m; i++){
    di = node_degree(i);
    mask[i] = i;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      mask[ja[j]] = i;
    }
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      if (i == k) continue;
      len = di + node_degree(k);
      for (l = ia[k]; l < ia[k+1]; l++){
	if (mask[ja[l]] == i) len--;
      }
      d[j] = len;
      assert(len > 0);
    }
    
  }

  sum = 0; sumd = 0;
  nz = 0;
  for (i = 0; i < D->m; i++){
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      nz++;
      sum += distance(x, dim, i, ja[j]);
      sumd += d[j];
    }
  }
  sum /= nz; sumd /= nz;
  sum = sum/sumd;

  for (i = 0; i < D->m; i++){
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      d[j] = sum*d[j];
    }
  }


  return D;
}

StressMajorizationSmoother StressMajorizationSmoother_new(SparseMatrix A, int dim, real lambda0, real *x, 
							  int ideal_dist_scheme){
  StressMajorizationSmoother sm;
  int i, j, k, l, m = A->m, *ia = A->ia, *ja = A->ja, *iw, *jw, *id, *jd;
  int *mask, nz;
  real *d, *w, *lambda;
  real *avg_dist, diag_d, diag_w, *dd, dist, s = 0, stop = 0, sbot = 0;
  SparseMatrix ID;

  assert(SparseMatrix_is_symmetric(A, FALSE));

  ID = ideal_distance_matrix(A, dim, x);
  dd = (real*) ID->a;

  sm = MALLOC(sizeof(struct StressMajorizationSmoother_struct));
  lambda = sm->lambda = MALLOC(sizeof(real)*m);
  for (i = 0; i < m; i++) sm->lambda[i] = lambda0;
  mask = MALLOC(sizeof(int)*m);

  avg_dist = MALLOC(sizeof(real)*m);

  for (i = 0; i < m ;i++){
    avg_dist[i] = 0;
    nz = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      avg_dist[i] += distance(x, dim, i, ja[j]);
      nz++;
    }
    assert(nz > 0);
    avg_dist[i] /= nz;
  }


  for (i = 0; i < m; i++) mask[i] = -1;

  nz = 0;
  for (i = 0; i < m; i++){
    mask[i] = i;
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      if (mask[k] != i){
	mask[k] = i;
	nz++;
      }
    }
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      for (l = ia[k]; l < ia[k+1]; l++){
	if (mask[ja[l]] != i){
	  mask[ja[l]] = i;
	  nz++;
	}
      }
    }
  }

  sm->Lw = SparseMatrix_new(m, m, nz + m, MATRIX_TYPE_REAL, FORMAT_CSR);
  sm->Lwd = SparseMatrix_new(m, m, nz + m, MATRIX_TYPE_REAL, FORMAT_CSR);
  if (!(sm->Lw) || !(sm->Lwd)) {
    StressMajorizationSmoother_delete(sm);
    return NULL;
  }

  iw = sm->Lw->ia; jw = sm->Lw->ja;
  id = sm->Lwd->ia; jd = sm->Lwd->ja;
  w = (real*) sm->Lw->a; d = (real*) sm->Lwd->a;
  iw[0] = id[0] = 0;

  nz = 0;
  for (i = 0; i < m; i++){
    mask[i] = i+m;
    diag_d = diag_w = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      if (mask[k] != i+m){
	mask[k] = i+m;

	jw[nz] = k;
	if (ideal_dist_scheme == IDEAL_GRAPH_DIST){
	  dist = 1;
	} else if (ideal_dist_scheme == IDEAL_AVG_DIST){
	  dist = (avg_dist[i] + avg_dist[k])*0.5;
	} else if (ideal_dist_scheme == IDEAL_POWER_DIST){
	  dist = pow(distance_cropped(x,dim,i,k),.4);
	} else {
	  fprintf(stderr,"ideal_dist_scheme value wrong");
	  assert(0);
	  exit(1);
	}

	/*	
	  w[nz] = -1./(ia[i+1]-ia[i]+ia[ja[j]+1]-ia[ja[j]]);
	  w[nz] = -2./(avg_dist[i]+avg_dist[k]);*/
	/* w[nz] = -1.;*//* use unit weight for now, later can try 1/(deg(i)+deg(k)) */
	w[nz] = -1/(dist*dist);

	diag_w += w[nz];

	jd[nz] = k;
	/*
	  d[nz] = w[nz]*distance(x,dim,i,k);
	  d[nz] = w[nz]*dd[j];
	  d[nz] = w[nz]*(avg_dist[i] + avg_dist[k])*0.5;
	*/
	d[nz] = w[nz]*dist;
	stop += d[nz]*distance(x,dim,i,k);
	sbot += d[nz]*dist;
	diag_d += d[nz];

	nz++;
      }
    }

    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      for (l = ia[k]; l < ia[k+1]; l++){
	if (mask[ja[l]] != i+m){
	  mask[ja[l]] = i+m;

	  if (ideal_dist_scheme == IDEAL_GRAPH_DIST){
	    dist = 2;
	  } else if (ideal_dist_scheme == IDEAL_AVG_DIST){
	    dist = (avg_dist[i] + 2*avg_dist[k] + avg_dist[ja[l]])*0.5;
	  } else if (ideal_dist_scheme == IDEAL_POWER_DIST){
	    dist = pow(distance_cropped(x,dim,i,ja[l]),.4);
	  } else {
	    fprintf(stderr,"ideal_dist_scheme value wrong");
	    assert(0);
	    exit(1);
	  }

	  jw[nz] = ja[l];
	  /*
	    w[nz] = -1/(ia[i+1]-ia[i]+ia[ja[l]+1]-ia[ja[l]]);
	    w[nz] = -2/(avg_dist[i] + 2*avg_dist[k] + avg_dist[ja[l]]);*/
	  /* w[nz] = -1.;*//* use unit weight for now, later can try 1/(deg(i)+deg(k)) */

	  w[nz] = -1/(dist*dist);

	  diag_w += w[nz];

	  jd[nz] = ja[l];
	  /*
	    d[nz] = w[nz]*(distance(x,dim,i,k)+distance(x,dim,k,ja[l]));
	    d[nz] = w[nz]*(dd[j]+dd[l]);
	    d[nz] = w[nz]*(avg_dist[i] + 2*avg_dist[k] + avg_dist[ja[l]])*0.5;
	  */
	  d[nz] = w[nz]*dist;
	  stop += d[nz]*distance(x,dim,ja[l],k);
	  sbot += d[nz]*dist;
	  diag_d += d[nz];

	  nz++;
	}
      }
    }
    jw[nz] = i;
    lambda[i] *= (-diag_w);/* alternatively don't do that then we have a constant penalty term scaled by lambda0 */

    w[nz] = -diag_w + lambda[i];
    jd[nz] = i;
    d[nz] = -diag_d;
    nz++;

    iw[i+1] = nz;
    id[i+1] = nz;
  }
  s = stop/sbot;
  for (i = 0; i < nz; i++) d[i] *= s;

  sm->Lw->nz = nz;
  sm->Lwd->nz = nz;

  FREE(mask);
  FREE(avg_dist);
  SparseMatrix_delete(ID);
  return sm;
}

static real total_distance(int m, int dim, real* x, real* y){
  real total = 0, dist = 0;
  int i, j;

  for (i = 0; i < m; i++){
    dist = 0.;
    for (j = 0; j < dim; j++){
      dist += (y[i*dim+j] - x[i*dim+j])*(y[i*dim+j] - x[i*dim+j]);
    }
    total += sqrt(dist);
  }
  return total;

}



void StressMajorizationSmoother_smooth(StressMajorizationSmoother sm, int dim, real *x, int maxit_sm) {
  SparseMatrix Lw = sm->Lw, Lwd = sm->Lwd, Lwdd = NULL;
  int i, j, m, *id, *jd, idiag, flag = 0, iter = 0;
  real *dd, *d, *y = NULL, *x0 = NULL, diag, diff = 1, tol = 0.001, *lambda = sm->lambda, maxit, res;

  Lwdd = SparseMatrix_copy(Lwd);
  m = Lw->m;
  x0 = MALLOC(sizeof(real)*dim*m);
  if (!x0) goto RETURN;

  x0 = MEMCPY(x0, x, sizeof(real)*dim*m);
  y = MALLOC(sizeof(real)*dim*m);
  if (!y) goto RETURN;

  id = Lwd->ia; jd = Lwd->ja;
  d = (real*) Lwd->a;
  dd = (real*) Lwdd->a;

  while (iter++ < maxit_sm && diff > tol){
    for (i = 0; i < m; i++){
      idiag = -1;
      diag = 0.;
      for (j = id[i]; j < id[i+1]; j++){
	if (i == jd[j]) {
	  idiag = j;
	  continue;
	}
	dd[j] = d[j]/distance_cropped(x, dim, i, jd[j]);
	diag += dd[j];
      }
      assert(idiag >= 0);
      dd[idiag] = -diag;
    }
    /* solve (Lw+lambda*I) x = Lwdd y + lambda x0 */

    SparseMatrix_multiply_dense(Lwdd, FALSE, x, FALSE, &y, FALSE, dim);
    for (i = 0; i < m; i++){
      for (j = 0; j < dim; j++){
	y[i*dim+j] += lambda[i]*x0[i*dim+j];
      }
    }

    maxit = sqrt((double) m);
    res = SparseMatrix_solve(Lw, dim, x, y, 0.01, maxit, SOLVE_METHOD_CG, &flag);

    if (flag) goto RETURN;

    diff = total_distance(m, dim, x, y)/m;
#ifdef DEBUG_PRINT
    if (Verbose){
      fprintf(stderr, "Outer iter = %d, res = %g Stress Majorization diff = %g\n",iter, res, diff);
    }
#endif
    MEMCPY(x, y, sizeof(real)*m*dim);
  }

#ifdef OVERLAP
  _statistics[1] += iter-1;
#endif

 RETURN:
  SparseMatrix_delete(Lwdd);
  if (!x0) FREE(x0);
  if (!y) FREE(y);
}

void StressMajorizationSmoother_delete(StressMajorizationSmoother sm){
  if (!sm) return;
  if (sm->Lw) SparseMatrix_delete(sm->Lw);
  if (sm->Lwd) SparseMatrix_delete(sm->Lwd);
  if (sm->lambda) FREE(sm->lambda);
}


TriangleSmoother TriangleSmoother_new(SparseMatrix A, int dim, real lambda0, real *x, int use_triangularization){
  TriangleSmoother sm;
  int i, j, k, m = A->m, *ia = A->ia, *ja = A->ja, *iw, *jw, *id, *jd, jdiag, nz;
  SparseMatrix B;
  real *avg_dist, *lambda, *d, *w, diag_d, diag_w, dist;
  real s = 0, stop = 0, sbot = 0;

  assert(SparseMatrix_is_symmetric(A, FALSE));

  avg_dist = MALLOC(sizeof(real)*m);

  for (i = 0; i < m ;i++){
    avg_dist[i] = 0;
    nz = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      avg_dist[i] += distance(x, dim, i, ja[j]);
      nz++;
    }
    assert(nz > 0);
    avg_dist[i] /= nz;
  }

  sm = MALLOC(sizeof(struct TriangleSmoother_struct));
  lambda = sm->lambda = MALLOC(sizeof(real)*m);
  for (i = 0; i < m; i++) sm->lambda[i] = lambda0;
  
  if (m > 2){
    if (use_triangularization){
      B= call_tri(m, dim, x);
    } else {
      B= call_tri2(m, dim, x);
    }
  } else {
    B = SparseMatrix_copy(A);
  }

  /*
  {FILE *fp;
  fp = fopen("/tmp/111","w");
  export_embedding(fp, dim, B, x, NULL);
  fclose(fp);}
  */


  sm->Lw = SparseMatrix_add(A, B);

  SparseMatrix_delete(B);
  sm->Lwd = SparseMatrix_copy(sm->Lw);
  if (!(sm->Lw) || !(sm->Lwd)) {
    TriangleSmoother_delete(sm);
    return NULL;
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
      /*      w[j] = -1./(ia[i+1]-ia[i]+ia[ja[j]+1]-ia[ja[j]]);
	      w[j] = -2./(avg_dist[i]+avg_dist[k]);
	      w[j] = -1.*/;/* use unit weight for now, later can try 1/(deg(i)+deg(k)) */
      dist = pow(distance_cropped(x,dim,i,k),0.6);
      w[j] = 1/(dist*dist);
      diag_w += w[j];

      /*      d[j] = w[j]*distance(x,dim,i,k);
	      d[j] = w[j]*(avg_dist[i] + avg_dist[k])*0.5;*/
      d[j] = w[j]*dist;
      stop += d[j]*distance(x,dim,i,k);
      sbot += d[j]*dist;
      diag_d += d[j];

    }

    lambda[i] *= (-diag_w);/* alternatively don't do that then we have a constant penalty term scaled by lambda0 */

    assert(jdiag >= 0);
    w[jdiag] = -diag_w + lambda[i];
    d[jdiag] = -diag_d;
  }

  s = stop/sbot;
  for (i = 0; i < iw[m]; i++) d[i] *= s;

  FREE(avg_dist);

  return sm;
}

void TriangleSmoother_delete(TriangleSmoother sm){

  StressMajorizationSmoother_delete(sm);

}

void TriangleSmoother_smooth(TriangleSmoother sm, int dim, real *x){

  StressMajorizationSmoother_smooth(sm, dim, x, 50);
}




/* ================================ spring and spring-electrical based smoother ================ */
SpringSmoother SpringSmoother_new(SparseMatrix A, int dim, spring_electrical_control ctrl, real *x){
  SpringSmoother sm;
  int i, j, k, l, m = A->m, *ia = A->ia, *ja = A->ja, *id, *jd;
  int *mask, nz;
  real *d, *dd;
  real *avg_dist;
  SparseMatrix ID = NULL;

  assert(SparseMatrix_is_symmetric(A, FALSE));

  ID = ideal_distance_matrix(A, dim, x);
  dd = (real*) ID->a;

  sm = MALLOC(sizeof(struct SpringSmoother_struct));
  mask = MALLOC(sizeof(int)*m);

  avg_dist = MALLOC(sizeof(real)*m);

  for (i = 0; i < m ;i++){
    avg_dist[i] = 0;
    nz = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      avg_dist[i] += distance(x, dim, i, ja[j]);
      nz++;
    }
    assert(nz > 0);
    avg_dist[i] /= nz;
  }


  for (i = 0; i < m; i++) mask[i] = -1;

  nz = 0;
  for (i = 0; i < m; i++){
    mask[i] = i;
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      if (mask[k] != i){
	mask[k] = i;
	nz++;
      }
    }
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      for (l = ia[k]; l < ia[k+1]; l++){
	if (mask[ja[l]] != i){
	  mask[ja[l]] = i;
	  nz++;
	}
      }
    }
  }

  sm->D = SparseMatrix_new(m, m, nz, MATRIX_TYPE_REAL, FORMAT_CSR);
  if (!(sm->D)){
    SpringSmoother_delete(sm);
    return NULL;
  }

  id = sm->D->ia; jd = sm->D->ja;
  d = (real*) sm->D->a;
  id[0] = 0;

  nz = 0;
  for (i = 0; i < m; i++){
    mask[i] = i+m;
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      if (mask[k] != i+m){
	mask[k] = i+m;
	jd[nz] = k;
	d[nz] = (avg_dist[i] + avg_dist[k])*0.5;
	d[nz] = dd[j];
	nz++;
      }
    }

    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      for (l = ia[k]; l < ia[k+1]; l++){
	if (mask[ja[l]] != i+m){
	  mask[ja[l]] = i+m;
	  jd[nz] = ja[l];
	  d[nz] = (avg_dist[i] + 2*avg_dist[k] + avg_dist[ja[l]])*0.5;
	  d[nz] = dd[j]+dd[l];
	  nz++;
	}
      }
    }
    id[i+1] = nz;
  }
  sm->D->nz = nz;
  sm->ctrl = spring_electrical_control_new();
  *(sm->ctrl) = *ctrl;
  sm->ctrl->random_start = FALSE;
  sm->ctrl->multilevels = 1;
  sm->ctrl->step /= 2;
  sm->ctrl->maxiter = 20;

  FREE(mask);
  FREE(avg_dist);
  SparseMatrix_delete(ID);

  return sm;
}


void SpringSmoother_delete(SpringSmoother sm){
  if (!sm) return;
  if (sm->D) SparseMatrix_delete(sm->D);
  if (sm->ctrl) spring_electrical_control_delete(sm->ctrl);
}




void SpringSmoother_smooth(SpringSmoother sm, SparseMatrix A, real *node_weights, int dim, real *x){
  int flag = 0;

  spring_electrical_spring_embedding(dim, A, sm->D, sm->ctrl, node_weights, x, &flag);
  assert(!flag);

}

/*=============================== end of spring and spring-electrical based smoother =========== */

void post_process_smoothing(int dim, SparseMatrix A, spring_electrical_control ctrl, real *node_weights, real *x, int *flag){
#ifdef TIME
  clock_t  cpu;
#endif

#ifdef TIME
  cpu = clock();
#endif
  *flag = 0;

  switch (ctrl->smoothing){
  case SMOOTHING_RNG:
  case SMOOTHING_TRIANGLE:{
    TriangleSmoother sm;
    
    if (ctrl->smoothing == SMOOTHING_RNG){
      sm = TriangleSmoother_new(A, dim, 0, x, FALSE);
    } else {
      sm = TriangleSmoother_new(A, dim, 0, x, TRUE);
    }
    TriangleSmoother_smooth(sm, dim, x);
    TriangleSmoother_delete(sm);
    break;
  }
  case SMOOTHING_STRESS_MAJORIZATION_GRAPH_DIST:
  case SMOOTHING_STRESS_MAJORIZATION_POWER_DIST:
  case SMOOTHING_STRESS_MAJORIZATION_AVG_DIST:
    {
      StressMajorizationSmoother sm;
      int k, dist_scheme = IDEAL_AVG_DIST;

      if (ctrl->smoothing == SMOOTHING_STRESS_MAJORIZATION_GRAPH_DIST){
	dist_scheme = IDEAL_GRAPH_DIST;
      } else if (ctrl->smoothing == SMOOTHING_STRESS_MAJORIZATION_AVG_DIST){
	dist_scheme = IDEAL_AVG_DIST;
      } else if (ctrl->smoothing == SMOOTHING_STRESS_MAJORIZATION_POWER_DIST){
	dist_scheme = IDEAL_POWER_DIST;
      }

      for (k = 0; k < 1; k++){
	sm = StressMajorizationSmoother_new(A, dim, 0.05, x, dist_scheme);
	StressMajorizationSmoother_smooth(sm, dim, x, 50);
	StressMajorizationSmoother_delete(sm);
      }
      break;
    }
  case SMOOTHING_SPRING:{
    SpringSmoother sm;
    int k;

    for (k = 0; k < 1; k++){
      sm = SpringSmoother_new(A, dim, ctrl, x);
      SpringSmoother_smooth(sm, A, node_weights, dim, x);
      SpringSmoother_delete(sm);
    }

    break;
  }

  }/* end switch between smoothing methods */

#ifdef TIME
  if (Verbose) fprintf(stderr, "post processing %f\n",((real) (clock() - cpu)) / CLOCKS_PER_SEC);
#endif
}
