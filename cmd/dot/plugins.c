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

#include        <stdio.h>

#include        "config.h"
#include        "types.h"
#include        "macros.h"
#include        "gvrender.h"

#ifndef DISABLE_GVRENDER
extern gvplugin_t gvplugin_cairo_LTX_plugin,
    gvplugin_gd_LTX_plugin,
    gvplugin_text_LTX_plugin;
#endif
extern gvplugin_layout_LTX_plugin;

gvplugin_t *builtins[] = {
#ifndef DISABLE_GVRENDER
    &gvplugin_cairo_LTX_plugin,
    &gvplugin_gd_LTX_plugin,
    &gvplugin_text_LTX_plugin,
#endif
    &gvplugin_layout_LTX_plugin,
    NULL
};

