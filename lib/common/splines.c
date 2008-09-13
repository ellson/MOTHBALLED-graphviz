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


/* Functions related to creating a spline and attaching it to
 * an edge, starting from a list of control points.
 */

#include "render.h"

#ifdef DEBUG
static int debugleveln(edge_t* e, int i)
{
    return (GD_showboxes(e->head->graph) == i ||
	    GD_showboxes(e->tail->graph) == i ||
	    ED_showboxes(e) == i ||
	    ND_showboxes(e->head) == i ||
	    ND_showboxes(e->tail) == i);
}

static void showPoints(pointf ps[], int pn)
{
    char buf[BUFSIZ];
    int newcnt = Show_cnt + pn + 3;
    int bi, li;

    Show_boxes = ALLOC(newcnt+2,Show_boxes,char*);
    li = Show_cnt+1;
    Show_boxes[li++] = strdup ("%% self list");
    Show_boxes[li++] = strdup ("dbgstart");
    for (bi = 0; bi < pn; bi++) {
	sprintf(buf, "%.3g %.3g point", ps[bi].x, ps[bi].y);
	Show_boxes[li++] = strdup (buf);
    }
    Show_boxes[li++] = strdup ("grestore");

    Show_cnt = newcnt;
    Show_boxes[Show_cnt+1] = NULL;
}
#endif

/* arrow_clip:
 * Clip arrow to node boundary.
 * The real work is done elsewhere. Here we get the real edge,
 * check that the edge has arrowheads, and that an endpoint
 * isn't a merge point where several parts of an edge meet.
 * (e.g., with edge concentrators).
 */
static void
arrow_clip(edge_t * fe, node_t * hn,
	   pointf * ps, int *startp, int *endp,
	   bezier * spl, splineInfo * info)
{
    edge_t *e;
    int i, j, sflag, eflag;

    for (e = fe; ED_to_orig(e); e = ED_to_orig(e));

    j = info->swapEnds(e);
    arrow_flags(e, &sflag, &eflag);
    if (info->splineMerge(hn))
	eflag = ARR_NONE;
    if (info->splineMerge(fe->tail))
	sflag = ARR_NONE;
    if (j) {
	i = sflag;
	sflag = eflag;
	eflag = i;
    }
    /* swap the two ends */
    if (sflag)
	*startp =
	    arrowStartClip(e, ps, *startp, *endp, spl, sflag);
    if (eflag)
	*endp =
	    arrowEndClip(e, ps, *startp, *endp, spl, eflag);
}

/* bezier_clip
 * Clip bezier to shape using binary search.
 * The details of the shape are passed in the inside_context;
 * The function providing the inside test is passed as a parameter.
 * left_inside specifies that sp[0] is inside the node, 
 * else sp[3] is taken as inside.
 * The points p are in node coordinates.
 */
void bezier_clip(inside_t * inside_context,
		 boolean(*inside) (inside_t * inside_context, pointf p),
		 pointf * sp, boolean left_inside)
{
    pointf seg[4], best[4], pt, opt, *left, *right;
    double low, high, t, *idir, *odir;
    boolean found;
    int i;

    if (left_inside) {
	left = NULL;
	right = seg;
	pt = sp[0];
	idir = &low;
	odir = &high;
    } else {
	left = seg;
	right = NULL;
	pt = sp[3];
	idir = &high;
	odir = &low;
    }
    found = FALSE;
    low = 0.0;
    high = 1.0;
    do {
	opt = pt;
	t = (high + low) / 2.0;
	pt = Bezier(sp, 3, t, left, right);
	if (inside(inside_context, pt)) {
	    *idir = t;
	} else {
	    for (i = 0; i < 4; i++)
		best[i] = seg[i];
	    found = TRUE;
	    *odir = t;
	}
    } while (ABS(opt.x - pt.x) > .5 || ABS(opt.y - pt.y) > .5);
    if (found)
	for (i = 0; i < 4; i++)
	    sp[i] = best[i];
    else
	for (i = 0; i < 4; i++)
	    sp[i] = seg[i];
}

/* shape_clip0:
 * Clip Bezier to node shape using binary search.
 * left_inside specifies that curve[0] is inside the node, else
 * curve[3] is taken as inside.
 * Assumes ND_shape(n) and ND_shape(n)->fns->insidefn are non-NULL.
 * See note on shape_clip.
 */
static void
shape_clip0(inside_t * inside_context, node_t * n, pointf curve[4],
	    boolean left_inside)
{
    int i, save_real_size;
    pointf c[4];

    save_real_size = ND_rw(n);
    for (i = 0; i < 4; i++) {
	c[i].x = curve[i].x - ND_coord(n).x;
	c[i].y = curve[i].y - ND_coord(n).y;
    }

    bezier_clip(inside_context, ND_shape(n)->fns->insidefn, c,
		left_inside);

    for (i = 0; i < 4; i++) {
	curve[i].x = c[i].x + ND_coord(n).x;
	curve[i].y = c[i].y + ND_coord(n).y;
    }
    ND_rw(n) = save_real_size;
}

