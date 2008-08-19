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


/*
 * set edge splines.
 */

#include "dot.h"

#ifdef ORTHO
#include <ortho.h>
#endif

#define	NSUB	9		/* number of subdivisions, re-aiming splines */
#define	CHUNK	128		/* in building list of edges */

#define MINW 16			/* minimum width of a box in the edge path */
#define HALFMINW 8

#define FWDEDGE    16
#define BWDEDGE    32

#define MAINGRAPH  64
#define AUXGRAPH  128
#define GRAPHTYPEMASK	192	/* the OR of the above */

#define MAKEFWDEDGE(new, old) { \
	edge_t *newp; \
	newp = new; \
	*newp = *old; \
	newp->tail = old->head; \
	newp->head = old->tail; \
	ED_tail_port(newp) = ED_head_port(old); \
	ED_head_port(newp) = ED_tail_port(old); \
	ED_edge_type(newp) = VIRTUAL; \
	ED_to_orig(newp) = old; \
}

#define P2PF(p, pf) (pf.x = p.x, pf.y = p.y)

#ifdef OBSOLETE
static int flatsidemap[16][6] = {
    {BOTTOM, BOTTOM, BOTTOM, CCW, CCW, FALSE},
    {TOP,    TOP,    TOP,    CW,  CW,  FALSE},
    {RIGHT,  LEFT,   BOTTOM, CW,  CW,  TRUE},
    {BOTTOM, TOP,    RIGHT,  CCW, CW,  TRUE},
    {TOP,    BOTTOM, RIGHT,  CW,  CCW, TRUE},
    {RIGHT,  TOP,    RIGHT,  CCW, CW,  TRUE},
    {RIGHT,  BOTTOM, RIGHT,  CW,  CCW, TRUE},
    {TOP,    LEFT,   TOP,    CW,  CCW, TRUE},
    {BOTTOM, LEFT,   BOTTOM, CCW, CW,  TRUE},
    {RIGHT,  RIGHT,  BOTTOM, CW,  CCW, TRUE},
    {LEFT,   LEFT,   BOTTOM, CCW, CW,  TRUE},
    {LEFT,   BOTTOM, BOTTOM, CCW, CCW, FALSE},
    {TOP,    RIGHT,  TOP,    CW,  CW,  FALSE},
    {LEFT,   TOP,    TOP,    CW,  CW,  FALSE},
    {BOTTOM, RIGHT,  BOTTOM, CCW, CCW, FALSE},
    {LEFT,   RIGHT,  BOTTOM, CCW, CCW, FALSE},
};
#endif

#define AVG(a, b) ((a + b) / 2)

static box boxes[1000];
typedef struct {
    int LeftBound, RightBound, Splinesep, Multisep;
    box* Rank_box;
} spline_info_t;

static void adjustregularpath(path *, int, int);
static Agedge_t *bot_bound(Agedge_t *, int);
static boolean pathscross(Agnode_t *, Agnode_t *, Agedge_t *, Agedge_t *);
#ifdef OBSOLETE
static void chooseflatsides(pathend_t *, pathend_t *, int *, int *, int *,
			    int *, int *, int *);
static void completeflatpath(path *, pathend_t *, pathend_t *,
			     box *, box *, int, int);
static box makeflatend(box, int, int, box);
static box makeflatcomponent(box, box, int, int, int, int, int);
#endif
static Agraph_t *cl_bound(Agnode_t *, Agnode_t *);
static int cl_vninside(Agraph_t *, Agnode_t *);
static void completeregularpath(path *, Agedge_t *, Agedge_t *,
				pathend_t *, pathend_t *, box *, int, int);
static int edgecmp(Agedge_t **, Agedge_t **);
static void make_flat_edge(spline_info_t*, path *, Agedge_t **, int, int, int);
static void make_regular_edge(spline_info_t*, path *, Agedge_t **, int, int, int);
static box makeregularend(box, int, int);
static box maximal_bbox(spline_info_t*, Agnode_t *, Agedge_t *, Agedge_t *);
static Agnode_t *neighbor(Agnode_t *, Agedge_t *, Agedge_t *, int);
static void place_vnlabel(Agnode_t *);
static box rank_box(spline_info_t* sp, Agraph_t *, int);
static void recover_slack(Agedge_t *, path *);
static void resize_vn(Agnode_t *, int, int, int);
static void setflags(Agedge_t *, int, int, int);
static int straight_len(Agnode_t *);
static Agedge_t *straight_path(Agedge_t *, int, point *, int *);
static Agedge_t *top_bound(Agedge_t *, int);

#define GROWEDGES (edges = ALLOC (n_edges + CHUNK, edges, edge_t*))

static edge_t*
getmainedge(edge_t * e)
{
    edge_t *le = e;
    while (ED_to_virt(le))
	le = ED_to_virt(le);
    while (ED_to_orig(le))
	le = ED_to_orig(le);
    return le;
}

static boolean spline_merge(node_t * n)
{
    return ((ND_node_type(n) == VIRTUAL)
	    && ((ND_in(n).size > 1) || (ND_out(n).size > 1)));
}

static boolean swap_ends_p(edge_t * e)
{
    while (ED_to_orig(e))
	e = ED_to_orig(e);
    if (ND_rank(e->head) > ND_rank(e->tail))
	return FALSE;
    if (ND_rank(e->head) < ND_rank(e->tail))
	return TRUE;
    if (ND_order(e->head) >= ND_order(e->tail))
	return FALSE;
    return TRUE;
}

static splineInfo sinfo = { swap_ends_p, spline_merge };

int portcmp(port p0, port p1)
{
    int rv;
    if (p1.defined == FALSE)
	return (p0.defined ? 1 : 0);
    if (p0.defined == FALSE)
	return -1;
    rv = p0.p.x - p1.p.x;
    if (rv == 0)
	rv = p0.p.y - p1.p.y;
    return rv;
}

/* swap_bezier:
 */
static void swap_bezier(bezier * old, bezier * new)
{
    point *list;
    point *lp;
    point *olp;
    int i, sz;

    sz = old->size;
    list = N_GNEW(sz, point);
    lp = list;
    olp = old->list + (sz - 1);
    for (i = 0; i < sz; i++) {	/* reverse list of points */
	*lp++ = *olp--;
    }

    new->list = list;
    new->size = sz;
    new->sflag = old->eflag;
    new->eflag = old->sflag;
    new->sp = old->ep;
    new->ep = old->sp;
}

/* swap_spline:
 */
static void swap_spline(splines * s)
{
    bezier *list;
    bezier *lp;
    bezier *olp;
    int i, sz;

    sz = s->size;
    list = N_GNEW(sz, bezier);
    lp = list;
    olp = s->list + (sz - 1);
    for (i = 0; i < sz; i++) {	/* reverse and swap list of beziers */
	swap_bezier(olp--, lp++);
    }

    /* free old structures */
    for (i = 0; i < sz; i++)
	free(s->list[i].list);
    free(s->list);

    s->list = list;
}

/* edge_normalize:
 * Some back edges are reversed during layout and the reversed edge
 * is used to compute the spline. We would like to guarantee that
 * the order of control points always goes from tail to head, so
 * we reverse them if necessary.
 */
static void edge_normalize(graph_t * g)
{
    edge_t *e;
    node_t *n;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (sinfo.swapEnds(e) && ED_spl(e))
		swap_spline(ED_spl(e));
	}
    }
}

/* _dot_splines:
 * Main spline routing code.
 * The normalize parameter allows this function to be called by the
 * recursive call in make_flat_edge without normalization occurring,
 * so that the edge will only be normalized once in the top level call
 * of dot_splines.
 */
