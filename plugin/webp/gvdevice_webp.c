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
#include "webp/encode.h"

typedef enum {
    FORMAT_WEBP,
} format_type;

static void webp_format(GVJ_t * job)
{
    WebPPicture picture;
    WebPConfig config;

    if (!WebPPictureInit(&picture) || !WebPConfigInit(&config)) {
	fprintf(stderr, "Error! Version mismatch!\n");
	goto Error;
    }

    picture.width = job->width;
    picture.height = job->height;

    if (!WebPPictureAlloc(&picture)) {
	fprintf(stderr, "Error! Cannot allocate memory\n");
	return;
    }

    if (!WebPPictureImportRGBA(&picture, (const uint8_t * const)(job->imagedata), 4)) {
	fprintf(stderr, "Error! Cannot import picture\n");
	goto Error;
    }

    if (!WebPEncode(&config, &picture)) {
	fprintf(stderr, "Error! Cannot encode picture as WebP\n");
//	fprintf(stderr, "Error code: %d (%s)\n",
//	    pic.error_code, kErrorMessages[picture.error_code]);
	goto Error;
     }
//     DumpPicture(&picture, job->output_file);

//  FIXME - unfinished code

Error:
     WebPPictureFree(&picture);
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
