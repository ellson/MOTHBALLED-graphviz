/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "SparseMatrix.h"
#include "overlap.h"
#include "call_tri.h"
#include "red_black_tree.h"
#include "types.h"
#include "memory.h"
#include "globals.h"
#include <time.h>

#define MACHINEACC 1.0e-16

static void ideal_distance_avoid_overlap(int dim, SparseMatrix  A,
					 real * x, real * width,
					 real * ideal_distance,
					 real * tmax, real * tmin)
{
    /*  if (x1>x2 && y1 > y2) we want either x1 + t (x1-x2) - x2 > (width1+width2), or y1 + t (y1-y2) - y2 > (height1+height2),
       hence t = MAX(expandmin, MIN(expandmax, (width1+width2)/(x1-x2) - 1, (height1+height2)/(y1-y2) - 1)), and
       new ideal distance = (1+t) old_distance. t can be negative sometimes.
       The result ideal distance is set to negative if the edge needs shrinking
     */
    int i, j, jj;
    int *ia = A->ia, *ja = A->ja;
    real dist, dx, dy, wx, wy, t;
    real expandmax = 1.5, expandmin = 1;

    *tmax = 0;
    *tmin = 1.e10;
    assert(SparseMatrix_is_symmetric(A, FALSE));
    for (i = 0; i < A->m; i++) {
	for (j = ia[i]; j < ia[i + 1]; j++) {
	    jj = ja[j];
	    if (jj == i)
		continue;
	    dist = distance(x, dim, i, jj);
	    dx = ABS(x[i * dim] - x[jj * dim]);
	    dy = ABS(x[i * dim + 1] - x[jj * dim + 1]);
	    wx = width[i * dim] + width[jj * dim];
	    wy = width[i * dim + 1] + width[jj * dim + 1];
	    if (dx < MACHINEACC * wx && dy < MACHINEACC * wy) {
		ideal_distance[j] = sqrt(wx * wx + wy * wy);
		*tmax = 2;
	    } else {
		if (dx < MACHINEACC * wx) {
		    t = wy / dy;
		} else if (dy < MACHINEACC * wy) {
		    t = wx / dx;
		} else {
		    t = MIN(wx / dx, wy / dy);
		}
		if (t > 1)
		    t = MAX(t, 1.001);	/* no point in things like t = 1.00000001 as this slow down convergence */
		*tmax = MAX(*tmax, t);
		*tmin = MIN(*tmin, t);
		t = MIN(expandmax, t);
		t = MAX(expandmin, t);
		if (t > 1) {
		    ideal_distance[j] = t * dist;
		} else {
		    ideal_distance[j] = -t * dist;
		}
	    }

	}
    }
    return;
}

#define collide(i,j) ((ABS(x[(i)*dim] - x[(j)*dim]) < width[(i)*dim]+width[(j)*dim]) || (ABS(x[(i)*dim+1] - x[(j)*dim+1]) < width[(i)*dim+1]+width[(j)*dim+1]))

enum { INTV_OPEN, INTV_CLOSE };

struct scan_point_struct {
    int node;
    real x;
    int status;
};

typedef struct scan_point_struct scan_point;


static int comp_scan_points(const void *p, const void *q)
{
    scan_point *pp = (scan_point *) p;
    scan_point *qq = (scan_point *) q;
    if (pp->x > qq->x) {
	return 1;
    } else if (pp->x < qq->x) {
	return -1;
    } else {
	if (pp->node > qq->node) {
	    return 1;
	} else if (pp->node < qq->node) {
	    return -1;
	}
	return 0;
    }
    return 0;
}


void NodeDest(void *a)
{
    /*  free((int*)a); */
}



int NodeComp(const void *a, const void *b)
{
    return comp_scan_points(a, b);

}

void NodePrint(const void *a)
{
    scan_point *aa;

    aa = (scan_point *) a;
    fprintf(stderr, "node {%d, %f, %d}\n", aa->node, aa->x, aa->status);

}

void InfoPrint(void *a)
{
    ;
}

void InfoDest(void *a)
{
    ;
}

