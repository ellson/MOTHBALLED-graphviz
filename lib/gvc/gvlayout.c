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

#include <stdio.h>

#include "logic.h"
#include "geom.h"
#include "const.h"
#include "types.h"
#include "macros.h"
#include "graph.h"
#include "cdt.h"

#include "gvplugin_layout.h"
#include "gvplugin.h"
#include "gvcint.h"
#include "gvcproc.h"

extern void graph_init(graph_t *g, boolean use_rankdir);
extern void graph_cleanup(graph_t *g);

int gvlayout_select(GVC_t * gvc, char *layout)
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

int gvLayoutJobs(GVC_t * gvc, graph_t * g)
{
    gvlayout_engine_t *gvle = gvc->layout.engine;

    if (! gvle)
	return -1;

    graph_init(g, gvc->layout.features->flags & LAYOUT_USES_RANKDIR);
    GD_gvc(g) = gvc;
    if (gvle && gvle->layout)
	gvle->layout(g);
    return 0;
}

int gvFreeLayout(GVC_t * gvc, graph_t * g)
{
    gvlayout_engine_t *gvle = gvc->layout.engine;

    g = g->root;
    if (gvc->active_jobs)
	gvdevice_finalize(gvc);
    if (gvle && gvle->cleanup)
	gvle->cleanup(g);
    graph_cleanup(g);
    return 0;
}
