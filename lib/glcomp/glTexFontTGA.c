/*
    Copyright (c) 1999 Nate Miller
    
    Notice: Usage of any code in this file is subject to the rules
    described in the LICENSE.TXT file included in this directory.
    Reading, compiling, or otherwise using this code constitutes
    automatic acceptance of the rules in said text file.

    File        -- glTexFontTGA.c
    Date        -- 5/30/99
    Author      -- Nate 'm|d' Miller
    Contact     -- vandals1@home.com
    Web         -- http://members.home.com/vandals1

    Change Log
    **********
    6/11/99 - added support for 8bit images, changed commenting
    5/30/99 - original file
*/
#include "glTexFont.h"
#include "glTexFontTGA.h"
#include "glTexFontInclude.h"

GLenum texFormat;

/*
=============
fontCheckSize

Make sure its a power of 2.
=============
*/
int fontCheckSize (int x)
{
	if (x == 2	 || x == 4 || 
		x == 8	 || x == 16 || 
		x == 32  || x == 64 ||
		x == 128 || x == 256 || x == 512)
		return 1;
	else return 0;
}
/*
=============
fontGetRGBA

Reads in RGBA data for a 32bit image. 
=============
*/
unsigned char *fontGetRGBA (FILE *s, int size)
{
	unsigned char *rgba;
	unsigned char temp;
	int bread;
	int i;

	rgba = malloc (size * 4); 

	if (rgba == NULL)
		return 0;

	bread = fread (rgba, sizeof (unsigned char), size * 4, s); 
	
    /* TGA is stored in BGRA, make it RGBA */
	if (bread != size * 4)
	{
		free (rgba);
		return 0;
	}

	for (i = 0; i < size * 4; i += 4 )
	{
		temp = rgba[i];
		rgba[i] = rgba[i + 2];
		rgba[i + 2] = temp;
	}

	texFormat = GL_RGBA;
	return rgba;
}
/*
=============
fontGetRGB

Reads in RGB data for a 24bit image. 
=============
*/
unsigned char *fontGetRGB (FILE *s, int size)
{
	unsigned char *rgb;
	unsigned char temp;
	int bread;
	int i;

	rgb = malloc (size * 3); 
	
	if (rgb == NULL)
		return 0;

	bread = fread (rgb, sizeof (unsigned char), size * 3, s);

	if (bread != size * 3)
	{
		free (rgb);
		return 0;
	}

    /* TGA is stored in BGR, make it RGB */
	for (i = 0; i < size * 3; i += 3)
	{
		temp = rgb[i];
		rgb[i] = rgb[i + 2];
		rgb[i + 2] = temp;
	}
	
	texFormat = GL_RGB;

	return rgb;
}
/*
=============
fontGetGray

Reads a gray scale image. 
=============
*/
unsigned char *fontGetGray (FILE *s, int size)
{
	unsigned char *grayData;
	int bread;

	grayData = malloc (size);

	if (grayData == NULL)
		return 0;

	bread = fread (grayData, sizeof (unsigned char), size, s);

	if (bread != size)
	{
		free (grayData);
		return 0;
	}
	
	texFormat = GL_ALPHA;

	return grayData;
}
/*
=============
fontGetData

Gets the image data for the specified bit depth.
=============
*/
unsigned char *fontGetData (FILE *s, int sz, int iBits)
{
	if (iBits == 32)
		return fontGetRGBA (s, sz);
	else if (iBits == 24)
		return fontGetRGB (s, sz);	
    else if (iBits == 8)
        return fontGetGray (s, sz);
    else
        return 0;
}
/*
=============
fontLoadTGA

Loads up a targa file.  Supported types are 8,24 and 32 uncompressed images.  
=============
*/
int fontLoadTGA (char *name, int id)
{
	unsigned char type[4];
	unsigned char info[7];
	unsigned char *imageData = NULL;
	int imageWidth, imageHeight;
	int imageBits, size;
	FILE *s;
	
#ifdef _WIN32
	if (!(s = fopen (name, "r+bt")))
#else
// FIXME - the file exists but I still error on this fopen() call
fprintf(stderr,"font: %s\n", name);
	if (!(s = fopen (name, "rb")))
#endif
		return FONT_FILE_NOT_FOUND;

	fread (&type, sizeof (char), 3, s); // read in colormap info and image type, byte 0 ignored
	fseek (s, 12, SEEK_SET);			// seek past the header and useless info
	fread (&info, sizeof (char), 6, s);

	if (type[1] != 0 || (type[2] != 2 && type[2] != 3))
		return FONT_BAD_IMAGE_TYPE;
    
    imageWidth = info[0] + info[1] * 256; 
	imageHeight = info[2] + info[3] * 256;
	imageBits =	info[4]; 

	size = imageWidth * imageHeight; 

	/* make sure dimension is a power of 2 */
	if (!fontCheckSize (imageWidth) || !fontCheckSize (imageHeight))
		return FONT_BAD_DIMENSION;

	/* make sure we are loading a supported type */
	if (imageBits != 32 && imageBits != 24 && imageBits != 8)
		return FONT_BAD_BITS;

	imageData = fontGetData (s, size, imageBits);
	
	fclose (s);

	/* no image data */
	if (imageData == NULL)
		return FONT_BAD_DATA;

	glBindTexture (GL_TEXTURE_2D, id);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/* glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); */
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); */
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D (GL_TEXTURE_2D, 0, texFormat, imageWidth, imageHeight, 0, texFormat, GL_UNSIGNED_BYTE, imageData);

	/* release data, its been uploaded */
	free (imageData);

	return 1;
}

