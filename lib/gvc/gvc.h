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

/* This is the public header for the callers of libgvc */

#ifndef GVC_H
#define GVC_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* context */

    extern GVC_t *gvNEWcontext(char **info, char *user);
    extern void gvFREEcontext(GVC_t * gvc);

/* configuration */

    extern void gvconfig(GVC_t * gvc, char *config);

/* plugins */

    extern boolean gvplugin_install(GVC_t * gvc, api_t api,
				    char *typestr, int quality, char *path,
				    gvplugin_type_t * typeptr);
    extern gv_plugin_t *gvplugin_load(GVC_t * gvc, api_t api, char *type);
    extern const char *gvplugin_list(GVC_t * gvc, api_t api, char *str);
    extern void gvplugin_builtins(GVC_t * gvc);
    extern api_t gvplugin_api(char *str);

/* job */

    extern void gvrender_output_filename_job(GVC_t * gvc, char *name);
    extern int gvrender_output_langname_job(GVC_t * gvc, char *name);
    extern gvrender_job_t *gvrender_first_job(GVC_t * gvc);
    extern gvrender_job_t *gvrender_next_job(GVC_t * gvc);
    extern void gvrender_delete_jobs(GVC_t * gvc);

/* emit */
    extern void gvemit_graph(GVC_t * gvc, graph_t * g);

/* text_layout */

    extern int gvtext_select(GVC_t * gvc, char *text_layout);
    extern double gvtext_width(GVC_t *gvc, textline_t *textline,
			       char *fontname, double fontsize,
			       char *fontpath);

/* render */

    extern int gvrender_select(GVC_t * gvc, char *lang);
    extern int gvrender_features(GVC_t * gvc);
    extern void gvrender_initialize(GVC_t * gvc);
    extern void gvrender_finalize(GVC_t * gvc);
    extern void gvrender_begin_job(GVC_t * gvc);
    extern void gvrender_end_job(GVC_t * gvc);
    extern void gvrender_begin_graph(GVC_t * gvc, graph_t * g);
    extern void gvrender_end_graph(GVC_t * gvc);
    extern void gvrender_begin_page(GVC_t * gvc);
    extern void gvrender_end_page(GVC_t * gvc);
    extern void gvrender_begin_layer(GVC_t * gvc);
    extern void gvrender_end_layer(GVC_t * gvc);
    extern void gvrender_begin_cluster(GVC_t * gvc, graph_t * sg);
    extern void gvrender_end_cluster(GVC_t * gvc);
    extern void gvrender_begin_nodes(GVC_t * gvc);
    extern void gvrender_end_nodes(GVC_t * gvc);
    extern void gvrender_begin_edges(GVC_t * gvc);
    extern void gvrender_end_edges(GVC_t * gvc);
    extern void gvrender_begin_node(GVC_t * gvc, node_t * n);
    extern void gvrender_end_node(GVC_t * gvc);
    extern void gvrender_begin_edge(GVC_t * gvc, edge_t * e);
    extern void gvrender_end_edge(GVC_t * gvc);
    extern void gvrender_begin_context(GVC_t * gvc);
    extern void gvrender_end_context(GVC_t * gvc);
    extern void gvrender_begin_anchor(GVC_t * gvc, char *href,
				      char *tooltip, char *target);
    extern void gvrender_end_anchor(GVC_t * gvc);
    extern void gvrender_set_font(GVC_t * gvc, char *fontname,
				  double fontsize);
    extern void gvrender_textline(GVC_t * gvc, pointf p, textline_t * str);
    extern void gvrender_set_pencolor(GVC_t * gvc, char *name);
    extern void gvrender_set_fillcolor(GVC_t * gvc, char *name);
    extern void gvrender_set_style(GVC_t * gvc, char **s);
    extern void gvrender_ellipse(GVC_t * gvc, point p, int rx, int ry,
				 int filled);
    extern void gvrender_polygon(GVC_t * gvc, point * A, int n,
				 int filled);
    extern void gvrender_beziercurve(GVC_t * gvc, pointf * AF, int n,
				     int arrow_at_start, int arrow_at_end);
    extern void gvrender_polyline(GVC_t * gvc, point * A, int n);
    extern void gvrender_comment(GVC_t * gvc, void *obj, attrsym_t * sym);
    extern void gvrender_user_shape(GVC_t * gvc, char *name, point * A,
				    int sides, int filled);

/* layout */

    extern int gvlayout_select(GVC_t * gvc, char *str);
    extern void gvlayout_layout(GVC_t * gvc, graph_t * g);
    extern void gvlayout_cleanup(GVC_t * gvc, graph_t * g);

#ifdef __cplusplus
}
#endif
#endif				/* GVC_H */
