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


/*  vladimir@cs.ualberta.ca,  9-Dec-1997
 *	merge edges with specified samehead/sametail onto the same port
 */

#include	"dot.h"


#define MAXSAME 5		/* max no of same{head,tail} groups on a node */

typedef struct same_t {
    char *id;			/* group id */
    elist l;			/* edges in the group */
    int n_arr;			/* number of edges with arrows */
    double arr_len;		/* arrow length of an edge in the group */
} same_t;
static int n_same;		/* number of same_t groups on current node */

static void sameedge(same_t * same, node_t * n, edge_t * e, char *id);
static void sameport(node_t * u, elist * l, double arr_len);

void dot_sameports(graph_t * g)
/* merge edge ports in G */
{
    node_t *n;
    edge_t *e;
    char *id;
    same_t same[MAXSAME];
    int i;

    E_samehead = agfindattr(g->proto->e, "samehead");
    E_sametail = agfindattr(g->proto->e, "sametail");
    if (!(E_samehead || E_sametail))
	return;
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	n_same = 0;
	for (e = agfstedge(g, n); e; e = agnxtedge(g, e, n)) {
	    if (e->head == n && E_samehead &&
		(id = agxget(e, E_samehead->index))[0])
		sameedge(same, n, e, id);
	    else if (e->tail == n && E_sametail &&
		     (id = agxget(e, E_sametail->index))[0])
		sameedge(same, n, e, id);
	}
	for (i = 0; i < n_same; i++) {
	    if (same[i].l.size > 1)
		sameport(n, &same[i].l, same[i].arr_len);
	    free_list(same[i].l);
	    /* I sure hope I don't need to free the char* id */
	}
    }
}

static void sameedge(same_t * same, node_t * n, edge_t * e, char *id)
/* register E in the SAME structure of N under ID. Uses static int N_SAME */
{
    int i, sflag, eflag, flag;

    for (i = 0; i < n_same; i++)
	if (streq(same[i].id, id)) {
	    elist_append(e, same[i].l);
	    goto set_arrow;
	}
    if (++n_same > MAXSAME) {
	agerr(AGERR, "too many same{head,tail} groups for node %s\n",
	      n->name);
	return;
    }
    alloc_elist(1, same[i].l);
    elist_fastapp(e, same[i].l);
    same[i].id = id;
    same[i].n_arr = 0;
    same[i].arr_len = 0;
  set_arrow:
    arrow_flags(e, &sflag, &eflag);
    if ((flag = e->head == n ? eflag : sflag))
	same[i].arr_len =
	    /* only consider arrows if there's exactly one arrow */
	    (++same[i].n_arr == 1) ? arrow_length(e, flag) : 0;
}

