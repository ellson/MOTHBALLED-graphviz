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


 
#include "spring_electrical.h"
#include "QuadTree.h"
#include "Multilevel.h"
#include "post_process.h"
#include "overlap.h"
#include "types.h"
#include "memory.h"
#include "arith.h"
#include "logic.h"
#include "math.h"
#include "globals.h"
#include <string.h>
#include <time.h>

#define drand() (rand()/(real) RAND_MAX)

#define DEBUG_PRINT
#define MALLOC gmalloc
#define REALLOC grealloc
#define FREE free
#define MEMCPY memcpy


spring_electrical_control spring_electrical_control_new(){
  spring_electrical_control ctrl;
  ctrl = N_GNEW(1,struct spring_electrical_control_struct);
  ctrl->p = AUTOP;/*a negativve number default to -1. repulsive force = dist^p */
  ctrl->random_start = TRUE;/* whether to apply SE from a random layout, or from exisiting layout */
  ctrl->K = -1;/* the natural distance. If K < 0, K will be set to the average distance of an edge */
  ctrl->C = 0.2;/* another parameter. f_a(i,j) = C*dist(i,j)^2/K * d_ij, f_r(i,j) = K^(3-p)/dist(i,j)^(-p). By default C = 0.2. */
  ctrl->multilevels = FALSE;/* if <=1, single level */
  ctrl->quadtree_size = 45;/* cut off size above which quadtree approximation is used */
  ctrl->max_qtree_level = 10;/* max level of quadtree */
  ctrl->bh = 0.6;/* Barnes-Hutt constant, if width(snode)/dist[i,snode] < bh, treat snode as a supernode.*/
  ctrl->tol = 0.001;/* minimum different between two subsequence config before terminating. ||x-xold||_infinity < tol/K */
  ctrl->maxiter = 500;
  ctrl->cool = 0.90;/* default 0.9 */
  ctrl->step = 0.1;
  ctrl->adaptive_cooling = TRUE;
  ctrl->random_seed = 123;
  ctrl->beautify_leaves = FALSE;
  ctrl->use_node_weights = FALSE;
  ctrl->smoothing = SMOOTHING_NONE;
  ctrl->overlap = 0;
  ctrl->tscheme = QUAD_TREE_NORMAL;
  ctrl->posSet = NULL;
  ctrl->initial_scaling = -4;
  
  return ctrl;
}

void spring_electrical_control_delete(spring_electrical_control ctrl){
  FREE(ctrl);
}

enum {MAX_I = 20, OPT_UP = 1, OPT_DOWN = -1, OPT_INIT = 0};
struct oned_optimizer_struct{
  int i;
  real work[MAX_I+1];
  int direction;
};

typedef struct oned_optimizer_struct *oned_optimizer;


static void oned_optimizer_delete(oned_optimizer opt){
  FREE(opt);
}

static oned_optimizer oned_optimizer_new(int i){
  oned_optimizer opt;
  opt = N_GNEW(1,struct oned_optimizer_struct);
  opt->i = i;
  opt->direction = OPT_INIT;
  return opt;
}

static void oned_optimizer_train(oned_optimizer opt, real work){
  int i = opt->i;
 
  assert(i >= 0);
  opt->work[i] = work;
  if (opt->direction == OPT_INIT){
    if (opt->i == MAX_I){
      opt->direction = OPT_DOWN;
      opt->i = opt->i - 1;
    } else {
      opt->direction = OPT_UP;
      opt->i = MIN(MAX_I, opt->i + 1);
    }
  } else if (opt->direction == OPT_UP){
    /*    fprintf(stderr, "{current_level, prev_level} = {%d,%d}, {work, work_prev} = {%f,%f}",i,i-1,opt->work[i], opt->work[i-1]);*/
    assert(i >= 1);
    if (opt->work[i] < opt->work[i-1] && opt->i < MAX_I){
      /*      fprintf(stderr, "keep going up to level %d\n",opt->i+1);*/
      opt->i = MIN(MAX_I, opt->i + 1);
    } else {
      /*      fprintf(stderr, "going down to level %d\n",opt->i-1);*/
      (opt->i)--;
      opt->direction = OPT_DOWN;
    }
  } else {
    assert(i < MAX_I);
    /*    fprintf(stderr, "{current_level, prev_level} = {%d,%d}, {work, work_prev} = {%f,%f}",i,i+1,opt->work[i], opt->work[i+1]);*/
    if (opt->work[i] < opt->work[i+1] && opt->i > 0){
      /*      fprintf(stderr, "keep going down to level %d\n",opt->i-1);*/
      opt->i = MAX(0, opt->i-1);
    } else {
      /*      fprintf(stderr, "keep up to level %d\n",opt->i+1);*/
      (opt->i)++;
      opt->direction = OPT_UP;
    }
  }
}

static int oned_optimizer_get(oned_optimizer opt){
  return opt->i;
}


real average_edge_length(SparseMatrix A, int dim, real *coord){
  real dist = 0, d;
  int *ia = A->ia, *ja = A->ja, i, j, k;
  assert(SparseMatrix_is_symmetric(A, TRUE));

  if (ia[A->m] == 0) return 1;
  for (i = 0; i < A->m; i++){
    for (j = ia[i]; j < ia[i+1]; j++){
      d = 0;
      for (k = 0; k < dim; k++){
	d += (coord[dim*i+k] - coord[dim*ja[j]])*(coord[dim*i+k] - coord[dim*ja[j]]);
      }
      dist += sqrt(d);
    }
  }
  return dist/ia[A->m];
}

static real
initPos (SparseMatrix A, int n, int dim, real* x, spring_electrical_control ctrl)
{
  int i, j;
  char* posSet;

  if (ctrl->random_start){
    srand(ctrl->random_seed);
    if ((posSet = ctrl->posSet)) {
	for (i = 0; i < n; i++) {
	    if (posSet[i]) continue;
	    for (j = 0; j < dim; j++) {
		x[dim*i + j] = drand();
	    }
	}
    }
    else
	for (i = 0; i < dim*n; i++) x[i] = drand();
  }

  if (ctrl->K < 0){
    ctrl->K = average_edge_length(A, dim, x);
  }

  return ctrl->K;
}

real distance_cropped(real *x, int dim, int i, int j){
  int k;
  real dist = 0.;
  for (k = 0; k < dim; k++) dist += (x[i*dim+k] - x[j*dim + k])*(x[i*dim+k] - x[j*dim + k]);
  dist = sqrt(dist);
  return MAX(dist, MINDIST);
}

real distance(real *x, int dim, int i, int j){
  int k;
  real dist = 0.;
  for (k = 0; k < dim; k++) dist += (x[i*dim+k] - x[j*dim + k])*(x[i*dim+k] - x[j*dim + k]);
  dist = sqrt(dist);
  return dist;
}

#ifdef ENERGY
static real spring_electrical_energy(int dim, SparseMatrix A, real *x, real p, real CRK, real KP){
      /* 1. Grad[||x-y||^k,x] = k||x-y||^(k-1)*0.5*(x-y)/||x-y|| = k/2*||x-y||^(k-2) (x-y) 
	 which should equal to -force (force = -gradient),
	 hence energy for force ||x-y||^m (y-x) is ||x-y||^(m+2)*2/(m+2) where m != 2
	 2. Grad[Log[||x-y||],x] = 1/||x-y||*0.5*(x-y)/||x-y|| = 0.5*(x-y)/||x-y||^2,
	 hence the energy to give force ||x-y||^-2 (x-y) is -2*Log[||x-y||]

      */
  int i, j, k, *ia = A->ia, *ja = A->ja, n = A->m;
  real energy = 0, dist;

  for (i = 0; i < n; i++){
    /* attractive force   C^((2-p)/3) ||x_i-x_j||/K * (x_j - x_i) */
    for (j = ia[i]; j < ia[i+1]; j++){
      if (ja[j] == i) continue;
      dist = distance(x, dim, i, ja[j]);
      energy += CRK*pow(dist, 3.)*2./3.;
    }
    
    /* repulsive force K^(1 - p)/||x_i-x_j||^(1 - p) (x_i - x_j) */
    for (j = 0; j < n; j++){
      if (j == i) continue;
      dist = distance_cropped(x, dim, i, j);
      for (k = 0; k < dim; k++){
	if (p == -1){
	  energy  += -KP*2*log(dist);
	} else {
	  energy += -KP*pow(dist,p+1)*2/(p+1);
	}
      }
    }
  }
  return energy;
}

