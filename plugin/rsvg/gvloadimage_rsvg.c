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

#ifdef HAVE_PANGOCAIRO
#ifdef HAVE_RSVG
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

typedef enum {
    FORMAT_SVG_CAIRO,
} format_type;

static cairo_status_t
reader (void *closure, unsigned char *data, unsigned int length)
{
    if (length == fread(data, 1, length, (FILE *)closure)
     || feof((FILE *)closure))
        return CAIRO_STATUS_SUCCESS;
    return CAIRO_STATUS_READ_ERROR;
}

static void cairo_freeimage(usershape_t *us)
{
    cairo_destroy((cairo_t*)us->data);
}

static cairo_surface_t* cairo_loadimage(GVJ_t * job, usershape_t *us)
{
    cairo_surface_t *surface = NULL; /* source surface */

    assert(job);
    assert(us);
    assert(us->name);
    assert(us->f);

    if (us->data) {
        if (us->datafree == cairo_freeimage)
             surface = (cairo_surface_t*)(us->data); /* use cached data */
        else {
             us->datafree(us);        /* free incompatible cache data */
             us->data = NULL;
        }
    }
    if (!surface) { /* read file into cache */
        fseek(us->f, 0, SEEK_SET);
        switch (us->type) {
#ifdef CAIRO_HAS_PNG_FUNCTIONS
            case FT_PNG:
                surface = cairo_image_surface_create_from_png_stream(reader, us->f);
                cairo_surface_reference(surface);
                break;
#endif
            default:
                surface = NULL;
        }
        if (surface) {
            us->data = (void*)surface;
            us->datafree = cairo_freeimage;
        }
    }
    return surface;
}

static void rsvg_loadimage_cairo(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    cairo_t *cr = (cairo_t *) job->context; /* target context */
    cairo_surface_t *surface;	 /* source surface */

    surface = cairo_loadimage(job, us);
    if (surface) {
        cairo_save(cr);
        cairo_translate(cr, ROUND(b.LL.x), ROUND(-b.UR.y));
        cairo_scale(cr, (b.UR.x - b.LL.x) / us->w,
                        (b.UR.y - b.LL.y) / us->h);
        cairo_set_source_surface (cr, surface, 0, 0);
        cairo_paint (cr);
        cairo_restore(cr);
    }
}

static gvloadimage_engine_t engine_cairo = {
    rsvg_loadimage_cairo
};
#endif
#endif

gvplugin_installed_t gvloadimage_rsvg_types[] = {
#ifdef HAVE_PANGOCAIRO
#ifdef HAVE_RSVG
    {FORMAT_SVG_CAIRO, "svg:cairo", 1, &engine_cairo, NULL},
#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
