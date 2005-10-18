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

#define XDOTVERSION "1.1"

static GVC_t *gvc;
static agxbuf xbuf0;
static agxbuf xbuf1;
static agxbuf xbuf2;
static agxbuf xbuf3;
static agxbuf xbuf4;
static agxbuf xbuf5;
static agxbuf* xbufs[6] = {
  &xbuf0, &xbuf1,
  &xbuf2, &xbuf3,
  &xbuf4, &xbuf5,
};
static Agraph_t *cluster_g;

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
 * that would be generated when an additional option is supplied to 
 * dot, etc. and 
 * these would be the arrow/label positions to use if a user want to flip the 
 * direction of an edge (as sometimes is there want).
 */
void extend_attrs(GVJ_t * job, graph_t *g, int s_arrows, int e_arrows)
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

    gvc = job->gvc;

    agsafeset (g, "xdotversion", XDOTVERSION, "");
    if (GD_has_labels(g) & GRAPH_LABEL)
	g_l_draw = safe_dcl(g, g, "_ldraw_", "", agraphattr);
    if (GD_n_cluster(g))
	g_draw = safe_dcl(g, g, "_draw_", "", agraphattr);

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

    agxbinit(&xbuf0, BUFSIZ, buf0);
    agxbinit(&xbuf1, BUFSIZ, buf1);
    agxbinit(&xbuf2, BUFSIZ, buf2);
    agxbinit(&xbuf3, BUFSIZ, buf3);
    agxbinit(&xbuf4, BUFSIZ, buf4);
    agxbinit(&xbuf5, BUFSIZ, buf5);

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

	    emit_edge_graphics (job, e);
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
	emit_label(job, EMIT_GLABEL, GD_label(g), (void *) g);
	agxset(g, g_l_draw->index, agxbuse(xbufs[EMIT_GLABEL]));
    }
    emit_clusters(job, g, 0);
    agxbfree(&xbuf0);
    agxbfree(&xbuf1);
    agxbfree(&xbuf2);
    agxbfree(&xbuf3);
    agxbfree(&xbuf4);
    agxbfree(&xbuf5);
}

static void xd_str (char* pfx, char* s)
{
    char buf[BUFSIZ];

    sprintf (buf, "%s%d -", pfx, (int)strlen(s));
    agxbput(xbufs[gvc->emit_state], buf);
    agxbput(xbufs[gvc->emit_state], s);
    agxbputc(xbufs[gvc->emit_state], ' ');
}

static void xd_textline(point p, textline_t * line)
{
    char buf[BUFSIZ];
    int j;

    switch (line->just) {
    case 'l':
	j = -1;
	break;
    case 'r':
	j = 1;
	break;
    default:
    case 'n':
	j = 0;
	break;
    }
    sprintf(buf, "T %d %d %d %d ", p.x, YDIR(p.y), j, (int) line->width);
    agxbput(xbufs[gvc->emit_state], buf);
    xd_str ("", line->str);
}

static void xd_ellipse(point p, int rx, int ry, int filled)
{
    char buf[BUFSIZ];

    agxbputc(xbufs[gvc->emit_state], (filled ? 'E' : 'e'));
    sprintf(buf, " %d %d %d %d ", p.x, YDIR(p.y), rx, ry);
    agxbput(xbufs[gvc->emit_state], buf);
}

static void xd_points(char c, point * A, int n)
{
    char buf[BUFSIZ];
    int i;
    point p;

    agxbputc(xbufs[gvc->emit_state], c);
    sprintf(buf, " %d ", n);
    agxbput(xbufs[gvc->emit_state], buf);
    for (i = 0; i < n; i++) {
	p = A[i];
	sprintf(buf, "%d %d ", p.x, YDIR(p.y));
	agxbput(xbufs[gvc->emit_state], buf);
    }
}

static void xd_polygon(point * A, int n, int filled)
{
    xd_points((filled ? 'P' : 'p'), A, n);
}

static void
xd_bezier(point * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    if (filled)
	xd_points('b', A, n);
    else
	xd_points('B', A, n);
}

static void xd_polyline(point * A, int n)
{
    xd_points('L', A, n);
}

static void 
xd_set_font (char *fontname, double fontsize)
{
    char buf[BUFSIZ];

    sprintf(buf, "F %f ", fontsize);
    agxbput(xbufs[gvc->emit_state], buf);
    xd_str ("", fontname);
}

static void 
xd_set_pencolor (char *name)
{
    xd_str ("c ", name);
}

static void 
xd_set_fillcolor (char *name)
{
    xd_str ("C ", name);
}

static void 
xd_set_style (char **s)
{
    unsigned char buf[BUFSIZ];
    agxbuf xbuf;
    char* p;
    int more;

    agxbinit(&xbuf, BUFSIZ, buf);
    while ((p = *s++)) {
	agxbput(&xbuf, p);
	while (*p)
	    p++;
	p++;
	if (*p) {  /* arguments */
	    agxbputc(&xbuf, '(');
            more = 0;
	    while (*p) {
		if (more)
		    agxbputc(&xbuf, ',');
		agxbput(&xbuf, p);
	        while (*p) p++;
		p++;
		more++;
	    }
	    agxbputc(&xbuf, ')');
	}
	xd_str ("S ", agxbuse(&xbuf));
    }
    agxbfree(&xbuf);
}

static void xd_begin_cluster(Agraph_t * sg)
{
    cluster_g = sg;
}

static void xd_end_cluster(void)
{
    agxset(cluster_g, g_draw->index, agxbuse(xbufs[EMIT_CDRAW]));
    if (GD_label(cluster_g))
	agxset(cluster_g, g_l_draw->index, agxbuse(xbufs[EMIT_CLABEL]));
}

codegen_t XDot_CodeGen = {
    0,				/* xd_reset */
    0, /* xd_begin_job */ 0,	/* xd_end_job */
    0, /* xd_begin_graph */ 0,	/* xd_end_graph */
    0, /* xd_begin_page */ 0,	/* xd_end_page */
    0, /* xd_begin_layer */ 0,	/* xd_end_layer */
    xd_begin_cluster, xd_end_cluster,
    0, /* xd_begin_nodes */ 0,	/* xd_end_nodes */
    0, /* xd_begin_edges */ 0,	/* xd_end_edges */
    0, /* xd_begin_node */ 0, /* xd_end_node */
    0, /* xd_begin_edge */ 0, /* xd_end_edge */
    0, /* xd_begin_context */ 0,	/* xd_context */
    0, /* xd_begin_anchor */ 0,	/* xd_anchor */
    xd_set_font, xd_textline,
    xd_set_pencolor, xd_set_fillcolor, xd_set_style,
    xd_ellipse, xd_polygon,
    xd_bezier, xd_polyline,
    0, /* xd_has_arrows */ 0,	/* xd_comment */
    0, /* xd_textsize */ 0, /* xd_user_shape */ 0	/* xd_usershapesize */
};
