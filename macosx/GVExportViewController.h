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

#import <Cocoa/Cocoa.h>

@interface GVExportViewController : NSViewController
{
	NSSavePanel *_panel;
	NSString *_filename;
	NSDictionary *_formatRender;
	NSString *_render;
}

@property(readonly) NSArray *formatRenders;
@property(readonly) NSString *device;
@property(retain) NSString *filename;
@property(retain) NSDictionary *formatRender;
@property(retain) NSString *render;

- (id)init;

- (void)beginSheetModalForWindow:(NSWindow *)window modalDelegate:(id)modalDelegate didEndSelector:(SEL)selector;

@end