/* shape_clip:
 * Clip Bezier to node shape
 * Uses curve[0] to determine which which side is inside the node. 
 * NOTE: This test is bad. It is possible for previous call to
 * shape_clip to produce a Bezier with curve[0] moved to the boundary
 * for which insidefn(curve[0]) is true. Thus, if the new Bezier is
 * fed back to shape_clip, it will again assume left_inside is true.
 * To be safe, shape_clip0 should guarantee that the computed boundary
 * point fails insidefn.
 * The edge e is used to provide a port box. If NULL, the spline is
 * clipped to the node shape.
 */
void shape_clip(node_t * n, pointf curve[4])
{
    int save_real_size;
    boolean left_inside;
    pointf c;
    inside_t inside_context;

    if (ND_shape(n) == NULL || ND_shape(n)->fns->insidefn == NULL)
	return;

    inside_context.s.n = n;
    inside_context.s.bp = NULL;
    save_real_size = ND_rw(n);
    c.x = curve[0].x - ND_coord(n).x;
    c.y = curve[0].y - ND_coord(n).y;
    left_inside = ND_shape(n)->fns->insidefn(&inside_context, c);
    ND_rw(n) = save_real_size;
    shape_clip0(&inside_context, n, curve, left_inside);
}

/* new_spline:
 * Create and attach a new bezier of size sz to the edge d
 */
bezier *new_spline(edge_t * e, int sz)
{
    bezier *rv;

    while (ED_edge_type(e) != NORMAL)
	e = ED_to_orig(e);
    if (ED_spl(e) == NULL)
	ED_spl(e) = NEW(splines);
    ED_spl(e)->list = ALLOC(ED_spl(e)->size + 1, ED_spl(e)->list, bezier);
    rv = &(ED_spl(e)->list[ED_spl(e)->size++]);
    rv->list = N_NEW(sz, pointf);
    rv->size = sz;
    rv->sflag = rv->eflag = FALSE;
    return rv;
}

/* clip_and_install:
 * Given a raw spline (pn control points in ps), representing
 * a path from edge fe->tail ending in node hn, clip the ends to
 * the node boundaries and attach the resulting spline to the
 * edge.
 */
void
clip_and_install(edge_t * fe, node_t * hn, pointf * ps, int pn,
		 splineInfo * info)
{
    pointf p2;
    bezier *newspl;
    node_t *tn;
    int start, end, i, clipTail, clipHead;
    graph_t *g;
    edge_t *orig;
    boxf *tbox, *hbox;
    inside_t inside_context;

    tn = fe->tail;
    g = tn->graph;
    newspl = new_spline(fe, pn);

    for (orig = fe; ED_edge_type(orig) != NORMAL; orig = ED_to_orig(orig));

    /* may be a reversed flat edge */
    if ((tn->u.rank == hn->u.rank) && (tn->u.order > hn->u.order)) {
	node_t *tmp;
	tmp = hn;
	hn = tn;
	tn = tmp;
    }
    if (tn == orig->tail) {
	clipTail = ED_tail_port(orig).clip;
	clipHead = ED_head_port(orig).clip;
	tbox = ED_tail_port(orig).bp;
	hbox = ED_head_port(orig).bp;
    }
    else { /* fe and orig are reversed */
	clipTail = ED_head_port(orig).clip;
	clipHead = ED_tail_port(orig).clip;
	hbox = ED_tail_port(orig).bp;
	tbox = ED_head_port(orig).bp;
    }

    /* spline may be interior to node */
    if(clipTail && ND_shape(tn) && ND_shape(tn)->fns->insidefn) {
	inside_context.s.n = tn;
	inside_context.s.bp = tbox;
	for (start = 0; start < pn - 4; start += 3) {
	    p2.x = ps[start + 3].x - ND_coord(tn).x;
	    p2.y = ps[start + 3].y - ND_coord(tn).y;
	    if (ND_shape(tn)->fns->insidefn(&inside_context, p2) == FALSE)
		break;
	}
	shape_clip0(&inside_context, tn, &ps[start], TRUE);
    } else
	start = 0;
    if(clipHead && ND_shape(hn) && ND_shape(hn)->fns->insidefn) {
	inside_context.s.n = hn;
	inside_context.s.bp = hbox;
	for (end = pn - 4; end > 0; end -= 3) {
	    p2.x = ps[end].x - ND_coord(hn).x;
	    p2.y = ps[end].y - ND_coord(hn).y;
	    if (ND_shape(hn)->fns->insidefn(&inside_context, p2) == FALSE)
		break;
	}
	shape_clip0(&inside_context, hn, &ps[end], FALSE);
    } else
	end = pn - 4;
    for (; start < pn - 4; start += 3) 
	if (! APPROXEQPT(ps[start], ps[start + 3], MILLIPOINT))
	    break;
    for (; end > 0; end -= 3)
	if (! APPROXEQPT(ps[end], ps[end + 3], MILLIPOINT))
	    break;
    arrow_clip(fe, hn, ps, &start, &end, newspl, info);
    for (i = start; i < end + 4; ) {
	pointf cp[4];
	newspl->list[i - start] = ps[i];
	cp[0] = ps[i];
	i++;
	if ( i >= end + 4)
	    break;
	newspl->list[i - start] = ps[i];
	cp[1] = ps[i];
	i++;
	newspl->list[i - start] = ps[i];
	cp[2] = ps[i];
	i++;
	cp[3] = ps[i];
	update_bb_bz(&GD_bb(g), cp);
    }
    newspl->size = end - start + 4;
}

