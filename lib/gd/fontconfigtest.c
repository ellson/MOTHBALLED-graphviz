#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"

int main (int argc, char *argv[])
{
	gdImagePtr im;
	int green, blue;
	gdFTStringExtra se;	
	FILE *out;
	im = gdImageCreateTrueColor(300, 100);
	green = gdImageColorAllocate(im, 128, 255, 128);
	gdImageFilledRectangle(im, 0, 0, 300, 100, green);
	blue = gdImageColorAllocate(im, 128, 128, 255);
	/* Default: fontlist argument is a pathname to a truetype font */
	gdImageStringFT(im, 0, blue, "arial", 
		12, 0, 20, 20, "plain pathname default");
	/* Specifically opt for fontconfig */
	se.flags = gdFTEX_FONTCONFIG;
	gdImageStringFTEx(im, 0, blue, "arial:bold", 
		12, 0, 20, 40, "fontconfig arial:bold", &se);
	/* Change the default to fontconfig */
	if (!gdFTUseFontConfig(1)) {
		fprintf(stderr, "fontconfig not compiled into gd\n");
	}
	/* Use fontconfig by (newly set) default */
	gdImageStringFT(im, 0, blue, "arial:bold",
		12, 0, 20, 60, "fontconfig arial:bold default");
	/* Explicitly use a pathname despite fontconfig default */
	se.flags = gdFTEX_FONTPATHNAME;
	gdImageStringFTEx(im, 0, blue, "arial", 
		12, 0, 20, 80, "plain pathname", &se);
#ifdef HAVE_LIBPNG
	out = fopen("fontconfigtest.png", "wb");
	gdImagePng(im, out);
	fclose(out);
#else
	fprintf(stderr, "PNG not compiled into gd\n");
#endif /* HAVE_LIBPNG */
	return 0;
}

