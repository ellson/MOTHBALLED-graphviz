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

#import <UIKit/UIKit.h>

@interface GVTextLayout : NSObject
{
	UIFont* _font;
	NSString* _text;
}

- (id)initWithFontName:(char*)fontName fontSize:(CGFloat)fontSize text:(char*)text;

- (void)sizeUpWidth:(double*)width height:(double*)height yoffset:(double*)yoffset;
- (void)drawInContext:(CGContextRef)context atPosition:(CGPoint)position;

- (void)dealloc;

@end
