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
 *  graphics code generator wrapper
 *
 *  This library forms the socket for run-time loadable device plugins.  
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "const.h"
#include "gvplugin_device.h"
#include "gvcint.h"
#include "gvcproc.h"

#if 0
/* This code is not used - see gvrender_select() in gvrender.c */

int gvdevice_select(GVJ_t * job, char *str)
{
    GVC_t *gvc = job->gvc;
    gvplugin_available_t *plugin;
    gvplugin_installed_t *typeptr;
#ifdef WITH_CODEGENS
    codegen_info_t *cg_info;
#endif

    plugin = gvplugin_load(gvc, API_device, str);
    if (plugin) {
#ifdef WITH_CODEGENS
	if (strcmp(plugin->packagename, "cg") == 0) {
	    cg_info = (codegen_info_t *) (plugin->typeptr);
	    job->codegen = cg_info->cg;
	    return cg_info->id;
	} else {
#endif
	    typeptr = plugin->typeptr;
	    job->device.engine = (gvdevice_engine_t *) (typeptr->engine);
	    job->device.features =
		(gvdevice_features_t *) (typeptr->features);
	    job->device.id = typeptr->id;
	    return GVRENDER_PLUGIN;
#ifdef WITH_CODEGENS
	}
#endif
    }
    return NO_SUPPORT;
}

int gvdevice_features(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;
    int features = 0;

    if (gvde)
	features = job->device.features->flags;
    return features;
}

#endif

void gvdevice_finalize(GVC_t * gvc)
{
    GVJ_t *active_job = gvc->active_jobs;
    gvdevice_engine_t *gvde = active_job->device.engine;

    if (gvde) {
	if (gvde->finalize) {
	    gvde->finalize(active_job);
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = active_job->codegen;

	if (cg && cg->reset)
	    cg->reset();
    }
#endif

    /* FIXME - file output should be its own device */
    while(active_job) {
	if (active_job->output_file != stdout) {
	    fclose(active_job->output_file);
	    active_job->output_file = NULL;
	}
        active_job->output_filename = NULL;
	active_job = active_job->next_active;
    }
}
