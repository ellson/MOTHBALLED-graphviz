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


/* Module for packing disconnected graphs together.
 * Based on "Disconnected Graph Layout and the Polyomino Packing Approach", 
 * K. Freivalds et al., GD0'01, LNCS 2265, pp. 378-391.
 */

#include <pack.h>
#include <pointset.h>
#include <math.h>

#define C 100			/* Max. avg. polyomino size */

#define MOVEPT(p) ((p).x += dx, (p).y += dy)
#define GRID(x,s) (((x) + ((s)-1)) / (s))
#define CELL(p,s) ((p).x = (p).x/(s), (p).y = ((p).y/(s)))
#define SGN(a)    (((a)<0)? -1 : 1)

typedef struct {
    Agraph_t *graph;		/* related graph */
    int perim;			/* half size of bounding rectangle perimeter */
    point *cells;		/* cells in covering polyomino */
    int nc;			/* no. of cells */
    int index;			/* index in original array */

} ginfo;

/* computeStep:
 * Compute grid step size. This is a root of the
 * quadratic equation al^2 +bl + c, where a, b and
 * c are defined below.
 */
static int computeStep(int ng, Agraph_t ** gs, int margin)
{
    double l1, l2;
    double a, b, c, d, r;
    double W, H;		/* width and height of graph, with margin */
    Agraph_t *g;
    int i;

    a = C * ng - 1;
    c = 0;
    b = 0;
    for (i = 0; i < ng; i++) {
	g = gs[i];
	W = GD_bb(g).UR.x - GD_bb(g).LL.x + 2 * margin;
	H = GD_bb(g).UR.y - GD_bb(g).LL.y + 2 * margin;
	b -= (W + H);
	c -= (W * H);
    }
    d = b * b - 4.0 * a * c;
    if (d < 0) {
	agerr(AGERR, "libpack: disc = %f ( < 0)\n", d);
	return -1;
    }
    r = sqrt(d);
    l1 = (-b + r) / (2 * a);
    l2 = (-b - r) / (2 * a);
    if (Verbose > 2) {
	fprintf(stderr, "Packing: compute grid size\n");
	fprintf(stderr, "a %f b %f c %f d %f r %f\n", a, b, c, d, r);
	fprintf(stderr, "root %d (%f) %d (%f)\n", (int) l1, l1, (int) l2,
		l2);
	fprintf(stderr, " r1 %f r2 %f\n", a * l1 * l1 + b * l1 + c,
		a * l2 * l2 + b * l2 + c);
    }

    return (int) l1;
}

/* cmpf;
 * Comparison function for polyominoes.
 * Size is determined by perimeter.
 */
static int cmpf(const void *X, const void *Y)
{
    ginfo *x = *(ginfo **) X;
    ginfo *y = *(ginfo **) Y;
    /* flip order to get descending array */
    return (y->perim - x->perim);
}

/* fillLine:
 * Mark cells crossed by line from cell p to cell q.
 * Bresenham's algorithm, from Graphics Gems I, pp. 99-100.
 */
/* static  */
void fillLine(point p, point q, PointSet * ps)
{
    int x1 = p.x;
    int y1 = p.y;
    int x2 = q.x;
    int y2 = q.y;
    int d, x, y, ax, ay, sx, sy, dx, dy;

    dx = x2 - x1;
    ax = ABS(dx) << 1;
    sx = SGN(dx);
    dy = y2 - y1;
    ay = ABS(dy) << 1;
    sy = SGN(dy);

/* fprintf (stderr, "fillLine %d %d - %d %d\n", x1,y1,x2,y2); */
    x = x1;
    y = y1;
    if (ax > ay) {		/* x dominant */
	d = ay - (ax >> 1);
	for (;;) {
/* fprintf (stderr, "  addPS %d %d\n", x,y); */
	    addPS(ps, x, y);
	    if (x == x2)
		return;
	    if (d >= 0) {
		y += sy;
		d -= ax;
	    }
	    x += sx;
	    d += ay;
	}
    } else {			/* y dominant */
	d = ax - (ay >> 1);
	for (;;) {
/* fprintf (stderr, "  addPS %d %d\n", x,y); */
	    addPS(ps, x, y);
	    if (y == y2)
		return;
	    if (d >= 0) {
		x += sx;
		d -= ay;
	    }
	    y += sy;
	    d += ax;
	}
    }
}

