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

#include <stdio.h>
#include <string.h>

#include "const.h"
#include "types.h"
#include "macros.h"

#include "gvplugin_device.h"
#include "gvc.h"

int gvdevice_select(GVJ_t * job, char *str)
{
    GVC_t *gvc = job->gvc;
    gv_plugin_t *plugin;
    gvplugin_type_t *typeptr;
#ifndef DISABLE_CODEGENS
    codegen_info_t *cg_info;
#endif

    plugin = gvplugin_load(gvc, API_device, str);
    if (plugin) {
#ifndef DISABLE_CODEGENS
	if (strcmp(plugin->packagename, "cg") == 0) {
	    cg_info = (codegen_info_t *) (plugin->typeptr);
	    job->codegen = cg_info->cg;
	    return cg_info->id;
	} else {
#endif
	    typeptr = plugin->typeptr;
	    job->device_engine = (gvdevice_engine_t *) (typeptr->engine);
	    job->device_features =
		(gvdevice_features_t *) (typeptr->features);
	    job->device_id = typeptr->id;
	    return GVRENDER_PLUGIN;
#ifndef DISABLE_CODEGENS
	}
#endif
    }
    return NO_SUPPORT;
}

int gvdevice_features(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device_engine;
    int features = 0;

    if (gvde)
	features = job->device_features->flags;
    return features;
}

void gvdevice_finalize(GVC_t * gvc)
{
    GVJ_t *job = gvc->active_jobs;
    gvdevice_engine_t *gvde = job->device_engine;

    if (gvde) {
	if (gvde->finalize)
	    gvde->finalize(gvc);
    }
#ifndef DISABLE_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->reset)
	    cg->reset();
    }
#endif
}
