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

#ifndef GVPLUGIN_GDIPLUS_H
#define GVPLUGIN_GDIPLUS_H

#include <vector>

#include <Windows.h>
#include <GdiPlus.h>

typedef enum {
	FORMAT_NONE,
	FORMAT_METAFILE,
	FORMAT_BMP,
	FORMAT_EMF,
	FORMAT_EMFPLUS,
	FORMAT_GIF,
	FORMAT_JPEG,
	FORMAT_PNG,
	FORMAT_TIFF
} format_type;

/* RAII for GetDC/ReleaseDC */

struct DeviceContext
{
	HWND hwnd;
	HDC hdc;
	
	DeviceContext(HWND wnd = NULL): hwnd(wnd), hdc(GetDC(wnd))
	{
	}
	
	~DeviceContext()
	{
		ReleaseDC(hwnd, hdc);
	}

};

/* textlayout etc. */

struct Layout
{
	Gdiplus::Font* font;
	std::vector<WCHAR> text;
	
	Layout(char *fontname, double fontsize, char* string);
	~Layout();
};

static const int BYTES_PER_PIXEL = 4;		/* bytes per pixel */

void gdiplus_free_layout(void *layout);

void UseGdiplus();
const Gdiplus::StringFormat* GetGenericTypographic();
void SaveBitmapToStream(Gdiplus::Bitmap &bitmap, IStream *stream, int format);

#endif
