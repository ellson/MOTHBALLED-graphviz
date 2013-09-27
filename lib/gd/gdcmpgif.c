#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h> /* For unlink function */
#endif

#include "gd.h"

/* A short program which converts a .png file into a .gd file, for
	your convenience in creating images on the fly from a
	basis image that must be loaded quickly. The .gd format
	is not intended to be a general-purpose format. */

void CompareImages(char *msg, gdImagePtr im1, gdImagePtr im2);


int main(int argc, char **argv)
{
	gdImagePtr im1, im2;
	FILE *in;

	if (argc != 3) {
		fprintf(stderr, "Usage: gdcmpgif filename.gif filename.gif\n");
		exit(1);
	}
	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "Input file does not exist!\n");
		exit(1);
	}
	im1 = gdImageCreateFromGif(in);
	fclose(in);

	if (!im1) {
		fprintf(stderr, "Input is not in GIF format!\n");
		exit(1);
	}

	in = fopen(argv[2], "rb");
	if (!in) {
		fprintf(stderr, "Input file 2 does not exist!\n");
		exit(1);
	}
	im2 = gdImageCreateFromGif(in);
	fclose(in);

	if (!im2) {
		fprintf(stderr, "Input 2 is not in GIF format!\n");
		exit(1);
	}

	CompareImages("gdcmpgif", im1, im2);

	gdImageDestroy(im1);
        gdImageDestroy(im2);

	return 0;
}

void CompareImages(char *msg, gdImagePtr im1, gdImagePtr im2)
{
	int cmpRes;

	cmpRes = gdImageCompare(im1, im2);

	if (cmpRes & GD_CMP_IMAGE) {
		printf("%%%s: ERROR images differ: BAD\n",msg);
	} else if (cmpRes != 0) {
		printf("%%%s: WARNING images differ: WARNING - Probably OK\n",msg);
	} else {
		printf("%%%s: OK\n",msg);
		return;
	}

	if (cmpRes & (GD_CMP_SIZE_X + GD_CMP_SIZE_Y)) {
		printf("-%s: INFO image sizes differ\n",msg);
	}

	if (cmpRes & GD_CMP_NUM_COLORS) {
		printf("-%s: INFO number of pallette entries differ %d Vs. %d\n",msg,
			im1->colorsTotal, im2->colorsTotal);
	}

	if (cmpRes & GD_CMP_COLOR) {
		printf("-%s: INFO actual colours of pixels differ\n",msg);
	}
}


