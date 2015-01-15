/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/
#include "general.h"
#include "QuadTree.h"
#include "furtherest_point.h"

static real dist(int dim, real *x, real *y){
  int k;
  real d = 0;
  for (k = 0; k < dim; k++) d += (x[k] - y[k])*(x[k]-y[k]);
  return sqrt(d);
}


static real distance_to_group(int k, int dim, real *wgt, real *pts, real *center, real (*usr_dist)(int, real*, real*)){
  int i;
  real distance = -1, dist_min = 0;
  if (!wgt){
    for (i = 0; i < k; i++){
      distance = (*usr_dist)(dim, &(pts[i*dim]), center);
      if (i == 0){
	dist_min = distance;
      } else {
	dist_min = MIN(dist_min, distance);
      }
    }
  } else {
    for (i = 0; i < k; i++){
      distance = (*usr_dist)(dim, &(pts[i*dim]), center);
      if (i == 0){
	dist_min = wgt[i]*distance;
      } else {
	dist_min = MIN(dist_min, wgt[i]*distance);
      }
    }
  }
  return dist_min;
}

void furtherest_point(int k, int dim, real *wgt, real *pts, real *center, real width, int max_level, real (*usr_dist)(int, real*, real*), real *dist_max, real **argmax){
  /* Assume that in the box defined by {center, width} are feasible;     
     find, in the, a point "furtherest_point" that is furtherest away from a group of k-points pts, using quadtree,
     with up to max_level. Here the distance of a point to a group of point is defined as the minimum 
     of the distance of that point to all the points in the group,
     and each distance is defined by the function dist.

     Input:
     
     k: number of points in the group
     dim: dimension
     wgt: if not null, the weighting factor for the i-th point is wgt[i]. The color distance
     .    of a point p to a group of points pts is min_i(wgt[i]*dist(p, pts[i])), instead of min_i(dist(p, pts[i]))
     pts: the i-th point is [i*k, (i+1)*k)
     center: the center of the root of quadtree
     width: the width of the root
     max_level: max level to go down
     usr_dist: the distance function. If NULL, assume Euclidean. If NULL, set to Euclidean.
     argmax: on entry, if NULL, will be allocated, iotherwise must be an array of size >= dim which will hold the furtherest point. 

     Return: the point (argmax) furtherest away from the group, and the distance dist_max.
   */
  QuadTree qt, qt0;
  int ncandidates = 10, ncandidates_max = 10, ntmp;
  QuadTree *candidates, *ctmp;/* a cadidate array of quadtrees */
  int ncandidates2 = 10, ncandidates2_max = 10;
  QuadTree *candidates2;/* a cadidate array of quadtrees */
  real distance;
  int level = 0;
  int i, ii, j, pruned;
  real wmax = 0;

  if (!usr_dist) usr_dist = dist;
  if (wgt){
    for (i = 0; i < k; i++) wmax = MAX(wgt[i], wmax);
  } else {
    wmax = 1.;
  }

  qt0 = qt = QuadTree_new(dim, center, width, max_level);

  qt->total_weight = *dist_max = distance_to_group(k, dim, wgt, pts, center, usr_dist);/* store distance in total_weight */
  if (!(*argmax)) *argmax = MALLOC(sizeof(real)*dim);
  MEMCPY(*argmax, center, sizeof(real)*dim);

  candidates = MALLOC(sizeof(qt)*ncandidates_max);
  candidates2 = MALLOC(sizeof(qt)*ncandidates2_max);
  candidates[0] = qt;
  ncandidates = 1;

  /* idea: maintain the current best point and best (largest) distance. check the list of candidate. Subdivide each into quadrants, if any quadrant gives better distance, update, and put on the candidate
     list. If we can not prune a quadrant (a auadrant can be pruned if the distance of its center to the group of points pts, plus that from the center to the corner of the quadrant, is smaller than the best), we
     also put it down on the candidate list. We then recurse on the candidate list, unless the max level is reached. */
  while (level++ < max_level){ 
    if (Verbose > 10) {
      fprintf(stderr,"level=%d=================\n", level);
    }
    ncandidates2 = 0;
    for (i = 0; i < ncandidates; i++){


      qt = candidates[i];
      assert(!(qt->qts));

      if (Verbose > 10) {
	fprintf(stderr,"candidate %d at {", i);
	for (j = 0; j < dim; j++) fprintf(stderr,"%f, ", qt->center[j]);
	fprintf(stderr,"}, width = %f, dist = %f\n", qt->width, qt->total_weight);
      }

      distance = qt->total_weight;/* total_weight is used to store the distance from teh center to the group */
      if (distance + wmax*sqrt(((real) dim))*qt->width < *dist_max) continue;/* this could happen if this candidate was entered into the list earlier than a better one later in the list */
      qt->qts = MALLOC(sizeof(QuadTree)*(1<<dim));
      for (ii = 0; ii < 1<<dim; ii++) {
	qt->qts[ii] = QuadTree_new_in_quadrant(qt->dim, qt->center, (qt->width)/2, max_level, ii);
	qt->qts[ii]->total_weight = distance = distance_to_group(k, dim, wgt, pts, qt->qts[ii]->center, usr_dist);/* store distance in total_weight */
	pruned = FALSE;
	if (distance > *dist_max){
	  *dist_max = distance;
	  if (Verbose > 10) {
	    fprintf(stderr,"new distmax=%f, pt={", *dist_max);
	    for (j = 0; j < dim; j++) fprintf(stderr,"%f, ", qt->qts[ii]->center[j]);
	    fprintf(stderr,"}\n");
 	  }
	  MEMCPY(*argmax, qt->qts[ii]->center, sizeof(real)*dim);
	} else if (distance + wmax*sqrt(((real) dim))*(qt->width)/2 < *dist_max){
	  pruned = TRUE;
	}
	if (!pruned){
	  if (ncandidates2 >= ncandidates2_max){
	    ncandidates2_max += (int)MAX(0.2*ncandidates2_max, 10);
	    candidates2 = REALLOC(candidates2, sizeof(QuadTree)*ncandidates2_max);
	  }
	  candidates2[ncandidates2++] = qt->qts[ii];
	}
      }/* finish checking every of the 2^dim siblings */
    }/* finish checking all the candidates */

    /* sawp the two lists */
    ntmp = ncandidates;
    ncandidates = ncandidates2;
    ncandidates2 = ntmp;

    ntmp = ncandidates_max;
    ncandidates_max = ncandidates2_max;
    ncandidates2_max = ntmp;

    ctmp = candidates;
    candidates = candidates2;
    candidates2 = ctmp;

  }/* continue down the quadtree */

  if (Verbose > 10) {
    FILE *fp;
    fp = fopen("/tmp/1.m","w");
    QuadTree_print(fp, qt0);
  }

  QuadTree_delete(qt0);

  FREE(candidates);
  FREE(candidates2);

}


