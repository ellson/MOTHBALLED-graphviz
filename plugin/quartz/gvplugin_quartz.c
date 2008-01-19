/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2008 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "gvplugin.h"

extern gvplugin_installed_t gvrender_quartz_types;
// extern gvplugin_installed_t gvtextlayout_quartz_types;
// extern gvplugin_installed_t gvloadimage_quartz_types;
extern gvplugin_installed_t gvdevice_quartz_types;

static gvplugin_api_t apis[] = {
    {API_render, &gvrender_quartz_types},
  //  {API_textlayout, &gvtextlayout_quartz_types},
  //  {API_loadimage, &gvloadimage_quartz_types},
    {API_device, &gvdevice_quartz_types},
    {(api_t)0, 0},
};

gvplugin_library_t gvplugin_quartz_LTX_library = { "quartz", apis };
