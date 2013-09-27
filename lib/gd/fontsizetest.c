
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"

void
dosizes (gdImagePtr im, int color, char *fontfile,
	 int x, int y, const char *string)
{
  int brect[8];
  double curang = 0.0;
  char *cp;
  int cursize;
  char buf[60];

  for (cursize = 1; cursize <= 20; cursize++)
    {
      sprintf (buf, "%d: %s", cursize, string);

      /* The case of newlines is taken care of in the gdImageStringTTF call */
#if defined(OLDER_GD)
      cp =
	gdImageStringTTF (im, brect, color, fontfile, cursize, curang, x, y,
			  buf);
#else
      cp =
	gdImageStringFT (im, brect, color, fontfile, cursize, curang, x, y,
			 buf);
#endif
      if (cp)
	fprintf (stderr, "%s\n", cp);
      y += cursize + 4;

/* render the same fontsize with antialiasing turned off */
#if defined(OLDER_GD)
      cp =
	gdImageStringTTF (im, brect, 0 - color, fontfile, cursize, curang, x,
			  y, buf);
#else
      cp =
	gdImageStringFT (im, brect, 0 - color, fontfile, cursize, curang, x,
			 y, buf);
#endif
      if (cp)
	fprintf (stderr, "%s\n", cp);
      y += cursize + 4;
    }
}

void
dotest (char *font, int w, int h, char *string, const char *filename)
{
  gdImagePtr im;
  FILE *out;
  int bg;
  int fc;

  im = gdImageCreate (w, h);
  bg = gdImageColorAllocate (im, 0, 0, 0);

  gdImageFilledRectangle (im, 1, 1, w - 1, h - 1, bg);

  fc = gdImageColorAllocate (im, 255, 192, 192);

  out = fopen (filename, "wb");

  dosizes (im, fc, font, 20, 20, string);

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
  dotest ("times", 400, 600, ".....Hello, there!", "fontsizetest1.png");
  dotest ("cour", 400, 600, ".....Hello, there!", "fontsizetest2.png");
  dotest ("arial", 400, 600, ".....Hello, there!", "fontsizetest3.png");
  dotest ("luximr", 400, 600, ".....Hello, there!", "fontsizetest4.png");
#elif defined(HAVE_LIBJPEG)
  dotest ("times", 400, 600, ".....Hello, there!", "fontsizetest1.jpeg");
  dotest ("cour", 400, 600, ".....Hello, there!", "fontsizetest2.jpeg");
  dotest ("arial", 400, 600, ".....Hello, there!", "fontsizetest3.jpeg");
  dotest ("luximr", 400, 600, ".....Hello, there!", "fontsizetest4.jpeg");
#else
  fprintf (stderr, "no PNG or JPEG support\n");
#endif

  return 0;
}