/* fillEdge:
 * It appears that spline_edges always have the start point at the
 * beginning and the end point at the end.
 */
static void
fillEdge(Agedge_t * e, point pt, PointSet * ps, int dx, int dy,
	 int ssize, int doS)
{
    int j, k;
    bezier bz;
    point hpt;
    Agnode_t *h;

    /* If doS is false or the edge has not splines, use line segment */
    if (!doS || !ED_spl(e)) {
	h = e->head;
	hpt = coord(h);
	MOVEPT(hpt);
	CELL(hpt, ssize);
	fillLine(pt, hpt, ps);
	return;
    }

    for (j = 0; j < ED_spl(e)->size; j++) {
	bz = ED_spl(e)->list[j];
	if (bz.sflag) {
	    pt = bz.sp;
	    hpt = bz.list[0];
	    k = 1;
	} else {
	    pt = bz.list[0];
	    hpt = bz.list[1];
	    k = 2;
	}
	MOVEPT(pt);
	CELL(pt, ssize);
	MOVEPT(hpt);
	CELL(hpt, ssize);
	fillLine(pt, hpt, ps);

	for (; k < bz.size; k++) {
	    pt = hpt;
	    hpt = bz.list[k];
	    MOVEPT(hpt);
	    CELL(hpt, ssize);
	    fillLine(pt, hpt, ps);
	}

	if (bz.eflag) {
	    pt = hpt;
	    hpt = bz.ep;
	    MOVEPT(hpt);
	    CELL(hpt, ssize);
	    fillLine(pt, hpt, ps);
	}
    }

}

/* genBox:
 * Generate polyomino info from graph using the bounding box of
 * the graph.
 */
static void
genBox(Agraph_t * g, ginfo * info, int ssize, int margin, point center)
{
    PointSet *ps;
    int W, H;
    point UR, LL;
    box bb = GD_bb(g);
    int x, y;

    ps = newPS();

    LL.x = center.x - margin;
    LL.y = center.y - margin;
    UR.x = center.x + bb.UR.x - bb.LL.x + margin;
    UR.y = center.y + bb.UR.y - bb.LL.y + margin;
    CELL(LL, ssize);
    CELL(UR, ssize);

    for (x = LL.x; x <= UR.x; x++)
	for (y = LL.y; y <= UR.y; y++)
	    addPS(ps, x, y);

    info->graph = g;
    info->cells = pointsOf(ps);
    info->nc = sizeOf(ps);
    W = GRID(bb.UR.x - bb.LL.x + 2 * margin, ssize);
    H = GRID(bb.UR.y - bb.LL.y + 2 * margin, ssize);
    info->perim = W + H;

    if (Verbose > 2) {
	int i;
	fprintf(stderr, "%s no. cells %d W %d H %d\n", g->name, info->nc,
		W, H);
	for (i = 0; i < info->nc; i++)
	    fprintf(stderr, "  %d %d cell\n", info->cells[i].x,
		    info->cells[i].y);
    }

    freePS(ps);
}

/* genPoly:
 * Generate polyomino info from graph.
 * We add all cells covered partially by the bounding box of the 
 * node. If doSplines is true and an edge has a spline, we use the 
 * polyline determined by the control point. Otherwise,
 * we use each cell crossed by a straight edge between the head and tail.
 * If mode = l_clust, we use the graph's GD_clust array to treat the
 * top level clusters like large nodes.
 * Returns 0 if okay.
 */
