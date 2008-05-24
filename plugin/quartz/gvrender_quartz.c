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

#include "gvplugin_quartz.h"

static void quartzgen_begin_job(GVJ_t *job)
{
	if (!job->external_context)
		job->context = NULL;
}

static void quartzgen_end_job(GVJ_t *job)
{
	if (!job->external_context) {
		CGContextRef context = (CGContextRef)job->context;
		switch (job->device.id) {
		
		case FORMAT_PDF:
			/* save the PDF */
			CGPDFContextClose(context);
			break;
			
		default:	/* bitmap formats */
			{
				/* create an image destination */
				CGDataConsumerRef data_consumer = CGDataConsumerCreate(job, &device_data_consumer_callbacks);
				CGImageDestinationRef image_destination = CGImageDestinationCreateWithDataConsumer(data_consumer, format_uti[job->device.id], 1, NULL);
				
				/* add the bitmap image to the destination and save it */
				CGImageRef image = CGBitmapContextCreateImage(context);
				CGImageDestinationAddImage(image_destination, image, NULL);
				CGImageDestinationFinalize(image_destination);
				
				/* clean up */
				if (image_destination)
					CFRelease(image_destination);
				CGImageRelease(image);
				CGDataConsumerRelease(data_consumer);
			}
			break;
		}
		CGContextRelease(context);
	}
}

static void quartzgen_begin_page(GVJ_t *job)
{
	CGRect bounds = CGRectMake(0.0, 0.0, job->width, job->height);
	
	if (!job->external_context && !job->context) {
		
		switch (job->device.id) {
		
		case FORMAT_PDF:
			{
				/* create the auxiliary info for PDF content, author and title */
				CFStringRef auxiliaryKeys[] = {
					kCGPDFContextCreator,
					kCGPDFContextAuthor,
					kCGPDFContextTitle
				};
				CFStringRef auxiliaryValues[] = {
					CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%s %s"), job->common->info[0], job->common->info[1]),
					CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8 *)job->common->user, strlen(job->common->user), kCFStringEncodingUTF8, false, kCFAllocatorNull),
					job->obj->type == ROOTGRAPH_OBJTYPE ?
						CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8 *)job->obj->u.g->name, strlen(job->obj->u.g->name), kCFStringEncodingUTF8, false, kCFAllocatorNull)
						: CFSTR("")
				};
				CFDictionaryRef auxiliaryInfo = CFDictionaryCreate(
					kCFAllocatorDefault,
					(const void **)&auxiliaryKeys,
					(const void **)&auxiliaryValues,
					sizeof(auxiliaryValues)/sizeof(auxiliaryValues[0]),
					&kCFTypeDictionaryKeyCallBacks,
					&kCFTypeDictionaryValueCallBacks
				);
				
				/* create a PDF for drawing into */
				CGDataConsumerRef data_consumer = CGDataConsumerCreate(job, &device_data_consumer_callbacks);
				job->context = CGPDFContextCreate(data_consumer, &bounds, auxiliaryInfo);
				
				/* clean up */
				CGDataConsumerRelease(data_consumer);
				CFRelease(auxiliaryInfo);
				int i;
				for (i = 0; i < sizeof(auxiliaryValues)/sizeof(auxiliaryValues[0]); ++i)
					CFRelease(auxiliaryValues[i]);
			}
			break;
		
		default: /* bitmap formats */
			{	
				/* create a true color bitmap for drawing into */
				CGColorSpaceRef color_space = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
				job->context = CGBitmapContextCreate(
					NULL,														/* data: let Quartz take care of memory management */
					job->width,													/* width in pixels */
					job->height,												/* height in pixels */
					BITS_PER_COMPONENT,											/* bits per component */
					(job->width * BYTES_PER_PIXEL + BYTE_ALIGN) & ~BYTE_ALIGN,	/* bytes per row: align to 16 byte boundary */
					color_space,												/* color space: sRGB */
					kCGImageAlphaPremultipliedFirst								/* bitmap info: premul ARGB has best support in OS X */
				);
				job->imagedata = CGBitmapContextGetData((CGContextRef)job->context);
				
				/* clean up */
				CGColorSpaceRelease(color_space);
			}
			break;
		}
		
	}
	
	/* start the page (if this is a paged context) and graphics state */
	CGContextRef context = (CGContextRef)job->context;
	CGContextBeginPage(context, &bounds);
	CGContextSaveGState(context);
	CGContextSetMiterLimit(context, 1.0);
	
	/* set up the context transformation */
	CGContextScaleCTM(context, job->scale.x, job->scale.y);
	CGContextRotateCTM(context, -job->rotation * M_PI / 180.0);
	CGContextTranslateCTM(context, job->translation.x, job->translation.y);
}

