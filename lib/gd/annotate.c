#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gd.h"

/* A neat little utility which adds freetype text to
	existing JPEG images. Type annotate -h for instructions. 
	Thanks to Joel Dubiner for supporting this work. -TBB */

enum
{
  left, center, right
};

int
main (int argc, char *argv[])
{
#ifndef HAVE_LIBFREETYPE
  /* 2.0.12 */
  fprintf (stderr, "annotate is not useful without freetype.\n"
	   "Install freetype, then './configure; make clean; make install'\n"
	   "the gd library again.\n");
  return 1;
#else
  gdImagePtr im;
  char *iin, *iout;
  FILE *in, *out;
  char s[1024];
  int bounds[8];
  int lines = 1;
  int color = gdTrueColor (0, 0, 0);
  char font[1024];
  int size = 12;
  int align = left;
  int x = 0, y = 0;
  char *fontError;
  strcpy (font, "times");
  if (argc != 3)
    {
      fprintf (stderr, "Usage: annotate imagein.jpg imageout.jpg\n\n");
      fprintf (stderr, "Standard input should consist of\n");
      fprintf (stderr, "lines in the following formats:\n");
      fprintf (stderr, "color r g b (0-255 each) [a (0-127, 0 is opaque)]\n");
      fprintf (stderr, "font fontname\n");
      fprintf (stderr, "size pointsize\n");
      fprintf (stderr, "align (left|right|center)\n");
      fprintf (stderr, "move x y\n");
      fprintf (stderr, "text actual-output-text\n\n");
      fprintf (stderr,
	       "If the file 'paris.ttf' exists in /usr/share/fonts/truetype or in a\n");
      fprintf (stderr,
	       "location specified in the GDFONTPATH environment variable, 'font paris' is\n");
      fprintf (stderr,
	       "sufficient. You may also specify the full, rooted path of a font file.\n");
      exit (1);
    }
  iin = argv[1];
  iout = argv[2];
  in = fopen (iin, "rb");
  if (!in)
    {
      fprintf (stderr, "Couldn't open %s\n", iin);
      exit (2);
    }
#ifdef HAVE_LIBJPEG
  im = gdImageCreateFromJpeg (in);
#else
  fprintf (stderr, "No JPEG library support available.\n");
#endif
  fclose (in);
  if (!im)
    {
      fprintf (stderr, "%s did not load properly\n", iin);
      exit (3);
    }
  while (fgets (s, sizeof (s), stdin))
    {
      char *st;
      char *text;
      st = strtok (s, " \t\r\n");
      if (!st)
	{
	  /* Be nice about blank lines */
	  continue;
	}
      if (!strcmp (st, "font"))
	{
	  char *st = strtok (0, " \t\r\n");
	  if (!st)
	    {
	      goto badLine;
	    }
	  strcpy (font, st);
	}
      else if (!strcmp (st, "align"))
	{
	  char *st = strtok (0, " \t\r\n");
	  if (!st)
	    {
	      goto badLine;
	    }
	  if (!strcmp (st, "left"))
	    {
	      align = 0;
	    }
	  else if (!strcmp (st, "center"))
	    {
	      align = 1;
	    }
	  else if (!strcmp (st, "right"))
	    {
	      align = 2;
	    }
	}
      else if (!strcmp (st, "size"))
	{
	  char *st = strtok (0, " \t\r\n");
	  if (!st)
	    {
	      goto badLine;
	    }
	  size = atoi (st);
	}
      else if (!strcmp (st, "color"))
	{
	  char *st = strtok (0, "\r\n");
	  int r, g, b, a = 0;
	  if (!st)
	    {
	      goto badLine;
	    }
	  if (sscanf (st, "%d %d %d %d", &r, &g, &b, &a) < 3)
	    {
	      fprintf (stderr, "Bad color at line %d\n", lines);
	      exit (2);
	    }
	  color = gdTrueColorAlpha (r, g, b, a);
	}
      else if (!strcmp (st, "move"))
	{
	  char *st = strtok (0, "\r\n");
	  if (!st)
	    {
	      goto badLine;
	    }
	  if (sscanf (st, "%d %d", &x, &y) != 2)
	    {
	      fprintf (stderr, "Missing coordinates at line %d\n", lines);
	      exit (3);
	    }
	}
      else if (!strcmp (st, "text"))
	{
	  int rx = x;
	  text = strtok (0, "\r\n");
	  if (!text)
	    {
	      text = "";
	    }
	  gdImageStringFT (0, bounds, color, font, size, 0, x, y, text);
	  switch (align)
	    {
	    case left:
	      break;
	    case center:
	      rx -= (bounds[2] - bounds[0]) / 2;
	      break;
	    case right:
	      rx -= (bounds[2] - bounds[0]);
	      break;
	    }
	  fontError = gdImageStringFT (im, 0, color, font,
				       size, 0, rx, y, text);
	  if (fontError)
	    {
	      fprintf (stderr, "font error at line %d: %s\n", lines,
		       fontError);
	      exit (7);
	    }
	  y -= (bounds[7] - bounds[1]);
	}
      else
	{
	  goto badLine;
	}
      lines++;
      continue;
    badLine:
      fprintf (stderr, "Bad syntax, line %d\n", lines);
      exit (4);
    }
  out = fopen (iout, "wb");
  if (!out)
    {
      fprintf (stderr, "Cannot create %s\n", iout);
      exit (5);
    }
#ifdef HAVE_LIBJPEG
  gdImageJpeg (im, out, 95);
#else
  fprintf (stderr, "No JPEG library support available.\n");
#endif
  gdImageDestroy (im);
  fclose (out);
  return 0;
#endif /* HAVE_LIBFREETYPE */
}
