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


/* Module for clipping splines to cluster boxes.
 */

#include	"dot.h"


/* midPt:
 * Return midpoint between two given points.
 */
static point midPt(point p, point q)
{
    point v;

    v.x = (p.x + q.x) / 2;
    v.y = (p.y + q.y) / 2;
    return v;
}

/* p2s:
 * Convert a point to its string representation.
 */
static char *p2s(point p, char *buf)
{
    sprintf(buf, "(%d,%d)", p.x, p.y);
    return buf;
}

/* Return point where line segment [pp,cp] intersects
 * the box bp. Assume cp is outside the box, and pp is
 * on or in the box. 
 */
static point boxIntersect(point pp, point cp, box * bp)
{
    point ipp;
    double ppx = pp.x;
    double ppy = pp.y;
    double cpx = cp.x;
    double cpy = cp.y;
    point ll = bp->LL;
    point ur = bp->UR;

    if (cp.x < ll.x) {
	ipp.x = ll.x;
	ipp.y = pp.y + (int) ((ipp.x - ppx) * (ppy - cpy) / (ppx - cpx));
	if (ipp.y >= ll.y && ipp.y <= ur.y)
	    return ipp;
    }
    if (cp.x > ur.x) {
	ipp.x = ur.x;
	ipp.y = pp.y + (int) ((ipp.x - ppx) * (ppy - cpy) / (ppx - cpx));
	if (ipp.y >= ll.y && ipp.y <= ur.y)
	    return ipp;
    }
    if (cp.y < ll.y) {
	ipp.y = ll.y;
	ipp.x = pp.x + (int) ((ipp.y - ppy) * (ppx - cpx) / (ppy - cpy));
	if (ipp.x >= ll.x && ipp.x <= ur.x)
	    return ipp;
    }
    if (cp.y > ur.y) {
	ipp.y = ur.y;
	ipp.x = pp.x + (int) ((ipp.y - ppy) * (ppx - cpx) / (ppy - cpy));
	if (ipp.x >= ll.x && ipp.x <= ur.x)
	    return ipp;
    }

    /* failure */
    {
	char ppbuf[100], cpbuf[100], llbuf[100], urbuf[100];

	agerr(AGERR,
	      "segment [%s,%s] does not intersect box ll=%s,ur=%s\n",
	      p2s(pp, ppbuf), p2s(cp, cpbuf), p2s(ll, llbuf), p2s(ur,
								  urbuf));
	assert(0);
    }
    return ipp;
}

/* inBox:
 * Returns true if p is on or in box bb
 */
static int inBox(point p, box * bb)
{
    return ((p.x >= bb->LL.x) && (p.x <= bb->UR.x) &&
	    (p.y >= bb->LL.y) && (p.y <= bb->UR.y));
}

/* getCluster:
 * Returns subgraph of g with given name.
 * Returns NULL if no name is given, or subgraph of
 * that name does not exist.
 */
static graph_t *getCluster(graph_t * g, char *cluster_name)
{
    graph_t *sg;

    if (!cluster_name || (*cluster_name == '\0'))
	return NULL;
    sg = agfindsubg(g, cluster_name);
    if (sg == NULL)
	agerr(AGWARN, "cluster named %s not found\n", cluster_name);
    return sg;
}

/* The following functions are derived from pp. 411-415 (pp. 791-795)
 * of Graphics Gems. In the code there, they use a SGN function to
 * count crossings. This doesn't seem to handle certain special cases,
 * as when the last point is on the line. It certainly doesn't work
 * for use; see bug 145. We need to use ZSGN instead.
 */
#define SGN(a,b)          (((a)<(b)) ? -1 : 1)
#define ZSGN(a,b)         (((a)<(b)) ? -1 : (a)>(b) ? 1 : 0)

/* countVertCross:
 * Return the number of times the Bezier control polygon crosses
 * the vertical line x = xcoord.
 */
static int countVertCross(pointf * pts, int xcoord)
{
    int i;
    int sign, old_sign;
    int num_crossings = 0;

    sign = old_sign = ZSGN(pts[0].x, xcoord);
    if (sign == 0)
	num_crossings++;
    for (i = 1; i <= 3; i++) {
	sign = ZSGN(pts[i].x, xcoord);
	if ((sign != old_sign) && (old_sign != 0))
	    num_crossings++;
	old_sign = sign;
    }

    return num_crossings;

}