#endif

void export_embedding(FILE *fp, int dim, SparseMatrix A, real *x, real *width){
  int i, j, k, *ia=A->ia, *ja = A->ja;
  int ne = 0;
  if (dim == 2){
    fprintf(fp,"Graphics[{GrayLevel[0.5],Line[{");
  } else {
    fprintf(fp,"Graphics3D[{GrayLevel[0.5],Line[{");
  }
  for (i = 0; i < A->m; i++){
    for (j = ia[i]; j < ia[i+1]; j++){
      if (ja[j] == i) continue;
      ne++;
      if (ne > 1) fprintf(fp, ",");
      fprintf(fp, "{{");
      for (k = 0; k < dim; k++) {
	if (k > 0) fprintf(fp,",");
	fprintf(fp, "%f",x[i*dim+k]);
      }
      fprintf(fp, "},{");
      for (k = 0; k < dim; k++) {
	if (k > 0) fprintf(fp,",");
	fprintf(fp, "%f",x[ja[j]*dim+k]);
      }
      fprintf(fp, "}}");
    }
  }

  fprintf(fp,"}],Hue[%f],",/*drand()*/1.);
 
  if (A->m < 100){
    for (i = 0; i < A->m; i++){
      if (i > 0) fprintf(fp,",");
      fprintf(fp,"Text[%d,{",i+1);
      for (k = 0; k < dim; k++) {
	if (k > 0) fprintf(fp,",");
	fprintf(fp, "%f",x[i*dim+k]);
      }
      fprintf(fp,"}]");
    }
  } else if (A->m < 500000){
    fprintf(fp, "Point[{");
    for (i = 0; i < A->m; i++){
      if (i > 0) fprintf(fp,",");
      fprintf(fp,"{");
      for (k = 0; k < dim; k++) {
	if (k > 0) fprintf(fp,",");
	fprintf(fp, "%f",x[i*dim+k]);
      }
      fprintf(fp,"}");
    }
    fprintf(fp, "}]");
  } else {
      fprintf(fp,"{}");
  }

  if (width && dim == 2){
    fprintf(fp, ",");
    for (i = 0; i < A->m; i++){
      if (i > 0) fprintf(fp,",");
      fprintf(fp,"(*%f,%f*){GrayLevel[.5,.5],Rectangle[{%f,%f},{%f,%f}]}", width[i*dim], width[i*dim+1],x[i*dim] - width[i*dim], x[i*dim+1] - width[i*dim+1],
	      x[i*dim] + width[i*dim], x[i*dim+1] + width[i*dim+1]);
    }
  }

 fprintf(fp,"},ImageSize->600]\n");

}

static real update_step(int adaptive_cooling, real step, real Fnorm, real Fnorm0, real cool){

  if (!adaptive_cooling) {
    return cool*step;
  }
  if (Fnorm >= Fnorm0){
    step = cool*step;
  } else if (Fnorm > 0.95*Fnorm0){
    step = step;
  } else {
    step = 0.99*step/cool;
  }
  return step;
}
 

#define node_degree(i) (ia[(i)+1] - ia[(i)])

void check_real_array_size(real **a, int len, int *lenmax){
  if (len >= *lenmax){
    *lenmax = len + MAX((int) 0.2*len, 10);
    *a = RALLOC((*lenmax),*a,real);
  }
  
}
void check_int_array_size(int **a, int len, int *lenmax){
  if (len >= *lenmax){
    *lenmax = len + MAX((int) 0.2*len, 10);
    *a = RALLOC((*lenmax),*a,int);
  }
  
}

real get_angle(real *x, int dim, int i, int j){
  /* between [0, 2Pi)*/
  int k;
  real y[2], res;
  real eps = 0.00001;
  for (k = 0; k < 2; k++){
    y[k] = x[j*dim+k] - x[i*dim+k];
  }
  if (ABS(y[0]) <= ABS(y[1])*eps){
    if (y[1] > 0) return 0.5*M_PI;
    return 1.5*M_PI;
  }
  res = atan(y[1]/y[0]);
  if (y[0] > 0){
    if (y[1] < 0) res = 2*M_PI+res;
  } else if (y[0] < 0){
    res = res + M_PI;
  }
  return res;
}

int comp_real(const void *x, const void *y){
  real *xx = (real*) x;
  real *yy = (real*) y;

  if (*xx > *yy){
    return 1;
  } else if (*xx < *yy){
    return -1;
  }
  return 0;
}
static void sort_real(int n, real *a){
  qsort(a, n, sizeof(real), comp_real);
}


static void set_leaves(real *x, int dim, real dist, real ang, int i, int j){
  x[dim*j] = cos(ang)*dist + x[dim*i];
  x[dim*j+1] = sin(ang)*dist + x[dim*i+1];
}

static void beautify_leaves(int dim, SparseMatrix A, real *x){
  int m = A->m, i, j, *ia = A->ia, *ja = A->ja, k;
  int *checked, p;
  real dist;
  int nleaves, nleaves_max = 10;
  real *angles, maxang, ang1 = 0, ang2 = 0, pad, step;
  int *leaves, nangles_max = 10, nangles;

  assert(!SparseMatrix_has_diagonal(A));

  checked = N_GNEW(m,int);
  angles = N_GNEW(nangles_max,real);
  leaves = N_GNEW(nleaves_max,int);


  for (i = 0; i < m; i++) checked[i] = FALSE;

  for (i = 0; i < m; i++){
    if (ia[i+1] - ia[i] != 1) continue;
    if (checked[i]) continue;
    p = ja[ia[i]];
    if (!checked[p]){
      checked[p] = TRUE;
      dist = 0; nleaves = 0; nangles = 0;
      for (j = ia[p]; j < ia[p+1]; j++){
	if (node_degree(ja[j]) == 1){
	  checked[ja[j]] = TRUE;
	  check_int_array_size(&leaves, nleaves, &nleaves_max);
	  dist += distance(x, dim, p, ja[j]);
	  leaves[nleaves] = ja[j];
	  nleaves++;
	} else {
	  check_real_array_size(&angles, nangles, &nangles_max);
	  angles[nangles++] = get_angle(x, dim, p, ja[j]);
	}
      }
      assert(nleaves > 0);
      dist /= nleaves;
      if (nangles > 0){
	sort_real(nangles, angles);
	maxang = 0;
	for (k = 0; k < nangles - 1; k++){
	  if (angles[k+1] - angles[k] > maxang){
	    maxang = angles[k+1] - angles[k]; 
	    ang1 = angles[k]; ang2 = angles[k+1];
	  }
	}
	if (2*M_PI + angles[0] - angles[nangles - 1] > maxang){
	  maxang = 2*M_PI + angles[0] - angles[nangles - 1];
	  ang1 = angles[nangles - 1];
	  ang2 = 2*M_PI + angles[0];
	}
      } else {
	ang1 = 0; ang2 = 2*M_PI; maxang = 2*M_PI;
      }
      pad = MAX(maxang - M_PI*0.166667*(nleaves-1), 0)*0.5;
      ang1 += pad*0.95;
      ang2 -= pad*0.95;
      assert(ang2 >= ang1);
      step = 0.;
      if (nleaves > 1) step = (ang2 - ang1)/(nleaves - 1);
      for (i = 0; i < nleaves; i++) {
	set_leaves(x, dim, dist, ang1, p, leaves[i]);
	ang1 += step;
      }
    }
  }


  FREE(checked);
  FREE(angles);
  FREE(leaves);
}

