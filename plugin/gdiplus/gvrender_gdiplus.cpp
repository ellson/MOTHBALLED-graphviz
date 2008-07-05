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
#include <string.h>

#include "gvplugin_device.h"
#include "gvplugin_render.h"
#include "graph.h"
#include "gvplugin_gdiplus.h"

#include <memory>
#include <vector>

extern "C" size_t gvdevice_write(GVJ_t *job, const unsigned char *s, unsigned int len);

using namespace std;
using namespace Gdiplus;

/* Graphics for internal use, so that we can record image etc. for subsequent retrieval off the job struct */
struct ImageGraphics: public Graphics
{
	GraphicsContext *context;
	Image *image;
	IStream *stream;
	
	ImageGraphics(GraphicsContext *newContext, Image *newImage, IStream *newStream):
		Graphics(newImage), context(newContext), image(newImage), stream(newStream)
	{
	}
};

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

static void gdiplusgen_begin_job(GVJ_t *job)
{
	if (!job->external_context)
		job->context = NULL;
}

static void gdiplusgen_end_job(GVJ_t *job)
{
	if (!job->external_context) {
		Graphics *context = (Graphics *)job->context;
		
		/* flush and delete the graphics */
		ImageGraphics *imageGraphics = static_cast<ImageGraphics *>(context);
		GraphicsContext *graphicsContext = imageGraphics->context;
		Image *image = imageGraphics->image;
		IStream *stream = imageGraphics->stream;
		delete imageGraphics;
		
		switch (job->device.id) {
			case FORMAT_EMF:
			case FORMAT_EMFPLUS:
				break;
			default:
				SaveBitmapToStream(*static_cast<Bitmap *>(image), stream, job->device.id);
				break;
		}
		
		delete image;	/* NOTE: in the case of EMF, this actually flushes out the image to the underlying stream */

		/* blast the streamed buffer back to the gvdevice */
		/* NOTE: this is somewhat inefficient since we should be streaming directly to gvdevice rather than buffering first */
		/* ... however, GDI+ requires any such direct IStream to implement Seek Read, Write, Stat methods and gvdevice really only offers a write-once model */
		HGLOBAL buffer = NULL;
		GetHGlobalFromStream(stream, &buffer);
		stream->Release();
		gvdevice_write(job, (unsigned char*)GlobalLock(buffer), GlobalSize(buffer));
		GlobalFree(buffer);
			
		/* since this is an internal job, shut down GDI+ */
		delete graphicsContext;
	}
}

static void gdiplusgen_begin_page(GVJ_t *job)
{
	if (!job->external_context && !job->context) {
		/* since this is an internal job, start up GDI+ */
		GraphicsContext *context = new GraphicsContext();
		
		/* allocate memory and attach stream to it */
		HGLOBAL buffer = GlobalAlloc(GMEM_MOVEABLE, 0);
		IStream *stream = NULL;
		CreateStreamOnHGlobal(buffer, FALSE, &stream);	/* FALSE means don't deallocate buffer when releasing stream */
		
		Image *image;
		switch (job->device.id) {
		
		case FORMAT_EMF:
		case FORMAT_EMFPLUS:
			/* EMF image */
			image = new Metafile (stream,
				DeviceContext().hdc,
				RectF(0.0f, 0.0f, job->width, job->height),
				MetafileFrameUnitPixel,
				job->device.id == FORMAT_EMFPLUS ? EmfTypeEmfPlusOnly : EmfTypeEmfOnly);
				/* output in EMF for wider compatibility; output in EMF+ for antialiasing etc. */
			break;
			
		default:
			/* bitmap image */
			image = new Bitmap (job->width, job->height, PixelFormat32bppARGB);
			break;
		}
		
		job->context = new ImageGraphics(context, image, stream);
	}
	
	/* start graphics state */
	Graphics *context = (Graphics *)job->context;
	context->SetSmoothingMode(SmoothingModeHighQuality);
	
	/* set up the context transformation */
	/* NOTE: we need to shift by height of graph and do a reflection before applying given transformations */
	context->ResetTransform();
	context->TranslateTransform(0, job->height);
	context->ScaleTransform(job->scale.x, -job->scale.y);
	context->RotateTransform(job->rotation);
	context->TranslateTransform(job->translation.x, job->translation.y);
}

