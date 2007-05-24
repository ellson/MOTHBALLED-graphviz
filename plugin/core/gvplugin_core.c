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

#include "gvplugin.h"

extern gvplugin_installed_t gvrender_core_dot_types;
extern gvplugin_installed_t gvrender_core_ps_types;
extern gvplugin_installed_t gvrender_core_fig_types;
extern gvplugin_installed_t gvrender_core_svg_types;
extern gvplugin_installed_t gvrender_core_vml_types;
extern gvplugin_installed_t gvrender_core_map_types;
extern gvplugin_installed_t gvloadimage_core_types;

static gvplugin_api_t apis[] = {
    {API_render, &gvrender_core_dot_types},
    {API_render, &gvrender_core_ps_types},
    {API_render, &gvrender_core_fig_types},
    {API_render, &gvrender_core_svg_types},
    {API_render, &gvrender_core_vml_types},
    {API_render, &gvrender_core_map_types},
    {API_loadimage, &gvloadimage_core_types},
    {(api_t)0, 0},
};

gvplugin_library_t gvplugin_core_LTX_library = { "core", apis };
