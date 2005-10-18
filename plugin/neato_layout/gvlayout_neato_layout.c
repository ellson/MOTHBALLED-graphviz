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

/*
 *  neato layout plugin
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "logic.h"
#include "geom.h"
#include "macros.h"
#include "const.h"
#include "types.h"

#include "gvplugin_layout.h"

typedef enum { LAYOUT_NEATO,
		LAYOUT_FDP,
		LAYOUT_TWOPI,
		LAYOUT_CIRCO,
	} layout_type;

extern void neato_layout(graph_t * g);
extern void fdp_layout(graph_t * g);
extern void twopi_layout(graph_t * g);
extern void circo_layout(graph_t * g);

extern void neato_cleanup(graph_t * g);
extern void fdp_cleanup(graph_t * g);
extern void twopi_cleanup(graph_t * g);
extern void circo_cleanup(graph_t * g);

gvlayout_engine_t neatogen_engine = {
    neato_layout,
    neato_cleanup,
};

gvlayout_engine_t fdpgen_engine = {
    fdp_layout,
    fdp_cleanup,
};

gvlayout_engine_t twopigen_engine = {
    twopi_layout,
    twopi_cleanup,
};

gvlayout_engine_t circogen_engine = {
    circo_layout,
    circo_cleanup,
};

gvlayout_features_t neatogen_features = {
        0,
};

gvplugin_installed_t gvlayout_neato_types[] = {
    {LAYOUT_NEATO, "neato", 0, &neatogen_engine, &neatogen_features},
    {LAYOUT_FDP, "fdp", 0, &fdpgen_engine, &neatogen_features},
    {LAYOUT_TWOPI, "twopi", 0, &twopigen_engine, &neatogen_features},
    {LAYOUT_CIRCO, "circo", 0, &circogen_engine, &neatogen_features},
    {0, NULL, 0, NULL, NULL}
};
