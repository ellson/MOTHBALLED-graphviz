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

#include <time.h>
#ifndef MSWIN32
#include <unistd.h>
#endif
#include "neato.h"
#include "pack.h"
#include "stress.h"
#ifdef DIGCOLA
#include "digcola.h"
#endif
#include "kkutils.h"
#include "pointset.h"
#include <ctype.h>

#ifndef HAVE_SRAND48
#define srand48 srand
#endif

static attrsym_t *N_pos;
static int Pack;		/* If >= 0, layout components separately and pack together
				 * The value of Pack gives margins around graphs.
				 */
static char *cc_pfx = "_neato_cc";

void neato_nodesize(node_t * n, bool flip)
{
    int w;

    w = ND_xsize(n) = POINTS(ND_width(n));
    ND_lw_i(n) = ND_rw_i(n) = w / 2;
    ND_ht_i(n) = ND_ysize(n) = POINTS(ND_height(n));
}

void neato_init_node(node_t * n)
{
    common_init_node(n);
    ND_pos(n) = ALLOC(GD_ndim(n->graph), 0, double);
    neato_nodesize(n, GD_flip(n->graph));
}

void neato_init_edge(edge_t * e)
{
    common_init_edge(e);

    ED_factor(e) = late_double(e, E_weight, 1.0, 1.0);
}

int user_pos(attrsym_t * posptr, attrsym_t * pinptr, node_t * np, int nG)
{
    double *pvec;
    char *p, c;
    double z;

    if (posptr == NULL)
	return FALSE;
    pvec = ND_pos(np);
    p = agxget(np, posptr->index);
    if (p[0]) {
	c = '\0';
	if ((Ndim >= 3) && 
            (sscanf(p, "%lf,%lf,%lf%c", pvec, pvec+1, pvec+2, &c) >= 3)){
	    ND_pinned(np) = P_SET;
	    if (PSinputscale > 0.0) {
		int i;
		for (i = 0; i < Ndim; i++)
		    pvec[i] = pvec[i] / PSinputscale;
	    }
	    if (Ndim > 3)
		jitter_d(np, nG, 3);
	    if ((c == '!')
		|| (pinptr && mapbool(agxget(np, pinptr->index))))
		ND_pinned(np) = P_PIN;
	    return TRUE;
	}
	else if (sscanf(p, "%lf,%lf%c", pvec, pvec + 1, &c) >= 2) {
	    ND_pinned(np) = P_SET;
	    if (PSinputscale > 0.0) {
		int i;
		for (i = 0; i < Ndim; i++)
		    pvec[i] = pvec[i] / PSinputscale;
	    }
	    if (Ndim > 2) {
		if (N_z && (p = agxget(np, N_z->index)) && 
                           (sscanf(p,"%lf",&z) == 1)) { 
		    if (PSinputscale > 0.0) {
			pvec[2] = z / PSinputscale;
		    }
		    else
			pvec[2] = z;
		    jitter_d(np, nG, 3);
		}
		else
		    jitter3d(np, nG);
	    }
	    if ((c == '!')
		|| (pinptr && mapbool(agxget(np, pinptr->index))))
		ND_pinned(np) = P_PIN;
	    return TRUE;
	} else
	    agerr(AGERR, "node %s, position %s, expected two doubles\n",
		  np->name, p);
    }
    return FALSE;
}

void neato_init_node_edge(graph_t * g)
{
    node_t *n;
    edge_t *e;
    int nG = agnnodes(g);
    attrsym_t *N_pin;

    N_pos = agfindattr(g->proto->n, "pos");
    N_pin = agfindattr(g->proto->n, "pin");

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	neato_init_node(n);
	user_pos(N_pos, N_pin, n, nG);	/* set user position if given */
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e))
	    neato_init_edge(e);
    }
}

void neato_cleanup_node(node_t * n)
{
    if (ND_shape(n)) {
	ND_shape(n)->fns->freefn(n);
    }
    free(ND_pos(n));
    free_label(ND_label(n));
    memset(&(n->u), 0, sizeof(Agnodeinfo_t));
}

void neato_free_splines(edge_t * e)
{
    int i;
    if (ED_spl(e)) {
	for (i = 0; i < ED_spl(e)->size; i++)
	    free(ED_spl(e)->list[i].list);
	free(ED_spl(e)->list);
	free(ED_spl(e));
    }
    ED_spl(e) = NULL;
}