static int
genPoly(Agraph_t * root, Agraph_t * g, ginfo * info,
	int ssize, pack_info * pinfo, point center)
{
    PointSet *ps;
    int W, H;
    point LL, UR;
    point pt, s2;
    Agraph_t *eg;		/* graph containing edges */
    Agnode_t *n;
    Agedge_t *e;
    int x, y;
    int dx, dy;
    graph_t *subg;
    int margin = pinfo->margin;
    int doSplines = pinfo->doSplines;
    box bb;

    if (root)
	eg = root;
    else
	eg = g;

    ps = newPS();
    dx = center.x - GD_bb(g).LL.x;
    dy = center.y - GD_bb(g).LL.y;

    if (pinfo->mode == l_clust) {
	int i;
	void **alg;

	/* backup the alg data */
	alg = N_GNEW(agnnodes(g), void *);
	for (i = 0, n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    alg[i++] = n->u.alg;
	    n->u.alg = 0;
	}

	/* do bbox of top clusters */
	for (i = 1; i <= GD_n_cluster(g); i++) {
	    subg = GD_clust(g)[i];
	    bb = GD_bb(subg);
	    if ((bb.UR.x > bb.LL.x) && (bb.UR.y > bb.LL.y)) {
		MOVEPT(bb.LL);
		MOVEPT(bb.UR);
		bb.LL.x -= margin;
		bb.LL.y -= margin;
		bb.UR.x += margin;
		bb.UR.y += margin;
		CELL(bb.LL, ssize);
		CELL(bb.UR, ssize);

		for (x = bb.LL.x; x <= bb.UR.x; x++)
		    for (y = bb.LL.y; y <= bb.UR.y; y++)
			addPS(ps, x, y);

		/* note which nodes are in clusters */
		for (n = agfstnode(subg); n; n = agnxtnode(subg, n))
		    ND_clust(n) = subg;
	    }
	}

	/* now do remaining nodes and edges */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    pt = coord(n);
	    MOVEPT(pt);
	    if (!ND_clust(n)) {	/* n is not in a top-level cluster */
		s2.x = margin + ND_xsize(n) / 2;
		s2.y = margin + ND_ysize(n) / 2;
		LL = sub_points(pt, s2);
		UR = add_points(pt, s2);
		CELL(LL, ssize);
		CELL(UR, ssize);

		for (x = LL.x; x <= UR.x; x++)
		    for (y = LL.y; y <= UR.y; y++)
			addPS(ps, x, y);

		CELL(pt, ssize);
		for (e = agfstout(eg, n); e; e = agnxtout(eg, e)) {
		    fillEdge(e, pt, ps, dx, dy, ssize, doSplines);
		}
	    } else {
		CELL(pt, ssize);
		for (e = agfstout(eg, n); e; e = agnxtout(eg, e)) {
		    if (ND_clust(n) == ND_clust(e->head))
			continue;
		    fillEdge(e, pt, ps, dx, dy, ssize, doSplines);
		}
	    }
	}

	/* restore the alg data */
	for (i = 0, n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    n->u.alg = alg[i++];
	}
	free(alg);

    } else
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    pt = coord(n);
	    MOVEPT(pt);
	    s2.x = margin + ND_xsize(n) / 2;
	    s2.y = margin + ND_ysize(n) / 2;
	    LL = sub_points(pt, s2);
	    UR = add_points(pt, s2);
	    CELL(LL, ssize);
	    CELL(UR, ssize);

	    for (x = LL.x; x <= UR.x; x++)
		for (y = LL.y; y <= UR.y; y++)
		    addPS(ps, x, y);

	    CELL(pt, ssize);
	    for (e = agfstout(eg, n); e; e = agnxtout(eg, e)) {
		fillEdge(e, pt, ps, dx, dy, ssize, doSplines);
	    }
	}

    info->graph = g;
    info->cells = pointsOf(ps);
    info->nc = sizeOf(ps);
    W = GRID(GD_bb(g).UR.x - GD_bb(g).LL.x + 2 * margin, ssize);
    H = GRID(GD_bb(g).UR.y - GD_bb(g).LL.y + 2 * margin, ssize);
    info->perim = W + H;

    if (Verbose > 2) {
	int i;
	fprintf(stderr, "%s no. cells %d W %d H %d\n", g->name, info->nc,
		W, H);
	for (i = 0; i < info->nc; i++)
	    fprintf(stderr, "  %d %d cell\n", info->cells[i].x,
		    info->cells[i].y);
    }

    freePS(ps);
    return 0;
}

/* fits:
 * Check if polyomino fits at given point.
 * If so, add cells to pointset, store point in place and return true.
 */
static int
fits(int x, int y, ginfo * info, PointSet * ps, point * place, int step)
{
    point *cells = info->cells;
    int n = info->nc;
    point cell;
    int i;
    point LL;

    for (i = 0; i < n; i++) {
	cell = *cells;
	cell.x += x;
	cell.y += y;
	if (inPS(ps, cell))
	    return 0;
	cells++;
    }

    LL = GD_bb(info->graph).LL;
    place->x = step * x - LL.x;
    place->y = step * y - LL.y;

    cells = info->cells;
    for (i = 0; i < n; i++) {
	cell = *cells;
	cell.x += x;
	cell.y += y;
	insertPS(ps, cell);
	cells++;
    }

    if (Verbose >= 2)
	fprintf(stderr, "cc (%d cells) at (%d,%d) (%d,%d)\n", n, x, y,
		place->x, place->y);
    return 1;
}

