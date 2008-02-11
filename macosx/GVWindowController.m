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
}

- (void)graphDidChange:(NSNotification*)notification
{
	/* whenever the graph changes, rerender its PDF and display that */
	[documentView setDocument:[[[PDFDocument alloc] initWithData:[_graph renderWithFormat:@"pdf:quartz"]] autorelease]];
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
