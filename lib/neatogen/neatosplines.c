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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "neato.h"
#include "adjust.h"
#include "pathplan.h"
#include "vispath.h"
#ifndef HAVE_DRAND48
extern double drand48(void);
#endif

#define P2PF(p, pf) (pf.x = p.x, pf.y = p.y)
#define PF2P(pf, p) (p.x = ROUND (pf.x), p.y = ROUND (pf.y))

extern void printvis(vconfig_t * cp);
extern int in_poly(Ppoly_t argpoly, Ppoint_t q);

static bool spline_merge(node_t * n)
{
    return FALSE;
}

static bool swap_ends_p(edge_t * e)
{
    return FALSE;
}

static splineInfo sinfo = { swap_ends_p, spline_merge };

static void
make_barriers(Ppoly_t ** poly, int npoly, int pp, int qp,
	      Pedge_t ** barriers, int *n_barriers)
{
    int i, j, k, n, b;
    Pedge_t *bar;

    n = 0;
    for (i = 0; i < npoly; i++) {
	if (i == pp)
	    continue;
	if (i == qp)
	    continue;
	n = n + poly[i]->pn;
    }
    bar = N_GNEW(n, Pedge_t);
    b = 0;
    for (i = 0; i < npoly; i++) {
	if (i == pp)
	    continue;
	if (i == qp)
	    continue;
	for (j = 0; j < poly[i]->pn; j++) {
	    k = j + 1;
	    if (k >= poly[i]->pn)
		k = 0;
	    bar[b].a = poly[i]->ps[j];
	    bar[b].b = poly[i]->ps[k];
	    b++;
	}
    }
    assert(b == n);
    *barriers = bar;
    *n_barriers = n;
}

/* recPt:
 */
static Ppoint_t recPt(double x, double y, point c, double sep)
{
    Ppoint_t p;

    p.x = (x * sep) + c.x;
    p.y = (y * sep) + c.y;
    return p;
}

static void makePortLabels(edge_t * e)
{
    if (ED_head_label(e) && !ED_head_label(e)->set) {
	place_portlabel(e, TRUE);
	updateBB(e->tail->graph, ED_head_label(e));
    }
    if (ED_tail_label(e) && !ED_tail_label(e)->set) {
	place_portlabel(e, FALSE);
	updateBB(e->tail->graph, ED_tail_label(e));
    }
}

/* endPoints:
 * Extract the actual end points of the spline, where
 * they touch the node.
 */
static void endPoints(splines * spl, point * p, point * q)
{
    bezier bz;

    bz = spl->list[0];
    if (bz.sflag)
	*p = bz.sp;
    else
	*p = bz.list[0];
    bz = spl->list[spl->size - 1];
    if (bz.eflag)
	*q = bz.ep;
    else
	*q = bz.list[bz.size - 1];
}

#define LEFTOF(a,b,c) (((a.y - b.y)*(c.x - b.x) - (c.y - b.y)*(a.x - b.x)) > 0)
#define MAXLABELWD  (POINTS_PER_INCH/2.0)

/* addEdgeLabels:
 * rp and rq are the port points of the tail and head node.
 * Adds label, headlabel and taillabel.
 * The use of 2 and 4 in computing ld.x and ld.y are fudge factors, to
 * introduce a bit of spacing.
 * Updates bounding box.
 * We try to use the actual endpoints of the spline, as they may differ
 * significantly from rp and rq, but if the spline is degenerate (e.g.,
 * the nodes overlap), we use rp and rq.
 */
