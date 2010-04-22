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

#include "types.h"
#include "gvcjob.h"

#include "gvplugin_quartz.h"

#if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 20000

#import "GVTextLayout.h"

void *quartz_new_layout(char* fontname, double fontsize, char* text)
{
	return [[GVTextLayout alloc] initWithFontName:fontname fontSize:fontsize text:text];
}

void quartz_size_layout(void *layout, double* width, double* height, double* yoffset_layout)
{
	[(GVTextLayout*)layout sizeUpWidth:width height:height yoffset:yoffset_layout];
}

void quartz_draw_layout(void *layout, CGContextRef context, CGPoint position)
{
	[(GVTextLayout*)layout drawInContext:context atPosition:position];	
}

void quartz_free_layout(void *layout)
{
	[(GVTextLayout*)layout release];
}

static NSString* _defaultFontName = @"TimesNewRomanPSMT";

@implementation GVTextLayout

- (id)initWithFontName:(char*)fontName fontSize:(CGFloat)fontSize text:(char*)text
{
	if (self = [super init])
	{
		_font = nil;
		if (fontName)
			_font = [[UIFont fontWithName:[NSString stringWithUTF8String:fontName] size:fontSize] retain];
		if (!_font)
			_font = [[UIFont fontWithName:_defaultFontName size:fontSize] retain];
			
		_text = text ? [[NSString alloc] initWithUTF8String:text] : nil;
	}
	return self;
}

- (void)sizeUpWidth:(double*)width height:(double*)height yoffset:(double*)yoffset
{
	CGSize size = [_text sizeWithFont:_font];
	CGFloat ascender = _font.ascender;
	
	*width = size.width;
	*height = size.height;
	*yoffset = ascender;
}

- (void)drawAtPoint:(CGPoint)point inContext:(CGContextRef)context
{
	UIGraphicsPushContext(context);
	[_text drawAtPoint:point withFont:_font];
	UIGraphicsPopContext();
}

- (void)drawInContext:(CGContextRef)context atPosition:(CGPoint)position
{
	UIGraphicsPushContext(context);
	[_text drawAtPoint:position withFont:_font];
	UIGraphicsPopContext();	
}

- (void)dealloc
{
	[_font release];
	[_text release];
	
	[super dealloc];
}


@end

#endif
