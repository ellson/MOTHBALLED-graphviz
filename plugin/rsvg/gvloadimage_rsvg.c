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

#ifdef WIN32
#define NUL_FILE "nul"
#else
#define NUL_FILE "/dev/null"
#endif

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

static RsvgHandle* cairo_loadimage(GVJ_t * job, usershape_t *us)
{
    RsvgHandle* rsvgh = NULL;

    assert(job);
    assert(us);
    assert(us->name);
    assert(us->f);

    if (us->data) {
        if (us->datafree == cairo_freeimage)
             rsvgh = (RsvgHandle*)(us->data); /* use cached data */
        else {
             us->datafree(us);        /* free incompatible cache data */
             us->data = NULL;
        }

    }

    if (!rsvgh) { /* read file into cache */
        switch (us->type) {
            case FT_SVG:

		rsvg_init();
       		rsvgh = rsvg_handle_new_from_file(us->name, NULL);
		
		if (rsvgh == NULL) {
			rsvg_term();
		}
	
		rsvg_handle_set_dpi(rsvgh, POINTS_PER_INCH);

                break;
            default:
                rsvgh = NULL;
        }

        if (rsvgh) {
            us->data = (void*)rsvgh;
            us->datafree = cairo_freeimage;
        }
    }
    return rsvgh;
}

static void rsvg_loadimage_cairo(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    RsvgHandle* rsvgh = cairo_loadimage(job, us);

    cairo_t *cr = (cairo_t *) job->context; /* target context */
    cairo_surface_t *surface;	 /* source surface */

    if (rsvgh) {
        cairo_save(cr);

       	surface = cairo_svg_surface_create(NUL_FILE, us->w, us->h); 

	cairo_surface_reference(surface);

        cairo_set_source_surface(cr, surface, 0, 0);

        cairo_translate(cr, ROUND(b.LL.x), ROUND(-b.UR.y));
        cairo_scale(cr, (b.UR.x - b.LL.x) / us->w,
                       (b.UR.y - b.LL.y) / us->h);


	rsvg_handle_render_cairo(rsvgh, cr);

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
