#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include "gd.h"

#define FALSE 0
#define TRUE (!FALSE)

int
main (int argc, char *argv[])
{
#ifdef HAVE_FREETYPE2
  	FILE *out;
	int transparent, green, black;
	gdImagePtr im;

	im = gdImageCreateTrueColor(100,100);

        black =  gdImageColorResolveAlpha(im, 0, 0, 0, gdAlphaOpaque);
        green =  gdImageColorResolveAlpha(im, 0, gdGreenMax, 0, gdAlphaOpaque);
        transparent = gdImageColorResolveAlpha(im,
                        gdRedMax-1, gdGreenMax, gdBlueMax, gdAlphaTransparent);
        gdImageColorTransparent(im, transparent);

	/* Blending must be off to lay a transparent basecolor.
                Nothing to blend with anyway. */
        gdImageAlphaBlending(im, FALSE);
        gdImageFill (im, im->sx/2, im->sy/2, transparent);
        /* Blend everything else together,
                especially fonts over non-transparent backgrounds */
        gdImageAlphaBlending(im, TRUE);

	gdImageFilledRectangle (im, 30, 30, 70, 70, green);
	gdImageStringFT (im, NULL, black, "Times", 18, 0, 50, 50, "Hello");

	gdImageSaveAlpha (im, TRUE);
#ifdef HAVE_LIBPNG
	out = fopen ("testtr.png", "wb");
	gdImagePng (im, out);
	fclose (out);
#else
	fprintf(stderr, "Compiled without libpng support\n");
#endif /* HAVE_LIBPNG */
	gdImageDestroy (im);

	return 0;
#else
	fprintf(stderr, "Compiled without freetype support\n");
	return 0;
#endif /* HAVE_FREETYPE2 */
}