static void addEdgeLabels(edge_t * e, point rp, point rq)
{
    point p, q;
    point d;			/* midpoint of segment p-q */
    point ld;
    point sp;
    point del;
    pointf spf;
    double f, ht, wd, dist2;
    int leftOf;

    if (ED_label(e) && !ED_label(e)->set) {
	endPoints(ED_spl(e), &p, &q);
	if ((p.x == q.x) && (p.y == q.y)) { /* degenerate spline */
	    p = rp;
	    q = rq;
	}
	d.x = (q.x + p.x) / 2;
	d.y = (p.y + q.y) / 2;
	del.x = q.x - p.x;
	del.y = q.y - p.y;
	dist2 = del.x*del.x + del.y*del.y;
	ht = (ED_label(e)->dimen.y + 2)/2.0;
	sp = dotneato_closest(ED_spl(e), d);
	spf.x = sp.x;
	spf.y = sp.y;
	if (dist2) {
	    wd = (MIN(ED_label(e)->dimen.x + 2, MAXLABELWD))/2.0;
	    leftOf = LEFTOF(p, q, sp);
	    if ((leftOf && (del.y >= 0)) || (!leftOf && (del.y < 0))) {
		if (del.x*del.y >= 0)
		    ht *= -1;
	    }
	    else {
		wd *= -1;
		if (del.x*del.y < 0)
		    ht *= -1;
	    }
	    f = (del.y*wd - del.x*ht)/dist2;
	    ld.x = -f*del.y;
	    ld.y = f*del.x;
	}
	else {    /* end points the same */
	    ld.x = 0;
	    ld.y = -ht;
	}

	ED_label(e)->p.x = spf.x + ld.x;
	ED_label(e)->p.y = spf.y + ld.y;
	ED_label(e)->set = TRUE;
	updateBB(e->tail->graph, ED_label(e));
    }
    makePortLabels(e);
}

typedef struct {
    node_t *n1;
    point p1;
    node_t *n2;
    point p2;
} edgeinfo;
typedef struct {
    Dtlink_t link;
    edgeinfo id;
    edge_t *e;
} edgeitem;

static void *newitem(Dt_t * d, edgeitem * obj, Dtdisc_t * disc)
{
    edgeitem *newp;

    NOTUSED(disc);
    newp = NEW(edgeitem);
    newp->id = obj->id;
    newp->e = obj->e;
    ED_count(newp->e) = 1;

    return newp;
}

static void freeitem(Dt_t * d, edgeitem * obj, Dtdisc_t * disc)
{
    free(obj);
}

static int
cmpitems(Dt_t * d, edgeinfo * key1, edgeinfo * key2, Dtdisc_t * disc)
{
    int x;

    if (key1->n1 > key2->n1)
	return 1;
    if (key1->n1 < key2->n1)
	return -1;
    if (key1->n2 > key2->n2)
	return 1;
    if (key1->n2 < key2->n2)
	return -1;

    if ((x = key1->p1.x - key2->p1.x))
	return x;
    if ((x = key1->p1.y - key2->p1.y))
	return x;
    if ((x = key1->p2.x - key2->p2.x))
	return x;
    return (key1->p2.y - key2->p2.y);
}

Dtdisc_t edgeItemDisc = {
    offsetof(edgeitem, id),
    sizeof(edgeinfo),
    offsetof(edgeitem, link),
    (Dtmake_f) newitem,
    (Dtfree_f) freeitem,
    (Dtcompar_f) cmpitems,
    0,
    0,
    0
};

/* equivEdge:
 * See if we have already encountered an edge between the same
 * node:port pairs. If so, return the earlier edge. If not, 
 * this edge is added to map and returned.
 * We first have to canonicalize the key fields using a lexicographic
 * ordering.
 */
static edge_t *equivEdge(Dt_t * map, edge_t * e)
{
    edgeinfo test;
    edgeitem dummy;
    edgeitem *ip;

    if (e->tail < e->head) {
	test.n1 = e->tail;
	test.p1 = ED_tail_port(e).p;
	test.n2 = e->head;
	test.p2 = ED_head_port(e).p;
    } else if (e->tail > e->head) {
	test.n2 = e->tail;
	test.p2 = ED_tail_port(e).p;
	test.n1 = e->head;
	test.p1 = ED_head_port(e).p;
    } else {
	point hp = ED_head_port(e).p;
	point tp = ED_tail_port(e).p;
	if (tp.x < hp.x) {
	    test.p1 = tp;
	    test.p2 = hp;
	} else if (tp.x > hp.x) {
	    test.p1 = hp;
	    test.p2 = tp;
	} else if (tp.y < hp.y) {
	    test.p1 = tp;
	    test.p2 = hp;
	} else if (tp.y > hp.y) {
	    test.p1 = hp;
	    test.p2 = tp;
	} else {
	    test.p1 = test.p2 = tp;
	}
	test.n2 = test.n1 = e->tail;
    }
    dummy.id = test;
    dummy.e = e;
    ip = dtinsert(map, &dummy);
    return ip->e;
}


/* makeSelfArcs:
 * Generate loops. We use the library routine makeSelfEdge
 * which also places the labels.
 * We have to handle port labels here.
 * as well as update the bbox from edge labels.
 */
