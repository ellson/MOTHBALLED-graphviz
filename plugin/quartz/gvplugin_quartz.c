/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2008 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include "gvplugin.h"
#include "gvplugin_quartz.h"

extern gvplugin_installed_t gvrender_quartz_types;
// extern gvplugin_installed_t gvtextlayout_quartz_types;
extern gvplugin_installed_t gvloadimage_quartz_types;
extern gvplugin_installed_t gvdevice_quartz_types;
extern gvplugin_installed_t gvdevice_quartz_types_for_cairo;

/* Uniform Type Identifiers corresponding to each format_type */
CFStringRef format_uti [] = {
	NULL,
	CFSTR("com.microsoft.bmp"),
	CFSTR("com.ilm.openexr-image"),
	CFSTR("com.compuserve.gif"),
	CFSTR("public.jpeg"),
	CFSTR("public.jpeg-2000"),
	CFSTR("com.adobe.pdf"),
	CFSTR("com.apple.pict"),
	CFSTR("public.png"),
	CFSTR("com.adobe.photoshop-image"),
	CFSTR("com.sgi.sgi-image"),
	CFSTR("public.tiff"),
	CFSTR("com.truevision.tga-image")
};

/* data consumer backed by the gvdevice */

extern size_t gvdevice_write(GVJ_t *job, const unsigned char *s, unsigned int len);

static size_t device_data_consumer_put_bytes (void *info, const void *buffer, size_t count)
{
	return gvdevice_write((GVJ_t *)info, (const unsigned char*)buffer, count);
}

CGDataConsumerCallbacks device_data_consumer_callbacks = {
	device_data_consumer_put_bytes,
	NULL
};

static gvplugin_api_t apis[] = {
    {API_render, &gvrender_quartz_types},
  //  {API_textlayout, &gvtextlayout_quartz_types},
	{API_loadimage, &gvloadimage_quartz_types},
    {API_device, &gvdevice_quartz_types},
    {API_device, &gvdevice_quartz_types_for_cairo},
    {(api_t)0, 0},
};

gvplugin_library_t gvplugin_quartz_LTX_library = { "quartz", apis };
