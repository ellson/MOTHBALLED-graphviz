
/*
   add ability to load xpm files to gd, requires the xpm
   library.
   Caolan.McNamara@ul.ie
   http://www.csn.ul.ie/~caolan
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gd.h"
#include "gdhelpers.h"

#ifndef HAVE_LIBXPM
BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm(char *filename) {
  fprintf(stderr, "libgd was not built with xpm support\n");
  return (NULL);
}

#else

#include <X11/xpm.h>

BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm(char *filename) {
  XpmInfo info;
  XpmImage image;
  int i, j, k, number;
  char buf[5];
  gdImagePtr im = 0;
  int *pointer;
  int red = 0, green = 0, blue = 0;
  int *colors;
  int ret;

  ret = XpmReadFileToXpmImage(filename, &image, &info);
  if (ret != XpmSuccess) return 0;

  if (!(im = gdImageCreate(image.width, image.height))) return 0;

  number = image.ncolors;
  if (overflow2(sizeof(int), number)) {
    return 0;
  }
  colors = (int *)gdMalloc(sizeof(int) * number);
  if (colors == NULL) return (0);
  for (i = 0; i < number; i++) {
    switch (strlen(image.colorTable[i].c_color)) {
      case 4:
        buf[1] = '\0';
        buf[0] = image.colorTable[i].c_color[1];
        red = strtol(buf, NULL, 16);

        buf[0] = image.colorTable[i].c_color[3];
        green = strtol(buf, NULL, 16);

        buf[0] = image.colorTable[i].c_color[5];
        blue = strtol(buf, NULL, 16);
        break;
      case 7:
        buf[2] = '\0';
        buf[0] = image.colorTable[i].c_color[1];
        buf[1] = image.colorTable[i].c_color[2];
        red = strtol(buf, NULL, 16);

        buf[0] = image.colorTable[i].c_color[3];
        buf[1] = image.colorTable[i].c_color[4];
        green = strtol(buf, NULL, 16);

        buf[0] = image.colorTable[i].c_color[5];
        buf[1] = image.colorTable[i].c_color[6];
        blue = strtol(buf, NULL, 16);
        break;
      case 10:
        buf[3] = '\0';
        buf[0] = image.colorTable[i].c_color[1];
        buf[1] = image.colorTable[i].c_color[2];
        buf[2] = image.colorTable[i].c_color[3];
        red = strtol(buf, NULL, 16);
        red /= 64;

        buf[0] = image.colorTable[i].c_color[4];
        buf[1] = image.colorTable[i].c_color[5];
        buf[2] = image.colorTable[i].c_color[6];
        green = strtol(buf, NULL, 16);
        green /= 64;

        buf[0] = image.colorTable[i].c_color[7];
        buf[1] = image.colorTable[i].c_color[8];
        buf[2] = image.colorTable[i].c_color[9];
        blue = strtol(buf, NULL, 16);
        blue /= 64;
        break;
      case 13:
        buf[4] = '\0';
        buf[0] = image.colorTable[i].c_color[1];
        buf[1] = image.colorTable[i].c_color[2];
        buf[2] = image.colorTable[i].c_color[3];
        buf[3] = image.colorTable[i].c_color[4];
        red = strtol(buf, NULL, 16);
        red /= 256;

        buf[0] = image.colorTable[i].c_color[5];
        buf[1] = image.colorTable[i].c_color[6];
        buf[2] = image.colorTable[i].c_color[7];
        buf[3] = image.colorTable[i].c_color[8];
        green = strtol(buf, NULL, 16);
        green /= 256;

        buf[0] = image.colorTable[i].c_color[9];
        buf[1] = image.colorTable[i].c_color[10];
        buf[2] = image.colorTable[i].c_color[11];
        buf[3] = image.colorTable[i].c_color[12];
        blue = strtol(buf, NULL, 16);
        blue /= 256;
        break;
    }

    colors[i] = gdImageColorResolve(im, red, green, blue);
    if (colors[i] == -1) fprintf(stderr, "ARRRGH\n");
  }

  pointer = (int *)image.data;
  for (i = 0; i < image.height; i++) {
    for (j = 0; j < image.width; j++) {
      k = *pointer++;
      gdImageSetPixel(im, j, i, colors[k]);
    }
  }
  gdFree(colors);
  return (im);
}
#endif /* HAVE_LIBXPM */
