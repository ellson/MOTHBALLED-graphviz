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
#include "lab.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "color_palette.h"
#include "lab_gamut.h"

color_rgb color_rgb_init(double r, double g, double b){
  color_rgb rgb;
  rgb.r = r; rgb.g = g; rgb.b = b;
  return rgb;
}

color_xyz color_xyz_init(double x, double y, double z){
  color_xyz xyz;
  xyz.x = x; xyz.y = y; xyz.z = z;
  return xyz;
}


color_lab color_lab_init(double l, double a, double b){
  color_lab lab;
  lab.l = l; lab.a = a; lab.b = b;
  return lab;
}

double XYZEpsilon = 216./24389.;
double XYZKappa = 24389./27.;

static double PivotXYZ(double n){
  if (n > XYZEpsilon) return pow(n, 1/3.);
  return (XYZKappa*n + 16)/116;
}

static double PivotRgb(double n){
  if (n > 0.04045) return 100*pow((n + 0.055)/1.055, 2.4);
  return 100*n/12.92;
}

color_xyz RGB2XYZ(color_rgb color){
  double r = PivotRgb(color.r/255.0);
  double g = PivotRgb(color.g/255.0);
  double b = PivotRgb(color.b/255.0);
  return color_xyz_init(r*0.4124 + g*0.3576 + b*0.1805, r*0.2126 + g*0.7152 + b*0.0722, r*0.0193 + g*0.1192 + b*0.9505);
}

color_lab RGB2LAB(color_rgb color){
  color_xyz white = color_xyz_init(95.047, 100.000, 108.883);
  color_xyz xyz = RGB2XYZ(color);
  double x = PivotXYZ(xyz.x/white.x);
  double y = PivotXYZ(xyz.y/white.y);
  double z = PivotXYZ(xyz.z/white.z);
  double L = MAX(0, 116*y - 16);
  double A = 500*(x - y);
  double B = 200*(y - z);
  return color_lab_init(L, A, B);
}

void LAB2RGB_real_01(real *color){
  /* convert an array[3] of LAB colors to RGB between 0 to 1, in place */
  color_rgb rgb;
  color_lab lab;

  lab.l = color[0];
  lab.a = color[1];
  lab.b = color[2];
  rgb = LAB2RGB(lab);
  color[0] = rgb.r/255;
  color[1] = rgb.g/255;
  color[2] = rgb.b/255;
}
color_rgb LAB2RGB(color_lab color){
  double y = (color.l + 16.0)/116.0;
  double x = color.a/500.0 + y;
  double  z = y - color.b/200.0;
  color_xyz white = color_xyz_init(95.047, 100.000, 108.883), xyz;
  double t1, t2, t3;
  if(pow(x, 3.) > XYZEpsilon){
    t1 = pow(x, 3.);
  } else {
    t1 = (x - 16.0/116.0)/7.787;
  }
  if (color.l > (XYZKappa*XYZEpsilon)){
    t2 = pow(((color.l + 16.0)/116.0), 3.);
  } else {
    t2 = color.l/XYZKappa;
  }
  if (pow(z, 3.) > XYZEpsilon){
    t3 = pow(z, 3.);
  } else {
    t3 = (z - 16.0/116.0)/7.787;
  }
  xyz = color_xyz_init(white.x*t1, white.y*t2, white.z*t3);
  return XYZ2RGB(xyz);
}

color_rgb XYZ2RGB(color_xyz color){
  double x = color.x/100.0;
  double y = color.y/100.0;
  double z = color.z/100.0;
  double r = x*3.2406 + y*(-1.5372) + z*(-0.4986);
  double g = x*(-0.9689) + y*1.8758 + z*0.0415;
  double b = x*0.0557 + y*(-0.2040) + z*1.0570;
  if (r > 0.0031308){
    r = 1.055*pow(r, 1/2.4) - 0.055;
  } else {
    r = 12.92*r;
  }
  if (g > 0.0031308) {
    g = 1.055*pow(g, 1/2.4) - 0.055;
  } else {
    g = 12.92*g;
  }
  if (b > 0.0031308){
    b = 1.055*pow(b, 1/2.4) - 0.055;
  } else {
    b = 12.92*b;
  }
  r = MAX(0, r);
  r = MIN(255, r*255);
  g = MAX(0, g);
  g = MIN(255, g*255);
  b = MAX(0, b);
  b = MIN(255, b*255);

  return color_rgb_init(r, g, b);
}

void get_level(QuadTree qt, int *level0){
  QuadTree q;
  int level_max = 0, level;
  int i;

  if (!qt->qts) return;
  for (i = 0; i < 1<<(qt->dim); i++){
    q = qt->qts[i];
    if (q) {
      level = *level0 + 1;
      get_level(q, &level);
      level_max = MAX(level_max, level);
    }
  }
  *level0 = level_max;
}