static void _dot_splines(graph_t * g, int normalize)
{
    int i, j, k, n_nodes, n_edges, ind, cnt;
    node_t *n;
    edge_t fwdedgea, fwdedgeb;
    edge_t *e, *e0, *e1, *ea, *eb, *le0, *le1, **edges;
    path *P;
    spline_info_t sd;
    int et = EDGE_TYPE(g->root);

    if (et == ET_NONE) return; 
#ifdef ORTHO
    if (et == ET_ORTHO) {
	orthoEdges (g, 0, &sinfo);
	goto finish;
    } 
#endif

    mark_lowclusters(g);
    routesplinesinit();
    P = NEW(path);
    /* FlatHeight = 2 * GD_nodesep(g); */
    sd.Splinesep = GD_nodesep(g) / 4;
    sd.Multisep = GD_nodesep(g);
    edges = N_NEW(CHUNK, edge_t *);

    /* compute boundaries and list of splines */
    sd.LeftBound = sd.RightBound = 0;
    n_edges = n_nodes = 0;
    for (i = GD_minrank(g); i <= GD_maxrank(g); i++) {
	n_nodes += GD_rank(g)[i].n;
	if ((n = GD_rank(g)[i].v[0]))
	    sd.LeftBound = MIN(sd.LeftBound, (ND_coord_i(n).x - ND_lw_i(n)));
	if (GD_rank(g)[i].n && (n = GD_rank(g)[i].v[GD_rank(g)[i].n - 1]))
	    sd.RightBound = MAX(sd.RightBound, (ND_coord_i(n).x + ND_rw_i(n)));
	sd.LeftBound -= MINW;
	sd.RightBound += MINW;

	for (j = 0; j < GD_rank(g)[i].n; j++) {
	    n = GD_rank(g)[i].v[j];
		/* if n is the label of a flat edge, copy its position to
		 * the label.
		 */
	    if (ND_alg(n)) {
		edge_t* fe = (edge_t*)ND_alg(n);
		assert (ED_label(fe));
		P2PF(ND_coord_i(n), ED_label(fe)->pos);
	    }
	    if ((ND_node_type(n) != NORMAL) &&
		(sinfo.splineMerge(n) == FALSE))
		continue;
	    for (k = 0; (e = ND_out(n).list[k]); k++) {
		if ((ED_edge_type(e) == FLATORDER)
		    || (ED_edge_type(e) == IGNORED))
		    continue;
		setflags(e, REGULAREDGE, FWDEDGE, MAINGRAPH);
		edges[n_edges++] = e;
		if (n_edges % CHUNK == 0)
		    GROWEDGES;
	    }
	    if (ND_flat_out(n).list)
		for (k = 0; (e = ND_flat_out(n).list[k]); k++) {
		    setflags(e, FLATEDGE, 0, AUXGRAPH);
		    edges[n_edges++] = e;
		    if (n_edges % CHUNK == 0)
			GROWEDGES;
		}
	    if (ND_other(n).list) {
		/* In position, each node has its rw stored in mval and,
                 * if a node is part of a loop, rw may be increased to
                 * reflect the loops and associated labels. We restore
                 * the original value here. 
                 */
		if (ND_node_type(n) == NORMAL) {
		    int tmp = ND_rw_i(n);
		    ND_rw_i(n) = ND_mval(n);
		    ND_mval(n) = tmp;
		}
		for (k = 0; (e = ND_other(n).list[k]); k++) {
		    setflags(e, 0, 0, AUXGRAPH);
		    edges[n_edges++] = e;
		    if (n_edges % CHUNK == 0)
			GROWEDGES;
		}
	    }
	}
    }

    for (i = 0; i < n_edges; i++)
	resolvePorts (getmainedge(edges[i]));

    /* Sort so that equivalent edges are contiguous. 
     * Equivalence should basically mean that 2 edges have the
     * same set {(tailnode,tailport),(headnode,headport)}, or
     * alternatively, the edges would be routed identically if
     * routed separately.
     */
    qsort((char *) &edges[0], n_edges, sizeof(edges[0]),
	  (qsort_cmpf) edgecmp);

    /* FIXME: just how many boxes can there be? */
    P->boxes = N_NEW(n_nodes + 20 * 2 * NSUB, box);
    sd.Rank_box = N_NEW(i, box);

    if (et == ET_LINE) {
    /* place regular edge labels */
	for (n = GD_nlist(g); n; n = ND_next(n)) {
	    if ((ND_node_type(n) == VIRTUAL) && (ND_label(n))) {
		place_vnlabel(n);
	    }
	}
    }

    for (i = 0; i < n_edges;) {
	ind = i;
	le0 = getmainedge((e0 = edges[i++]));
	ea = (ED_tail_port(e0).defined
	      || ED_head_port(e0).defined) ? e0 : le0;
	if (ED_tree_index(ea) & BWDEDGE) {
	    MAKEFWDEDGE(&fwdedgea, ea);
	    ea = &fwdedgea;
	}
	for (cnt = 1; i < n_edges; cnt++, i++) {
	    if (le0 != (le1 = getmainedge((e1 = edges[i]))))
		break;
	    if (ED_adjacent(e0)) continue; /* all flat adjacent edges at once */
	    eb = (ED_tail_port(e1).defined
		  || ED_head_port(e1).defined) ? e1 : le1;
	    if (ED_tree_index(eb) & BWDEDGE) {
		MAKEFWDEDGE(&fwdedgeb, eb);
		eb = &fwdedgeb;
	    }
	    if (portcmp(ED_tail_port(ea), ED_tail_port(eb)))
		break;
	    if (portcmp(ED_head_port(ea), ED_head_port(eb)))
		break;
	    if ((ED_tree_index(e0) & EDGETYPEMASK) == FLATEDGE
		&& ED_label(e0) != ED_label(e1))
		break;
	    if (ED_tree_index(edges[i]) & MAINGRAPH)	/* Aha! -C is on */
		break;
	}

	if (e0->tail == e0->head) {
	    int b, sizey, r;
	    n = e0->tail;
	    r = ND_rank(n);
	    if (r == GD_maxrank(g)) {
		if (r > 0)
		    sizey = ND_coord_i(GD_rank(g)[r-1].v[0]).y - ND_coord_i(n).y;
		else
		    sizey = ND_ht_i(n);
	    }
	    else if (r == GD_minrank(g)) {
		sizey = ND_coord_i(n).y - ND_coord_i(GD_rank(g)[r+1].v[0]).y;
	    }
	    else {
		int upy = ND_coord_i(GD_rank(g)[r-1].v[0]).y - ND_coord_i(n).y;
		int dwny = ND_coord_i(n).y - ND_coord_i(GD_rank(g)[r+1].v[0]).y;
		sizey = MIN(upy, dwny);
	    }
	    makeSelfEdge(P, edges, ind, cnt, sd.Multisep, sizey/2, &sinfo);
	    for (b = 0; b < cnt; b++) {
		e = edges[ind+b];
		if (ED_label(e))
		    updateBB(g, ED_label(e));
	    }
	}
	else if (ND_rank(e0->tail) == ND_rank(e0->head)) {
	    make_flat_edge(&sd, P, edges, ind, cnt, et);
	}
	else
	    make_regular_edge(&sd, P, edges, ind, cnt, et);
    }

    /* place regular edge labels */
    for (n = GD_nlist(g); n; n = ND_next(n)) {
	if ((ND_node_type(n) == VIRTUAL) && (ND_label(n))) {
	    place_vnlabel(n);
	    updateBB(g, ND_label(n));
	}
    }

    /* normalize splines so they always go from tail to head */
    /* place_portlabel relies on this being done first */
    if (normalize)
	edge_normalize(g);

#ifdef ORTHO
finish :
#endif
    /* vladimir: place port labels */
    /* FIX: head and tail labels are not part of cluster bbox */
    if (E_headlabel || E_taillabel) {
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    if (E_headlabel) {
		for (e = agfstin(g, n); e; e = agnxtin(g, e))
		    if (ED_head_label(e)) {
			place_portlabel(e, TRUE);
			updateBB(g, ED_head_label(e));
		    }
	    }
	    if (E_taillabel) {
		for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		    if (ED_tail_label(e)) {
			place_portlabel(e, FALSE);
			updateBB(g, ED_tail_label(e));
		    }
		}
	    }
	}
    }
    /* end vladimir */

#ifdef ORTHO
    if (et != ET_ORTHO) {
#endif
	free(edges);
	free(P->boxes);
	free(P);
	free(sd.Rank_box);
	routesplinesterm();
#ifdef ORTHO
    } 
#endif
    State = GVSPLINES;
}

/* dot_splines:
 * If the splines attribute is defined but equal to "", skip edge routing.
 */
void dot_splines(graph_t * g)
{
    _dot_splines (g, 1);
}

/* place_vnlabel:
 * assign position of an edge label from its virtual node
 * This is for regular edges only.
 */
static void 
place_vnlabel(node_t * n)
{
    pointf dimen;
    double width;
    edge_t *e;
    if (ND_in(n).size == 0)
	return;			/* skip flat edge labels here */
    for (e = ND_out(n).list[0]; ED_edge_type(e) != NORMAL;
	 e = ED_to_orig(e));
    dimen = ED_label(e)->dimen;
    width = GD_flip(n->graph) ? dimen.y : dimen.x;
    ED_label(e)->pos.x = ND_coord_i(n).x + width / 2.0;
    ED_label(e)->pos.y = ND_coord_i(n).y;
}

static void 
setflags(edge_t *e, int hint1, int hint2, int f3)
{
    int f1, f2;
    if (hint1 != 0)
	f1 = hint1;
    else {
	if (e->tail == e->head)
	    if (ED_tail_port(e).defined || ED_head_port(e).defined)
		f1 = SELFWPEDGE;
	    else
		f1 = SELFNPEDGE;
	else if (ND_rank(e->tail) == ND_rank(e->head))
	    f1 = FLATEDGE;
	else
	    f1 = REGULAREDGE;
    }
    if (hint2 != 0)
	f2 = hint2;
    else {
	if (f1 == REGULAREDGE)
	    f2 = (ND_rank(e->tail) < ND_rank(e->head)) ? FWDEDGE : BWDEDGE;
	else if (f1 == FLATEDGE)
	    f2 = (ND_order(e->tail) < ND_order(e->head)) ?
		FWDEDGE : BWDEDGE;
	else			/* f1 == SELF*EDGE */
	    f2 = FWDEDGE;
    }
    ED_tree_index(e) = (f1 | f2 | f3);
}

/* edgecmp:
 * lexicographically order edges by
 *  - edge type
 *  - |rank difference of nodes|
 *  - |x difference of nodes|
 *  - id of witness edge for equivalence class
 *  - port comparison
 *  - graph type
 *  - labels if flat edges
 *  - edge id
 */
static int edgecmp(edge_t** ptr0, edge_t** ptr1)
{
    edge_t fwdedgea, fwdedgeb, *e0, *e1, *ea, *eb, *le0, *le1;
    int et0, et1, v0, v1, rv;

    e0 = (edge_t *) * ptr0;
    e1 = (edge_t *) * ptr1;
    et0 = ED_tree_index(e0) & EDGETYPEMASK;
    et1 = ED_tree_index(e1) & EDGETYPEMASK;
    if (et0 != et1)
	return (et1 - et0);
    le0 = getmainedge(e0);
    le1 = getmainedge(e1);
    v0 = ND_rank(le0->tail) - ND_rank(le0->head), v0 = ABS(v0);
    v1 = ND_rank(le1->tail) - ND_rank(le1->head), v1 = ABS(v1);
    if (v0 != v1)
	return (v0 - v1);
    v0 = ND_coord_i(le0->tail).x - ND_coord_i(le0->head).x, v0 = ABS(v0);
    v1 = ND_coord_i(le1->tail).x - ND_coord_i(le1->head).x, v1 = ABS(v1);
    if (v0 != v1)
	return (v0 - v1);
    /* This provides a cheap test for edges having the same set of endpoints.
     */
    if (le0->id != le1->id)
	return (le0->id - le1->id);
    ea = (ED_tail_port(e0).defined || ED_head_port(e0).defined) ? e0 : le0;
    if (ED_tree_index(ea) & BWDEDGE) {
	MAKEFWDEDGE(&fwdedgea, ea);
	ea = &fwdedgea;
    }
    eb = (ED_tail_port(e1).defined || ED_head_port(e1).defined) ? e1 : le1;
    if (ED_tree_index(eb) & BWDEDGE) {
	MAKEFWDEDGE(&fwdedgeb, eb);
	eb = &fwdedgeb;
    }
    if ((rv = portcmp(ED_tail_port(ea), ED_tail_port(eb))))
	return rv;
    if ((rv = portcmp(ED_head_port(ea), ED_head_port(eb))))
	return rv;
    v0 = ED_tree_index(e0) & GRAPHTYPEMASK;
    v1 = ED_tree_index(e1) & GRAPHTYPEMASK;
    if (v0 != v1)
	return (v0 - v1);
    if (et0 == FLATEDGE && ED_label(e0) != ED_label(e1))
	return (int) (ED_label(e0) - ED_label(e1));
    return (e0->id - e1->id);
}

#if 0
/* fledgecmp:
 * Sort edges by mid y value of ports.
 * If this is the same, and all y values are the same,
 * check if one segment lies within the other.
 */
static int 
fledgecmp(edge_t** ptr0, edge_t** ptr1)
{
    edge_t *e0, *e1;
    point tp0, tp1, hp0, hp1;
    int y0, y1;

    e0 = *ptr0;
    e1 = *ptr1;
    tp0 = ED_tail_port(e0).p;
    hp0 = ED_head_port(e0).p;
    tp1 = ED_tail_port(e1).p;
    hp1 = ED_head_port(e1).p;
    y0 = (tp0.y + hp0.y)/2;
    y1 = (tp1.y + hp1.y)/2;
    if (y0 != y1) return (y0-y1);
    if ((tp0.y == hp0.y) && (tp1.y == hp1.y)) {
	if ((tp0.x <= tp1.x) && (hp0.x >= hp1.x)) {
	    if (tp0.y <= 0) return -1;
	    else return 1;
	}
	else if ((tp0.x >= tp1.x) && (hp0.x <= hp1.x)) {
	    if (tp0.y <= 0) return 1;
	    else return -1;
	}
    }
    return (e0->id - e1->id);

}

#define LABEL_SPACE 8

/* setFlatAdjPos:
 * Create middle boxes for routing using ordered list of edges going from
 * bottom to top.
 * Also, set label positions.
 */