void force_print(FILE *fp, int n, int dim, real *x, real *force){
  int i, k;
  
  fprintf(fp,"Graphics[{");
  for (i = 0; i < n; i++){
    if (i > 0) fprintf(fp, ",");
    fprintf(fp, "Arrow[{{");
    for (k = 0; k < dim; k++){
      if (k > 0) fprintf(fp, ",");
      fprintf(fp, "%f",x[i*dim+k]);
    }
    fprintf(fp, "},{");
    for (k = 0; k < dim; k++){
      if (k > 0) fprintf(fp, ",");
      fprintf(fp, "%f",x[i*dim+k]+0.5*force[i*dim+k]);
    }
    fprintf(fp, "}}]");
  }
  fprintf(fp,",");
  for (i = 0; i < n; i++){
    if (i > 0) fprintf(fp, ",");
    fprintf(fp, "Tooltip[Point[{");
    for (k = 0; k < dim; k++){
      if (k > 0) fprintf(fp, ",");
      fprintf(fp, "%f",x[i*dim+k]);
    }
    fprintf(fp, "}],%d]",i);
  }




  fprintf(fp,"}]\n");

}


void spring_electrical_embedding_fast(int dim, SparseMatrix A0, spring_electrical_control ctrl, real *node_weights, real *x, int *flag){
  /* x is a point to a 1D array, x[i*dim+j] gives the coordinate of the i-th node at dimension j.  */
  SparseMatrix A = A0;
  int m, n;
  int i, j, k;
  real p = ctrl->p, K = ctrl->K, C = ctrl->C, CRK, tol = ctrl->tol, maxiter = ctrl->maxiter, cool = ctrl->cool, step = ctrl->step, KP;
  int *ia = NULL, *ja = NULL;
  real *xold = NULL;
  real *f = NULL, dist, F, Fnorm = 0, Fnorm0;
  int iter = 0;
  int adaptive_cooling = ctrl->adaptive_cooling;
  QuadTree qt = NULL;
  real counts[4], *force = NULL;
#ifdef TIME
  clock_t start, end, start0, start2;
  real qtree_cpu = 0, qtree_cpu0 = 0, qtree_new_cpu = 0, qtree_new_cpu0 = 0;
  real total_cpu = 0;
  start0 = clock();
#endif  
  int max_qtree_level = ctrl->max_qtree_level;
  oned_optimizer qtree_level_optimizer = NULL;

  if (!A) return;

  m = A->m, n = A->n;
  if (n <= 0 || dim <= 0) return;

  qtree_level_optimizer = oned_optimizer_new(max_qtree_level);

  *flag = 0;
  if (m != n) {
    *flag = ERROR_NOT_SQUARE_MATRIX;
    goto RETURN;
  }
  assert(A->format == FORMAT_CSR);
  A = SparseMatrix_symmetrize(A, TRUE);
  ia = A->ia;
  ja = A->ja;

  K = initPos (A, n, dim, x, ctrl);

  if (C < 0) ctrl->C = C = 0.2;
  if (p >= 0) ctrl->p = p = -1;
  KP = pow(K, 1 - p);
  CRK = pow(C, (2.-p)/3.)/K;

  xold = N_GNEW(dim*n,real); 
  force = N_GNEW(dim*n,real);

  do {
#ifdef TIME
    start2 = clock();
#endif

    iter++;
    xold = MEMCPY(xold, x, sizeof(real)*dim*n);
    Fnorm0 = Fnorm;
    Fnorm = 0.;

    max_qtree_level = oned_optimizer_get(qtree_level_optimizer);
    
#ifdef TIME
    start = clock();
#endif
    if (ctrl->use_node_weights){
      qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, node_weights);
    } else {
      qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, NULL);
    }
  
#ifdef TIME
    qtree_new_cpu += ((real) (clock() - start))/CLOCKS_PER_SEC;
#endif

    /* repulsive force */
#ifdef TIME
    start = clock();
#endif

    QuadTree_get_repulvice_force(qt, force, x, ctrl->bh, p, KP, counts, flag);

    assert(!(*flag));

#ifdef TIME
    end = clock();
    qtree_cpu += ((real) (end - start)) / CLOCKS_PER_SEC;
#endif

    /* attractive force   C^((2-p)/3) ||x_i-x_j||/K * (x_j - x_i) */
    for (i = 0; i < n; i++){
      f = &(force[i*dim]);
      for (j = ia[i]; j < ia[i+1]; j++){
	if (ja[j] == i) continue;
	dist = distance(x, dim, i, ja[j]);
	for (k = 0; k < dim; k++){
	  f[k] -= CRK*(x[i*dim+k] - x[ja[j]*dim+k])*dist;
	}
      }
    }
  

    /* move */
    for (i = 0; i < n; i++){
      f = &(force[i*dim]);
      F = 0.;
      for (k = 0; k < dim; k++) F += f[k]*f[k];
      F = sqrt(F);
      Fnorm += F;
      if (F > 0) for (k = 0; k < dim; k++) f[k] /= F;
      for (k = 0; k < dim; k++) x[i*dim+k] += step*f[k];
    }/* done vertex i */



    if (qt) {
#ifdef TIME
      start = clock();
#endif
      QuadTree_delete(qt);
#ifdef TIME
      end = clock();
      qtree_new_cpu += ((real) (end - start)) / CLOCKS_PER_SEC;
#endif

#ifdef TIME
      qtree_cpu0 = qtree_cpu - qtree_cpu0;
      qtree_new_cpu0 = qtree_new_cpu - qtree_new_cpu0;
      if (Verbose) fprintf(stderr, "\r iter=%d cpu=%.2f, quadtree=%.2f quad_force=%.2f other=%.2f counts={%.2f,%.2f,%.2f} step=%f Fnorm=%f nz=%d  K=%f qtree_lev = %d",
			   iter, ((real) (clock() - start2)) / CLOCKS_PER_SEC, qtree_new_cpu0, 
			   qtree_cpu0,((real) (clock() - start2))/CLOCKS_PER_SEC - qtree_cpu0 - qtree_new_cpu0, 
			   counts[0], counts[1], counts[2],
			   step, Fnorm, A->nz,K,max_qtree_level);
      qtree_cpu0 = qtree_cpu;
      qtree_new_cpu0 = qtree_new_cpu;
#endif
      oned_optimizer_train(qtree_level_optimizer, counts[0]+0.85*counts[1]+3.3*counts[2]);
    } else {   
#ifdef DEBUG_PRINT
      if (Verbose) {
        fprintf(stderr, "\r                iter = %d, step = %f Fnorm = %f nz = %d  K = %f                                  ",iter, step, Fnorm, A->nz,K);
#ifdef ENERGY
        fprintf(stderr, "energy = %f\n",spring_electrical_energy(dim, A, x, p, CRK, KP));
#endif
      }
#endif
    }

    step = update_step(adaptive_cooling, step, Fnorm, Fnorm0, cool);
  } while (step > tol && iter < maxiter);

#ifdef DEBUG_PRINT
  if (Verbose && 0) fputs("\n", stderr);
#endif