/* placeFixed:
 * Position fixed graph. Store final translation and
 * fill polyomino set. Note that polyomino set for the
 * graph is constructed where it will be.
 */
static void
placeFixed(ginfo * info, PointSet * ps, point * place, point center)
{
    point *cells = info->cells;
    int n = info->nc;
    int i;

    place->x = -center.x;
    place->y = -center.y;

    for (i = 0; i < n; i++) {
	insertPS(ps, *cells++);
    }

    if (Verbose >= 2)
	fprintf(stderr, "cc (%d cells) at (%d,%d)\n", n, place->x,
		place->y);
}

/* placeGraph:
 * Search for points on concentric "circles" out
 * from the origin. Check if polyomino can be placed
 * with bounding box origin at point.
 * First graph (i == 0) is centered on the origin if possible.
 */
static void
placeGraph(int i, ginfo * info, PointSet * ps, point * place, int step,
	   int margin)
{
    int x, y;
    int W, H;
    int bnd;

    if (i == 0) {
	Agraph_t *g = info->graph;
	W = GRID(GD_bb(g).UR.x - GD_bb(g).LL.x + 2 * margin, step);
	H = GRID(GD_bb(g).UR.y - GD_bb(g).LL.y + 2 * margin, step);
	if (fits(-W / 2, -H / 2, info, ps, place, step))
	    return;
    }

    if (fits(0, 0, info, ps, place, step))
	return;
    W = GD_bb(info->graph).UR.x - GD_bb(info->graph).LL.x;
    H = GD_bb(info->graph).UR.y - GD_bb(info->graph).LL.y;
    if (W >= H) {
	for (bnd = 1;; bnd++) {
	    x = 0;
	    y = -bnd;
	    for (; x < bnd; x++)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; y < bnd; y++)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; x > -bnd; x--)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; y > -bnd; y--)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; x < 0; x++)
		if (fits(x, y, info, ps, place, step))
		    return;
	}
    } else {
	for (bnd = 1;; bnd++) {
	    y = 0;
	    x = -bnd;
	    for (; y > -bnd; y--)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; x < bnd; x++)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; y < bnd; y++)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; x > -bnd; x--)
		if (fits(x, y, info, ps, place, step))
		    return;
	    for (; y > 0; y--)
		if (fits(x, y, info, ps, place, step))
		    return;
	}
    }
}

#ifdef DEBUG
void dumpp(ginfo * info, char *pfx)
{
    point *cells = info->cells;
    int i, c_cnt = info->nc;

    fprintf(stderr, "%s\n", pfx);
    for (i = 0; i < c_cnt; i++) {
	fprintf(stderr, "%d %d box\n", cells[i].x, cells[i].y);
    }
}
#endif

/* putGraphs:
 *  Given a collection of graphs, reposition them in the plane
 *  to not overlap but pack "nicely".
 *   ng is the number of graphs
 *   gs is a pointer to an array of graph pointers
 *   root gives the graph containing the edges; if null, the function
 *     looks in each graph in gs for its edges
 *   pinfo->margin gives the amount of extra space left around nodes in points
 *   If pinfo->doSplines is true, use edge splines, if computed,
 *     in calculating polyomino.
 *   pinfo->mode specifies the packing granularity and technique:
 *     l_node : pack at the node/cluster level
 *     l_graph : pack at the bounding box level
 *  Returns array of points to which graphs should be translated;
 *  the array needs to be freed;
 * Returns NULL if problem occur or if ng == 0.
 * 
 * Depends on graph fields bb, node fields pos, xsize and ysize, and
 * edge field spl.
 */
