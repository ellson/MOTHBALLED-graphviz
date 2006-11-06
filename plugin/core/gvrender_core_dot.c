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

#ifdef WIN32
#include <io.h>
#include "compat.h"
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "const.h"

#include "gvplugin_render.h"
#include "graph.h"
#include "agxbuf.h"
#include "utils.h"

extern void attach_attrs(graph_t * g);
extern void attach_attrs_and_arrows(graph_t*, int*, int*);
extern char *xml_string(char *str);
extern void write_plain(GVJ_t * job, graph_t * g, FILE * f, bool extend);

#define GNEW(t)          (t*)malloc(sizeof(t))

typedef enum { FORMAT_DOT, FORMAT_CANON, FORMAT_PLAIN, FORMAT_PLAIN_EXT, FORMAT_XDOT } format_type;

#define XDOTVERSION "1.1"

#define NUMXBUFS (EMIT_HLABEL+1)
/* There are as many xbufs as there are values of emit_state_t.
 * However, only the first NUMXBUFS are distinct. Nodes, clusters, and
 * edges are drawn atomically, so they share the DRAW and LABEL buffers
 * are shared.
 */
static agxbuf xbuf[NUMXBUFS];
static agxbuf* xbufs[] = {
    xbuf+EMIT_GDRAW, xbuf+EMIT_CDRAW, xbuf+EMIT_TDRAW, xbuf+EMIT_HDRAW, 
    xbuf+EMIT_GLABEL, xbuf+EMIT_CLABEL, xbuf+EMIT_TLABEL, xbuf+EMIT_HLABEL, 
    xbuf+EMIT_CDRAW, xbuf+EMIT_CDRAW, xbuf+EMIT_CLABEL, xbuf+EMIT_CLABEL, 
};

typedef struct {
    attrsym_t *g_draw;
    attrsym_t *g_l_draw;
    attrsym_t *n_draw;
    attrsym_t *n_l_draw;
    attrsym_t *e_draw;
    attrsym_t *h_draw;
    attrsym_t *t_draw;
    attrsym_t *e_l_draw;
    attrsym_t *hl_draw;
    attrsym_t *tl_draw;
    unsigned char buf[NUMXBUFS][BUFSIZ];
} xdot_state_t;
static xdot_state_t* xd;

static void xdot_str (GVJ_t *job, char* pfx, char* s)
{   
    emit_state_t emit_state = job->obj->emit_state;
    char buf[BUFSIZ];
    
    sprintf (buf, "%s%d -", pfx, (int)strlen(s));
    agxbput(xbufs[emit_state], buf);
    agxbput(xbufs[emit_state], s);
    agxbputc(xbufs[emit_state], ' ');
}

static void xdot_points(GVJ_t *job, char c, pointf * A, int n)
{
    emit_state_t emit_state = job->obj->emit_state;
    char buf[BUFSIZ];
    int i, rc;

    rc = agxbputc(xbufs[emit_state], c);
    sprintf(buf, " %d ", n);
    agxbput(xbufs[emit_state], buf);
    for (i = 0; i < n; i++) {
        sprintf(buf, "%d %d ", ROUND(A[i].x), ROUND(A[i].y));
        agxbput(xbufs[emit_state], buf);
    }
}

static void xdot_pencolor (GVJ_t *job)
{
    xdot_str (job, "c ", job->obj->pencolor.u.string);
}

static void xdot_fillcolor (GVJ_t *job)
{
    xdot_str (job, "C ", job->obj->fillcolor.u.string);
}

static void xdot_style (GVJ_t *job)
{
    unsigned char buf[BUFSIZ];
    agxbuf xbuf;
    char* p, **s;
    int more;

    s = job->obj->rawstyle;
    if (!s)
	return;
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
        xdot_str (job, "S ", agxbuse(&xbuf));
    }
    agxbfree(&xbuf);
}