static double 
conc_slope(node_t* n)
{
    double s_in, s_out, m_in, m_out;
    int cnt_in, cnt_out;
    pointf p;
    edge_t *e;

    s_in = s_out = 0.0;
    for (cnt_in = 0; (e = ND_in(n).list[cnt_in]); cnt_in++)
	s_in += ND_coord(e->tail).x;
    for (cnt_out = 0; (e = ND_out(n).list[cnt_out]); cnt_out++)
	s_out += ND_coord(e->head).x;
    p.x = ND_coord(n).x - (s_in / cnt_in);
    p.y = ND_coord(n).y - ND_coord(ND_in(n).list[0]->tail).y;
    m_in = atan2(p.y, p.x);
    p.x = (s_out / cnt_out) - ND_coord(n).x;
    p.y = ND_coord(ND_out(n).list[0]->head).y - ND_coord(n).y;
    m_out = atan2(p.y, p.x);
    return ((m_in + m_out) / 2.0);
}

void add_box(path * P, boxf b)
{
    if (b.LL.x < b.UR.x && b.LL.y < b.UR.y)
	P->boxes[P->nbox++] = b;
}

/* beginpath:
 * Set up boxes near the tail node.
 * For regular nodes, the result should be a list of continguous rectangles 
 * such that the last one ends has the smallest LL.y and its LL.y is above
 * the bottom of the rank (rank.ht1).
 * 
 * For flat edges, we assume endp->sidemask has been set. For regular
 * edges, we set this, but it doesn't appear to be needed any more.
 * 
 * In many cases, we tweak the x or y coordinate of P->start.p by 1.
 * This is because of a problem in the path routing code. If the starting
 * point actually lies on the polygon, in some cases, the router gets
 * confused and routes the path outside the polygon. So, the offset ensures
 * the starting point is in the polygon.
 *
 * FIX: Creating the initial boxes only really works for rankdir=TB and
 * rankdir=LR. For the others, we rely on compassPort flipping the side
 * and then assume that the node shape has top-bottom symmetry. Since we
 * at present only put compass points on the bounding box, this works.
 * If we attempt to implement compass points on actual node perimeters,
 * something major will probably be necessary. Doing the coordinate
 * flip (postprocess) before spline routing will be too disruptive. The
 * correct solution is probably to have beginpath/endpath create the
 * boxes assuming an inverted node. Note that compassPort already does
 * some flipping. Even better would be to allow the *_path function
 * to provide a polygon.
 *
 * The extra space provided by FUDGE-2 prevents the edge from getting
 * too close the side of the node.
 */
#define FUDGE 2