void neato_cleanup_edge(edge_t * e)
{
    neato_free_splines(e);
    free_label(ED_label(e));
    memset(&(e->u), 0, sizeof(Agedgeinfo_t));
}

void neato_cleanup_graph(graph_t * g)
{
    if (Nop || (Pack < 0))
	free_scan_graph(g);
}

void neato_cleanup(graph_t * g)
{
    node_t *n;
    edge_t *e;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    neato_cleanup_edge(e);
	}
	neato_cleanup_node(n);
    }
    neato_cleanup_graph(g);
}

static int numFields(unsigned char *pos)
{
    int cnt = 0;
    unsigned char c;

    do {
	while (isspace(*pos))
	    pos++;		/* skip white space */
	if ((c = *pos)) { /* skip token */
	    cnt++;
	    while ((c = *pos) && !isspace(c) && (c != ';'))
		pos++;
	}
    } while (isspace(c));
    return cnt;
}

static void set_elabel(edge_t * e, textlabel_t * l, char *name)
{
    double x, y;
    point pt;
    char *lp;
    lp = agget(e, name);
    if (lp && (sscanf(lp, "%lf,%lf", &x, &y) == 2)) {
	pt.x = (int) (x);
	pt.y = (int) (y);
	l->p = pt;
	l->set = TRUE;
    }
}

/* user_spline:
 * Attempt to use already existing pos info for spline
 * Return 1 if successful, 0 otherwise.
 * Assume E_pos != NULL and ED_spl(e) == NULL.
 */
static int user_spline(attrsym_t * E_pos, edge_t * e)
{
    char *pos;
    int i, n, npts, nc;
    point *ps = 0;
    point *pp;
    double x, y;
    int sflag = 0, eflag = 0;
    point sp = { 0, 0 }, ep = {
    0, 0};
    bezier *newspl;
    int more = 1;
    int stype, etype;

    pos = agxget(e, E_pos->index);
    if (*pos == '\0')
	return 0;

    arrow_flags(e, &stype, &etype);
    do {
	/* check for s head */
	i = sscanf(pos, "s,%lf,%lf%n", &x, &y, &nc);
	if (i == 2) {
	    sflag = 1;
	    pos = pos + nc;
	    sp.x = (int) (x);
	    sp.y = (int) (y);
	}

	/* check for e head */
	i = sscanf(pos, " e,%lf,%lf%n", &x, &y, &nc);
	if (i == 2) {
	    eflag = 1;
	    pos = pos + nc;
	    ep.x = (int) (x);
	    ep.y = (int) (y);
	}

	npts = numFields((unsigned char *) pos);	/* count potential points */
	n = npts;
	if ((n < 4) || (n % 3 != 1)) {
	    neato_free_splines(e);
	    return 0;
	}
	ps = ALLOC(n, 0, point);
	pp = ps;
	while (n) {
	    i = sscanf(pos, "%lf,%lf%n", &x, &y, &nc);
	    if (i < 2) {
		free(ps);
		neato_free_splines(e);
		return 0;
	    }
	    pos = pos + nc;
	    pp->x = (int) (x);
	    pp->y = (int) (y);
	    pp++;
	    n--;
	}
 	while (isspace(*pos)) pos++;
	if (*pos == '\0')
	    more = 0;
	else
	    pos++;

	/* parsed successfully; create spline */
	newspl = new_spline(e, npts);
	if (sflag) {
	    newspl->sflag = stype;
	    newspl->sp = sp;
	}
	if (eflag) {
	    newspl->eflag = etype;
	    newspl->ep = ep;
	}
	for (i = 0; i < npts; i++) {
	    newspl->list[i] = ps[i];
	}
	free(ps);
    } while (more);

    if (ED_label(e))
	set_elabel(e, ED_label(e), "lp");
    if (ED_head_label(e))
	set_elabel(e, ED_head_label(e), "head_lp");
    if (ED_tail_label(e))
	set_elabel(e, ED_tail_label(e), "tail_lp");

    return 1;
}

/* Nop can be:
 *  0 - do full layout
 *  1 - assume initial node positions, do (optional) adjust and all splines
 *  2 - assume final node and edges positions, do nothing except compute
 *      missing splines
 */

 /* Indicates the amount of edges with position information */
typedef enum { NoEdges, SomeEdges, AllEdges } pos_edge;

/* nop_init_edges:
 * Check edges for position info.
 * If position info exists, check for edge label positions.
 * Return number of edges with position info.
 */
