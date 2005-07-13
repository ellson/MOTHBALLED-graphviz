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
 * This program generates an image where each pixel is the difference between
 * the corresponding pixel in each of the two source images.  Thus, if the source images
 * are the same the resulting image will be black, otherwise it will have regions of
 * non-black where the images differ.
 *
 * Currently only supports PNG images.
 *
 * John Ellson <ellson@research.att.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

#define ABS(x) (((x) < 0) ? -(x) : (x))

void imageDiff (gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX,
             int srcY, int w, int h)
{
  int s, d;
  int x, y;
/*  assert (dst->trueColor); */
  for (y = 0; (y < h); y++)
    {
      for (x = 0; (x < w); x++)
        {
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

int main(int argc, char **argv)
{
    gdImagePtr im1, im2, im3;
    FILE *in;

    if (argc != 3) {
        fprintf(stderr, "Usage: diffimg file1.png file2.png \n");
        exit(1);
    }
    in = fopen(argv[1], "rb");
    if (!in) {
        fprintf(stderr, "Input file1 does not exist!\n");
        exit(1);
    }
    im1 = gdImageCreateFromPng(in);
    fclose(in);
    if (!im1) {
        fprintf(stderr, "Input file1 is not in PNG format!\n");
        exit(1);
    }

    in = fopen(argv[2], "rb");
    if (!in) {
        fprintf(stderr, "Input file2 does not exist!\n");
        exit(1);
    }
    im2 = gdImageCreateFromPng(in);
    fclose(in);
    if (!im2) {
        fprintf(stderr, "Input file2 is not in PNG format!\n");
        exit(1);
    }

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