static void xdot_end_node(GVJ_t* job)
{
    Agnode_t* n = job->obj->u.n; 
    if (agxblen(xbufs[EMIT_NDRAW]))
	agxset(n, xd->n_draw->index, agxbuse(xbufs[EMIT_NDRAW]));
    if (agxblen(xbufs[EMIT_NLABEL]))
	agxset(n, xd->n_l_draw->index, agxbuse(xbufs[EMIT_NLABEL]));
}

static void xdot_end_edge(GVJ_t* job)
{
    Agedge_t* e = job->obj->u.e; 

    if (agxblen(xbufs[EMIT_EDRAW]))
	agxset(e, xd->e_draw->index, agxbuse(xbufs[EMIT_EDRAW]));
    if (agxblen(xbufs[EMIT_TDRAW]))
	agxset(e, xd->t_draw->index, agxbuse(xbufs[EMIT_TDRAW]));
    if (agxblen(xbufs[EMIT_HDRAW]))
	agxset(e, xd->h_draw->index, agxbuse(xbufs[EMIT_HDRAW]));
    if (agxblen(xbufs[EMIT_ELABEL]))
	agxset(e, xd->e_l_draw->index,agxbuse(xbufs[EMIT_ELABEL]));
    if (agxblen(xbufs[EMIT_TLABEL]))
	agxset(e, xd->tl_draw->index, agxbuse(xbufs[EMIT_TLABEL]));
    if (agxblen(xbufs[EMIT_HLABEL]))
	agxset(e, xd->hl_draw->index, agxbuse(xbufs[EMIT_HLABEL]));
}

