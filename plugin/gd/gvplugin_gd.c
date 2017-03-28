/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include "gvplugin.h"

extern gvplugin_installed_t gvrender_gd_types[];
extern gvplugin_installed_t gvrender_vrml_types[];
extern gvplugin_installed_t gvtextlayout_gd_types[];
extern gvplugin_installed_t gvloadimage_gd_types[];
extern gvplugin_installed_t gvdevice_gd_types[];
extern gvplugin_installed_t gvdevice_gd_types2[];
extern gvplugin_installed_t gvdevice_vrml_types[];

static gvplugin_api_t apis[] = {
    {API_render, gvrender_gd_types},
    {API_render, gvrender_vrml_types},
    {API_textlayout, gvtextlayout_gd_types},
    {API_loadimage, gvloadimage_gd_types},
    {API_device, gvdevice_gd_types},
    {API_device, gvdevice_gd_types2},
    {API_device, gvdevice_vrml_types},
    {(api_t)0, 0},
};

#ifdef _WIN32
#   define GVPLUGIN_GD_API __declspec(dllexport)
#else
#   define GVPLUGIN_GD_API
#endif

GVPLUGIN_GD_API gvplugin_library_t gvplugin_gd_LTX_library = { "gd", apis };