static SparseMatrix get_overlap_graph(int dim, int n, real * x,
				       real * width)
{
    scan_point *scanpointsx, *scanpointsy;
    int i, k, neighbor;
    SparseMatrix A = NULL;
    SparseMatrix B = NULL;
    rb_red_blk_node *newNode, *newNode0;
    rb_red_blk_tree *treey;
    real one = 1;

    A = SparseMatrix_new(n, n, 1, MATRIX_TYPE_REAL, FORMAT_COORD);

    scanpointsx = N_GNEW(2 * n, scan_point);
    for (i = 0; i < n; i++) {
	scanpointsx[2 * i].node = i;
	scanpointsx[2 * i].x = x[i * dim] - width[i * dim];
	scanpointsx[2 * i].status = INTV_OPEN;
	scanpointsx[2 * i + 1].node = i + n;
	scanpointsx[2 * i + 1].x = x[i * dim] + width[i * dim];
	scanpointsx[2 * i + 1].status = INTV_CLOSE;
    }
    qsort(scanpointsx, 2 * n, sizeof(scan_point), comp_scan_points);

    scanpointsy = N_GNEW(2 * n, scan_point);
    for (i = 0; i < n; i++) {
	scanpointsy[i].node = i;
	scanpointsy[i].x = x[i * dim + 1] - width[i * dim + 1];
	scanpointsy[i].status = INTV_OPEN;
	scanpointsy[i + n].node = i;
	scanpointsy[i + n].x = x[i * dim + 1] + width[i * dim + 1];
	scanpointsy[i + n].status = INTV_CLOSE;
    }


    treey =
	RBTreeCreate(NodeComp, NodeDest, InfoDest, NodePrint, InfoPrint);

    for (i = 0; i < 2 * n; i++) {
#ifdef DEBUG_RBTREE
	fprintf(stderr, " k = %d node = %d x====%f\n",
		(scanpointsx[i].node) % n, (scanpointsx[i].node),
		(scanpointsx[i].x));
#endif

	k = (scanpointsx[i].node) % n;


	if (scanpointsx[i].status == INTV_OPEN) {
#ifdef DEBUG_RBTREE
	    fprintf(stderr, "inserting...");
	    treey->PrintKey(&(scanpointsy[k]));
#endif

	    RBTreeInsert(treey, &(scanpointsy[k]), NULL);	/* add both open and close int for y */

#ifdef DEBUG_RBTREE
	    fprintf(stderr, "inserting2...");
	    treey->PrintKey(&(scanpointsy[k + n]));
#endif

	    RBTreeInsert(treey, &(scanpointsy[k + n]), NULL);
	} else {
	    assert(scanpointsx[i].node >= n);

	    newNode = newNode0 =
		RBExactQuery(treey, &(scanpointsy[k + n]));

#ifdef DEBUG_RBTREE
	    fprintf(stderr, "poping..%d....", scanpointsy[k + n].node);
	    treey->PrintKey(newNode->key);
#endif

	    assert(treey->nil != newNode);
	    while (((newNode =
		     TreePredecessor(treey, newNode)) != treey->nil)
		   && ((scan_point *) newNode->key)->node != k) {
		neighbor = (((scan_point *) newNode->key)->node) % n;
		A = SparseMatrix_coordinate_form_add_entries(A, 1,
							     &neighbor, &k,
							     &one);
#ifdef DEBUG_RBTREE
		fprintf(stderr, "%d %d\n", k, neighbor);
#endif

	    }

#ifdef DEBUG_RBTREE
	    fprintf(stderr, "deleting...");
	    treey->PrintKey(newNode0->key);
#endif

	    RBDelete(treey, newNode0);
	    if (newNode != treey->nil && newNode != newNode0) {

#ifdef DEBUG_RBTREE
		fprintf(stderr, "deleting2...");
		treey->PrintKey(newNode->key)
#endif
		    RBDelete(treey, newNode);
	    }
	}
    }

    free(scanpointsx);
    free(scanpointsy);
    RBTreeDestroy(treey);

    B = SparseMatrix_from_coordinate_format(A);
    SparseMatrix_delete(A);
    A = SparseMatrix_symmetrize(B, FALSE);
    SparseMatrix_delete(B);
    if (Verbose)
	fprintf(stderr, "found %d clashes\n", A->nz);
    return A;
}



/* ============================== label overlap smoother ==================*/

#define OverlapSmoother_s StressMajorizationSmoother_s

