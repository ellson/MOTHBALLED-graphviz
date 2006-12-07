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
 *  This library forms the socket for run-time loadable loadimage plugins.  
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "const.h"
#include "gvplugin_loadimage.h"
#include "gvcint.h"
#include "gvcproc.h"

/* for agerr() */
#include "graph.h"

int gvloadimage_select(GVJ_t * job, char *str)
{
    gvplugin_available_t *plugin;
    gvplugin_installed_t *typeptr;

    plugin = gvplugin_load(job->gvc, API_loadimage, str);
    if (plugin) {
        typeptr = plugin->typeptr;
        job->loadimage.engine = (gvloadimage_engine_t *) (typeptr->engine);
        job->loadimage.id = typeptr->id;
        return GVRENDER_PLUGIN;
    }
    return NO_SUPPORT;
}

void gvloadimage(GVJ_t * job, usershape_t *us, boxf b, boolean filled, char *target)
{
    gvloadimage_engine_t *gvli;
    char type[SMALLBUF];

    strcpy(type, us->stringtype);
    strcat(type, "2");
    strcat(type, target);

    if (gvloadimage_select(job, type) == NO_SUPPORT)
	    agerr (AGWARN, "No loadimage plugin for \"%s\"\n", type);

    if ((gvli = job->loadimage.engine) && gvli->loadimage)
	gvli->loadimage(job, us, b, filled);
}