static void
setFlatAdjPos (edge_t** edges, int n_edges, int flip, box* boxes, edge_t* e0)
{
    int r, i, x, boxw, availht;
    edge_t* e;
    double  y, wd, ht, totalht = 0;
    textlabel_t* lbl;
    node_t *tn, *hn;
    graph_t* g;

assert(0);
    tn = e0->tail, hn = e0->head;
    g = tn->graph;
    x = (ND_coord_i(tn).x + ND_coord_i(hn).x)/2;
    y = ND_coord_i(tn).y;
    r = ND_rank(tn);
    availht = GD_rank(g)[r].ht2 + GD_rank(g)[r].ht1 + GD_ranksep(g);
    boxw = (ND_coord_i(hn).x - ND_coord_i(tn).x - ND_rw_i(tn) - ND_lw_i(hn))/3;
    for (i = 0; i < n_edges; i++) {
	if (!((lbl = ED_label(e)))) continue;
	if (flip) {
	    ht = lbl->dimen.x;
	    wd = lbl->dimen.y;
	}
	else {
	    ht = lbl->dimen.y; 
	    wd = lbl->dimen.x; 
	}
	totalht += ht;
        boxw = MAX(boxw, wd);
    }
    for (i = 0; i < n_edges; i++) {
	e = edges[i];
	lbl = ED_label(e);
	if (GD_flip(g)) ht = lbl->dimen.x;
	else ht = lbl->dimen.y; 
	lbl->p.x = x;
	lbl->p.y = ROUND(y - ht/2);
	y -= ht + LABEL_SPACE;
    }
}
#endif
 
/* cloneGraph:
 */
static struct {
    attrsym_t* E_constr;
    attrsym_t* E_samehead;
    attrsym_t* E_sametail;
    attrsym_t* E_weight;
    attrsym_t* E_minlen;
    attrsym_t* N_group;
    int        State;
} attr_state;

/* cloneGraph:
 * Create clone graph. It stores the global Agsyms, to be
 * restored in cleanupCloneGraph. The graph uses the main
 * graph's settings for certain geometry parameters, and
 * declares all node and edge attributes used in the original
 * graph.
 */
static graph_t*
cloneGraph (graph_t* g)
{
    Agsym_t* sym;
    graph_t* auxg;
    Agsym_t **list;
    
    auxg = agopen ("auxg", AG_IS_DIRECTED(g)?AGDIGRAPH:AGRAPH);
    agraphattr(auxg, "rank", "");
    GD_drawing(auxg) = NEW(layout_t);
    GD_drawing(auxg)->quantum = GD_drawing(g)->quantum; 
    GD_drawing(auxg)->dpi = GD_drawing(g)->dpi;

    GD_charset(auxg) = GD_charset (g);
    if (GD_flip(g))
	SET_RANKDIR(auxg, RANKDIR_TB);
    else
	SET_RANKDIR(auxg, RANKDIR_LR);
    GD_nodesep(auxg) = GD_nodesep(g);
    GD_ranksep(auxg) = GD_ranksep(g);

    list = g->root->univ->nodeattr->list;
    while ((sym = *list++)) {
	agnodeattr (auxg, sym->name, sym->value);
    }

    list = g->root->univ->edgeattr->list;
    while ((sym = *list++)) {
	agedgeattr (auxg, sym->name, sym->value);
    }

    attr_state.E_constr = E_constr;
    attr_state.E_samehead = E_samehead;
    attr_state.E_sametail = E_sametail;
    attr_state.E_weight = E_weight;
    attr_state.E_minlen = E_minlen;
    attr_state.N_group = N_group;
    attr_state.State = State;
    E_constr = NULL;
    E_samehead = agfindattr(auxg->proto->e, "samehead");
    E_sametail = agfindattr(auxg->proto->e, "sametail");
    E_weight = agfindattr(auxg->proto->e, "weight");
    if (!E_weight)
	E_weight = agedgeattr (auxg, "weight", "");
    E_minlen = NULL;
    N_group = NULL;

    return auxg;
}

/* cleanupCloneGraph:
 */
static void
cleanupCloneGraph (graph_t* g)
{
    /* restore main graph syms */
    E_constr = attr_state.E_constr;
    E_samehead = attr_state.E_samehead;
    E_sametail = attr_state.E_sametail;
    E_weight = attr_state.E_weight;
    E_minlen = attr_state.E_minlen;
    N_group = attr_state.N_group;
    State = attr_state.State;

    dot_cleanup(g);
    agclose(g);
}

/* cloneNode:
 * If flipped is true, original graph has rankdir=LR or RL.
 * In this case, records change shape, so we wrap a record node's
 * label in "{...}" to prevent this.
 */
static node_t*
cloneNode (graph_t* g, node_t* orign, int flipped)
{
    node_t* n = agnode(g, orign->name);
    agcopyattr (orign, n);
    if (shapeOf(orign) == SH_RECORD) {
	int lbllen = strlen(ND_label(orign)->text);
        char* buf = N_GNEW(lbllen+3,char);
        sprintf (buf, "{%s}", ND_label(orign)->text);
	agset (n, "label", buf);
    }

    return n;
}

/* cloneEdge:
 */
static edge_t*
cloneEdge (graph_t* g, node_t* tn, node_t* hn, edge_t* orig)
{
    edge_t* e = agedge(g, tn, hn);
    for (; ED_edge_type(orig) != NORMAL; orig = ED_to_orig(orig));
    agcopyattr (orig, e);

    return e;
}

/* transform:
 * Rotate, if necessary, then translate points.
 */
static point
transform (point p, point del, int flip)
{
    if (flip) {
	int i = p.x;
	p.x = p.y;
	p.y = -i;
    }
    return add_points(p, del);
}

/* makeSimpleFlat:
 */
static void
makeSimpleFlat (node_t* tn, node_t* hn, edge_t** edges, int ind, int cnt, int et)
{
    edge_t* e = edges[ind];
    point points[10];
    int i, pointn, stepy = (cnt > 1) ? ND_ht_i(tn) / (cnt - 1) : 0;
    point tp = add_points(ND_coord_i(tn), ED_tail_port(e).p);
    point hp = add_points(ND_coord_i(hn), ED_head_port(e).p);
    int dy = tp.y - ((cnt > 1) ? ND_ht_i(tn) / 2 : 0);
    for (i = 0; i < cnt; i++) {
	e = edges[ind + i];
	pointn = 0;
	if ((et == ET_SPLINE) || (et == ET_LINE)) {
	    points[pointn++] = tp;
	    points[pointn++] = pointof((2 * tp.x + hp.x) / 3, dy);
	    points[pointn++] = pointof((2 * hp.x + tp.x) / 3, dy);
	    points[pointn++] = hp;
	}
	else {   /* ET_PLINE */
	    points[pointn++] = tp;
	    points[pointn++] = tp;
	    points[pointn++] = pointof((2 * tp.x + hp.x) / 3, dy);
	    points[pointn++] = pointof((2 * tp.x + hp.x) / 3, dy);
	    points[pointn++] = pointof((2 * tp.x + hp.x) / 3, dy);
	    points[pointn++] = pointof((2 * hp.x + tp.x) / 3, dy);
	    points[pointn++] = pointof((2 * hp.x + tp.x) / 3, dy);
	    points[pointn++] = pointof((2 * hp.x + tp.x) / 3, dy);
	    points[pointn++] = hp;
	    points[pointn++] = hp;
	}
	dy += stepy;
	clip_and_install(e, e->head, points, pointn, &sinfo);
    }
}

/* make_flat_adj_edges:
 * In the simple case, with no labels or ports, this creates a simple
 * spindle of splines.
 * Otherwise, we run dot recursively on the 2 nodes and the edges, 
 * essentially using rankdir=LR, to get the needed spline info.
 */
static void
make_flat_adj_edges(path* P, edge_t** edges, int ind, int cnt, edge_t* e0,
                    int et)
{
    node_t* n;
    node_t *tn, *hn;
    edge_t* e;
    int labels = 0, ports = 0;
    graph_t* g;
    graph_t* auxg;
    graph_t* subg;
    node_t *auxt, *auxh;
    edge_t* auxe;
    int     i, j, midx, midy, leftx, rightx;
    point   del;
    edge_t* hvye = NULL;

    g = e0->tail->graph;
    tn = e0->tail, hn = e0->head;
    for (i = 0; i < cnt; i++) {
	e = edges[ind + i];
	if (ND_label(e)) labels++;
	if (ED_tail_port(e).defined || ED_head_port(e).defined) ports = 1;
    }

    /* flat edges without ports and labels can go straight left to right */
    if ((labels == 0) && (ports == 0)) {
	makeSimpleFlat (tn, hn, edges, ind, cnt, et);
	return;
    }

    auxg = cloneGraph (g);
    subg = agsubg (auxg, "xxx");
    agset (subg, "rank", "source");
    rightx = ND_coord_i(hn).x;
    leftx = ND_coord_i(tn).x;
    if (GD_flip(g)) {
        node_t* n;
        n = tn;
        tn = hn;
        hn = n;
    }
    auxt = cloneNode(subg, tn, GD_flip(g)); 
    auxh = cloneNode(auxg, hn, GD_flip(g)); 
    for (i = 0; i < cnt; i++) {
	e = edges[ind + i];
	if (e->tail == tn)
	    auxe = cloneEdge (auxg, auxt, auxh, e);
	else
	    auxe = cloneEdge (auxg, auxh, auxt, e);
	ED_alg(e) = auxe;
	if (!hvye && !ED_tail_port(e0).defined && !ED_head_port(e0).defined) {
	    hvye = auxe;
	    ED_alg(hvye) = e;
	}
    }
    if (!hvye) {
	hvye = agedge (auxg, auxt, auxh);
    }
    agxset (hvye, E_weight->index, "10000");
    GD_gvc(auxg) = GD_gvc(g);
    setEdgeType (auxg, et);
    dot_init_node_edge(auxg);

    dot_rank(auxg);
    dot_mincross(auxg);
    dot_position(auxg);
    
    /* reposition */
    midx = (ND_coord_i(tn).x - ND_rw_i(tn) + ND_coord_i(hn).x + ND_lw_i(hn))/2;
    midy = (ND_coord_i(auxt).x + ND_coord_i(auxh).x)/2;
    for (n = GD_nlist(auxg); n; n = ND_next(n)) {
	if (n == auxt) {
	    ND_coord_i(n).y = rightx;
	    ND_coord_i(n).x = midy;
	}
	else if (n == auxh) {
	    ND_coord_i(n).y = leftx;
	    ND_coord_i(n).x = midy;
	}
	else ND_coord_i(n).y = midx;
    }
    dot_sameports(auxg);
    _dot_splines(auxg, 0);
    dotneato_postprocess(auxg);

       /* copy splines */
    if (GD_flip(g)) {
	del.x = ND_coord_i(tn).x - ND_coord_i(auxt).y;
	del.y = ND_coord_i(tn).y + ND_coord_i(auxt).x;
    }
    else {
	del.x = ND_coord_i(tn).x - ND_coord_i(auxt).x;
	del.y = ND_coord_i(tn).y - ND_coord_i(auxt).y;
    }
    for (i = 0; i < cnt; i++) {
	bezier* auxbz;
	bezier* bz;

	e = edges[ind + i];
	auxe = (edge_t*)ED_alg(e);
	if ((auxe == hvye) & !ED_alg(auxe)) continue; /* pseudo-edge */
	auxbz = ED_spl(auxe)->list;
	bz = new_spline(e, auxbz->size);
	if (GD_flip(g)) {
	    bz->sflag = auxbz->eflag;
	    bz->sp = transform(auxbz->ep, del, 1);
	    bz->eflag = auxbz->sflag;
	    bz->ep = transform(auxbz->sp, del, 1);
	}
	else {
	    bz->sflag = auxbz->sflag;
	    bz->sp = transform(auxbz->sp, del, 0);
	    bz->eflag = auxbz->eflag;
	    bz->ep = transform(auxbz->ep, del, 0);
	}
	for (j = 0; j <  auxbz->size; ) {
	    point pt;
	    pointf cp[4];
	    pt = bz->list[j] = transform(auxbz->list[j], del, GD_flip(g));
	    P2PF(pt,cp[0]);
	    j++;
	    if ( j >= auxbz->size ) 
		break;
	    pt = bz->list[j] = transform(auxbz->list[j], del, GD_flip(g));
	    P2PF(pt,cp[1]);
	    j++;
	    pt = bz->list[j] = transform(auxbz->list[j], del, GD_flip(g));
	    P2PF(pt,cp[2]);
	    j++;
	    pt = transform(auxbz->list[j], del, GD_flip(g));
	    P2PF(pt,cp[3]);
	    update_bb_bz(&GD_bb(g), cp);
        }
	if (ED_label(e)) {
	    point pt;
	    PF2P(ED_label(auxe)->pos, pt);
	    pt = transform(pt, del, GD_flip(g));
	    P2PF(pt, ED_label(e)->pos);
	    updateBB(g, ED_label(e));
	}
    }

    cleanupCloneGraph (auxg);
}