void
beginpath(path * P, edge_t * e, int et, pathend_t * endp, boolean merge)
{
    int side, mask;
    node_t *n;
    int (*pboxfn) (node_t*, port*, int, boxf*, int*);

    n = e->tail;

    if (ED_tail_port(e).dyna)
	ED_tail_port(e) = resolvePort(e->tail, e->head, &ED_tail_port(e));
    if (ND_shape(n))
	pboxfn = ND_shape(n)->fns->pboxfn;
    else
	pboxfn = NULL;
    P->start.p = add_pointf(ND_coord(n), ED_tail_port(e).p);
    if (merge) {
	/*P->start.theta = - M_PI / 2; */
	P->start.theta = conc_slope(e->tail);
	P->start.constrained = TRUE;
    } else {
	if (ED_tail_port(e).constrained) {
	    P->start.theta = ED_tail_port(e).theta;
	    P->start.constrained = TRUE;
	} else
	    P->start.constrained = FALSE;
    }
    P->nbox = 0;
    P->data = (void *) e;
    endp->np = P->start.p;
    if ((et == REGULAREDGE) && (ND_node_type(n) == NORMAL) && ((side = ED_tail_port(e).side))) {
	edge_t* orig;
	boxf b0, b = endp->nb;
	if (side & TOP) {
	    endp->sidemask = TOP;
	    if (P->start.p.x < ND_coord(n).x) { /* go left */
		b0.LL.x = b.LL.x - 1;
		/* b0.LL.y = ND_coord(n).y + ND_ht(n)/2; */
		b0.LL.y = P->start.p.y;
		b0.UR.x = b.UR.x;
		b0.UR.y = ND_coord(n).y + ND_ht(n)/2 + GD_ranksep(n->graph)/2;
		b.UR.x = ND_coord(n).x - ND_lw(n) - (FUDGE-2);
		b.UR.y = b0.LL.y;
		b.LL.y = ND_coord(n).y - ND_ht(n)/2;
		b.LL.x -= 1;
		endp->boxes[0] = b0;
		endp->boxes[1] = b;
	    }
	    else {
		b0.LL.x = b.LL.x;
		b0.LL.y = P->start.p.y;
		/* b0.LL.y = ND_coord(n).y + ND_ht(n)/2; */
		b0.UR.x = b.UR.x+1;
		b0.UR.y = ND_coord(n).y + ND_ht(n)/2 + GD_ranksep(n->graph)/2;
		b.LL.x = ND_coord(n).x + ND_rw(n) + (FUDGE-2);
		b.UR.y = b0.LL.y;
		b.LL.y = ND_coord(n).y - ND_ht(n)/2;
		b.UR.x += 1;
		endp->boxes[0] = b0;
		endp->boxes[1] = b;
	    } 
	    P->start.p.y += 1;
	    endp->boxn = 2;
	}
	else if (side & BOTTOM) {
	    endp->sidemask = BOTTOM;
	    b.UR.y = MAX(b.UR.y,P->start.p.y);
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    P->start.p.y -= 1;
	}
	else if (side & LEFT) {
	    endp->sidemask = LEFT;
	    b.UR.x = P->start.p.x;
	    b.LL.y = ND_coord(n).y - ND_ht(n)/2;
	    b.UR.y = P->start.p.y;
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    P->start.p.x -= 1;
	}
	else {
	    endp->sidemask = RIGHT;
	    b.LL.x = P->start.p.x;
	    b.LL.y = ND_coord(n).y - ND_ht(n)/2;
	    b.UR.y = P->start.p.y;
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    P->start.p.x += 1;
	}
	for (orig = e; ED_edge_type(orig) != NORMAL; orig = ED_to_orig(orig));
	if (n == orig->tail)
	    ED_tail_port(orig).clip = FALSE;
	else
	    ED_head_port(orig).clip = FALSE;
	return;
    }
    if ((et == FLATEDGE) && ((side = ED_tail_port(e).side))) {
	boxf b0, b = endp->nb;
	edge_t* orig;
	if (side & TOP) {
	    b.LL.y = MIN(b.LL.y,P->end.p.y);
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	}
	else if (side & BOTTOM) {
	    if (endp->sidemask == TOP) {
		b0.UR.y = ND_coord(n).y - ND_ht(n)/2;
		b0.UR.x = b.UR.x+1;
		b0.LL.x = P->start.p.x;
		b0.LL.y = b0.UR.y - GD_ranksep(n->graph)/2;
		b.LL.x = ND_coord(n).x + ND_rw(n) + (FUDGE-2);
		b.LL.y = b0.UR.y;
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.UR.x += 1;
		endp->boxes[0] = b0;
		endp->boxes[1] = b;
		endp->boxn = 2;
	    }
	    else {
		b.UR.y = MAX(b.UR.y,P->start.p.y);
		endp->boxes[0] = b;
		endp->boxn = 1;
	    }
	}
	else if (side & LEFT) {
	    b.UR.x = P->start.p.x+1;
	    if (endp->sidemask == TOP) {
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.LL.y = P->start.p.y-1;
	    }
	    else {
		b.LL.y = ND_coord(n).y - ND_ht(n)/2;
		b.UR.y = P->start.p.y+1;
	    }
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	}
	else {
	    b.LL.x = P->start.p.x;
	    if (endp->sidemask == TOP) {
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.LL.y = P->start.p.y;
	    }
	    else {
		b.LL.y = ND_coord(n).y - ND_ht(n)/2;
		b.UR.y = P->start.p.y+1;
	    }
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	}
	for (orig = e; ED_edge_type(orig) != NORMAL; orig = ED_to_orig(orig));
	if (n == orig->tail)
	    ED_tail_port(orig).clip = FALSE;
	else
	    ED_head_port(orig).clip = FALSE;
	endp->sidemask = side;
	return;
    }

    if (et == REGULAREDGE) side = BOTTOM;
    else side = endp->sidemask;  /* for flat edges */
    if (pboxfn
	&& (mask = (*pboxfn) (n, &ED_tail_port(e), side, &endp->boxes[0], &endp->boxn)))
	endp->sidemask = mask;
    else {
	endp->boxes[0] = endp->nb;
	endp->boxn = 1;

	switch (et) {
	case SELFEDGE:
	/* moving the box UR.y by + 1 avoids colinearity between
	   port point and box that confuses Proutespline().  it's
	   a bug in Proutespline() but this is the easiest fix. */
	    assert(0);  /* at present, we don't use beginpath for selfedges */
	    endp->boxes[0].UR.y = P->start.p.y - 1;
	    endp->sidemask = BOTTOM;
	    break;
	case FLATEDGE:
	    if (endp->sidemask == TOP)
		endp->boxes[0].LL.y = P->start.p.y;
	    else
		endp->boxes[0].UR.y = P->start.p.y;
	    break;
	case REGULAREDGE:
	    endp->boxes[0].UR.y = P->start.p.y;
	    endp->sidemask = BOTTOM;
	    P->start.p.y -= 1;
	    break;
	}    
    }    
}