void furtherest_point_in_list(int k, int dim, real *wgt, real *pts, QuadTree qt, int max_level, 
			      real (*usr_dist)(int, real*, real*), real *dist_max, real **argmax){
  /* Given a list of points in the Euclidean space contained in the quadtree qt (called the feasible points), find among them one that
     is closest to another list of point {dim, k, pts}.


     find, in the, a point "furtherest_point" that is furtherest away from a group of k-points pts, using quadtree,
     with up to max_level. Here the distance of a point to a group of point is defined as the minimum 
     of the distance of that point to all the points in the group,
     and each distance is defined by the function dist.

     Input:
     
     k: number of points in the group
     dim: dimension
     wgt: if not null, the weighting factor for the i-th point is wgt[i]. The color distance
     .    of a point p to a group of points pts is min_i(wgt[i]*dist(p, pts[i])), instead of min_i(dist(p, pts[i]))
     pts: the i-th point is [i*k, (i+1)*k)
     center: the center of the root of quadtree
     width: the width of the root
     max_level: max level to go down
     usr_dist: the distance function. If NULL, assume Euclidean. If NULL, set to Euclidean.
     argmax: on entry, if NULL, will be allocated, iotherwise must be an array of size >= dim which will hold the furtherest point. 

     Return: the point (argmax) furtherest away from the group, and the distance dist_max.
   */

  int ncandidates = 10, ncandidates_max = 10, ntmp;
  QuadTree *candidates, *ctmp;/* a cadidate array of quadtrees */
  int ncandidates2 = 10, ncandidates2_max = 10;
  QuadTree *candidates2;/* a cadidate array of quadtrees */
  real distance;
  int level = 0;
  int i, ii, j, pruned;
  real *average;
  real wmax = 0.;

  if (!usr_dist) usr_dist = dist;

  if (wgt){
    for (i = 0; i < k; i++) wmax = MAX(wgt[i], wmax);
  } else {
    wmax = 1.;
  }

  average = qt->average;
  qt->total_weight = *dist_max = distance_to_group(k, dim, wgt, pts, average, usr_dist);/* store distance in total_weight */
  if (!(*argmax)) *argmax = MALLOC(sizeof(real)*dim);
  MEMCPY(*argmax, average, sizeof(real)*dim);

  candidates = MALLOC(sizeof(qt)*ncandidates_max);
  candidates2 = MALLOC(sizeof(qt)*ncandidates2_max);
  candidates[0] = qt;
  ncandidates = 1;

  /* idea: maintain the current best point and best (largest) distance. check the list of candidate. Subdivide each into quadrants, if any quadrant gives better distance, update, and put on the candidate
     list. If we can not prune a quadrant (a auadrant can be pruned if the distance of its center to the group of points pts, plus that from the center to the corner of the quadrant, is smaller than the best), we
     also put it down on the candidate list. We then recurse on the candidate list, unless the max level is reached. */
  while (level++ < max_level){ 
    if (Verbose > 10) {
      fprintf(stderr,"level=%d=================\n", level);
    }
    ncandidates2 = 0;
    for (i = 0; i < ncandidates; i++){
      qt = candidates[i];

      if (Verbose > 10) {
	fprintf(stderr,"candidate %d at {", i);
	for (j = 0; j < dim; j++) fprintf(stderr,"%f, ", qt->center[j]);
	fprintf(stderr,"}, width = %f, dist = %f\n", qt->width, qt->total_weight);
      }

      distance = qt->total_weight;/* total_weight is used to store the distance from average feasible points to the group */
      if (qt->n == 1 || distance + wmax*2*sqrt(((real) dim))*qt->width < *dist_max) continue;/* this could happen if this candidate was entered into the list earlier than a better one later in the list. Since the distance
									     is from the average of the feasible points in the square which may not be at the center */

      if (!(qt->qts)) continue;

      for (ii = 0; ii < 1<<dim; ii++) {
	if (!(qt->qts[ii])) continue;
	qt->qts[ii]->total_weight = distance = distance_to_group(k, dim, wgt, pts, qt->qts[ii]->average, usr_dist);/* store distance in total_weight */
	pruned = FALSE;
	if (distance > *dist_max){
	  *dist_max = distance;
	  if (Verbose > 10) {
	    fprintf(stderr,"new distmax=%f, pt={", *dist_max);
	    for (j = 0; j < dim; j++) fprintf(stderr,"%f, ", qt->qts[ii]->average[j]);
	    fprintf(stderr,"}\n");
 	  }
	  MEMCPY(*argmax, qt->qts[ii]->average, sizeof(real)*dim);
	} else if (distance + wmax*sqrt(((real) dim))*(qt->width) < *dist_max){/* average feasible point in this square is too close to the point set */
	  pruned = TRUE;
	}
	if (!pruned){
	  if (ncandidates2 >= ncandidates2_max){
	    ncandidates2_max += (int)MAX(0.2*ncandidates2_max, 10);
	    candidates2 = REALLOC(candidates2, sizeof(QuadTree)*ncandidates2_max);
	  }
	  candidates2[ncandidates2++] = qt->qts[ii];
	}
      }/* finish checking every of the 2^dim siblings */
    }/* finish checking all the candidates */

    /* sawp the two lists */
    ntmp = ncandidates;
    ncandidates = ncandidates2;
    ncandidates2 = ntmp;

    ntmp = ncandidates_max;
    ncandidates_max = ncandidates2_max;
    ncandidates2_max = ntmp;

    ctmp = candidates;
    candidates = candidates2;
    candidates2 = ctmp;

  }/* continue down the quadtree */

  FREE(candidates);
  FREE(candidates2);

}
