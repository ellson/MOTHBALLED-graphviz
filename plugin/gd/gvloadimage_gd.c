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
#include <stddef.h>
#include <string.h>

#ifdef HAVE_LIBZ
#include <zlib.h>
#ifdef MSWIN32
#include <io.h>
#endif
#endif

#include "gvplugin_loadimage.h"

#ifdef HAVE_LIBGD
#include "gd.h"

typedef enum {
    FORMAT_PNG_GD, FORMAT_GIF_GD, FORMAT_JPG_GD, FORMAT_GD_GD, FORMAT_GD2_GD, FORMAT_XPM_GD, FORMAT_WBMP_GD,
    FORMAT_PNG_PS, FORMAT_GIF_PS, FORMAT_JPG_PS, FORMAT_GD_PS, FORMAT_GD2_PS, FORMAT_XPM_PS, FORMAT_WBMP_PS,
} format_type;


static void gd_freeimage(usershape_t *us)
{
    gdImageDestroy((gdImagePtr)us->data);
}

static gdImagePtr gd_loadimage(GVJ_t * job, usershape_t *us)
{
    assert(job);
    assert(us);
    assert(us->name);
    assert(us->f);

    if (us->data) {
	if (us->datafree != gd_freeimage) {
	     us->datafree(us);        /* free incompatible cache data */
	     us->data = NULL;
	     us->datafree = NULL;
	}
    }
    if (!us->data) { /* read file into cache */
	fseek(us->f, 0, SEEK_SET);
	switch (us->type) {
#if 0
	    case FT_GD:
		im = gdImageCreateFromGd(us->f);
		break;
	    case FT_GD2:
		im = gdImageCreateFromGd2(us->f);
		break;
#endif
#ifdef HAVE_GD_PNG
	    case FT_PNG:
		us->data = (void*)gdImageCreateFromPng(us->f);
		break;
#endif
#ifdef HAVE_GD_GIF
	    case FT_GIF:
		us->data = (void*)gdImageCreateFromGif(us->f);
		break;
#endif
#ifdef HAVE_GD_JPEG
	    case FT_JPEG:
		us->data = (void*)gdImageCreateFromJpeg(us->f);
		break;
#endif
#if 0
#ifdef HAVE_GD_XPM
	    case FT_XPM:
		us->data = (void*)gdImageCreateFromXpm(us->f);
		break;
#endif
#ifdef HAVE_GD_WBMP
	    case FT_WBMP:
		us->data = (void*)gdImageCreateFromWbmp(us->f);
		break;
#endif
#endif
	    default:
		break;
	}
        if (us->data)
	    us->datafree = gd_freeimage;
    }
    return (gdImagePtr)(us->data);
}

static void gd_loadimage_gd(GVJ_t * job, usershape_t *us, boxf b, bool filled)
{
    gdImagePtr im3, im2 = NULL, im = (gdImagePtr) job->surface;

    if ((im2 = gd_loadimage(job, us))) {
        if (job->rotation) {
            im3 = gdImageCreate(im2->sy, im2->sx); /* scratch image for rotation */
            gdImageCopyRotated(im3, im2, im3->sx / 2., im3->sy / 2.,
                0, 0, im2->sx, im2->sy, job->rotation);
            gdImageCopyResized(im, im3, ROUND(b.LL.x), ROUND(b.LL.y), 0, 0,
                ROUND(b.UR.y - b.LL.y), ROUND(b.UR.x - b.LL.x), im3->sx, im3->sy);
            gdImageDestroy(im3);
        }
        else {
            gdImageCopyResized(im, im2, ROUND(b.LL.x), ROUND(b.LL.y), 0, 0,
                ROUND(b.UR.x - b.LL.x), ROUND(b.UR.y - b.LL.y), im2->sx, im2->sy);
        }
    }
}

