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

inline point pointof(int x, int y)
{
    point r;

    r.x = x;
    r.y = y;
    return r;
}

inline pointf pointfof(double x, double y)
{
    pointf r;

    r.x = x;
    r.y = y;
    return r;
}

inline point cvt2pt(pointf p)
{
    point r;

    r.x = POINTS(p.x);
    r.y = POINTS(p.y);
    return r;
}

inline pointf cvt2ptf(point p)
{
    pointf r;

    r.x = PS2INCH(p.x);
    r.y = PS2INCH(p.y);
    return r;
}

inline box boxof(int llx, int lly, int urx, int ury)
{
    box b;

    b.LL.x = llx, b.LL.y = lly;
    b.UR.x = urx, b.UR.y = ury;
    return b;
}

inline boxf boxfof(double llx, double lly, double urx, double ury)
{
    boxf b;

    b.LL.x = llx, b.LL.y = lly;
    b.UR.x = urx, b.UR.y = ury;
    return b;
}

box mkbox(point p, point q)
{
    box r;

    if (p.x < q.x) {
	r.LL.x = p.x;
	r.UR.x = q.x;
    } else {
	r.LL.x = q.x;
	r.UR.x = p.x;
    }
    if (p.y < q.y) {
	r.LL.y = p.y;
	r.UR.y = q.y;
    } else {
	r.LL.y = q.y;
	r.UR.y = p.y;
    }
    return r;
}

boxf mkboxf(pointf p, pointf q)
{
    boxf r;

    if (p.x < q.x) {
	r.LL.x = p.x;
	r.UR.x = q.x;
    } else {
	r.LL.x = q.x;
	r.UR.x = p.x;
    }
    if (p.y < q.y) {
	r.LL.y = p.y;
	r.UR.y = q.y;
    } else {
	r.LL.y = q.y;
	r.UR.y = p.y;
    }
    return r;
}

inline point add_point(point p, point q)
{
    point r;

    r.x = p.x + q.x;
    r.y = p.y + q.y;
    return r;
}

inline pointf add_pointf(pointf p, pointf q)
{
    pointf r;

    r.x = p.x + q.x;
    r.y = p.y + q.y;
    return r;
}

inline point sub_point(point p, point q)
{
    point r;

    r.x = p.x - q.x;
    r.y = p.y - q.y;
    return r;
}

inline pointf sub_pointf(pointf p, pointf q)
{
    pointf r;

    r.x = p.x - q.x;
    r.y = p.y - q.y;
    return r;
}

/* for +ve coord values, this rounds towards p */
inline point mid_point(point p, point q)
{
    point r;

    r.x = (p.x + q.x) / 2;
    r.y = (p.y + q.y) / 2;
    return r;
}

inline pointf mid_pointf(pointf p, pointf q)
{
    pointf r;

    r.x = (p.x + q.x) / 2.;
    r.y = (p.y + q.y) / 2.;
    return r;
}

inline pointf interpolate(double t, pointf p, pointf q)
{
    pointf r; 

    r.x = p.x + t * (q.x - p.x);
    r.y = p.y + t * (q.y - p.y);
    return r;
}

inline point exch_xy(point p)
{
    point r;

    r.x = p.y;
    r.y = p.x;
    return r;
}

inline pointf exch_xyf(pointf p)
{
    pointf r;

    r.x = p.y;
    r.y = p.x;
    return r;
}

inline box box_bb(box b0, box b1)
{
    box b;

    b.LL.x = MIN(b0.LL.x, b1.LL.x);
    b.LL.y = MIN(b0.LL.y, b1.LL.y);
    b.UR.x = MAX(b0.UR.x, b1.UR.x);
    b.UR.y = MAX(b0.UR.y, b1.UR.y);

    return b;
}

inline boxf boxf_bb(boxf b0, boxf b1)
{
    boxf b;

    b.LL.x = MIN(b0.LL.x, b1.LL.x);
    b.LL.y = MIN(b0.LL.y, b1.LL.y);
    b.UR.x = MAX(b0.UR.x, b1.UR.x);
    b.UR.y = MAX(b0.UR.y, b1.UR.y);

    return b;
}

inline box box_intersect(box b0, box b1)
{
    box b;

    b.LL.x = MAX(b0.LL.x, b1.LL.x);
    b.LL.y = MAX(b0.LL.y, b1.LL.y);
    b.UR.x = MIN(b0.UR.x, b1.UR.x);
    b.UR.y = MIN(b0.UR.y, b1.UR.y);

    return b;
}

inline boxf boxf_intersect(boxf b0, boxf b1)
{
    boxf b;

    b.LL.x = MAX(b0.LL.x, b1.LL.x);
    b.LL.y = MAX(b0.LL.y, b1.LL.y);
    b.UR.x = MIN(b0.UR.x, b1.UR.x);
    b.UR.y = MIN(b0.UR.y, b1.UR.y);

    return b;
}

