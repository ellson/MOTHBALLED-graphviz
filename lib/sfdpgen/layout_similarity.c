/* $Id$ $Revision$ */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "general.h"
#include "SparseMatrix.h"
#include "call_tri.h"

static double boundingbox_area(int n, double *x){
  double xmax, xmin, ymax, ymin;
  int i;
  xmax = xmin = x[0];
  ymax = ymin = x[1];
  for (i = 0; i < n; i++){
    xmax = MAX(xmax, x[2*i]);
    xmin = MIN(xmin, x[2*i]);
    ymax = MAX(ymax, x[2*i+1]);
    ymin = MIN(ymin, x[2*i+1]);
  }
  return (xmax-xmin)*(ymax-ymin);

}


#define MINDIST 0.000000001

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

static real get_r(int n, real *x, real *y, real theta, real *r, real *p, real *q){
  /* find the best scaling factor r* such that
     f[theta, r] = Sum[(x[i,1] + p - r(Sin[theta] y[i,1] + Cos[theta] y[i,2]))^2+(x[i,2] + q - r(Cos[theta] y[i,1] -Sin[theta] y[i,2]))^2, i = 1, n] is minimized. Return the value f[theta, r*],
     also return r.
  */
  real top = 0, bot = 0, c, s, f = 0, yy, yx, xxavg = 0, xyavg = 0, yxavg = 0, yyavg = 0;
  int i;

  for (i = 0; i < n; i++){
    xxavg += x[2*i];
    xyavg += x[2*i+1];
  }
  xxavg /= n;
  xyavg /= n;

  for (i = 0; i < n; i++){
    s = sin(theta);
    c = cos(theta);
    yx = c*y[2*i] - s*y[2*i+1];
    yy = s*y[2*i] + c*y[2*i+1];
    top += (x[2*i] - xxavg)*yx + (x[2*i+1] - xyavg)*yy;
    bot += yx*yx+yy*yy;
    yxavg += yx;
    yyavg += yy;
  }
  *r = top/(bot - yxavg*yxavg/n - yyavg*yyavg/n);

  *p = yxavg/n*(*r)-xxavg;
  *q = yyavg/n*(*r)-xyavg;

  for (i = 0; i < n; i++){
    s = sin(theta);
    c = cos(theta);
    yx = c*y[2*i] - s*y[2*i+1];
    yy = s*y[2*i] + c*y[2*i+1];
    f += (x[2*i] + *p - (*r)*yx)*(x[2*i] + *p - (*r)*yx)+(x[2*i+1] + *q - (*r)*yy)*(x[2*i+1] + *q - (*r)*yy);
  }
  return f;
}

static real dispacement(int n, real *x, real *y, real *rmin, real *thetamin, real *pmin, real *qmin){
  /* find the distance between two sets of cordinates by finding a suitable
     rotation and scaling to minimize the distance */
  real dist = 0, theta, normx = 0, r, d, p, q;
  int i;
  
  *thetamin = 0.;
  dist = get_r(n, x, y, 0., &r, &p, &q);
  *rmin = r; *pmin = p; *qmin = q;

  for (i = 0; i < 180; i++){
    theta = 3.1415626*i/180;
    d = get_r(n, x, y, theta, &r, &p, &q);
    if (d < dist){
      dist = d;
      *rmin = r; *pmin = p; *qmin = q;
      *thetamin = theta;
    }
  }


  for (i = 0; i < n; i++) normx += x[2*i]*x[2*i]+x[2*i+1]*x[2*i+1];
  return sqrt(dist/normx);
}

int main(int argc, char *argv[])
{


  char *infile;

  SparseMatrix B = NULL;
  int dim = 2, n = 0, i, *ia, *ja, j, k, nz = 0;
  real *x, *y, mean, dev, ratio, *z, r, theta, p, q, xx;

  FILE *fp;

  
  if (argc < 2){
    fprintf(stderr,"Usage: similarity graph layout1 layout2\n");
  }

  infile = argv[1];
  fp = fopen(infile,"r");
  while (fscanf(fp,"%lf %lf\n",&xx, &xx) == 2) n++;
  fclose(fp);

  infile = argv[1];
  fp = fopen(infile,"r");
  x = N_GNEW(n*2,real);
  for (i = 0; i < n; i++){
    fscanf(fp,"%lf %lf\n",&(x[2*i]), &(x[2*i+1]));
  }
  fclose(fp);


  infile = argv[2];
  fp = fopen(infile,"r");
  y = N_GNEW(n*2,real);
  nz = 0;
  for (i = 0; i < n; i++){
    if (fscanf(fp,"%lf %lf\n",&(y[2*i]), &(y[2*i+1])) != 2) goto ERROR;
  }
  fclose(fp);

  B = call_tri(n, 2, x);

  z = N_GNEW(B->nz,real);
  ia = B->ia; ja = B->ja;
  nz = 0;
  for (i = 0; i < n; i++){
    for (j = ia[i]; j < ia[i+1]; j++){
      k = ja[j];
      if (k == i) continue;
      z[nz++] = distance(y,dim,i,k)/distance_cropped(x,dim,i,k);
    }
  }

  /* mean */
  mean = 0;
  for (i = 0; i < nz; i++) mean += z[i];
  mean /= nz;

  /* deviation*/
  dev = 0;
  for (i = 0; i < nz; i++) {
    dev += (z[i]-mean)*(z[i]-mean);
  }
  dev /= nz;
  dev = sqrt(dev);

  /* bounding box area */
  ratio = boundingbox_area(n, y);
  /*/MAX(boundingbox_area(n, x), 0.001);*/


  fprintf(stderr, "mean = %f std = %f disimilarity = %f area = %f badness = %f displacement = %f\n",
	  mean, dev, dev/mean, ratio, (dev/mean+1)*ratio, dispacement(n, x, y, &r, &theta, &p, &q));
  fprintf(stderr, "theta = %f scaling = %f, shift = {%f, %f}\n",theta, 1/r, p, q);


  printf("%.2f %.2f %.2f\n",dev/mean, dispacement(n, x, y, &r, &theta, &p, &q),ratio/1000000.);

  SparseMatrix_delete(B);
  FREE(x);
  FREE(y);
  FREE(z);


  return 0;

 ERROR:
  printf("- - -\n");
  return 0;
}

