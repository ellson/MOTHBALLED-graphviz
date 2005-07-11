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

    extern void gvconfig(GVC_t * gvc);
    extern char *gvhostname(void);

/* plugins */

    extern boolean gvplugin_install(GVC_t * gvc, api_t api,
		    char *typestr, int quality, char *packagename, char *path,
		    gvplugin_installed_t * typeptr);
    extern gvplugin_available_t *gvplugin_load(GVC_t * gvc, api_t api, char *type);
    extern gvplugin_library_t *gvplugin_library_load(char *path);
    extern const char *gvplugin_list(GVC_t * gvc, api_t api, char *str);
    extern api_t gvplugin_api(char *str);
    extern char * gvplugin_api_name(api_t api);

/* job */

    extern void gvrender_output_filename_job(GVC_t * gvc, char *name);
    extern boolean gvrender_output_langname_job(GVC_t * gvc, char *name);
    extern GVJ_t *gvrender_first_job(GVC_t * gvc);
    extern GVJ_t *gvrender_next_job(GVC_t * gvc);
    extern void gvrender_delete_jobs(GVC_t * gvc);

/* emit */
    extern void gvemit_graph(GVC_t * gvc, graph_t * g);

/* textlayout */

    extern int gvtextlayout_select(GVC_t * gvc, char *textlayout);
    extern double gvtextlayout_width(GVC_t *gvc, textline_t *textline,
		       char *fontname, double fontsize, char *fontpath);

/* usershape */

    extern int gvusershape_select(GVC_t * gvc, char *name);
    extern void * gvusershape_getshape(GVC_t * gvc, char *usershape);
    extern void gvusershape_freeeshapes(GVC_t * gvc);
    extern point gvusershape_imagesize(GVC_t * gvc, char *usershape);

/* device */

    extern void gvdevice_initialize(GVC_t * gvc);
    extern void gvdevice_finalize(GVC_t * gvc);
    extern void gvdevice_begin_job(GVJ_t * job);
    extern void gvdevice_end_job(GVJ_t * job);

/* render */

    extern void gvrender_begin_job(GVJ_t * job);
    extern void gvrender_end_job(GVJ_t * job);
    extern int gvrender_select(GVJ_t * job, char *lang);
    extern int gvrender_features(GVJ_t * job);
    extern void gvrender_begin_graph(GVJ_t * job, graph_t * g);
    extern void gvrender_end_graph(GVJ_t * job);
    extern void gvrender_begin_page(GVJ_t * job);
    extern void gvrender_end_page(GVJ_t * job);
    extern void gvrender_begin_layer(GVJ_t * job);
    extern void gvrender_end_layer(GVJ_t * job);
    extern void gvrender_begin_cluster(GVJ_t * job, graph_t * sg);
    extern void gvrender_end_cluster(GVJ_t * job, graph_t *g);
    extern void gvrender_begin_nodes(GVJ_t * job);
    extern void gvrender_end_nodes(GVJ_t * job);
    extern void gvrender_begin_edges(GVJ_t * job);
    extern void gvrender_end_edges(GVJ_t * job);
    extern void gvrender_begin_node(GVJ_t * job, node_t * n);
    extern void gvrender_end_node(GVJ_t * job);
    extern void gvrender_begin_edge(GVJ_t * job, edge_t * e);
    extern void gvrender_end_edge(GVJ_t * job);
    extern void gvrender_begin_context(GVJ_t * job);
    extern void gvrender_end_context(GVJ_t * job);
    extern void gvrender_begin_anchor(GVJ_t * job, char *href,
				      char *tooltip, char *target);
    extern void gvrender_end_anchor(GVJ_t * job);
    extern void gvrender_set_font(GVJ_t * job, char *fontname,
				  double fontsize);
    extern void gvrender_textline(GVJ_t * job, pointf p, textline_t * str);
    extern void gvrender_set_pencolor(GVJ_t * job, char *name);
    extern void gvrender_set_fillcolor(GVJ_t * job, char *name);
    extern void gvrender_set_style(GVJ_t * job, char **s);
    extern void gvrender_ellipse(GVJ_t * job, point p, int rx, int ry,
				 int filled);
    extern void gvrender_polygon(GVJ_t * job, point * A, int n,
				 int filled);
    extern void gvrender_beziercurve(GVJ_t * job, pointf * AF, int n,
				     int arrow_at_start, int arrow_at_end, int);
    extern void gvrender_polyline(GVJ_t * job, point * A, int n);
    extern void gvrender_comment(GVJ_t * job, char *str);
    extern void gvrender_user_shape(GVJ_t * job, char *name, point * A,
				    int sides, int filled);

/* layout */

    extern int gvlayout_select(GVC_t * gvc, char *str);
    extern void gvlayout_layout(GVC_t * gvc, graph_t * g);
    extern void gvlayout_cleanup(GVC_t * gvc, graph_t * g);

#ifdef __cplusplus
}
#endif
#endif				/* GVC_H */