static void quartzgen_end_page(GVJ_t *job)
{
	/* end the page (if this is a paged context) and graphics state */
	CGContextRef context = (CGContextRef)job->context;
	CGContextRestoreGState(context);
	CGContextEndPage(context);
}

static void quartzgen_begin_anchor(GVJ_t *job, char *url, char *tooltip, char *target)
{
    pointf *url_map = job->obj->url_map_p;
	if (url && url_map) {
		/* set up the hyperlink to the given url */
		CGContextRef context = (CGContextRef)job->context;
		CFURLRef uri = CFURLCreateWithBytes (kCFAllocatorDefault, (const UInt8 *)url, strlen(url), kCFStringEncodingUTF8, NULL);
		CGPDFContextSetURLForRect(
			context,
			uri,
			/* need to reverse the CTM on the area to get it to work */
			CGRectApplyAffineTransform(CGRectMake(url_map[0].x, url_map[0].y, url_map[1].x - url_map[0].x, url_map[1].y - url_map[0].y),CGContextGetCTM(context))
		);

		/* clean up */
		CFRelease(uri);
	}
}

static void quartzgen_textpara(GVJ_t *job, pointf p, textpara_t *para)
{
	CGContextRef context = (CGContextRef)job->context;
	
	CFStringRef str = CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8 *)para->str, strlen(para->str), kCFStringEncodingUTF8, FALSE, kCFAllocatorNull);
	if (str) {
	/* set up the Core Text line */
	CFStringRef attribute_keys[] = {
		kCTFontAttributeName,
		kCTForegroundColorAttributeName
	};
	CFStringRef fontname = CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8 *)para->fontname, strlen(para->fontname), kCFStringEncodingUTF8, FALSE, kCFAllocatorNull);
	CFTypeRef attribute_values[] = {
		CTFontCreateWithName(fontname, para->fontsize, NULL),
		CGColorCreateGenericRGB(job->obj->pencolor.u.RGBA [0], job->obj->pencolor.u.RGBA [1], job->obj->pencolor.u.RGBA [2], job->obj->pencolor.u.RGBA [3])
	};
	CFDictionaryRef attributes = CFDictionaryCreate(
		kCFAllocatorDefault,
		(const void**)&attribute_keys,
		(const void**)&attribute_values,
		sizeof(attribute_values) / sizeof(attribute_values[0]),
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);
	CFAttributedStringRef attributed_str = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);
	CTLineRef line = CTLineCreateWithAttributedString(attributed_str);
	
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
		p.y += para->yoffset_centerline;
	
	/* draw it */
	CGContextSetTextPosition(context, p.x, p.y);
	CTLineDraw(line, context);
	
	/* clean up */
	CFRelease(line);
	CFRelease(attributed_str);
	CFRelease(str);
	CFRelease(attributes);
	int i;
	for (i = 0; i < sizeof(attribute_values) / sizeof(attribute_values[0]); ++i)
		CFRelease(attribute_values[i]);
	CFRelease(fontname);
	}

}

static void quartzgen_path(GVJ_t *job, int filled)
{
	CGContextRef context = (CGContextRef)job->context;
	
	/* set up colors */
	if (filled)
		CGContextSetRGBFillColor(context, job->obj->fillcolor.u.RGBA [0], job->obj->fillcolor.u.RGBA [1], job->obj->fillcolor.u.RGBA [2], job->obj->fillcolor.u.RGBA [3]);
	CGContextSetRGBStrokeColor(context, job->obj->pencolor.u.RGBA [0], job->obj->pencolor.u.RGBA [1], job->obj->pencolor.u.RGBA [2], job->obj->pencolor.u.RGBA [3]);
	CGContextSetLineWidth(context, job->obj->penwidth); // *job->scale.x);
	
	/* draw the path */
	CGContextDrawPath(context, filled ? kCGPathFillStroke : kCGPathStroke);
}

static void quartzgen_ellipse(GVJ_t *job, pointf *A, int filled)
{
	/* convert ellipse into the current path */
	CGContextRef context = (CGContextRef)job->context;
	double dx = A[1].x - A[0].x;
	double dy = A[1].y - A[0].y;
	CGContextAddEllipseInRect(context, CGRectMake(A[0].x - dx, A[0].y - dy, dx * 2.0, dy * 2.0));

	/* draw the ellipse */
	quartzgen_path(job, filled);
}

