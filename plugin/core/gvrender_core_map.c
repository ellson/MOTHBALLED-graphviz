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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "gvplugin_render.h"
#include "graph.h"
#include "agxbuf.h"
#include "utils.h"

extern char *xml_string(char *str);

typedef enum { FORMAT_IMAP, FORMAT_ISMAP, FORMAT_CMAP, FORMAT_CMAPX, } format_type;

static void mapgen_begin_job(GVJ_t * job)
{
    switch (job->render.id) {
    case FORMAT_IMAP:
        fprintf(job->output_file, "base referer\n");
	break;
    default:
	break;
    }
}

static void mapgen_begin_page(GVJ_t * job)
{
    obj_state_t *obj = job->obj;
    char *name = xml_string(obj->g->name);

    switch (job->render.id) {
    case FORMAT_CMAPX:
	fprintf(job->output_file, "<map id=\"%s\" name=\"%s\">\n", name, name);
        break;
    default:
	break;
    }
    if (obj->url) {
        switch (job->render.id) {
        case FORMAT_IMAP:
	    fprintf(job->output_file, "default %s\n", obj->url);
            break;
        case FORMAT_ISMAP:
	    fprintf(job->output_file, "default %s %s\n", obj->url, obj->g->name);
            break;
        case FORMAT_CMAP:
        case FORMAT_CMAPX:
	/* FIXME */
            break;
        default:
	    break;
        }
    }
}

static void mapgen_end_page(GVJ_t * job)
{
    switch (job->render.id) {
    case FORMAT_CMAPX:
        fprintf(job->output_file, "</map>\n");
	break;
    default:
	break;
    }
}

static void mapgen_begin_cluster(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    fprintf(job->output_file, "%% %s\n", obj->sg->name);

    if (obj->url) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
    }
}

static void mapgen_begin_node(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    if (obj->url_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
    }
}

static void
mapgen_begin_edge(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    if (obj->url_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->url_map_p[0].x, obj->url_map_p[0].y,
		obj->url_map_p[1].x, obj->url_map_p[1].y);
    }
    if (obj->tailurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->tailurl_map_p[0].x, obj->tailurl_map_p[0].y,
		obj->tailurl_map_p[1].x, obj->tailurl_map_p[1].y);
    }
    if (obj->headurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->headurl_map_p[0].x, obj->headurl_map_p[0].y,
		obj->headurl_map_p[1].x, obj->headurl_map_p[1].y);
    }
    if (obj->tailendurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->tailendurl_map_p[0].x, obj->tailendurl_map_p[0].y,
		obj->tailendurl_map_p[1].x, obj->tailendurl_map_p[1].y);
    }
    if (obj->headendurl_map_p) {
        fprintf(job->output_file, "[ /Rect [ %g %g %g %g ]\n",
		obj->headendurl_map_p[0].x, obj->headendurl_map_p[0].y,
		obj->headendurl_map_p[1].x, obj->headendurl_map_p[1].y);
    }
}

static gvrender_engine_t mapgen_engine = {
    mapgen_begin_job,
    0,				/* mapgen_end_job */
    0,				/* mapgen_begin_graph */
    0,				/* mapgen_end_graph */
    0,				/* mapgen_begin_layer */
    0,				/* mapgen_end_layer */
    mapgen_begin_page,
    mapgen_end_page,
    mapgen_begin_cluster,
    0,				/* mapgen_end_cluster */
    0,				/* mapgen_begin_nodes */
    0,				/* mapgen_end_nodes */
    0,				/* mapgen_begin_edges */
    0,				/* mapgen_end_edges */
    mapgen_begin_node,
    0,				/* mapgen_end_node */
    mapgen_begin_edge,
    0,				/* mapgen_end_edge */
    0,				/* mapgen_begin_anchor */
    0,				/* mapgen_end_anchor */
    0,				/* mapgen_textpara */
    0,				/* mapgen_resolve_color */
    0,				/* mapgen_ellipse */
    0,				/* mapgen_polygon */
    0,				/* mapgen_bezier */
    0,				/* mapgen_polyline */
    0,				/* mapgen_comment */
    0				/* mapgen_usershape */
};

static gvrender_features_t mapgen_features_poly = {
    GVRENDER_DOES_MAPS
	| GVRENDER_DOES_MAP_RECT
	| GVRENDER_DOES_MAP_POLY,
    0,				/* default margin - points */
    {72.,72.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    0,				/* color_type */
};

static gvrender_features_t mapgen_features = {
    GVRENDER_DOES_MAPS
	| GVRENDER_DOES_MAP_RECT,
    0,				/* default margin - points */
    {72.,72.},			/* default dpi */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    0,				/* color_type */
};

gvplugin_installed_t gvrender_core_map_types[] = {
    {FORMAT_IMAP, "imap", -1, &mapgen_engine, &mapgen_features_poly},
    {FORMAT_ISMAP, "ismap", -1, &mapgen_engine, &mapgen_features},
    {FORMAT_CMAP, "cmap", -1, &mapgen_engine, &mapgen_features},
    {FORMAT_CMAPX, "cmapx", -1, &mapgen_engine, &mapgen_features_poly},
    {0, NULL, 0, NULL, NULL}
};
