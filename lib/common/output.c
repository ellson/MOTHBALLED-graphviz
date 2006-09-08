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
#include "agxbuf.h"

static int e_arrows;		/* graph has edges with end arrows */
static int s_arrows;		/* graph has edges with start arrows */
static attrsym_t *g_draw;
static attrsym_t *g_l_draw;

static void printptf(FILE * f, point pt)
{
    fprintf(f, " %.3f %.3f", PS2INCH(pt.x), PS2INCH(YDIR(pt.y)));
}

/* setYInvert:
 * Set parameters used to flip coordinate system (y=0 at top).
 * Values do not need to be unset, since if Y_invert is set, it's
 * set for * all graphs during current run, so each will 
 * reinitialize the values for its bbox.
 */
static void setYInvert(graph_t * g)
{
    if (Y_invert) {
	Y_off = GD_bb(g).UR.y + GD_bb(g).LL.y;
	YF_off = PS2INCH(Y_off);
    }
}

static void writenodeandport(FILE * fp, node_t * node, char *port)
{
    char *name;
    if (IS_CLUST_NODE(node))
	name = strchr(node->name, ':') + 1;
    else
	name = node->name;
    fprintf(fp, "%s", agcanonical(name));	/* slimey i know */
    if (port && *port)
	fprintf(fp, ":%s", agcanonical(port));
}

/* FIXME - there must be a proper way to get port info - these are 
 * supposed to be private to libgraph - from libgraph.h */
#define TAILX 1
#define HEADX 2

/* _write_plain:
 */
void write_plain(GVJ_t * job, graph_t * g, FILE * f, bool extend)
{
    int i, j, splinePoints;
    char *tport, *hport;
    node_t *n;
    edge_t *e;
    bezier bz;
    point pt;
    char *lbl;

//    setup_graph(job, g);
    setYInvert(g);
    pt = GD_bb(g).UR;
    fprintf(f, "graph %.3f %.3f %.3f\n", job->zoom, PS2INCH(pt.x), PS2INCH(pt.y));
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (IS_CLUST_NODE(n))
	    continue;
	fprintf(f, "node %s ", agcanonical(n->name));
	printptf(f, ND_coord_i(n));
	if (ND_label(n)->html)   /* if html, get original text */
	    lbl = agxget(n, N_label->index);
	else
	    lbl = ND_label(n)->text;
	if (lbl)
	    lbl = agcanonical(lbl);
	else
	    lbl = "\"\"";
	fprintf(f, " %.3f %.3f %s %s %s %s %s\n",
		ND_width(n), ND_height(n), lbl,
		late_nnstring(n, N_style, "solid"),
		ND_shape(n)->name,
		late_nnstring(n, N_color, DEFAULT_COLOR),
		late_nnstring(n, N_fillcolor, DEFAULT_FILL));
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    if (extend && e->attr) {
		tport = e->attr[TAILX];
		hport = e->attr[HEADX];
	    } else
		tport = hport = "";
	    if (ED_spl(e)) {
		splinePoints = 0;
		for (i = 0; i < ED_spl(e)->size; i++) {
		    bz = ED_spl(e)->list[i];
		    splinePoints += bz.size;
		}
		fprintf(f, "edge ");
		writenodeandport(f, e->tail, tport);
		fprintf(f, " ");
		writenodeandport(f, e->head, hport);
		fprintf(f, " %d", splinePoints);
		for (i = 0; i < ED_spl(e)->size; i++) {
		    bz = ED_spl(e)->list[i];
		    for (j = 0; j < bz.size; j++)
			printptf(f, bz.list[j]);
		}
	    }
	    if (ED_label(e)) {
		fprintf(f, " %s", agcanonical(ED_label(e)->text));
		printptf(f, ED_label(e)->p);
	    }
	    fprintf(f, " %s %s\n", late_nnstring(e, E_style, "solid"),
		    late_nnstring(e, E_color, DEFAULT_COLOR));
	}
    }
    fprintf(f, "stop\n");
}

static void set_record_rects(node_t * n, field_t * f, agxbuf * xb)
{
    int i;
    char buf[BUFSIZ];

    if (f->n_flds == 0) {
	sprintf(buf, "%d,%d,%d,%d ",
		f->b.LL.x + ND_coord_i(n).x,
		YDIR(f->b.LL.y + ND_coord_i(n).y),
		f->b.UR.x + ND_coord_i(n).x,
		YDIR(f->b.UR.y + ND_coord_i(n).y));
	agxbput(xb, buf);
    }
    for (i = 0; i < f->n_flds; i++)
	set_record_rects(n, f->fld[i], xb);
}