static void quartzgen_polygon(GVJ_t *job, pointf *A, int n, int filled)
{
	/* convert polygon into the current path */
	CGContextRef context = (CGContextRef)job->context;
	CGContextMoveToPoint(context, A[0].x, A[0].y);
	int i;
	for (i = 1; i < n; ++i)
		CGContextAddLineToPoint(context, A[i].x, A[i].y);
	CGContextClosePath(context);

	/* draw the ellipse */
	quartzgen_path(job, filled);
}

static void
quartzgen_bezier(GVJ_t *job, pointf *A, int n, int arrow_at_start,
	     int arrow_at_end, int filled)
{
	/* convert bezier into the current path */
	CGContextRef context = (CGContextRef)job->context;
	CGContextMoveToPoint(context, A[0].x, A[0].y);
	int i;
	for (i = 1; i < n; i += 3)
		CGContextAddCurveToPoint(context, A[i].x, A[i].y, A[i+1].x, A[i+1].y, A[i+2].x, A[i+2].y);
	
	/* draw the ellipse */
	quartzgen_path(job, filled);
}

static void quartzgen_polyline(GVJ_t *job, pointf *A, int n)
{
	/* convert polyline into the current path */
	CGContextRef context = (CGContextRef)job->context;
	CGContextMoveToPoint(context, A[0].x, A[0].y);
	int i;
	for (i = 1; i < n; ++i)
		CGContextAddLineToPoint(context, A[i].x, A[i].y);
	
	/* draw the ellipse */
	quartzgen_path(job, FALSE);
}

static gvrender_engine_t quartzgen_engine = {
    quartzgen_begin_job,
    quartzgen_end_job,
    0,							/* quartzgen_begin_graph */
    0,							/* quartzgen_end_graph */
    0,							/* quartzgen_begin_layer */
    0,							/* quartzgen_end_layer */
    quartzgen_begin_page,
    quartzgen_end_page,
    0,							/* quartzgen_begin_cluster */
    0,							/* quartzgen_end_cluster */
    0,							/* quartzgen_begin_nodes */
    0,							/* quartzgen_end_nodes */
    0,							/* quartzgen_begin_edges */
    0,							/* quartzgen_end_edges */
    0,							/* quartzgen_begin_node */
    0,							/* quartzgen_end_node */
    0,							/* quartzgen_begin_edge */
    0,							/* quartzgen_end_edge */
    quartzgen_begin_anchor,
    0,							/* quartzgen_end_anchor */
    quartzgen_textpara,
    0,
    quartzgen_ellipse,
    quartzgen_polygon,
    quartzgen_bezier,
    quartzgen_polyline,
    0,							/* quartzgen_comment */
    0,							/* quartzgen_library_shape */
};

static gvrender_features_t render_features_quartz = {
    GVRENDER_DOES_MAPS
		| GVRENDER_DOES_MAP_RECTANGLE
		| GVRENDER_DOES_TRANSFORM, /* flags */
    4.,							/* default pad - graph units */
    NULL,						/* knowncolors */
    0,							/* sizeof knowncolors */
    RGBA_DOUBLE				/* color_type */
};

static gvdevice_features_t device_features_quartz = {
    GVDEVICE_BINARY_FORMAT
      | GVDEVICE_DOES_TRUECOLOR,/* flags */
    {0.,0.},                    /* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {96.,96.}                  /* dpi */
};

static gvdevice_features_t device_features_quartz_paged = {
	GVDEVICE_DOES_PAGES
	  | GVDEVICE_BINARY_FORMAT
      | GVDEVICE_DOES_TRUECOLOR
	  | GVRENDER_NO_BG,			/* flags */
    {36.,36.},                    /* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {72.,72.}                  /* dpi */
};

gvplugin_installed_t gvrender_quartz_types[] = {
    {0, "quartz", 1, &quartzgen_engine, &render_features_quartz},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_quartz_types[] = {
	{FORMAT_BMP, "bmp:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_GIF, "gif:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_EXR, "exr:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_JPEG, "jpe:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_JPEG, "jpeg:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_JPEG, "jpg:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_JPEG2000, "jp2:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_PDF, "pdf:quartz", 8, NULL, &device_features_quartz_paged},
	{FORMAT_PICT, "pct:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_PICT, "pict:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_PNG, "png:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_PSD, "psd:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_SGI, "sgi:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_TIFF, "tif:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_TIFF, "tiff:quartz", 8, NULL, &device_features_quartz},
	{FORMAT_TGA, "tga:quartz", 8, NULL, &device_features_quartz},
	{0, NULL, 0, NULL, NULL}
};