static pos_edge nop_init_edges(Agraph_t * g)
{
    node_t *n;
    edge_t *e;
    int nedges = 0;
    attrsym_t *E_pos = agfindattr(g->proto->e, "pos");

    if (!E_pos || (Nop < 2))
	return NoEdges;

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (user_spline(E_pos, e)) {
		nedges++;
	    }
	}
    }
    if (nedges) {
	if (nedges == agnedges(g))
	    return AllEdges;
	else
	    return SomeEdges;
    } else
	return NoEdges;
}

/* chkBB:
 * Scans for a correct bb attribute. If available, sets it
 * in the graph and returns 1.
 */
#define BS "%d,%d,%d,%d"

static int chkBB(Agraph_t * g, attrsym_t * G_bb)
{
    char *s;
    box bb;

    s = agxget(g, G_bb->index);
    if (sscanf(s, BS, &bb.LL.x, &bb.LL.y, &bb.UR.x, &bb.UR.y) == 4) {
	GD_bb(g) = bb;
	return 1;
    } else
	return 0;
}

static void add_cluster(Agraph_t * g, Agraph_t * subg)
{
    int cno;
    cno = ++(GD_n_cluster(g));
    GD_clust(g) = ZALLOC(cno + 1, GD_clust(g), graph_t *, GD_n_cluster(g));
    GD_clust(g)[cno] = subg;
    do_graph_label(subg);
}


static void nop_init_graphs(Agraph_t *, attrsym_t *, attrsym_t *);

/* dfs:
 */
static void
dfs(node_t * mn, Agraph_t * g, attrsym_t * G_lp, attrsym_t * G_bb)
{
    graph_t *subg;

    subg = agusergraph(mn);
    if (!strncmp(subg->name, "cluster", 7) && chkBB(subg, G_bb)) {
	add_cluster(g, subg);
	nop_init_graphs(subg, G_lp, G_bb);
    } else {
	graph_t *mg = g->meta_node->graph;
	edge_t *me;
	for (me = agfstout(mg, mn); me; me = agnxtout(mg, me)) {
	    dfs(me->head, g, G_lp, G_bb);
	}
    }
}

/* nop_init_graphs:
 * Read in clusters and graph label info.
 * A subgraph is a cluster if its name starts with "cluster" and
 * it has a valid bb.
 */
static void
nop_init_graphs(Agraph_t * g, attrsym_t * G_lp, attrsym_t * G_bb)
{
    graph_t *mg;
    edge_t *me;
    char *s;
    point p;

    if (GD_label(g) && G_lp) {
	s = agxget(g, G_lp->index);
	if (sscanf(s, "%d,%d", &p.x, &p.y) == 2) {
	    GD_label(g)->set = TRUE;
	    GD_label(g)->p = p;
	}
    }

    if (!G_bb)
	return;
    mg = g->meta_node->graph;
    for (me = agfstout(mg, g->meta_node); me; me = agnxtout(mg, me)) {
	dfs(me->head, g, G_lp, G_bb);
    }
}

/* translateE:
 * Translate edge by offset.
 * Assume ED_spl(e) != NULL
 */
static void translateE(edge_t * e, point offset)
{
    int i, j;
    point *pt;
    bezier *bez;

    bez = ED_spl(e)->list;
    for (i = 0; i < ED_spl(e)->size; i++) {
	pt = bez->list;
	for (j = 0; j < bez->size; j++) {
	    pt->x -= offset.x;
	    pt->y -= offset.y;
	    pt++;
	}
	if (bez->sflag) {
	    bez->sp.x -= offset.x;
	    bez->sp.y -= offset.y;
	}
	if (bez->eflag) {
	    bez->ep.x -= offset.x;
	    bez->ep.y -= offset.y;
	}
	bez++;
    }

    if (ED_label(e) && ED_label(e)->set) {
	ED_label(e)->p.x -= offset.x;
	ED_label(e)->p.y -= offset.y;
    }
    if (ED_head_label(e) && ED_head_label(e)->set) {
	ED_head_label(e)->p.x -= offset.x;
	ED_head_label(e)->p.y -= offset.y;
    }
    if (ED_tail_label(e) && ED_tail_label(e)->set) {
	ED_tail_label(e)->p.x -= offset.x;
	ED_tail_label(e)->p.y -= offset.y;
    }
}

/* translateG:
 */