/* makeFlatEnd;
 */
static void
makeFlatEnd (spline_info_t* sp, path* P, node_t* n, edge_t* e, pathend_t* endp,
             boolean isBegin)
{
    box b;
    graph_t* g = n->graph;

    b = endp->nb = maximal_bbox(sp, n, NULL, e);
    endp->sidemask = TOP;
    if (isBegin) beginpath(P, e, FLATEDGE, endp, FALSE);
    else endpath(P, e, FLATEDGE, endp, FALSE);
    b.UR.y = endp->boxes[endp->boxn - 1].UR.y;
    b.LL.y = endp->boxes[endp->boxn - 1].LL.y;
    b = makeregularend(b, TOP, ND_coord_i(n).y + GD_rank(g)[ND_rank(n)].ht2);
    if (b.LL.x < b.UR.x && b.LL.y < b.UR.y)
	endp->boxes[endp->boxn++] = b;
}
/* makeBottomFlatEnd;
 */
static void
makeBottomFlatEnd (spline_info_t* sp, path* P, node_t* n, edge_t* e, 
	pathend_t* endp, boolean isBegin)
{
    box b;
    graph_t* g = n->graph;

    b = endp->nb = maximal_bbox(sp, n, NULL, e);
    endp->sidemask = BOTTOM;
    if (isBegin) beginpath(P, e, FLATEDGE, endp, FALSE);
    else endpath(P, e, FLATEDGE, endp, FALSE);
    b.UR.y = endp->boxes[endp->boxn - 1].UR.y;
    b.LL.y = endp->boxes[endp->boxn - 1].LL.y;
    b = makeregularend(b, BOTTOM, ND_coord_i(n).y - GD_rank(g)[ND_rank(n)].ht2);
    if (b.LL.x < b.UR.x && b.LL.y < b.UR.y)
	endp->boxes[endp->boxn++] = b;
}


/* make_flat_labeled_edge:
 */
static void
make_flat_labeled_edge(spline_info_t* sp, path* P, edge_t* e, int et)
{
    graph_t *g;
    node_t *tn, *hn, *ln;
    point *ps;
    pathend_t tend, hend;
    box lb;
    int boxn, i, pn, ydelta;
    edge_t *f;
    point points[7];

    g = e->tail->graph;
    tn = e->tail;
    hn = e->head;

    for (f = ED_to_virt(e); ED_to_virt(f); f = ED_to_virt(f));
    ln = f->tail;
    P2PF(ND_coord_i(ln), ED_label(e)->pos);

    if (et == ET_LINE) {
	point startp, endp, lp;
	startp = add_points(ND_coord_i(tn), ED_tail_port(e).p);
	endp = add_points(ND_coord_i(hn), ED_head_port(e).p);
        PF2P(ED_label(e)->pos, lp);
	lp.y -= (ED_label(e)->dimen.y)/2.0;
	points[1] = points[0] = startp;
	points[2] = points[3] = points[4] = lp;
	points[5] = points[6] = endp;
	ps = points;
	pn = 7;
    }
    else {
	lb.LL.x = ND_coord_i(ln).x - ND_lw_i(ln);
	lb.UR.x = ND_coord_i(ln).x + ND_rw_i(ln);
	lb.UR.y = ND_coord_i(ln).y + ND_ht_i(ln)/2;
	ydelta = ND_coord_i(ln).y - GD_rank(g)[ND_rank(tn)].ht1 -
		ND_coord_i(tn).y + GD_rank(g)[ND_rank(tn)].ht2;
	ydelta /= 6;
	lb.LL.y = lb.UR.y - MAX(5,ydelta); 

	boxn = 0;
	makeFlatEnd (sp, P, tn, e, &tend, TRUE);
	makeFlatEnd (sp, P, hn, e, &hend, FALSE);

	boxes[boxn].LL.x = tend.boxes[tend.boxn - 1].LL.x; 
	boxes[boxn].LL.y = tend.boxes[tend.boxn - 1].UR.y; 
	boxes[boxn].UR.x = lb.LL.x;
	boxes[boxn].UR.y = lb.LL.y;
	boxn++;
	boxes[boxn].LL.x = tend.boxes[tend.boxn - 1].LL.x; 
	boxes[boxn].LL.y = lb.LL.y;
	boxes[boxn].UR.x = hend.boxes[hend.boxn - 1].UR.x;
	boxes[boxn].UR.y = lb.UR.y;
	boxn++;
	boxes[boxn].LL.x = lb.UR.x;
	boxes[boxn].UR.y = lb.LL.y;
	boxes[boxn].LL.y = hend.boxes[hend.boxn - 1].UR.y; 
	boxes[boxn].UR.x = hend.boxes[hend.boxn - 1].UR.x;
	boxn++;

	for (i = 0; i < tend.boxn; i++) add_box(P, tend.boxes[i]);
	for (i = 0; i < boxn; i++) add_box(P, boxes[i]);
	for (i = hend.boxn - 1; i >= 0; i--) add_box(P, hend.boxes[i]);

	if (et == ET_SPLINE) ps = routesplines(P, &pn);
	else ps = routepolylines(P, &pn);
	if (pn == 0) return;
    }
    clip_and_install(e, e->head, ps, pn, &sinfo);
}

/* make_flat_bottom_edges:
 */
static void
make_flat_bottom_edges(spline_info_t* sp, path * P, edge_t ** edges, int 
	ind, int cnt, edge_t* e, int splines)
{
    node_t *tn, *hn;
    int j, i, stepx, stepy, vspace, r;
    rank_t* nextr;
    int pn;
    point *ps;
    pathend_t tend, hend;
    graph_t* g;

    tn = e->tail, hn = e->head;
    g = tn->graph;
    r = ND_rank(tn);
    if (r < GD_maxrank(g)) {
	nextr = GD_rank(g) + (r+1);
	vspace = ND_coord_i(tn).y - GD_rank(g)[r].pht1 -
		(ND_coord_i(nextr->v[0]).y + nextr->pht2);
    }
    else {
	vspace = GD_ranksep(g);
    }
    stepx = sp->Multisep / (cnt+1); 
    stepy = vspace / (cnt+1);

    makeBottomFlatEnd (sp, P, tn, e, &tend, TRUE);
    makeBottomFlatEnd (sp, P, hn, e, &hend, FALSE);

    for (i = 0; i < cnt; i++) {
	int boxn;
	box b;
	e = edges[ind + i];
	boxn = 0;

	b = tend.boxes[tend.boxn - 1];
 	boxes[boxn].LL.x = b.LL.x; 
	boxes[boxn].UR.y = b.LL.y; 
	boxes[boxn].UR.x = b.UR.x + (i + 1) * stepx;
	boxes[boxn].LL.y = b.LL.y - (i + 1) * stepy;
	boxn++;
	boxes[boxn].LL.x = tend.boxes[tend.boxn - 1].LL.x; 
	boxes[boxn].UR.y = boxes[boxn-1].LL.y;
	boxes[boxn].UR.x = hend.boxes[hend.boxn - 1].UR.x;
	boxes[boxn].LL.y = boxes[boxn].UR.y - stepy;
	boxn++;
	b = hend.boxes[hend.boxn - 1];
	boxes[boxn].UR.x = b.UR.x;
	boxes[boxn].UR.y = b.LL.y;
	boxes[boxn].LL.x = b.LL.x - (i + 1) * stepx;
	boxes[boxn].LL.y = boxes[boxn-1].UR.y;
	boxn++;

	for (j = 0; j < tend.boxn; j++) add_box(P, tend.boxes[j]);
	for (j = 0; j < boxn; j++) add_box(P, boxes[j]);
	for (j = hend.boxn - 1; j >= 0; j--) add_box(P, hend.boxes[j]);

	if (splines) ps = routesplines(P, &pn);
	else ps = routepolylines(P, &pn);
	if (pn == 0)
	    return;
	clip_and_install(e, e->head, ps, pn, &sinfo);
	P->nbox = 0;
    }
}

/* make_flat_edge:
 * Construct flat edges edges[ind...ind+cnt-1]
 * There are 4 main cases:
 *  - all edges between a and b where a and b are adjacent 
 *  - one labeled edge
 *  - all non-labeled edges with identical ports between non-adjacent a and b 
 *     = connecting bottom to bottom/left/right - route along bottom
 *     = the rest - route along top
 */
