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
 *  text_layout engine wrapper
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "const.h"
#include "types.h"
#include "macros.h"

#include "gvplugin_text.h"
#include "gvc.h"

#if 0
int gvlayout_select(GVC_t * gvc, char *layout)
{
    gv_plugin_t *plugin;
    gvplugin_type_t *typeptr;

    plugin = gvplugin_load(gvc, API_layout, layout);
    if (plugin) {
	typeptr = plugin->typeptr;
	gvc->layout_type = typeptr->type;
	gvc->layout_engine = (gvlayout_engine_t *) (typeptr->engine);
	gvc->layout_id = typeptr->id;
	return GVRENDER_PLUGIN;
    }
    return NO_SUPPORT;
}
#endif

double gvtext_width(GVC_t *gvc, textline_t *textline, char *fontname, double fontsize, char *fontpath)
{
    gvtext_engine_t *gvte = gvc->text_engine;
    int rc;

    if (gvte && gvte->width) {
	rc = gvte->width(textline, fontname, fontsize, fontpath);
    }
    return textline->width;
}