static void translateG(Agraph_t * g, point offset)
{
    int i;

    GD_bb(g).UR.x -= offset.x;
    GD_bb(g).UR.y -= offset.y;
    GD_bb(g).LL.x -= offset.x;
    GD_bb(g).LL.y -= offset.y;

    if (GD_label(g) && GD_label(g)->set) {
	GD_label(g)->p.x -= offset.x;
	GD_label(g)->p.y -= offset.y;
    }

    for (i = 1; i <= GD_n_cluster(g); i++)
	translateG(GD_clust(g)[i], offset);
}

/* translate:
 */
static void translate(Agraph_t * g, pos_edge posEdges)
{
    node_t *n;
    edge_t *e;
    pointf offset;

    offset = cvt2ptf(GD_bb(g).LL);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	ND_pos(n)[0] -= offset.x;
	ND_pos(n)[1] -= offset.y;
    }
    if (posEdges != NoEdges) {
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e))
		if (ED_spl(e))
		    translateE(e, GD_bb(g).LL);
	}
    }
    translateG(g, GD_bb(g).LL);
}

/* init_nop:
 * This assumes all nodes have been positioned, and the
 * root graph has a bb defined, as name/value pairs. 
 * (We could possible weaken the latter and perform a 
 * recomputation of all bb.) It also assumes none of the 
 * relevant fields in A*info_t have set.
 * The input may provide additional position information for
 * clusters, edges and labels. If certain position information
 * is missing, init_nop will use a standard neato technique to
 * supply it.
 */
int init_nop(Agraph_t * g, int adjust)
{
    int i;
    node_t *np;
    pos_edge posEdges;		/* How many edges have spline info */
    attrsym_t *G_lp = agfindattr(g, "lp");
    attrsym_t *G_bb = agfindattr(g, "bb");

    scan_graph(g);		/* mainly to set up GD_neato_nlist */
    for (i = 0; (np = GD_neato_nlist(g)[i]); i++) {
	if (!hasPos(np) && strncmp(np->name, "cluster", 7)) {
	    agerr(AGERR, "node %s in graph %s has no position\n",
		  np->name, g->name);
	    return 1;
	}
    }
    nop_init_graphs(g, G_lp, G_bb);
    posEdges = nop_init_edges(g);

    if (adjust && Nop == 1)
	adjustNodes(g);

    /* If G_bb not defined, define it */
    if (!G_bb)
	G_bb = agraphattr(g, "bb", "");

    /* If g does not have a good "bb" attribute, compute it. */
    if (!chkBB(g, G_bb))
	compute_bb(g);

    /* At this point, all bounding boxes should be correctly defined.
     * If necessary, we translate the graph to the origin.
     */
    if (adjust && (GD_bb(g).LL.x || GD_bb(g).LL.y))
	translate(g, posEdges);

    if (!adjust) {
	node_t *n;
	State = GVSPLINES;
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    ND_coord_i(n).x = POINTS(ND_pos(n)[0]);
	    ND_coord_i(n).y = POINTS(ND_pos(n)[1]);
	}
    }
    else if (posEdges != AllEdges)
	spline_edges0(g);
    else {
	State = GVSPLINES;
	neato_set_aspect(g);
    }
    return 0;
}

void neato_init_graphn(Agraph_t * g, int dfltdim)
{
    GD_ndim(g->root) = late_int(g, agfindattr(g, "dim"), dfltdim, 2);
    Ndim = GD_ndim(g->root) = MIN(GD_ndim(g->root), MAXDIM);
    neato_init_node_edge(g);
}

void neato_init_graph(Agraph_t * g)
{
    neato_init_graphn(g, 2);
}

static int neatoModel(graph_t * g)
{
    char *p = agget(g, "model");
    char c;

    if (!p || (!(c = *p)))
	return MODEL_SHORTPATH;
    if ((c == 'c') && streq(p, "circuit"))
	return MODEL_CIRCUIT;
    if (c == 's') {
	if (streq(p, "subset"))
	    return MODEL_SUBSET;
	else if (streq(p, "shortpath"))
	    return MODEL_SHORTPATH;
    }
    agerr(AGWARN,
	  "Unknown value %s for attribute \"model\" in graph %s - ignored\n",
	  p, g->name);
    return MODEL_SHORTPATH;
}

/* neatoMode:
 */
static int neatoMode(graph_t * g)
{
    char *str;
    int mode = MODE_MAJOR;	/* default mode */

    str = agget(g, "mode");
    if (str && *str) {
	if (streq(str, "KK"))
	    mode = MODE_KK;
	else if (streq(str, "major"))
	    mode = MODE_MAJOR;
#ifdef DIGCOLA
	else if (streq(str, "hier"))
	    mode = MODE_HIER;
#endif
	else
	    agerr(AGWARN,
		  "Illegal value %s for attribute \"mode\" in graph %s - ignored\n",
		  str, g->name);
    }

    return mode;
}