void makeSelfArcs(path * P, edge_t * e, int stepx)
{
    int cnt = ED_count(e);

    if (cnt == 1) {
	edge_t *edges1[1];
	edges1[0] = e;
	makeSelfEdge(P, edges1, 0, 1, stepx, stepx, &sinfo);
	if (ED_label(e))
	    updateBB(e->tail->graph, ED_label(e));
	makePortLabels(e);
    } else {
	int i;
	edge_t **edges = N_GNEW(cnt, edge_t *);
	for (i = 0; i < cnt; i++) {
	    edges[i] = e;
	    e = ED_to_virt(e);
	}
	makeSelfEdge(P, edges, 0, cnt, stepx, stepx, &sinfo);
	for (i = 0; i < cnt; i++) {
	    e = edges[i];
	    if (ED_label(e))
		updateBB(e->tail->graph, ED_label(e));
	    makePortLabels(e);
	}
	free(edges);
    }
}

static void makeStraightEdge(graph_t * g, edge_t * e)
{
    point dumb[4];
    node_t *n = e->tail;
    node_t *head = e->head;
    int e_cnt = ED_count(e);
    pointf perp;
    point del;
    edge_t *e0;
    int i, j, xstep, dx;
    double l_perp;
    point dumber[4];
    point p, q;

    p = dumb[1] = dumb[0] = add_points(ND_coord_i(n), ED_tail_port(e).p);
    q = dumb[2] = dumb[3] =
	add_points(ND_coord_i(head), ED_head_port(e).p);

    if (e_cnt == 1) {
	clip_and_install(e, e, dumb, 4, &sinfo);
	addEdgeLabels(e, p, q);
	return;
    }

    e0 = e;
    perp.x = dumb[0].y - dumb[3].y;
    perp.y = dumb[3].x - dumb[0].x;
    if ((perp.x == 0) && (perp.y == 0)) { 
	/* degenerate case */
	dumb[1] = dumb[0];
	dumb[2] = dumb[3];
	del.x = 0;
	del.y = 0;
    }
    else {
	l_perp = sqrt(perp.x * perp.x + perp.y * perp.y);
	xstep = GD_nodesep(g);
	dx = xstep * (e_cnt - 1) / 2;
	dumb[1].x = dumb[0].x + (dx * perp.x) / l_perp;
	dumb[1].y = dumb[0].y + (dx * perp.y) / l_perp;
	dumb[2].x = dumb[3].x + (dx * perp.x) / l_perp;
	dumb[2].y = dumb[3].y + (dx * perp.y) / l_perp;
	del.x = -xstep * perp.x / l_perp;
	del.y = -xstep * perp.y / l_perp;
    }

    for (i = 0; i < e_cnt; i++) {
	if (e0->head == head) {
	    p = dumb[0];
	    q = dumb[3];
	    for (j = 0; j < 4; j++) {
		dumber[j] = dumb[j];
	    }
	} else {
	    p = dumb[3];
	    q = dumb[0];
	    for (j = 0; j < 4; j++) {
		dumber[3 - j] = dumb[j];
	    }
	}
	clip_and_install(e0, e0, dumber, 4, &sinfo);
	addEdgeLabels(e0, p, q);
	e0 = ED_to_virt(e0);
	dumb[1].x += del.x;
	dumb[1].y += del.y;
	dumb[2].x += del.x;
	dumb[2].y += del.y;
    }
}

/* makeObstacle:
 * Given a node, return an obstacle reflecting the
 * node's geometry. SEP specifies how much space to allow
 * around the polygon. 
 * Returns the constructed polygon on success, NULL on failure.
 * Failure means the node shape is not supported. 
 *
 * The polygon has its vertices in CW order.
 * 
 * N.B. Point, epsf and user shapes are not handled. Point should
 * be easy, and user shapes are boxes. FIX
 */
