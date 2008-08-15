/* $Id$ $Revision$ */
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

/* geometric functions (e.g. on points and boxes) with application to, but
 * no specific dependance on graphs */

#include "geom.h"
#include "geomprocs.h"

point pointof(int x, int y)
{
    point rv;
    rv.x = x, rv.y = y;
    return rv;
}

pointf pointfof(double x, double y)
{
    pointf rv;
    rv.x = x, rv.y = y;
    return rv;
}

point cvt2pt(pointf p)
{
    point rv;
    rv.x = POINTS(p.x);
    rv.y = POINTS(p.y);
    return rv;
}

pointf cvt2ptf(point p)
{
    pointf rv;
    rv.x = PS2INCH(p.x);
    rv.y = PS2INCH(p.y);
    return rv;
}

box boxof(int llx, int lly, int urx, int ury)
{
    box b;

    b.LL.x = llx, b.LL.y = lly;
    b.UR.x = urx, b.UR.y = ury;
    return b;
}

boxf boxfof(double llx, double lly, double urx, double ury)
{
    boxf b;

    b.LL.x = llx, b.LL.y = lly;
    b.UR.x = urx, b.UR.y = ury;
    return b;
}

box mkbox(point p0, point p1)
{
    box rv;

    if (p0.x < p1.x) {
	rv.LL.x = p0.x;
	rv.UR.x = p1.x;
    } else {
	rv.LL.x = p1.x;
	rv.UR.x = p0.x;
    }
    if (p0.y < p1.y) {
	rv.LL.y = p0.y;
	rv.UR.y = p1.y;
    } else {
	rv.LL.y = p1.y;
	rv.UR.y = p0.y;
    }
    return rv;
}

boxf mkboxf(pointf p0, pointf p1)
{
    boxf rv;

    if (p0.x < p1.x) {
	rv.LL.x = p0.x;
	rv.UR.x = p1.x;
    } else {
	rv.LL.x = p1.x;
	rv.UR.x = p0.x;
    }
    if (p0.y < p1.y) {
	rv.LL.y = p0.y;
	rv.UR.y = p1.y;
    } else {
	rv.LL.y = p1.y;
	rv.UR.y = p0.y;
    }
    return rv;
}

point add_points(point p0, point p1)
{
    p0.x += p1.x;
    p0.y += p1.y;
    return p0;
}

point sub_points(point p0, point p1)
{
    p0.x -= p1.x;
    p0.y -= p1.y;
    return p0;
}

pointf add_pointfs(pointf p0, pointf p1)
{
    p0.x += p1.x;
    p0.y += p1.y;
    return p0;
}

pointf sub_pointfs(pointf p0, pointf p1)
{
    p0.x -= p1.x;
    p0.y -= p1.y;
    return p0;
}

point exch_xy(point p)
{
    int t;
    t = p.x;
    p.x = p.y;
    p.y = t;
    return p;
}

pointf exch_xyf(pointf p)
{
    double t;
    t = p.x;
    p.x = p.y;
    p.y = t;
    return p;
}

box box_bb(box b0, box b1)
{
    box b;

    b.LL.x = MIN(b0.LL.x, b1.LL.x);
    b.LL.y = MIN(b0.LL.y, b1.LL.y);
    b.UR.x = MAX(b0.UR.x, b1.UR.x);
    b.UR.y = MAX(b0.UR.y, b1.UR.y);

    return b;
}

boxf boxf_bb(boxf b0, boxf b1)
{
    boxf b;

    b.LL.x = MIN(b0.LL.x, b1.LL.x);
    b.LL.y = MIN(b0.LL.y, b1.LL.y);
    b.UR.x = MAX(b0.UR.x, b1.UR.x);
    b.UR.y = MAX(b0.UR.y, b1.UR.y);

    return b;
}

box box_intersect(box b0, box b1)
{
    box b;

    b.LL.x = MAX(b0.LL.x, b1.LL.x);
    b.LL.y = MAX(b0.LL.y, b1.LL.y);
    b.UR.x = MIN(b0.UR.x, b1.UR.x);
    b.UR.y = MIN(b0.UR.y, b1.UR.y);

    return b;
}

boxf boxf_intersect(boxf b0, boxf b1)
{
    boxf b;

    b.LL.x = MAX(b0.LL.x, b1.LL.x);
    b.LL.y = MAX(b0.LL.y, b1.LL.y);
    b.UR.x = MIN(b0.UR.x, b1.UR.x);
    b.UR.y = MIN(b0.UR.y, b1.UR.y);

    return b;
}

