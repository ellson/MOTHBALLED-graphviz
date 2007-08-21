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

#ifdef HAVE_LIBGD
#include "gd.h"

typedef enum {
	FORMAT_GD,
	FORMAT_GD2,
	FORMAT_GIF,
	FORMAT_JPEG,
	FORMAT_PNG,
	FORMAT_WBMP,
	FORMAT_XBM,
} format_type;

static void gd_formatter(GVJ_t * job)
{
    gdImagePtr im = (gdImagePtr) job->surface;

    if (!im)
	return;

#ifdef HAVE_SETMODE
#ifdef O_BINARY
	/*
	 * Windows will do \n -> \r\n  translations on stdout
	 * unless told otherwise.  */
	setmode(fileno(job->output_file), O_BINARY);
#endif
#endif

	/* Only save the alpha channel in outputs that support it if
	   the base color was transparent.   Otherwise everything
	   was blended so there is no useful alpha info */
//FIXME   	gdImageSaveAlpha(im, (basecolor == transparent));
	switch (job->render.id) {
#if 0
	case FORMAT_GD:
	    gdImageGd(im, job->output_file);
	    break;
	case FORMAT_GD2:
#define GD2_CHUNKSIZE 128
#define GD2_RAW 1
#define GD2_COMPRESSED 2
	    gdImageGd2(im, job->output_file, GD2_CHUNKSIZE,
		       GD2_COMPRESSED);
	    break;
#endif
	case FORMAT_GIF:
#ifdef HAVE_GD_GIF
	    gdImageTrueColorToPalette(im, 0, 256);
	    gdImageGif(im, job->output_file);
#endif
	    break;
	case FORMAT_JPEG:
#ifdef HAVE_GD_JPEG
	    /*
	     * Write IM to OUTFILE as a JFIF-formatted JPEG image, using
	     * quality JPEG_QUALITY.  If JPEG_QUALITY is in the range
	     * 0-100, increasing values represent higher quality but also
	     * larger image size.  If JPEG_QUALITY is negative, the
	     * IJG JPEG library's default quality is used (which should
	     * be near optimal for many applications).  See the IJG JPEG
	     * library documentation for more details.  */
#define JPEG_QUALITY -1
	    gdImageJpeg(im, job->output_file, JPEG_QUALITY);
#endif
	    break;
	case FORMAT_PNG:
#ifdef HAVE_GD_PNG
	    gdImagePng(im, job->output_file);
#endif
	    break;
	case FORMAT_WBMP:
	    /* Use black for the foreground color for the B&W wbmp image. */
//FIXME	    gdImageWBMP(im, black, job->output_file);
	    break;
	case FORMAT_XBM:
#if 0
#ifdef HAVE_GD_XPM
	    gdImageXbm(im, job->output_file);
#endif
#endif
	    break;
	}
	gdImageDestroy(im);
}

static gvformatter_engine_t gd_engine = {
    gd_formatter,
};

static gvformatter_features_t gd_features = {
    0,  /* flags */
};

#endif

gvplugin_installed_t gvformatter_gd_types[] = {
#ifdef HAVE_LIBGD
    {FORMAT_GD, "cairo2gd", -1, &gd_engine, &gd_features},
    {FORMAT_GD2, "cairo2gd2", -1, &gd_engine, &gd_features},
#ifdef HAVE_GD_GIF
    {FORMAT_GIF, "cairo2gif", -1, &gd_engine, &gd_features},
#endif
#ifdef HAVE_GD_JPEG
    {FORMAT_JPEG, "cairo2jpe", -1, &gd_engine, &gd_features},
    {FORMAT_JPEG, "cairo2jpeg", -1, &gd_engine, &gd_features},
    {FORMAT_JPEG, "cairo2jpg", -1, &gd_engine, &gd_features},
#endif
#ifdef HAVE_GD_PNG
    {FORMAT_PNG, "cairo2png", -1, &gd_engine, &gd_features},
#endif
    {FORMAT_WBMP, "cairo2wbmp", -1, &gd_engine, &gd_features},
#ifdef HAVE_GD_XPM
    {FORMAT_XBM, "cairo2xbm", -1, &gd_engine, &gd_features},
#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