/* countHorzCross:
 * Return the number of times the Bezier control polygon crosses
 * the horizontal line y = ycoord.
 */
static int countHorzCross(pointf * pts, int ycoord)
{
    int i;
    int sign, old_sign;
    int num_crossings = 0;

    sign = old_sign = ZSGN(pts[0].y, ycoord);
    if (sign == 0)
	num_crossings++;
    for (i = 1; i <= 3; i++) {
	sign = ZSGN(pts[i].y, ycoord);
	if ((sign != old_sign) && (old_sign != 0))
	    num_crossings++;
	old_sign = sign;
    }

    return num_crossings;

}

/* findVertical:
 * Given 4 Bezier control points pts, corresponding to the portion
 * of an initial spline with path parameter in the range
 * 0.0 <= tmin <= t <= tmax <= 1.0, return t where the spline 
 * first crosses a vertical line segment
 * [(xcoord,ymin),(xcoord,ymax)]. Return -1 if not found.
 * This is done by binary subdivision. 
 */
static double
findVertical(pointf * pts, double tmin, double tmax,
	     int xcoord, int ymin, int ymax)
{
    pointf Left[4];
    pointf Right[4];
    double t;
    int no_cross = countVertCross(pts, xcoord);

    if (no_cross == 0)
	return -1.0;

    /* if 1 crossing and on the line x = xcoord */
    if ((no_cross == 1) && (ROUND(pts[3].x) == xcoord)) {
	if ((ymin <= pts[3].y) && (pts[3].y <= ymax)) {
	    return tmax;
	} else
	    return -1.0;
    }

    /* split the Bezier into halves, trying the first half first. */
    Bezier(pts, 3, 0.5, Left, Right);
    t = findVertical(Left, tmin, (tmin + tmax) / 2.0, xcoord, ymin, ymax);
    if (t >= 0.0)
	return t;
    return findVertical(Right, (tmin + tmax) / 2.0, tmax, xcoord, ymin,
			ymax);

}

/* findHorizontal:
 * Given 4 Bezier control points pts, corresponding to the portion
 * of an initial spline with path parameter in the range
 * 0.0 <= tmin <= t <= tmax <= 1.0, return t where the spline 
 * first crosses a horizontal line segment
 * [(xmin,ycoord),(xmax,ycoord)]. Return -1 if not found.
 * This is done by binary subdivision. 
 */
static double
findHorizontal(pointf * pts, double tmin, double tmax,
	       int ycoord, int xmin, int xmax)
{
    pointf Left[4];
    pointf Right[4];
    double t;
    int no_cross = countHorzCross(pts, ycoord);

    if (no_cross == 0)
	return -1.0;

    /* if 1 crossing and on the line y = ycoord */
    if ((no_cross == 1) && (ROUND(pts[3].y) == ycoord)) {
	if ((xmin <= pts[3].x) && (pts[3].x <= xmax)) {
	    return tmax;
	} else
	    return -1.0;
    }

    /* split the Bezier into halves, trying the first half first. */
    Bezier(pts, 3, 0.5, Left, Right);
    t = findHorizontal(Left, tmin, (tmin + tmax) / 2.0, ycoord, xmin,
		       xmax);
    if (t >= 0.0)
	return t;
    return findHorizontal(Right, (tmin + tmax) / 2.0, tmax, ycoord, xmin,
			  xmax);

}

#define P2PF(p, pf) (pf.x = p.x, pf.y = p.y)
#define PF2P(pf, p) (p.x = ROUND (pf.x), p.y = ROUND (pf.y))

/* splineIntersect:
 * Given four spline control points and a box,
 * find the shortest portion of the spline from
 * ipts[0] to the intersection with the box, if any.
 * If an intersection is found, the four points are stored in ipts[0..3]
 * with ipts[3] being on the box, and 1 is returned. Otherwise, ipts
 * is left unchanged and 0 is returned.
 */
