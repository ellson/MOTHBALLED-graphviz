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

#include <stdio.h>

#include "geom.h"

#include "types.h"

#include "gvplugin_layout.h"

typedef enum { LAYOUT_DOT, } layout_type;

extern void dot_layout(graph_t * g);
extern void dot_cleanup(graph_t * g);

gvlayout_engine_t dotgen_engine = {
    dot_layout,
    dot_cleanup,
};

gvlayout_features_t dotgen_features = {
    LAYOUT_USES_RANKDIR,
};

gvplugin_installed_t gvlayout_dot_layout[] = {
    {LAYOUT_DOT, "dot", 0, &dotgen_engine, &dotgen_features},
    {0, NULL, 0, NULL, NULL}
};
