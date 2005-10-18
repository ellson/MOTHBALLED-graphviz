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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include	<stdio.h>
#include	<stdlib.h>

#include	"logic.h"
#include	"geom.h"
#include	"types.h"
#include        "gvplugin.h"
#include        "gvcint.h"
#include        "gvcproc.h"

/* from common/utils.c */
extern void *zmalloc(size_t);

static GVJ_t *output_filename_job;
static GVJ_t *output_langname_job;

/*
 * -T and -o can be specified in any order relative to the other, e.g.
 *            -T -T -o -o
 *            -T -o -o -T
 * The first -T is paired with the first -o, the second with the second, and so on.
 *
 * If there are more -T than -o, then the last -o is repeated for the remaining -T
 * and vice-versa
 *
 * If there are no -T or -o then a single job is instantiated.
 *
 * If there is no -T on the first job, then "dot" is used.
 *
 * As many -R as are specified before a completed -T -o pair (according to the above rules)
 * are used as renderer-specific switches for just that one job.  -R must be restated for 
 * each job.
 */

/* -o switches */
void gvrender_output_filename_job(GVC_t * gvc, char *name)
{
    if (!gvc->jobs) {
	output_filename_job = gvc->job = gvc->jobs =
	    zmalloc(sizeof(GVJ_t));
    } else {
	if (!output_filename_job) {
	    output_filename_job = gvc->jobs;
	} else {
	    if (!output_filename_job->next) {
		output_filename_job->next =
		    zmalloc(sizeof(GVJ_t));
	    }
	    output_filename_job = output_filename_job->next;
	}
    }
    output_filename_job->output_filename = name;
    output_filename_job->gvc = gvc;
}

/* -T switches */
boolean gvrender_output_langname_job(GVC_t * gvc, char *name)
{
    if (!gvc->jobs) {
	output_langname_job = gvc->job = gvc->jobs =
	    zmalloc(sizeof(GVJ_t));
    } else {
	if (!output_langname_job) {
	    output_langname_job = gvc->jobs;
	} else {
	    if (!output_langname_job->next) {
		output_langname_job->next =
		    zmalloc(sizeof(GVJ_t));
	    }
	    output_langname_job = output_langname_job->next;
	}
    }
    output_langname_job->output_langname = name;
    output_langname_job->gvc = gvc;

    /* load it now to check that it exists */
    if (gvplugin_load(gvc, API_render, name))
	return TRUE;
    return FALSE;
}

GVJ_t *gvrender_first_job(GVC_t * gvc)
{
    return (gvc->job = gvc->jobs);
}

GVJ_t *gvrender_next_job(GVC_t * gvc)
{
    GVJ_t *job = gvc->job->next;

    if (job) {
	/* if langname not specified, then repeat previous value */
	if (!job->output_langname)
	    job->output_langname = gvc->job->output_langname;
	/* if filename not specified, then leave NULL to indicate stdout */
    }
    return (gvc->job = job);
}

/* FIXME - gv_argvlist_set_item and gv_argvlist_free should be in a utilities sourcefile */
static void gv_argvlist_free(gv_argvlist_t *list)
{
    if (list->argv)
	free(list->argv);
    list->argv = NULL;
    list->alloc = 0;
    list->argc = 0;
}

void gvrender_delete_jobs(GVC_t * gvc)
{
    GVJ_t *job, *j;

    job = gvc->jobs;
    while ((j = job)) {
	job = job->next;
	gv_argvlist_free(&(j->selected_obj_attributes));
	gv_argvlist_free(&(j->selected_obj_type_name));
	if (j->active_tooltip)
	    free(j->active_tooltip);
	if (j->selected_href)
	    free(j->selected_href);
	free(j);
    }
    gvc->jobs = gvc->job = output_filename_job = output_langname_job =
	NULL;
}
