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

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif
#include <stdlib.h>
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

static void auto_output_filename(GVJ_t *job)
{
    static char *buf;
    static int bufsz;
    char gidx[100];  /* large enough for '.' plus any integer */
    char *fn, *p;
    int len;

    if (job->graph_index)
        sprintf(gidx, ".%d", job->graph_index + 1);
    else
        gidx[0] = '\0';
    if (!(fn = job->input_filename))
        fn = "noname.dot";
    len = strlen(fn)                    /* typically "something.dot" */
        + strlen(gidx)                  /* "", ".2", ".3", ".4", ... */
        + 1                             /* "." */
        + strlen(job->output_langname)  /* e.g. "png" */
        + 1;                            /* null terminaor */
    if (bufsz < len) {
            bufsz = len + 10;
            buf = realloc(buf, bufsz * sizeof(char));
    }
    strcpy(buf, fn);
    strcat(buf, gidx);
    strcat(buf, ".");
    if ((p = strchr(job->output_langname, ':'))) {
        strcat(buf, p+1);
        strcat(buf, ".");
        strncat(buf, job->output_langname, (p - job->output_langname));
    }
    else {
        strcat(buf, job->output_langname);
    }

    job->output_filename = buf;
}

#ifdef WITH_CODEGENS
extern FILE* Output_file;
#endif

void gvdevice_initialize(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;
    GVC_t *gvc = job->gvc;

    if (gvde && gvde->initialize) {
	gvde->initialize(job);
    }
    else {
        /* if the device has now initialization then it uses file output */
        if (!job->output_file) {        /* if not yet opened */
            if (gvc->common.auto_outfile_names)
                 auto_output_filename(job);
            if (job->output_filename) {
                 job->output_file = fopen(job->output_filename, "w");
                 if (job->output_file == NULL) {
                     perror(job->output_filename);
                     exit(1);
                 }
            }
            else
                 job->output_file = stdout;

#ifdef WITH_CODEGENS
            Output_file = job->output_file;
#endif

#ifdef HAVE_SETMODE
#ifdef O_BINARY
            if (job->flags & GVDEVICE_BINARY_FORMAT)
                setmode(fileno(job->output_file), O_BINARY);
#endif
#endif
        }
    }
}

void gvdevice_format(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;

    if (gvde && gvde->format)
	gvde->format(job);
    if (job->output_file && ! job->external_context && job->output_lang != TK)
	fflush(job->output_file);
}

void gvdevice_finalize(GVJ_t * firstjob)
{
    gvdevice_engine_t *gvde = firstjob->device.engine;
    GVJ_t *job;
    boolean finalized_p = FALSE;

    if (gvde) {
	if (gvde->finalize) {
	    gvde->finalize(firstjob);
	    finalized_p = TRUE;
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = firstjob->codegen;

	if (cg && cg->reset)
	    cg->reset();
    }
#endif

    if (! finalized_p) {
        /* if the device has no finalization then it uses file output */
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
}
