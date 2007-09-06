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

#include "gvplugin_device.h"
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
/* define indexes to color bytes in each format */
#define Ra 2
#define Ga 1
#define Ba 0
#define Aa 3

#define Rb 0
#define Gb 1
#define Bb 2
#define Ab 3

/* only need to process those bytes whose index is different */
#if (Ra != Rb)
    unsigned char r;
#endif
#if (Ga != Gb)
    unsigned char g;
#endif
#if (Ba != Bb)
    unsigned char b;
#endif
#if (Aa != Ab)
    unsigned char a;
#endif
    unsigned int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
#if (Ra != Rb)
            r = data[Ra];
#endif
#if (Ga != Gb)
            g = data[Ga];
#endif
#if (Ba != Bb)
            b = data[Ba];
#endif
#if (Aa != Ab)
            a = data[Aa];
#endif

#if (Ra != Rb)
            data[Rb] = r;
#endif
#if (Ga != Gb)
            data[Gb] = g;
#endif
#if (Ba != Bb)
            data[Bb] = b;
#endif
#if (Aa != Ab)
            data[Ab] = a;
#endif

            data += 4;
        }
    }
}

static gboolean
writer ( const gchar *buf, gsize count, GError **error, gpointer data)
{
    if (count == fwrite(buf, 1, count, (FILE *)data))
        return TRUE;
    return FALSE;
}

static void gdk_pixbuf_format(GVJ_t * job)
{
    char *format_str = "";
    GdkPixbuf *pixbuf;

    switch (job->device.id) {
    case FORMAT_BMP:
	format_str = "bmp";
	break;
    case FORMAT_ICO:
	format_str = "ico";
	break;
    case FORMAT_JPEG:
	format_str = "jpeg";
	break;
    case FORMAT_PNG:
	format_str = "png";
	break;
    case FORMAT_TIFF:
	format_str = "tiff";
	break;
    }

    argb2rgba(job->width, job->height, job->imagedata);

    pixbuf = gdk_pixbuf_new_from_data(
                job->imagedata,         // data
                GDK_COLORSPACE_RGB,     // colorspace
                TRUE,                   // has_alpha
                8,                      // bits_per_sample
                job->width,             // width
                job->height,            // height
                4 * job->width,         // rowstride
                NULL,                   // destroy_fn
                NULL                    // destroy_fn_data
               );

    gdk_pixbuf_save_to_callback(pixbuf, writer, job->output_file, format_str, NULL, NULL);

    gdk_pixbuf_unref(pixbuf);
}

static gvdevice_engine_t gdk_pixbuf_engine = {
    NULL,
    NULL,
    gdk_pixbuf_format,
    NULL,
};

static gvdevice_features_t device_features_gdk_pixbuf = {
    GVDEVICE_BINARY_FORMAT
      | GVDEVICE_DOES_TRUECOLOR,/* flags */
    {0.,0.},                    /* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {96.,96.},                  /* dpi */
};

gvplugin_installed_t gvdevice_gdk_pixbuf_types[] = {
    {FORMAT_BMP, "bmp:cairo", 10, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {FORMAT_ICO, "ico:cairo", 10, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {FORMAT_JPEG, "jpe:cairo", 10, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {FORMAT_JPEG, "jpeg:cairo", 10, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {FORMAT_JPEG, "jpg:cairo", 10, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {FORMAT_PNG, "png:cairo", 5, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {FORMAT_TIFF, "tif:cairo", 10, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {FORMAT_TIFF, "tiff:cairo", 10, &gdk_pixbuf_engine, &device_features_gdk_pixbuf},
    {0, NULL, 0, NULL, NULL}
};