static void
make_flat_edge(spline_info_t* sp, path * P, edge_t ** edges, int ind, int cnt, int et)
{
    node_t *tn, *hn;
    edge_t fwdedge, *e;
    int j, i, stepx, stepy, vspace, r;
    int tside, hside, pn;
    point *ps;
    pathend_t tend, hend;
    graph_t* g;

    /* Get sample edge; normalize to go from left to right */
    e = edges[ind];
    if (ED_tree_index(e) & BWDEDGE) {
	MAKEFWDEDGE(&fwdedge, e);
	e = &fwdedge;
    }
    if (ED_adjacent(edges[ind])) {
	make_flat_adj_edges (P, edges, ind, cnt, e, et);
	return;
    }
    if (ED_label(e)) {  /* edges with labels aren't multi-edges */
	make_flat_labeled_edge (sp, P, e, et);
	return;
    }

    if (et == ET_LINE) {
	makeSimpleFlat (e->tail, e->head, edges, ind, cnt, et);
	return;
    }

    tside = ED_tail_port(e).side;
    hside = ED_head_port(e).side;
    if (((tside == BOTTOM) && (hside != TOP)) ||
        ((hside == BOTTOM) && (tside != TOP))) {
	make_flat_bottom_edges (sp, P, edges, ind, cnt, e, et == ET_SPLINE);
	return;
    }

    tn = e->tail, hn = e->head;
    g = tn->graph;
    r = ND_rank(tn);
    if (r > 0) {
	rank_t* prevr;
	if (GD_has_labels(g) & EDGE_LABEL)
	    prevr = GD_rank(g) + (r-2);
	else
	    prevr = GD_rank(g) + (r-1);
	vspace = ND_coord_i(prevr->v[0]).y - prevr->ht1 - ND_coord_i(tn).y - GD_rank(g)[r].ht2;
    }
    else {
	vspace = GD_ranksep(g);
    }
    stepx = sp->Multisep / (cnt+1); 
    stepy = vspace / (cnt+1);

    makeFlatEnd (sp, P, tn, e, &tend, TRUE);
    makeFlatEnd (sp, P, hn, e, &hend, FALSE);

    for (i = 0; i < cnt; i++) {
	int boxn;
	box b;
	e = edges[ind + i];
	boxn = 0;

	b = tend.boxes[tend.boxn - 1];
 	boxes[boxn].LL.x = b.LL.x; 
	boxes[boxn].LL.y = b.UR.y; 
	boxes[boxn].UR.x = b.UR.x + (i + 1) * stepx;
	boxes[boxn].UR.y = b.UR.y + (i + 1) * stepy;
	boxn++;
	boxes[boxn].LL.x = tend.boxes[tend.boxn - 1].LL.x; 
	boxes[boxn].LL.y = boxes[boxn-1].UR.y;
	boxes[boxn].UR.x = hend.boxes[hend.boxn - 1].UR.x;
	boxes[boxn].UR.y = boxes[boxn].LL.y + stepy;
	boxn++;
	b = hend.boxes[hend.boxn - 1];
	boxes[boxn].UR.x = b.UR.x;
	boxes[boxn].LL.y = b.UR.y;
	boxes[boxn].LL.x = b.LL.x - (i + 1) * stepx;
	boxes[boxn].UR.y = boxes[boxn-1].LL.y;
	boxn++;

	for (j = 0; j < tend.boxn; j++) add_box(P, tend.boxes[j]);
	for (j = 0; j < boxn; j++) add_box(P, boxes[j]);
	for (j = hend.boxn - 1; j >= 0; j--) add_box(P, hend.boxes[j]);

	if (et == ET_SPLINE) ps = routesplines(P, &pn);
	else ps = routepolylines(P, &pn);
	if (pn == 0)
	    return;
	clip_and_install(e, e->head, ps, pn, &sinfo);
	P->nbox = 0;
    }
}

/* ccw:
 * Return true if p3 is to left of ray p1->p2
 */
static int
leftOf (point p1, point p2, point p3)
{
    int d;

    d = ((p1.y - p2.y) * (p3.x - p2.x)) -
        ((p3.y - p2.y) * (p1.x - p2.x));
    return (d > 0);
}

/* makeLineEdge:
 * Create an edge as line segment. We guarantee that the points
 * are always drawn downwards. This means that for flipped edges,
 * we draw from the head to the tail. The routine returns the
 * end node of the edge in *hp. The points are stored in the
 * given array of points, and the number of points is returned.
 *
 * If the edge has a label, the edge is draw as two segments, with
 * the bend near the label.
 *
 * If the endpoints are on adjacent ranks, revert to usual code by
 * returning 0.
 * This is done because the usual code handles the interaction of
 * multiple edges better.
 */
static int 
makeLineEdge(edge_t* fe, point* points, node_t** hp)
{
    int delr, pn;
    node_t* hn;
    node_t* tn;
    edge_t* e = fe;
    point startp, endp, lp;
    pointf dimen;
    double width, height;

    while (ED_edge_type(e) != NORMAL)
	e = ED_to_orig(e);
    hn = e->head;
    tn = e->tail;
    delr = ABS(ND_rank(hn)-ND_rank(tn));
    if ((delr == 1) || ((delr == 2) && (GD_has_labels(hn->graph) & EDGE_LABEL)))
	return 0;
    if (fe->tail == e->tail) {
	*hp = hn;
	startp = add_points(ND_coord_i(tn), ED_tail_port(e).p);
	endp = add_points(ND_coord_i(hn), ED_head_port(e).p);
    }
    else {
 	*hp = tn; 
	startp = add_points(ND_coord_i(hn), ED_head_port(e).p);
	endp = add_points(ND_coord_i(tn), ED_tail_port(e).p);
    }

    if (ED_label(e)) {
	dimen = ED_label(e)->dimen;
	if (GD_flip(hn->graph)) {
	    width = dimen.y;
	    height = dimen.x;
	}
	else {
	    width = dimen.x;
	    height = dimen.y;
	}

	PF2P(ED_label(e)->pos, lp);
	if (leftOf (endp,startp,lp)) {
	    lp.x += width/2.0;
	    lp.y -= height/2.0;
	}    
	else {
	    lp.x -= width/2.0;
	    lp.y += height/2.0;
	}

	points[1] = points[0] = startp;
	points[2] = points[3] = points[4] = lp;
	points[5] = points[6] = endp;
	pn = 7;
    }
    else {
	points[1] = points[0] = startp;
	points[3] = points[2] = endp;
	pn = 4;
    }

    return pn;
}

/* make_regular_edge:
 */
static void
make_regular_edge(spline_info_t* sp, path * P, edge_t ** edges, int ind, int cnt, int et)
{
    graph_t *g;
    node_t *tn, *hn;
    edge_t fwdedgea, fwdedgeb, fwdedge, *e, *fe, *le, *segfirst;
    point *ps;
    pathend_t tend, hend;
    box b;
    int boxn, sl, si, smode, i, j, dx, pn, hackflag, longedge;
    point points[1000], points2[1000];
    int pointn;

    sl = 0;
    e = edges[ind];
    g = e->tail->graph;
    hackflag = FALSE;
    if (ABS(ND_rank(e->tail) - ND_rank(e->head)) > 1) {
	fwdedgea = *e;
	if (ED_tree_index(e) & BWDEDGE) {
	    MAKEFWDEDGE(&fwdedgeb, e);
	    fwdedgea.tail = e->head;
	    fwdedgea.u.tail_port = ED_head_port(e);
	} else {
	    fwdedgeb = *e;
	    fwdedgea.tail = e->tail;
	}
	le = getmainedge(e);
	while (ED_to_virt(le))
	    le = ED_to_virt(le);
	fwdedgea.head = le->head;
	fwdedgea.u.head_port.defined = FALSE;
	fwdedgea.u.edge_type = VIRTUAL;
	fwdedgea.u.head_port.p.x = fwdedgea.u.head_port.p.y = 0;
	fwdedgea.u.to_orig = e;
	e = &fwdedgea;
	hackflag = TRUE;
    } else {
	if (ED_tree_index(e) & BWDEDGE) {
	    MAKEFWDEDGE(&fwdedgea, e);
	    e = &fwdedgea;
	}
    }
    fe = e;

    /* compute the spline points for the edge */

    if ((et == ET_LINE) && (pointn = makeLineEdge (fe, points, &hn))) {
    }
    else {
	int splines = et == ET_SPLINE;
	boxn = 0;
	pointn = 0;
	segfirst = e;
	tn = e->tail;
	hn = e->head;
	b = tend.nb = maximal_bbox(sp, tn, NULL, e);
	beginpath(P, e, REGULAREDGE, &tend, spline_merge(e->tail));
	b.UR.y = tend.boxes[tend.boxn - 1].UR.y;
	b.LL.y = tend.boxes[tend.boxn - 1].LL.y;
	b = makeregularend(b, BOTTOM,
	    	   ND_coord_i(tn).y - GD_rank(tn->graph)[ND_rank(tn)].ht1);
	if (b.LL.x < b.UR.x && b.LL.y < b.UR.y)
	    tend.boxes[tend.boxn++] = b;
	longedge = 0;
	smode = FALSE, si = -1;
	while (ND_node_type(hn) == VIRTUAL && !sinfo.splineMerge(hn)) {
	    longedge = 1;
	    boxes[boxn++] = rank_box(sp, g, ND_rank(tn));
	    if (!smode
	        && ((sl = straight_len(hn)) >=
	    	((GD_has_labels(g) & EDGE_LABEL) ? 4 + 1 : 2 + 1))) {
	        smode = TRUE;
	        si = 1, sl -= 2;
	    }
	    if (!smode || si > 0) {
	        si--;
	        boxes[boxn++] = maximal_bbox(sp, hn, e, ND_out(hn).list[0]);
	        e = ND_out(hn).list[0];
	        tn = e->tail;
	        hn = e->head;
	        continue;
	    }
	    hend.nb = maximal_bbox(sp, hn, e, ND_out(hn).list[0]);
	    endpath(P, e, REGULAREDGE, &hend, spline_merge(e->head));
	    b = makeregularend(hend.boxes[hend.boxn - 1], TOP,
	    	       ND_coord_i(hn).y + GD_rank(hn->graph)[ND_rank(hn)].ht2);
	    if (b.LL.x < b.UR.x && b.LL.y < b.UR.y)
	        hend.boxes[hend.boxn++] = b;
	    P->end.theta = M_PI / 2, P->end.constrained = TRUE;
	    completeregularpath(P, segfirst, e, &tend, &hend, boxes, boxn, 1);
	    if (splines) ps = routesplines(P, &pn);
	    else {
		ps = routepolylines (P, &pn);
		if ((et == ET_LINE) && (pn > 4)) {
		    ps[1] = ps[0];
		    ps[3] = ps[2] = ps[pn-1];
		    pn = 4;
		}
	    }
	    if (pn == 0)
	        return;
	    for (i = 0; i < pn; i++)
	        points[pointn++] = ps[i];
	    e = straight_path(ND_out(hn).list[0], sl, points, &pointn);
	    recover_slack(segfirst, P);
	    segfirst = e;
	    tn = e->tail;
	    hn = e->head;
	    boxn = 0;
	    tend.nb = maximal_bbox(sp, tn, ND_in(tn).list[0], e);
	    beginpath(P, e, REGULAREDGE, &tend, spline_merge(e->tail));
	    b = makeregularend(tend.boxes[tend.boxn - 1], BOTTOM,
	    	       ND_coord_i(tn).y - GD_rank(tn->graph)[ND_rank(tn)].ht1);
	    if (b.LL.x < b.UR.x && b.LL.y < b.UR.y)
	        tend.boxes[tend.boxn++] = b;
	    P->start.theta = -M_PI / 2, P->start.constrained = TRUE;
	    smode = FALSE;
	}
	boxes[boxn++] = rank_box(sp, g, ND_rank(tn));
	b = hend.nb = maximal_bbox(sp, hn, e, NULL);
	endpath(P, hackflag ? &fwdedgeb : e, REGULAREDGE, &hend,
	        spline_merge(e->head));
	b.UR.y = hend.boxes[hend.boxn - 1].UR.y;
	b.LL.y = hend.boxes[hend.boxn - 1].LL.y;
	b = makeregularend(b, TOP,
	    	   ND_coord_i(hn).y + GD_rank(hn->graph)[ND_rank(hn)].ht2);
	if (b.LL.x < b.UR.x && b.LL.y < b.UR.y)
	    hend.boxes[hend.boxn++] = b;
	completeregularpath(P, segfirst, e, &tend, &hend, boxes, boxn,
	    		longedge);
	if (splines) ps = routesplines(P, &pn);
	else ps = routepolylines (P, &pn);
	if ((et == ET_LINE) && (pn > 4)) {
	    /* Here we have used the polyline case to handle
	     * an edge between two nodes on adjacent ranks. If the
	     * results really is a polyline, straighten it.
	     */
	    ps[1] = ps[0];
	    ps[3] = ps[2] = ps[pn-1];
	    pn = 4;
        }
	if (pn == 0)
	    return;
	for (i = 0; i < pn; i++)
	    points[pointn++] = ps[i];
	recover_slack(segfirst, P);
	hn = hackflag ? fwdedgeb.head : e->head;
    }

    /* make copies of the spline points, one per multi-edge */

    if (cnt == 1) {
	clip_and_install(fe, hn, points, pointn, &sinfo);
	return;
    }
    dx = sp->Multisep * (cnt - 1) / 2;
    for (i = 1; i < pointn - 1; i++)
	points[i].x -= dx;
    for (i = 0; i < pointn; i++)
	points2[i] = points[i];
    clip_and_install(fe, hn, points2, pointn, &sinfo);
    for (j = 1; j < cnt; j++) {
	e = edges[ind + j];
	if (ED_tree_index(e) & BWDEDGE) {
	    MAKEFWDEDGE(&fwdedge, e);
	    e = &fwdedge;
	}
	for (i = 1; i < pointn - 1; i++)
	    points[i].x += sp->Multisep;
	for (i = 0; i < pointn; i++)
	    points2[i] = points[i];
	clip_and_install(e, e->head, points2, pointn, &sinfo);
    }
}

