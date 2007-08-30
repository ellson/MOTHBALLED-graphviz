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
#include "gvcjob.h"
#include "gvcint.h"
#include "gvcproc.h"

#if 0
/* This code is not used - see gvrender_select() in gvrender.c */

int gvdevice_select(GVC_t * gvc, char *str)
{
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
	    gvc->codegen = cg_info->cg;
	    return cg_info->id;
	} else {
#endif
	    typeptr = plugin->typeptr;
	    gvc->device.engine = (gvdevice_engine_t *) (typeptr->engine);
	    gvc->device.features =
		(gvdevice_features_t *) (typeptr->features);
	    gvc->device.id = typeptr->id;
	    return GVRENDER_PLUGIN;
#ifdef WITH_CODEGENS
	}
#endif
    }
    return NO_SUPPORT;
}

int gvdevice_features(GVC_t * gvc)
{
    gvdevice_engine_t *gvde = gvc->device.engine;
    int features = 0;

    if (gvde)
	features = gvc->device.features->flags;
    return features;
}

#endif

void gvdevice_initialize(GVJ_t * firstjob)
{
    gvdevice_engine_t *gvde = firstjob->device.engine;

    if (gvde && gvde->initialize)
	gvde->initialize(firstjob);
}

void gvdevice_prepare(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;

    if (gvde && gvde->prepare)
	gvde->prepare(job);
}

void gvdevice_format(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;

    if (gvde && gvde->format)
	gvde->format(job);
}

void gvdevice_finalize(GVJ_t * firstjob)
{
    gvdevice_engine_t *gvde = firstjob->device.engine;
    GVJ_t *job;

    if (gvde) {
	if (gvde->finalize) {
	    gvde->finalize(firstjob);
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = firstjob->codegen;

	if (cg && cg->reset)
	    cg->reset();
    }
#endif

    /* FIXME - file output should be its own device */
    for (job = firstjob; job; job = job->next_active) {
	if (job->output_filename
	  && job->output_file != stdout 
	  && ! job->external_context) {
	    if (job->output_file) {
	        fclose(job->output_file);
	        job->output_file = NULL;
	    }
            job->output_filename = NULL;
	}
    }
}