static int splineIntersect(point * ipts, box * bb)
{
    double tmin = 2.0;
    double t;
    pointf pts[4];
    pointf origpts[4];
    int i;

    for (i = 0; i < 4; i++) {
	P2PF(ipts[i], origpts[i]);
	pts[i] = origpts[i];
    }

    t = findVertical(pts, 0.0, 1.0, bb->LL.x, bb->LL.y, bb->UR.y);
    if ((t >= 0) && (t < tmin)) {
	Bezier(origpts, 3, t, pts, NULL);
	tmin = t;
    }
    t = findVertical(pts, 0.0, MIN(1.0, tmin), bb->UR.x, bb->LL.y,
		     bb->UR.y);
    if ((t >= 0) && (t < tmin)) {
	Bezier(origpts, 3, t, pts, NULL);
	tmin = t;
    }
    t = findHorizontal(pts, 0.0, MIN(1.0, tmin), bb->LL.y, bb->LL.x,
		       bb->UR.x);
    if ((t >= 0) && (t < tmin)) {
	Bezier(origpts, 3, t, pts, NULL);
	tmin = t;
    }
    t = findHorizontal(pts, 0.0, MIN(1.0, tmin), bb->UR.y, bb->LL.x,
		       bb->UR.x);
    if ((t >= 0) && (t < tmin)) {
	Bezier(origpts, 3, t, pts, NULL);
	tmin = t;
    }

    if (tmin < 2.0) {
	for (i = 0; i < 4; i++) {
	    PF2P(pts[i], ipts[i]);
	}
	return 1;
    } else
	return 0;

}

/* makeCompoundEdge:
 * If edge e has a cluster head and/or cluster tail,
 * clip spline to outside of cluster. 
 * Requirement: spline is composed of only one part, 
 * with n control points where n >= 4 and n (mod 3) = 1.
 * If edge has arrowheads, reposition them.
 */