/* flat edges */

#ifdef OBSOLETE
static void 
chooseflatsides(pathend_t* tendp, pathend_t *hendp,
                int* tsidep, int* hsidep, int* msidep, int* tdirp, 
                int* hdirp, int* crossp)
{
    int i;

    for (i = 0; i < 16; i++)
	if ((flatsidemap[i][0] & tendp->sidemask) &&
	    (flatsidemap[i][1] & hendp->sidemask))
	    break;
    if (i == 16)
	abort();
    *tsidep = flatsidemap[i][0], *hsidep = flatsidemap[i][1];
    *msidep = flatsidemap[i][2];
    *tdirp = flatsidemap[i][3], *hdirp = flatsidemap[i][4];
    *crossp = flatsidemap[i][5];
}

static void
completeflatpath(path * P,
		 pathend_t * tendp, pathend_t * hendp,
		 int tside, int hside, int mside, int tdir, int hdir,
		 box * arg_lb, box * arg_rb, int w, int h)
{
    int i, side, boxn;
    box boxes[8];
    box tb, hb;
    box lb, rb;
    lb = *arg_lb;
    rb = *arg_rb;

    tb = makeflatend(tendp->boxes[tendp->boxn - 1], tside, tdir, lb);
    hb = makeflatend(hendp->boxes[hendp->boxn - 1], hside, OTHERDIR(hdir),
		     rb);

    boxn = 0;
    for (side = tside;; side = NEXTSIDE(side, tdir)) {
	boxes[boxn++] = makeflatcomponent(lb, rb, side,
					  (side == mside) ? 0 : -1, tdir,
					  w, h);
	if (side == mside)
	    break;
    }
    if (mside == RIGHT)
	mside = LEFT;
    if (mside != hside) {
	for (side = NEXTSIDE(mside, hdir);; side = NEXTSIDE(side, hdir)) {
	    boxes[boxn++] = makeflatcomponent(lb, rb, side, 1, hdir, w, h);
	    if (side == hside)
		break;
	}
    }

    for (i = 0; i < tendp->boxn; i++)
	add_box(P, tendp->boxes[i]);
    if (tb.LL.x != tb.UR.x && tb.LL.y != tb.UR.y)
	add_box(P, tb);
    for (i = 0; i < boxn; i++)
	add_box(P, boxes[i]);
    if (hb.LL.x != hb.UR.x && hb.LL.y != hb.UR.y)
	add_box(P, hb);
    for (i = hendp->boxn - 1; i >= 0; i--)
	add_box(P, hendp->boxes[i]);
}

static box 
makeflatend(box b, int side, int dir, box bb)
{
    box eb = { {0, 0}, {0, 0} };

    switch (side) {
    case BOTTOM:
	eb = boxof(b.LL.x, bb.LL.y, b.UR.x, b.LL.y);
	if (dir == CCW)
	    eb.UR.x += (bb.UR.x - b.UR.x) / 2;
	else
	    eb.LL.x -= (b.LL.x - bb.LL.x) / 2;
	break;
    case RIGHT:
	eb = boxof(b.UR.x, b.LL.y, bb.UR.x, b.UR.y);
	if (dir == CCW)
	    eb.UR.y += (bb.UR.y - b.UR.y) / 2;
	else
	    eb.LL.y -= (b.LL.y - bb.LL.y) / 2;
	break;
    case TOP:
	eb = boxof(b.LL.x, b.UR.y, b.UR.x, bb.UR.y);
	if (dir == CCW)
	    eb.LL.x -= (b.LL.x - bb.LL.x) / 2;
	else
	    eb.UR.x += (bb.UR.x - b.UR.x) / 2;
	break;
    case LEFT:
	eb = boxof(bb.LL.x, b.LL.y, b.LL.x, b.UR.y);
	if (dir == CCW)
	    eb.LL.y -= (bb.UR.y - b.UR.y) / 2;
	else
	    eb.UR.y += (b.LL.y - bb.LL.y) / 2;
	break;
    }
    return eb;
}

static box makeflatcomponent(lb, rb, side, mode, dir, w, h)
box lb, rb;
int side, mode, dir, w, h;
{
    box b = { {0, 0}, {0, 0} };

    /* mode == -1 means use left box, 1 means use right box
       and 0 means use mostly the left box */

    switch (side) {
    case BOTTOM:
	b.LL.x = lb.LL.x - w, b.UR.x = rb.UR.x + w;
	if (mode <= 0)
	    b.LL.y = lb.LL.y - h, b.UR.y = lb.LL.y;
	else
	    b.LL.y = rb.LL.y - h, b.UR.y = rb.LL.y;
	break;
    case RIGHT:
	if (mode == -1) {
	    b.LL.x = lb.UR.x, b.UR.x = lb.UR.x + w;
	    b.LL.y = lb.LL.y, b.UR.y = lb.UR.y;
	} else if (mode == 0) {
	    b.LL.x = lb.UR.x, b.UR.x = lb.UR.x + w;
	    if (dir == CCW)
		b.LL.y = lb.LL.y, b.UR.y = rb.UR.y;
	    else
		b.LL.y = rb.LL.y, b.UR.y = lb.UR.y;
	} else {
	    b.LL.x = rb.UR.x, b.UR.x = rb.UR.x + w;
	    b.LL.y = rb.LL.y, b.UR.y = rb.UR.y;
	}
	break;
    case TOP:
	b.LL.x = lb.LL.x - w, b.UR.x = rb.UR.x + w;
	if (mode <= 0)
	    b.LL.y = lb.UR.y, b.UR.y = lb.UR.y + h;
	else
	    b.LL.y = rb.UR.y, b.UR.y = rb.UR.y + h;
	break;
    case LEFT:
	if (mode == -1) {
	    b.LL.x = lb.LL.x - w, b.UR.x = lb.LL.x;
	    b.LL.y = lb.LL.y, b.UR.y = lb.UR.y;
	} else if (mode == 0) {
	    b.LL.x = lb.LL.x - w, b.UR.x = lb.LL.x;
	    if (dir == CCW)
		b.LL.y = lb.LL.y, b.UR.y = rb.UR.y;
	    else
		b.LL.y = rb.LL.y, b.UR.y = lb.UR.y;
	} else {
	    b.LL.x = rb.LL.x - w, b.UR.x = rb.LL.x;
	    b.LL.y = rb.LL.y, b.UR.y = rb.UR.y;
	}
	break;
    }
    return b;
}
static void
completeflatpath(path* P, pathend_t* tendp, pathend_t* hendp,
		 box* lbp, box* rbp, int w, int h)
{
    int i;
    box wbox;
    box tb, hb;
    box lb, rb;
    lb = *lbp;
    rb = *rbp;

    tb = makeflatend(tendp->boxes[tendp->boxn - 1], TOP, CW, lb);
    hb = makeflatend(hendp->boxes[hendp->boxn - 1], TOP, CCW, rb);

    wbox = makeflatcomponent(lb, rb, TOP, 0, CW, w, h);

    for (i = 0; i < tendp->boxn; i++)
	add_box(P, tendp->boxes[i]);
    add_box(P, tb);
    add_box(P, wbox);
    for (i = hendp->boxn - 1; i >= 0; i--)
	add_box(P, hendp->boxes[i]);
}
#endif

/* regular edges */

#define DONT_WANT_ANY_ENDPOINT_PATH_REFINEMENT
#ifdef DONT_WANT_ANY_ENDPOINT_PATH_REFINEMENT
static void
completeregularpath(path * P, edge_t * first, edge_t * last,
		    pathend_t * tendp, pathend_t * hendp, box * boxes,
		    int boxn, int flag)
{
    edge_t *uleft, *uright, *lleft, *lright;
    int i, fb, lb;
    splines *spl;
    point *pp;
    int pn;

    fb = lb = -1;
    uleft = uright = NULL;
    uleft = top_bound(first, -1), uright = top_bound(first, 1);
    if (uleft) {
	spl = getsplinepoints(uleft);
	pp = spl->list[0].list, pn = spl->list[0].size;
	P->ulpp = &pp[0];
    }
    if (uright) {
	spl = getsplinepoints(uright);
	pp = spl->list[0].list, pn = spl->list[0].size;
	P->urpp = &pp[0];
    }
    lleft = lright = NULL;
    lleft = bot_bound(last, -1), lright = bot_bound(last, 1);
    if (lleft) {
	spl = getsplinepoints(lleft);
	pp = spl->list[spl->size - 1].list, pn =
	    spl->list[spl->size - 1].size;
	P->llpp = &pp[pn - 1];
    }
    if (lright) {
	spl = getsplinepoints(lright);
	pp = spl->list[spl->size - 1].list, pn =
	    spl->list[spl->size - 1].size;
	P->lrpp = &pp[pn - 1];
    }
    for (i = 0; i < tendp->boxn; i++)
	add_box(P, tendp->boxes[i]);
    fb = P->nbox + 1;
    lb = fb + boxn - 3;
    for (i = 0; i < boxn; i++)
	add_box(P, boxes[i]);
    for (i = hendp->boxn - 1; i >= 0; i--)
	add_box(P, hendp->boxes[i]);
    adjustregularpath(P, fb, lb);
}
#else
void refineregularends(edge_t * left, edge_t * right, pathend_t * endp,
		       int dir, box b, box * boxes, int *boxnp);