void endpath(path * P, edge_t * e, int et, pathend_t * endp, boolean merge)
{
    int side, mask;
    node_t *n;
    int (*pboxfn) (node_t* n, port*, int, boxf*, int*);

    n = e->head;

    if (ED_head_port(e).dyna) 
	ED_head_port(e) = resolvePort(e->head, e->tail, &ED_head_port(e));
    if (ND_shape(n))
	pboxfn = ND_shape(n)->fns->pboxfn;
    else
	pboxfn = NULL;
    P->end.p = add_pointf(ND_coord(n), ED_head_port(e).p);
    if (merge) {
	/*P->end.theta = M_PI / 2; */
	P->end.theta = conc_slope(e->head) + M_PI;
	assert(P->end.theta < 2 * M_PI);
	P->end.constrained = TRUE;
    } else {
	if (ED_head_port(e).constrained) {
	    P->end.theta = ED_head_port(e).theta;
	    P->end.constrained = TRUE;
	} else
	    P->end.constrained = FALSE;
    }
    endp->np = P->end.p;
    if ((et == REGULAREDGE) && (ND_node_type(n) == NORMAL) && ((side = ED_head_port(e).side))) {
	edge_t* orig;
	boxf b0, b = endp->nb;
	if (side & TOP) {
	    endp->sidemask = TOP;
	    b.LL.y = MIN(b.LL.y,P->end.p.y);
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    P->start.p.y += 1;
	}
	else if (side & BOTTOM) {
	    endp->sidemask = BOTTOM;
	    if (P->end.p.x < ND_coord(n).x) { /* go left */
		b0.LL.x = b.LL.x-1;
		/* b0.UR.y = ND_coord(n).y - ND_ht(n)/2; */
		b0.UR.y = P->end.p.y;
		b0.UR.x = b.UR.x;
		b0.LL.y = ND_coord(n).y - ND_ht(n)/2 - GD_ranksep(n->graph)/2;
		b.UR.x = ND_coord(n).x - ND_lw(n) - (FUDGE-2);
		b.LL.y = b0.UR.y;
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.LL.x -= 1;
		endp->boxes[0] = b0;
		endp->boxes[1] = b;
	    }
	    else {
		b0.LL.x = b.LL.x;
		b0.UR.y = P->end.p.y;
		/* b0.UR.y = ND_coord(n).y - ND_ht(n)/2; */
		b0.UR.x = b.UR.x+1;
		b0.LL.y = ND_coord(n).y - ND_ht(n)/2 - GD_ranksep(n->graph)/2;
		b.LL.x = ND_coord(n).x + ND_rw(n) + (FUDGE-2);
		b.LL.y = b0.UR.y;
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.UR.x += 1;
		endp->boxes[0] = b0;
		endp->boxes[1] = b;
	    } 
	    endp->boxn = 2;
	    P->end.p.y -= 1;
	}
	else if (side & LEFT) {
	    endp->sidemask = LEFT;
	    b.UR.x = P->end.p.x;
	    b.UR.y = ND_coord(n).y + ND_ht(n)/2;
	    b.LL.y = P->end.p.y;
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    P->start.p.x -= 1;
	}
	else {
	    endp->sidemask = RIGHT;
	    b.LL.x = P->end.p.x;
	    b.UR.y = ND_coord(n).y + ND_ht(n)/2;
	    b.LL.y = P->end.p.y;
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    P->start.p.x -= 1;
	}
	for (orig = e; ED_edge_type(orig) != NORMAL; orig = ED_to_orig(orig));
	if (n == orig->head)
	    ED_head_port(orig).clip = FALSE;
	else
	    ED_tail_port(orig).clip = FALSE;
	endp->sidemask = side;
	return;
    }

    if ((et == FLATEDGE) && ((side = ED_head_port(e).side))) {
	edge_t* orig;
	boxf b0, b = endp->nb;
	switch (side) {
	case LEFT:
	    b.UR.x = P->end.p.x;
	    if (endp->sidemask == TOP) {
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.LL.y = P->end.p.y;
	    }
	    else {
		b.LL.y = ND_coord(n).y - ND_ht(n)/2;
		b.UR.y = P->end.p.y;
	    }
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    break;
	case RIGHT:
	    b.LL.x = P->end.p.x-1;
	    if (endp->sidemask == TOP) {
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.LL.y = P->end.p.y-1;
	    }
	    else {
		b.LL.y = ND_coord(n).y - ND_ht(n)/2;
		b.UR.y = P->end.p.y;
	    }
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    break;
	case TOP:
	    b.LL.y = MIN(b.LL.y,P->end.p.y);
	    endp->boxes[0] = b;
	    endp->boxn = 1;
	    break;
	case BOTTOM:
	    if (endp->sidemask == TOP) {
		b0.LL.x = b.LL.x-1;
		b0.UR.y = ND_coord(n).y - ND_ht(n)/2;
		b0.UR.x = P->end.p.x;
		b0.LL.y = b0.UR.y - GD_ranksep(n->graph)/2;
		b.UR.x = ND_coord(n).x - ND_lw(n) - 2;
		b.LL.y = b0.UR.y;
		b.UR.y = ND_coord(n).y + ND_ht(n)/2;
		b.LL.x -= 1;
		endp->boxes[0] = b0;
		endp->boxes[1] = b;
		endp->boxn = 2;
	    }
	    else {
		b.UR.y = MAX(b.UR.y,P->start.p.y);
		endp->boxes[0] = b;
		endp->boxn = 1;
	    }
	    break;
	}
	for (orig = e; ED_edge_type(orig) != NORMAL; orig = ED_to_orig(orig));
	if (n == orig->head)
	    ED_head_port(orig).clip = FALSE;
	else
	    ED_tail_port(orig).clip = FALSE;
	endp->sidemask = side;
	return;
    }

    if (et == REGULAREDGE) side = TOP;
    else side = endp->sidemask;  /* for flat edges */
    if (pboxfn
	&& (mask = (*pboxfn) (n, &ED_head_port(e), side, &endp->boxes[0], &endp->boxn)))
	endp->sidemask = mask;
    else {
	endp->boxes[0] = endp->nb;
	endp->boxn = 1;
	switch (et) {
	case SELFEDGE:
	    /* offset of -1 is symmetric w.r.t. beginpath() 
	     * FIXME: is any of this right?  what if self-edge 
	     * doesn't connect from BOTTOM to TOP??? */
	    assert(0);  /* at present, we don't use endpath for selfedges */
	    endp->boxes[0].LL.y = P->end.p.y + 1;
	    endp->sidemask = TOP;
	    break;
	case FLATEDGE:
	    if (endp->sidemask == TOP)
		endp->boxes[0].LL.y = P->start.p.y;
	    else
		endp->boxes[0].UR.y = P->start.p.y;
	    break;
	case REGULAREDGE:
	    endp->boxes[0].LL.y = P->end.p.y;
	    endp->sidemask = TOP;
	    P->start.p.y += 1;
	    break;
	}
    }
}

