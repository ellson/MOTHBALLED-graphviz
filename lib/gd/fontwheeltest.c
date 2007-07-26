
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include "gd.h"
#define DEGTORAD(x) ( (x) * (2.0 * 3.14159265) / 360.0 )

void
doerr (FILE * err, const char *msg)
{
  if (err)
    {
      fprintf (err, "%s\n", msg);
      fflush (err);
    }
}

void
dowheel (gdImagePtr im, int color, char *fontfile, int fontsize,
	double angle, int x, int y, int offset, char *string)
{
  int brect[8];
  FILE *err;
  double curangrads, curang, x0, y0;
  char *cp;

  err = fopen ("err.out", "a");
  doerr (err, "------------- New fontwheel --------------");
  doerr (err, fontfile);
  doerr (err, string);
  doerr (err, "------------------------------------------");

  for (curang = 0.0; curang < 360.0; curang += angle)
    {
      curangrads = DEGTORAD(curang);
      x0 = x + cos (curangrads) * offset;
      y0 = y - sin (curangrads) * offset;

      /* The case of newlines is taken care of in the gdImageStringTTF call */
#if defined(OLDER_GD)
      cp = gdImageStringTTF (im, brect, color, fontfile, fontsize,
			  curangrads, x0, y0, string);
#else
      cp = gdImageStringFT (im, brect, color, fontfile, fontsize,
			 curangrads, x0, y0, string);
#endif
      if (cp)
	doerr (err, cp);

      gdImagePolygon (im, (gdPointPtr)brect, 4, color);
    }

  fclose (err);
}

#if 0
void
dolines (gdImagePtr im, int color, double incr, int x, int y, int offset,
	 int length)
{
  double curang;
  double angle;
  double x0, x1, y0, y1;
  for (curang = 0.0; curang < 360.0; curang += incr)
    {
      angle = curang * (2.0 * 3.14159265) / 360.0;
      x0 = cos (angle) * offset + x;
      x1 = cos (angle) * (offset + length) + x;
      y0 = sin (angle) * offset + y;
      y1 = sin (angle) * (offset + length) + y;
      gdImageLine (im, x0, y0, x1, y1, color);
    }
}
#endif

void
dotest (char *font, int size, double incr,
	int w, int h, char *string, const char *filename)
{
  gdImagePtr im;
  FILE *out;
  int bg;
  int fc;
#if 0
  int lc;
#endif
  int xc = w / 2;
  int yc = h / 2;

  im = gdImageCreate (w, h);
  bg = gdImageColorAllocate (im, 0, 0, 0);

  gdImageFilledRectangle (im, 1, 1, w - 1, h - 1, bg);

  fc = gdImageColorAllocate (im, 255, 192, 192);
#if 0
  lc = gdImageColorAllocate (im, 192, 255, 255);
#endif

  out = fopen (filename, "wb");

  dowheel (im, fc, font, size, incr, xc, yc, 20, string);
#if 0
  dolines (im, lc, incr, xc, yc, 20, 120);
#endif

#if defined(HAVE_LIBPNG)
  gdImagePng (im, out);
#elif defined(HAVE_LIBJPEG)
  gdImageJpeg (im, out, -1);
#endif

  fclose (out);
}

int
main (int argc, char **argv)
{

#if defined(HAVE_LIBPNG)
  dotest ("times", 16, 20.0, 400, 400, "Hello, there!",
	  "fontwheeltest1.png");
  dotest ("times", 16, 30.0, 400, 400, "Hello, there!",
	  "fontwheeltest2.png");
  dotest ("arial", 16, 45.0, 400, 400, "Hello, there!",
	  "fontwheeltest3.png");
  dotest ("arial", 16, 90.0, 400, 400, "Hello\nthere!",
	  "fontwheeltest4.png");
#elif defined(HAVE_LIBJPEG)
  dotest ("times", 16, 20.0, 400, 400, "Hello, there!",
	  "fontwheeltest1.jpeg");
  dotest ("times", 16, 30.0, 400, 400, "Hello, there!",
	  "fontwheeltest2.jpeg");
  dotest ("arial", 16, 45.0, 400, 400, "Hello, there!",
	  "fontwheeltest3.jpeg");
  dotest ("arial", 16, 90.0, 400, 400, "Hello\nthere!",
	  "fontwheeltest4.jpeg");
#else
  fprintf (stderr, "no PNG or JPEG support\n");
#endif

  return 0;
}
