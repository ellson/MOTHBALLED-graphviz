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

#include <Availability.h>

#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#include <ApplicationServices/ApplicationServices.h>
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#include <CoreGraphics/CoreGraphics.h>
#endif

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1040
extern CFStringRef format_uti [];
#endif

extern CGDataConsumerCallbacks device_data_consumer_callbacks;

typedef struct {
	CGFontRef font;
	CGGlyph* glyphs;
	size_t glyph_count;
} quartz_layout;

/* gvtextlayout_quartz.c in Mac OS X */
/* GVTextLayout.m in iPhoneOS */

void quartz_free_layout(void *layout);
boolean quartz_textlayout(textpara_t *para, char **fontpath);

/* gvrender_quartz.c in Mac OS X */
/* GVTextLayout.m in iPhoneOS */

void quartzgen_textpara(GVJ_t *job, pointf p, textpara_t *para);

#ifdef __cplusplus
}
#endif

#endif
