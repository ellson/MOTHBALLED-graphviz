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
	void (*begin_job) (gvrender_job_t * job);
	void (*end_job) (gvrender_job_t * job);
	void (*begin_graph) (gvrender_job_t * job, char *graphname);
	void (*end_graph) (gvrender_job_t * job);
	void (*begin_page) (gvrender_job_t * job, char *pagename);
	void (*end_page) (gvrender_job_t * job);
	void (*begin_layer) (gvrender_job_t * job, char *layername);
	void (*end_layer) (gvrender_job_t * job);
	void (*begin_cluster) (gvrender_job_t * job, char *clustername, long id);
	void (*end_cluster) (gvrender_job_t * job);
	void (*begin_nodes) (gvrender_job_t * job);
	void (*end_nodes) (gvrender_job_t * job);
	void (*begin_edges) (gvrender_job_t * job);
	void (*end_edges) (gvrender_job_t * job);
	void (*begin_node) (gvrender_job_t * job, char *nodename, long id);
	void (*end_node) (gvrender_job_t * job);
	void (*begin_edge) (gvrender_job_t * job, char *tailname, boolean directed,
			    char *headname, long id);
	void (*end_edge) (gvrender_job_t * job);
	void (*begin_anchor) (gvrender_job_t * job, char *href, char *tooltip,
			      char *target);
	void (*end_anchor) (gvrender_job_t * job);
	void (*textline) (gvrender_job_t * job, pointf p, textline_t * str);
	void (*resolve_color) (gvrender_job_t * job, color_t * color);
	void (*ellipse) (gvrender_job_t * job, pointf * A, int filled);
	void (*polygon) (gvrender_job_t * job, pointf * A, int n, int filled);
	void (*beziercurve) (gvrender_job_t * job, pointf * A, int n,
			     int arrow_at_start, int arrow_at_end);
	void (*polyline) (gvrender_job_t * job, pointf * A, int n);
	void (*comment) (gvrender_job_t * job, char *comment);
	void (*user_shape) (gvrender_job_t * job, char *name, pointf * A, int sides,
			    int filled);
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVRENDER_PLUGIN_H */
