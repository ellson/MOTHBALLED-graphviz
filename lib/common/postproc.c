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


#include "render.h"


static int Rankdir;
static boolean Flip;
static pointf Offset;

static void place_flip_graph_label(graph_t * g);

#define M1 \
"/pathbox {\n\
    /Y exch %.3g sub def\n\
    /X exch %.3g sub def\n\
    /y exch %.3g sub def\n\
    /x exch %.3g sub def\n\
    newpath x y moveto\n\
    X y lineto\n\
    X Y lineto\n\
    x Y lineto\n\
    closepath stroke\n \
} def\n\
/dbgstart { gsave %.3g %.3g translate } def\n\
/arrowlength 10 def\n\
/arrowwidth arrowlength 2 div def\n\
/arrowhead {\n\
    gsave\n\
    rotate\n\
    currentpoint\n\
    newpath\n\
    moveto\n\
    arrowlength arrowwidth 2 div rlineto\n\
    0 arrowwidth neg rlineto\n\
    closepath fill\n\
    grestore\n\
} bind def\n\
/makearrow {\n\
    currentpoint exch pop sub exch currentpoint pop sub atan\n\
    arrowhead\n\
} bind def\n\
/point {\
    newpath\
    2 0 360 arc fill\
} def\
/makevec {\n\
    /Y exch def\n\
    /X exch def\n\
    /y exch def\n\
    /x exch def\n\
    newpath x y moveto\n\
    X Y lineto stroke\n\
    X Y moveto\n\
    x y makearrow\n\
} def\n"

#define M2 \
"/pathbox {\n\
    /X exch neg %.3g sub def\n\
    /Y exch %.3g sub def\n\
    /x exch neg %.3g sub def\n\
    /y exch %.3g sub def\n\
    newpath x y moveto\n\
    X y lineto\n\
    X Y lineto\n\
    x Y lineto\n\
    closepath stroke\n\
} def\n"

static pointf map_point(pointf p)
{
    p = ccwrotatepf(p, Rankdir*90);
    p.x -= Offset.x;
    p.y -= Offset.y;
    return p;
}

static void map_edge(edge_t * e)
{
    int j, k;
    bezier bz;

    if (ED_spl(e) == NULL) {
	if ((Concentrate == FALSE) || (ED_edge_type(e) != IGNORED))
	    agerr(AGERR, "lost %s %s edge\n", e->tail->name,
		  e->head->name);
	return;
    }
    for (j = 0; j < ED_spl(e)->size; j++) {
	bz = ED_spl(e)->list[j];
	for (k = 0; k < bz.size; k++)
	    bz.list[k] = map_point(bz.list[k]);
	if (bz.sflag)
	    ED_spl(e)->list[j].sp = map_point(ED_spl(e)->list[j].sp);
	if (bz.eflag)
	    ED_spl(e)->list[j].ep = map_point(ED_spl(e)->list[j].ep);
    }
    if (ED_label(e))
	ED_label(e)->pos = map_point(ED_label(e)->pos);
    /* vladimir */
    if (ED_head_label(e))
	ED_head_label(e)->pos = map_point(ED_head_label(e)->pos);
    if (ED_tail_label(e))
	ED_tail_label(e)->pos = map_point(ED_tail_label(e)->pos);
}

void translate_bb(graph_t * g, int rankdir)
{
    int c;
    boxf bb, new_bb;

    bb = GD_bb(g);
    if (rankdir == RANKDIR_LR || rankdir == RANKDIR_BT) {
	new_bb.LL = map_point(pointfof(bb.LL.x, bb.UR.y));
	new_bb.UR = map_point(pointfof(bb.UR.x, bb.LL.y));
    } else {
	new_bb.LL = map_point(pointfof(bb.LL.x, bb.LL.y));
	new_bb.UR = map_point(pointfof(bb.UR.x, bb.UR.y));
    }
    GD_bb(g) = new_bb;
    if (GD_label(g)) {
	GD_label(g)->pos = map_point(GD_label(g)->pos);
    }
    for (c = 1; c <= GD_n_cluster(g); c++)
	translate_bb(GD_clust(g)[c], rankdir);
}