/* checkEdge:
 *
 */
static int checkEdge(PointMap * pm, edge_t * ep, int idx)
{
    int i = ND_id(ep->tail);
    int j = ND_id(ep->head);
    int tmp;

    if (i > j) {
	tmp = i;
	i = j;
	j = tmp;
    }
    return insertPM(pm, i, j, idx);
}

#ifdef DIGCOLA
/* dfsCycle:
 * dfs for breaking cycles in vtxdata
 */
static void
dfsCycle (vtx_data* graph, int i)
{
    node_t *np, *hp;
    int j, e, f;

    np = graph[i].np;
    ND_mark(np) = TRUE;
    ND_onstack(np) = TRUE;
    for (e = 1; e < graph[i].nedges; e++) {
	if (graph[i].edists[e] == 1.0) continue;  /* in edge */
	j = graph[i].edges[e];
	hp = graph[j].np;
	if (ND_onstack(hp)) {  /* back edge: reverse it */
	    graph[i].edists[e] = 1.0;
	    for (f = 1; (f < graph[j].nedges) &&(graph[j].edges[f] != i); f++) ;
	    assert (f < graph[j].nedges);
	    graph[j].edists[f] = -1.0;
        }
	else if (ND_mark(hp) == FALSE) dfsCycle(graph, j);

    }
    ND_onstack(np) = FALSE;
}

/* acyclic:
 * Do a dfs of the vtx_data, looking for cycles, reversing edges.
 */
static void
acyclic (vtx_data* graph, int nv)
{
    int i;
    node_t* np;

    for (i = 0; i < nv; i++) {
	np = graph[i].np;
	ND_mark(np) = FALSE;
	ND_onstack(np) = FALSE;
    }
    for (i = 0; i < nv; i++) {
	if (ND_mark(graph[i].np)) continue;
	dfsCycle (graph, i);	
    }

}
#endif

/* makeGraphData:
 * Create sparse graph representation via arrays.
 * Each node is represented by a vtx_data.
 * The index of each neighbor is stored in the edges array;
 * the corresponding edge lengths and weights go on ewgts and eweights.
 * We do not allocate the latter 2 if the graph does not use them.
 * By convention, graph[i].edges[0] == i.
 * The values graph[i].ewgts[0] and graph[i].eweights[0] are left undefined.
 *
 * In constructing graph from g, we neglect loops. We track multiedges (ignoring
 * direction). Edge weights are additive; the final edge length is the max.
 *
 * If direction is used, we set the edists field, -1 for tail, +1 for head. 
 * graph[i].edists[0] is left undefined. If multiedges exist, the direction
 * of the first one encountered is used. Finally, a pass is made to guarantee
 * the graph is acyclic.
 *
 */
