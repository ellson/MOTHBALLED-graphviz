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

#ifndef GVPLUGIN_QUARTZ_H
#define GVPLUGIN_QUARTZ_H

#include <ApplicationServices/ApplicationServices.h>

typedef enum {
	FORMAT_NONE,
	FORMAT_BMP,
	FORMAT_EXR,
	FORMAT_GIF,
	FORMAT_JPEG,
	FORMAT_JPEG2000,
	FORMAT_PDF,
	FORMAT_PICT,
	FORMAT_PNG,
	FORMAT_PSD,
	FORMAT_SGI,
	FORMAT_TIFF,
	FORMAT_TGA
} format_type;

static const int BYTE_ALIGN = 15;			/* align to 16 bytes */
static const int BITS_PER_COMPONENT = 8;	/* bits per color component */
static const int BYTES_PER_PIXEL = 4;		/* bytes per pixel */

extern CFStringRef format_uti [];
extern CGDataConsumerCallbacks device_data_consumer_callbacks;

#endif