Ppoly_t *makeObstacle(node_t * n, double SEP)
{
    Ppoly_t *obs;
    polygon_t *poly;
    double adj = 0.0;
    int j, sides;
    pointf polyp;
    box b;
    point pt;
    field_t *fld;

    switch (shapeOf(n)) {
    case SH_POLY:
	obs = NEW(Ppoly_t);
	poly = (polygon_t *) ND_shape_info(n);
	if (poly->sides >= 3) {
	    sides = poly->sides;
	} else {		/* ellipse */
	    sides = 8;
	    adj = drand48() * .01;
	}
	obs->pn = sides;
	obs->ps = N_NEW(sides, Ppoint_t);
	/* assuming polys are in CCW order, and pathplan needs CW */
	for (j = 0; j < sides; j++) {
	    if (poly->sides >= 3) {
		polyp.x = poly->vertices[j].x * SEP;
		polyp.y = poly->vertices[j].y * SEP;
	    } else {
		double c, s;
		c = cos(2.0 * PI * j / sides + adj);
		s = sin(2.0 * PI * j / sides + adj);
		polyp.x = SEP * c * (ND_lw_i(n) + ND_rw_i(n)) / 2.0;
		polyp.y = SEP * s * ND_ht_i(n) / 2.0;
	    }
	    obs->ps[sides - j - 1].x = polyp.x + ND_coord_i(n).x;
	    obs->ps[sides - j - 1].y = polyp.y + ND_coord_i(n).y;
	}
	break;
    case SH_RECORD:
	fld = (field_t *) ND_shape_info(n);
	b = fld->b;
	obs = NEW(Ppoly_t);
	obs->pn = 4;
	obs->ps = N_NEW(4, Ppoint_t);
	/* CW order */
	pt = ND_coord_i(n);
	obs->ps[0] = recPt(b.LL.x, b.LL.y, pt, SEP);
	obs->ps[1] = recPt(b.LL.x, b.UR.y, pt, SEP);
	obs->ps[2] = recPt(b.UR.x, b.UR.y, pt, SEP);
	obs->ps[3] = recPt(b.UR.x, b.LL.y, pt, SEP);
	break;
    default:
	obs = NULL;
	break;
    }
    return obs;
}

/* getPath
 * Construct the shortest path from one endpoint of e to the other.
 * The obstacles and their number are given by obs and npoly.
 * vconfig is a precomputed data structure to help in the computation.
 * If chkPts is true, the function finds the polygons, if any, containing
 * the endpoints and tells the shortest path computation to ignore them. 
 * Assumes this info is set in ND_lim, usually from _spline_edges.
 * Returns the shortest path.
 */
Ppolyline_t
getPath(edge_t * e, vconfig_t * vconfig, int chkPts, Ppoly_t ** obs,
	int npoly)
{
    Ppolyline_t line;
    int pp, qp;
    Ppoint_t p, q;
    point p1, q1;

    p1 = add_points(ND_coord_i(e->tail), ED_tail_port(e).p);
    q1 = add_points(ND_coord_i(e->head), ED_head_port(e).p);
    P2PF(p1, p);
    P2PF(q1, q);

    /* determine the polygons (if any) that contain the endpoints */
    pp = qp = POLYID_NONE;
    if (chkPts) {
	pp = ND_lim(e->tail);
	qp = ND_lim(e->head);
/*
	for (i = 0; i < npoly; i++) {
	    if ((pp == POLYID_NONE) && in_poly(*obs[i], p))
		pp = i;
	    if ((qp == POLYID_NONE) && in_poly(*obs[i], q))
		qp = i;
	}
*/
    }
    Pobspath(vconfig, p, pp, q, qp, &line);
    return line;
}

/* makeSpline:
 * Construct a spline connecting the endpoints of e, avoiding the npoly
 * obstacles obs.
 * The resultant spline is attached to the edge, the positions of any 
 * edge labels are computed, and the graph's bounding box is recomputed.
 * 
 * If chkPts is true, the function checks if one or both of the endpoints 
 * is on or inside one of the obstacles and, if so, tells the shortest path
 * computation to ignore them. 
 */