static void makeCompoundEdge(graph_t * g, edge_t * e)
{
    graph_t *lh;		/* cluster containing head */
    graph_t *lt;		/* cluster containing tail */
    bezier *bez;		/* original Bezier for e */
    bezier *nbez;		/* new Bezier  for e */
    int starti = 0, endi = 0;	/* index of first and last control point */
    node_t *head;
    node_t *tail;
    box *bb;
    int i, j;
    int size;
    point pts[4];
    point p;
    int fixed;

    /* find head and tail target clusters, if defined */
    lh = getCluster(g, agget(e, "lhead"));
    lt = getCluster(g, agget(e, "ltail"));
    if (!lt && !lh)
	return;

    /* at present, we only handle single spline case */
    if (ED_spl(e)->size > 1) {
	agerr(AGWARN, "%s -> %s: spline size > 1 not supported\n",
	      e->tail->name, e->head->name);
	return;
    }
    bez = ED_spl(e)->list;
    size = bez->size;

    head = e->head;
    tail = e->tail;

    /* allocate new Bezier */
    nbez = GNEW(bezier);
    nbez->eflag = bez->eflag;
    nbez->sflag = bez->sflag;

    /* if Bezier has four points, almost collinear,
     * make line - unimplemented optimization?
     */

    /* If head cluster defined, find first Bezier
     * crossing head cluster, and truncate spline to
     * box edge.
     * Otherwise, leave end alone.
     */
    fixed = 0;
    if (lh) {
	bb = &(GD_bb(lh));
	if (!inBox(ND_coord_i(head), bb)) {
	    agerr(AGWARN, "%s -> %s: head not inside head cluster %s\n",
		  e->tail->name, e->head->name, agget(e, "lhead"));
	} else {
	    /* If first control point is in bb, degenerate case. Spline
	     * reduces to four points between the arrow head and the point 
	     * where the segment between the first control point and arrow head 
	     * crosses box.
	     */
	    if (inBox(bez->list[0], bb)) {
		if (inBox(ND_coord_i(tail), bb)) {
		    agerr(AGWARN,
			  "%s -> %s: tail is inside head cluster %s\n",
			  e->tail->name, e->head->name, agget(e, "lhead"));
		} else {
		    assert(bez->sflag);	/* must be arrowhead on tail */
		    p = boxIntersect(bez->list[0], bez->sp, bb);
		    bez->list[3] = p;
		    bez->list[1] = midPt(p, bez->sp);
		    bez->list[0] = midPt(bez->list[1], bez->sp);
		    bez->list[2] = midPt(bez->list[1], p);
		    if (bez->eflag)
			endi =
			    arrowEndClip(e, bez->list,
					 starti, 0, nbez, bez->eflag);
		    endi += 3;
		    fixed = 1;
		}
	    } else {
		for (endi = 0; endi < size - 1; endi += 3) {
		    if (splineIntersect(&(bez->list[endi]), bb))
			break;
		}
		if (endi == size - 1) {	/* no intersection */
		    assert(bez->eflag);
		    nbez->ep = boxIntersect(bez->ep, bez->list[endi], bb);
		} else {
		    if (bez->eflag)
			endi =
			    arrowEndClip(e, bez->list,
					 starti, endi, nbez, bez->eflag);
		    endi += 3;
		}
		fixed = 1;
	    }
	}
    }
    if (fixed == 0) {		/* if no lh, or something went wrong, use original head */
	endi = size - 1;
	if (bez->eflag)
	    nbez->ep = bez->ep;
    }

    /* If tail cluster defined, find last Bezier
     * crossing tail cluster, and truncate spline to
     * box edge.
     * Otherwise, leave end alone.
     */
    fixed = 0;
    if (lt) {
	bb = &(GD_bb(lt));
	if (!inBox(ND_coord_i(tail), bb)) {
	    agerr(AGWARN, "%s -> %s: tail not inside tail cluster %s\n",
		  e->tail->name, head->name, agget(e, "ltail"));
	} else {
	    /* If last control point is in bb, degenerate case. Spline
	     * reduces to four points between arrow head, and the point
	     * where the segment between the last control point and the 
	     * arrow head crosses box.
	     */
	    if (inBox(bez->list[endi], bb)) {
		if (inBox(ND_coord_i(head), bb)) {
		    agerr(AGWARN,
			  "%s -> %s: head is inside tail cluster %s\n",
			  e->tail->name, e->head->name, agget(e, "ltail"));
		} else {
		    assert(bez->eflag);	/* must be arrowhead on head */
		    p = boxIntersect(bez->list[endi], nbez->ep, bb);
		    starti = endi - 3;
		    bez->list[starti] = p;
		    bez->list[starti + 2] = midPt(p, nbez->ep);
		    bez->list[starti + 3] =
			midPt(bez->list[starti + 2], nbez->ep);
		    bez->list[starti + 1] =
			midPt(bez->list[starti + 2], p);
		    if (bez->sflag)
			starti =
			    arrowStartClip(e, bez->list,
					   starti, endi - 3, nbez,
					   bez->sflag);
		    fixed = 1;
		}
	    } else {
		for (starti = endi; starti > 0; starti -= 3) {
		    for (i = 0; i < 4; i++)
			pts[i] = bez->list[starti - i];
		    if (splineIntersect(pts, bb)) {
			for (i = 0; i < 4; i++)
			    bez->list[starti - i] = pts[i];
			break;
		    }
		}
		if (starti == 0) {
		    assert(bez->sflag);
		    nbez->sp =
			boxIntersect(bez->sp, bez->list[starti], bb);
		} else {
		    starti -= 3;
		    if (bez->sflag)
			starti =
			    arrowStartClip(e, bez->list,
					   starti, endi - 3, nbez,
					   bez->sflag);
		}
		fixed = 1;
	    }
	}
    }
    if (fixed == 0) {		/* if no lt, or something went wrong, use original tail */
	/* Note: starti == 0 */
	if (bez->sflag)
	    nbez->sp = bez->sp;
    }

    /* complete Bezier, free garbage and attach new Bezier to edge 
     */
    nbez->size = endi - starti + 1;
    nbez->list = N_GNEW(nbez->size, point);
    for (i = 0, j = starti; i < nbez->size; i++, j++)
	nbez->list[i] = bez->list[j];
    free(bez->list);
    free(bez);
    ED_spl(e)->list = nbez;

}

/* dot_compoundEdges:
 */
void dot_compoundEdges(graph_t * g)
{
    edge_t *e;
    node_t *n;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    makeCompoundEdge(g, e);
	}
    }
}