#ifdef DEBUG_PRINT
    if (Verbose) {
      fprintf(stderr, "\n iter = %d, step = %f Fnorm = %f nz = %d  K = %f   ",iter, step, Fnorm, A->nz, K);
    }
#endif

  if (ctrl->beautify_leaves) beautify_leaves(dim, A, x);

#ifdef TIME
  total_cpu += ((real) (clock() - start0)) / CLOCKS_PER_SEC;
  if (Verbose) fprintf(stderr, "\n time for qtree = %f, qtree_force = %f, total cpu = %f\n",qtree_new_cpu, qtree_cpu, total_cpu);
#endif


 RETURN:
  oned_optimizer_delete(qtree_level_optimizer);
  ctrl->max_qtree_level = max_qtree_level;

  if (xold) FREE(xold);
  if (A != A0) SparseMatrix_delete(A);
  if (force) FREE(force);

}


void spring_electrical_embedding_slow(int dim, SparseMatrix A0, spring_electrical_control ctrl, real *node_weights, real *x, int *flag){
  /* a version that does vertex moves in one go, instead of one at a time, use for debugging the fast version. Quadtree is not used. */
  /* x is a point to a 1D array, x[i*dim+j] gives the coordinate of the i-th node at dimension j.  */
  SparseMatrix A = A0;
  int m, n;
  int i, j, k;
  real p = ctrl->p, K = ctrl->K, C = ctrl->C, CRK, tol = ctrl->tol, maxiter = ctrl->maxiter, cool = ctrl->cool, step = ctrl->step, KP;
  int *ia = NULL, *ja = NULL;
  real *xold = NULL;
  real *f = NULL, dist, F, Fnorm = 0, Fnorm0;
  int iter = 0;
  int adaptive_cooling = ctrl->adaptive_cooling;
  QuadTree qt = NULL;
  int USE_QT = FALSE;
  int nsuper = 0, nsupermax = 10;
  real *center = NULL, *supernode_wgts = NULL, *distances = NULL, nsuper_avg, counts = 0, counts_avg = 0;
  real *force;
#ifdef TIME
  clock_t start, end, start0, start2;
  real qtree_cpu = 0, qtree_cpu0 = 0;
  real total_cpu = 0;
  start0 = clock();
#endif  
  int max_qtree_level = ctrl->max_qtree_level;
  oned_optimizer qtree_level_optimizer = NULL;

  fprintf(stderr,"spring_electrical_embedding_slow");
  if (!A) return;

  m = A->m, n = A->n;
  if (n <= 0 || dim <= 0) return;
  force = N_GNEW(n*dim,real);

  if (n >= ctrl->quadtree_size) {
    USE_QT = TRUE;
    qtree_level_optimizer = oned_optimizer_new(max_qtree_level);
    center = N_GNEW(nsupermax*dim,real);
    supernode_wgts = N_GNEW(nsupermax,real);
    distances = N_GNEW(nsupermax,real);
  }
  USE_QT = FALSE;
  *flag = 0;
  if (m != n) {
    *flag = ERROR_NOT_SQUARE_MATRIX;
    goto RETURN;
  }
  assert(A->format == FORMAT_CSR);
  A = SparseMatrix_symmetrize(A, TRUE);
  ia = A->ia;
  ja = A->ja;

  K = initPos (A, n, dim, x, ctrl);

  if (C < 0) ctrl->C = C = 0.2;
  if (p >= 0) ctrl->p = p = -1;
  KP = pow(K, 1 - p);
  CRK = pow(C, (2.-p)/3.)/K;

#ifdef DEBUG_0
  {
    FILE *f;
    char fname[10000];
    strcpy(fname,"/tmp/graph_layout_0_");
    sprintf(&(fname[strlen(fname)]), "%d",n);
    f = fopen(fname,"w");
    export_embedding(f, dim, A, x, NULL);
    fclose(f);
  }
#endif

  f = N_GNEW(dim,real);
  xold = N_GNEW(dim*n,real); 
  do {
    for (i = 0; i < dim*n; i++) force[i] = 0;

    iter++;
    xold = MEMCPY(xold, x, sizeof(real)*dim*n);
    Fnorm0 = Fnorm;
    Fnorm = 0.;
    nsuper_avg = 0;

    if (USE_QT) {
      max_qtree_level = oned_optimizer_get(qtree_level_optimizer);
      if (ctrl->use_node_weights){
	qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, node_weights);
      } else {
	qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, NULL);
      }
    }
#ifdef TIME
    start2 = clock();
#endif


    for (i = 0; i < n; i++){
      for (k = 0; k < dim; k++) f[k] = 0.;
     /* repulsive force K^(1 - p)/||x_i-x_j||^(1 - p) (x_i - x_j) */
      if (USE_QT){
#ifdef TIME
	start = clock();
#endif
	QuadTree_get_supernodes(qt, ctrl->bh, &(x[dim*i]), i, &nsuper, &nsupermax, 
				&center, &supernode_wgts, &distances, &counts, flag);
#ifdef TIME
	end = clock();
	qtree_cpu += ((real) (end - start)) / CLOCKS_PER_SEC;
#endif
	counts_avg += counts;
	nsuper_avg += nsuper;
	if (*flag) goto RETURN;
	for (j = 0; j < nsuper; j++){
	  dist = MAX(distances[j], MINDIST);
	  for (k = 0; k < dim; k++){
	    if (p == -1){
	      f[k] += supernode_wgts[j]*KP*(x[i*dim+k] - center[j*dim+k])/(dist*dist);
	    } else {
	      f[k] += supernode_wgts[j]*KP*(x[i*dim+k] - center[j*dim+k])/pow(dist, 1.- p);
	    }
	  }
	}
      } else {
	if (ctrl->use_node_weights && node_weights){
	  for (j = 0; j < n; j++){
	    if (j == i) continue;
	    dist = distance_cropped(x, dim, i, j);
	    for (k = 0; k < dim; k++){
	      if (p == -1){
		f[k] += node_weights[j]*KP*(x[i*dim+k] - x[j*dim+k])/(dist*dist);
	      } else {
		f[k] += node_weights[j]*KP*(x[i*dim+k] - x[j*dim+k])/pow(dist, 1.- p);
	      }
	    }
	  }
	} else {
	  for (j = 0; j < n; j++){
	    if (j == i) continue;
	    dist = distance_cropped(x, dim, i, j);
	    for (k = 0; k < dim; k++){
	      if (p == -1){
		f[k] += KP*(x[i*dim+k] - x[j*dim+k])/(dist*dist);
	      } else {
		f[k] += KP*(x[i*dim+k] - x[j*dim+k])/pow(dist, 1.- p);
	      }
	    }
	  }
	}
      }
      for (k = 0; k < dim; k++) force[i*dim+k] += f[k];
    }

 

    for (i = 0; i < n; i++){
      for (k = 0; k < dim; k++) f[k] = 0.;
      /* attractive force   C^((2-p)/3) ||x_i-x_j||/K * (x_j - x_i) */
      for (j = ia[i]; j < ia[i+1]; j++){
	if (ja[j] == i) continue;
	dist = distance(x, dim, i, ja[j]);
	for (k = 0; k < dim; k++){
	  f[k] -= CRK*(x[i*dim+k] - x[ja[j]*dim+k])*dist;
	}
      }
      for (k = 0; k < dim; k++) force[i*dim+k] += f[k];
    }



    for (i = 0; i < n; i++){
     /* normalize force */
      for (k = 0; k < dim; k++) f[k] = force[i*dim+k];
     
      F = 0.;
      for (k = 0; k < dim; k++) F += f[k]*f[k];
      F = sqrt(F);
      Fnorm += F;

      if (F > 0) for (k = 0; k < dim; k++) f[k] /= F;

      for (k = 0; k < dim; k++) x[i*dim+k] += step*f[k];

    }/* done vertex i */

    if (qt) {
      QuadTree_delete(qt);
      nsuper_avg /= n;
      counts_avg /= n;
#ifdef TIME
      qtree_cpu0 = qtree_cpu - qtree_cpu0;
      if (Verbose && 0) fprintf(stderr, "\n cpu this outer iter = %f, quadtree time = %f other time = %f\n",((real) (clock() - start2)) / CLOCKS_PER_SEC, qtree_cpu0,((real) (clock() - start2))/CLOCKS_PER_SEC - qtree_cpu0);
      qtree_cpu0 = qtree_cpu;
#endif
      if (Verbose && 0) fprintf(stderr, "nsuper_avg=%f, counts_avg = %f 2*nsuper+counts=%f\n",nsuper_avg,counts_avg, 2*nsuper_avg+counts_avg);
      oned_optimizer_train(qtree_level_optimizer, 5*nsuper_avg + counts_avg);
    }    