double *lab_gamut_from_file(char *gamut_file, const char *lightness, int *n){
  /* give a list of n points  in the file defining the LAB color gamut. return NULL if the mgamut file is not found.
     lightness is a string of the form 0,70, or NULL.
   */
  FILE *fp; 
  enum {buf_len = 10000};
  char buf[buf_len];
  double *xx, *x;

  int l1 = 0, l2 = 70;
  
  if (lightness && sscanf(lightness, "%d,%d", &l1, &l2) == 2){
    if (l1 < 0) l1 = 0;
    if (l2 > 100) l2 = 100;
    if (l1 > l2) l1 = l2;
  } else {
    l1 = 0; l2 = 70;
  }


  *n = 0;

  if (Verbose)
    fprintf(stderr,"LAB color lightness range = %d,%d\n", l1, l2);

  fp = fopen(gamut_file, "r");
  if (!fp) return NULL;
  while (fgets(buf, buf_len, fp)){
    (*n)++;
  }
  rewind(fp);

  x = malloc(sizeof(double)*3*(*n));
  xx = x;
  *n = 0;
  while (fgets(buf, buf_len, fp)){
    sscanf(buf,"%lf %lf %lf", xx, xx+1, xx+2);
    if (*xx >= l1 && *xx <= l2){
      xx += 3;
      (*n)++;
    }
  }
  fclose(fp);
  return x;
}


double *lab_gamut(const char *lightness, int *n){
  /* give a list of n points  in the file defining the LAB color gamut. return NULL if the mgamut file is not found.
     lightness is a string of the form 0,70, or NULL.
   */
  double *xx, *x;

  int l1 = 0, l2 = 70, m, i;


  if (lightness && sscanf(lightness, "%d,%d", &l1, &l2) == 2){
    if (l1 < 0) l1 = 0;
    if (l2 > 100) l2 = 100;
    if (l1 > l2) l1 = l2;
  } else {
    l1 = 0; l2 = 70;
  }


  if (Verbose)
    fprintf(stderr,"LAB color lightness range = %d,%d\n", l1, l2);

  m = sizeof(lab_gamut_data)/sizeof(lab_gamut_data[0]);

  if (Verbose)
    fprintf(stderr,"size of lab gamut = %d\n", m);

  x = malloc(sizeof(double)*3*m);
  xx = x;
  *n = 0;
  for (i = 0; i < m; i++){
    if (lab_gamut_data[i].l >= l1 && lab_gamut_data[i].l <= l2){
      xx[0] = lab_gamut_data[i].l;
      xx[1] = lab_gamut_data[i].a;
      xx[2] = lab_gamut_data[i].b;
      xx += 3;
      (*n)++;
    }
  }

  return x;
}

QuadTree lab_gamut_quadtree(char *gamut_file, const char *lightness, int max_qtree_level){
  /* read the color gamut points list in the form "x y z\n ..." and store in the octtree. return NULL if file not openable */
  int n;
  //  double *x = lab_gamut_from_file(gamut_file, lightness, &n);
  double *x = lab_gamut(lightness, &n);
  QuadTree qt;
  int dim = 3;

  if (!x) return NULL;
  qt = QuadTree_new_from_point_list(dim, n, max_qtree_level, x, NULL);


  FREE(x);
  return qt;
}

static double lab_dist(color_lab x, color_lab y){
  return sqrt((x.l-y.l)*(x.l-y.l) +(x.a-y.a)*(x.a-y.a) +(x.b-y.b)*(x.b-y.b));
}

static void lab_interpolate(color_lab lab1, color_lab lab2, double t, double *colors){
  colors[0] = lab1.l + t*(lab2.l - lab1.l);
  colors[1] = lab1.a + t*(lab2.a - lab1.a);
  colors[2] = lab1.b + t*(lab2.b - lab1.b);
}

