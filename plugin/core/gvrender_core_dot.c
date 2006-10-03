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
extern char *xml_string(char *str);
extern void write_plain(GVJ_t * job, graph_t * g, FILE * f, bool extend);
extern void extend_attrs(GVJ_t * job, graph_t *g, agxbuf** xbufs);
extern void extend_attrs_glabel(graph_t *sg, agxbuf **xbufs);

typedef enum { FORMAT_DOT, FORMAT_CANON, FORMAT_PLAIN, FORMAT_PLAIN_EXT, FORMAT_XDOT } format_type;

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

static void dot_begin_graph(GVJ_t *job)
{
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
	    attach_attrs(g);
    }
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
	    extend_attrs(job, g, xbufs);
	    if (!(job->flags & OUTPUT_NOT_REQUIRED))
		agwrite(g, job->output_file);
	    break;
    }
}

static void xdot_end_cluster(GVJ_t *job)
{
    extend_attrs_glabel(job->obj->u.sg, xbufs);
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
    0,				/* xdot_end_node */
    0,				/* xdot_begin_edge */
    0,				/* xdot_end_edge */
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