#ifdef ENERGY
    if (Verbose) {
        fprintf(stderr, "\r                iter = %d, step = %f Fnorm = %f nsuper = %d nz = %d  K = %f                                  ",iter, step, Fnorm, (int) nsuper_avg,A->nz,K);
        fprintf(stderr, "energy = %f\n",spring_electrical_energy(dim, A, x, p, CRK, KP));
    }
#endif


    step = update_step(adaptive_cooling, step, Fnorm, Fnorm0, cool);
  } while (step > tol && iter < maxiter);

#ifdef DEBUG_PRINT
  if (Verbose && 0) fputs("\n", stderr);
#endif

#ifdef DEBUG_PRINT_0
  {
    FILE *f;
    char fname[10000];
    strcpy(fname,"/tmp/graph_layout");
    sprintf(&(fname[strlen(fname)]), "%d",n);
    f = fopen(fname,"w");
    export_embedding(f, dim, A, x, NULL);
    fclose(f);
  }
#endif


#ifdef DEBUG_PRINT
    if (Verbose) {
      if (USE_QT){
	fprintf(stderr, "iter = %d, step = %f Fnorm = %f qt_level = %d nsuper = %d nz = %d  K = %f   ",iter, step, Fnorm, max_qtree_level, (int) nsuper_avg,A->nz,K);
      } else {
	fprintf(stderr, "iter = %d, step = %f Fnorm = %f nsuper = %d nz = %d  K = %f   ",iter, step, Fnorm, (int) nsuper_avg,A->nz,K);
      }
    }
#endif

  if (ctrl->beautify_leaves) beautify_leaves(dim, A, x);

#ifdef TIME
  total_cpu += ((real) (clock() - start0)) / CLOCKS_PER_SEC;
  if (Verbose) fprintf(stderr, "time for supernode = %f, total cpu = %f\n",qtree_cpu, total_cpu);
#endif

 RETURN:
  if (USE_QT) {
    oned_optimizer_delete(qtree_level_optimizer);
    ctrl->max_qtree_level = max_qtree_level;
  }
  if (xold) FREE(xold);
  if (A != A0) SparseMatrix_delete(A);
  if (f) FREE(f);
  if (center) FREE(center);
  if (supernode_wgts) FREE(supernode_wgts);
  if (distances) FREE(distances);
  FREE(force);

}



void spring_electrical_embedding(int dim, SparseMatrix A0, spring_electrical_control ctrl, real *node_weights, real *x, int *flag){
  /* x is a point to a 1D array, x[i*dim+j] gives the coordinate of the i-th node at dimension j.  */
  SparseMatrix A = A0;
  int m, n;
  int i, j, k;
  real p = ctrl->p, K = ctrl->K, C = ctrl->C, CRK, tol = ctrl->tol, maxiter = ctrl->maxiter, cool = ctrl->cool, step = ctrl->step, KP;
  int *ia = NULL, *ja = NULL;
  real *xold = NULL;
  real *f = NULL, dist, F, Fnorm = 0, Fnorm0;
  int iter = 0;
  int adaptive_cooling = ctrl->adaptive_cooling;
  QuadTree qt = NULL;
  int USE_QT = FALSE;
  int nsuper = 0, nsupermax = 10;
  real *center = NULL, *supernode_wgts = NULL, *distances = NULL, nsuper_avg, counts = 0, counts_avg = 0;
#ifdef TIME
  clock_t start, end, start0, start2;
  real qtree_cpu = 0, qtree_cpu0 = 0;
  real total_cpu = 0;
  start0 = clock();
#endif  
  int max_qtree_level = ctrl->max_qtree_level;
  oned_optimizer qtree_level_optimizer = NULL;

  if (!A) return;

  m = A->m, n = A->n;
  if (n <= 0 || dim <= 0) return;

  if (n >= ctrl->quadtree_size) {
    USE_QT = TRUE;
    qtree_level_optimizer = oned_optimizer_new(max_qtree_level);
    center = N_GNEW(nsupermax*dim,real);
    supernode_wgts = N_GNEW(nsupermax,real);
    distances = N_GNEW(nsupermax,real);
  }
  *flag = 0;
  if (m != n) {
    *flag = ERROR_NOT_SQUARE_MATRIX;
    goto RETURN;
  }
  assert(A->format == FORMAT_CSR);
  A = SparseMatrix_symmetrize(A, TRUE);
  ia = A->ia;
  ja = A->ja;

  K = initPos (A, n, dim, x, ctrl);

  if (C < 0) ctrl->C = C = 0.2;
  if (p >= 0) ctrl->p = p = -1;
  KP = pow(K, 1 - p);
  CRK = pow(C, (2.-p)/3.)/K;

#ifdef DEBUG_0
  {
    FILE *f;
    char fname[10000];
    strcpy(fname,"/tmp/graph_layout_0_");
    sprintf(&(fname[strlen(fname)]), "%d",n);
    f = fopen(fname,"w");
    export_embedding(f, dim, A, x, NULL);
    fclose(f);
  }
#endif

  f = N_GNEW(dim,real);
  xold = N_GNEW(dim*n,real); 
  do {
    iter++;
    xold = MEMCPY(xold, x, sizeof(real)*dim*n);
    Fnorm0 = Fnorm;
    Fnorm = 0.;
    nsuper_avg = 0;
    counts_avg = 0;

    if (USE_QT) {

      max_qtree_level = oned_optimizer_get(qtree_level_optimizer);
      if (ctrl->use_node_weights){
	qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, node_weights);
      } else {
	qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, NULL);
      }

	  
    }
#ifdef TIME
    start2 = clock();
