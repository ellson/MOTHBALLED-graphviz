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

#import "GVDocument.h"
#import "GVGraph.h"
#import "GVWindowController.h"

@implementation GVDocument

@synthesize graph = _graph;

- (id)init
{
	if (self = [super init]) {
		_graph = nil;
	}
    return self;
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	[_graph release];
	_graph = [[GVGraph alloc] initWithURL:absoluteURL error:outError];
	[_graph.arguments setValue:@"dot" forKey:@"layout"];
	
	return _graph != nil;
}

- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	return [_graph writeToURL:absoluteURL error:outError];
}

- (void)makeWindowControllers
{
	[self addWindowController: [[[GVWindowController alloc] init] autorelease]];
}

- (void)dealloc
{
	[_graph release];
	[super dealloc];
}

@end
