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
 * Currently supports: .png, .gif, .jpg, and .ps by using ghostscript
 *
 * John Ellson <ellson@research.att.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sysexits.h>
#include <gd.h>

#define NOT(v) (!(v))
#if ! defined HAVE_BOOL && ! defined __cplusplus
typedef unsigned char bool;
#define false 0
#define true NOT(false)
#endif

static char *pstopng="gs -dNOPAUSE -sDEVICE=pngalpha -sOutputFile=- -q -";

static gdImagePtr imageLoad (char *filename)
{
    FILE *f;
    char *ext, *cmd, *tmp;
    gdImagePtr im;
    int rc;
    struct stat statbuf;

    ext = strrchr(filename, '.');
    if (!ext) {
        fprintf(stderr, "Filename \"%s\" has no file extension.\n", filename);
        exit(EX_USAGE);
    }
    rc = stat(filename, &statbuf);
    if (rc) {
	 fprintf(stderr, "Failed to stat \"%s\"\n", filename);
         exit(EX_NOINPUT);
    }
    if (strcasecmp(ext, ".ps") == 0) {
	ext = ".png";
	tmp = malloc(strlen(filename) + strlen(ext) + 1);
	strcpy(tmp,filename);
	strcat(tmp,ext);
	
	cmd = malloc(strlen(pstopng) + 2 + strlen(filename) + 2 + strlen(tmp) + 1);
	strcpy(cmd,pstopng);
	strcat(cmd," <");
	strcat(cmd,filename);
	strcat(cmd," >");
	strcat(cmd,tmp);
	rc = system(cmd);
	free(cmd);
	
        f = fopen(tmp, "rb");
	free(tmp);
        if (!f) {
            fprintf(stderr, "Failed to open converted \"%s%s\"\n", filename, ext);
            exit(EX_NOINPUT);
        }
    }
    else {
        f = fopen(filename, "rb");
        if (!f) {
            fprintf(stderr, "Failed to open \"%s\"\n", filename);
            exit(EX_NOINPUT);
        }
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
        exit(EX_DATAERR);
    }
    return im;
}

static bool imageDiff (gdImagePtr A, gdImagePtr B, gdImagePtr C,
	unsigned int w, unsigned int h,
	unsigned char black, unsigned char white)
{
    unsigned int x, y;
    bool d, rc;

    rc = false;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
	    d = (bool)( gdImageGetTrueColorPixel(B,x,y)
		      - gdImageGetTrueColorPixel(A,x,y));
            gdImageSetPixel (C, x, y, (d ? white : black));
	    rc |= d;
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    gdImagePtr A, B, C;
    unsigned char black, white;
    unsigned int minSX, minSY, maxSX, maxSY;
    FILE *f;
    bool rc;

    if (argc < 3) {
        fprintf(stderr, "Usage: diffimg image1 image2 [outimage]\n");
        exit(EX_USAGE);
    }
    A = imageLoad(argv[1]);
    B = imageLoad(argv[2]);

    minSX = (gdImageSX(A) < gdImageSX(B)) ? gdImageSX(A) : gdImageSX(B);
    minSY = (gdImageSY(A) < gdImageSY(B)) ? gdImageSY(A) : gdImageSY(B);
    maxSX = (gdImageSX(A) > gdImageSX(B)) ? gdImageSX(A) : gdImageSX(B);
    maxSY = (gdImageSY(A) > gdImageSY(B)) ? gdImageSY(A) : gdImageSY(B);
    
    C = gdImageCreatePalette (maxSX, maxSY);

    white = gdImageColorAllocate(C, gdRedMax, gdGreenMax, gdBlueMax);
    black = gdImageColorAllocate(C, 0, 0, 0);

    if (maxSX > minSX && maxSY > minSY)
	gdImageFilledRectangle(C, minSX, minSY, maxSX-1, maxSY-1, black);

    rc = imageDiff (A, B, C, minSX, minSY, black, white);

    if ((argc > 3) && ((f = fopen(argv[3], "wb")))) {
	gdImagePng (C, f);
	fclose(f);
    }
    else
        gdImagePng (C, stdout);

    gdImageDestroy(A);
    gdImageDestroy(B);
    gdImageDestroy(C);

    return (rc ? EXIT_FAILURE : EXIT_SUCCESS);
}