static void sameport(node_t * u, elist * l, double arr_len)
/* make all edges in L share the same port on U. The port is placed on the
   node boundary and the average angle between the edges. FIXME: this assumes
   naively that the edges are straight lines, which is wrong if they are long.
   In that case something like concentration could be done.

   An arr_port is also computed that's ARR_LEN away from the node boundary.
   It's used for edges that don't themselves have an arrow.
*/
{
    node_t *v;
    edge_t *e, *f;
    int i;
    double x = 0, y = 0, x1, y1, x2, y2, r;
    port prt;
    int sflag, eflag;
#ifdef OLD
    int ht;
    port arr_prt;
#endif

    /* Compute the direction vector (x,y) of the average direction. We compute
       with direction vectors instead of angles because else we have to first
       bring the angles within PI of each other. av(a,b)!=av(a,b+2*PI) */
    for (i = 0; i < l->size; i++) {
	e = l->list[i];
	if (e->head == u)
	    v = e->tail;
	else
	    v = e->head;
	x1 = ND_coord_i(v).x - ND_coord_i(u).x;
	y1 = ND_coord_i(v).y - ND_coord_i(u).y;
	r = hypot(x1, y1);
	x += x1 / r;
	y += y1 / r;
    }
    r = hypot(x, y);
    x /= r;
    y /= r;

    /* (x1,y1),(x2,y2) is a segment that must cross the node boundary */
    x1 = ND_coord_i(u).x;
    y1 = ND_coord_i(u).y;	/* center of node */
    r = MAX(ND_lw_i(u) + ND_rw_i(u), ND_ht_i(u) + GD_ranksep(u->graph));	/* far away */
    x2 = x * r + ND_coord_i(u).x;
    y2 = y * r + ND_coord_i(u).y;
    {				/* now move (x1,y1) to the node boundary */
	point curve[4];		/* bezier control points for a straight line */
	curve[0].x = ROUND(x1);
	curve[0].y = ROUND(y1);
	curve[1].x = ROUND((2 * x1 + x2) / 3);
	curve[1].y = ROUND((2 * y1 + y2) / 3);
	curve[2].x = ROUND((2 * x2 + x1) / 3);
	curve[2].y = ROUND((2 * y2 + y1) / 3);
	curve[3].x = ROUND(x2);
	curve[3].y = ROUND(y2);

	shape_clip(u, curve, 0);
	x1 = curve[0].x - ND_coord_i(u).x;
	y1 = curve[0].y - ND_coord_i(u).y;
    }

    /* compute PORT on the boundary */
    prt.p.x = ROUND(x1);
    prt.p.y = ROUND(y1);
    prt.bp = 0;
    prt.order =
	(MC_SCALE * (ND_lw_i(u) + prt.p.x)) / (ND_lw_i(u) + ND_rw_i(u));
    prt.constrained = FALSE;
    prt.defined = TRUE;
    prt.clip = 0;
    prt.theta = 0;
    prt.side = 0;

#ifdef OLD
This code appears obsolete and wrong. First, we don't use arr_prt
anymore, as we have previously ifdef'ed out the code below where it
is used. In addition, it resets the rank height. But we've already
positioned the nodes, so it can cause the new ht2, when added to a
node's y coordinate to give a value in the middle of the rank above.
This causes havoc when constructing box for spline routing.
See bug 419.
If we really want to make room for arrowheads, this should be done in
the general case that the user sets a small ranksep, and requires repositioning
nodes and maintaining equal separation when specified
    /* compute ARR_PORT at a distance ARR_LEN away from the boundary */
    if ((arr_prt.defined = arr_len && TRUE)) {
	arr_prt.p.x = ROUND(x1 + x * arr_len);
	arr_prt.p.y = ROUND(y1 + y * arr_len);
	arr_prt.bp = 0;
	arr_prt.side = 0;
	arr_prt.constrained = FALSE;
	arr_prt.order =
	    (MC_SCALE * (ND_lw_i(u) + arr_prt.p.x)) / (ND_lw_i(u) +
						       ND_rw_i(u));
	/* adjust ht so that splines.c uses feasible boxes.
	   FIXME: I guess this adds an extra box for all edges in the rank */
	if (u == l->list[0]->head) {
	    if (GD_rank(u->graph)[ND_rank(u)].ht2 <
		(ht = ABS(arr_prt.p.y)))
		GD_rank(u->graph)[ND_rank(u)].ht2 = ht;
	} else {
	    if (GD_rank(u->graph)[ND_rank(u)].ht1 <
		(ht = ABS(arr_prt.p.y)))
		GD_rank(u->graph)[ND_rank(u)].ht1 = ht;
	}
    }
#endif

    /* assign one of the ports to every edge */
    for (i = 0; i < l->size; i++) {
	e = l->list[i];
	arrow_flags(e, &sflag, &eflag);
#ifndef OLD
	for (; e; e = ED_to_virt(e)) {	/* assign to all virt edges of e */
	    for (f = e; f;
		 f = ED_edge_type(f) == VIRTUAL &&
		 ND_node_type(f->head) == VIRTUAL &&
		 ND_out(f->head).size == 1 ?
		 ND_out(f->head).list[0] : NULL) {
		if (f->head == u)
		    ED_head_port(f) = prt;
		if (f->tail == u)
		    ED_tail_port(f) = prt;
	    }
	    for (f = e; f;
		 f = ED_edge_type(f) == VIRTUAL &&
		 ND_node_type(f->tail) == VIRTUAL &&
		 ND_in(f->tail).size == 1 ?
		 ND_in(f->tail).list[0] : NULL) {
		if (f->head == u)
		    ED_head_port(f) = prt;
		if (f->tail == u)
		    ED_tail_port(f) = prt;
	    }
	}
#else
	for (; e; e = ED_to_virt(e)) {	/* assign to all virt edges of e */
	    if (e->head == u)
		ED_head_port(e) =
		    arr_port.defined && !eflag ? arr_prt : prt;
	    if (e->tail == u)
		ED_tail_port(e) =
		    arr_port.defined && !sflag ? arr_prt : prt;
	}
#endif
    }

    ND_has_port(u) = TRUE;	/* kinda pointless, because mincross is already done */
}