void color_blend_rgb2lab(char *color_list, const int maxpoints, double **colors0){
  /* give a color list of the form "#ff0000,#00ff00,...", get a list of around maxpoints
     colors in an array colors0 of size [maxpoints*3] of the form {{l,a,b},...}.
     If *colors0 is NULL, it will be allocated.
     color_list: either "#ff0000,#00ff00,...", or "pastel"
  */

  int nc = 1, r, g, b, i, ii, jj, cdim = 3;
  char *cl;
  color_lab *lab;
  color_rgb rgb;
  double *dists, step, dist_current;
  double *colors = NULL;
  char *cp;

  cp = color_palettes_get(color_list);
  if (cp){
    color_list = cp;
  }

  if (maxpoints <= 0) return;

  cl = color_list;
  while ((cl=strstr(cl, ",")) != NULL){
    cl++; nc++;
  }
  lab = malloc(sizeof(color_lab)*MAX(nc,1));

  cl = color_list - 1;
  nc = 0;
  do {
    cl++;
    if (sscanf(cl,"#%02X%02X%02X", &r, &g, &b) != 3) break;
    rgb.r = r; rgb.g = g; rgb.b = b;
    lab[nc++] = RGB2LAB(rgb);
  } while ((cl=strstr(cl, ",")) != NULL);

  dists = malloc(sizeof(double)*MAX(1, nc));
  dists[0] = 0;
  for (i = 0; i < nc - 1; i++){
    dists[i+1] = lab_dist(lab[i], lab[i+1]);
  }
  /* dists[i] is now the summed color distance from  the 0-th color to the i-th color */
  for (i = 0; i < nc - 1; i++){
    dists[i+1] += dists[i];
  }
  if (Verbose)
    fprintf(stderr,"sum = %f\n", dists[nc-1]);

  if (!(*colors0)){
    *colors0 = malloc(sizeof(double)*maxpoints*cdim);
  }
  colors = *colors0; 
  if (maxpoints == 1){
    colors[0] = lab[0].l;
    colors[1] = lab[0].a;
    colors[2] = lab[0].b;
  } else {
    step = dists[nc-1]/(maxpoints - 1);
    ii = 0; jj = 0; dist_current = 0;
    while (dists[jj] < dists[ii] + step) jj++;
    
    for (i = 0; i < maxpoints; i++){
      lab_interpolate(lab[ii], lab[jj], (dist_current - dists[ii])/MAX(0.001, (dists[jj] - dists[ii])), colors);
      dist_current += step;
      colors += cdim;
      if (dist_current > dists[jj]) ii = jj;
      while (jj < nc -1 && dists[jj] < dists[ii] + step) jj++;
    }
  }
  FREE(dists);
  FREE(lab);  
}





color_rgb color_blend_rgb(char *color_list, real ratio, int *flag){
  /* give a color list of the form "#ff0000,#00ff00,...", get a blend at ratio*100 percent of the position in that list and return in string color0 of the form #abcdef
     If *colors0 is NULL, it will be allocated.
     color_list: either "#ff0000,#00ff00,...", or "pastel"
  */

  int nc = 1, r, g, b, i, ii;
  char *cl;
  color_lab *lab = NULL;
  color_rgb rgb;
  double *dists = NULL;
  char *cp;
  color_lab clab;
  double color[3];

  *flag = 0;

  ratio = MAX(ratio, 0);
  ratio = MIN(ratio, 1);

  cp = color_palettes_get(color_list);
  if (cp){
    color_list = cp;
  }

  cl = color_list;
  while ((cl=strstr(cl, ",")) != NULL){
    cl++; nc++;
  }

  lab = malloc(sizeof(color_lab)*MAX(nc,1));

  cl = color_list - 1;
  nc = 0;
  do {
    cl++;
    if (sscanf(cl,"#%02X%02X%02X", &r, &g, &b) != 3) break;
    rgb.r = r; rgb.g = g; rgb.b = b;
    lab[nc++] = RGB2LAB(rgb);
  } while ((cl=strstr(cl, ",")) != NULL);

  if (nc == 1 || ratio == 0){
    rgb = LAB2RGB(lab[0]);
    goto RETURN;
  } else if (nc == 0){
    fprintf(stderr, "no color\n");
    *flag = -1;
    goto RETURN;
  }

  dists = malloc(sizeof(double)*MAX(1, nc));
  dists[0] = 0;
  for (i = 0; i < nc - 1; i++){
    dists[i+1] = lab_dist(lab[i], lab[i+1]);
  }
  /* dists[i] is now the summed color distance from  the 0-th color to the i-th color */
  for (i = 0; i < nc - 1; i++){
    dists[i+1] += dists[i];
  }
  if (dists[nc-1] == 0){/* same color in the list */
    rgb = LAB2RGB(lab[0]);
    goto RETURN;
  }
  for (i = 0; i < nc; i++){
    dists[i] /= dists[nc - 1];
  }

  ii = 0;
  while (dists[ii] < ratio) ii++;
    
  assert(ii < nc && ii > 0);

  lab_interpolate(lab[ii - 1], lab[ii], (ratio - dists[ii - 1])/MAX(0.001, (dists[ii] - dists[ii - 1])), color);
  clab = color_lab_init(color[0], color[1], color[2]);
  rgb = LAB2RGB(clab);

  

  RETURN:
  if (dists) FREE(dists);
  if (lab) FREE(lab);  
  return rgb;
}

void color_blend_rgbstring(char *color_list, real ratio, char **color0, int *flag){
  color_rgb rgb;

  if (!(*color0)){
    *color0 = malloc(sizeof(char)*7);
  }
  rgb = color_blend_rgb(color_list, ratio, flag);
  sprintf(*color0, "%02X%02X%02X", (int) (rgb.r), (int) (rgb.g), (int) (rgb.b));
}
