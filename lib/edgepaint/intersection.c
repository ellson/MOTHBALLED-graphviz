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

static real cross(real *u, real *v){
  return u[0]*v[1] - u[1]*v[0];
}


/*

There's a nice approach to this problem that uses vector cross
products. Define the 2-dimensional vector cross product v * w to be
vxwy \[Minus] vywx (this is the magnitude of the 3-dimensional cross
product).

Suppose the two line segments run from p to p + r and from q to q +s. 
Then any point on the first line is representable as p + t r (for a
scalar parameter t) and any point on the second line as q + u s (for a
scalar parameter u).

The two lines intersect if we can find t and u such that:

p + t r = q + u s

cross both sides with s, getting

(p + t r) * s = (q + u s) * s

And since s * s = 0, this means

t(r * s) = (q \[Minus] p) * s

And therefore, solving for t:

t = (q \[Minus] p) * s / (r * s)

In the same way, we can solve for u:

u = (q \[Minus] p) * r / (r * s)

Now if r * s = 0 then the two lines are parallel. 
(There are two cases: if (q \[Minus] p) * r = 0 too, 
then the lines are collinear, otherwise they never intersect.)

Otherwise the intersection point is on the original pair 
of line segments if 0 <= t <= 1 and 0 <= u <= 1.

*/

static real dist(int dim, real *x, real *y){
  int k;
  real d = 0;
  for (k = 0; k < dim; k++) d += (x[k] - y[k])*(x[k]-y[k]);
  return sqrt(d);
}

real point_line_distance(real *p, real *q, real *r){
  /* distance between point p and line q--r */
  enum {dim = 2};
  real t = 0, b = 0;
  int i;
  real tmp;
  
  /*   t = ((p - q).(r - q))/((r - q).(r - q)) gives the position of the project of p on line r--q */
  for (i = 0; i < dim; i++){
    t += (p[i] - q[i])*(r[i] - q[i]);
    b += (r[i] - q[i])*(r[i] - q[i]);
  }
  if (b <= MACHINEACC) return dist(dim, p, q);
  t = t/b;

  /*    pointLine = Norm[p - (q + t (r - q))]; 
	If [t >= 0 && t <= 1, pointLine, Min[{Norm[p - q], Norm[p - r]}]]];
  */

  if (t >= 0 && t <= 1){
    b = 0;
    for (i = 0; i < dim; i++){
      tmp = p[i] - (q[i] + t*(r[i] - q[i]));
      b += tmp*tmp;
    }
    return sqrt(b);
  } 
  t = dist(dim, p, q);
  b = dist(dim, p, r);
  return MIN(t, b);

}

static real line_segments_distance(real *p1, real *p2, real *q1, real *q2){
  /* distance between line segments p1--p2 and q1--q2 */
  real t1, t2, t3, t4;
  t1 = point_line_distance(p1, q1, q2);
  t2 = point_line_distance(p2, q1, q2);
  t3 = point_line_distance(q1, p1, p2);
  t4 = point_line_distance(q2, p1, p2);
  t1 = MIN(t1,t2);
  t3 = MIN(t3,t4);
  return MIN(t1, t3);
}
 

real intersection_angle(real *p1, real *p2, real *q1, real *q2){

  /* give two lines p1--p2 and q1--q2, find their intersection agle
     and return Abs[Cos[theta]] of that angle. 
     - If the two lines are very close, treat as if they intersect.
     - If they do not intersect or being very close, return -2.
     - If the return value is close to 1, the two lines intersects and is close to an angle of 0 o Pi;
     .  lines that intersect at close to 90 degree give return value close to 0
     - in the special case of two lines sharing an end point, we return Cos[theta], instead of 
     . the absolute value, where theta
     . is the angle of the two rays emitting from the shared end point, thus the value can be
     . from -1 to 1.
 */
  enum {dim = 2};
  real r[dim], s[dim], qp[dim];
  real rnorm = 0, snorm = 0, b, t, u;
  // real epsilon = sqrt(MACHINEACC), close = 0.01;
  //this may be better. Apply to ngk10_4 and look at double edge between 28 and 43.  real epsilon = sin(10/180.), close = 0.1;
  real epsilon = sin(1/180.), close = 0.01;
  int line_dist_close;
  int i;
  real res;

  for (i = 0; i < dim; i++) {
    r[i] = p2[i] - p1[i];
    rnorm += r[i]*r[i];
  }
  rnorm = sqrt(rnorm);

  for (i = 0; i < dim; i++) {
    s[i] = q2[i] - q1[i];
    snorm += s[i]*s[i];
  }
  snorm = sqrt(snorm);
  b = cross(r, s);
  line_dist_close = (line_segments_distance(p1, p2, q1, q2)  <= close*MAX(rnorm, snorm));
  if (ABS(b) <= epsilon*snorm*rnorm){/* parallel */
    if (line_dist_close) {/* two parallel lines that are close */
      return 1;
    }
    return -2;/* parallel but not close */
  }
  for (i = 0; i < dim; i++) qp[i] = q1[i] - p1[i];
  t = cross(qp, s)/b;
  u = cross(qp, r)/b;
  if ((t >= 0 && t <= 1 && u >= 0 && u <= 1) /* they intersect */
      || line_dist_close){/* or lines are close */
    real rs = 0;
    if (rnorm*snorm < MACHINEACC) return 0;
    for (i = 0; i < dim; i++){
      rs += r[i]*s[i];
    }
    res = rs/(rnorm*snorm);
    /* if the two lines share an end point */
    if (p1[0] == q1[0] && p1[1] == q1[1]){
      return res;
    } else if (p1[0] == q2[0] && p1[1] == q2[1]){
      return -res;
    } else if (p2[0] == q1[0] && p2[1] == q1[1]){
      return -res;
    } else if (p2[0] == q2[0] && p2[1] == q2[1]){
      return res;
    }

    /* normal case of intersect or very close */
    return ABS(res);
  }
  return -2;/* no intersection, and lines are not even close */
}
 