void makeSpline(edge_t * e, Ppoly_t ** obs, int npoly, bool chkPts)
{
    Ppolyline_t line, spline;
    Pvector_t slopes[2];
    int i, n_barriers;
    int pp, qp;
    Ppoint_t p, q;
    point *ispline;
    Pedge_t *barriers;
    point p1, q1;

    line = ED_path(e);
    p = line.ps[0];
    q = line.ps[line.pn - 1];
    /* determine the polygons (if any) that contain the endpoints */
    pp = qp = POLYID_NONE;
    if (chkPts)
	for (i = 0; i < npoly; i++) {
	    if ((pp == POLYID_NONE) && in_poly(*obs[i], p))
		pp = i;
	    if ((qp == POLYID_NONE) && in_poly(*obs[i], q))
		qp = i;
	}

    make_barriers(obs, npoly, pp, qp, &barriers, &n_barriers);
    slopes[0].x = slopes[0].y = 0.0;
    slopes[1].x = slopes[1].y = 0.0;
    Proutespline(barriers, n_barriers, line, slopes, &spline);

    /* north why did you ever use int coords */
    ispline = N_GNEW(spline.pn, point);
    for (i = 0; i < spline.pn; i++) {
	ispline[i].x = ROUND(spline.ps[i].x);
	ispline[i].y = ROUND(spline.ps[i].y);
    }
    if (Verbose > 1)
	fprintf(stderr, "spline %s %s\n", e->tail->name, e->head->name);
    clip_and_install(e, e, ispline, spline.pn, &sinfo);
    free(ispline);
    free(barriers);
    PF2P(p, p1);
    PF2P(q, q1);
    addEdgeLabels(e, p1, q1);
}

/* _spline_edges:
 * Basic default routine for creating edges.
 * If splines are requested, we construct the obstacles.
 * If not, or nodes overlap, the function reverts to line segments.
 * NOTE: intra-cluster edges are not constrained to
 * remain in the cluster's bounding box and, conversely, a cluster's box
 * is not altered to reflect intra-cluster edges.
 * If Nop > 1 and the spline exists, it is just copied.
 */
static int _spline_edges(graph_t * g, double SEP, int splines)
{
    node_t *n;
    edge_t *e;
    Ppoly_t **obs;
    Ppoly_t *obp;
    int i = 0, npoly;
    vconfig_t *vconfig;
    path *P = NULL;

    /* build configuration */
    if (splines) {
	obs = N_NEW(agnnodes(g), Ppoly_t *);
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    obp = makeObstacle(n, SEP);
	    if (obp) {
		ND_lim(n) = i; 
		obs[i++] = obp;
	    }
	    else
		ND_lim(n) = POLYID_NONE; 
	}
    } else {
	obs = 0;
    }
    npoly = i;
    if (obs) {
	if (Plegal_arrangement(obs, npoly))
	    vconfig = Pobsopen(obs, npoly);
	else {
	    if (Verbose)
		fprintf(stderr,
			"nodes touch - falling back to straight line edges\n");
	    vconfig = 0;
	}
    } else
	vconfig = 0;

    /* route edges  */
    if (Verbose)
	fprintf(stderr, "Creating edges using %s\n",
		(vconfig ? "splines" : "line segments"));
    if (vconfig) {
	/* path-finding pass */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		ED_path(e) = getPath(e, vconfig, TRUE, obs, npoly);
	    }
	}
    }

    /* spline-drawing pass */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    node_t *head = e->head;
	    if ((Nop > 1) && ED_spl(e)) {
		addEdgeLabels(e,
			      add_points(ND_coord_i(n), ED_tail_port(e).p),
			      add_points(ND_coord_i(head),
					 ED_head_port(e).p));
	    } else if (n == head) {    /* self arc */
		if (ED_count(e) == 0) continue;   /* only do representative */
		if (!P) {
		    P = NEW(path);
		    P->boxes = N_NEW(agnnodes(g) + 20 * 2 * 9, box);
		}
		makeSelfArcs(P, e, GD_nodesep(g));
	    } else if (vconfig) {
		makeSpline(e, obs, npoly, TRUE);
	    } else if (ED_count(e)) {
		makeStraightEdge(g, e);
	    }
	}
    }

    if (P) {
	free(P->boxes);
	free(P);
    }
    return 0;
}

/* splineEdges:
 * Main wrapper code for generating edges.
 * Sets desired separation. 
 * Coalesces equivalent edges (edges * with the same endpoints). 
 * It then calls the edge generating function, and marks the
 * spline phase complete.
 * Returns 0 on success.
 *
 * The edge function is given the graph, the separation to be added
 * around obstacles, and the type of edge. (At present, this is a bool,
 * with 1 meaning splines and 0 meaning line segments.) It must guarantee 
 * that all bounding boxes are current; in particular, the bounding box of 
 * g must reflect the addition of the edges.
 */