OverlapSmoother OverlapSmoother_new(SparseMatrix  A, int dim,
				     real lambda0, real * x, real * width,
				     int include_original_graph,
				     int neighborhood_only,
				     real * max_overlap,
				     real * min_overlap)
{
    OverlapSmoother sm;
    int i, j, k, m = A->m, *iw, *jw, *id, *jd, jdiag;
    SparseMatrix B;
    real *lambda, *d, *w, diag_d, diag_w, dist;

    assert((!A) || SparseMatrix_is_symmetric(A, FALSE));

    sm = GNEW(struct OverlapSmoother_s);
    lambda = sm->lambda = N_GNEW(m, real);
    for (i = 0; i < m; i++)
	sm->lambda[i] = lambda0;

    B = call_tri(m, dim, x);

    if (!neighborhood_only) {
	SparseMatrix C;
	SparseMatrix D;
	C = get_overlap_graph(dim, A->m, x, width);
	D = SparseMatrix_add(B, C);
	SparseMatrix_delete(B);
	SparseMatrix_delete(C);
	B = D;
    }
    if (include_original_graph) {
	sm->Lw = SparseMatrix_add(A, B);
	SparseMatrix_delete(B);
    } else {
	sm->Lw = B;
    }
    sm->Lwd = SparseMatrix_copy(sm->Lw);

#ifdef DEBUG
    {
	FILE *fp;
	fp = fopen("/tmp/111", "w");
	export_embedding(fp, dim, sm->Lwd, x, NULL);
	fclose(fp);
    }
#endif

    if (!(sm->Lw) || !(sm->Lwd)) {
	OverlapSmoother_delete(sm);
	return NULL;
    }

    assert((sm->Lwd)->type == MATRIX_TYPE_REAL);

    ideal_distance_avoid_overlap(dim, sm->Lwd, x, width,
				 (real *) (sm->Lwd->a), max_overlap,
				 min_overlap);

    /* no overlap at all! */
    if (*max_overlap < 1) {
	if (Verbose)
	    fprintf(stderr,
		    " no overlap (overlap = %f), rescale to shrink\n",
		    *max_overlap - 1);
	for (i = 0; i < dim * m; i++) {
	    x[i] *= (*max_overlap);
	}
	*max_overlap = 1;
	goto RETURN;
    }

    iw = sm->Lw->ia;
    jw = sm->Lw->ja;
    id = sm->Lwd->ia;
    jd = sm->Lwd->ja;
    w = (real *) sm->Lw->a;
    d = (real *) sm->Lwd->a;

    for (i = 0; i < m; i++) {
	diag_d = diag_w = 0;
	jdiag = -1;
	for (j = iw[i]; j < iw[i + 1]; j++) {
	    k = jw[j];
	    if (k == i) {
		jdiag = j;
		continue;
	    }
	    if (d[j] > 0) {	/* those edges that needs expansion */
		w[j] = 100 / d[j] / d[j];
	    } else {		/* those that needs shrinking is set to negative in ideal_distance_avoid_overlap */
		w[j] = 1 / d[j] / d[j];
		d[j] = -d[j];
	    }
	    dist = d[j];
	    diag_w += w[j];
	    d[j] = w[j] * dist;
	    diag_d += d[j];

	}

	lambda[i] *= (-diag_w);	/* alternatively don't do that then we have a constant penalty term scaled by lambda0 */

	assert(jdiag >= 0);
	w[jdiag] = -diag_w + lambda[i];
	d[jdiag] = -diag_d;
    }
  RETURN:
    return sm;
}

void OverlapSmoother_delete(OverlapSmoother  sm)
{

    StressMajorizationSmoother_delete(sm);

}

void OverlapSmoother_smooth(OverlapSmoother  sm, int dim, real * x)
{
    int maxit_sm = 1;		/* only using 1 iteration of stress majorization 
				   is found to give better results and save time! */
    StressMajorizationSmoother_smooth(sm, dim, x, maxit_sm);
#ifdef DEBUG
    {
	FILE *fp;
	fp = fopen("/tmp/222", "w");
	export_embedding(fp, dim, sm->Lwd, x, NULL);
	fclose(fp);
    }
#endif
}

/*================================= end OverlapSmoother =============*/