static int CALLBACK fetch_first_font(
	const LOGFONTA *logFont,
	const TEXTMETRICA *textMetrics,
	DWORD fontType,
	LPARAM lParam)
{
	/* save the first font we see in the font enumeration */
	*((LOGFONTA *)lParam) = *logFont;
	return 0;
}

static auto_ptr<Font> find_font(char *fontname, double fontsize)
{
	/* search for a font with this name. if we can't find it, use the generic serif instead */
	/* NOTE: GDI font search is more comprehensive than GDI+ and will look for variants e.g. Arial Bold */
	DeviceContext reference;
	LOGFONTA font_to_find;
	font_to_find.lfCharSet = ANSI_CHARSET;
	strncpy(font_to_find.lfFaceName, fontname, sizeof(font_to_find.lfFaceName) - 1);
	font_to_find.lfFaceName[sizeof(font_to_find.lfFaceName) - 1] = '\0';
	font_to_find.lfPitchAndFamily = 0;
	LOGFONTA found_font;
	if (EnumFontFamiliesExA(reference.hdc,
		&font_to_find,
		fetch_first_font,
		(LPARAM)&found_font,
		0) == 0) {
		found_font.lfHeight = (LONG)-fontsize;
		found_font.lfWidth = 0;
		return auto_ptr<Font>(new Font(reference.hdc, &found_font));
	}
	else
		return auto_ptr<Font>(new Font(FontFamily::GenericSerif(), fontsize));
}

static void gdiplusgen_textpara(GVJ_t *job, pointf p, textpara_t *para)
{
	/* convert incoming UTF8 string to wide chars */
	/* NOTE: conversion is 1 or more UTF8 chars to 1 wide char */
	int wide_count = MultiByteToWideChar(CP_UTF8, 0, para->str, -1, NULL, 0);
	if (wide_count > 1) {
		vector<WCHAR> wide_str(wide_count);
		MultiByteToWideChar(CP_UTF8, 0, para->str, -1, &wide_str.front(), wide_count);
		
		/* adjust text position */
		switch (para->just) {
		case 'r':
			p.x -= para->width;
			break;
		case 'l':
			p.x -= 0.0;
			break;
		case 'n':
		default:
			p.x -= para->width / 2.0;
			break;
		}
		p.y -= para->yoffset_centerline;

		Graphics *context = (Graphics *)job->context;

		/* reverse the reflection done in begin_page */
		GraphicsState saved = context->Save();
		double center = para->fontsize / 2.0;
		context->TranslateTransform(p.x, p.y + center);
		context->ScaleTransform(1.0, -1.0);

		/* draw the text */
		SolidBrush brush(Color(job->obj->pencolor.u.rgba [3], job->obj->pencolor.u.rgba [0], job->obj->pencolor.u.rgba [1], job->obj->pencolor.u.rgba [2]));
		context->DrawString(&wide_str.front(), wide_count - 1, find_font(para->fontname, para->fontsize).get(), PointF(0, -center), &brush);
		context->Restore(saved);
	}
}


static vector<PointF> points(pointf *A, int n)
{
	/* convert Graphviz pointf (struct of double) to GDI+ PointF (struct of float) */
	vector<PointF> newPoints;
	for (int i = 0; i < n; ++i)
		newPoints.push_back(PointF(A[i].x, A[i].y));
	return newPoints;
}

static void gdiplusgen_path(GVJ_t *job, const GraphicsPath *path, int filled)
{
	Graphics *context = (Graphics *)job->context;
	
	/* fill the given path with job fill color */
	if (filled) {
		SolidBrush fill_brush(Color(job->obj->fillcolor.u.rgba [3], job->obj->fillcolor.u.rgba [0], job->obj->fillcolor.u.rgba [1], job->obj->fillcolor.u.rgba [2]));
		context->FillPath(&fill_brush, path);
	}
	
	/* draw the given path from job pen color and pen width */
	Pen draw_pen(Color(job->obj->pencolor.u.rgba [3], job->obj->pencolor.u.rgba [0], job->obj->pencolor.u.rgba [1], job->obj->pencolor.u.rgba [2]),
		job->obj->penwidth);
	context->DrawPath(&draw_pen, path);
}

