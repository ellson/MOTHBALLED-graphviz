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

#include <render.h>

/* wantclip:
 * Return false if head/tail end of edge should not be clipped
 * to node.
 */
static boolean wantclip(edge_t * e, node_t * n)
{
    char *str;
    attrsym_t *sym = 0;
    boolean rv = TRUE;

    if (n == e->tail)
	sym = E_tailclip;
    if (n == e->head)
	sym = E_headclip;
    if (sym) {			/* mapbool isn't a good fit, because we want "" to mean TRUE */
	str = agxget(e, sym->index);
	if (str && str[0])
	    rv = mapbool(str);
	else
	    rv = TRUE;
    }
    return rv;
}

/* arrow_clip:
 * Clip arrow to node boundary.
 * The real work is done elsewhere. Here we get the real edge,
 * check that the edge has arrowheads, and that an endpoint
 * isn't a merge point where several parts of an edge meet.
 * (e.g., with edge concentrators).
 */
static void
arrow_clip(edge_t * fe, edge_t * le,
	   point * ps, int *startp, int *endp,
	   bezier * spl, splineInfo * info)
{
    edge_t *e;
    int i, j, sflag, eflag;
    inside_t inside_context;

    for (e = fe; ED_to_orig(e); e = ED_to_orig(e));
    inside_context.e = e;

    j = info->swapEnds(e);
    arrow_flags(e, &sflag, &eflag);
    if (info->splineMerge(le->head))
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
	    arrowStartClip(&inside_context, ps, *startp, *endp, spl,
			   sflag);
    if (eflag)
	*endp =
	    arrowEndClip(&inside_context, ps, *startp, *endp, spl, eflag);
}

/* bezier_clip
 * Clip bezier to shape using binary search.
 * The details of the shape are passed in the inside_context;
 * The function providing the inside test is passed as a parameter.
 * left_inside specifies that sp[0] is inside the node, else * sp[3] is taken as inside.
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
shape_clip0(inside_t * inside_context, node_t * n, point curve[4],
	    boolean left_inside)
{
    int i, save_real_size;
    pointf c[4];

    save_real_size = ND_rw_i(n);
    for (i = 0; i < 4; i++) {
	c[i].x = curve[i].x - ND_coord_i(n).x;
	c[i].y = curve[i].y - ND_coord_i(n).y;
    }

    bezier_clip(inside_context, ND_shape(n)->fns->insidefn, c,
		left_inside);

    for (i = 0; i < 4; i++) {
	curve[i].x = ROUND(c[i].x + ND_coord_i(n).x);
	curve[i].y = ROUND(c[i].y + ND_coord_i(n).y);
    }
    ND_rw_i(n) = save_real_size;
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
 */
void shape_clip(node_t * n, point curve[4], edge_t * e)
{
    int save_real_size;
    boolean left_inside;
    pointf c;
    inside_t inside_context;

    if (ND_shape(n) == NULL || ND_shape(n)->fns->insidefn == NULL)
	return;

    inside_context.n = n;
    inside_context.e = e;
    save_real_size = ND_rw_i(n);
    c.x = curve[0].x - ND_coord_i(n).x;
    c.y = curve[0].y - ND_coord_i(n).y;
    left_inside = ND_shape(n)->fns->insidefn(&inside_context, c);
    ND_rw_i(n) = save_real_size;
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
    rv->list = N_NEW(sz, point);
    rv->size = sz;
    rv->sflag = rv->eflag = FALSE;
    return rv;
}

/* update_bb:
 * Update the bounding box of g based on the addition of
 * point p.
 */
static void update_bb(graph_t * g, point pt)
{
    if (pt.x > GD_bb(g).UR.x)
	GD_bb(g).UR.x = pt.x;
    if (pt.y > GD_bb(g).UR.y)
	GD_bb(g).UR.y = pt.y;
    if (pt.x < GD_bb(g).LL.x)
	GD_bb(g).LL.x = pt.x;
    if (pt.y < GD_bb(g).LL.y)
	GD_bb(g).LL.y = pt.y;
}