#endif

    for (i = 0; i < n; i++){
      for (k = 0; k < dim; k++) f[k] = 0.;
      /* attractive force   C^((2-p)/3) ||x_i-x_j||/K * (x_j - x_i) */
      for (j = ia[i]; j < ia[i+1]; j++){
	if (ja[j] == i) continue;
	dist = distance(x, dim, i, ja[j]);
	for (k = 0; k < dim; k++){
	  f[k] -= CRK*(x[i*dim+k] - x[ja[j]*dim+k])*dist;
	}
      }

      /* repulsive force K^(1 - p)/||x_i-x_j||^(1 - p) (x_i - x_j) */
      if (USE_QT){
#ifdef TIME
	start = clock();
#endif
	QuadTree_get_supernodes(qt, ctrl->bh, &(x[dim*i]), i, &nsuper, &nsupermax, 
				&center, &supernode_wgts, &distances, &counts, flag);

#ifdef TIME
	end = clock();
	qtree_cpu += ((real) (end - start)) / CLOCKS_PER_SEC;
#endif
	counts_avg += counts;
	nsuper_avg += nsuper;
	if (*flag) goto RETURN;
	for (j = 0; j < nsuper; j++){
	  dist = MAX(distances[j], MINDIST);
	  for (k = 0; k < dim; k++){
	    if (p == -1){
	      f[k] += supernode_wgts[j]*KP*(x[i*dim+k] - center[j*dim+k])/(dist*dist);
	    } else {
	      f[k] += supernode_wgts[j]*KP*(x[i*dim+k] - center[j*dim+k])/pow(dist, 1.- p);
	    }
	  }
	}
      } else {
	if (ctrl->use_node_weights && node_weights){
	  for (j = 0; j < n; j++){
	    if (j == i) continue;
	    dist = distance_cropped(x, dim, i, j);
	    for (k = 0; k < dim; k++){
	      if (p == -1){
		f[k] += node_weights[j]*KP*(x[i*dim+k] - x[j*dim+k])/(dist*dist);
	      } else {
		f[k] += node_weights[j]*KP*(x[i*dim+k] - x[j*dim+k])/pow(dist, 1.- p);
	      }
	    }
	  }
	} else {
	  for (j = 0; j < n; j++){
	    if (j == i) continue;
	    dist = distance_cropped(x, dim, i, j);
	    for (k = 0; k < dim; k++){
	      if (p == -1){
		f[k] += KP*(x[i*dim+k] - x[j*dim+k])/(dist*dist);
	      } else {
		f[k] += KP*(x[i*dim+k] - x[j*dim+k])/pow(dist, 1.- p);
	      }
	    }
	  }
	}
      }
	
      /* normalize force */
      F = 0.;
      for (k = 0; k < dim; k++) F += f[k]*f[k];
      F = sqrt(F);
      Fnorm += F;

      if (F > 0) for (k = 0; k < dim; k++) f[k] /= F;

      for (k = 0; k < dim; k++) x[i*dim+k] += step*f[k];

    }/* done vertex i */

    if (qt) {
      QuadTree_delete(qt);
      nsuper_avg /= n;
      counts_avg /= n;
#ifdef TIME
      qtree_cpu0 = qtree_cpu - qtree_cpu0;
      if (Verbose && 0) fprintf(stderr, "\n cpu this outer iter = %f, quadtree time = %f other time = %f\n",((real) (clock() - start2)) / CLOCKS_PER_SEC, qtree_cpu0,((real) (clock() - start2))/CLOCKS_PER_SEC - qtree_cpu0);
      qtree_cpu0 = qtree_cpu;
#endif
      if (Verbose & 0) fprintf(stderr, "nsuper_avg=%f, counts_avg = %f 2*nsuper+counts=%f\n",nsuper_avg,counts_avg, 2*nsuper_avg+counts_avg);
      oned_optimizer_train(qtree_level_optimizer, 5*nsuper_avg + counts_avg);
    }    

#ifdef ENERGY
    if (Verbose) {
        fprintf(stderr, "\r                iter = %d, step = %f Fnorm = %f nsuper = %d nz = %d  K = %f                                  ",iter, step, Fnorm, (int) nsuper_avg,A->nz,K);
        fprintf(stderr, "energy = %f\n",spring_electrical_energy(dim, A, x, p, CRK, KP));
    }
#endif


    step = update_step(adaptive_cooling, step, Fnorm, Fnorm0, cool);
  } while (step > tol && iter < maxiter);

#ifdef DEBUG_PRINT
  if (Verbose && 0) fputs("\n", stderr);
#endif

#ifdef DEBUG_PRINT_0
  {
    FILE *f;
    char fname[10000];
    strcpy(fname,"/tmp/graph_layout");
    sprintf(&(fname[strlen(fname)]), "%d",n);
    f = fopen(fname,"w");
    export_embedding(f, dim, A, x, NULL);
    fclose(f);
  }
#endif


#ifdef DEBUG_PRINT
    if (Verbose) {
      if (USE_QT){
	fprintf(stderr, "iter = %d, step = %f Fnorm = %f qt_level = %d nsuper = %d nz = %d  K = %f   ",iter, step, Fnorm, max_qtree_level, (int) nsuper_avg,A->nz,K);
      } else {
	fprintf(stderr, "iter = %d, step = %f Fnorm = %f nsuper = %d nz = %d  K = %f   ",iter, step, Fnorm, (int) nsuper_avg,A->nz,K);
      }
    }
#endif

  if (ctrl->beautify_leaves) beautify_leaves(dim, A, x);

#ifdef TIME
  total_cpu += ((real) (clock() - start0)) / CLOCKS_PER_SEC;
  if (Verbose) fprintf(stderr, "time for supernode = %f, total cpu = %f\n",qtree_cpu, total_cpu);
#endif

 RETURN:
  if (USE_QT) {
    oned_optimizer_delete(qtree_level_optimizer);
    ctrl->max_qtree_level = max_qtree_level;
  }
  if (xold) FREE(xold);
  if (A != A0) SparseMatrix_delete(A);
  if (f) FREE(f);
  if (center) FREE(center);
  if (supernode_wgts) FREE(supernode_wgts);
  if (distances) FREE(distances);

}




