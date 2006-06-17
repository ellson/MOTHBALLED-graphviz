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

#include <stdlib.h>

#include "gvplugin_loadimage.h"

extern void svggen_fputs(GVJ_t * job, char *s);
extern void svggen_printf(GVJ_t * job, const char *format, ...);

typedef enum {
    FORMAT_PNG_SVG, FORMAT_GIF_SVG, FORMAT_JPEG_SVG,
    FORMAT_PS_PS,
} format_type;

static void core_loadimage_svg(GVJ_t * job, usershape_t *us, boxf b, bool filled)
{
    if (us->name) {
        svggen_fputs(job, "<image xlink:href=\"");
        svggen_fputs(job, us->name);
        if (job->rotation) {
            svggen_printf (job, "\" width=\"%gpx\" height=\"%gpx\" preserveAspectRatio=\"xMidYMid meet\" x=\"%g\" y=\"%g\"",
                b.UR.y - b.LL.y, b.UR.x - b.LL.x, b.LL.x, b.UR.y);
            svggen_printf (job, " transform=\"rotate(%d %g %g)\"",
                job->rotation, b.LL.x, b.UR.y);
        }
        else {
            svggen_printf (job, "\" width=\"%gpx\" height=\"%gpx\" preserveAspectRatio=\"xMidYMid meet\" x=\"%g\" y=\"%g\"",
                b.UR.x - b.LL.x, b.UR.y - b.LL.y, b.LL.x, b.LL.y);
        }
        svggen_fputs(job, "/>\n");
    }
}

static void core_loadimage_ps(GVJ_t * job, usershape_t *us, boxf b, bool filled)
{
    if (us->name) {
    }
}

static gvloadimage_engine_t engine_svg = {
    core_loadimage_svg
};

static gvloadimage_engine_t engine_ps = {
    core_loadimage_ps
};

gvplugin_installed_t gvloadimage_core_types[] = {
    {FORMAT_PNG_SVG, "png2svg", 1, &engine_svg, NULL},
    {FORMAT_GIF_SVG, "gif2svg", 1, &engine_svg, NULL},
    {FORMAT_JPEG_SVG, "jpeg2svg", 1, &engine_svg, NULL},
    {FORMAT_PS_PS, "ps2ps", 1, &engine_ps, NULL},
    {0, NULL, 0, NULL, NULL}
};