point *putGraphs(int ng, Agraph_t ** gs, Agraph_t * root,
		 pack_info * pinfo)
{
    int stepSize;
    ginfo *info;
    ginfo **sinfo;
    point *places;
    Dict_t *ps;
    int i;
    boolean *fixed = pinfo->fixed;
    int fixed_cnt = 0;
    box fixed_bb = { {0, 0}, {0, 0} };
    point center;

    if (ng <= 0)
	return 0;

    /* update bounding box info for each graph */
    /* If fixed, compute bbox of fixed graphs */
    for (i = 0; i < ng; i++) {
	Agraph_t *g = gs[i];
	compute_bb(g);
	if (fixed && fixed[i]) {
	    if (fixed_cnt) {
		box bb = GD_bb(g);
		fixed_bb.LL.x = MIN(bb.LL.x, fixed_bb.LL.x);
		fixed_bb.LL.y = MIN(bb.LL.y, fixed_bb.LL.y);
		fixed_bb.UR.x = MAX(bb.UR.x, fixed_bb.UR.x);
		fixed_bb.UR.y = MAX(bb.UR.y, fixed_bb.UR.y);
	    } else
		fixed_bb = GD_bb(g);
	    fixed_cnt++;
	}
	if (Verbose > 2) {
	    fprintf(stderr, "bb[%s] %d %d %d %d\n", g->name, GD_bb(g).LL.x,
		    GD_bb(g).LL.y, GD_bb(g).UR.x, GD_bb(g).UR.y);
	}
    }

    /* calculate grid size */
    stepSize = computeStep(ng, gs, pinfo->margin);
    if (Verbose)
	fprintf(stderr, "step size = %d\n", stepSize);
    if (stepSize < 0)
	return 0;

    /* generate polyomino cover for the graphs */
    if (fixed) {
	center.x = (fixed_bb.LL.x + fixed_bb.UR.x) / 2;
	center.y = (fixed_bb.LL.y + fixed_bb.UR.y) / 2;
    } else
	center.x = center.y = 0;
    info = N_NEW(ng, ginfo);
    for (i = 0; i < ng; i++) {
	info[i].index = i;
	if (pinfo->mode == l_graph)
	    genBox(gs[i], info + i, stepSize, pinfo->margin, center);
	else if (genPoly(root, gs[i], info + i, stepSize, pinfo, center)) {
	    return 0;
	}
    }

    /* sort */
    sinfo = N_NEW(ng, ginfo *);
    for (i = 0; i < ng; i++) {
	sinfo[i] = info + i;
    }
    qsort(sinfo, ng, sizeof(ginfo *), cmpf);

    ps = newPS();
    places = N_NEW(ng, point);
    if (fixed) {
	for (i = 0; i < ng; i++) {
	    if (fixed[i])
		placeFixed(sinfo[i], ps, places + (sinfo[i]->index),
			   center);
	}
	for (i = 0; i < ng; i++) {
	    if (!fixed[i])
		placeGraph(i, sinfo[i], ps, places + (sinfo[i]->index),
			   stepSize, pinfo->margin);
	}
    } else {
	for (i = 0; i < ng; i++)
	    placeGraph(i, sinfo[i], ps, places + (sinfo[i]->index),
		       stepSize, pinfo->margin);
    }

    free(sinfo);
    for (i = 0; i < ng; i++)
	free(info[i].cells);
    free(info);
    freePS(ps);

    if (Verbose > 1)
	for (i = 0; i < ng; i++)
	    fprintf(stderr, "pos[%d] %d %d\n", i, places[i].x,
		    places[i].y);

    return places;
}

/* shiftEdge:
 * Translate all of the edge components by the given offset.
 */
static void shiftEdge(Agedge_t * e, int dx, int dy)
{
    int j, k;
    bezier bz;

    if (ED_label(e))
	MOVEPT(ED_label(e)->p);
    if (ED_head_label(e))
	MOVEPT(ED_head_label(e)->p);
    if (ED_tail_label(e))
	MOVEPT(ED_tail_label(e)->p);

    if (ED_spl(e) == NULL)
	return;

    for (j = 0; j < ED_spl(e)->size; j++) {
	bz = ED_spl(e)->list[j];
	for (k = 0; k < bz.size; k++)
	    MOVEPT(bz.list[k]);
	if (bz.sflag)
	    MOVEPT(ED_spl(e)->list[j].sp);
	if (bz.eflag)
	    MOVEPT(ED_spl(e)->list[j].ep);
    }
}

/* shiftGraph:
 */
static void shiftGraph(Agraph_t * g, int dx, int dy)
{
    graph_t *subg;
    box bb = GD_bb(g);
    int i;

    bb.LL.x += dx;
    bb.UR.x += dx;
    bb.LL.y += dy;
    bb.UR.y += dy;
    GD_bb(g) = bb;

    if (GD_label(g))
	MOVEPT(GD_label(g)->p);

    for (i = 1; i <= GD_n_cluster(g); i++) {
	subg = GD_clust(g)[i];
	shiftGraph(subg, dx, dy);
    }
}