static void gdiplusgen_ellipse(GVJ_t *job, pointf *A, int filled)
{
	/* convert ellipse into path */
	GraphicsPath path;
	double dx = A[1].x - A[0].x;
	double dy = A[1].y - A[0].y;
	path.AddEllipse(RectF(A[0].x - dx, A[0].y - dy, dx * 2.0, dy * 2.0));
	
	/* draw the path */
	gdiplusgen_path(job, &path, filled);
}

static void gdiplusgen_polygon(GVJ_t *job, pointf *A, int n, int filled)
{
	/* convert polygon into path */
	GraphicsPath path;
	path.AddPolygon(&points(A,n).front(), n);
	
	/* draw the path */
	gdiplusgen_path(job, &path, filled);
}

static void
gdiplusgen_bezier(GVJ_t *job, pointf *A, int n, int arrow_at_start,
	     int arrow_at_end, int filled)
{
	/* convert the beziers into path */
	GraphicsPath path;
	path.AddBeziers(&points(A,n).front(), n);
	
	/* draw the path */
	gdiplusgen_path(job, &path, filled);
}

static void gdiplusgen_polyline(GVJ_t *job, pointf *A, int n)
{
	/* convert the lines into path */
	GraphicsPath path;
	path.AddLines(&points(A,n).front(), n);
	
	/* draw the path */
	gdiplusgen_path(job, &path, 0);
}

static gvrender_engine_t gdiplusgen_engine = {
    gdiplusgen_begin_job,
    gdiplusgen_end_job,
    0,							/* gdiplusgen_begin_graph */
    0,							/* gdiplusgen_end_graph */
    0,							/* gdiplusgen_begin_layer */
    0,							/* gdiplusgen_end_layer */
    gdiplusgen_begin_page,
    0,							/* gdiplusgen_end_page */
    0,							/* gdiplusgen_begin_cluster */
    0,							/* gdiplusgen_end_cluster */
    0,							/* gdiplusgen_begin_nodes */
    0,							/* gdiplusgen_end_nodes */
    0,							/* gdiplusgen_begin_edges */
    0,							/* gdiplusgen_end_edges */
    0,							/* gdiplusgen_begin_node */
    0,							/* gdiplusgen_end_node */
    0,							/* gdiplusgen_begin_edge */
    0,							/* gdiplusgen_end_edge */
    0,							/* gdiplusgen_begin_anchor */
    0,							/* gdiplusgen_end_anchor */
    gdiplusgen_textpara,
    0,
    gdiplusgen_ellipse,
    gdiplusgen_polygon,
    gdiplusgen_bezier,
    gdiplusgen_polyline,
    0,							/* gdiplusgen_comment */
    0,							/* gdiplusgen_library_shape */
};

static gvrender_features_t render_features_gdiplus = {
	GVRENDER_DOES_TRANSFORM, /* flags */
    4.,							/* default pad - graph units */
    NULL,						/* knowncolors */
    0,							/* sizeof knowncolors */
    RGBA_BYTE				/* color_type */
};

static gvdevice_features_t device_features_gdiplus = {
    GVDEVICE_BINARY_FORMAT
      | GVDEVICE_DOES_TRUECOLOR,/* flags */
    {0.,0.},                    /* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {96.,96.}                  /* dpi */
};

gvplugin_installed_t gvrender_gdiplus_types[] = {
    {0, "gdiplus", 1, &gdiplusgen_engine, &render_features_gdiplus},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_gdiplus_types[] = {
	{FORMAT_BMP, "bmp:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_EMF, "emf:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_EMFPLUS, "emfplus:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_GIF, "gif:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_JPEG, "jpe:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_JPEG, "jpeg:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_JPEG, "jpg:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_PNG, "png:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_TIFF, "tif:gdiplus", 8, NULL, &device_features_gdiplus},
	{FORMAT_TIFF, "tiff:gdiplus", 8, NULL, &device_features_gdiplus},
	{0, NULL, 0, NULL, NULL}
};
