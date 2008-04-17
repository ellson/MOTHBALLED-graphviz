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

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "gvplugin_loadimage.h"

#include "gvplugin_quartz.h"

static size_t file_data_provider_get_bytes(void *info, void *buffer, size_t count)
{
	return fread(buffer, 1, count, (FILE*)info);
}

static off_t file_data_provider_skip_forward(void *info, off_t count)
{
	fseek((FILE*)info, count, SEEK_CUR);
	return count;
}

static void file_data_provider_rewind(void *info)
{
	fseek((FILE*)info, 0, SEEK_SET);
}

/* bridge FILE* to a sequential CGDataProvider */
static CGDataProviderSequentialCallbacks file_data_provider_callbacks = {
	0,
	file_data_provider_get_bytes,
	file_data_provider_skip_forward,
	file_data_provider_rewind,
	NULL
};

static void quartz_freeimage(usershape_t *us)
{
	CGImageRelease((CGImageRef)us->data);
}

static CGImageRef quartz_loadimage(GVJ_t * job, usershape_t *us)
{
    assert(job);
    assert(us);
    assert(us->name);

    if (us->data && us->datafree != quartz_freeimage) {
	     us->datafree(us);        /* free incompatible cache data */
	     us->data = NULL;
	     us->datafree = NULL;
	}
    
    if (!us->data) { /* read file into cache */
		if (!gvusershape_file_access(us))
			return NULL;
			
		/* match usershape format to a UTI for type hinting, if possible */
		format_type hint_format_type;
		switch (us->type) {
		case FT_BMP:
			hint_format_type = FORMAT_BMP;
			break;
		case FT_GIF:
			hint_format_type = FORMAT_GIF;
			break;
		case FT_PNG:
			hint_format_type = FORMAT_PNG;
			break;
		case FT_JPEG:
			hint_format_type = FORMAT_JPEG;
			break;
		case FT_PDF:
			hint_format_type = FORMAT_PDF;
			break;
		default:
			hint_format_type = FORMAT_NONE;
			break;
		}
		CFDictionaryRef options = hint_format_type == FORMAT_NONE ? NULL : CFDictionaryCreate(
			kCFAllocatorDefault,
			(const void **)&kCGImageSourceTypeIdentifierHint,
			(const void **)(format_uti + hint_format_type),
			1,
			&kCFTypeDictionaryKeyCallBacks,
			&kCFTypeDictionaryValueCallBacks);

		/* get first image from usershape file */
		CGDataProviderRef data_provider = CGDataProviderCreateSequential(us->f, &file_data_provider_callbacks);
		CGImageSourceRef image_source = CGImageSourceCreateWithDataProvider(data_provider, options);
		us->data = CGImageSourceCreateImageAtIndex(image_source, 0, NULL);
		
		/* clean up */
		if (us->data)
			us->datafree = quartz_freeimage;
		if (image_source)
			CFRelease(image_source);
		CGDataProviderRelease(data_provider);
		if (options)
			CFRelease(options);
			
		gvusershape_file_release(us);
    }
    return (CGImageRef)(us->data);
}

static void quartz_loadimage_quartz(GVJ_t * job, usershape_t *us, boxf b, boolean filled)
{
	/* get the image from usershape details, then blit it to the context */
	CGImageRef image = quartz_loadimage(job, us);
	if (image)
		CGContextDrawImage((CGContextRef)job->context, CGRectMake(b.LL.x, b.LL.y, b.UR.x - b.LL.x, b.UR.y - b.LL.y), image);
}

static gvloadimage_engine_t engine = {
    quartz_loadimage_quartz
};

gvplugin_installed_t gvloadimage_quartz_types[] = {
	{FORMAT_BMP, "bmp:quartz", 8, &engine, NULL},
	{FORMAT_GIF, "gif:quartz", 8, &engine, NULL},
	{FORMAT_JPEG, "jpe:quartz", 8, &engine, NULL},
	{FORMAT_JPEG, "jpeg:quartz", 8, &engine, NULL},
	{FORMAT_JPEG, "jpg:quartz", 8, &engine, NULL},
	{FORMAT_PDF, "pdf:quartz", 8, &engine, NULL},
	{FORMAT_PNG, "png:quartz", 8, &engine, NULL},
	{0, NULL, 0, NULL, NULL}
};