/* shiftGraphs:
 * Uses points computed from putGraphs to shift points, edges and clusters.
 * Always shifts pos.
 * If doSplines is true, assumes node position is also in coord, 
 * and edges and edge labels have been placed, so it shifts those.
 * If root is non-null, it is used to find edges.
 *
 * Depends on graph field bb, node field pos and coord, and edge field spl.
 * as well as labels in graphs and edges
 */
int
shiftGraphs(int ng, Agraph_t ** gs, point * pp, Agraph_t * root,
	    int doSplines)
{
    int i;
    int dx, dy;
    double fx, fy;
    point p;
    Agraph_t *g;
    Agraph_t *eg;
    Agnode_t *n;
    Agedge_t *e;

    if (ng <= 0)
	return abs(ng);

    for (i = 0; i < ng; i++) {
	g = gs[i];
	if (root)
	    eg = root;
	else
	    eg = g;
	p = pp[i];
	dx = p.x;
	dy = p.y;
	fx = PS2INCH(dx);
	fy = PS2INCH(dy);

	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    ND_pos(n)[0] += fx;
	    ND_pos(n)[1] += fy;
	    if (doSplines) {
		MOVEPT(ND_coord_i(n));
		for (e = agfstout(eg, n); e; e = agnxtout(eg, e))
		    shiftEdge(e, dx, dy);
	    }
	}
	shiftGraph(g, dx, dy);
    }

    return 0;
}

/* packGraphs:
 * Packs graphs.
 *  ng - number of graphs
 *  gs - pointer to array of graphs
 *  root - graph used to find edges
 *  info - parameters used in packing
 *  info->doSplines - if true, use already computed spline control points
 * This decides where to layout the graphs and repositions the graph's
 * position info.
 *
 * Returns 0 on success.
 */
int packGraphs(int ng, Agraph_t ** gs, Agraph_t * root, pack_info * info)
{
    int ret;
    point *pp = putGraphs(ng, gs, root, info);

    if (!pp)
	return 1;
    ret = shiftGraphs(ng, gs, pp, root, info->doSplines);
    free(pp);
    return ret;
}

/* packSubgraphs:
 *  Packs subgraphs of given root graph, then recalculates root's bounding
 * box.
 * Note that it does not recompute subgraph bounding boxes.
 * Cluster bounding boxes are recomputed in shiftGraphs.
 */
int
packSubgraphs(int ng, Agraph_t ** gs, Agraph_t * root, pack_info * info)
{
    int ret;

    ret = packGraphs(ng, gs, root, info);
    if (ret == 0)
	compute_bb(root);
    return ret;
}

/* getPackMode;
 * Return pack_mode of graph using "packmode" attribute.
 * If not defined, return dflt
 */
pack_mode getPackMode(Agraph_t * g, pack_mode dflt)
{
    char *p = agget(g, "packmode");
    pack_mode mode = dflt;

    if (p && *p) {
	switch (*p) {
#ifdef NOT_IMPLEMENTED
	case 'b':
	    if (streq(p, "bisect"))
		mode = l_bisect;
	    break;
#endif
	case 'c':
	    if (streq(p, "cluster"))
		mode = l_clust;
	    break;
	case 'g':
	    if (streq(p, "graph"))
		mode = l_graph;
	    break;
#ifdef NOT_IMPLEMENTED
	case 'h':
	    if (streq(p, "hull"))
		mode = l_hull;
	    break;
#endif
	case 'n':
	    if (streq(p, "node"))
		mode = l_node;
	    break;
#ifdef NOT_IMPLEMENTED
	case 't':
	    if (streq(p, "tile"))
		mode = l_tile;
	    break;
#endif
	}
    }
    return mode;
}

/* getPack;
 * Return "pack" attribute of g.
 * If not defined or negative, return not_def.
 * If defined but not specified, return dflt.
 */
int getPack(Agraph_t * g, int not_def, int dflt)
{
    char *p;
    int i;
    int v = not_def;

    if ((p = agget(g, "pack"))) {
	if ((sscanf(p, "%d", &i) == 1) && (i >= 0))
	    v = i;
	else if ((*p == 't') || (*p == 'T'))
	    v = dflt;
    }

    return v;
}