int
splineEdges(graph_t * g, int (*edgefn) (graph_t *, double, int),
	    int splines)
{
    node_t *n;
    edge_t *e;
    double SEP;
    Dt_t *map;
    char* marg;

    /* This value should be independent of the sep value used to expand
     * nodes during adjustment. If not, when the adjustment pass produces
     * a fairly tight layout, the spline code will find that some nodes
     * still overlap.
     */
    if ((marg = agget(g, "esep")))
	SEP = 1.0 + atof(marg);
    else 
          /* expFactor = 1 + "sep" value */
	SEP = 1.0 + SEPFACT*(expFactor(g) - 1.0); 

    /* find equivalent edges */
    map = dtopen(&edgeItemDisc, Dtoset);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    edge_t *leader = equivEdge(map, e);
	    if (leader != e) {
		ED_count(leader)++;
		ED_to_virt(e) = ED_to_virt(leader);
		ED_to_virt(leader) = e;
	    }
	}
    }
    dtclose(map);

    if (edgefn(g, SEP, splines))
	return 1;

    State = GVSPLINES;
    return 0;
}

/* spline_edges1:
 * Construct edges using default algorithm and given splines value.
 * Return 0 on success.
 */
int spline_edges1(graph_t * g, int splines)
{
    return splineEdges(g, _spline_edges, splines);
}

/* spline_edges0:
 * Sets the graph's aspect ratio.
 * Check splines attribute and construct edges using default algorithm.
 * If the splines attribute is defined but equal to "", skip edge routing.
 * 
 * Assumes u.bb for has been computed for g and all clusters
 * (not just top-level clusters), and  that GD_bb(g).LL is at the origin.
 *
 * This last criterion is, I believe, mainly to simplify the code
 * in neato_set_aspect. It would be good to remove this constraint,
 * as this would allow nodes pinned on input to have the same coordinates
 * when output in dot or plain format.
 *
 */
void spline_edges0(graph_t * g)
{
    char* s = agget(g, "splines");

    neato_set_aspect(g);
    if (s && (*s == '\0')) return; 
    spline_edges1(g, mapbool(s));
}

/* spline_edges:
 * Compute bounding box, translate graph to origin,
 * then construct all edges. We assume the graph
 * has no clusters, and only nodes have been
 * positioned.
 */
void spline_edges(graph_t * g)
{
    node_t *n;
    pointf offset;

    compute_bb(g);
    offset = cvt2ptf(GD_bb(g).LL);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	ND_pos(n)[0] -= offset.x;
	ND_pos(n)[1] -= offset.y;
    }
    GD_bb(g).UR.x -= GD_bb(g).LL.x;
    GD_bb(g).UR.y -= GD_bb(g).LL.y;
    GD_bb(g).LL.x = 0;
    GD_bb(g).LL.y = 0;
    spline_edges0(g);
}

/* scaleEdge:
 * Scale edge by given factor.
 * Assume ED_spl != NULL.
 */
static void scaleEdge(edge_t * e, double xf, double yf)
{
    int i, j;
    point *pt;
    bezier *bez;
    point   delh, delt;

    delh.x = POINTS(ND_pos(e->head)[0] * (xf - 1.0));
    delh.y = POINTS(ND_pos(e->head)[1] * (yf - 1.0));
    delt.x = POINTS(ND_pos(e->tail)[0] * (xf - 1.0));
    delt.y = POINTS(ND_pos(e->tail)[1] * (yf - 1.0));

    bez = ED_spl(e)->list;
    for (i = 0; i < ED_spl(e)->size; i++) {
	pt = bez->list;
	for (j = 0; j < bez->size; j++) {
	    if ((i == 0) && (j == 0)) {
		pt->x += delt.x;
		pt->y += delt.y;
	    }
	    else if ((i == ED_spl(e)->size-1) && (j == bez->size-1)) {
		pt->x += delh.x;
		pt->y += delh.y;
	    }
	    else {
		pt->x *= xf;
		pt->y *= yf;
	    }
	    pt++;
	}
	if (bez->sflag) {
	    bez->sp.x += delt.x;
	    bez->sp.y += delt.y;
	}
	if (bez->eflag) {
	    bez->ep.x += delh.x;
	    bez->ep.y += delh.y;
	}
	bez++;
    }

    if (ED_label(e) && ED_label(e)->set) {
	ED_label(e)->p.x *= xf;
	ED_label(e)->p.y *= yf;
    }
    if (ED_head_label(e) && ED_head_label(e)->set) {
	ED_head_label(e)->p.x += delh.x;
	ED_head_label(e)->p.y += delh.y;
    }
    if (ED_tail_label(e) && ED_tail_label(e)->set) {
	ED_tail_label(e)->p.x += delt.x;
	ED_tail_label(e)->p.y += delt.y;
    }
}

