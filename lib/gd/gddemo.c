
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "gd.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"

int
main (void)
{
#ifdef HAVE_LIBPNG
  /* Input and output files */
  FILE *in;
  FILE *out;

  /* Input and output images */
  gdImagePtr im_in = 0, im_out = 0;

  /* Brush image */
  gdImagePtr brush;

  /* Color indexes */
  int white;
  int blue;
  int red;
  int green;

  /* Points for polygon */
  gdPoint points[3];
  int i;

  /* gd fonts for font test */
  gdFontPtr fonts[5];
  int y;

  /* Create output image, in true color. */
  im_out = gdImageCreateTrueColor (256 + 384, 384);
  /* 2.0.2: first color allocated would automatically be background in a 
     palette based image. Since this is a truecolor image, with an 
     automatic background of black, we must fill it explicitly. */
  white = gdImageColorAllocate (im_out, 255, 255, 255);
  gdImageFilledRectangle (im_out, 0, 0, gdImageSX (im_out),
			  gdImageSY (im_out), white);

  /* Set transparent color. */
  gdImageColorTransparent (im_out, white);

  /* Try to load demoin.png and paste part of it into the
     output image. */
  in = fopen ("demoin.png", "rb");
  if (!in)
    {
      fprintf (stderr, "Can't load source image; this demo\n");
      fprintf (stderr, "is much more impressive if demoin.png\n");
      fprintf (stderr, "is available.\n");
      im_in = 0;
    }
  else
    {
      int a;
      im_in = gdImageCreateFromPng (in);
      fclose (in);
      /* Now copy, and magnify as we do so */
      gdImageCopyResampled (im_out, im_in, 32, 32, 0, 0, 192, 192, 255, 255);
      /* Now display variously rotated space shuttles in a circle of our own */
      for (a = 0; (a < 360); a += 45)
	{
	  int cx = cos (a * .0174532925) * 128;
	  int cy = -sin (a * .0174532925) * 128;
	  gdImageCopyRotated (im_out, im_in,
			      256 + 192 + cx, 192 + cy,
			      0, 0, gdImageSX (im_in), gdImageSY (im_in), a);
	}
    }
  red = gdImageColorAllocate (im_out, 255, 0, 0);
  green = gdImageColorAllocate (im_out, 0, 255, 0);
  blue = gdImageColorAllocate (im_out, 0, 0, 255);
  /* Fat Rectangle */
  gdImageSetThickness (im_out, 4);
  gdImageLine (im_out, 16, 16, 240, 16, green);
  gdImageLine (im_out, 240, 16, 240, 240, green);
  gdImageLine (im_out, 240, 240, 16, 240, green);
  gdImageLine (im_out, 16, 240, 16, 16, green);
  gdImageSetThickness (im_out, 1);
  /* Circle */
  gdImageArc (im_out, 128, 128, 60, 20, 0, 720, blue);
  /* Arc */
  gdImageArc (im_out, 128, 128, 40, 40, 90, 270, blue);
  /* Flood fill: doesn't do much on a continuously
     variable tone jpeg original. */
  gdImageFill (im_out, 8, 8, blue);
  /* Polygon */
  points[0].x = 64;
  points[0].y = 0;
  points[1].x = 0;
  points[1].y = 128;
  points[2].x = 128;
  points[2].y = 128;
  gdImageFilledPolygon (im_out, points, 3, green);
  /* 2.0.12: Antialiased Polygon */
  gdImageSetAntiAliased (im_out, green);
  for (i = 0; (i < 3); i++)
    {
      points[i].x += 128;
    }
  gdImageFilledPolygon (im_out, points, 3, gdAntiAliased);
  /* Brush. A fairly wild example also involving a line style! */
  if (im_in)
    {
      int style[8];
      brush = gdImageCreateTrueColor (16, 16);
      gdImageCopyResized (brush, im_in,
			  0, 0, 0, 0,
			  gdImageSX (brush), gdImageSY (brush),
			  gdImageSX (im_in), gdImageSY (im_in));
      gdImageSetBrush (im_out, brush);
      /* With a style, so they won't overprint each other.
         Normally, they would, yielding a fat-brush effect. */
      style[0] = 0;
      style[1] = 0;
      style[2] = 0;
      style[3] = 0;
      style[4] = 0;
      style[5] = 0;
      style[6] = 0;
      style[7] = 1;
      gdImageSetStyle (im_out, style, 8);
      /* Draw the styled, brushed line */
      gdImageLine (im_out, 0, 255, 255, 0, gdStyledBrushed);
    }
  /* Text (non-truetype; see gdtestft for a freetype demo) */
  fonts[0] = gdFontGetTiny ();
  fonts[1] = gdFontGetSmall ();
  fonts[2] = gdFontGetMediumBold ();
  fonts[3] = gdFontGetLarge ();
  fonts[4] = gdFontGetGiant ();
  y = 0;
  for (i = 0; (i <= 4); i++)
    {
      gdImageString (im_out, fonts[i], 32, 32 + y, (unsigned char *) "hi",
		     red);
      y += fonts[i]->h;
    }
  y = 0;
  for (i = 0; (i <= 4); i++)
    {
      gdImageStringUp (im_out, fonts[i], 64 + y, 64,
		       (unsigned char *) "hi", red);
      y += fonts[i]->h;
    }
  /* Random antialiased lines; coordinates all over the image, 
     but the output will respect a small clipping rectangle */
  gdImageSetClip (im_out, 0, gdImageSY (im_out) - 100,
		  100, gdImageSY (im_out));
  /* Fixed seed for reproducibility of results */
  srand (100);
  for (i = 0; (i < 100); i++)
    {
      int x1 = rand () % gdImageSX (im_out);
      int y1 = rand () % gdImageSY (im_out);
      int x2 = rand () % gdImageSX (im_out);
      int y2 = rand () % gdImageSY (im_out);
      gdImageSetAntiAliased (im_out, white); 
      gdImageLine (im_out, x1, y1, x2, y2, gdAntiAliased); 
    }
  /* Make output image interlaced (progressive, in the case of JPEG) */
  gdImageInterlace (im_out, 1);
  out = fopen ("demoout.png", "wb");
  /* Write PNG */
  gdImagePng (im_out, out);
  fclose (out);
  out = fopen ("demoout.gif", "wb");
  /* Write GIF (2.0.28) */
  gdImageGif (im_out, out);
  fclose (out);
  /* 2.0.12: also write a paletteized png comparable to the gif */
  out = fopen ("demooutp.png", "wb");
  gdImageTrueColorToPalette (im_out, 0, 256);
  gdImagePng (im_out, out);
  fclose (out);
  gdImageDestroy (im_out);
  if (im_in)
    {
      gdImageDestroy (im_in);
    }
#else
  fprintf (stderr, "No PNG library support.\n");
#endif /* HAVE_LIBPNG */
  return 0;
}
