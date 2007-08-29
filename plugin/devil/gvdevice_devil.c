/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif
#include "gvplugin_device.h"
#include <IL/il.h>
#include <IL/ilu.h>

static void
Y_inv ( unsigned int width, unsigned int height, unsigned char *data)
{
        unsigned int x, y, rowsize, i;
        unsigned char tmp, *data2;

#define STRIDE 4

        rowsize = width * STRIDE;
        data2 = data + (height-1) * rowsize;
        for (y = 0; y < height/2; y++) {
                for (x = 0; x < width; x++) {
                        for (i = 0; i < STRIDE; i++) {
                                tmp = *data;
                                *data++ = *data2;
                                *data2++ = tmp;
                        }
                }
                data2 -= 2*rowsize;
        }
}

static void devil_format(GVJ_t * job, unsigned int width, unsigned int height, unsigned char *data)
{
    ILuint	ImgId;
    ILenum	Error;
    ILboolean rc;

#ifdef HAVE_SETMODE
#ifdef O_BINARY
    /*
     * Windows will do \n -> \r\n  translations on stdout
     * unless told otherwise.
     */
    setmode(fileno(job->output_file), O_BINARY);
#endif
#endif

    // Check if the shared lib's version matches the executable's version.
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
    	iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) {
    	fprintf(stderr, "DevIL version is different...exiting!\n");
    }

    // Initialize DevIL.
    ilInit();

    // Generate the main image name to use.
    ilGenImages(1, &ImgId);

    // Bind this image name.
    ilBindImage(ImgId);

    Y_inv ( width, height, data );
    
    rc = ilTexImage( width, height,
    		1,		// Depth
    		4,		// Bpp
    		IL_BGRA,	// Format
    		IL_UNSIGNED_BYTE,// Type
    		data);
    
#if 1
    ilSaveF(job->device.id, job->output_file);
#endif

#if 0
    ilEnable(IL_FILE_OVERWRITE);
    ilSaveImage("test-devil.bmp");
#endif
    
    // We're done with the image, so let's delete it.
    ilDeleteImages(1, &ImgId);
    
    // Simple Error detection loop that displays the Error to the user in a human-readable form.
    while ((Error = ilGetError())) {
    	fprintf(stderr, "Error: %s\n", iluErrorString(Error));
    }
}

static gvdevice_engine_t devil_engine = {
    NULL,
    devil_format,
    NULL,
};

static gvdevice_features_t devil_features = {
    0,  /* flags */
};

gvplugin_installed_t gvdevice_devil_types[] = {
    {IL_BMP, "bmp:cairo", -1, &devil_engine, &devil_features},
//    {IL_GIF, "gif:cairo", -1, &devil_engine, &devil_features},
    {IL_JPG, "jpg:cairo", -1, &devil_engine, &devil_features},
    {IL_JPG, "jpe:cairo", -1, &devil_engine, &devil_features},
    {IL_JPG, "jpeg:cairo", -1, &devil_engine, &devil_features},
    {IL_PNG, "png:cairo", -1, &devil_engine, &devil_features},
    {IL_TIF, "tif:cairo", -1, &devil_engine, &devil_features},
    {IL_TIF, "tiff:cairo", -1, &devil_engine, &devil_features},
    {IL_TGA, "tga:cairo", -1, &devil_engine, &devil_features},
    {0, NULL, 0, NULL, NULL}
};
