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

/* This is the public header for the libgvre_* plugins */

#ifndef GVRENDER_PLUGIN_H
#define GVRENDER_PLUGIN_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvrender_engine_s {
	void (*begin_job) (GVC_t * gvc);
	void (*end_job) (GVC_t * gvc);
	void (*begin_graph) (GVC_t * gvc, char *graphname);
	void (*end_graph) (GVC_t * gvc);
	void (*begin_page) (GVC_t * gvc, char *pagename);
	void (*end_page) (GVC_t * gvc);
	void (*begin_layer) (GVC_t * gvc, char *layername);
	void (*end_layer) (GVC_t * gvc);
	void (*begin_cluster) (GVC_t * gvc, char *clustername, long id);
	void (*end_cluster) (GVC_t * gvc);
	void (*begin_nodes) (GVC_t * gvc);
	void (*end_nodes) (GVC_t * gvc);
	void (*begin_edges) (GVC_t * gvc);
	void (*end_edges) (GVC_t * gvc);
	void (*begin_node) (GVC_t * gvc, char *nodename, long id);
	void (*end_node) (GVC_t * gvc);
	void (*begin_edge) (GVC_t * gvc, char *tailname, boolean directed,
			    char *headname, long id);
	void (*end_edge) (GVC_t * gvc);
	void (*begin_anchor) (GVC_t * gvc, char *href, char *tooltip,
			      char *target);
	void (*end_anchor) (GVC_t * gvc);
	void (*textline) (GVC_t * gvc, pointf p, textline_t * str);
	void (*resolve_color) (GVC_t * gvc, color_t * color);
	void (*ellipse) (GVC_t * gvc, pointf * A, int filled);
	void (*polygon) (GVC_t * gvc, pointf * A, int n, int filled);
	void (*beziercurve) (GVC_t * gvc, pointf * A, int n,
			     int arrow_at_start, int arrow_at_end);
	void (*polyline) (GVC_t * gvc, pointf * A, int n);
	void (*comment) (GVC_t * gvc, char *comment);
	void (*user_shape) (GVC_t * gvc, char *name, pointf * A, int sides,
			    int filled);
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVRENDER_PLUGIN_H */
