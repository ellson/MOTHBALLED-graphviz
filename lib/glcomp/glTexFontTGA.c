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
#include <png.h>


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
	size_t bread;
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
unsigned char *fontGetRGB (FILE *s, size_t size)
{
	unsigned char *rgb;
	unsigned char temp;
	size_t bread;
	int i;

	rgb = malloc (size * 3); 
	
	if (rgb == NULL)
		return 0;

	bread = fread (rgb, sizeof (unsigned char), size * (size_t) 3, s);

	if (bread != size * 3)
	{
		free (rgb);
		return 0;
	}

    /* TGA is stored in BGR, make it RGB */
	for (i = 0; i < (int)size * 3; i += 3)
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
	size_t bread;

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
unsigned char *load_png_font(char* file_name,int *imageWidth,int *imageHeight)
{
	unsigned char *imageData = NULL;
	unsigned char header[8];
	int i,ii,b0,b1,b2,b3,pixeloffset;
	long int c;
	png_structp png_ptr;
	png_infop info_ptr;
	png_infop end_info;
	png_bytepp row_pointers; //actual image data
	int is_png=0;
	FILE *fp = fopen(file_name, "rb");
    if (!fp)
    {
        return (unsigned char*)0;
    }
    fread(header, 1, 8, fp);
    is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png)
    {
		printf ("glcomp error:file is not a valid PNG file\n");
		return (unsigned char*)0;
    }

    png_ptr = png_create_read_struct
       (PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
    if (!png_ptr)
	{
		printf ("glcomp error:file can not be read\n");
		return (unsigned char*)0;
	}

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr,
           (png_infopp)NULL, (png_infopp)NULL);
		printf ("glcomp error:PNG file header is corrupted\n");
		return (unsigned char*)0;
    }

    end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
		printf ("glcomp error:PNG file header is corrupted\n");
		png_destroy_read_struct(&png_ptr, &info_ptr,
          (png_infopp)NULL);
		return (unsigned char*)0;
    }


	png_init_io(png_ptr, fp);

    png_set_sig_bytes(png_ptr, 8);	//pass signature bytes
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL); //read real image data

   row_pointers = png_malloc(png_ptr,
	   info_ptr->height*sizeof(png_bytepp));
   row_pointers = png_get_rows(png_ptr, info_ptr);
	*imageWidth=info_ptr->width;
	*imageHeight=info_ptr->height;
	texFormat = GL_ALPHA;	//it is always GL_ALPHA, we dont need textures have their own colors
	imageData=malloc(info_ptr->height*info_ptr->width);
	c=0;
	//decide what pixel offset to use, ro
	pixeloffset = png_get_rowbytes(png_ptr, info_ptr)/info_ptr->width;

	b0=-1;b1=-1;b2=-1;b3=-1;

	for (i=0; i < (int)info_ptr->height; i++)
   {
	   for (ii=0;ii < (int)png_get_rowbytes(png_ptr, info_ptr); ii=ii+pixeloffset)	
		{
			imageData[c]=row_pointers[info_ptr->height-i-1][ii];

			if(
					(b0!=row_pointers[info_ptr->height-i-1][ii])
							||
					(b1!=row_pointers[info_ptr->height-i-1][ii+1])
							||
					(b2!=row_pointers[info_ptr->height-i-1][ii+2])
							||
					(b3!=row_pointers[info_ptr->height-i-1][ii+3])
					)
			{
				b0=row_pointers[info_ptr->height-i-1][ii];
				b1=row_pointers[info_ptr->height-i-1][ii+1];
				b2=row_pointers[info_ptr->height-i-1][ii+2];
				b3=row_pointers[info_ptr->height-i-1][ii+3];

			}


			c++;
		}
   }
	//cleaning libpng mess
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	png_free(png_ptr, row_pointers);
   return imageData;
}


/*#define imageWidth 256
#define imageHeight 256 
static GLubyte imageData[imageWidth][imageHeight][4];*/

int fontLoadPNG (char *name, int id)
{
	GLubyte *imageData = NULL;
	int imageWidth,imageHeight,idx2,c;
	
//	imageData = fontGetData (s, size, imageBits);
	imageData =load_png_font(name,&imageWidth,&imageHeight);

	c=0;
	idx2=0;
/*	for (idx=0;idx < imageWidth*imageHeight+30000;idx=idx+1)
		{
			if (c!=imageData[idx])
			{
				c=imageData[idx];
				printf ("%i) %i \n ",idx2,imageData[idx]);
			}

			idx2++;
			c=((((idx&0x8)==0)^((idx2&0x8))==0))*255;
				imageData[idx][idx2][0] = c;
				imageData[idx][idx2][1] = c;
				imageData[idx][idx2][2] = c;
				imageData[idx][idx2][3] = 255;

		}*/


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
//	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE , GL_DECAL);

	glTexImage2D (GL_TEXTURE_2D, 0, texFormat, imageWidth, imageHeight, 0, texFormat, GL_UNSIGNED_BYTE, imageData);
//	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	/* release data, its been uploaded */

	return 1;
}