static void scale_to_edge_length(int dim, SparseMatrix  A, real * x,
				 real avg_label_size)
{
    real dist;
    int i;

    if (!A)
	return;
    dist = average_edge_length(A, dim, x);
    if (Verbose)
	fprintf(stderr, "avg edge len=%f avg_label-size= %f\n", dist,
		avg_label_size);


    dist = avg_label_size / MAX(dist, MACHINEACC);

    for (i = 0; i < dim * A->m; i++)
	x[i] *= dist;
}

static void print_bounding_box(int n, int dim, real * x)
{
    real *xmin, *xmax;
    int i, k;

    xmin = N_GNEW(dim, real);
    xmax = N_GNEW(dim, real);

    for (i = 0; i < dim; i++)
	xmin[i] = xmax[i] = x[i];

    for (i = 0; i < n; i++) {
	for (k = 0; k < dim; k++) {
	    xmin[k] = MIN(xmin[k], x[i * dim + k]);
	    xmax[k] = MAX(xmax[k], x[i * dim + k]);
	}
    }
    fprintf(stderr, "bounding box = \n");
    for (i = 0; i < dim; i++)
	fprintf(stderr, "{%f,%f}, ", xmin[i], xmax[i]);
    fprintf(stderr, "\n");

    free(xmin);
    free(xmax);
}

int remove_overlap(int dim, SparseMatrix  A, real * x,
		    real * label_sizes, int ntry)
{
    real lambda = 0.00;
    OverlapSmoother sm;
    int include_original_graph = 0, i;
    real avg_label_size;
    real max_overlap = 0, min_overlap = 999;
    int neighborhood_only = TRUE;
    int flag = 0;

#ifdef DEBUG
    clock_t cpu = clock();
#endif

    if (!label_sizes)
	return flag;

    avg_label_size = 0;
    for (i = 0; i < A->m; i++)
	avg_label_size += label_sizes[i * dim] + label_sizes[i * dim + 1];
    /*  for (i = 0; i < A->m; i++) avg_label_size += 2*MAX(label_sizes[i*dim],label_sizes[i*dim+1]); */
    avg_label_size /= A->m;

    scale_to_edge_length(dim, A, x, 4 * avg_label_size);

    if (!ntry)
	return flag;


#ifdef DEBUG
    _statistics[0] = _statistics[1] = 0.;
    {
	FILE *fp;
	fp = fopen("x1", "w");
	for (i = 0; i < A->m; i++) {
	    fprintf(fp, "%f %f\n", x[i * 2], x[i * 2 + 1]);
	}
	fclose(fp);
    }
#endif

    for (i = 0; i < ntry; i++) {
	if (Verbose)
	    print_bounding_box(A->m, dim, x);
	sm = OverlapSmoother_new(A, dim, lambda, x, label_sizes,
				 include_original_graph, neighborhood_only,
				 &max_overlap, &min_overlap);
	if (Verbose)
	    fprintf(stderr,
		    "overlap removal neighbors only?= %d iter -- %d, overlap factor = %g underlap factor = %g\n",
		    neighborhood_only, i, max_overlap - 1, min_overlap);
	if (max_overlap <= 1) {
	    OverlapSmoother_delete(sm);
	    if (neighborhood_only == FALSE) {
		break;
	    } else {
		neighborhood_only = FALSE;
		continue;
	    }
	}

	OverlapSmoother_smooth(sm, dim, x);
	OverlapSmoother_delete(sm);
    }

#ifdef DEBUG
    fprintf(stderr,
	    " number of cg iter = %f, number of stress majorization iter = %f number of overlap removal try = %d\n",
	    _statistics[0], _statistics[1], i - 1);

    {
	FILE *fp;
	fp = fopen("x2", "w");
	for (i = 0; i < A->m; i++) {
	    fprintf(fp, "%f %f\n", x[i * 2], x[i * 2 + 1]);
	}
	fclose(fp);
    }
#endif

#ifdef DEBUG
    {
	FILE *fp;
	fp = fopen("/tmp/m", "w");
	export_embedding(fp, dim, A, x, label_sizes);
    }
#endif
#ifdef DEBUG
    fprintf(stderr, "post processing %f\n",
	    ((real) (clock() - cpu)) / CLOCKS_PER_SEC);
#endif
    return flag;
}