void spring_electrical_spring_embedding(int dim, SparseMatrix A0, SparseMatrix D, spring_electrical_control ctrl, real *node_weights, real *x, int *flag){
  /* x is a point to a 1D array, x[i*dim+j] gives the coordinate of the i-th node at dimension j. Same as the spring-electrical except we also 
     introduce force due to spring length
   */
  SparseMatrix A = A0;
  int m, n;
  int i, j, k;
  real p = ctrl->p, K = ctrl->K, C = ctrl->C, CRK, tol = ctrl->tol, maxiter = ctrl->maxiter, cool = ctrl->cool, step = ctrl->step, KP;
  int *ia = NULL, *ja = NULL;
  int *id = NULL, *jd = NULL;
  real *d;
  real *xold = NULL;
  real *f = NULL, dist, F, Fnorm = 0, Fnorm0;
  int iter = 0;
  int adaptive_cooling = ctrl->adaptive_cooling;
  QuadTree qt = NULL;
  int USE_QT = FALSE;
  int nsuper = 0, nsupermax = 10;
  real *center = NULL, *supernode_wgts = NULL, *distances = NULL, nsuper_avg, counts = 0;
  int max_qtree_level = 10;

  if (!A) return;
  m = A->m, n = A->n;
  if (n <= 0 || dim <= 0) return;

  if (n >= ctrl->quadtree_size) {
    USE_QT = TRUE;
    center = N_GNEW(nsupermax*dim,real);
    supernode_wgts = N_GNEW(nsupermax,real);
    distances = N_GNEW(nsupermax,real);
  }
  *flag = 0;
  if (m != n) {
    *flag = ERROR_NOT_SQUARE_MATRIX;
    goto RETURN;
  }
  assert(A->format == FORMAT_CSR);
  A = SparseMatrix_symmetrize(A, TRUE);
  ia = A->ia;
  ja = A->ja;
  id = D->ia;
  jd = D->ja;
  d = (real*) D->a;

  K = initPos (A, n, dim, x, ctrl);

  if (C < 0) ctrl->C = C = 0.2;
  if (p >= 0) ctrl->p = p = -1;
  KP = pow(K, 1 - p);
  CRK = pow(C, (2.-p)/3.)/K;

#ifdef DEBUG_0
  {
    FILE *f;
    char fname[10000];
    strcpy(fname,"/tmp/graph_layout_0_");
    sprintf(&(fname[strlen(fname)]), "%d",n);
    f = fopen(fname,"w");
    export_embedding(f, dim, A, x, NULL);
    fclose(f);
  }
#endif

  f = N_GNEW(dim,real);
  xold = N_GNEW(dim*n,real); 
  do {
    iter++;
    xold = MEMCPY(xold, x, sizeof(real)*dim*n);
    Fnorm0 = Fnorm;
    Fnorm = 0.;
    nsuper_avg =- 0;

    if (USE_QT) {
      if (ctrl->use_node_weights){
	qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, node_weights);
      } else {
	qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, NULL);
      }
    }

    for (i = 0; i < n; i++){
      for (k = 0; k < dim; k++) f[k] = 0.;
      /* attractive force   C^((2-p)/3) ||x_i-x_j||/K * (x_j - x_i) */

      for (j = ia[i]; j < ia[i+1]; j++){
	if (ja[j] == i) continue;
	dist = distance(x, dim, i, ja[j]);
	for (k = 0; k < dim; k++){
	  f[k] -= CRK*(x[i*dim+k] - x[ja[j]*dim+k])*dist;
	}
      }

      for (j = id[i]; j < id[i+1]; j++){
	if (jd[j] == i) continue;
	dist = distance_cropped(x, dim, i, jd[j]);
	for (k = 0; k < dim; k++){
	  if (dist < d[j]){
	    f[k] += 0.2*CRK*(x[i*dim+k] - x[jd[j]*dim+k])*(dist - d[j])*(dist - d[j])/dist;
	  } else {
	    f[k] -= 0.2*CRK*(x[i*dim+k] - x[jd[j]*dim+k])*(dist - d[j])*(dist - d[j])/dist;
	  }
	  /* f[k] -= 0.2*CRK*(x[i*dim+k] - x[jd[j]*dim+k])*(dist - d[j]);*/
	}
      }

      /* repulsive force K^(1 - p)/||x_i-x_j||^(1 - p) (x_i - x_j) */
      if (USE_QT){
	QuadTree_get_supernodes(qt, ctrl->bh, &(x[dim*i]), i, &nsuper, &nsupermax, 
				&center, &supernode_wgts, &distances, &counts, flag);
	nsuper_avg += nsuper;
	if (*flag) goto RETURN;
	for (j = 0; j < nsuper; j++){
	  dist = MAX(distances[j], MINDIST);
	  for (k = 0; k < dim; k++){
	    if (p == -1){
	      f[k] += supernode_wgts[j]*KP*(x[i*dim+k] - center[j*dim+k])/(dist*dist);
	    } else {
	      f[k] += supernode_wgts[j]*KP*(x[i*dim+k] - center[j*dim+k])/pow(dist, 1.- p);
	    }
	  }
	}
      } else {
	if (ctrl->use_node_weights && node_weights){
	  for (j = 0; j < n; j++){
	    if (j == i) continue;
	    dist = distance_cropped(x, dim, i, j);
	    for (k = 0; k < dim; k++){
	      if (p == -1){
		f[k] += node_weights[j]*KP*(x[i*dim+k] - x[j*dim+k])/(dist*dist);
	      } else {
		f[k] += node_weights[j]*KP*(x[i*dim+k] - x[j*dim+k])/pow(dist, 1.- p);
	      }
	    }
	  }
	} else {
	  for (j = 0; j < n; j++){
	    if (j == i) continue;
	    dist = distance_cropped(x, dim, i, j);
	    for (k = 0; k < dim; k++){
	      if (p == -1){
		f[k] += KP*(x[i*dim+k] - x[j*dim+k])/(dist*dist);
	      } else {
		f[k] += KP*(x[i*dim+k] - x[j*dim+k])/pow(dist, 1.- p);
	      }
	    }
	  }
	}
      }
	
      /* normalize force */
      F = 0.;
      for (k = 0; k < dim; k++) F += f[k]*f[k];
      F = sqrt(F);
      Fnorm += F;

      if (F > 0) for (k = 0; k < dim; k++) f[k] /= F;

      for (k = 0; k < dim; k++) x[i*dim+k] += step*f[k];

    }/* done vertex i */

    if (qt) QuadTree_delete(qt);
    nsuper_avg /= n;
#ifdef DEBUG_PRINT
    if (Verbose && 0) {
        fprintf(stderr, "\r                iter = %d, step = %f Fnorm = %f nsuper = %d nz = %d  K = %f                                  ",iter, step, Fnorm, (int) nsuper_avg,A->nz,K);
#ifdef ENERGY
        fprintf(stderr, "energy = %f\n",spring_electrical_energy(dim, A, x, p, CRK, KP));
#endif
    }
#endif
    
    step = update_step(adaptive_cooling, step, Fnorm, Fnorm0, cool);
  } while (step > tol && iter < maxiter);

#ifdef DEBUG_PRINT
  if (Verbose && 0) fputs("\n", stderr);
#endif

#ifdef DEBUG_PRINT_0
  {
    FILE *f;
    char fname[10000];
    strcpy(fname,"/tmp/graph_layout");
    sprintf(&(fname[strlen(fname)]), "%d",n);
    f = fopen(fname,"w");
    export_embedding(f, dim, A, x, NULL);
    fclose(f);
  }
#endif

  if (ctrl->beautify_leaves) beautify_leaves(dim, A, x);

 RETURN:
  if (xold) FREE(xold);
  if (A != A0) SparseMatrix_delete(A);
  if (f) FREE(f);
  if (center) FREE(center);
  if (supernode_wgts) FREE(supernode_wgts);
  if (distances) FREE(distances);

}




void print_matrix(real *x, int n, int dim){
  int i, k;
  printf("{");
  for (i = 0; i < n; i++){
    if (i != 0) printf(",");
    printf("{");
    for (k = 0; k < dim; k++) {
      if (k != 0) printf(",");
      printf("%f",x[i*dim+k]);
    }
    printf("}");
  }
  printf("}\n");
}

/*
static void interpolate2(int dim, SparseMatrix A, real *x){
  int i, j, k, *ia = A->ia, *ja = A->ja, nz, m = A->m;
  real alpha = 0.5, beta, *y;

  y = N_GNEW(dim*m,real);
  for (k = 0; k < dim*m; k++) y[k] = 0;
  for (i = 0; i < m; i++){
    nz = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (ja[j] == i) continue;
      nz++;
      for (k = 0; k < dim; k++){
	y[i*dim+k] += x[ja[j]*dim + k];
      }
    }
    if (nz > 0){
      beta = (1-alpha)/nz;
      for (k = 0; k < dim; k++) y[i*dim+k] = alpha*x[i*dim+k] +  beta*y[i*dim+k];
    }
  }
  for (k = 0; k < dim*m; k++) x[k] = y[k];

  FREE(y);
}

*/

static void interpolate0(int dim, SparseMatrix A, real *x){
  int i, j, k, *ia = A->ia, *ja = A->ja, nz;
  real alpha = 0.5, beta, *y;

  y = N_GNEW(dim,real);
  for (i = 0; i < A->m; i++){
    for (k = 0; k < dim; k++) y[k] = 0;
    nz = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (ja[j] == i) continue;
      nz++;
      for (k = 0; k < dim; k++){
	y[k] += x[ja[j]*dim + k];
      }
    }
    if (nz > 0){
      beta = (1-alpha)/nz;
      for (k = 0; k < dim; k++) x[i*dim+k] = alpha*x[i*dim+k] +  beta*y[k];
    }
  }

  FREE(y);
}
static void prolongate(int dim, SparseMatrix A, SparseMatrix P, SparseMatrix R, real *x, real *y, int coarsen_scheme_used, real delta){
  int nc, *ia, *ja, i, j, k;
  SparseMatrix_multiply_dense(P, FALSE, x, FALSE, &y, FALSE, dim);

  /* xu yao rao dong */
  if (coarsen_scheme_used > EDGE_BASED_STA && coarsen_scheme_used < EDGE_BASED_STO){
    interpolate0(dim, A, y);
    nc = R->m;
    ia = R->ia;
    ja = R->ja;
    for (i = 0; i < nc; i++){
      for (j = ia[i]+1; j < ia[i+1]; j++){
	for (k = 0; k < dim; k++){
	  y[ja[j]*dim + k] += delta*(drand() - 0.5);
	}
      }
    }
  }
}