static void xdot_end_cluster(GVJ_t * job)
{
    Agraph_t* cluster_g = job->obj->u.sg;

    agxset(cluster_g, xd->g_draw->index, agxbuse(xbufs[EMIT_CDRAW]));
    if (GD_label(cluster_g))
	agxset(cluster_g, xd->g_l_draw->index, agxbuse(xbufs[EMIT_CLABEL]));
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
 * 
 * N.B. John M. asks:
 *   By the way, I don't know if you ever plan to add other letters for 
 * the xdot spec, but could you reserve "a" and also "A" (for  attribute), 
 * "n" and also "N" (for numeric), "w" (for sWitch),  "s" (for string) 
 * and "t" (for tooltip) and "x" (for position). We use  those letters in 
 * our drawing spec (and also "<" and ">"), so if you  start generating 
 * output with them, it could break what we have. 
 */
static void
xdot_begin_graph (graph_t *g, int s_arrows, int e_arrows)
{
    int i;

    xd = GNEW(xdot_state_t);

    if (GD_has_labels(g) & GRAPH_LABEL)
	xd->g_l_draw = safe_dcl(g, g, "_ldraw_", "", agraphattr);
    else
	xd->g_l_draw = NULL;
    if (GD_n_cluster(g))
	xd->g_draw = safe_dcl(g, g, "_draw_", "", agraphattr);
    else
	xd->g_draw = NULL;

    xd->n_draw = safe_dcl(g, g->proto->n, "_draw_", "", agnodeattr);
    xd->n_l_draw = safe_dcl(g, g->proto->n, "_ldraw_", "", agnodeattr);

    xd->e_draw = safe_dcl(g, g->proto->e, "_draw_", "", agedgeattr);
    if (e_arrows)
	xd->h_draw = safe_dcl(g, g->proto->e, "_hdraw_", "", agedgeattr);
    else
	xd->h_draw = NULL;
    if (s_arrows)
	xd->t_draw = safe_dcl(g, g->proto->e, "_tdraw_", "", agedgeattr);
    else
	xd->t_draw = NULL;
    if (GD_has_labels(g) & EDGE_LABEL)
	xd->e_l_draw = safe_dcl(g, g->proto->e, "_ldraw_", "", agedgeattr);
    else
	xd->e_l_draw = NULL;
    if (GD_has_labels(g) & HEAD_LABEL)
	xd->hl_draw = safe_dcl(g, g->proto->e, "_hldraw_", "", agedgeattr);
    else
	xd->hl_draw = NULL;
    if (GD_has_labels(g) & TAIL_LABEL)
	xd->tl_draw = safe_dcl(g, g->proto->e, "_tldraw_", "", agedgeattr);
    else
	xd->tl_draw = NULL;

    for (i = 0; i < NUMXBUFS; i++)
	agxbinit(xbuf+i, BUFSIZ, xd->buf[i]);
}

static void dot_begin_graph(GVJ_t *job)
{
    int e_arrows;            /* graph has edges with end arrows */
    int s_arrows;            /* graph has edges with start arrows */
    graph_t *g = job->obj->u.g;

    switch (job->render.id) {
	case FORMAT_DOT:
	    attach_attrs(g);
	    break;
	case FORMAT_CANON:
	    if (HAS_CLUST_EDGE(g))
		undoClusterEdges(g);
	    break;
	case FORMAT_PLAIN:
	case FORMAT_PLAIN_EXT:
	    break;
	case FORMAT_XDOT:
	    attach_attrs_and_arrows(g, &s_arrows, &e_arrows);
	    xdot_begin_graph(g, s_arrows, e_arrows);
    }
}

static void xdot_end_graph(graph_t* g)
{
    int i;

    if (agxblen(xbufs[EMIT_GDRAW])) {
	if (!xd->g_draw)
	    xd->g_draw = safe_dcl(g, g, "_draw_", "", agraphattr);
	agxset(g, xd->g_draw->index, agxbuse(xbufs[EMIT_GDRAW]));
    }
    if (GD_label(g))
	agxset(g, xd->g_l_draw->index, agxbuse(xbufs[EMIT_GLABEL]));
    agsafeset (g, "xdotversion", XDOTVERSION, "");

    for (i = 0; i < NUMXBUFS; i++)
	agxbfree(xbuf+i);
    free (xd);
}

static void dot_end_graph(GVJ_t *job)
{
    graph_t *g = job->obj->u.g;

    switch (job->render.id) {
	case FORMAT_PLAIN:
	    write_plain(job, g, job->output_file, false);
	    break;
	case FORMAT_PLAIN_EXT:
	    write_plain(job, g, job->output_file, true);
	    break;
	case FORMAT_DOT:
	case FORMAT_CANON:
	    if (!(job->flags & OUTPUT_NOT_REQUIRED))
		agwrite(g, job->output_file);
	    break;
	case FORMAT_XDOT:
	    xdot_end_graph(g);
	    if (!(job->flags & OUTPUT_NOT_REQUIRED))
		agwrite(g, job->output_file);
	    break;
    }
}

static void xdot_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    emit_state_t emit_state = job->obj->emit_state;

    char buf[BUFSIZ];
    int j;
    
    sprintf(buf, "F %f ", para->fontsize);
    agxbput(xbufs[emit_state], buf);
    xdot_str (job, "", para->fontname);
    xdot_pencolor(job);

    switch (para->just) {
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
    sprintf(buf, "T %d %d %d %d ", ROUND(p.x), ROUND(p.y), j, (int) para->width);
    agxbput(xbufs[emit_state], buf);
    xdot_str (job, "", para->str);
}

static void xdot_ellipse(GVJ_t * job, pointf * A, int filled)
{
    emit_state_t emit_state = job->obj->emit_state;

    char buf[BUFSIZ];
    int rc;

    xdot_style (job);
    xdot_pencolor (job);
    if (filled) {
        xdot_fillcolor (job);
        rc = agxbputc(xbufs[emit_state], 'E');
    }
    else
        rc = agxbputc(xbufs[emit_state], 'e');
    sprintf(buf, " %d %d %d %d ",
		ROUND(A[0].x), ROUND(A[0].y), ROUND(A[1].x - A[0].x), ROUND(A[1].y - A[0].y));
    agxbput(xbufs[emit_state], buf);
}

static void xdot_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start, int arrow_at_end, int filled)
{
    xdot_style (job);
    xdot_pencolor (job);
    if (filled) {
        xdot_fillcolor (job);
        xdot_points(job, 'b', A, n);   /* NB - 'B' & 'b' are reversed in comparison to the other items */
    }
    else
        xdot_points(job, 'B', A, n);
}