static void gd_loadimage_ps(GVJ_t * job, usershape_t *us, boxf b, bool filled)
{
    gdImagePtr im = NULL;
    FILE *out = job->output_file;
    int X, Y, x, y, px;

    if ((im = gd_loadimage(job, us))) {
	X = im->sx;
	Y = im->sy;

        fprintf(out, "gsave\n");

        /* this sets the position of the image */
        fprintf(out, "%g %g translate %% lower-left coordinate\n", b.LL.x, b.LL.y);
    
        /* this sets the rendered size to fit the box */
        fprintf(out,"%g %g scale\n", b.UR.x - b.LL.x, b.UR.y - b.LL.y);
    
        /* xsize ysize bits-per-sample [matrix] */
        fprintf(out, "%d %d 8 [%d 0 0 %d 0 %d]\n", X, Y, X, -Y, Y);
    
        fprintf(out, "{<\n");
        if (im->trueColor) {
            for (y = 0; y < Y; y++) {
                for (x = 0; x < X; x++) {
                    px = gdImageTrueColorPixel(im, x, y);
                    fprintf(out, "%02x%02x%02x",
                        gdTrueColorGetRed(px),
                        gdTrueColorGetGreen(px),
                        gdTrueColorGetBlue(px));
                }
            }
            fprintf(out, "\n");
	}
        else {
            for (y = 0; y < Y; y++) {
                for (x = 0; x < X; x++) {
                    px = gdImagePalettePixel(im, x, y);
                    fprintf(out, "%02x%02x%02x",
                        im->red[px],
                        im->green[px],
                        im->blue[px]);
                }
            }
            fprintf(out, "\n");
        }
    
        fprintf(out, ">}\n");
        fprintf(out, "false 3 colorimage\n");
    
        fprintf(out, "grestore\n");
    }
}

static gvloadimage_engine_t engine = {
    gd_loadimage_gd
};

static gvloadimage_engine_t engine_ps = {
    gd_loadimage_ps
};

#endif

gvplugin_installed_t gvloadimage_gd_types[] = {
#ifdef HAVE_LIBGD
#if 0
    {FORMAT_GD_GD, "gd2gd", 1, &engine, NULL},
    {FORMAT_GD2_GD, "gd22gd", 1, &engine, NULL},
#endif
#ifdef HAVE_GD_GIF
    {FORMAT_GIF_GD, "gif2gd", 1, &engine, NULL},
#endif
#ifdef HAVE_GD_JPEG
    {FORMAT_JPG_GD, "jpeg2gd", 1, &engine, NULL},
#endif
#ifdef HAVE_GD_PNG
    {FORMAT_PNG_GD, "png2gd", 1, &engine, NULL},
#endif
#if 0
#ifdef HAVE_GD_WBMP
    {FORMAT_WBMP_GD, "wbmp2gd", 1, &engine, NULL},
#endif
#ifdef HAVE_GD_XPM
    {FORMAT_XBM_GD, "xbm2gd", 1, &engine, NULL},
#endif
#endif
#if 0
    {FORMAT_GD_PS, "gd2ps", 1, &engine_ps, NULL},
    {FORMAT_GD2_PS, "gd22ps", 1, &engine_ps, NULL},
#endif
#ifdef HAVE_GD_GIF
    {FORMAT_GIF_PS, "gif2ps", 1, &engine_ps, NULL},
#endif
#ifdef HAVE_GD_JPEG
    {FORMAT_JPG_PS, "jpeg2ps", 1, &engine_ps, NULL},
#endif
#ifdef HAVE_GD_PNG
    {FORMAT_PNG_PS, "png2ps", 1, &engine_ps, NULL},
#endif
#if 0
#ifdef HAVE_GD_WBMP
    {FORMAT_WBMP_PS, "wbmp2ps", 1, &engine_ps, NULL},
#endif
#ifdef HAVE_GD_XPM
    {FORMAT_XBM_PS, "xbm2ps", 1, &engine_ps, NULL},
#endif
#endif
#endif
    {0, NULL, 0, NULL, NULL}
};