static vtx_data *makeGraphData(graph_t * g, int nv, int *nedges, int mode, int model)
{
    vtx_data *graph;
    int ne = agnedges(g);	/* upper bound */
    int *edges;
    float *ewgts = NULL;
    node_t *np;
    edge_t *ep;
    float *eweights = NULL;
#ifdef DIGCOLA
    float *edists = NULL;
#endif
    int haveLen;
    int haveWt;
    int haveDir;
    PointMap *ps = newPM();
    int i, i_nedges, idx;

    /* lengths and weights unused in reweight model */
    if (model == MODEL_SUBSET) {
	haveLen = FALSE;
	haveWt = FALSE;
    } else {
	haveLen = (agindex(g->root->proto->e, "len") >= 0);
	haveWt = (E_weight != 0);
    }
    if (mode == MODE_HIER)
	haveDir = TRUE;
    else
	haveDir = FALSE;

    graph = N_GNEW(nv, vtx_data);
    edges = N_GNEW(2 * ne + nv, int);	/* reserve space for self loops */
    if (haveLen || haveDir)
	ewgts = N_GNEW(2 * ne + nv, float);
    if (haveWt)
	eweights = N_GNEW(2 * ne + nv, float);
#ifdef DIGCOLA
    if (haveDir)
	edists = N_GNEW(2*ne+nv,float);
#endif

    i = 0;
    ne = 0;
    for (np = agfstnode(g); np; np = agnxtnode(g, np)) {
	int j = 1;		/* index of neighbors */
	clearPM(ps);
	assert(ND_id(np) == i);
	graph[i].np = np;
	graph[i].edges = edges++;	/* reserve space for the self loop */
	if (haveLen || haveDir)
	    graph[i].ewgts = ewgts++;
	else
	    graph[i].ewgts = NULL;
	if (haveWt)
	    graph[i].eweights = eweights++;
	else
	    graph[i].eweights = NULL;
#ifdef DIGCOLA
	if (haveDir) {
	    graph[i].edists = edists++;
	}
	else
	    graph[i].edists = NULL;
#endif
	i_nedges = 1;		/* one for the self */

	for (ep = agfstedge(g, np); ep; ep = agnxtedge(g, ep, np)) {
	    if (ep->head == ep->tail)
		continue;	/* ignore loops */
	    idx = checkEdge(ps, ep, j);
	    if (idx != j) {	/* seen before */
		if (haveWt)
		    graph[i].eweights[idx] += ED_factor(ep);
		if (haveLen) {
		    int curlen = graph[i].ewgts[idx];
		    graph[i].ewgts[idx] = MAX(ED_dist(ep), curlen);
		}
	    } else {
		node_t *vp = (((ep->tail) == np) ? ep->head : ep->tail);
		ne++;
		j++;

		*edges++ = ND_id(vp);
		if (haveWt)
		    *eweights++ = ED_factor(ep);
		if (haveLen)
		    *ewgts++ = ED_dist(ep);
		else if (haveDir)
		    *ewgts++ = 1.0;
#ifdef DIGCOLA
		if (haveDir) {
			*edists++ = (np == ep->head ? 1.0 : -1.0);
		}
#endif
		i_nedges++;
	    }
	}

	graph[i].nedges = i_nedges;
	graph[i].edges[0] = i;
#ifdef USE_STYLES
	graph[i].styles = NULL;
#endif
	i++;
    }
#ifdef DIGCOLA
    if (haveDir) {
    /* Make graph acyclic */
	acyclic (graph, nv);
    }
#endif

    ne /= 2;			/* every edge is counted twice */

    /* If necessary, release extra memory. */
    if (ne != agnedges(g)) {
	edges = RALLOC(2 * ne + nv, graph[0].edges, int);
	if (haveLen)
	    ewgts = RALLOC(2 * ne + nv, graph[0].ewgts, float);
	if (haveWt)
	    eweights = RALLOC(2 * ne + nv, graph[0].eweights, float);

	for (i = 0; i < nv; i++) {
	    int sz = graph[i].nedges;
	    graph[i].edges = edges;
	    edges += sz;
	    if (haveLen) {
		graph[i].ewgts = ewgts;
		ewgts += sz;
	    }
	    if (haveWt) {
		graph[i].eweights = eweights;
		eweights += sz;
	    }
	}
    }

    *nedges = ne;
    freePM(ps);
    return graph;
}

static void freeGraphData(vtx_data * graph)
{
    if (graph != NULL) {
	if (graph[0].edges != NULL)
	    free(graph[0].edges);
	if (graph[0].ewgts != NULL)
	    free(graph[0].ewgts);
#ifdef UNIMPLEMENTED
	if (graph[0].elens != NULL)
	    free(graph[0].elens);
#endif
#ifdef USE_STYLES
	if (graph[0].styles != NULL)
	    free(graph[0].styles);
#endif
	free(graph);
    }
}

static void initRegular(graph_t * G, int nG)
{
    int i;
    double a, da;
    node_t *np;

    a = 0.0;
    da = (2 * PI) / nG;
    for (i = 0; (np = GD_neato_nlist(G)[i]); i++) {
	ND_pos(np)[0] = nG * Spring_coeff * cos(a);
	ND_pos(np)[1] = nG * Spring_coeff * sin(a);
	ND_pinned(np) = P_SET;
	a = a + da;
	if (Ndim > 2)
	    jitter3d(np, nG);
    }
}

#define SLEN(s) (sizeof(s)-1)
#define SMART   "self"
#define REGULAR "regular"
#define RANDOM  "random"

/* checkStart:
 * Analyzes start attribute, setting seed.
 * If set,
 *   If start is regular, places nodes and returns INIT_REGULAR.
 *   If start is self, returns INIT_SELF.
 *   If start is random, returns INIT_RANDOM
 *   If number follows, use as seed.
 *   else set seed random
 * else return default
 * 
 */