static void selfBottom (edge_t* edges[], int ind, int cnt,
	double sizex, double stepy, splineInfo* sinfo) 
{
    pointf tp, hp, np;
    node_t *n;
    edge_t *e;
    int i, sgn;
    double hy, ty, stepx, dx, dy, width, height; 
    pointf points[1000];
    int pointn;

    e = edges[ind];
    n = e->tail;

    stepx = (sizex / 2.) / cnt;
    stepx = MAX(stepx,2.);
    pointn = 0;
    np = ND_coord(n);
    tp = ED_tail_port(e).p;
    tp.x += np.x;
    tp.y += np.y;
    hp = ED_head_port(e).p;
    hp.x += np.x;
    hp.y += np.y;
    if (tp.x >= hp.x) sgn = 1;
    else sgn = -1;
    dy = ND_ht(n)/2., dx = 0.;
    ty = MIN(dy, 3*(tp.y + dy - np.y));
    hy = MIN(dy, 3*(hp.y + dy - np.y));
    for (i = 0; i < cnt; i++) {
        e = edges[ind++];
        dy += stepy, ty += stepy, hy += stepy, dx += sgn*stepx;
        pointn = 0;
        points[pointn++] = tp;
        points[pointn++] = pointfof(tp.x + dx, tp.y - ty / 3);
        points[pointn++] = pointfof(tp.x + dx, np.y - dy);
        points[pointn++] = pointfof((tp.x+hp.x)/2, np.y - dy);
        points[pointn++] = pointfof(hp.x - dx, np.y - dy);
        points[pointn++] = pointfof(hp.x - dx, hp.y - hy / 3);
        points[pointn++] = hp;
        if (ED_label(e)) {
    	if (GD_flip(e->tail->graph)) {
    	    width = ED_label(e)->dimen.y;
    	    height = ED_label(e)->dimen.x;
    	} else {
    	    width = ED_label(e)->dimen.x;
    	    height = ED_label(e)->dimen.y;
    	}
    	ED_label(e)->pos.y = ND_coord(n).y - dy - height / 2.0;
    	ED_label(e)->pos.x = ND_coord(n).x;
    	ED_label(e)->set = TRUE;
    	if (height > stepy)
    	    dy += height - stepy;
    	if (dx + stepx < width)
    	    dx += width - stepx;
        }
        clip_and_install(e, e->head, points, pointn, sinfo);
#ifdef DEBUG
        if (debugleveln(e,1))
	    showPoints (points, pointn);
#endif
    }
}


static void
selfTop (edge_t* edges[], int ind, int cnt, double sizex, double stepy,
           splineInfo* sinfo) 
{
    int i, sgn;
    double hy, ty,  stepx, dx, dy, width, height; 
    pointf tp, hp, np;
    node_t *n;
    edge_t *e;
    pointf points[1000];
    int pointn;

    e = edges[ind];
    n = e->tail;

    stepx = (sizex / 2.) / cnt;
    stepx = MAX(stepx, 2.);
    pointn = 0;
    np = ND_coord(n);
    tp = ED_tail_port(e).p;
    tp.x += np.x;
    tp.y += np.y;
    hp = ED_head_port(e).p;
    hp.x += np.x;
    hp.y += np.y;
    if (tp.x >= hp.x) sgn = 1;
    else sgn = -1;
    dy = ND_ht(n)/2., dx = 0.;
    ty = MIN(dy, 3*(np.y + dy - tp.y));
    hy = MIN(dy, 3*(np.y + dy - hp.y));
    for (i = 0; i < cnt; i++) {
        e = edges[ind++];
        dy += stepy, ty += stepy, hy += stepy, dx += sgn*stepx;
        pointn = 0;
        points[pointn++] = tp;
        points[pointn++] = pointfof(tp.x + dx, tp.y + ty / 3);
        points[pointn++] = pointfof(tp.x + dx, np.y + dy);
        points[pointn++] = pointfof((tp.x+hp.x)/2, np.y + dy);
        points[pointn++] = pointfof(hp.x - dx, np.y + dy);
        points[pointn++] = pointfof(hp.x - dx, hp.y + hy / 3);
        points[pointn++] = hp;
        if (ED_label(e)) {
	    if (GD_flip(e->tail->graph)) {
		width = ED_label(e)->dimen.y;
		height = ED_label(e)->dimen.x;
	    } else {
		width = ED_label(e)->dimen.x;
		height = ED_label(e)->dimen.y;
	    }
	    ED_label(e)->pos.y = ND_coord(n).y + dy + height / 2.0;
	    ED_label(e)->pos.x = ND_coord(n).x;
	    ED_label(e)->set = TRUE;
	    if (height > stepy)
		dy += height - stepy;
	    if (dx + stepx < width)
		dx += width - stepx;
        }
        clip_and_install(e, e->head, points, pointn, sinfo);
#ifdef DEBUG
        if (debugleveln(e,1))
	    showPoints (points, pointn);
#endif
    }
    return;
}