static void xdot_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    xdot_style (job);
    xdot_pencolor (job);
    if (filled) {
        xdot_fillcolor (job);
        xdot_points(job, 'P', A, n);
    }
    else
        xdot_points(job, 'p', A, n);
}

static void xdot_polyline(GVJ_t * job, pointf * A, int n)
{
    xdot_style (job);
    xdot_pencolor (job);
    xdot_points(job, 'L', A, n);
}

gvrender_engine_t dot_engine = {
    0,				/* dot_begin_job */
    0,				/* dot_end_job */
    dot_begin_graph,
    dot_end_graph,
    0,				/* dot_begin_layer */
    0,				/* dot_end_layer */
    0,				/* dot_begin_page */
    0,				/* dot_end_page */
    0,				/* dot_begin_cluster */
    0,				/* dot_end_cluster */
    0,				/* dot_begin_nodes */
    0,				/* dot_end_nodes */
    0,				/* dot_begin_edges */
    0,				/* dot_end_edges */
    0,				/* dot_begin_node */
    0,				/* dot_end_node */
    0,				/* dot_begin_edge */
    0,				/* dot_end_edge */
    0,				/* dot_begin_anchor */
    0,				/* dot_end_anchor */
    0,				/* dot_textpara */
    0,				/* dot_resolve_color */
    0,				/* dot_ellipse */
    0,				/* dot_polygon */
    0,				/* dot_bezier */
    0,				/* dot_polyline */
    0,				/* dot_comment */
    0,				/* dot_library_shape */
};

gvrender_engine_t xdot_engine = {
    0,				/* xdot_begin_job */
    0,				/* xdot_end_job */
    dot_begin_graph,
    dot_end_graph,
    0,				/* xdot_begin_layer */
    0,				/* xdot_end_layer */
    0,				/* xdot_begin_page */
    0,				/* xdot_end_page */
    0,				/* xdot_begin_cluster */
    xdot_end_cluster,
    0,				/* xdot_begin_nodes */
    0,				/* xdot_end_nodes */
    0,				/* xdot_begin_edges */
    0,				/* xdot_end_edges */
    0,				/* xdot_begin_node */
    xdot_end_node,
    0,				/* xdot_begin_edge */
    xdot_end_edge,
    0,				/* xdot_begin_anchor */
    0,				/* xdot_end_anchor */
    xdot_textpara,
    0,				/* xdot_resolve_color */
    xdot_ellipse,
    xdot_polygon,
    xdot_bezier,
    xdot_polyline,
    0,				/* xdot_comment */
    0,				/* xdot_library_shape */
};

gvrender_features_t canon_features = {
    LAYOUT_NOT_REQUIRED,	/* flags */
    0.,				/* default margin - points */
    0.,                         /* default pad - graph units */
    {0,0},                      /* default height, width - device units */
    {72.,72.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    COLOR_STRING,		/* color_type */
    NULL,                       /* device */
    NULL,                       /* gvloadimage target for usershapes */
};

gvrender_features_t dot_features = {
    GVRENDER_DOES_TRANSFORM,	/* not really - uses raw graph coords */
    0.,				/* default margin - points */
    0.,                         /* default pad - graph units */
    {0.,0.},			/* default page width, height - points */
    {72.,72.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    COLOR_STRING,		/* color_type */
    NULL,                       /* device */
    NULL,                       /* gvloadimage target for usershapes */
};

gvplugin_installed_t gvrender_core_dot_types[] = {
    {FORMAT_DOT, "dot", 1, &dot_engine, &dot_features},
    {FORMAT_CANON, "canon", 1, &dot_engine, &canon_features},
    {FORMAT_PLAIN, "plain", 1, &dot_engine, &dot_features},
    {FORMAT_PLAIN_EXT, "plain-ext", 1, &dot_engine, &dot_features},
    {FORMAT_XDOT, "xdot", 1, &xdot_engine, &dot_features},
    {0, NULL, 0, NULL, NULL}
};
