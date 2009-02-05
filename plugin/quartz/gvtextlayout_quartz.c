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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "gvplugin_textlayout.h"
#include "gvplugin_quartz.h"

#if __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050

static CTLineRef quartz_new_layout(char* fontname, CGFloat fontsize, char* text)
{
	CFStringRef fontnameref = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)fontname, strlen(fontname), kCFStringEncodingUTF8, FALSE);
	CFStringRef textref = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)text, strlen(text), kCFStringEncodingUTF8, FALSE);
	CTLineRef line = NULL;
	
	if (fontnameref && textref) {
		/* set up the Core Text line */
		CTFontRef font = CTFontCreateWithName(fontnameref, fontsize, NULL);
		
		CFDictionaryRef attributes = CFDictionaryCreate(
			kCFAllocatorDefault,
			(const void**)&kCTFontAttributeName,
			(const void**)&font,
			1,
			&kCFTypeDictionaryKeyCallBacks,
			&kCFTypeDictionaryValueCallBacks);
		CFAttributedStringRef attributed = CFAttributedStringCreate(kCFAllocatorDefault, textref, attributes);
		line = CTLineCreateWithAttributedString(attributed);
		
		CFRelease(attributed);
		CFRelease(attributes);
		CFRelease(font);
	}
	
	if (textref)
		CFRelease(textref);
	if (fontnameref)
		CFRelease(fontnameref);
	return line;
}


boolean quartz_textlayout(textpara_t *para, char **fontpath)
{
	CTLineRef line = quartz_new_layout(para->fontname, para->fontsize, para->str);
	if (line)
	{
		/* get the typographic bounds */
		CGFloat ascent = 0.0;
		CGFloat descent = 0.0;
		CGFloat leading = 0.0;
		double width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
		CGFloat height = ascent + descent;
		
		/* report the layout */
		para->layout = (void*)line;
		para->free_layout = &quartz_free_layout;
		para->width = width;
		para->height = leading == 0.0 ? height * 1.2 : height + leading;	/* if no leading, use 20% of height */
		para->yoffset_layout = ascent;
		para->yoffset_centerline = 0;
		return TRUE;
	}
	else
		return FALSE;
};

void quartz_free_layout(void *layout)
{
	if (layout)
		CFRelease(layout);
};

void quartzgen_textpara(GVJ_t *job, pointf p, textpara_t *para)
{
	CGContextRef context = (CGContextRef)job->context;

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
	
	CTLineRef layout;
	if (para->free_layout == &quartz_free_layout)
		layout = (CTLineRef)para->layout;
	else
		layout = quartz_new_layout(para->fontname, para->fontsize, para->str);
		
	/* draw it */
	CGContextSetTextPosition(context, p.x, p.y);
	if (job->obj->pencolor.u.RGBA [0] == 0.0 && job->obj->pencolor.u.RGBA [1] == 0.0 && job->obj->pencolor.u.RGBA [2] == 0.0 && job->obj->pencolor.u.RGBA [3] == 1.0)
		/* optimized case for foreground color black */
		CTLineDraw(layout, context);
	else
	{
		/* non-black foreground color, need to disassemble line manually and color it */
		CFArrayRef runs = CTLineGetGlyphRuns(layout);
		CFIndex run_count = CFArrayGetCount(runs);
		CFIndex run_index;
		for (run_index = 0; run_index < run_count; ++run_index)
		{
			CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, run_index);
			CTFontRef run_font = CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
			CGFontRef glyph_font = CTFontCopyGraphicsFont(run_font, NULL);
			CFIndex glyph_count = CTRunGetGlyphCount(run);
			CGGlyph glyphs[glyph_count];
			CGPoint positions[glyph_count];
			CFRange everything = CFRangeMake(0, 0);
			CTRunGetGlyphs(run, everything, glyphs);
			CTRunGetPositions(run, everything, positions);
			
			CGContextSetFont(context, glyph_font);
			CGContextSetFontSize(context, CTFontGetSize(run_font));
			CGContextSetRGBFillColor(context, job->obj->pencolor.u.RGBA [0], job->obj->pencolor.u.RGBA [1], job->obj->pencolor.u.RGBA [2], job->obj->pencolor.u.RGBA [3]);
			CGContextShowGlyphsAtPositions(context, glyphs, positions, glyph_count);
		
			CGFontRelease(glyph_font);
		}
	}

	if (para->free_layout != &quartz_free_layout)
		CFRelease(layout);
}

#endif

static gvtextlayout_engine_t quartz_textlayout_engine = {
    quartz_textlayout
};

gvplugin_installed_t gvtextlayout_quartz_types[] = {
    {0, "textlayout", 8, &quartz_textlayout_engine, NULL},
    {0, NULL, 0, NULL, NULL}
};