static void
selfRight (edge_t* edges[], int ind, int cnt, double stepx, double sizey,
           splineInfo* sinfo) 
{
    int i, sgn;
    double hx, tx, stepy, dx, dy, width, height; 
    pointf tp, hp, np;
    node_t *n;
    edge_t *e;
    pointf points[1000];
    int pointn;

    e = edges[ind];
    n = e->tail;

    stepy = (sizey / 2.) / cnt;
    stepy = MAX(stepy, 2.);
    pointn = 0;
    np = ND_coord(n);
    tp = ED_tail_port(e).p;
    tp.x += np.x;
    tp.y += np.y;
    hp = ED_head_port(e).p;
    hp.x += np.x;
    hp.y += np.y;
    if (tp.y >= hp.y) sgn = 1;
    else sgn = -1;
    dx = ND_rw(n), dy = 0;
    tx = MIN(dx, 3*(np.x + dx - tp.x));
    hx = MIN(dx, 3*(np.x + dx - hp.x));
    for (i = 0; i < cnt; i++) {
        e = edges[ind++];
        dx += stepx, tx += stepx, hx += stepx, dy += sgn*stepy;
        pointn = 0;
        points[pointn++] = tp;
        points[pointn++] = pointfof(tp.x + tx / 3, tp.y + dy);
        points[pointn++] = pointfof(np.x + dx, tp.y + dy);
        points[pointn++] = pointfof(np.x + dx, (tp.y+hp.y)/2);
        points[pointn++] = pointfof(np.x + dx, hp.y - dy);
        points[pointn++] = pointfof(hp.x + hx / 3, hp.y - dy);
        points[pointn++] = hp;
        if (ED_label(e)) {
	    if (GD_flip(e->tail->graph)) {
		width = ED_label(e)->dimen.y;
		height = ED_label(e)->dimen.x;
	    } else {
		width = ED_label(e)->dimen.x;
		height = ED_label(e)->dimen.y;
	    }
	    ED_label(e)->pos.x = ND_coord(n).x + dx + width / 2.0;
	    ED_label(e)->pos.y = ND_coord(n).y;
	    ED_label(e)->set = TRUE;
	    if (width > stepx)
		dx += width - stepx;
	    if (dy + stepy < height)
		dy += height - stepy;
        }
        clip_and_install(e, e->head, points, pointn, sinfo);
#ifdef DEBUG
        if (debugleveln(e,1))
	    showPoints (points, pointn);
#endif
    }
    return;
}

static void
selfLeft (edge_t* edges[], int ind, int cnt, double stepx, double sizey,
          splineInfo* sinfo) 
{
    int i, sgn;
    double hx, tx, stepy, dx, dy, width, height; 
    pointf tp, hp, np;
    node_t *n;
    edge_t *e;
    pointf points[1000];
    int pointn;

    e = edges[ind];
    n = e->tail;

    stepy = (sizey / 2.) / cnt;
    stepy = MAX(stepy,2.);
    pointn = 0;
    np = ND_coord(n);
    tp = ED_tail_port(e).p;
    tp.x += np.x;
    tp.y += np.y;
    hp = ED_head_port(e).p;
    hp.x += np.x;
    hp.y += np.y;
    if (tp.y >= hp.y) sgn = 1;
    else sgn = -1;
    dx = ND_lw(n), dy = 0.;
    tx = MIN(dx, 3*(tp.x + dx - np.x));
    hx = MIN(dx, 3*(hp.x + dx - np.x));
    for (i = 0; i < cnt; i++) {
        e = edges[ind++];
        dx += stepx, tx += stepx, hx += stepx, dy += sgn*stepy;
        pointn = 0;
        points[pointn++] = tp;
        points[pointn++] = pointfof(tp.x - tx / 3, tp.y + dy);
        points[pointn++] = pointfof(np.x - dx, tp.y + dy);
        points[pointn++] = pointfof(np.x - dx, (tp.y+hp.y)/2);
        points[pointn++] = pointfof(np.x - dx, hp.y - dy);
        points[pointn++] = pointfof(hp.x - hx / 3, hp.y - dy);
        points[pointn++] = hp;
        if (ED_label(e)) {
    	if (GD_flip(e->tail->graph)) {
    	    width = ED_label(e)->dimen.y;
    	    height = ED_label(e)->dimen.x;
    	} else {
    	    width = ED_label(e)->dimen.x;
    	    height = ED_label(e)->dimen.y;
    	}
    	ED_label(e)->pos.x = ND_coord(n).x - dx - width / 2.0;
    	ED_label(e)->pos.y = ND_coord(n).y;
    	ED_label(e)->set = TRUE;
    	if (width > stepx)
    	    dx += width - stepx;
    	if (dy + stepy < height)
    	    dy += height - stepy;
        }
        clip_and_install(e, e->head, points, pointn, sinfo);
#ifdef DEBUG
        if (debugleveln(e,1))
	    showPoints (points, pointn);
#endif
    }
}