/* scaleBB:
 * Scale bounding box of clusters of g by given factors.
 */
static void scaleBB(graph_t * g, double xf, double yf)
{
    int i;

    GD_bb(g).UR.x *= xf;
    GD_bb(g).UR.y *= yf;
    GD_bb(g).LL.x *= xf;
    GD_bb(g).LL.y *= yf;

    if (GD_label(g) && GD_label(g)->set) {
	GD_label(g)->p.x *= xf;
	GD_label(g)->p.y *= yf;
    }

    for (i = 1; i <= GD_n_cluster(g); i++)
	scaleBB(GD_clust(g)[i], xf, yf);
}

/* _neato_set_aspect;
 * Assume all bounding boxes are correct and
 * that GD_bb(g).LL is at origin.
 */
static void _neato_set_aspect(graph_t * g)
{
    /* int          i; */
    double xf, yf, actual, desired;
    node_t *n;

    /* compute_bb(g); */
    if (GD_drawing(g)->ratio_kind) {
	/* normalize */
	assert(GD_bb(g).LL.x == 0);
	assert(GD_bb(g).LL.y == 0);
	if (GD_flip(g)) {
	    int t = GD_bb(g).UR.x;
	    GD_bb(g).UR.x = GD_bb(g).UR.y;
	    GD_bb(g).UR.y = t;
	}
	if (GD_drawing(g)->ratio_kind == R_FILL) {
	    /* fill is weird because both X and Y can stretch */
	    if (GD_drawing(g)->size.x <= 0)
		return;
	    xf = (double) GD_drawing(g)->size.x / (double) GD_bb(g).UR.x;
	    yf = (double) GD_drawing(g)->size.y / (double) GD_bb(g).UR.y;
	    /* handle case where one or more dimensions is too big */
	    if ((xf < 1.0) || (yf < 1.0)) {
		if (xf < yf) {
		    yf = yf / xf;
		    xf = 1.0;
		} else {
		    xf = xf / yf;
		    yf = 1.0;
		}
	    }
	} else if (GD_drawing(g)->ratio_kind == R_EXPAND) {
	    if (GD_drawing(g)->size.x <= 0)
		return;
	    xf = (double) GD_drawing(g)->size.x / (double) GD_bb(g).UR.x;
	    yf = (double) GD_drawing(g)->size.y / (double) GD_bb(g).UR.y;
	    if ((xf > 1.0) && (yf > 1.0)) {
		double scale = MIN(xf, yf);
		xf = yf = scale;
	    } else
		return;
	} else if (GD_drawing(g)->ratio_kind == R_VALUE) {
	    desired = GD_drawing(g)->ratio;
	    actual = ((double) GD_bb(g).UR.y) / ((double) GD_bb(g).UR.x);
	    if (actual < desired) {
		yf = desired / actual;
		xf = 1.0;
	    } else {
		xf = actual / desired;
		yf = 1.0;
	    }
	} else
	    return;
	if (GD_flip(g)) {
	    double t = xf;
	    xf = yf;
	    yf = t;
	}

	if (Nop > 1) {
	    edge_t *e;
	    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
		for (e = agfstout(g, n); e; e = agnxtout(g, e))
		    if (ED_spl(e))
			scaleEdge(e, xf, yf);
	    }
	}
	/* Not relying on neato_nlist here allows us not to have to 
	 * allocate it in the root graph and the connected components. 
	 */
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    ND_pos(n)[0] = ND_pos(n)[0] * xf;
	    ND_pos(n)[1] = ND_pos(n)[1] * yf;
	}
	scaleBB(g, xf, yf);
    }
}

/* neato_set_aspect:
 * Sets aspect ratio if necessary; real work done in _neato_set_aspect;
 * This also copies the internal layout coordinates (ND_pos) to the 
 * external ones (ND_coord_i).
 */
void neato_set_aspect(graph_t * g)
{
    node_t *n;

    _neato_set_aspect(g);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	ND_coord_i(n).x = POINTS(ND_pos(n)[0]);
	ND_coord_i(n).y = POINTS(ND_pos(n)[1]);
    }
}

