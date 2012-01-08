/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gvplugin_device.h"

#ifdef HAVE_WEBP

typedef enum {
    FORMAT_WEBP,
} format_type;

static void
Y_inv ( unsigned int width, unsigned int height, char *data)
{
        unsigned int x, y, *a, *b, t;

	a = (unsigned int*)data;
        b = a + (height-1) * width;
        for (y = 0; y < height/2; y++) {
                for (x = 0; x < width; x++) {
			t = *a;
			*a++ = *b;
			*b++ = t;
                }
                b -= 2*width;
        }
}

static void webp_format(GVJ_t * job)
{
#if 0
    ILuint	ImgId;
    ILenum	Error;
    ILboolean rc;

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

    // cairo's inmemory image format needs inverting for DevIL 
    Y_inv ( job->width, job->height, job->imagedata );
    
    // let the DevIL do its thing
    rc = ilTexImage( job->width, job->height,
    		1,		// Depth
    		4,		// Bpp
    		IL_BGRA,	// Format
    		IL_UNSIGNED_BYTE,// Type
    		job->imagedata);
    
    // output to the provided open file handle
    ilSaveF(job->device.id, job->output_file);
    
    // We're done with the image, so delete it.
    ilDeleteImages(1, &ImgId);
    
    // Simple Error detection loop that displays the Error to the user in a human-readable form.
    while ((Error = ilGetError())) {
    	fprintf(stderr, "Error: %s\n", iluErrorString(Error));
    }
#endif
}

static gvdevice_engine_t webp_engine = {
    NULL,		/* webp_initialize */
    webp_format,
    NULL,		/* webp_finalize */
};

static gvdevice_features_t device_features_webp = {
	GVDEVICE_BINARY_FORMAT        
          | GVDEVICE_NO_WRITER
          | GVDEVICE_DOES_TRUECOLOR,/* flags */
	{0.,0.},                    /* default margin - points */
	{0.,0.},                    /* default page width, height - points */
	{96.,96.},                  /* 96 dpi */
};
#endif

gvplugin_installed_t gvdevice_webp_types[] = {
#ifdef HAVE_WEBP
    {FORMAT_WEBP, "webp:cairo", -1, &webp_engine, &device_features_webp},
#endif
    {0, NULL, 0, NULL, NULL}
};
