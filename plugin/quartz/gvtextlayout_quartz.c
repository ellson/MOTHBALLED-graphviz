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

#include <stdlib.h>
#include <string.h>

#include "gvplugin_textlayout.h"
#include "gvplugin_quartz.h"

#if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 30200
#include <CoreText/CoreText.h>
#endif

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050 || __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 30200

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 1060
/* symbol defined in 10.5.x dylib but not in headers */
extern const CFStringRef kCTForegroundColorFromContextAttributeName;
#endif

void *quartz_new_layout(char* fontname, double fontsize, char* text)
{
	CFStringRef fontnameref = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)fontname, strlen(fontname), kCFStringEncodingUTF8, FALSE);
	CFStringRef textref = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)text, strlen(text), kCFStringEncodingUTF8, FALSE);
	CTLineRef line = NULL;
	
	if (fontnameref && textref) {
		/* set up the Core Text line */
		CTFontRef font = CTFontCreateWithName(fontnameref, fontsize, NULL);
		CFTypeRef attributeNames[] = { kCTFontAttributeName, kCTForegroundColorFromContextAttributeName };
		CFTypeRef attributeValues[] = { font, kCFBooleanTrue };
		CFDictionaryRef attributes = CFDictionaryCreate(
			kCFAllocatorDefault,
			(const void**)attributeNames,
			(const void**)attributeValues,
			2,
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
	return (void *)line;
}

void quartz_size_layout(void *layout, double* width, double* height, double* yoffset_layout)
{
	/* get the typographic bounds */
	CGFloat ascent = 0.0;
	CGFloat descent = 0.0;
	CGFloat leading = 0.0;
	
	*width = CTLineGetTypographicBounds((CTLineRef)layout, &ascent, &descent, &leading);
	*height = ascent + descent + leading;
	*yoffset_layout = ascent;
}

void quartz_draw_layout(void *layout, CGContextRef context, CGPoint position)
{
	CGContextSetTextPosition(context, position.x, position.y);
	CTLineDraw((CTLineRef)layout, context);
}

void quartz_free_layout(void *layout)
{
	if (layout)
		CFRelease((CTLineRef)layout);
};

#endif

boolean quartz_textlayout(textspan_t *para, char **fontpath)
{
	void *line = quartz_new_layout(para->font->name, para->font->size, para->str);
	if (line)
	{
		/* report the layout */
		para->layout = (void*)line;
		para->free_layout = &quartz_free_layout;
		quartz_size_layout((void*)line, &para->size.x, &para->size.y, &para->yoffset_layout);
		para->yoffset_centerline = 0.2 * para->font->size;
		return TRUE;
	}
	else
		return FALSE;
};

static gvtextlayout_engine_t quartz_textlayout_engine = {
    quartz_textlayout
};

gvplugin_installed_t gvtextlayout_quartz_types[] = {
    {0, "textlayout", 8, &quartz_textlayout_engine, NULL},
    {0, NULL, 0, NULL, NULL}
};
