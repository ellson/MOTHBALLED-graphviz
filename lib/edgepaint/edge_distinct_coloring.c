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
#include "time.h"
#include "SparseMatrix.h"
#include "node_distinct_coloring.h"
#include "DotIO.h"
#include "intersection.h"
#include "QuadTree.h"

static int splines_intersect(int dim, int u1, int v1, int u2, int v2, 
			     real cos_critical, int check_edges_with_same_endpoint, 
			     real *x, char *xsplines1, char *xsplines2){
  /* u1, v2 an u2, v2: the node index of the two edn points of two edges.
     cos_critical: cos of critical angle
     check_edges_with_same_endpoint: whether need to treat two splines from
     .     the same end point specially in ignoring splines that exit/enter the same end pont at around 180
     x: the coordinates of nodes
     xsplines1,xsplines2: the first and second splines corresponding to two edges

  */
  int itmp;
  int len1 = 100, len2 = 100;
  real *x1, *x2;
  int ns1 = 0, ns2 = 0;
  int i, j, iter1 = 0, iter2 = 0;
  real cos_a, tmp[2];
  int endp1 = 0, endp2 = 0;

  tmp[0] = tmp[1] = 0;
  x1 = MALLOC(sizeof(real)*len1);
  x2 = MALLOC(sizeof(real)*len2);

  assert(dim <= 3);
  /* if two end points are the same, make sure they are the first in each edge */
  if (u1 == v2){/* switch u2 and v2 */
    itmp = u2; u2 = v2; v2 = itmp;
  } else if (v1 == u2){/* switch u1 and v1 */
    itmp = u1; u1 = v1; v1 = itmp;
  } else if (v1 == v2){/* switch both */
    itmp = u2; u2 = v2; v2 = itmp;
    itmp = u1; u1 = v1; v1 = itmp;
  }

  /* origonally I though the two end points has to be included. But now I think it does not need to since
     the splie might start from the border of the label box
  MEMCPY(x1, &(x[dim*u1]), sizeof(real)*dim);
  MEMCPY(x2, &(x[dim*u2]), sizeof(real)*dim);
  ns1++; ns2++;
  */

  /* splines could be a list of 
     1. 3n points
     2. of the form "e,x,y" followed by 3n points, where x,y is really padded to the end of the 3n points
     3. of the form "s,x,y" followed by 3n points, where x,y is padded to the start of the 3n points
  */
  if (xsplines1){
    if(strstr(xsplines1, "e,")){
      endp1 = 1;
      xsplines1 = strstr(xsplines1, "e,") + 2;
    } else if (strstr(xsplines2, "s,")){
      xsplines1 = strstr(xsplines1, "s,") + 2;
    }
  }
  while (xsplines1 && sscanf(xsplines1,"%lf,%lf", &(x1[ns1*dim]), &x1[ns1*dim + 1]) == 2){
    if (endp1 && iter1 == 0){
      tmp[0] = x1[ns1*dim]; tmp[1] = x1[ns1*dim + 1];
    } else {
      ns1++;
    }
    iter1++;
    xsplines1 = strstr(xsplines1, " ");
    if (!xsplines1) break;
    xsplines1++;
    if (ns1*dim >= len1){
      len1 = ns1*dim + (int)MAX(10, 0.2*ns1*dim);
      x1 = REALLOC(x1, sizeof(real)*len1);
    }
  }
  if (endp1){/* pad the end point at the last position */
    ns1++;
    if (ns1*dim >= len1){
      len1 = ns1*dim + (int)MAX(10, 0.2*ns1*dim);
      x1 = REALLOC(x1, sizeof(real)*len1);
    }
    x1[(ns1-1)*dim] = tmp[0];  x1[(ns1-1)*dim + 1] = tmp[1]; 
  }


  /* splines could be a list of 
     1. 3n points
     2. of the form "e,x,y" followed by 3n points, where x,y is really padded to the end of the 3n points
     3. of the form "s,x,y" followed by 3n points, where x,y is padded to the start of the 3n points
  */
  if (xsplines2){
    if(strstr(xsplines2, "e,")){
      endp2 = 1;
      xsplines2 = strstr(xsplines2, "e,") + 2;
    } else if (strstr(xsplines2, "s,")){
      xsplines2 = strstr(xsplines2, "s,") + 2;
    }
  }
  while (xsplines2 && sscanf(xsplines2,"%lf,%lf", &(x2[ns2*dim]), &x2[ns2*dim + 1]) == 2){
    if (endp2 && iter2 == 0){
      tmp[0] = x2[ns2*dim]; tmp[1] = x2[ns2*dim + 1];
    } else {
      ns2++;
    }
    iter2++;
    xsplines2 = strstr(xsplines2, " ");
    if (!xsplines2) break;
    xsplines2++;
    if (ns2*dim >= len2){
      len2 = ns2*dim + (int)MAX(10, 0.2*ns2*dim);
      x2 = REALLOC(x2, sizeof(real)*len2);
    }
  }
  if (endp2){/* pad the end point at the last position */
    ns2++;
    if (ns2*dim >= len2){
      len2 = ns2*dim + (int)MAX(10, 0.2*ns2*dim);
      x2 = REALLOC(x2, sizeof(real)*len2);
    }
    x2[(ns2-1)*dim] = tmp[0];  x2[(ns2-1)*dim + 1] = tmp[1]; 
  }

  /* origonally I though the two end points has to be included. But now I think it does not need to since
     the splie might start from the border of the label box
  MEMCPY(&(x2[dim*ns2]), &(x[dim*v2]), sizeof(real)*dim);
  ns2++;
  MEMCPY(&(x1[dim*ns1]), &(x[dim*v1]), sizeof(real)*dim);
  ns1++;
  */

for (i = 0; i < ns1 - 1; i++){
    for (j = 0; j < ns2 - 1; j++){
      cos_a = intersection_angle(&(x1[dim*i]), &(x1[dim*(i + 1)]), &(x2[dim*j]), &(x2[dim*(j+1)]));
      if (!check_edges_with_same_endpoint && cos_a >= -1) cos_a = ABS(cos_a);
      if (cos_a > cos_critical) {
	return 1;
      }

    }
  }

  FREE(x1);
  FREE(x2);
  return 0;
}


