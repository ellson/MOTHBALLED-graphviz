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

#ifndef GVTEXT_PLUGIN_H
#define GVTEXT_PLUGIN_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvtext_engine_s {
	int (*width) (textline_t *textline, char *fontname, double fontsize, char* fontpath);
    };

#ifdef __cplusplus
}
#endif
#endif				/* GVTEXT_PLUGIN_H */
