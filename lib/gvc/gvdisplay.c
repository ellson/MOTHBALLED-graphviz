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
 *  display engine wrapper
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "const.h"
#include "types.h"
#include "macros.h"

#include "gvplugin_display.h"
#include "gvc.h"

int gvdisplay_select(GVC_t * gvc, char *display)
{
    gv_plugin_t *plugin;
    gvplugin_type_t *typeptr;

    plugin = gvplugin_load(gvc, API_display, display);
    if (plugin) {
	typeptr = plugin->typeptr;
	gvc->display_engine = (gvdisplay_engine_t *) (typeptr->engine);
	return GVRENDER_PLUGIN; /* FIXME - need more suitable success code */
    }
    return NO_SUPPORT;
}

void gvdisplay_init(GVC_t *gvc)
{
    gvdisplay_engine_t *gvde = gvc->display_engine;

    if (gvde && gvde->init) {
	gvde->init(gvc);
    }
}

void gvdisplay_process(GVC_t *gvc)
{
    gvdisplay_engine_t *gvde = gvc->display_engine;

    if (gvde && gvde->process) {
	gvde->process(gvc);
    }
}

void gvdisplay_deinit(GVC_t *gvc)
{
    gvdisplay_engine_t *gvde = gvc->display_engine;

    if (gvde && gvde->deinit) {
	gvde->deinit(gvc);
    }
}
