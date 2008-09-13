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

/*
 *  layout engine wrapper
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "const.h"
#include "gvplugin_layout.h"
#include "gvcint.h"
#include "graph.h"
#include "gvcproc.h"

extern void graph_init(graph_t *g, boolean use_rankdir);
extern void graph_cleanup(graph_t *g);

int gvlayout_select(GVC_t * gvc, const char *layout)
{
    gvplugin_available_t *plugin;
    gvplugin_installed_t *typeptr;

    plugin = gvplugin_load(gvc, API_layout, layout);
    if (plugin) {
	typeptr = plugin->typeptr;
	gvc->layout.type = typeptr->type;
	gvc->layout.engine = (gvlayout_engine_t *) (typeptr->engine);
	gvc->layout.id = typeptr->id;
	gvc->layout.features = (gvlayout_features_t *) (typeptr->features);
	return GVRENDER_PLUGIN;  /* FIXME - need better return code */
    }
    return NO_SUPPORT;
}

/* gvLayoutJobs:
 * Layout input graph g based on layout engine attached to gvc.
 * Check that the root graph has been initialized. If not, initialize it.
 * Return 0 on success.
 */
int gvLayoutJobs(GVC_t * gvc, graph_t * g)
{
    gvlayout_engine_t *gvle;
    char *p;
    int rc;

    GD_gvc(g) = gvc;
    if (g != g->root) GD_gvc(g->root) = gvc;

    if ((p = agget(g, "layout"))) {
	rc = gvlayout_select(gvc, p);
	if (rc == NO_SUPPORT) {
	    agerr (AGERR, "Layout type: \"%s\" not recognized. Use one of:%s\n",
	        p, gvplugin_list(gvc, API_layout, p));
	    return -1;
	}
    }

    gvle = gvc->layout.engine;
    if (! gvle)
	return -1;

    graph_init(g, gvc->layout.features->flags & LAYOUT_USES_RANKDIR);
    GD_drawing(g->root) = GD_drawing(g);
    if (gvle && gvle->layout) {
	gvle->layout(g);
	if (gvle->cleanup)
	    GD_cleanup(g) = gvle->cleanup;
    }
    return 0;
}

/* gvFreeLayout:
 * Free layout resources.
 * First, if the graph has a layout-specific cleanup function attached,
 * use it and reset.
 * Then, if the root graph has not been cleaned up, clean it up and reset.
 * Only the root graph has GD_drawing non-null.
 */
int gvFreeLayout(GVC_t * gvc, graph_t * g)
{
    if (GD_cleanup(g)) {
	(GD_cleanup(g))(g);
	GD_cleanup(g) = NULL;
    }
    
    if (GD_drawing(g)) {
	graph_cleanup(g);
	GD_drawing(g) = NULL;
	GD_drawing(g->root) = NULL;
    }
    return 0;
}
