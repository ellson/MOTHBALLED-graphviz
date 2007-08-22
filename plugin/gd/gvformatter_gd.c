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
#include "gvplugin_formatter.h"

#ifdef HAVE_LIBGD
#ifdef HAVE_PANGOCAIRO
#include <pango/pangocairo.h>
#include "gd.h"

typedef enum {
	FORMAT_GIF,
	FORMAT_JPEG,
#if 0
	FORMAT_PNG,
	FORMAT_WBMP,
	FORMAT_GD,
	FORMAT_GD2,
	FORMAT_XBM,
#endif
} format_type;

static void
cairo_surface_write_to_gd(cairo_surface_t *surface, format_type format, FILE *f)
{
    gdImagePtr im;
    unsigned int width, height, x, y, *data, color;

    width = cairo_image_surface_get_width(surface);
    height = cairo_image_surface_get_height(surface);
    data = (unsigned int*)cairo_image_surface_get_data(surface);

    im = gdImageCreateTrueColor(width, height);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
	    color = *data++;
	    /* gd uses a transparency alpha instead of the more typical opacity */
	    /* gd's max alpha is 127 */
	    color = (color & 0xffffff) | ((127 - (color >> 25)) << 24);
	    gdImageSetPixel (im, x, y, color);
	}
    }

    switch (format) {
#ifdef HAVE_GD_GIF
    case FORMAT_GIF:
	gdImageTrueColorToPalette(im, 0, 256);
	gdImageGif(im, f);
        break;
#endif

#ifdef HAVE_GD_JPEG
    case FORMAT_JPEG:
	/*
	 * Write IM to OUTFILE as a JFIF-formatted JPEG image, using
	 * quality JPEG_QUALITY.  If JPEG_QUALITY is in the range
	 * 0-100, increasing values represent higher quality but also
	 * larger image size.  If JPEG_QUALITY is negative, the
	 * IJG JPEG library's default quality is used (which should
	 * be near optimal for many applications).  See the IJG JPEG
	 * library documentation for more details.
	 */ 
#define JPEG_QUALITY -1
	gdImageJpeg(im, f, JPEG_QUALITY);
	break;
#endif

#if 0

#ifdef HAVE_GD_PNG
    case FORMAT_PNG:
	gdImagePng(im, f);
        break;
#endif

    case FORMAT_GD:
	gdImageGd(im, f);
	break;

    case FORMAT_GD2:
#define GD2_CHUNKSIZE 128
#define GD2_RAW 1
#define GD2_COMPRESSED 2
	gdImageGd2(im, f, GD2_CHUNKSIZE, GD2_COMPRESSED);
	break;

#ifdef HAVE_GD_GIF
    case FORMAT_WBMP:
	/* Use black for the foreground color for the B&W wbmp image. */
//FIXME - black not defined - is it really needed? 
	gdImageWBMP(im, black, f);
	break;
#endif

#ifdef HAVE_GD_XPM
    case FORMAT_XBM:
	gdImageXbm(im, f);
#endif

#endif
	break;
    default:
	break;
    }

    gdImageDestroy(im);
}

static void gd_formatter(GVJ_t * job)
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

    cairo_surface_write_to_gd(surface, job->formatter.id, job->output_file);
}

static gvformatter_engine_t gd_engine = {
    gd_formatter,
};

static gvformatter_features_t gd_features = {
    0,  /* flags */
};

#endif
#endif

gvplugin_installed_t gvformatter_gd_types[] = {
#ifdef HAVE_PANGOCAIRO
#ifdef HAVE_LIBGD

#ifdef HAVE_GD_GIF
    {FORMAT_GIF, "cairo2gif", 10, &gd_engine, &gd_features},
#endif

#ifdef HAVE_GD_JPEG
    {FORMAT_JPEG, "cairo2jpe", 5, &gd_engine, &gd_features},
    {FORMAT_JPEG, "cairo2jpeg", 5, &gd_engine, &gd_features},
    {FORMAT_JPEG, "cairo2jpg", 5, &gd_engine, &gd_features},
#endif

#if 0

#ifdef HAVE_GD_PNG
    {FORMAT_PNG, "cairo2png", -1, &gd_engine, &gd_features},
#endif

    {FORMAT_GD, "cairo2gd", -1, &gd_engine, &gd_features},
    {FORMAT_GD2, "cairo2gd2", -1, &gd_engine, &gd_features},
#ifdef HAVE_GD_GIF
    {FORMAT_WBMP, "cairo2wbmp", -1, &gd_engine, &gd_features},
#endif
#ifdef HAVE_GD_XPM
    {FORMAT_XBM, "cairo2xbm", -1, &gd_engine, &gd_features},
#endif
#endif

#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
