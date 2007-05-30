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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gvplugin.h"

#if defined(GVDLL) && !defined(ENABLE_LTDL)
#define extern	__declspec(dllimport)
#endif

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
extern gvplugin_library_t gvplugin_neato_layout_LTX_library;
#ifdef HAVE_LIBGD
extern gvplugin_library_t gvplugin_gd_LTX_library;
#endif
#ifdef HAVE_PANGOCAIRO
extern gvplugin_library_t gvplugin_pango_LTX_library;
#endif
extern gvplugin_library_t gvplugin_core_LTX_library;

#if defined(GVDLL) && !defined(ENABLE_LTDL)
lt_symlist_t lt_preloaded_symbols[] = {
	{ "gvplugin_dot_layout_LTX_library", 0},
	{ "gvplugin_neato_layout_LTX_library", 0},
#ifdef HAVE_PANGOCAIRO
	{ "gvplugin_pango_LTX_library", 0},
#endif
#ifdef HAVE_LIBGD
	{ "gvplugin_gd_LTX_library", 0},
#endif
	{ "gvplugin_core_LTX_library", 0},
	{ 0, 0 }
};

static void*
lt_lookup (const char* name) 
{
    void* addr = 0;
    switch (*(name + 9)) { /* skip "gvplugin_" */
    case 'c' :
	addr = &gvplugin_core_LTX_library;
	break;
    case 'd' :
	addr = &gvplugin_dot_layout_LTX_library;
	break;
#ifdef HAVE_LIBGD
    case 'g' :
	addr = &gvplugin_gd_LTX_library;
	break;
#endif
    case 'n' :
	addr = &gvplugin_neato_layout_LTX_library;
	break;
#ifdef HAVE_PANGOCAIRO
    case 'p' :
	addr = &gvplugin_pango_LTX_library;
	break;
#endif
    }
    return addr;
}

void
init_lt_preloaded_symbols()
{
    const char* s;
    int i;
    for (i = 0; (s = lt_preloaded_symbols[i].name); i++) {
	lt_preloaded_symbols[0].address = lt_lookup (s);
    }
}
#else
const lt_symlist_t lt_preloaded_symbols[] = {
	{ "gvplugin_dot_layout_LTX_library", (void*)(&gvplugin_dot_layout_LTX_library) },
	{ "gvplugin_neato_layout_LTX_library", (void*)(&gvplugin_neato_layout_LTX_library) },
#ifdef HAVE_PANGOCAIRO
	{ "gvplugin_pango_LTX_library", (void*)(&gvplugin_pango_LTX_library) },
#endif
#ifdef HAVE_LIBGD
	{ "gvplugin_gd_LTX_library", (void*)(&gvplugin_gd_LTX_library) },
#endif
	{ "gvplugin_core_LTX_library", (void*)(&gvplugin_core_LTX_library) },
	{ 0, 0 }
};
#endif