inline int box_overlap(box b0, box b1)
{
    return OVERLAP(b0, b1);
}

inline int boxf_overlap(boxf b0, boxf b1)
{
    return OVERLAP(b0, b1);
}

inline int box_contains(box b0, box b1)
{
    return CONTAINS(b0, b1);
}

inline int boxf_contains(boxf b0, boxf b1)
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
 *      -1 is returned if the line given by p and q
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

int lineToBox(pointf p, pointf q, boxf b)
{
    int inside1, inside2;

    /*
     * First check the two points individually to see whether they
     * are inside the rectangle or not.
     */

    inside1 = (p.x >= b.LL.x) && (p.x <= b.UR.x)
            && (p.y >= b.LL.y) && (p.y <= b.UR.y);
    inside2 = (q.x >= b.LL.x) && (q.x <= b.UR.x)
            && (q.y >= b.LL.y) && (q.y <= b.UR.y);
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

    if (p.x == q.x) {
        /*
         * Vertical line.
         */

        if (((p.y >= b.LL.y) ^ (q.y >= b.LL.y))
                && (p.x >= b.LL.x)
                && (p.x <= b.UR.x)) {
            return 0;
        }
    } else if (p.y == q.y) {
        /*
         * Horizontal line.
         */
        if (((p.x >= b.LL.x) ^ (q.x >= b.LL.x))
                && (p.y >= b.LL.y)
                && (p.y <= b.UR.y)) {
            return 0;
        }
    } else {
        double m, x, y, low, high;

        /*
         * Diagonal line.  Compute slope of line and use
         * for intersection checks against each of the
         * sides of the rectangle: left, right, bottom, top.
         */

        m = (q.y - p.y)/(q.x - p.x);
        if (p.x < q.x) {
            low = p.x;  high = q.x;
        } else {
            low = q.x; high = p.x;
        }

        /*
         * Left edge.
         */

        y = p.y + (b.LL.x - p.x)*m;
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

        if (p.y < q.y) {
            low = p.y;  high = q.y;
        } else {
            low = q.y; high = p.y;
        }
        x = p.x + (b.LL.y - p.y)/m;
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

inline void rect2poly(pointf *p)
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

inline box flip_rec_box(box b, point p)
{
    box r;
    /* flip box */
    r.UR.x = b.UR.y;
    r.UR.y = b.UR.x;
    r.LL.x = b.LL.y;
    r.LL.y = b.LL.x;
    /* move box */
    r.LL.x += p.x;
    r.LL.y += p.y;
    r.UR.x += p.x;
    r.UR.y += p.y;
    return r;
}

inline boxf flip_rec_boxf(boxf b, pointf p)
{
    boxf r;
    /* flip box */
    r.UR.x = b.UR.y;
    r.UR.y = b.UR.x;
    r.LL.x = b.LL.y;
    r.LL.y = b.LL.x;
    /* move box */
    r.LL.x += p.x;
    r.LL.y += p.y;
    r.UR.x += p.x;
    r.UR.y += p.y;
    return r;
}

#define SMALL 0.0000000001

/* ptToLine2:
 * Return distance from point p to line a-b squared.
 */
double ptToLine2 (pointf a, pointf b, pointf p)
{
  double dx = b.x-a.x;
  double dy = b.y-a.y;
  double a2 = (p.y-a.y)*dx - (p.x-a.x)*dy;
  a2 *= a2;   /* square - ensures that it is positive */
  if (a2 < SMALL) return 0.;  /* avoid 0/0 problems */
  return a2 / (dx*dx + dy*dy);
}

inline pointf perp (pointf p)
{
    pointf r;

    r.x = -p.y;
    r.y = p.x;
    return r;
}

#define dot(v,w) (v.x*w.x+v.y*w.y)

inline pointf scale (double c, pointf p)
{
    pointf r;

    r.x = c * p.x;
    r.y = c * p.y;
    return r;
}

/* intersect:
 * Computes intersection of lines a-b and c-d, returning intersection
 * point in *p.
 * Returns 0 if no intersection (lines parallel), 1 otherwise.
 */
int seg_intersect (pointf a, pointf b, pointf c, pointf d, pointf* p)
{
    pointf mv = sub_pointf(b,a);
    pointf lv = sub_pointf(d,c);
    pointf ln = perp (lv);
    double lc = -dot(ln,c);
    double dt = dot(ln,mv);

    if (fabs(dt) < SMALL) return 0;

    *p = sub_pointf(a,scale((dot(ln,a)+lc)/dt,mv));
    return 1;
}