void dot_nodesize(node_t * n, boolean flip)
{
    double x, y;
    int ps;

    if (flip == FALSE) {
	x = ND_width(n);
	y = ND_height(n);
    } else {
	y = ND_width(n);
	x = ND_height(n);
    }
    ps = POINTS(x) / 2;
    if (ps < 1) ps = 1;
    ND_lw(n) = ND_rw(n) = ps;

    if (x == y)
	ND_ht(n) = 2*ps;
    else
	ND_ht(n) = POINTS(y);
}

/* translate_drawing:
 * Translate and/or rotate nodes, spline points, and bbox info if
 * Offset is non-trivial.
 * Also, if Rankdir, reset ND_lw, ND_rw, and ND_ht to correct value.
 */
static void translate_drawing(graph_t * g)
{
    node_t *v;
    edge_t *e;
    int shift = (Offset.x || Offset.y);

    if (!shift && !Rankdir) return;
    for (v = agfstnode(g); v; v = agnxtnode(g, v)) {
	if (Rankdir) dot_nodesize(v, FALSE);
	if (shift) {
	    ND_coord(v) = map_point(ND_coord(v));
	    if (State == GVSPLINES)
		for (e = agfstout(g, v); e; e = agnxtout(g, e))
		    map_edge(e);
	}
    }
    if (shift)
	translate_bb(g, GD_rankdir(g));
}

/* place_root_label:
 * Set position of root graph label.
 * Note that at this point, after translate_drawing, a
 * flipped drawing has been transposed, so we don't have
 * to worry about switching x and y.
 */
static void place_root_label(graph_t * g, pointf d)
{
    pointf p;

    if (GD_label_pos(g) & LABEL_AT_RIGHT) {
	p.x = GD_bb(g).UR.x - d.x / 2;
    } else if (GD_label_pos(g) & LABEL_AT_LEFT) {
	p.x = GD_bb(g).LL.x + d.x / 2;
    } else {
	p.x = (GD_bb(g).LL.x + GD_bb(g).UR.x) / 2;
    }

    if (GD_label_pos(g) & LABEL_AT_TOP) {
	p.y = GD_bb(g).UR.y - d.y / 2;
    } else {
	p.y = GD_bb(g).LL.y + d.y / 2;
    }

    GD_label(g)->pos = p;
    GD_label(g)->set = TRUE;
}

/* dotneato_postprocess:
 * Set graph and cluster label positions.
 * Add space for root graph label and translate graph accordingly.
 * Set final nodesize using ns.
 * Assumes the boxes of all clusters have been computed.
 * When done, the bounding box of g has LL at origin.
 */
