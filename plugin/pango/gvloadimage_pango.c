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
#include <cairo.h>

typedef enum {
    FORMAT_PNG_CAIRO, FORMAT_PNG_PS,
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

    if (us->data) {
        if (us->datafree == cairo_freeimage)
             surface = (cairo_surface_t*)(us->data); /* use cached data */
        else {
             us->datafree(us);        /* free incompatible cache data */
             us->data = NULL;
        }
    }
    if (!surface) { /* read file into cache */
	if (!gvusershape_file_access(us))
	    return NULL;
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
	gvusershape_file_release(us);
    }
    return surface;
}

static void pango_loadimage_cairo(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
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

static void pango_loadimage_ps(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
    cairo_surface_t *surface; 	/* source surface */
    FILE *out = job->output_file;
    int X, Y, x, y, stride;
    unsigned char *data, *ix, alpha, red, green, blue;

    surface = cairo_loadimage(job, us);
    if (surface && (cairo_image_surface_get_format(surface) == CAIRO_FORMAT_ARGB32)) {
	X = cairo_image_surface_get_width(surface);
	Y = cairo_image_surface_get_height(surface);
	stride = cairo_image_surface_get_stride(surface);
	data = cairo_image_surface_get_data(surface);

        fprintf(out, "save\n");

	/* define image data as string array (one per raster line) */
	/* see parallel code in gd_loadimage_ps().  FIXME: refactor... */
        fprintf(out, "/myctr 0 def\n");
        fprintf(out, "/myarray [\n");
        for (y = 0; y < Y; y++) {
	    fprintf(out, "<");
	    ix = data + y * stride;
            for (x = 0; x < X; x++) {
		/* FIXME - this code may have endian problems */
		blue = *ix++;
		green = *ix++;
		red = *ix++;
		alpha = *ix++;
                fprintf(out, "%02x%02x%02x", red, green, blue);
            }
	    fprintf(out, ">\n");
        }
	fprintf(out, "] def\n");
        fprintf(out,"/myproc { myarray myctr get /myctr myctr 1 add def } def\n");

        /* this sets the position of the image */
        fprintf(out, "%g %g translate %% lower-left coordinate\n", b.LL.x, b.LL.y);

        /* this sets the rendered size to fit the box */
        fprintf(out,"%g %g scale\n", b.UR.x - b.LL.x, b.UR.y - b.LL.y);

        /* xsize ysize bits-per-sample [matrix] */
        fprintf(out, "%d %d 8 [%d 0 0 %d 0 %d]\n", X, Y, X, -Y, Y);

        fprintf(out, "{myproc} false 3 colorimage\n");

        fprintf(out, "restore\n");
    }
}

static gvloadimage_engine_t engine_cairo = {
    pango_loadimage_cairo
};

static gvloadimage_engine_t engine_ps = {
    pango_loadimage_ps
};
#endif

gvplugin_installed_t gvloadimage_pango_types[] = {
#ifdef HAVE_PANGOCAIRO
    {FORMAT_PNG_CAIRO, "png:cairo", 1, &engine_cairo, NULL},
    {FORMAT_PNG_PS, "png:ps", 2, &engine_ps, NULL},
#endif
    {0, NULL, 0, NULL, NULL}
};
