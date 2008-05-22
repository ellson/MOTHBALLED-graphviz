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

#import "GVWindowController.h"
#import "GVGraph.h"
#import "GVDocument.h"

@implementation GVWindowController

@synthesize graph = _graph;

- (id)init
{
	if (self = [super initWithWindowNibName: @"Document"])
		_graph = nil;
	return self;
}

- (void)setDocument: (NSDocument *)document
{
	if ([document respondsToSelector:@selector(graph)]) {
		GVGraph *newGraph = [(GVDocument *)document graph];
		if (_graph != newGraph) {
			/* retain the new document graph and start observing any changes from it */
			NSNotificationCenter* defaultCenter = [NSNotificationCenter defaultCenter];
			if (_graph) {
				[defaultCenter removeObserver:self name:@"GVGraphDidChange" object:_graph];
				[_graph release];
			}
			_graph = nil;
			if (newGraph) {
				_graph = [newGraph retain];
				[defaultCenter addObserver:self selector:@selector(graphDidChange:) name:@"GVGraphDidChange" object:newGraph];
			}
		}
	}
	
	[super setDocument:document];
}

- (void)awakeFromNib
{
	[self graphDidChange:nil];
	
	/* if window is not at standard size, make it standard size */
	NSWindow *window = [self window];
	if (![window isZoomed])
		[window zoom:self];
}

- (void)graphDidChange:(NSNotification*)notification
{
	/* whenever the graph changes, rerender its PDF and display that */
	[documentView setDocument:[[[PDFDocument alloc] initWithData:[_graph renderWithFormat:@"pdf:quartz"]] autorelease]];
}

- (NSRect)windowWillUseStandardFrame:(NSWindow *)window defaultFrame:(NSRect)defaultFrame
{
	/* standard size for zooming is whatever will fit the content exactly */
	NSRect currentFrame = [window frame];
	NSRect standardFrame = [window frameRectForContentRect:[[documentView documentView] bounds]];
	standardFrame.origin.x = currentFrame.origin.x;
	standardFrame.origin.y = currentFrame.origin.y + currentFrame.size.height - standardFrame.size.height;
	return standardFrame;
}

- (IBAction)actualSizeView:(id)sender
{
	[documentView setScaleFactor:1.0];
}

- (IBAction)zoomInView:(id)sender
{
	[documentView zoomIn:sender];
}

- (IBAction)zoomOutView:(id)sender
{
	[documentView zoomOut:sender];
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem
{
	/* validate toolbar or menu items */
	if ([anItem action] == @selector(actualSizeView:))
		return YES;
	else if ([anItem action] == @selector(zoomInView:))
		return [documentView canZoomIn];
	else if ([anItem action] == @selector(zoomOutView:))
		return [documentView canZoomOut];
	else
		return NO;
}
- (void)dealloc
{
	if (_graph) {
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"GVGraphDidChange" object:_graph];
		[_graph release];
	}

	[super dealloc];
}

@end
