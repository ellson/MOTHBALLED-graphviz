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

extern gvplugin_installed_t gvlayout_neato_types[];

static gvplugin_api_t apis[] = {
    {API_layout, gvlayout_neato_types},
    {(api_t)0, 0},
};

#ifdef _WIN32
#   define GVPLUGIN_NEATO_LAYOUT_API __declspec(dllexport)
#else
#   define GVPLUGIN_NEATO_LAYOUT_API
#endif

GVPLUGIN_NEATO_LAYOUT_API gvplugin_library_t gvplugin_neato_layout_LTX_library = { "neato_layout", apis };

