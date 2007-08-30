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

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif
#include <stdlib.h>
#include "gvplugin_device.h"

#ifdef HAVE_LIBGD
#include "gd.h"

typedef enum {
	FORMAT_GIF,
	FORMAT_JPEG,
	FORMAT_PNG,
	FORMAT_WBMP,
	FORMAT_GD,
	FORMAT_GD2,
	FORMAT_XBM,
} format_type;

static void gd_format(GVJ_t * job)
{
    gdImagePtr im;
    unsigned int x, y, color, alpha;
    unsigned int *data = (unsigned int*)(job->imagedata);
    unsigned int width = job->width;
    unsigned int height = job->height;

#ifdef HAVE_SETMODE
#ifdef O_BINARY
    /*
     * Windows will do \n -> \r\n  translations on stdout
     * unless told otherwise.
     */
    setmode(fileno(job->output_file), O_BINARY);
#endif
#endif

    im = gdImageCreateTrueColor(width, height);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
	    color = *data++;
	    /* gd's max alpha is 127 */
	    if ((alpha = (color >> 25) & 0x7f))
	        /* gd's alpha is transparency instead of opacity */
	    	color = (color & 0xffffff) | ((127 - alpha) << 24);
	    else
		color = im->transparent;
	    gdImageSetPixel (im, x, y, color);
	}
    }

    switch (job->device.id) {
#ifdef HAVE_GD_GIF
    case FORMAT_GIF:
	gdImageTrueColorToPalette(im, 0, 256);
	gdImageGif(im, job->output_file);
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
	gdImageJpeg(im, job->output_file, JPEG_QUALITY);
	break;
#endif

#ifdef HAVE_GD_PNG
    case FORMAT_PNG:
	gdImagePng(im, job->output_file);
        break;
#endif

#if 0
    case FORMAT_GD:
	gdImageGd(im, job->output_file);
	break;

    case FORMAT_GD2:
#define GD2_CHUNKSIZE 128
#define GD2_RAW 1
#define GD2_COMPRESSED 2
	gdImageGd2(im, job->output_file, GD2_CHUNKSIZE, GD2_COMPRESSED);
	break;

#ifdef HAVE_GD_GIF
    case FORMAT_WBMP:
	/* Use black for the foreground color for the B&W wbmp image. */
//FIXME - black not defined - is it really needed? 
	gdImageWBMP(im, black, job->output_file);
	break;
#endif

#ifdef HAVE_GD_XPM
    case FORMAT_XBM:
	gdImageXbm(im, job->output_file);
#endif

#endif
	break;
    default:
	break;
    }

    gdImageDestroy(im);
}

static gvdevice_engine_t gd_engine = {
    NULL,
    NULL,
    gd_format,
    NULL,
};

static gvdevice_features_t gd_features = {
    0,  /* flags */
};

#endif

gvplugin_installed_t gvdevice_gd_types[] = {
#ifdef HAVE_LIBGD

#ifdef HAVE_GD_GIF
    {FORMAT_GIF, "gif:cairo", 10, &gd_engine, &gd_features},
#endif

#ifdef HAVE_GD_JPEG
    {FORMAT_JPEG, "jpe:cairo", 5, &gd_engine, &gd_features},
    {FORMAT_JPEG, "jpeg:cairo", 5, &gd_engine, &gd_features},
    {FORMAT_JPEG, "jpg:cairo", 5, &gd_engine, &gd_features},
#endif

#ifdef HAVE_GD_PNG
    {FORMAT_PNG, "png:cairo", -1, &gd_engine, &gd_features},
#endif

#if 0
    {FORMAT_GD, "gd:cairo", -1, &gd_engine, &gd_features},
    {FORMAT_GD2, "gd2:cairo", -1, &gd_engine, &gd_features},

#ifdef HAVE_GD_GIF
    {FORMAT_WBMP, "wbmp:cairo", -1, &gd_engine, &gd_features},
#endif

#ifdef HAVE_GD_XPM
    {FORMAT_XBM, "xbm:cairo", -1, &gd_engine, &gd_features},
#endif
#endif

#endif
    {0, NULL, 0, NULL, NULL}
};