/* box subdivision is obsolete, I think... ek */
static void
completeregularpath(path * P, edge_t * first, edge_t * last,
		    pathend_t * tendp, pathend_t * hendp, box * boxes,
		    int boxn, int flag)
{
    edge_t *uleft, *uright, *lleft, *lright;
    box uboxes[NSUB], lboxes[NSUB];
    box b;
    int uboxn, lboxn, i, y, fb, lb;

    fb = lb = -1;
    uleft = uright = NULL;
    if (flag || ND_rank(first->tail) + 1 != ND_rank(last->head))
	uleft = top_bound(first, -1), uright = top_bound(first, 1);
    refineregularends(uleft, uright, tendp, 1, boxes[0], uboxes, &uboxn);
    lleft = lright = NULL;
    if (flag || ND_rank(first->tail) + 1 != ND_rank(last->head))
	lleft = bot_bound(last, -1), lright = bot_bound(last, 1);
    refineregularends(lleft, lright, hendp, -1, boxes[boxn - 1], lboxes,
		      &lboxn);
    for (i = 0; i < tendp->boxn; i++)
	add_box(P, tendp->boxes[i]);
    if (ND_rank(first->tail) + 1 == ND_rank(last->head)) {
	if ((!uleft && !uright) && (lleft || lright)) {
	    b = boxes[0];
	    y = b.UR.y - b.LL.y;
	    for (i = 0; i < NSUB; i++) {
		uboxes[i] = b;
		uboxes[i].UR.y = b.UR.y - y * i / NSUB;
		uboxes[i].LL.y = b.UR.y - y * (i + 1) / NSUB;
	    }
	    uboxn = NSUB;
	} else if ((uleft || uright) && (!lleft && !lright)) {
	    b = boxes[boxn - 1];
	    y = b.UR.y - b.LL.y;
	    for (i = 0; i < NSUB; i++) {
		lboxes[i] = b;
		lboxes[i].UR.y = b.UR.y - y * i / NSUB;
		lboxes[i].LL.y = b.UR.y - y * (i + 1) / NSUB;
	    }
	    lboxn = NSUB;
	}
	for (i = 0; i < uboxn; i++) {
	    uboxes[i].LL.x = MAX(uboxes[i].LL.x, lboxes[i].LL.x);
	    uboxes[i].UR.x = MIN(uboxes[i].UR.x, lboxes[i].UR.x);
	}
	for (i = 0; i < uboxn; i++)
	    add_box(P, uboxes[i]);
    } else {
	for (i = 0; i < uboxn; i++)
	    add_box(P, uboxes[i]);
	fb = P->nbox;
	lb = fb + boxn - 3;
	for (i = 1; i < boxn - 1; i++)
	    add_box(P, boxes[i]);
	for (i = 0; i < lboxn; i++)
	    add_box(P, lboxes[i]);
    }
    for (i = hendp->boxn - 1; i >= 0; i--)
	add_box(P, hendp->boxes[i]);
    adjustregularpath(P, fb, lb);
}
#endif

/* makeregularend:
 * Add box to fill between node and interrank space. Needed because
 * nodes in a given rank can differ in height.
 * for now, regular edges always go from top to bottom 
 */
static box makeregularend(box b, int side, int y)
{
    box newb;
    switch (side) {
    case BOTTOM:
	newb = boxof(b.LL.x, y, b.UR.x, b.LL.y);
	break;
    case TOP:
	newb = boxof(b.LL.x, b.UR.y, b.UR.x, y);
	break;
    }
    return newb;
}

#ifndef DONT_WANT_ANY_ENDPOINT_PATH_REFINEMENT
void refineregularends(left, right, endp, dir, b, boxes, boxnp)
edge_t *left, *right;
pathend_t *endp;
int dir;
box b;
box *boxes;
int *boxnp;
{
    splines *lspls, *rspls;
    point pp, cp;
    box eb;
    box *bp;
    int y, i, j, k;
    int nsub;

    y = b.UR.y - b.LL.y;
    if ((y == 1) || (!left && !right)) {
	boxes[0] = b;
	*boxnp = 1;
	return;
    }
    nsub = MIN(NSUB, y);
    for (i = 0; i < nsub; i++) {
	boxes[i] = b;
	boxes[i].UR.y = b.UR.y - y * i / nsub;
	boxes[i].LL.y = b.UR.y - y * (i + 1) / nsub;
	if (boxes[i].UR.y == boxes[i].LL.y)
	    abort();
    }
    *boxnp = nsub;
    /* only break big boxes */
    for (j = 0; j < endp->boxn; j++) {
	eb = endp->boxes[j];
	y = eb.UR.y - eb.LL.y;
#ifdef STEVE_AND_LEFTY_GRASPING_AT_STRAWS
	if (y < 15)
	    continue;
#else
	if (y < nsub)
	    continue;
#endif
	for (k = endp->boxn - 1; k > j; k--)
	    endp->boxes[k + (nsub - 1)] = endp->boxes[k];
	for (i = 0; i < nsub; i++) {
	    bp = &endp->boxes[j + ((dir == 1) ? i : (nsub - i - 1))];
	    *bp = eb;
	    bp->UR.y = eb.UR.y - y * i / nsub;
	    bp->LL.y = eb.UR.y - y * (i + 1) / nsub;
	    if (bp->UR.y == bp->LL.y)
		abort();
	}
	endp->boxn += (nsub - 1);
	j += nsub - 1;
    }
    if (left) {
	lspls = getsplinepoints(left);
	pp = spline_at_y(lspls, boxes[0].UR.y);
	for (i = 0; i < nsub; i++) {
	    cp = spline_at_y(lspls, boxes[i].LL.y);
	    /*boxes[i].LL.x = AVG (pp.x, cp.x); */
	    boxes[i].LL.x = MAX(pp.x, cp.x);
	    pp = cp;
	}
	pp = spline_at_y(lspls, (dir == 1) ?
			 endp->boxes[1].UR.y : endp->boxes[1].LL.y);
	for (i = 1; i < endp->boxn; i++) {
	    cp = spline_at_y(lspls, (dir == 1) ?
			     endp->boxes[i].LL.y : endp->boxes[i].UR.y);
	    endp->boxes[i].LL.x = MIN(endp->nb.UR.x, MAX(pp.x, cp.x));
	    pp = cp;
	}
	i = (dir == 1) ? 0 : *boxnp - 1;
	if (boxes[i].LL.x > endp->boxes[endp->boxn - 1].UR.x - MINW)
	    boxes[i].LL.x = endp->boxes[endp->boxn - 1].UR.x - MINW;
    }
    if (right) {
	rspls = getsplinepoints(right);
	pp = spline_at_y(rspls, boxes[0].UR.y);
	for (i = 0; i < nsub; i++) {
	    cp = spline_at_y(rspls, boxes[i].LL.y);
	    /*boxes[i].UR.x = AVG (pp.x, cp.x); */
	    boxes[i].UR.x = AVG(pp.x, cp.x);
	    pp = cp;
	}
	pp = spline_at_y(rspls, (dir == 1) ?
			 endp->boxes[1].UR.y : endp->boxes[1].LL.y);
	for (i = 1; i < endp->boxn; i++) {
	    cp = spline_at_y(rspls, (dir == 1) ?
			     endp->boxes[i].LL.y : endp->boxes[i].UR.y);
	    endp->boxes[i].UR.x = MAX(endp->nb.LL.x, AVG(pp.x, cp.x));
	    pp = cp;
	}
	i = (dir == 1) ? 0 : *boxnp - 1;
	if (boxes[i].UR.x < endp->boxes[endp->boxn - 1].LL.x + MINW)
	    boxes[i].UR.x = endp->boxes[endp->boxn - 1].LL.x + MINW;
    }
}
#endif

/* adjustregularpath:
 * make sure the path is wide enough.
 * the % 2 was so that in rank boxes would only be grown if
 * they were == 0 while inter-rank boxes could be stretched to a min
 * width.
 * The list of boxes has three parts: tail boxes, path boxes, and head
 * boxes. (Note that because of back edges, the tail boxes might actually
 * belong to the head node, and vice versa.) fb is the index of the
 * first interrank path box and lb is the last interrank path box.
 * If fb > lb, there are none.
 *
 * The second for loop was added by ek long ago, and apparently is intended
 * to guarantee an overlap between adjacent boxes of at least MINW.
 * It doesn't do this, and the ifdef'ed part has the potential of moving 
 * a box within a node for more complex paths.
 */
static void adjustregularpath(path * P, int fb, int lb)
{
    box *bp1, *bp2;
    int i, x;

    for (i = fb-1; i < lb+1; i++) {
	bp1 = &P->boxes[i];
	if ((i - fb) % 2 == 0) {
	    if (bp1->LL.x >= bp1->UR.x) {
		x = (bp1->LL.x + bp1->UR.x) / 2;
		bp1->LL.x = x - HALFMINW, bp1->UR.x = x + HALFMINW;
	    }
	} else {
	    if (bp1->LL.x + MINW > bp1->UR.x) {
		x = (bp1->LL.x + bp1->UR.x) / 2;
		bp1->LL.x = x - HALFMINW, bp1->UR.x = x + HALFMINW;
	    }
	}
    }
    for (i = 0; i < P->nbox - 1; i++) {
	bp1 = &P->boxes[i], bp2 = &P->boxes[i + 1];
	if (i >= fb && i <= lb && (i - fb) % 2 == 0) {
	    if (bp1->LL.x + MINW > bp2->UR.x)
		bp2->UR.x = bp1->LL.x + MINW;
	    if (bp1->UR.x - MINW < bp2->LL.x)
		bp2->LL.x = bp1->UR.x - MINW;
	} else if (i + 1 >= fb && i < lb && (i + 1 - fb) % 2 == 0) {
	    if (bp1->LL.x + MINW > bp2->UR.x)
		bp1->LL.x = bp2->UR.x - MINW;
	    if (bp1->UR.x - MINW < bp2->LL.x)
		bp1->UR.x = bp2->LL.x + MINW;
	} 
#ifdef OLD
	else {
	    if (bp1->LL.x + MINW > bp2->UR.x) {
		x = (bp1->LL.x + bp2->UR.x) / 2;
		bp1->LL.x = x - HALFMINW;
		bp2->UR.x = x + HALFMINW;
	    }
	    if (bp1->UR.x - MINW < bp2->LL.x) {
		x = (bp1->UR.x + bp2->LL.x) / 2;
		bp1->UR.x = x + HALFMINW;
		bp2->LL.x = x - HALFMINW;
	    }
	}
#endif
    }
}

static box rank_box(spline_info_t* sp, graph_t * g, int r)
{
    box b;
    node_t /* *right0, *right1, */  * left0, *left1;

    b = sp->Rank_box[r];
    if (b.LL.x == b.UR.x) {
	left0 = GD_rank(g)[r].v[0];
	/* right0 = GD_rank(g)[r].v[GD_rank(g)[r].n - 1]; */
	left1 = GD_rank(g)[r + 1].v[0];
	/* right1 = GD_rank(g)[r + 1].v[GD_rank(g)[r + 1].n - 1]; */
	b.LL.x = sp->LeftBound;
	b.LL.y = ND_coord_i(left1).y + GD_rank(g)[r + 1].ht2;
	b.UR.x = sp->RightBound;
	b.UR.y = ND_coord_i(left0).y - GD_rank(g)[r].ht1;
	sp->Rank_box[r] = b;
    }
    return b;
}

