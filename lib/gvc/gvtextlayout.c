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
 *  textlayout engine wrapper
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "const.h"
#include "types.h"
#include "gvplugin_textlayout.h"
#include "gvcint.h"
#include "gvcproc.h"

int gvtextlayout_select(GVC_t * gvc)
{
    gvplugin_available_t *plugin;
    gvplugin_installed_t *typeptr;

    plugin = gvplugin_load(gvc, API_textlayout, "textlayout");
    if (plugin) {
	typeptr = plugin->typeptr;
	gvc->textlayout.engine = (gvtextlayout_engine_t *) (typeptr->engine);
	return GVRENDER_PLUGIN;  /* FIXME - need more suitable success code */
    }
    return NO_SUPPORT;
}

bool gvtextlayout(GVC_t *gvc, textpara_t *para, char **fontpath)
{
    gvtextlayout_engine_t *gvte = gvc->textlayout.engine;

    if (gvte && gvte->textlayout) {
	gvte->textlayout(para, fontpath);
	return TRUE;
    }
    return FALSE;
}
