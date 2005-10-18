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
 *  usershape engine wrapper
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifndef HAVE_STRCASECMP
    extern int strcasecmp(const char *s1, const char *s2);
#endif
#ifndef HAVE_STRNCASECMP
    extern int strncasecmp(const char *s1, const char *s2, unsigned int n);
#endif

#include <stdio.h>

#include "geom.h"

#include "macros.h"
#include "const.h"
#include "types.h"
#include "graph.h"

#include "gvplugin_usershape.h"
#include "gvcint.h"
#include "gvcproc.h"

extern point gd_image_size(graph_t * g, char *shapeimagefile);
extern point ps_image_size(graph_t * g, char *shapeimagefile);
extern point svg_image_size(graph_t * g, char *shapeimagefile);
extern point quartz_image_size(graph_t * g, char *shapeimagefile);

int gvusershape_select(GVC_t * gvc, char *usershape)
{
    gvplugin_available_t *plugin;
    gvplugin_installed_t *typeptr;

    plugin = gvplugin_load(gvc, API_usershape, usershape);
    if (plugin) {
	typeptr = plugin->typeptr;
	gvc->usershape.engine = (gvusershape_engine_t *) (typeptr->engine);
	return GVRENDER_PLUGIN;  /* FIXME - need more suitable success code */
    }
    return NO_SUPPORT;
}

/*
 * This routine extracts the usershape size from known filetypes,
 * it does not check that the output renderer knows how to render
 * this file type
 * Returns the size required for the shape in points;
 * returns (-1,-1) on error;
 * returns (0,0) to indicate "don't care". For example, in
 * postscript, a node can have a user-defined shape but no shapefile.
 */
point image_size(graph_t * g, char *shapefile)
{
    char *suffix;
    point rv;

    /* no shape file, no shape size */
    if (!shapefile) {
        rv.x = rv.y = -1;
        return rv;
    }
    if (*shapefile == '\0') {
        rv.x = rv.y = 0;
        return rv;              /* no shapefile; probably postscript custom */
    }
    if (!strncasecmp(shapefile, "http://", 7)) {
        rv.x = rv.y = 0;
        return rv;              /* punt on obvious web addresses */
    }

    suffix = strrchr(shapefile, '.');
    if (!suffix)
        suffix = shapefile;
    else
        suffix++;
    if (!strcasecmp(suffix, "wbmp")
#ifdef HAVE_GD_GIF
        || !strcasecmp(suffix, "gif")
#endif
#ifdef HAVE_GD_JPEG
        || !strcasecmp(suffix, "jpeg") || !strcasecmp(suffix, "jpg")
#endif
#ifdef HAVE_GD_PNG
        || !strcasecmp(suffix, "png")
#endif
#ifdef HAVE_GD_XPM
        || !strcasecmp(suffix, "xbm")
/* FIXME - is "xpm" also supported by gd for input ? */
#endif
        ) {
        rv = gd_image_size(g, shapefile);
    } else
#ifdef QUARTZ_RENDER
    if (Output_lang == QPDF || Output_lang == QEPDF ||
            (Output_lang >= QBM_FIRST && Output_lang <= QBM_LAST))
        return quartz_image_size(g, shapefile);
    else
#endif

    if (!strcasecmp(suffix, "ps")) {
        rv = ps_image_size(g, shapefile);
    } else if (!strcasecmp(suffix, "svg")) {
        rv = svg_image_size(g, shapefile);
    } else {
        agerr(AGERR, "shapefile \"%s\" suffix not recognized\n",
              shapefile);
        rv.x = rv.y = -1;
    }
    return rv;
}

