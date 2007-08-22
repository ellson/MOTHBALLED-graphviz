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

#include "gvplugin_formatter.h"
#include <pango/pangocairo.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

typedef enum {
	FORMAT_BMP,
	FORMAT_ICO,
	FORMAT_JPEG,
	FORMAT_PNG,
	FORMAT_TIFF,
    } format_type;

/* 
 * Does an in-place conversion of a CAIRO ARGB32 image to GDK RGBA
 */
static void
argb2rgba ( unsigned int width, unsigned int height, unsigned char *data)
{
    unsigned int x, y;
    unsigned char r, g, b, a;

#define Ra 2
#define Ga 1
#define Ba 0
#define Aa 3

#define Rb 0
#define Gb 1
#define Bb 2
#define Ab 3

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            r = data[Ra];
            g = data[Ga];
            b = data[Ba];
            a = data[Aa];

            data[Rb] = r;
            data[Gb] = g;
            data[Bb] = b;
            data[Ab] = a;

            data += 4;
        }
    }
}

static gboolean
writer ( const gchar *buf, gsize count, GError **error, gpointer data)
{
    error = NULL;
    if (count == fwrite(buf, 1, count, (FILE *)data))
        return TRUE;
    return FALSE;
}

static void
cairo_surface_write_to_gdk_pixbuf(cairo_surface_t *surface, const char *format, FILE *f) 
{
    GdkPixbuf *pixbuf;
    unsigned int width, height;
    unsigned char *data;

    width = cairo_image_surface_get_width(surface);
    height = cairo_image_surface_get_height(surface);
    data = cairo_image_surface_get_data(surface);

    argb2rgba(width, height, data);

    pixbuf = gdk_pixbuf_new_from_data(
                data,                   // data
                GDK_COLORSPACE_RGB,     // colorspace
                TRUE,                   // has_alpha
                8,                      // bits_per_sample
                width,                  // width
                height,                 // height
                4 * width,              // rowstride
                NULL,                   // destroy_fn
                NULL                    // destroy_fn_data
               );

    gdk_pixbuf_save_to_callback(pixbuf, writer, f, format, NULL, NULL);

    gdk_pixbuf_unref(pixbuf);
}

static void gdk_pixbuf_formatter(GVJ_t * job)
{
    cairo_t *cr = (cairo_t *) job->surface;
    cairo_surface_t *surface;

    surface = cairo_get_target(cr);
#ifdef HAVE_SETMODE
#ifdef O_BINARY
    /*
     * Windows will do \n -> \r\n  translations on stdout
     * unless told otherwise.
     */
    setmode(fileno(job->output_file), O_BINARY);
#endif
#endif
    switch (job->formatter.id) {
    case FORMAT_BMP:
	cairo_surface_write_to_gdk_pixbuf(surface, "bmp", job->output_file);
	break;
    case FORMAT_ICO:
	cairo_surface_write_to_gdk_pixbuf(surface, "ico", job->output_file);
	break;
    case FORMAT_JPEG:
	cairo_surface_write_to_gdk_pixbuf(surface, "jpeg", job->output_file);
	break;
    case FORMAT_PNG:
	cairo_surface_write_to_gdk_pixbuf(surface, "png", job->output_file);
	break;
    case FORMAT_TIFF:
	cairo_surface_write_to_gdk_pixbuf(surface, "tiff", job->output_file);
	break;
    default:
	break;
    }
}

static gvformatter_engine_t gdk_pixbuf_engine = {
    gdk_pixbuf_formatter,
};

static gvformatter_features_t gdk_pixbuf_features = {
    0,  /* flags */
};

gvplugin_installed_t gvformatter_gdk_pixbuf_types[] = {
    {FORMAT_BMP, "cairo2bmp", 10, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {FORMAT_ICO, "cairo2ico", 10, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {FORMAT_JPEG, "cairo2jpg", 10, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {FORMAT_JPEG, "cairo2jpe", 10, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {FORMAT_JPEG, "cairo2jpeg", 10, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {FORMAT_PNG, "cairo2png", 5, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {FORMAT_TIFF, "cairo2tif", 10, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {FORMAT_TIFF, "cairo2tiff", 10, &gdk_pixbuf_engine, &gdk_pixbuf_features},
    {0, NULL, 0, NULL, NULL}
};