static void rec_attach_bb(graph_t * g)
{
    int c;
    char buf[32];
    point pt;

    sprintf(buf, "%d,%d,%d,%d", GD_bb(g).LL.x, GD_bb(g).LL.y,
	    GD_bb(g).UR.x, GD_bb(g).UR.y);
    agset(g, "bb", buf);
    if (GD_label(g) && GD_label(g)->text[0]) {
	pt = GD_label(g)->p;
	sprintf(buf, "%d,%d", pt.x, YDIR(pt.y));
	agset(g, "lp", buf);
    }
    for (c = 1; c <= GD_n_cluster(g); c++)
	rec_attach_bb(GD_clust(g)[c]);
}

void attach_attrs(graph_t * g)
{
    int i, j, sides;
    char buf[BUFSIZ];		/* Used only for small strings */
    unsigned char xbuffer[BUFSIZ];	/* Initial buffer for xb */
    agxbuf xb;
    node_t *n;
    edge_t *e;
    point pt;

    e_arrows = s_arrows = 0;
    setYInvert(g);
    agxbinit(&xb, BUFSIZ, xbuffer);
    safe_dcl(g, g->proto->n, "pos", "", agnodeattr);
    safe_dcl(g, g->proto->n, "rects", "", agnodeattr);
    N_width = safe_dcl(g, g->proto->n, "width", "", agnodeattr);
    N_height = safe_dcl(g, g->proto->n, "height", "", agnodeattr);
    safe_dcl(g, g->proto->e, "pos", "", agedgeattr);
    if (GD_has_labels(g) & EDGE_LABEL)
	safe_dcl(g, g->proto->e, "lp", "", agedgeattr);
    if (GD_has_labels(g) & HEAD_LABEL)
	safe_dcl(g, g->proto->e, "head_lp", "", agedgeattr);
    if (GD_has_labels(g) & TAIL_LABEL)
	safe_dcl(g, g->proto->e, "tail_lp", "", agedgeattr);
    if (GD_label(g)) {
	safe_dcl(g, g, "lp", "", agraphattr);
	if (GD_label(g)->text[0]) {
	    pt = GD_label(g)->p;
	    sprintf(buf, "%d,%d", pt.x, YDIR(pt.y));
	    agset(g, "lp", buf);
	}
    }
    safe_dcl(g, g, "bb", "", agraphattr);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	sprintf(buf, "%d,%d", ND_coord_i(n).x, YDIR(ND_coord_i(n).y));
	agset(n, "pos", buf);
	sprintf(buf, "%.2f", PS2INCH(ND_ht_i(n)));
	agxset(n, N_height->index, buf);
	sprintf(buf, "%.2f", PS2INCH(ND_lw_i(n) + ND_rw_i(n)));
	agxset(n, N_width->index, buf);
	if (strcmp(ND_shape(n)->name, "record") == 0) {
	    set_record_rects(n, ND_shape_info(n), &xb);
	    agxbpop(&xb);	/* get rid of last space */
	    agset(n, "rects", agxbuse(&xb));
	} else {
	    polygon_t *poly;
	    int i;
	    if (N_vertices && isPolygon(n)) {
		poly = (polygon_t *) ND_shape_info(n);
		sides = poly->sides;
		if (sides < 3) {
		    char *p = agget(n, "samplepoints");
		    if (p)
			sides = atoi(p);
		    else
			sides = 8;
		    if (sides < 3)
			sides = 8;
		}
		for (i = 0; i < sides; i++) {
		    if (i > 0)
			agxbputc(&xb, ' ');
		    if (poly->sides >= 3)
			sprintf(buf, "%.3f %.3f",
				PS2INCH(poly->vertices[i].x),
				YFDIR(PS2INCH(poly->vertices[i].y)));
		    else
			sprintf(buf, "%.3f %.3f",
				ND_width(n) / 2.0 * cos(i /
							(double) sides *
							PI * 2.0),
				YFDIR(ND_height(n) / 2.0 *
				   sin(i / (double) sides * PI * 2.0)));
		    agxbput(&xb, buf);
		}
		agxset(n, N_vertices->index, agxbuse(&xb));
	    }
	}
	if (State >= GVSPLINES) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		if (ED_edge_type(e) == IGNORED)
		    continue;
		if (ED_spl(e) == NULL)
		    continue;	/* reported in postproc */
		for (i = 0; i < ED_spl(e)->size; i++) {
		    if (i > 0)
			agxbputc(&xb, ';');
		    if (ED_spl(e)->list[i].sflag) {
			s_arrows = 1;
			sprintf(buf, "s,%d,%d ",
				ED_spl(e)->list[i].sp.x,
				YDIR(ED_spl(e)->list[i].sp.y));
			agxbput(&xb, buf);
		    }
		    if (ED_spl(e)->list[i].eflag) {
			e_arrows = 1;
			sprintf(buf, "e,%d,%d ",
				ED_spl(e)->list[i].ep.x,
				YDIR(ED_spl(e)->list[i].ep.y));
			agxbput(&xb, buf);
		    }
		    for (j = 0; j < ED_spl(e)->list[i].size; j++) {
			if (j > 0)
			    agxbputc(&xb, ' ');
			pt = ED_spl(e)->list[i].list[j];
			sprintf(buf, "%d,%d", pt.x, YDIR(pt.y));
			agxbput(&xb, buf);
		    }
		}
		agset(e, "pos", agxbuse(&xb));
		if (ED_label(e)) {
		    pt = ED_label(e)->p;
		    sprintf(buf, "%d,%d", pt.x, YDIR(pt.y));
		    agset(e, "lp", buf);
		}
		if (ED_head_label(e)) {
		    pt = ED_head_label(e)->p;
		    sprintf(buf, "%d,%d", pt.x, YDIR(pt.y));
		    agset(e, "head_lp", buf);
		}
		if (ED_tail_label(e)) {
		    pt = ED_tail_label(e)->p;
		    sprintf(buf, "%d,%d", pt.x, YDIR(pt.y));
		    agset(e, "tail_lp", buf);
		}
	    }
	}
    }
    rec_attach_bb(g);
    agxbfree(&xb);

    if (HAS_CLUST_EDGE(g))
	undoClusterEdges(g);
}

