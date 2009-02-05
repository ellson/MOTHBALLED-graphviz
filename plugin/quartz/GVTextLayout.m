//
//  GVTextLayout.m
//  
//
//  Created by Glen Low on 3/01/09.
//  Copyright 2009 Pixelglow Software. All rights reserved.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "gvcjob.h"

#include "gvplugin_quartz.h"

#if __IPHONE_OS_VERSION_MIN_REQUIRED >= 20000

#import "GVTextLayout.h"

boolean quartz_textlayout(textpara_t *para, char **fontpath)
{
	GVTextLayout* layout = [[GVTextLayout alloc] initWithFontName:para->fontname fontSize:para->fontsize text:para->str];
	CGSize size = layout.size;
	
	para->layout = layout;
	para->free_layout = &quartz_free_layout;
	para->width = size.width;
	para->height = size.height;
	para->yoffset_layout = layout.font.ascender;
	para->yoffset_centerline = 0;
	
	return TRUE;
}

void quartz_free_layout(void *layout)
{
	[(GVTextLayout*)layout release];
}

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
	
	GVTextLayout* layout;
	if (para->free_layout == &quartz_free_layout)
		layout = (GVTextLayout*)para->layout;
	else
		layout = [[GVTextLayout alloc] initWithFontName:para->fontname fontSize:para->fontsize text:para->str];
		
	CGContextSaveGState(context);
	CGContextScaleCTM(context, 1.0, -1.0);
	CGContextSetRGBFillColor(context, job->obj->pencolor.u.RGBA [0], job->obj->pencolor.u.RGBA [1], job->obj->pencolor.u.RGBA [2], job->obj->pencolor.u.RGBA [3]);
	[layout drawAtPoint:CGPointMake(p.x, -p.y - para->yoffset_layout) inContext:context];
	CGContextRestoreGState(context);
	
	if (para->free_layout != &quartz_free_layout)
		[layout release];
}

static NSString* _defaultFontName = @"TimesNewRomanPSMT";

@implementation GVTextLayout

@synthesize font = _font;
@synthesize text = _text;

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

- (void)drawAtPoint:(CGPoint)point inContext:(CGContextRef)context
{
	UIGraphicsPushContext(context);
	[_text drawAtPoint:point withFont:_font];
	UIGraphicsPopContext();
}

- (CGSize)size
{
	return [_text sizeWithFont:_font];
}

- (void)dealloc
{
	[_font release];
	[_text release];
	
	[super dealloc];
}


@end

#endif
