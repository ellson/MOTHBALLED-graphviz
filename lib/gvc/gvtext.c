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

int gvtext_select(GVC_t * gvc, char *text_layout)
{
    gv_plugin_t *plugin;
    gvplugin_type_t *typeptr;

    plugin = gvplugin_load(gvc, API_text, text_layout);
    if (plugin) {
	typeptr = plugin->typeptr;
	gvc->layout_engine = (gvlayout_engine_t *) (typeptr->engine);
	return GVRENDER_PLUGIN;  /* FIXME - need more suitable success code */
    }
    return NO_SUPPORT;
}

double gvtext_width(GVC_t *gvc, textline_t *textline, char *fontname, double fontsize, char *fontpath)
{
    gvtext_engine_t *gvte = gvc->text_engine;
    boolean rc;

    if (gvte && gvte->width) {
	rc = gvte->width(textline, fontname, fontsize, fontpath);
    }
    return textline->width;
}