int box_overlap(box b0, box b1)
{
    return OVERLAP(b0, b1);
}

int boxf_overlap(boxf b0, boxf b1)
{
    return OVERLAP(b0, b1);
}

int box_contains(box b0, box b1)
{
    return CONTAINS(b0, b1);
}

int boxf_contains(boxf b0, boxf b1)
{
    return CONTAINS(b0, b1);
}

/*
 *--------------------------------------------------------------
 *
 * lineToBox --
 *
 *      Determine whether a line lies entirely inside, entirely
 *      outside, or overlapping a given rectangular area.
 *
 * Results:
 *      -1 is returned if the line given by p1 and p2
 *      is entirely outside the rectangle given by b.
 * 	0 is returned if the polygon overlaps the rectangle, and
 *	1 is returned if the polygon is entirely inside the rectangle.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

/* This code steals liberally from algorithms in tk/generic/tkTrig.c -- jce */

int lineToBox(pointf p1, pointf p2, boxf b)
{
    int inside1, inside2;

    /*
     * First check the two points individually to see whether they
     * are inside the rectangle or not.
     */

    inside1 = (p1.x >= b.LL.x) && (p1.x <= b.UR.x)
            && (p1.y >= b.LL.y) && (p1.y <= b.UR.y);
    inside2 = (p2.x >= b.LL.x) && (p2.x <= b.UR.x)
            && (p2.y >= b.LL.y) && (p2.y <= b.UR.y);
    if (inside1 != inside2) {
        return 0;
    }
    if (inside1 & inside2) {
        return 1;
    }

    /*
     * Both points are outside the rectangle, but still need to check
     * for intersections between the line and the rectangle.  Horizontal
     * and vertical lines are particularly easy, so handle them
     * separately.
     */

    if (p1.x == p2.x) {
        /*
         * Vertical line.
         */

        if (((p1.y >= b.LL.y) ^ (p2.y >= b.LL.y))
                && (p1.x >= b.LL.x)
                && (p1.x <= b.UR.x)) {
            return 0;
        }
    } else if (p1.y == p2.y) {
        /*
         * Horizontal line.
         */
        if (((p1.x >= b.LL.x) ^ (p2.x >= b.LL.x))
                && (p1.y >= b.LL.y)
                && (p1.y <= b.UR.y)) {
            return 0;
        }
    } else {
        double m, x, y, low, high;

        /*
         * Diagonal line.  Compute slope of line and use
         * for intersection checks against each of the
         * sides of the rectangle: left, right, bottom, top.
         */

        m = (p2.y - p1.y)/(p2.x - p1.x);
        if (p1.x < p2.x) {
            low = p1.x;  high = p2.x;
        } else {
            low = p2.x; high = p1.x;
        }

        /*
         * Left edge.
         */

        y = p1.y + (b.LL.x - p1.x)*m;
        if ((b.LL.x >= low) && (b.LL.x <= high)
                && (y >= b.LL.y) && (y <= b.UR.y)) {
            return 0;
        }

        /*
         * Right edge.
         */

        y += (b.UR.x - b.LL.x)*m;
        if ((y >= b.LL.y) && (y <= b.UR.y)
                && (b.UR.x >= low) && (b.UR.x <= high)) {
            return 0;
        }

        /*
         * Bottom edge.
         */

        if (p1.y < p2.y) {
            low = p1.y;  high = p2.y;
        } else {
            low = p2.y; high = p1.y;
        }
        x = p1.x + (b.LL.y - p1.y)/m;
        if ((x >= b.LL.x) && (x <= b.UR.x)
                && (b.LL.y >= low) && (b.LL.y <= high)) {
            return 0;
        }

        /*
         * Top edge.
         */

        x += (b.UR.y - b.LL.y)/m;
        if ((x >= b.LL.x) && (x <= b.UR.x)
                && (b.UR.y >= low) && (b.UR.y <= high)) {
            return 0;
        }
    }
    return -1;
}

void rect2poly(pointf *p)
{
    p[3].x = p[2].x = p[1].x;
    p[2].y = p[1].y;
    p[3].y = p[0].y;
    p[1].x = p[0].x;
}

static pointf rotatepf(pointf p, int cwrot)
{
    static double sina, cosa;
    static int last_cwrot;
    pointf P;

    /* cosa is initially wrong for a cwrot of 0
     * this caching only works because we are never called for 0 rotations */
    if (cwrot != last_cwrot) {
	sincos(cwrot / (2 * M_PI), &sina, &cosa);
	last_cwrot = cwrot;
    }
    P.x = p.x * cosa - p.y * sina;
    P.y = p.y * cosa + p.x * sina;
    return P;
}