static int isInvis(char *style)
{
    char **styles = 0;
    char **sp;
    char *p;

    if (style[0]) {
        styles = parse_style(style);
        sp = styles;
        while ((p = *sp++)) {
            if (streq(p, "invis"))
                return 1;
        }
    }
    return 0;
}

/* 
 * John M. suggests:
 * You might want to add four more:
 *
 * _ohdraw_ (optional head-end arrow for edges)
 * _ohldraw_ (optional head-end label for edges)
 * _otdraw_ (optional tail-end arrow for edges)
 * _otldraw_ (optional tail-end label for edges)
 * 
 * that would be generated when an additional option is supplied to dot, etc. and 
 * these would be the arrow/label positions to use if a user want to flip the 
 * direction of an edge (as sometimes is there want).
 * 
 * N.B. John M. asks:
 *   By the way, I don't know if you ever plan to add other letters for 
 * the xdot spec, but could you reserve "a" and also "A" (for  attribute), 
 * "n" and also "N" (for numeric), "w" (for sWitch),  "s" (for string) 
 * and "t" (for tooltip) and "x" (for position). We use  those letters in 
 * our drawing spec (and also "<" and ">"), so if you  start generating 
 * output with them, it could break what we have. 
 */

#define XDOTVERSION "1.1"

void extend_attrs(GVJ_t * job, graph_t *g, agxbuf** xbufs)
{
    node_t *n;
    edge_t *e;
    attrsym_t *n_draw = NULL;
    attrsym_t *n_l_draw = NULL;
    attrsym_t *e_draw = NULL;
    attrsym_t *h_draw = NULL;
    attrsym_t *t_draw = NULL;
    attrsym_t *e_l_draw = NULL;
    attrsym_t *hl_draw = NULL;
    attrsym_t *tl_draw = NULL;
    unsigned char buf0[BUFSIZ];
    unsigned char buf1[BUFSIZ];
    unsigned char buf2[BUFSIZ];
    unsigned char buf3[BUFSIZ];
    unsigned char buf4[BUFSIZ];
    unsigned char buf5[BUFSIZ];

    agsafeset (g, "xdotversion", XDOTVERSION, "");
    if (GD_has_labels(g) & GRAPH_LABEL)
        g_l_draw = safe_dcl(g, g, "_ldraw_", "", agraphattr);
    else
        g_l_draw = NULL;
    if (GD_n_cluster(g))
        g_draw = safe_dcl(g, g, "_draw_", "", agraphattr);
    else
        g_draw = NULL;

    n_draw = safe_dcl(g, g->proto->n, "_draw_", "", agnodeattr);
    n_l_draw = safe_dcl(g, g->proto->n, "_ldraw_", "", agnodeattr);

    e_draw = safe_dcl(g, g->proto->e, "_draw_", "", agedgeattr);
    if (e_arrows)
        h_draw = safe_dcl(g, g->proto->e, "_hdraw_", "", agedgeattr);
    if (s_arrows)
        t_draw = safe_dcl(g, g->proto->e, "_tdraw_", "", agedgeattr);
    if (GD_has_labels(g) & EDGE_LABEL)
        e_l_draw = safe_dcl(g, g->proto->e, "_ldraw_", "", agedgeattr);
    if (GD_has_labels(g) & HEAD_LABEL)
        hl_draw = safe_dcl(g, g->proto->e, "_hldraw_", "", agedgeattr);
    if (GD_has_labels(g) & TAIL_LABEL)
        tl_draw = safe_dcl(g, g->proto->e, "_tldraw_", "", agedgeattr);

    agxbinit(xbufs[0], BUFSIZ, buf0);
    agxbinit(xbufs[1], BUFSIZ, buf1);
    agxbinit(xbufs[2], BUFSIZ, buf2);
    agxbinit(xbufs[3], BUFSIZ, buf3);
    agxbinit(xbufs[4], BUFSIZ, buf4);
    agxbinit(xbufs[5], BUFSIZ, buf5);

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
        if (ND_shape(n) && !isInvis(late_string(n, N_style, ""))) {
            ND_shape(n)->fns->codefn(job, n);
            agxset(n, n_draw->index, agxbuse(xbufs[EMIT_NDRAW]));
            agxset(n, n_l_draw->index, agxbuse(xbufs[EMIT_NLABEL]));
        }
        if (State < GVSPLINES)
            continue;
        for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
            if (ED_edge_type(e) == IGNORED)
                continue;
            if (isInvis(late_string(e, E_style, "")))
                continue;
            if (ED_spl(e) == NULL)
                continue;

            emit_begin_edge (job, e);
            emit_edge_graphics (job, e);
            emit_end_edge (job);
            agxset(e, e_draw->index, agxbuse(xbufs[EMIT_EDRAW]));
            if (t_draw) agxset(e, t_draw->index, agxbuse(xbufs[EMIT_TDRAW]));
            if (h_draw) agxset(e, h_draw->index, agxbuse(xbufs[EMIT_HDRAW]));
            if (e_l_draw) agxset(e, e_l_draw->index,agxbuse(xbufs[EMIT_ELABEL]));
            if (tl_draw) agxset(e, tl_draw->index, agxbuse(xbufs[EMIT_TLABEL]));
            if (hl_draw) agxset(e, hl_draw->index, agxbuse(xbufs[EMIT_HLABEL]));
        }
    }

    emit_background(job, g);
    if (agxblen(xbufs[EMIT_GDRAW])) {
        if (!g_draw)
            g_draw = safe_dcl(g, g, "_draw_", "", agraphattr);
        agxset(g, g_draw->index, agxbuse(xbufs[EMIT_GDRAW]));
    }
    if (GD_label(g)) {
        emit_label(job, EMIT_GLABEL, GD_label(g));
        agxset(g, g_l_draw->index, agxbuse(xbufs[EMIT_GLABEL]));
    }
    emit_clusters(job, g, 0);
    agxbfree(xbufs[0]);
    agxbfree(xbufs[1]);
    agxbfree(xbufs[2]);
    agxbfree(xbufs[3]);
    agxbfree(xbufs[4]);
    agxbfree(xbufs[5]);
}

void extend_attrs_glabel(graph_t *sg, agxbuf **xbufs)
{
    agxset(sg, g_draw->index, agxbuse(xbufs[EMIT_CDRAW]));
    if (GD_label(sg))
	agxset(sg, g_l_draw->index, agxbuse(xbufs[EMIT_CLABEL]));
}
