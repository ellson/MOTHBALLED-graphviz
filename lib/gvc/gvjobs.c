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

#include	"types.h"
#include	"gvc.h"

/* from common/utils.c */
extern void *zmalloc(size_t);

static gvrender_job_t *output_filename_job;
static gvrender_job_t *output_langname_job;

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
	    zmalloc(sizeof(gvrender_job_t));
    } else {
	if (!output_filename_job) {
	    output_filename_job = gvc->jobs;
	} else {
	    if (!output_filename_job->next) {
		output_filename_job->next =
		    zmalloc(sizeof(gvrender_job_t));
	    }
	    output_filename_job = output_filename_job->next;
	}
    }
    output_filename_job->output_filename = name;
}

/* -T switches */
boolean gvrender_output_langname_job(GVC_t * gvc, char *name)
{
    if (!gvc->jobs) {
	output_langname_job = gvc->job = gvc->jobs =
	    zmalloc(sizeof(gvrender_job_t));
    } else {
	if (!output_langname_job) {
	    output_langname_job = gvc->jobs;
	} else {
	    if (!output_langname_job->next) {
		output_langname_job->next =
		    zmalloc(sizeof(gvrender_job_t));
	    }
	    output_langname_job = output_langname_job->next;
	}
    }
    output_langname_job->output_langname = name;

    /* load it now to check that it exists */
    if (gvplugin_load(gvc, API_render, name))
	return TRUE;
    return FALSE;
}

#if 0
/* -R switches */
void gvrender_output_option_job(GVC_t * gvc, char *name, char *value)
{
}
#endif

gvrender_job_t *gvrender_first_job(GVC_t * gvc)
{
    return (gvc->job = gvc->jobs);
}

gvrender_job_t *gvrender_next_job(GVC_t * gvc)
{
    gvrender_job_t *job = gvc->job->next;

    if (job) {
	/* if langname not specified, then repeat previous value */
	if (!job->output_langname)
	    job->output_langname = gvc->job->output_langname;
	/* if filename not specified, then leave NULL to indicate stdout */
    }
    return (gvc->job = job);
}

void gvrender_delete_jobs(GVC_t * gvc)
{
    gvrender_job_t *job, *j;

    job = gvc->jobs;
    while ((j = job)) {
	job = job->next;
	free(j);
    }
    gvc->jobs = gvc->job = output_filename_job = output_langname_job =
	NULL;
}