/* returns count of vertically aligned edges starting at n */
static int straight_len(node_t * n)
{
    int cnt = 0;
    node_t *v;

    v = n;
    while (1) {
	v = ND_out(v).list[0]->head;
	if (ND_node_type(v) != VIRTUAL)
	    break;
	if ((ND_out(v).size != 1) || (ND_in(v).size != 1))
	    break;
	if (ND_coord_i(v).x != ND_coord_i(n).x)
	    break;
	cnt++;
    }
    return cnt;
}

static edge_t *straight_path(edge_t * e, int cnt, point * plist, int *np)
{
    int n = *np;
    edge_t *f = e;

    while (cnt--)
	f = ND_out(f->head).list[0];
    plist[(*np)++] = plist[n - 1];
    plist[(*np)++] = plist[n - 1];
    plist[(*np)] = ND_coord_i(f->tail);	/* will be overwritten by next spline */
    return f;
}

static void recover_slack(edge_t * e, path * p)
{
    int b;
    node_t *vn;

    b = 0;			/* skip first rank box */
    for (vn = e->head;
	 ND_node_type(vn) == VIRTUAL && !sinfo.splineMerge(vn);
	 vn = ND_out(vn).list[0]->head) {
	while ((b < p->nbox) && (p->boxes[b].LL.y > ND_coord_i(vn).y))
	    b++;
	if (b >= p->nbox)
	    break;
	if (p->boxes[b].UR.y < ND_coord_i(vn).y)
	    continue;
	if (ND_label(vn))
	    resize_vn(vn, p->boxes[b].LL.x, p->boxes[b].UR.x,
		      p->boxes[b].UR.x + ND_rw_i(vn));
	else
	    resize_vn(vn, p->boxes[b].LL.x, (p->boxes[b].LL.x +
					     p->boxes[b].UR.x) / 2,
		      p->boxes[b].UR.x);
    }
}

static void resize_vn(vn, lx, cx, rx)
node_t *vn;
int lx, cx, rx;
{
    ND_coord_i(vn).x = cx;
    ND_lw_i(vn) = cx - lx, ND_rw_i(vn) = rx - cx;
}

/* side > 0 means right. side < 0 means left */
static edge_t *top_bound(edge_t * e, int side)
{
    edge_t *f, *ans = NULL;
    int i;

    for (i = 0; (f = ND_out(e->tail).list[i]); i++) {
#if 0				/* were we out of our minds? */
	if (ED_tail_port(e).p.x != ED_tail_port(f).p.x)
	    continue;
#endif
	if (side * (ND_order(f->head) - ND_order(e->head)) <= 0)
	    continue;
	if ((ED_spl(f) == NULL)
	    && ((ED_to_orig(f) == NULL) || (ED_to_orig(f)->u.spl == NULL)))
	    continue;
	if ((ans == NULL)
	    || (side * (ND_order(ans->head) - ND_order(f->head)) > 0))
	    ans = f;
    }
    return ans;
}

static edge_t *bot_bound(edge_t * e, int side)
{
    edge_t *f, *ans = NULL;
    int i;

    for (i = 0; (f = ND_in(e->head).list[i]); i++) {
#if 0				/* same here */
	if (ED_head_port(e).p.x != ED_head_port(f).p.x)
	    continue;
#endif
	if (side * (ND_order(f->tail) - ND_order(e->tail)) <= 0)
	    continue;
	if ((ED_spl(f) == NULL)
	    && ((ED_to_orig(f) == NULL) || (ED_to_orig(f)->u.spl == NULL)))
	    continue;
	if ((ans == NULL)
	    || (side * (ND_order(ans->tail) - ND_order(f->tail)) > 0))
	    ans = f;
    }
    return ans;
}

point closest(splines * spl, point p)
{
    int i, j, k, besti, bestj;
    double bestdist2, d2, dlow2, dhigh2; /* squares of distance */
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

/* common routines */

static int cl_vninside(graph_t * cl, node_t * n)
{
    return (BETWEEN(GD_bb(cl).LL.x, (double)(ND_coord_i(n).x), GD_bb(cl).UR.x) &&
	    BETWEEN(GD_bb(cl).LL.y, (double)(ND_coord_i(n).y), GD_bb(cl).UR.y));
}

/* returns the cluster of (adj) that interferes with n,
 */
static graph_t *cl_bound(n, adj)
node_t *n, *adj;
{
    graph_t *rv, *cl, *tcl, *hcl;
    edge_t *orig;

    rv = NULL;
    if (ND_node_type(n) == NORMAL)
	tcl = hcl = ND_clust(n);
    else {
	orig = ND_out(n).list[0]->u.to_orig;
	tcl = ND_clust(orig->tail);
	hcl = ND_clust(orig->head);
    }
    if (ND_node_type(adj) == NORMAL) {
	cl = ND_clust(adj);
	if (cl && (cl != tcl) && (cl != hcl))
	    rv = cl;
    } else {
	orig = ED_to_orig(ND_out(adj).list[0]);
	cl = ND_clust(orig->tail);
	if (cl && (cl != tcl) && (cl != hcl) && cl_vninside(cl, adj))
	    rv = cl;
	else {
	    cl = ND_clust(orig->head);
	    if (cl && (cl != tcl) && (cl != hcl) && cl_vninside(cl, adj))
		rv = cl;
	}
    }
    return rv;
}

/* maximal_bbox:
 * Return an initial bounding box to be used for building the
 * beginning or ending of the path of boxes.
 * Height reflects height of tallest node on rank.
 * The extra space provided by FUDGE allows begin/endpath to create a box
 * FUDGE-2 away from the node, so the routing can avoid the node and the
 * box is at least 2 wide.
 */
#define FUDGE 4

static box maximal_bbox(spline_info_t* sp, node_t* vn, edge_t* ie, edge_t* oe)
{
    double b, nb;
    graph_t *g = vn->graph, *left_cl, *right_cl;
    node_t *left, *right;
    box rv;

    left_cl = right_cl = NULL;

    /* give this node all the available space up to its neighbors */
    b = (double)(ND_coord_i(vn).x - ND_lw_i(vn) - FUDGE);
    if ((left = neighbor(vn, ie, oe, -1))) {
	if ((left_cl = cl_bound(vn, left)))
	    nb = GD_bb(left_cl).UR.x + (double)(sp->Splinesep);
	else {
	    nb = (double)(ND_coord_i(left).x + ND_mval(left));
	    if (ND_node_type(left) == NORMAL)
		nb += GD_nodesep(g) / 2.;
	    else
		nb += (double)(sp->Splinesep);
	}
	if (nb < b)
	    b = nb;
	rv.LL.x = ROUND(b);
    } else
	rv.LL.x = MIN(ROUND(b), sp->Splinesep);

    /* we have to leave room for our own label! */
    if ((ND_node_type(vn) == VIRTUAL) && (ND_label(vn)))
	b = (double)(ND_coord_i(vn).x + 10);
    else
	b = (double)(ND_coord_i(vn).x + ND_rw_i(vn) + FUDGE);
    if ((right = neighbor(vn, ie, oe, 1))) {
	if ((right_cl = cl_bound(vn, right)))
	    nb = GD_bb(right_cl).LL.x - (double)(sp->Splinesep);
	else {
	    nb = ND_coord_i(right).x - ND_lw_i(right);
	    if (ND_node_type(right) == NORMAL)
		nb -= GD_nodesep(g) / 2.;
	    else
		nb -= (double)(sp->Splinesep);
	}
	if (nb > b)
	    b = nb;
	rv.UR.x = ROUND(b);
    } else
	rv.UR.x = MAX(ROUND(b), sp->RightBound);

    if ((ND_node_type(vn) == VIRTUAL) && (ND_label(vn)))
	rv.UR.x -= ND_rw_i(vn);

    rv.LL.y = ND_coord_i(vn).y - GD_rank(g)[ND_rank(vn)].ht1;
    rv.UR.y = ND_coord_i(vn).y + GD_rank(g)[ND_rank(vn)].ht2;
    return rv;
}

static node_t *neighbor(vn, ie, oe, dir)
node_t *vn;
edge_t *ie, *oe;
int dir;
{
    int i;
    node_t *n, *rv = NULL;
    rank_t *rank = &(GD_rank(vn->graph)[ND_rank(vn)]);

    for (i = ND_order(vn) + dir; ((i >= 0) && (i < rank->n)); i += dir) {
	n = rank->v[i];
	if ((ND_node_type(n) == VIRTUAL) && (ND_label(n))) {
	    rv = n;
	    break;
	}
	if (ND_node_type(n) == NORMAL) {
	    rv = n;
	    break;
	}
	if (pathscross(n, vn, ie, oe) == FALSE) {
	    rv = n;
	    break;
	}
    }
    return rv;
}

static boolean pathscross(n0, n1, ie1, oe1)
node_t *n0, *n1;
edge_t *ie1, *oe1;
{
    edge_t *e0, *e1;
    node_t *na, *nb;
    int order, cnt;

    order = (ND_order(n0) > ND_order(n1));
    if ((ND_out(n0).size != 1) && (ND_out(n0).size != 1))
	return FALSE;
    e1 = oe1;
    if (ND_out(n0).size == 1 && e1) {
	e0 = ND_out(n0).list[0];
	for (cnt = 0; cnt < 2; cnt++) {
	    if ((na = e0->head) == (nb = e1->head))
		break;
	    if (order != (ND_order(na) > ND_order(nb)))
		return TRUE;
	    if ((ND_out(na).size != 1) || (ND_node_type(na) == NORMAL))
		break;
	    e0 = ND_out(na).list[0];
	    if ((ND_out(nb).size != 1) || (ND_node_type(nb) == NORMAL))
		break;
	    e1 = ND_out(nb).list[0];
	}
    }
    e1 = ie1;
    if (ND_in(n0).size == 1 && e1) {
	e0 = ND_in(n0).list[0];
	for (cnt = 0; cnt < 2; cnt++) {
	    if ((na = e0->tail) == (nb = e1->tail))
		break;
	    if (order != (ND_order(na) > ND_order(nb)))
		return TRUE;
	    if ((ND_in(na).size != 1) || (ND_node_type(na) == NORMAL))
		break;
	    e0 = ND_in(na).list[0];
	    if ((ND_in(nb).size != 1) || (ND_node_type(nb) == NORMAL))
		break;
	    e1 = ND_in(nb).list[0];
	}
    }
    return FALSE;
}

#ifdef DEBUG
void showpath(path * p)
{
    int i;
    point LL, UR;

    fprintf(stderr, "%%!PS\n");
    for (i = 0; i < p->nbox; i++) {
	LL = p->boxes[i].LL;
	UR = p->boxes[i].UR;
	fprintf(stderr,
		"newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto closepath stroke\n",
		LL.x, LL.y, UR.x, LL.y, UR.x, UR.y, LL.x, UR.y);
    }
    fprintf(stderr, "showpage\n");
}
#endif
