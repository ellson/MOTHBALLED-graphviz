
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

/* A short program which converts a .png file into a .gd file, for
   your convenience in creating images on the fly from a
   basis image that must be loaded quickly. The .gd format
   is not intended to be a general-purpose format. */

int main(int argc, char **argv) {
  gdImagePtr im;
  FILE *in, *out;
  if (argc != 3) {
    fprintf(stderr, "Usage: pngtogd filename.png filename.gd\n");
    exit(1);
  }
  in = fopen(argv[1], "rb");
  if (!in) {
    fprintf(stderr, "Input file does not exist!\n");
    exit(1);
  }
#ifdef HAVE_LIBPNG
  im = gdImageCreateFromPng(in);
#else
  fprintf(stderr, "No PNG library support available.\n");
#endif
  fclose(in);
  if (!im) {
    fprintf(stderr, "Input is not in PNG format!\n");
    exit(1);
  }
  out = fopen(argv[2], "wb");
  if (!out) {
    fprintf(stderr, "Output file cannot be written to!\n");
    gdImageDestroy(im);
    exit(1);
  }
  gdImageGd(im, out);
  fclose(out);
  gdImageDestroy(im);

  return 0;
}
