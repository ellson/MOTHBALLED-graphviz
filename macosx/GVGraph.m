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

#import "GVGraph.h"
#import "GVGraphArguments.h"
#import "GVGraphDefaultAttributes.h"

extern double PSinputscale;

static GVC_t *_graphContext = nil;

@implementation GVGraph

@synthesize graph = _graph;
@synthesize arguments = _arguments;
@synthesize graphAttributes = _graphAttributes;
@synthesize defaultNodeAttributes = _defaultNodeAttributes;
@synthesize defaultEdgeAttributes = _defaultEdgeAttributes;

+ (void)initialize
{
	_graphContext = gvContext();
}

- (id)initWithURL:(NSURL*)URL error:(NSError**)outError
{
	if (self = [super init]) {
		if ([URL isFileURL]) {
			/* open a FILE* on the file URL */
			FILE *file = fopen([[URL path] fileSystemRepresentation], "r");
			if (!file) {
				if (outError)
					*outError = [NSError errorWithDomain:NSPOSIXErrorDomain code:errno userInfo:nil];
				[self autorelease];
				return nil;
			}
			
			_graph = agread(file);
			fclose(file);
		}
		else {
			/* read the URL into memory */
			NSMutableData *memory = [NSMutableData dataWithContentsOfURL:URL options:0 error:outError];
			if (!memory) {
				[self autorelease];
				return nil;
			}
			
			/* null terminate the data */
			char nullByte = '\0';
			[memory appendBytes:&nullByte length:1];
			
			_graph = agmemread((char*)[memory bytes]);
		}

		_freeLastLayout = NO;
		_arguments = [[GVGraphArguments alloc] initWithGraph:self];
		_graphAttributes = [[GVGraphDefaultAttributes alloc] initWithGraph:self prototype:_graph];
		_defaultNodeAttributes = [[GVGraphDefaultAttributes alloc] initWithGraph:self prototype:agprotonode(_graph)];
		_defaultEdgeAttributes = [[GVGraphDefaultAttributes alloc] initWithGraph:self prototype:agprotoedge(_graph)];
	}
	
	return self;
}

- (BOOL)writeToURL:(NSURL*)URL error:(NSError**)outError
{
	if ([URL isFileURL]) {
		/* open a FILE* on the file URL */
		FILE *file = fopen([[URL path] fileSystemRepresentation], "w");
		if (!file) {
			if (outError)
				*outError = [NSError errorWithDomain:NSPOSIXErrorDomain code:errno userInfo:nil];
			return NO;
		}
		
		/* write it out */
		if (agwrite(_graph, file) != 0) {
			if (outError)
				*outError = [NSError errorWithDomain:NSPOSIXErrorDomain code:errno userInfo:nil];
			return NO;
		}
		
		fclose(file);
		return YES;
	}
	else
		/* can't write out to non-file URL */
		return NO;
}

- (void)noteChanged:(BOOL)relayout
{
	/* if we need to layout, apply globals and then relayout */
	if (relayout) {
		NSString* layout = [_arguments objectForKey:@"layout"];
		if (layout) {
			if (_freeLastLayout)
				gvFreeLayout(_graphContext, _graph);
				
			/* apply scale */
			NSString* scale = [_arguments objectForKey:@"scale"];
			PSinputscale = scale ? [scale doubleValue] : 0.0;
			if (PSinputscale == 0.0)
				PSinputscale = 72.0;
		
			if (gvLayout(_graphContext, _graph, (char*)[layout UTF8String]) != 0)
				@throw [NSException exceptionWithName:@"GVException" reason:@"bad layout" userInfo:nil];
			_freeLastLayout = YES;
		}
	}
	

	[[NSNotificationCenter defaultCenter] postNotificationName: @"GVGraphDidChange" object:self];
}

- (NSData*)renderWithFormat:(NSString*)format
{
	char *renderedData = NULL;
	unsigned int renderedLength = 0;
	if (gvRenderData(_graphContext, _graph, (char*)[format UTF8String], &renderedData, &renderedLength) != 0)
		@throw [NSException exceptionWithName:@"GVException" reason:@"bad render" userInfo:nil];
	return [NSData dataWithBytesNoCopy:renderedData length:renderedLength freeWhenDone:YES];

}

- (void)renderWithFormat:(NSString*)format toURL:(NSURL*)URL
{
	if ([URL isFileURL]) {
		if (gvRenderFilename(_graphContext, _graph, (char*)[format UTF8String], (char*)[[URL path] UTF8String]) != 0)
			@throw [NSException exceptionWithName:@"GVException" reason:@"bad render" userInfo:nil];
	}
	else
		[[self renderWithFormat:format] writeToURL:URL atomically:NO];
}


- (void)dealloc
{
	if (_graph)
		agclose(_graph);
	
	[_arguments release];
	[_graphAttributes release];
	[_defaultNodeAttributes release];
	[_defaultEdgeAttributes release];
	
	[super dealloc];
}

@end
