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
    A gvcontext is a single instance of a GVC_t data structure providing
    for a set of plugins for processing one graph at a time, and a job
    description provividing for a sequence of graph jobs.

    Sometime in the future it may become the basis for a thread.
 */

#include "config.h"

#include <stdlib.h>

#include "builddate.h"
#include "types.h"
#include "gvplugin.h"
#include "gvcjob.h"
#include "gvcint.h"
#include "gvcproc.h"
#include "gvc.h"

/* from common/utils.c */
extern void *zmalloc(size_t);

/* from common/textspan.c */
extern void textfont_dict_close(GVC_t *gvc);

/* from common/emit.c */
extern void emit_once_reset(void);

/* from common/globals.c */
extern int graphviz_errors;

static char *LibInfo[] = {
    "graphviz",         /* Program */
    PACKAGE_VERSION,   /* Version */
    BUILDDATE           /* Build Date */
};

GVC_t *gvNEWcontext(const lt_symlist_t *builtins, int demand_loading)
{
    GVC_t *gvc = zmalloc(sizeof(GVC_t));

    if (gvc) {
	gvc->common.info = LibInfo;
	gvc->common.errorfn = agerrorf;
	gvc->common.builtins = builtins;
	gvc->common.demand_loading = demand_loading;
    }
    return gvc;
}

void gvFinalize(GVC_t * gvc)
{
    if (gvc->active_jobs)
	gvrender_end_job(gvc->active_jobs);
}


int gvFreeContext(GVC_t * gvc)
{
    GVG_t *gvg, *gvg_next;
    gvplugin_package_t *package, *package_next;
    gvplugin_available_t *api, *api_next;

#define ELEM(x) +1
    /* See gvcext.h for APIS and gvcint.h for an example usage of "+1"
       to get the number of APIs. */
    unsigned int num_apis = APIS, i;
#undef ELEM

    emit_once_reset();
    gvg_next = gvc->gvgs;
    while ((gvg = gvg_next)) {
	gvg_next = gvg->next;
	free(gvg);
    }
    package_next = gvc->packages;
    while ((package = package_next)) {
	package_next = package->next;
	free(package->path);
	free(package->name);
	free(package);
    }
    gvjobs_delete(gvc);
    if (gvc->config_path)
	free(gvc->config_path);
    if (gvc->input_filenames)
	free(gvc->input_filenames);
    textfont_dict_close(gvc);
    for (i = 0; i != num_apis; ++i) {
	for (api = gvc->apis[i]; api != NULL; api = api_next) {
	    api_next = api->next;
	    free(api);
	}
    }
    free(gvc);
    return (graphviz_errors + agerrors());
}

GVC_t* gvCloneGVC (GVC_t * gvc0)
{
    GVC_t *gvc = zmalloc(sizeof(GVC_t));

    gvc->common = gvc0->common;
    memcpy (&gvc->apis, &gvc0->apis, sizeof(gvc->apis));
    memcpy (&gvc->api, &gvc0->api, sizeof(gvc->api));
    gvc->packages = gvc->packages;
    
    return gvc;
}

void gvFreeCloneGVC (GVC_t * gvc)
{
    gvjobs_delete(gvc);
    free(gvc);
}

