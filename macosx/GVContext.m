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

#import "GVContext.h"

@implementation GVContext

+ (GVContext *)sharedContext
{
	static GVContext *shared = nil;
	if (!shared)
		shared = [[GVContext alloc] init];
	return shared;
}

- (id)init
{
	if (self = [super init])
		_context = gvContext();
	return self;
}

- (void)layoutGraph:(graph_t *)graph withEngine:(NSString*)engine
{
	if (gvLayout(_context, graph, (char*)[engine UTF8String]) != 0)
		@throw [NSException exceptionWithName:@"GVException" reason:@"Bad layout" userInfo:nil];
}

- (NSData*)renderGraph:(graph_t *)graph withFormat:(NSString*)format
{
	char *renderedData = NULL;
	unsigned int renderedLength = 0;
	if (gvRenderData(_context, graph, (char*)[format UTF8String], &renderedData, &renderedLength) != 0)
		@throw [NSException exceptionWithName:@"GVException" reason:@"Bad render" userInfo:nil];
	return [NSData dataWithBytesNoCopy:renderedData length:renderedLength freeWhenDone:YES];
}

- (void)renderGraph:(graph_t *)graph withFormat:(NSString*)format toURL:(NSURL*)URL
{
	if ([URL isFileURL]) {
		if (gvRenderFilename(_context, graph, (char*)[format UTF8String], (char*)[[URL path] UTF8String]) != 0)
			@throw [NSException exceptionWithName:@"GVException" reason:@"Bad render" userInfo:nil];
	}
	else
		[[self renderGraph:graph withFormat:format] writeToURL:URL atomically:NO];
}

- (void)dealloc
{
	if(_context)
		gvFreeContext(_context);
	[super dealloc];
}
@end
