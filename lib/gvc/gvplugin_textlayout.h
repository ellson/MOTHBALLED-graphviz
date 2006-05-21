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

#ifndef GVTEXTLAYOUT_PLUGIN_H
#define GVTEXTLAYOUT_PLUGIN_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvtextlayout_engine_s {
	void (*textlayout) (textline_t *textline, char *fontname, double fontsize, char** fontpath);
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVTEXTLAYOUT_PLUGIN_H */
