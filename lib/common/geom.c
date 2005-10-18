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

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "types.h"
#include "macros.h"
#include "const.h"
#include "geom.h"

point pointof(int x, int y)
{
    point rv;
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

/* from Glassner's Graphics Gems */
#define W_DEGREE 5

/*
 *  Bezier : 
 *	Evaluate a Bezier curve at a particular parameter value
 *      Fill in control points for resulting sub-curves if "Left" and
 *	"Right" are non-null.
 * 
 */
pointf Bezier(pointf * V, int degree, double t, pointf * Left,
	      pointf * Right)
{
    int i, j;			/* Index variables      */
    pointf Vtemp[W_DEGREE + 1][W_DEGREE + 1];

    /* Copy control points  */
    for (j = 0; j <= degree; j++) {
	Vtemp[0][j] = V[j];
    }

    /* Triangle computation */
    for (i = 1; i <= degree; i++) {
	for (j = 0; j <= degree - i; j++) {
	    Vtemp[i][j].x =
		(1.0 - t) * Vtemp[i - 1][j].x + t * Vtemp[i - 1][j + 1].x;
	    Vtemp[i][j].y =
		(1.0 - t) * Vtemp[i - 1][j].y + t * Vtemp[i - 1][j + 1].y;
	}
    }

    if (Left != NULL)
	for (j = 0; j <= degree; j++)
	    Left[j] = Vtemp[j][0];
    if (Right != NULL)
	for (j = 0; j <= degree; j++)
	    Right[j] = Vtemp[degree - j][j];

    return (Vtemp[degree][0]);
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

boolean box_overlap(box b0, box b1)
{
    return OVERLAP(b0, b1);
}

boolean boxf_overlap(boxf b0, boxf b1)
{
    return OVERLAP(b0, b1);
}

boolean box_contains(box b0, box b1)
{
    return CONTAINS(b0, b1);
}

boolean boxf_contains(boxf b0, boxf b1)
{
    return CONTAINS(b0, b1);
}

point closest_spline_point(splines * spl, point p)
{
    int i, j, k, besti, bestj;
    double bestdist2, d2, dlow2, dhigh2; /* squares of distances */
    double low, high, t;
    pointf c[4], pt2, pt;
    point rv;
    bezier bz;

    besti = bestj = -1;
    bestdist2 = 1e+38;
    P2PF(p, pt);
    for (i = 0; i < spl->size; i++) {
	bz = spl->list[i];
	for (j = 0; j < bz.size; j++) {
	    pointf b;

	    b.x = bz.list[j].x;
	    b.y = bz.list[j].y;
	    d2 = DIST2(b, pt);
	    if ((bestj == -1) || (d2 < bestdist2)) {
		besti = i;
		bestj = j;
		bestdist2 = d2;
	    }
	}
    }

    bz = spl->list[besti];
    j = bestj / 3;
    if (j >= spl->size)
	j--;
    for (k = 0; k < 4; k++) {
	c[k].x = bz.list[j + k].x;
	c[k].y = bz.list[j + k].y;
    }
    low = 0.0;
    high = 1.0;
    dlow2 = DIST2(c[0], pt);
    dhigh2 = DIST2(c[3], pt);
    do {
	t = (low + high) / 2.0;
	pt2 = Bezier(c, 3, t, NULL, NULL);
	if (fabs(dlow2 - dhigh2) < 1.0)
	    break;
	if (fabs(high - low) < .00001)
	    break;
	if (dlow2 < dhigh2) {
	    high = t;
	    dhigh2 = DIST2(pt2, pt);
	} else {
	    low = t;
	    dlow2 = DIST2(pt2, pt);
	}
    } while (1);
    PF2P(pt2, rv);
    return rv;
}

point spline_at_y(splines * spl, int y)
{
    int i, j;
    double low, high, d, t;
    pointf c[4], pt2;
    point pt;
    static bezier bz;

/* this caching seems to prevent pt.x from getting set from bz.list[0].x
	- optimizer problem ? */

#if 0
    static splines *mem = NULL;

    if (mem != spl) {
	mem = spl;
#endif
	for (i = 0; i < spl->size; i++) {
	    bz = spl->list[i];
	    if (BETWEEN(bz.list[bz.size - 1].y, y, bz.list[0].y))
		break;
	}
#if 0
    }
#endif
    if (y > bz.list[0].y)
	pt = bz.list[0];
    else if (y < bz.list[bz.size - 1].y)
	pt = bz.list[bz.size - 1];
    else {
	for (i = 0; i < bz.size; i += 3) {
	    for (j = 0; j < 3; j++) {
		if ((bz.list[i + j].y <= y) && (y <= bz.list[i + j + 1].y))
		    break;
		if ((bz.list[i + j].y >= y) && (y >= bz.list[i + j + 1].y))
		    break;
	    }
	    if (j < 3)
		break;
	}
	assert(i < bz.size);
	for (j = 0; j < 4; j++) {
	    c[j].x = bz.list[i + j].x;
	    c[j].y = bz.list[i + j].y;
	    /* make the spline be monotonic in Y, awful but it works for now */
	    if ((j > 0) && (c[j].y > c[j - 1].y))
		c[j].y = c[j - 1].y;
	}
	low = 0.0;
	high = 1.0;
	do {
	    t = (low + high) / 2.0;
	    pt2 = Bezier(c, 3, t, NULL, NULL);
	    d = pt2.y - y;
	    if (ABS(d) <= 1)
		break;
	    if (d < 0)
		high = t;
	    else
		low = t;
	} while (1);
	pt.x = pt2.x;
	pt.y = pt2.y;
    }
    pt.y = y;
    return pt;
}

point neato_closest(splines * spl, point p)
{
/* this is a stub so that we can share a common emit.c between dot and neato */

    return spline_at_y(spl, p.y);
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

/* flip_ptf:
 * Transform point =>
 *  LR - rotate ccw by 90
 *  BT - reflect across x axis
 *  RL - TB o LR
 */
point flip_pt(point p, int rankdir)
{
    int x = p.x, y = p.y;
    switch (rankdir) {
    case RANKDIR_TB:
	break;
    case RANKDIR_LR:
	p.x = -y;
	p.y = x;
	break;
    case RANKDIR_BT:
	p.x = x;
	p.y = -y;
	break;
    case RANKDIR_RL:
	p.x = y;
	p.y = x;
	break;
    }
    return p;
}

/* flip_ptf:
 * flip_pt for pointf type.
 */
pointf flip_ptf(pointf p, int rankdir)
{
    double x = p.x, y = p.y;
    switch (rankdir) {
    case RANKDIR_TB:
	break;
    case RANKDIR_LR:
	p.x = -y;
	p.y = x;
	break;
    case RANKDIR_BT:
	p.x = x;
	p.y = -y;
	break;
    case RANKDIR_RL:
	p.x = y;
	p.y = x;
	break;
    }
    return p;
}

/* invflip_pt:
 * Transform point =>
 *  LR - rotate cw by 90
 *  BT - reflect across x axis
 *  RL - TB o LR
 * Note that flip and invflip only differ on LR
 */
point invflip_pt(point p, int rankdir)
{
    int x = p.x, y = p.y;
    switch (rankdir) {
    case RANKDIR_TB:
	break;
    case RANKDIR_LR:
	p.x = y;
	p.y = -x;
	break;
    case RANKDIR_BT:
	p.x = x;
	p.y = -y;
	break;
    case RANKDIR_RL:
	p.x = y;
	p.y = x;
	break;
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