void dotneato_postprocess(Agraph_t * g)
{
    double diff;
    pointf dimen = {0., 0.};

    Rankdir = GD_rankdir(g);
    Flip = GD_flip(g);
    if (Flip)
	place_flip_graph_label(g);
    else
	place_graph_label(g);

    if (GD_label(g) && !GD_label(g)->set) {
	dimen = GD_label(g)->dimen;
	PAD(dimen);
	if (Flip) {
	    if (GD_label_pos(g) & LABEL_AT_TOP) {
		GD_bb(g).UR.x += dimen.y;
	    } else {
		GD_bb(g).LL.x -= dimen.y;
	    }

	    if (dimen.x > (GD_bb(g).UR.y - GD_bb(g).LL.y)) {
		diff = dimen.x - (GD_bb(g).UR.y - GD_bb(g).LL.y);
		diff = diff / 2.;
		GD_bb(g).LL.y -= diff;
		GD_bb(g).UR.y += diff;
	    }
	} else {
	    if (GD_label_pos(g) & LABEL_AT_TOP) {
		if (Rankdir == RANKDIR_TB)
		    GD_bb(g).UR.y += dimen.y;
		else
		    GD_bb(g).LL.y -= dimen.y;
	    } else {
		if (Rankdir == RANKDIR_TB)
		    GD_bb(g).LL.y -= dimen.y;
		else
		    GD_bb(g).UR.y += dimen.y;
	    }

	    if (dimen.x > (GD_bb(g).UR.x - GD_bb(g).LL.x)) {
		diff = dimen.x - (GD_bb(g).UR.x - GD_bb(g).LL.x);
		diff = diff / 2.;
		GD_bb(g).LL.x -= diff;
		GD_bb(g).UR.x += diff;
	    }
	}
    }
    switch (Rankdir) {
    case RANKDIR_TB:
	Offset = GD_bb(g).LL;
	break;
    case RANKDIR_LR:
	Offset = pointfof(-GD_bb(g).UR.y, GD_bb(g).LL.x);
	break;
    case RANKDIR_BT:
	Offset = pointfof(GD_bb(g).LL.x, -GD_bb(g).UR.y);
	break;
    case RANKDIR_RL:
	Offset = pointfof(GD_bb(g).LL.y, GD_bb(g).LL.x);
	break;
    }
    translate_drawing(g);
    if (GD_label(g) && !GD_label(g)->set)
	place_root_label(g, dimen);

    if (Show_boxes) {
	char buf[BUFSIZ];
	if (Flip)
	    sprintf(buf, M2, Offset.x, Offset.y, Offset.x, Offset.y);
	else
	    sprintf(buf, M1, Offset.y, Offset.x, Offset.y, Offset.x, 
                 -Offset.x, -Offset.y);
	Show_boxes[0] = strdup(buf);
    }
}

/* place_flip_graph_label:
 * Put cluster labels recursively in the flip case.
 */
static void place_flip_graph_label(graph_t * g)
{
    int c;
    pointf p, d;

    if ((g != g->root) && (GD_label(g)) && !GD_label(g)->set) {

	if (GD_label_pos(g) & LABEL_AT_TOP) {
	    d = GD_border(g)[RIGHT_IX];
	    p.x = GD_bb(g).UR.x - d.x / 2;
	} else {
	    d = GD_border(g)[LEFT_IX];
	    p.x = GD_bb(g).LL.x + d.x / 2;
	}

	if (GD_label_pos(g) & LABEL_AT_RIGHT) {
	    p.y = GD_bb(g).LL.y + d.y / 2;
	} else if (GD_label_pos(g) & LABEL_AT_LEFT) {
	    p.y = GD_bb(g).UR.y - d.y / 2;
	} else {
	    p.y = (GD_bb(g).LL.y + GD_bb(g).UR.y) / 2;
	}
	GD_label(g)->pos = p;
	GD_label(g)->set = TRUE;
    }

    for (c = 1; c <= GD_n_cluster(g); c++)
	place_flip_graph_label(GD_clust(g)[c]);
}

/* place_graph_label:
 * Put cluster labels recursively in the non-flip case.
 * The adjustments to the bounding boxes should no longer
 * be necessary, since we now guarantee the label fits in
 * the cluster.
 */
void place_graph_label(graph_t * g)
{
    int c;
    pointf p, d;

    if ((g != g->root) && (GD_label(g)) && !GD_label(g)->set) {
	if (GD_label_pos(g) & LABEL_AT_TOP) {
	    d = GD_border(g)[TOP_IX];
	    p.y = GD_bb(g).UR.y - d.y / 2;
	} else {
	    d = GD_border(g)[BOTTOM_IX];
	    p.y = GD_bb(g).LL.y + d.y / 2;
	}

	if (GD_label_pos(g) & LABEL_AT_RIGHT) {
	    p.x = GD_bb(g).UR.x - d.x / 2;
	} else if (GD_label_pos(g) & LABEL_AT_LEFT) {
	    p.x = GD_bb(g).LL.x + d.x / 2;
	} else {
	    p.x = (GD_bb(g).LL.x + GD_bb(g).UR.x) / 2;
	}
	GD_label(g)->pos = p;
	GD_label(g)->set = TRUE;
    }

    for (c = 1; c <= GD_n_cluster(g); c++)
	place_graph_label(GD_clust(g)[c]);
}
