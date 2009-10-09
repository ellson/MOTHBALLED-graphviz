/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2007 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/
#include "glcompfont.h"
#include <gtk/gtk.h>
#include <png.h>

unsigned char *load_png(char *filename, int *imageWidth, int *imageHeight)
{
	cairo_surface_t* surface;
	cairo_format_t format;
	int w,h;
	unsigned char* d;
	surface=NULL;

/*	d=create_pango_texture("Arial",14,"hello world \ n hello mars",surface,&w,&h);
	*imageWidth=w;
	*imageHeight=h;*/


	surface = cairo_image_surface_create_from_png(filename);
	w=cairo_image_surface_get_width(surface);
	h=cairo_image_surface_get_height(surface);
	*imageWidth=w;
	*imageHeight=h;
    format=cairo_image_surface_get_format(surface);
	d=cairo_image_surface_get_data (surface);
	return d;




}


unsigned char *load_raw(char *filename, int width, int height)
{
    unsigned char *data;
	FILE* file;
    // allocate buffer
    data = malloc(width * height * 3);
    // open and read texture data
    file = fopen(filename, "rb");
    fread(data, width * height * 3, 1, file);
    return data;
}



unsigned char *load_png2(char *file_name, int *imageWidth,int *imageHeight)
{
    unsigned char *imageData = NULL;
    unsigned char header[8];
    int i, ii, b0, b1, b2, b3, pixeloffset;
    long int c;
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_info;
    png_bytepp row_pointers;	//actual image data
    int is_png = 0;
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
	return (unsigned char *) 0;
    }
    fread(header, 1, 8, fp);
    is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
	printf("glcomp error:file is not a valid PNG file\n");
	return (unsigned char *) 0;
    }

    png_ptr = png_create_read_struct
	(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
	printf("glcomp error:file can not be read\n");
	return (unsigned char *) 0;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
	png_destroy_read_struct(&png_ptr,
				(png_infopp) NULL, (png_infopp) NULL);
	printf("glcomp error:PNG file header is corrupted\n");
	return (unsigned char *) 0;
    }

    end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
	printf("glcomp error:PNG file header is corrupted\n");
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	return (unsigned char *) 0;
    }


    png_init_io(png_ptr, fp);

    png_set_sig_bytes(png_ptr, 8);	//pass signature bytes
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);	//read real image data

    row_pointers = png_malloc(png_ptr,
			      info_ptr->height * sizeof(png_bytepp));
    row_pointers = png_get_rows(png_ptr, info_ptr);
    *imageWidth = info_ptr->width;
    *imageHeight = info_ptr->height;
    imageData = malloc(info_ptr->height * info_ptr->width);
    c = 0;
    //decide what pixel offset to use, ro
    pixeloffset = png_get_rowbytes(png_ptr, info_ptr) / info_ptr->width;

    b0 = -1;
    b1 = -1;
    b2 = -1;
    b3 = -1;

    for (i = 0; i < (int) info_ptr->height; i++) 
	{
		for (ii = 0; ii < (int) png_get_rowbytes(png_ptr, info_ptr); ii = ii + pixeloffset) 
		{
			imageData[c] = row_pointers[info_ptr->height - i - 1][ii];
		    if ((b0 != row_pointers[info_ptr->height - i - 1][ii])|| (b1 != row_pointers[info_ptr->height - i - 1][ii + 1])|| (b2 != row_pointers[info_ptr->height - i - 1][ii + 2])|| (b3 != row_pointers[info_ptr->height - i - 1][ii + 3])) 
			{
				b0 = row_pointers[info_ptr->height - i - 1][ii];
				b1 = row_pointers[info_ptr->height - i - 1][ii + 1];
				b2 = row_pointers[info_ptr->height - i - 1][ii + 2];
				b3 = row_pointers[info_ptr->height - i - 1][ii + 3];
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

int glCompLoadFontPNG(char *name, int id)
{
    GLubyte *imageData = NULL;
    int imageWidth, imageHeight, idx2, c;

//      imageData = fontGetData (s, size, imageBits);
    imageData = load_png(name, &imageWidth, &imageHeight);

    c = 0;
    idx2 = 0;
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
	return -1;

    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    /* glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /* glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE , GL_DECAL);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, imageWidth, imageHeight, 0,
		 GL_ALPHA, GL_UNSIGNED_BYTE, imageData);
//      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    /* release data, its been uploaded */

    return 1;
}
