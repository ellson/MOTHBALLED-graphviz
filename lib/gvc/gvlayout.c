/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

/*
 *  layout engine wrapper
 *
 */

#include "config.h"

#include "const.h"
#include "gvplugin_layout.h"
#include "gvcint.h"
#include "cgraph.h"
#include "gvcproc.h"
#include "gvc.h"

extern void graph_init(Agraph_t *g, boolean use_rankdir);
extern void graph_cleanup(Agraph_t *g);
extern void gv_fixLocale (int set);
extern void gv_initShapes (void);

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
int gvLayoutJobs(GVC_t * gvc, Agraph_t * g)
{
    gvlayout_engine_t *gvle;
    char *p;
    int rc;

    agbindrec(g, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);
    GD_gvc(g) = gvc;
    if (g != agroot(g))
	GD_gvc(agroot(g)) = gvc;

    if ((p = agget(g, "layout"))) {
        gvc->layout.engine = NULL;
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

    gv_fixLocale (1);
    graph_init(g, gvc->layout.features->flags & LAYOUT_USES_RANKDIR);
    GD_drawing(agroot(g)) = GD_drawing(g);
    gv_initShapes ();
    if (gvle && gvle->layout) {
	gvle->layout(g);


	if (gvle->cleanup)
	    GD_cleanup(g) = gvle->cleanup;
    }
    gv_fixLocale (0);
    return 0;
}

/* gvFreeLayout:
 * Free layout resources.
 * First, if the graph has a layout-specific cleanup function attached,
 * use it and reset.
 * Then, if the root graph has not been cleaned up, clean it up and reset.
 * Only the root graph has GD_drawing non-null.
 */
int gvFreeLayout(GVC_t * gvc, Agraph_t * g)
{
    /* skip if no Agraphinfo_t yet */
    if (! agbindrec(g, "Agraphinfo_t", 0, TRUE))
	    return 0;

    if (GD_cleanup(g)) {
	(GD_cleanup(g))(g);
	GD_cleanup(g) = NULL;
    }
    
    if (GD_drawing(g)) {
	graph_cleanup(g);
    }
    return 0;
}