/* clip_and_install:
 * Given a raw spline (pn control points in ps), representing
 * a path from edge fe ending in edge le, clip the ends to
 * the node boundaries and attach the resulting spline to the
 * edge.
 */
void
clip_and_install(edge_t * fe, edge_t * le, point * ps, int pn,
		 splineInfo * info)
{
    pointf p2;
    bezier *newspl;
    node_t *tn, *hn;
    int start, end, i;
    graph_t *g;
    edge_t *orig;
    inside_t inside_context;

    tn = fe->tail;
    hn = le->head;
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

    /* spline may be interior to node */
    if (wantclip(orig, tn) && ND_shape(tn) && ND_shape(tn)->fns->insidefn) {
	inside_context.n = tn;
	inside_context.e = fe;
	for (start = 0; start < pn - 4; start += 3) {
	    p2.x = ps[start + 3].x - ND_coord_i(tn).x;
	    p2.y = ps[start + 3].y - ND_coord_i(tn).y;
	    if (ND_shape(tn)->fns->insidefn(&inside_context, p2) == FALSE)
		break;
	}
	shape_clip0(&inside_context, tn, &ps[start], TRUE);
    } else
	start = 0;
    if (wantclip(orig, hn) && ND_shape(hn) && ND_shape(hn)->fns->insidefn) {
	inside_context.n = hn;
	inside_context.e = le;
	for (end = pn - 4; end > 0; end -= 3) {
	    p2.x = ps[end].x - ND_coord_i(hn).x;
	    p2.y = ps[end].y - ND_coord_i(hn).y;
	    if (ND_shape(hn)->fns->insidefn(&inside_context, p2) == FALSE)
		break;
	}
	shape_clip0(&inside_context, hn, &ps[end], FALSE);
    } else
	end = pn - 4;
    for (; start < pn - 4; start += 3)
	if (ps[start].x != ps[start + 3].x
	    || ps[start].y != ps[start + 3].y)
	    break;
    for (; end > 0; end -= 3)
	if (ps[end].x != ps[end + 3].x || ps[end].y != ps[end + 3].y)
	    break;
    arrow_clip(fe, le, ps, &start, &end, newspl, info);
    for (i = start; i < end + 4; i++) {
	point pt;
	pt = newspl->list[i - start] = ps[i];
	update_bb(g, pt);
    }
    newspl->size = end - start + 4;
}

static double conc_slope(node_t * n)
{
    double s_in, s_out, m_in, m_out;
    int cnt_in, cnt_out;
    pointf p;
    edge_t *e;

    s_in = s_out = 0.0;
    for (cnt_in = 0; (e = ND_in(n).list[cnt_in]); cnt_in++)
	s_in += ND_coord_i(e->tail).x;
    for (cnt_out = 0; (e = ND_out(n).list[cnt_out]); cnt_out++)
	s_out += ND_coord_i(e->head).x;
    p.x = ND_coord_i(n).x - (s_in / cnt_in);
    p.y = ND_coord_i(n).y - ND_coord_i(ND_in(n).list[0]->tail).y;
    m_in = atan2(p.y, p.x);
    p.x = (s_out / cnt_out) - ND_coord_i(n).x;
    p.y = ND_coord_i(ND_out(n).list[0]->head).y - ND_coord_i(n).y;
    m_out = atan2(p.y, p.x);
    return ((m_in + m_out) / 2.0);
}

void add_box(path * P, box b)
{
    P->boxes[P->nbox++] = b;
}

/* beginpath:
 * Set up boxes near the tail node.
 */