/* selfRightSpace:
 * Assume e is self-edge.
 * Return extra space necessary on the right for this edge.
 * If the edge does not go on the right, return 0.
 * NOTE: the actual space is determined dynamically by GD_nodesep,
 * so using the constant SELF_EDGE_SIZE is going to be wrong.
 * Fortunately, the default nodesep is the same as SELF_EDGE_SIZE.
 */
int
selfRightSpace (edge_t* e)
{
    int sw;
    double label_width;
    textlabel_t* l = ED_label(e);

    if (((!ED_tail_port(e).defined) && (!ED_head_port(e).defined)) ||
        (!(ED_tail_port(e).side & LEFT) && 
         !(ED_head_port(e).side & LEFT) &&
          ((ED_tail_port(e).side != ED_head_port(e).side) || 
          (!(ED_tail_port(e).side & (TOP|BOTTOM)))))) {
	sw = SELF_EDGE_SIZE;
	if (l) {
	    label_width = GD_flip(e->head->graph) ? l->dimen.y : l->dimen.x;
	    sw += label_width;
	}
    }
    else sw = 0;
    return sw;
}

/* makeSelfEdge:
 * The routing is biased towards the right side because this is what
 * dot supports, and leaves room for.
 * FIX: With this bias, labels tend to be placed on top of each other.
 * Perhaps for self-edges, the label should be centered.
 */
void
makeSelfEdge(path * P, edge_t * edges[], int ind, int cnt, double sizex,
	     double sizey, splineInfo * sinfo)
{
    edge_t *e;

    e = edges[ind];

    /* self edge without ports or
     * self edge with all ports inside, on the right, or at most 1 on top 
     * and at most 1 on bottom 
     */
    if (((!ED_tail_port(e).defined) && (!ED_head_port(e).defined)) ||
        (!(ED_tail_port(e).side & LEFT) && 
         !(ED_head_port(e).side & LEFT) &&
          ((ED_tail_port(e).side != ED_head_port(e).side) || 
          (!(ED_tail_port(e).side & (TOP|BOTTOM)))))) {
	selfRight(edges, ind, cnt, sizex, sizey, sinfo);
    }

    /* self edge with port on left side */
    else if ((ED_tail_port(e).side & LEFT) || (ED_head_port(e).side & LEFT)) {

	/* handle L-R specially */
	if ((ED_tail_port(e).side & RIGHT) || (ED_head_port(e).side & RIGHT)) {
	    selfTop(edges, ind, cnt, sizex, sizey, sinfo);
	}
	else {
	    selfLeft(edges, ind, cnt, sizex, sizey, sinfo);
	}
    }

    /* self edge with both ports on top side */
    else if (ED_tail_port(e).side & TOP) {
	selfTop(edges, ind, cnt, sizex, sizey, sinfo);
    }
    else if (ED_tail_port(e).side & BOTTOM) {
	selfBottom(edges, ind, cnt, sizex, sizey, sinfo);
    }

    else assert(0);
}

/* vladimir */
void place_portlabel(edge_t * e, boolean head_p)
/* place the {head,tail}label (depending on HEAD_P) of edge E */
/* N.B. Assume edges are normalized, so tail is at spl->list[0].list[0]
 * and head is at spl->list[spl->size-l].list[bez->size-1]
 */
{
    textlabel_t *l;
    splines *spl;
    bezier *bez;
    double dist, angle;
    pointf c[4], pe, pf;
    int i;

    if (ED_edge_type(e) == IGNORED)
	return;
    l = head_p ? ED_head_label(e) : ED_tail_label(e);
    spl = getsplinepoints(e);
    if (!head_p) {
	bez = &spl->list[0];
	if (bez->sflag) {
	    pe = bez->sp;
	    pf = bez->list[0];
	} else {
	    pe = bez->list[0];
	    for (i = 0; i < 4; i++)
		c[i] = bez->list[i];
	    pf = Bezier(c, 3, 0.1, NULL, NULL);
	}
    } else {
	bez = &spl->list[spl->size - 1];
	if (bez->eflag) {
	    pe = bez->ep;
	    pf = bez->list[bez->size - 1];
	} else {
	    pe = bez->list[bez->size - 1];
	    for (i = 0; i < 4; i++)
		c[i] = bez->list[bez->size - 4 + i];
	    pf = Bezier(c, 3, 0.9, NULL, NULL);
	}
    }
    angle = atan2(pf.y - pe.y, pf.x - pe.x) +
	RADIANS(late_double(e, E_labelangle, PORT_LABEL_ANGLE, -180.0));
    dist = PORT_LABEL_DISTANCE * late_double(e, E_labeldistance, 1.0, 0.0);
    l->pos.x = pe.x + dist * cos(angle);
    l->pos.y = pe.y + dist * sin(angle);
    l->set = TRUE;
}

splines *getsplinepoints(edge_t * e)
{
    edge_t *le;
    splines *sp;

    for (le = e; !(sp = ED_spl(le)) && ED_edge_type(le) != NORMAL;
	 le = ED_to_orig(le));
    if (sp == NULL)
	abort();
    return sp;
}