Agraph_t* edge_distinct_coloring(char *color_scheme, char *lightness, Agraph_t* g, real angle, real accuracy, int check_edges_with_same_endpoint, int seed){
  /* color the edges of a graph so that conflicting edges are as dinstrinct in color as possibl.
     color_scheme: rgb, lab, gray, or a list of comma separaterd RGB colors in hex, like #ff0000,#00ff00
     lightness: of the form 0,70, specifying the range of lightness of LAB color. Ignored if scheme is not COLOR_LAB.
     .          if NULL, 0,70 is assumed
     g: the graph
     angle: if two edges cross at an angle < "angle", consider they as conflict
     accuracy: how accurate when finding  color of an edge to be as different from others
     check_edges_with_same_endpoint: if TRUE, we will check edges with same end point and only consider them as conflict if
     .                   their angle is very small. Edges that share an end point and is close to 180 degree
     .                   are not consider conflict.
     seed: random_seed. If negative, consider -seed as the number of random start iterations
  */
  real *x = NULL;
  int dim = 2;
  SparseMatrix A, B, C;
  int *irn, *jcn, nz, nz2 = 0;
  real cos_critical = cos(angle/180*3.14159), cos_a;
  int u1, v1, u2, v2, i, j;
  real *colors = NULL, color_diff, color_diff_sum;
  int flag, ne;
  char **xsplines = NULL;
  int cdim;

  A = SparseMatrix_import_dot(g, dim, NULL, &x, NULL, NULL, FORMAT_COORD, NULL);
  if (!x){
    fprintf(stderr,"The gv file contains no or improper 2D coordinates\n");
    return NULL;
  }


  irn = A->ia; jcn = A->ja;
  nz = A->nz;

  /* get rid of self edges */
  for (i = 0; i < nz; i++){
    if (irn[i] != jcn[i]){
      irn[nz2] = irn[i];
      jcn[nz2++] = jcn[i];
    }
  }

  if (Verbose)
    fprintf(stderr,"cos = %f, nz2 = %d\n", cos_critical, nz2);
  /* now find edge collision */
  nz = 0;
  B = SparseMatrix_new(nz2, nz2, 1, MATRIX_TYPE_REAL, FORMAT_COORD);

  if (Import_dot_splines(g, &ne, &xsplines)){
#ifdef TIME
    clock_t start = clock();
#endif
    assert(ne == nz2);
    cos_a = 1.;/* for splines we exit conflict check as soon as we find an conflict, so the anle may not be representitive, hence set to constant */
    for (i = 0; i < nz2; i++){
      u1 = irn[i]; v1 = jcn[i];
      for (j = i+1; j < nz2; j++){
	u2 = irn[j]; v2 = jcn[j];
	if (splines_intersect(dim, u1, v1, u2, v2, cos_critical, check_edges_with_same_endpoint, x, xsplines[i], xsplines[j])){
	  B = SparseMatrix_coordinate_form_add_entries(B, 1, &i, &j, &cos_a);
	}
      }
    }
#ifdef TIME
    fprintf(stderr, "cpu for dual graph =%10.3f", ((real) (clock() - start))/CLOCKS_PER_SEC);
#endif
    
  } else {
    /* no splines, justsimple edges */
#ifdef TIME
    clock_t start = clock();
#endif
    
    
    for (i = 0; i < nz2; i++){
      u1 = irn[i]; v1 = jcn[i];
      for (j = i+1; j < nz2; j++){
	u2 = irn[j]; v2 = jcn[j];
	cos_a = intersection_angle(&(x[dim*u1]), &(x[dim*v1]), &(x[dim*u2]), &(x[dim*v2]));
	if (!check_edges_with_same_endpoint && cos_a >= -1) cos_a = ABS(cos_a);
	if (cos_a > cos_critical) {
	  B = SparseMatrix_coordinate_form_add_entries(B, 1, &i, &j, &cos_a);
	}
      }
    }
#ifdef TIME
    fprintf(stderr, "cpu for dual graph (splines) =%10.3f\n", ((real) (clock() - start))/CLOCKS_PER_SEC);
#endif
  } 
  C = SparseMatrix_from_coordinate_format(B);
  if (B != C) SparseMatrix_delete(B);
  
  {
#ifdef TIME
    clock_t start = clock();
#endif
    int weightedQ = FALSE;
    int iter_max = 100;
    node_distinct_coloring(color_scheme, lightness, weightedQ, C, accuracy, iter_max, seed, &cdim, &colors, &color_diff, &color_diff_sum, &flag);
    if (flag) goto RETURN;
#ifdef TIME
    fprintf(stderr, "cpu for color assignmment =%10.3f\n", ((real) (clock() - start))/CLOCKS_PER_SEC);
#endif
  }

  /* for printing dual*/
#if 0
  {
    FILE*fp;
    fp = fopen("/tmp/dual.gv","w");
    fprintf(fp,"graph {\n");
    for (i = 0; i < nz2; i++){
      u1 = irn[i]; v1 = jcn[i];
      for (j = i+1; j < nz2; j++){
	u2 = irn[j]; v2 = jcn[j];
	cos_a = intersection_angle(&(x[dim*u1]), &(x[dim*v1]), &(x[dim*u2]), &(x[dim*v2]));
	if (!check_edges_with_same_endpoint && cos_a >= -1) cos_a = ABS(cos_a);
	if (cos_a > cos_critical) {
	  fprintf(fp,"\"%d_%d\" -- \"%d_%d\"\n", u1,v1,u2,v2);
	}
      }
    }
    for (i = 0; i < nz2; i++){
      u1 = irn[i]; v1 = jcn[i];
      if (cdim == 3) {
	fprintf(fp,"\"%d_%d\" [label2=%d pos=\"%f,%f\", color=\"#%02x%02x%02x\"]\n", u1,v1, i, 0.5*(x[dim*u1] + x[dim*v1]), 0.5*(x[dim*u1+1] + x[dim*v1+1]), 
		MIN((unsigned int)(colors[i*cdim]*255),255), MIN((unsigned int) (colors[i*cdim+1]*255), 255), MIN((unsigned int)(colors[i*cdim+2]*255), 255));
      } else if (cdim == 2){
	fprintf(fp,"\"%d_%d\" [label2=%d pos=\"%f,%f\", color=\"#%02x%02x%02x\"]\n", u1,v1, i, 0.5*(x[dim*u1] + x[dim*v1]), 0.5*(x[dim*u1+1] + x[dim*v1+1]), 
		MIN((unsigned int)(colors[i*cdim]*255),255), MIN((unsigned int) (colors[i*cdim+1]*255), 255), 255);
      } else {
	fprintf(fp,"\"%d_%d\" [label2=%d pos=\"%f,%f\", color=\"#%02x%02x%02x\"]\n", u1,v1, i, 0.5*(x[dim*u1] + x[dim*v1]), 0.5*(x[dim*u1+1] + x[dim*v1+1]), 
		MIN((unsigned int)(colors[i*cdim]*255),255), MIN((unsigned int) (colors[i*cdim]*255), 255), MIN((unsigned int)(colors[i*cdim]*255), 255));
      }
    }
    fprintf(fp,"}\n");
  }
#endif

  if (Verbose)
    fprintf(stderr,"The edge conflict graph has %d nodes and %d edges, final color_diff=%f color_diff_sum = %f\n",
	  C->m, C->nz, color_diff, color_diff_sum);

  attach_edge_colors(g, cdim, colors);

 RETURN:
  SparseMatrix_delete(A);
  SparseMatrix_delete(C);
  FREE(colors);
  FREE(x);
  if (xsplines){
    for (i = 0; i < ne; i++){
      if (xsplines[i]) FREE(xsplines[i]);
    }
    FREE(xsplines);
  }
  return g;

}