static point rotatep(point p, int cwrot)
{
    pointf pf;

    P2PF(p, pf);
    pf = rotatepf(pf, cwrot);
    PF2P(pf, p);
    return p;
}

point cwrotatep(point p, int cwrot)
{
    int x = p.x, y = p.y;
    switch (cwrot) {
    case 0:
	break;
    case 90:
	p.x = y;
	p.y = -x;
	break;
    case 180:
	p.x = x;
	p.y = -y;
	break;
    case 270:
	p.x = y;
	p.y = x;
	break;
    default:
	if (cwrot < 0)
	    return ccwrotatep(p, -cwrot);
        if (cwrot > 360)
	    return cwrotatep(p, cwrot%360);
	return rotatep(p, cwrot);
    }
    return p;
}

pointf cwrotatepf(pointf p, int cwrot)
{
    double x = p.x, y = p.y;
    switch (cwrot) {
    case 0:
	break;
    case 90:
	p.x = y;
	p.y = -x;
	break;
    case 180:
	p.x = x;
	p.y = -y;
	break;
    case 270:
	p.x = y;
	p.y = x;
	break;
    default:
	if (cwrot < 0)
	    return ccwrotatepf(p, -cwrot);
        if (cwrot > 360)
	    return cwrotatepf(p, cwrot%360);
	return rotatepf(p, cwrot);
    }
    return p;
}

point ccwrotatep(point p, int ccwrot)
{
    int x = p.x, y = p.y;
    switch (ccwrot) {
    case 0:
	break;
    case 90:
	p.x = -y;
	p.y = x;
	break;
    case 180:
	p.x = x;
	p.y = -y;
	break;
    case 270:
	p.x = y;
	p.y = x;
	break;
    default:
	if (ccwrot < 0)
	    return cwrotatep(p, -ccwrot);
        if (ccwrot > 360)
	    return ccwrotatep(p, ccwrot%360);
	return rotatep(p, 360-ccwrot);
    }
    return p;
}

pointf ccwrotatepf(pointf p, int ccwrot)
{
    double x = p.x, y = p.y;
    switch (ccwrot) {
    case 0:
	break;
    case 90:
	p.x = -y;
	p.y = x;
	break;
    case 180:
	p.x = x;
	p.y = -y;
	break;
    case 270:
	p.x = y;
	p.y = x;
	break;
    default:
	if (ccwrot < 0)
	    return cwrotatepf(p, -ccwrot);
        if (ccwrot > 360)
	    return ccwrotatepf(p, ccwrot%360);
	return rotatepf(p, 360-ccwrot);
    }
    return p;
}

box flip_rec_box(box b, point p)
{
    box rv;
    /* flip box */
    rv.UR.x = b.UR.y;
    rv.UR.y = b.UR.x;
    rv.LL.x = b.LL.y;
    rv.LL.y = b.LL.x;
    /* move box */
    rv.LL.x += p.x;
    rv.LL.y += p.y;
    rv.UR.x += p.x;
    rv.UR.y += p.y;
    return rv;
}

boxf flip_rec_boxf(boxf b, pointf p)
{
    boxf rv;
    /* flip box */
    rv.UR.x = b.UR.y;
    rv.UR.y = b.UR.x;
    rv.LL.x = b.LL.y;
    rv.LL.y = b.LL.x;
    /* move box */
    rv.LL.x += p.x;
    rv.LL.y += p.y;
    rv.UR.x += p.x;
    rv.UR.y += p.y;
    return rv;
}

/* ptToLine2:
 * Return distance from point p to line a-b squared.
 */
double ptToLine2 (pointf a, pointf b, pointf p)
{
  double dx = b.x-a.x;
  double dy = b.y-a.y;
  double a2 = (p.y-a.y)*dx - (p.x-a.x)*dy;
  a2 *= a2;   /* square - ensures that it is positive */
  if (a2 < .00001) return 0.;  /* avoid 0/0 problems */
  return a2 / (dx*dx + dy*dy);
}

#define SMALL 0.0000000001

pointf
addPt (pointf a, pointf b)
{
    pointf c;

    c.x = a.x+b.x;
    c.y = a.y+b.y;
    return c;
}

pointf
subPt (pointf a, pointf b)
{
    pointf c;

    c.x = a.x-b.x;
    c.y = a.y-b.y;
    return c;
}

pointf
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

pointf
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
seg_intersect (pointf a, pointf b, pointf c, pointf d, pointf* x)
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

