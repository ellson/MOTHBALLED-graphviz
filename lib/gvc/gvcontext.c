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

#include        <stdio.h>
#include        <stdlib.h>

#include	"config.h"
#include	"types.h"
#include	"macros.h"
#include	"graph.h"
#include	"gvc.h"

/* from common/utils.c */
extern void *zmalloc(size_t);

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

void gvFREEcontext(GVC_t * gvc)
{
    gvrender_delete_jobs(gvc);
    free(gvc);
}