int power_law_graph(SparseMatrix A){
  int *mask, m, max = 0, i, *ia = A->ia, *ja = A->ja, j, deg;
  int res = FALSE;
  m = A->m;
  mask = N_GNEW((m+1),int);

  for (i = 0; i < m + 1; i++){
    mask[i] = 0;
  }

  for (i = 0; i < m; i++){
    deg = 0;
    for (j = ia[i]; j < ia[i+1]; j++){
      if (i == ja[j]) continue;
      deg++;
    }
    mask[deg]++;
    max = MAX(max, mask[deg]);
  }
  if (mask[1] > 0.8*max && mask[1] > 0.3*m) res = TRUE;
  FREE(mask);
  return res;
}

void pcp_rotate(int n, int dim, real *x){
  int i, k,l;
  real y[4], axis[2], center[2], dist, x0, x1;

  assert(dim == 2);
  for (i = 0; i < dim*dim; i++) y[i] = 0;
  for (i = 0; i < dim; i++) center[i] = 0;
  for (i = 0; i < n; i++){
    for (k = 0; k < dim; k++){
      center[k] += x[i*dim+k];
    }
  }
  for (i = 0; i < dim; i++) center[i] /= n;
  for (i = 0; i < n; i++){
    for (k = 0; k < dim; k++){
      x[dim*i+k] =  x[dim*i+k] - center[k];
    }
  }

  for (i = 0; i < n; i++){
    for (k = 0; k < dim; k++){
      for (l = 0; l < dim; l++){
	y[dim*k+l] += x[i*dim+k]*x[i*dim+l];
      }
    }
  }
  if (y[1] == 0) {
    axis[0] = 0; axis[1] = 1;
  } else {
    /*         Eigensystem[{{x0, x1}, {x1, x3}}] = 
	       {{(x0 + x3 - Sqrt[x0^2 + 4*x1^2 - 2*x0*x3 + x3^2])/2, 
	       (x0 + x3 + Sqrt[x0^2 + 4*x1^2 - 2*x0*x3 + x3^2])/2}, 
	       {{-(-x0 + x3 + Sqrt[x0^2 + 4*x1^2 - 2*x0*x3 + x3^2])/(2*x1), 1}, 
	       {-(-x0 + x3 - Sqrt[x0^2 + 4*x1^2 - 2*x0*x3 + x3^2])/(2*x1), 1}}}
    */
    axis[0] = -(-y[0] + y[3] - sqrt(y[0]*y[0]+4*y[1]*y[1]-2*y[0]*y[3]+y[3]*y[3]))/(2*y[1]);
    axis[1] = 1;
  }
  dist = sqrt(1+axis[0]*axis[0]);
  axis[0] = axis[0]/dist;
  axis[1] = axis[1]/dist;
  for (i = 0; i < n; i++){
    x0 = x[dim*i]*axis[0]+x[dim*i+1]*axis[1];
    x1 = -x[dim*i]*axis[1]+x[dim*i+1]*axis[0];
    x[dim*i] = x0;
    x[dim*i + 1] = x1;

  }


}


void multilevel_spring_electrical_embedding(int dim, SparseMatrix A0, spring_electrical_control ctrl, real *node_weights, real *label_sizes, 
					    real *x, int *flag){
  

  Multilevel_control mctrl = NULL;
  int n, plg, coarsen_scheme_used;
  SparseMatrix A = A0, P = NULL;
  Multilevel grid, grid0;
  real *xc = NULL, *xf = NULL;
#ifdef TIME
  clock_t  cpu;
#endif
  struct spring_electrical_control_struct ctrl0;

  ctrl0 = *ctrl;

#ifdef TIME
  cpu = clock();
#endif

  *flag = 0;
  if (!A) return;
  n = A->n;
  if (n <= 0 || dim <= 0) return;

  if (!SparseMatrix_is_symmetric(A, FALSE) || A->type != MATRIX_TYPE_REAL){
    A = SparseMatrix_get_real_adjacency_matrix_symmetrized(A);
  } else {
    A = SparseMatrix_remove_diagonal(A);
  }

  mctrl = Multilevel_control_new();
  mctrl->maxlevel = ctrl->multilevels;
  grid0 = Multilevel_new(A, node_weights, mctrl);

  grid = Multilevel_get_coarsest(grid0);
  if (Multilevel_is_finest(grid)){
    xc = x;
  } else {
    xc = N_GNEW(grid->n*dim,real);
  }

  plg = power_law_graph(A);
  if (ctrl->p == AUTOP){
    ctrl->p = -1;
    if (plg) ctrl->p = -1.8;
  }
  
  do {
#ifdef DEBUG_PRINT
    if (Verbose) {
      print_padding(grid->level);
      if (Multilevel_is_coarsest(grid)){
	fprintf(stderr, "coarsest level -- %d, n = %d\n", grid->level, grid->n);
      } else {
	fprintf(stderr, "level -- %d, n = %d\n", grid->level, grid->n);
      }
    }
#endif
    if (ctrl->tscheme == QUAD_TREE_NONE){
      spring_electrical_embedding_slow(dim, grid->A, ctrl, grid->node_weights, xc, flag);
    } else if (ctrl->tscheme == QUAD_TREE_NORMAL){
      spring_electrical_embedding(dim, grid->A, ctrl, grid->node_weights, xc, flag);
    } else if (ctrl->tscheme == QUAD_TREE_FAST){
      spring_electrical_embedding_fast(dim, grid->A, ctrl, grid->node_weights, xc, flag);
    } else {
      assert(0);
    }
    if (Multilevel_is_finest(grid)) break;
    if (*flag) {
      FREE(xc);
      goto RETURN;
    }
    P = grid->P;
    coarsen_scheme_used = grid->coarsen_scheme_used;
    grid = grid->prev;
    if (Multilevel_is_finest(grid)){
      xf = x;
    } else {
      xf = N_GNEW(grid->n*dim,real);
    }
    prolongate(dim, grid->A, P, grid->R, xc, xf, coarsen_scheme_used, (ctrl->K)*0.001);
    FREE(xc);
    xc = xf;
    ctrl->random_start = FALSE;
    ctrl->K = ctrl->K * 0.75;
    ctrl->adaptive_cooling = FALSE;
    if (grid->next->coarsen_scheme_used > VERTEX_BASED_STA &&
	grid->next->coarsen_scheme_used < VERTEX_BASED_STO){
      ctrl->step = 1;
    } else {
      ctrl->step = .1;
    }
  } while (grid);

#ifdef TIME
  if (Verbose)
    fprintf(stderr, "layout time %f\n",((real) (clock() - cpu)) / CLOCKS_PER_SEC);
  cpu = clock();
#endif

  post_process_smoothing(dim, A, ctrl, node_weights, x, flag);

  if (dim == 2){
    pcp_rotate(n, dim, x);
  }

  if (Verbose) 
    fprintf(stderr, "sfdp: overlap=%d scaling %.02f\n",
      ctrl->overlap, ctrl->initial_scaling);

  if (ctrl->overlap >= 0)
    remove_overlap(dim, A, A->m, x, label_sizes, ctrl->overlap, ctrl->initial_scaling, flag);

 RETURN:
  *ctrl = ctrl0;
  if (A != A0) SparseMatrix_delete(A);
  Multilevel_control_delete(mctrl);
  Multilevel_delete(grid0);
}