void
beginpath(path * P, edge_t * e, int et, pathend_t * endp, boolean merge)
{
    int mask;
    node_t *n;
    int (*pboxfn) (node_t * n, edge_t * e, int, box *, int *);

    n = e->tail;

    if (ND_shape(n))
	pboxfn = ND_shape(n)->fns->pboxfn;
    else
	pboxfn = NULL;
    P->start.p = add_points(ND_coord_i(n), ED_tail_port(e).p);
    P->ulpp = P->urpp = P->llpp = P->lrpp = NULL;
    if (merge) {
	/*P->start.theta = - PI / 2; */
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
    /* FIXME: check that record_path returns a good path */
    if (pboxfn
	&& (mask = (*pboxfn) (n, e, 1, &endp->boxes[0], &endp->boxn)))
	endp->sidemask = mask;
    else {
	endp->boxes[0] = endp->nb;
	endp->boxn = 1;
    }
    switch (et) {
    case SELFEDGE:
	/* moving the box UR.y by + 1 avoids colinearity between
	   port point and box that confuses Proutespline().  it's
	   a bug in Proutespline() but this is the easiest fix. */
	endp->boxes[0].UR.y = P->start.p.y - 1;
	endp->sidemask = BOTTOM;
	break;
    case FLATEDGE:
	endp->boxes[0].LL.y = P->start.p.y;
	endp->sidemask = TOP;
	break;
    case REGULAREDGE:
	endp->boxes[0].UR.y = P->start.p.y;
	endp->sidemask = BOTTOM;
	break;
    }
}

void endpath(path * P, edge_t * e, int et, pathend_t * endp, boolean merge)
{
    int mask;
    node_t *n;
    int (*pboxfn) (node_t * n, edge_t * e, int, box *, int *);

    n = e->head;

    if (ND_shape(n))
	pboxfn = ND_shape(n)->fns->pboxfn;
    else
	pboxfn = NULL;
    P->end.p = add_points(ND_coord_i(n), ED_head_port(e).p);
    if (merge) {
	/*P->end.theta = PI / 2; */
	P->end.theta = conc_slope(e->head) + PI;
	assert(P->end.theta < 2 * PI);
	P->end.constrained = TRUE;
    } else {
	if (ED_head_port(e).constrained) {
	    P->end.theta = ED_head_port(e).theta;
	    P->end.constrained = TRUE;
	} else
	    P->end.constrained = FALSE;
    }
    endp->np = P->end.p;
    if (pboxfn
	&& (mask = (*pboxfn) (n, e, 2, &endp->boxes[0], &endp->boxn)))
	endp->sidemask = mask;
    else {
	endp->boxes[0] = endp->nb;
	endp->boxn = 1;
    }
    switch (et) {
    case SELFEDGE:
	/* offset of -1 is symmetric w.r.t. beginpath() 
	 * FIXME: is any of this right?  what if self-edge 
	 * doesn't connect from BOTTOM to TOP??? */
	endp->boxes[0].LL.y = P->end.p.y + 1;
	endp->sidemask = TOP;
	break;
    case FLATEDGE:
	endp->boxes[0].LL.y = P->end.p.y;
	endp->sidemask = TOP;
	break;
    case REGULAREDGE:
	endp->boxes[0].LL.y = P->end.p.y;
	endp->sidemask = TOP;
	break;
    }
}

/* self edges */
#define ANYW  0			/* could go either way */

static int selfsidemap[16][3] = {
    {BOTTOM, BOTTOM, ANYW},
    {TOP, TOP, ANYW},
    {RIGHT, RIGHT, ANYW},
    {LEFT, LEFT, ANYW},
    {BOTTOM, LEFT, CCW},
    {LEFT, BOTTOM, CW},
    {TOP, RIGHT, CW},
    {RIGHT, TOP, CCW},
    {TOP, LEFT, CCW},
    {LEFT, TOP, CW},
    {BOTTOM, RIGHT, CCW},
    {RIGHT, BOTTOM, CW},
    {BOTTOM, TOP, CCW},
    {TOP, BOTTOM, CW},
    {LEFT, RIGHT, CCW},
    {RIGHT, LEFT, CW},
};

static void
chooseselfsides(pathend_t * tendp, pathend_t * hendp,
		int *tsidep, int *hsidep, int *dirp)
{
    int i;

    for (i = 0; i < 16; i++)
	if ((selfsidemap[i][0] & tendp->sidemask) &&
	    (selfsidemap[i][1] & hendp->sidemask))
	    break;
    if (i == 16)
	abort();
    *tsidep = selfsidemap[i][0], *hsidep = selfsidemap[i][1];
    *dirp = selfsidemap[i][2];
    if (*dirp == ANYW) {	/* ANYW can appear when tside == hside */
	switch (*tsidep) {
	case BOTTOM:
	    *dirp = (tendp->np.x < hendp->np.x) ? CCW : CW;
	    break;
	case RIGHT:
	    *dirp = (tendp->np.y < hendp->np.y) ? CCW : CW;
	    break;
	case TOP:
	    *dirp = (tendp->np.x > hendp->np.x) ? CCW : CW;
	    break;
	case LEFT:
	    *dirp = (tendp->np.y > hendp->np.y) ? CCW : CW;
	    break;
	}
    }
}

static box makeselfend(box b, int side, int dir, int dx, int dy)
{
    box eb = { {0, 0}, {0, 0} };

    switch (side) {
    case BOTTOM:
	eb = boxof(b.LL.x, b.LL.y - dy, b.UR.x, b.LL.y);
	(dir == CCW) ? (eb.UR.x += dx / 2) : (eb.LL.x -= dx / 2);
	break;
    case RIGHT:
	eb = boxof(b.UR.x, b.LL.y, b.UR.x + dx, b.UR.y);
	(dir == CCW) ? (eb.UR.y += dy / 2) : (eb.LL.y -= dy / 2);
	break;
    case TOP:
	eb = boxof(b.LL.x, b.UR.y, b.UR.x, b.UR.y + dy);
	(dir == CCW) ? (eb.LL.x -= dx / 2) : (eb.UR.x += dx / 2);
	break;
    case LEFT:
	eb = boxof(b.LL.x - dx, b.LL.y, b.LL.x, b.UR.y);
	(dir == CCW) ? (eb.LL.y -= dy / 2) : (eb.UR.y += dy / 2);
	break;
    }
    return eb;
}

static box
makeselfcomponent(box nb, int side, int dx, int dy, int w, int h)
{
    box b = { {0, 0}, {0, 0} };

    switch (side) {
    case BOTTOM:
	b.LL.x = nb.LL.x - dx - w, b.LL.y = nb.LL.y - dy - h;
	b.UR.x = nb.UR.x + dx + w, b.UR.y = b.LL.y + h;
	break;
    case RIGHT:
	b.LL.x = nb.UR.x + dx, b.LL.y = nb.LL.y - dy;
	b.UR.x = b.LL.x + w, b.UR.y = nb.UR.y + dy;
	break;
    case TOP:
	b.LL.x = nb.LL.x - dx - w, b.LL.y = nb.UR.y + dy;
	b.UR.x = nb.UR.x + dx + w, b.UR.y = b.LL.y + h;
	break;
    case LEFT:
	b.LL.x = nb.LL.x - dx - w, b.LL.y = nb.LL.y - dy;
	b.UR.x = b.LL.x + w, b.UR.y = nb.UR.y + dy;
	break;
    }
    return b;
}

static void
adjustselfends(box * tbp, box * hbp, point p, int side, int dir)
{
    switch (side) {
    case BOTTOM:
	if (dir == CCW) {
	    tbp->LL.x -= (tbp->UR.x - p.x), tbp->UR.x = p.x;
	    hbp->UR.x += (p.x - hbp->LL.x), hbp->LL.x = p.x;
	} else {
	    tbp->UR.x -= (tbp->LL.x - p.x), tbp->LL.x = p.x;
	    hbp->LL.x += (p.x - hbp->UR.x), hbp->UR.x = p.x;
	}
	break;
    case RIGHT:
	if (dir == CCW) {
	    tbp->LL.y -= (tbp->UR.y - p.y), tbp->UR.y = p.y;
	    hbp->UR.y += (p.y - hbp->LL.y), hbp->LL.y = p.y;
	} else {
	    tbp->UR.y -= (tbp->LL.y - p.y), tbp->LL.y = p.y;
	    hbp->LL.y += (p.y - hbp->UR.y), hbp->UR.y = p.y;
	}
	break;
    case TOP:
	if (dir == CW) {
	    tbp->LL.x -= (tbp->UR.x - p.x), tbp->UR.x = p.x;
	    hbp->UR.x += (p.x - hbp->LL.x), hbp->LL.x = p.x;
	} else {
	    tbp->UR.x -= (tbp->LL.x - p.x), tbp->LL.x = p.x;
	    hbp->LL.x += (p.x - hbp->UR.x), hbp->UR.x = p.x;
	}
	break;
    case LEFT:
	if (dir == CW) {
	    tbp->LL.y -= (tbp->UR.y - p.y), tbp->UR.y = p.y;
	    hbp->UR.y += (p.y - hbp->LL.y), hbp->LL.y = p.y;
	} else {
	    tbp->UR.y -= (tbp->LL.y - p.y), tbp->LL.y = p.y;
	    hbp->LL.y += (p.y - hbp->UR.y), hbp->UR.y = p.y;
	}
	break;
    }
}

static void
completeselfpath(path * P, pathend_t * tendp, pathend_t * hendp,
		 int tside, int hside, int dir, int dx, int dy, int w,
		 int h)
{
    int i, side;
    box boxes[4];		/* can't have more than 6 boxes */
    box tb, hb;
    int boxn;

    tb = makeselfend(tendp->boxes[tendp->boxn - 1], tside, dir, dx, dy);
    hb = makeselfend(hendp->boxes[hendp->boxn - 1],
		     hside, OTHERDIR(dir), dx, dy);

    if (tside == hside && tendp->np.x == hendp->np.x &&
	tendp->np.y == hendp->np.y)
	adjustselfends(&tb, &hb, tendp->np, tside, dir);

    boxn = 0;
    for (side = tside;; side = NEXTSIDE(side, dir)) {
	boxes[boxn++] = makeselfcomponent(tendp->nb, side, dx, dy, w, h);
	if (side == hside)
	    break;
    }
    for (i = 0; i < tendp->boxn; i++)
	add_box(P, tendp->boxes[i]);
    add_box(P, tb);
    for (i = 0; i < boxn; i++)
	add_box(P, boxes[i]);
    add_box(P, hb);
    for (i = hendp->boxn - 1; i >= 0; i--)
	add_box(P, hendp->boxes[i]);
}

void
makeSelfEdge(path * P, edge_t * edges[], int ind, int cnt, int stepx,
	     splineInfo * sinfo)
{
    node_t *n;
    edge_t *e;
    point *ps, np;
    pathend_t tend, hend;
    int i, j, maxx, stepy, dx, dy, tside, hside, dir, pn;
    double width, height;
    point points[1000];
    int pointn;

    e = edges[ind];
    n = e->tail;

    /* self edge without ports */

    if ((!ED_tail_port(e).defined) && (!ED_head_port(e).defined)) {
	stepy = (ND_ht_i(n) / 2) / cnt;
	pointn = 0;
	np = ND_coord_i(n);
	dx = ND_rw_i(n), dy = 0;
	for (i = 0; i < cnt; i++) {
	    e = edges[ind++];
	    dx += stepx, dy += stepy;
	    pointn = 0;
	    points[pointn++] = np;
	    points[pointn++] = pointof(np.x + dx / 3, np.y - dy);
	    points[pointn++] = pointof(np.x + dx, np.y - dy);
	    points[pointn++] = pointof(np.x + dx, np.y);
	    points[pointn++] = pointof(np.x + dx, np.y + dy);
	    points[pointn++] = pointof(np.x + dx / 3, np.y + dy);
	    points[pointn++] = np;
	    if (ED_label(e)) {
		if (GD_flip(e->tail->graph)) {
		    width = ED_label(e)->dimen.y;
		    height = ED_label(e)->dimen.x;
		} else {
		    width = ED_label(e)->dimen.x;
		    height = ED_label(e)->dimen.y;
		}
		ED_label(e)->p.x = ND_coord_i(n).x + dx + width / 2.0;
		ED_label(e)->p.y = ND_coord_i(n).y;
		ED_label(e)->set = TRUE;
		if (width > stepx)
		    dx += width - stepx;
		if (dy + stepy < height)
		    dy += height - stepy;
	    }
	    clip_and_install(e, e, points, pointn, sinfo);
	}
	return;
    }

    /* self edge with ports */

    tend.nb =
	boxof(ND_coord_i(n).x - ND_lw_i(n),
	      ND_coord_i(n).y - ND_ht_i(n) / 2,
	      ND_coord_i(n).x + ND_rw_i(n),
	      ND_coord_i(n).y + ND_ht_i(n) / 2);
    hend.nb = tend.nb;
    stepy = stepx / 2;
    dx = 0, dy = 0;
    for (i = 0; i < cnt; i++) {
	e = edges[ind++];
	dx += stepx, dy += stepy;

	/* tail setup */
	beginpath(P, e, SELFEDGE, &tend, sinfo->splineMerge(e->tail));

	/* head setup */
	endpath(P, e, SELFEDGE, &hend, sinfo->splineMerge(e->head));

	chooseselfsides(&tend, &hend, &tside, &hside, &dir);
	completeselfpath(P, &tend, &hend, tside, hside, dir,
			 dx, dy, stepx, stepx);

	ps = routesplines(P, &pn);
	if (pn == 0)
	    return;		/* will result in a lost edge */
	if (ED_label(e)) {
	    /* FIXME: labels only right for BOTTOM -> TOP edges */
	    for (j = 0, maxx = ND_coord_i(n).x; j < P->nbox; j++)
		if (P->boxes[j].UR.x > maxx)
		    maxx = P->boxes[j].UR.x;
	    if (GD_flip(e->tail->graph))
		width = ED_label(e)->dimen.y;
	    else
		width = ED_label(e)->dimen.x;
	    ED_label(e)->p.x = maxx + width / 2.0;
	    ED_label(e)->p.y = ND_coord_i(n).y;
	    ED_label(e)->set = TRUE;
	    if (width > stepx)
		dx += width - stepx;
	}
	clip_and_install(e, e, ps, pn, sinfo);
    }
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
    point p;
    pointf c[4], pf;
    int i;

    if (ED_edge_type(e) == IGNORED)
	return;
    l = head_p ? ED_head_label(e) : ED_tail_label(e);
    spl = getsplinepoints(e);
    if (!head_p) {
	bez = &spl->list[0];
	if (bez->sflag) {
	    p = bez->sp;
	    P2PF(bez->list[0], pf);
	} else {
	    p = bez->list[0];
	    for (i = 0; i < 4; i++)
		P2PF(bez->list[i], c[i]);
	    pf = Bezier(c, 3, 0.1, NULL, NULL);
	}
    } else {
	bez = &spl->list[spl->size - 1];
	if (bez->eflag) {
	    p = bez->ep;
	    P2PF(bez->list[bez->size - 1], pf);
	} else {
	    p = bez->list[bez->size - 1];
	    for (i = 0; i < 4; i++)
		P2PF(bez->list[bez->size - 4 + i], c[i]);
	    pf = Bezier(c, 3, 0.9, NULL, NULL);
	}
    }
    angle = atan2(pf.y - p.y, pf.x - p.x) +
	RADIANS(late_double(e, E_labelangle, PORT_LABEL_ANGLE, -180.0));
    dist = PORT_LABEL_DISTANCE * late_double(e, E_labeldistance, 1.0, 0.0);
    l->p.x = p.x + ROUND(dist * cos(angle));
    l->p.y = p.y + ROUND(dist * sin(angle));
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
