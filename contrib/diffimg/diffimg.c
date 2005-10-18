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

/*
 * This program generates an image where each pixel is the
 * difference between the corresponding pixel in each of the
 * two source images.  Thus, if the source images are the same
 * the resulting image will be black, otherwise it will have
 * regions of non-black where the images differ.
 *
 * Currently supports: .png, .gif, .jpg
 *
 * John Ellson <ellson@research.att.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>

#define ABS(x) (((x) < 0) ? -(x) : (x))

static void imageDiff (gdImagePtr dst, gdImagePtr src,
			int dstX, int dstY,
			int srcX, int srcY,
			int w, int h)
{
    int s, d;
    int x, y;

    for (y = 0; (y < h); y++) {
        for (x = 0; (x < w); x++) {
	    s = gdImageGetTrueColorPixel (src, srcX + x, srcY + y);
	    d = gdImageGetTrueColorPixel (dst, dstX + x, dstY + y);
    
	    d = gdTrueColorAlpha(
		  ABS(gdTrueColorGetRed(s) - gdTrueColorGetRed(d)),
		  ABS(gdTrueColorGetGreen(s) - gdTrueColorGetGreen(d)),
		  ABS(gdTrueColorGetBlue(s) - gdTrueColorGetBlue(d)),
		  ABS(gdTrueColorGetAlpha(s) - gdTrueColorGetAlpha(d)));
    
            gdImageSetPixel (dst, dstX + x, dstY + y, d);
        }
    }
}

static gdImagePtr imageLoad (char *filename)
{
    FILE *f;
    char *ext;
    gdImagePtr im;

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Input file \"%s\" does not exist!\n", filename);
        exit(1);
    }
    ext = strrchr(filename, '.');
    if (!ext) {
        fprintf(stderr, "Filename \"%s\" has no file extension.\n", filename);
        exit(1);
    }
    im = 0;
    if (strcasecmp(ext, ".png") == 0) 
        im = gdImageCreateFromPng(f);
    else if (strcasecmp(ext, ".gif") == 0)
        im = gdImageCreateFromGif(f);
    else if (strcasecmp(ext, ".jpg") == 0)
        im = gdImageCreateFromJpeg(f);
    fclose(f);
    if (!im) {
        fprintf(stderr, "Loading image from file  \"%s\" failed!\n", filename);
        exit(1);
    }
    return im;
}

int main(int argc, char **argv)
{
    gdImagePtr im1, im2, im3;

    if (argc != 3) {
        fprintf(stderr, "Usage: diffimg image1 image2\n");
        exit(1);
    }
    im1 = imageLoad(argv[1]);
    im2 = imageLoad(argv[2]);

    im3 = gdImageCreateTrueColor (
	(gdImageSX(im1) > gdImageSX(im2)) ? gdImageSX(im1) : gdImageSX(im2),
	(gdImageSY(im1) > gdImageSY(im2)) ? gdImageSY(im1) : gdImageSY(im2));

    gdImageCopy (im3, im1, 0, 0, 0, 0, gdImageSX(im1), gdImageSY(im1));

    imageDiff (im3, im2, 0, 0, 0, 0, gdImageSX(im2), gdImageSY(im2));

    gdImagePng (im3, stdout);

    gdImageDestroy(im1);
    gdImageDestroy(im2);
    gdImageDestroy(im3);

    return 0;
}

