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
    A gvcontext is a single instance of a GVC_t data structure providing
    for a set of plugins for processing one graph at a time, and a job
    description provividing for a sequence of graph jobs.

    Sometime in the future it may become the basis for a thread.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include        <stdio.h>
#include        <stdlib.h>

#include	"logic.h"
#include	"geom.h"
#include	"macros.h"
#include	"types.h"
#include	"graph.h"
#include        "gvplugin.h"
#include        "gvcint.h"
#include        "gvcproc.h"

/* from common/utils.c */
extern void *zmalloc(size_t);

/* from common/emit.c */
extern void emit_jobs_eof(GVC_t *gvc);

/* from common/globals.c */
extern int graphviz_errors;

GVC_t *gvNEWcontext(char **info, char *user)
{
    GVC_t *gvc = zmalloc(sizeof(GVC_t));

    if (gvc) {
	gvc->info = info;
	gvc->user = user;
	gvc->errorfn = agerrorf;
    }
    return gvc;
}

int gvFreeContext(GVC_t * gvc)
{
    if (gvc->active_jobs)
	gvdevice_finalize(gvc);
    emit_jobs_eof(gvc);
    gvrender_delete_jobs(gvc);
    if (gvc->config_path)
	free(gvc->config_path);
    free(gvc);
    return (graphviz_errors + agerrors());
}
