/* $Id: dot_builtins.c,v 1.6 2011/01/25 16:30:46 ellson Exp $ $Revision: 1.6 $ */
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gvplugin.h"
#include "gvc.h"

#if defined(GVDLL) && !defined(ENABLE_LTDL)
#define extern
#endif

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
extern gvplugin_library_t gvplugin_neato_layout_LTX_library;
extern gvplugin_library_t gvplugin_core_LTX_library;

lt_symlist_t lt_preloaded_symbols[] = {
    { "gvplugin_dot_layout_LTX_library", (void*)(&gvplugin_dot_layout_LTX_library) },
    //{ "gvplugin_neato_layout_LTX_library", (void*)(&gvplugin_neato_layout_LTX_library) },
    //{ "gvplugin_core_LTX_library", (void*)(&gvplugin_core_LTX_library) },
    { 0, 0 }
};


/// NOTE: Here's the entry point: Call this instead of gvContext() in your application!
GVC_t* gvContextWithStaticPlugins()
{
    return gvContextPlugins(lt_preloaded_symbols, 0);
}
