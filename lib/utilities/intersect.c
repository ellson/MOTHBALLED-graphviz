#include <math.h>

typedef struct {
    double x, y;
} pointf;

#define SMALL 0.0000000001

static pointf
subPt (pointf a, pointf b)
{
    pointf c;

    c.x = a.x-b.x;
    c.y = a.y-b.y;
    return c;
}

static pointf
perp (pointf a)
{
    pointf c;

    c.x = -a.y;
    c.y = a.x;
    return c;
}

static double
dot (pointf a, pointf b)
{
    return (a.x*b.x + a.y*b.y);
}

static pointf
scale (double c, pointf a)
{
    pointf b;

    b.x = c*a.x;
    b.y = c*a.y;
    return b;
}

/* intersect:
 * Computes intersection of lines a-b and c-d, returning intersection
 * point in *x.
 * Returns 0 if no intersection (lines parallel), 1 otherwise.
 */
int
intersect (pointf a, pointf b, pointf c, pointf d, pointf* x)
{
    pointf mv = subPt (b,a);
    pointf lv = subPt (d,c);
    pointf ln = perp (lv);
    double lc = -dot(ln,c);
    double dt = dot(ln,mv);

    if (fabs(dt) < SMALL) return 0;

    *x = subPt(a,scale((dot(ln,a)+lc)/dt,mv));
    return 1;
}

#ifdef DEBUG
#include <stdio.h>

main ()
{
    pointf a, b, c, d, x;
    int ax, ay, bx, by, cx, cy, dx, dy;
    char buf[1024];
    while (1) {
	  printf ("> "); fflush(stdin);
      fgets (buf, 1024, stdin);
      sscanf (buf, "%d %d %d %d", &ax, &ay, &bx, &by);
	  printf ("> "); fflush(stdin);
      fgets (buf, 1024, stdin);
      sscanf (buf, "%d %d %d %d", &cx, &cy, &dx, &dy);
      a.x = ax;
      a.y = ay;
      b.x = bx;
      b.y = by;
      c.x = cx;
      c.y = cy;
      d.x = dx;
      d.y = dy;
      if (intersect (a,b,c,d,&x))
		printf ("(%f,%f)\n", x.x,x.y);
      else
		printf ("no intersection\n");
      }
}
#endif