int checkStart(graph_t * G, int nG, int dflt)
{
    char *p;
    unsigned int seed;
    char smallbuf[32];
    int init = dflt;

    seed = 1;
    p = agget(G, "start");
    if (p) {
	unsigned char uc = *(unsigned char *) p;
	if (isalpha(uc)) {
	    if (!strncmp(p, SMART, SLEN(SMART))) {
		if (N_pos)
		    agerr(AGWARN,
			  "node positions are ignored with start=%s\n",
			  SMART);
		init = INIT_SELF;
		p += SLEN(SMART);
	    } else if (!strncmp(p, REGULAR, SLEN(REGULAR))) {
		if (N_pos)
		    agerr(AGWARN,
			  "node positions are ignored with start=%s\n",
			  REGULAR);
		initRegular(G, nG);
		return INIT_REGULAR;
	    } else if (!strncmp(p, RANDOM, SLEN(RANDOM))) {
		init = INIT_RANDOM;
		p += SLEN(RANDOM);
	    }
	}

	/* Check for seed value */
	if (sscanf(p, "%d", &seed) < 1) {
#ifdef MSWIN32
	    seed = (unsigned) time(NULL);
#else
	    seed = (unsigned) getpid() ^ (unsigned) time(NULL);
#endif
	    sprintf(smallbuf, "%u", seed);
	    agset(G, "start", smallbuf);
	}
    }
    srand48(seed);
    return init;
}

#ifdef UNUSED
void dumpData(graph_t * g, vtx_data * gp, int nv, int ne)
{
    node_t *v;
    int i, j, n;

    fprintf(stderr, "n %d e %d\n", nv, ne);
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	fprintf(stderr, "\"%s\" %d\n", v->name, ND_id(v));
    }
    for (i = 0; i < nv; i++) {
	n = gp[i].nedges;
	fprintf(stderr, "[%d] %d\n", i, n);
	for (j = 0; j < n; j++) {
	    fprintf(stderr, "  %3d", gp[i].edges[j]);
	}
	fputs("\n", stderr);
	if (gp[i].ewgts)
	    for (j = 0; j < n; j++) {
		fprintf(stderr, "  %3f", gp[i].ewgts[j]);
	    }
	fputs("\n", stderr);

    }
}
#endif

/* majorization:
 * Solve stress using majorization.
 * Old neato attributes to incorporate:
 *  weight?
 */
static void
majorization(graph_t * g, int nv, int mode, int model, int dim, int steps)
{
    double **coords;
    int ne;
    int i;
    node_t *v;
    vtx_data *gp;
    int init;

    init = checkStart(g, nv, (mode == MODE_HIER ? INIT_SELF : INIT_RANDOM));
    coords = N_GNEW(dim, double *);
    coords[0] = N_GNEW(nv * dim, double);
    for (i = 1; i < Ndim; i++) {
	coords[i] = coords[0] + i * nv;
    }
    if (Verbose) {
	fprintf(stderr, "model %d smart_init %d iterations %d tol %f\n",
		model, (init == INIT_SELF), MaxIter, Epsilon);
	fprintf(stderr, "convert graph: ");
	start_timer();
    }
    gp = makeGraphData(g, nv, &ne, mode, model);
    if (Verbose) {
	fprintf(stderr, "%d nodes %.2f sec\n", nv, elapsed_sec());
    }

#ifdef DIGCOLA
    if (mode == MODE_HIER) {
	double lgap = late_double(g, agfindattr(g, "levelsgap"), 0.0, -MAXDOUBLE);
	stress_majorization_with_hierarchy(gp, nv, ne, coords, Ndim,
				   (init == INIT_SELF), model, MaxIter, lgap);
    }
    else
#endif
	stress_majorization_kD_mkernel(gp, nv, ne, coords, Ndim,
				   (init == INIT_SELF), model, MaxIter);

    /* store positions back in nodes */
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	int idx = ND_id(v);
	int i;
	for (i = 0; i < Ndim; i++) {
	    ND_pos(v)[i] = coords[i][idx];
	}
    }
    freeGraphData(gp);
    free(coords[0]);
    free(coords);
}

static void subset_model(Agraph_t * G, int nG)
{
    int i, j, ne;
    DistType **Dij;
    vtx_data *gp;

    gp = makeGraphData(G, nG, &ne, MODE_KK, MODEL_SUBSET);
    Dij = compute_apsp_artifical_weights(gp, nG);
    for (i = 0; i < nG; i++) {
	for (j = 0; j < nG; j++) {
	    GD_dist(G)[i][j] = Dij[i][j];
	}
    }
    free(Dij[0]);
    free(Dij);
    freeGraphData(gp);
}

