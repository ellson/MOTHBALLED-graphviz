
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

/* A short program which converts a .png file into a .gd2 file, for
   your convenience in creating images on the fly from a
   basis image that must be loaded quickly. The .gd2 format
   is not intended to be a general-purpose format. */

int
main (int argc, char **argv)
{
  gdImagePtr im;
  FILE *in, *out;
  int x=0, y=0, w=0, h=0;
  if ((argc != 3) && (argc != 7))
    {
      fprintf (stderr,
	       "Usage: gd2topng filename.gd2 filename.png [srcx srcy width height]\n");
      fprintf (stderr,
	       "If the coordinates are absent,t he entire image is converted.\n");
      exit (1);
    }
  if (argc == 7)
    {
      x = atoi (argv[3]);
      y = atoi (argv[4]);
      w = atoi (argv[5]);
      h = atoi (argv[6]);
    }
  in = fopen (argv[1], "rb");
  if (!in)
    {
      fprintf (stderr, "Input file does not exist!\n");
      exit (1);
    }
  if (argc == 7)
    {
      im = gdImageCreateFromGd2Part (in, x, y, w, h);
    }
  else
    {
      im = gdImageCreateFromGd2 (in);
    }
  fclose (in);
  if (!im)
    {
      fprintf (stderr, "Input is not in GD2 format!\n");
      exit (1);
    }
  out = fopen (argv[2], "wb");
  if (!out)
    {
      fprintf (stderr, "Output file cannot be written to!\n");
      gdImageDestroy (im);
      exit (1);
    }
#ifdef HAVE_LIBPNG
  gdImagePng (im, out);
#else
  fprintf (stderr, "No PNG library support available.\n");
#endif
  fclose (out);
  gdImageDestroy (im);

  return 0;
}
