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

#include "gvplugin_gdiplus.h"

#include <vector>

extern gvplugin_installed_t gvrender_gdiplus_types;
// extern gvplugin_installed_t gvtextlayout_gdiplus_types;
extern gvplugin_installed_t gvloadimage_gdiplus_types;
extern gvplugin_installed_t gvdevice_gdiplus_types;
extern gvplugin_installed_t gvdevice_gdiplus_types_for_cairo;

using namespace std;
using namespace Gdiplus;

GraphicsContext::GraphicsContext()
{
	GdiplusStartupInput startupInput;
	GdiplusStartup(&token, &startupInput, NULL);
}

GraphicsContext::~GraphicsContext()
{
	GdiplusShutdown(token);
}

/* class id corresponding to each format_type */
static GUID format_id [] = {
	GUID_NULL,
	ImageFormatBMP,
	ImageFormatEMF,
	ImageFormatEMF,
	ImageFormatGIF,
	ImageFormatJPEG,
	ImageFormatPNG,
	ImageFormatTIFF
};

void SaveBitmapToStream(Bitmap &bitmap, IStream *stream, int format)
{
	/* search the encoders for one that matches our device id, then save the bitmap there */
	UINT encoderNum;
	UINT encoderSize;
	GetImageEncodersSize(&encoderNum, &encoderSize);
	vector<char> codec_buffer(encoderSize);
	ImageCodecInfo *codecs = (ImageCodecInfo *)&codec_buffer.front();
	GetImageEncoders(encoderNum, encoderSize, codecs);
	for (UINT i = 0; i < encoderNum; ++i)
		if (memcmp(&(format_id[format]), &codecs[i].FormatID, sizeof(GUID)) == 0) {
			bitmap.Save(stream, &codecs[i].Clsid, NULL);
			break;
		}
}

static gvplugin_api_t apis[] = {
    {API_render, &gvrender_gdiplus_types},
  //  {API_textlayout, &gvtextlayout_gdiplus_types},
	{API_loadimage, &gvloadimage_gdiplus_types},
    {API_device, &gvdevice_gdiplus_types},
	{API_device, &gvdevice_gdiplus_types_for_cairo},
    {(api_t)0, 0},
};

gvplugin_library_t gvplugin_gdiplus_LTX_library = { "gdiplus", apis };