/* kkNeato:
 * Solve using gradient descent a la Kamada-Kawai.
 */
static void kkNeato(Agraph_t * g, int nG, int model)
{
    if (model == MODEL_SUBSET) {
	subset_model(g, nG);
    } else if (model == MODEL_CIRCUIT) {
	if (!circuit_model(g, nG)) {
	    agerr(AGWARN,
		  "graph %s is disconnected. Hence, the circuit model\n",
		  g->name);
	    agerr(AGPREV,
		  "is undefined. Reverting to the shortest path model.\n");
	    agerr(AGPREV,
		  "Alternatively, consider running neato using -Gpack=true or decomposing\n");
	    agerr(AGPREV, "the graph into connected components.\n");
	    shortest_path(g, nG);
	}
    } else
	shortest_path(g, nG);
    initial_positions(g, nG);
    diffeq_model(g, nG);
    if (Verbose) {
	fprintf(stderr, "Solving model %d iterations %d tol %f\n",
		model, MaxIter, Epsilon);
	start_timer();
    }
    solve_model(g, nG);
}

/* neatoLayout:
 * Use stress optimization to layout a single component
 */
void neatoLayout(Agraph_t * g, int layoutMode, int layoutModel)
{
    int nG;
    char *str;

    if ((str = agget(g, "maxiter")))
	MaxIter = atoi(str);
    else if (layoutMode == MODE_MAJOR)
	MaxIter = DFLT_ITERATIONS;
    else
	MaxIter = 100 * agnnodes(g);

    nG = scan_graph_mode(g, layoutMode);
    if (!nG)
	return;
    if (layoutMode)
	majorization(g, nG, layoutMode, layoutModel, Ndim, MaxIter);
    else
	kkNeato(g, nG, layoutModel);
}

/* addZ;
 * If dimension == 3 and z attribute is declared, 
 * attach z value to nodes if not defined.
 */
static void
addZ (Agraph_t* g)
{
    node_t* n;
    char    buf[BUFSIZ];

    if ((Ndim >= 3) && N_z) { 
	for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	    sprintf(buf, "%d", POINTS(ND_pos(n)[2]));
	    agxset(n, N_z->index, buf);
	}
    }
}

/* neato_layout:
 */
void neato_layout(Agraph_t * g)
{

    neato_init_graph(g);
    if (Nop) {
	addZ (g);
	if (init_nop(g, 1)) {
	    agerr(AGPREV, "as required by the -n flag\n");
	    exit(1);
	}
    } else {
	int layoutMode = neatoMode(g);
	int model = neatoModel(g);
	pack_mode mode = getPackMode(g, l_undef);
	Pack = getPack(g, -1, CL_OFFSET);
	/* pack if just packmode defined. */
	if (mode == l_undef) {
	    /* If the user has not indicated packing but we are
	     * using the new neato, turn packing on.
	     */
	    if ((Pack < 0) && layoutMode)
		Pack = CL_OFFSET;
	    mode = l_node;
	} else if (Pack < 0)
	    Pack = CL_OFFSET;
	if (Pack >= 0) {
	    graph_t *gc;
	    graph_t **cc;
	    int n_cc;
	    int i;
	    pack_info pinfo;
	    bool pin;

	    cc = pccomps(g, &n_cc, cc_pfx, &pin);

	    for (i = 0; i < n_cc; i++) {
		gc = cc[i];
		nodeInduce(gc);
		neatoLayout(gc, layoutMode, model);
		adjustNodes(gc);
	    }
	    if (n_cc > 1) {
		bool *bp;
		if (pin) {
		    bp = N_NEW(n_cc, bool);
		    bp[0] = TRUE;
		} else
		    bp = 0;
		pinfo.margin = Pack;
		pinfo.doSplines = 0;
		pinfo.mode = mode;
		pinfo.fixed = bp;
		packGraphs(n_cc, cc, 0, &pinfo);
		if (bp)
		    free(bp);
	    }
	    compute_bb(g);
	    addZ (g);
	    spline_edges(g);

	    /* cleanup and remove component subgraphs */
	    for (i = 0; i < n_cc; i++) {
		gc = cc[i];
		free_scan_graph(gc);
		agdelete(g, gc);
	    }
	} else {
	    neatoLayout(g, layoutMode, model);
	    adjustNodes(g);
	    addZ (g);
	    spline_edges(g);
	}
    }
    dotneato_postprocess(g);
}
