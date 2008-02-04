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
#import "GVWindowController.h"

@implementation GVDocument

@synthesize graph = _graph;

+ (void)initialize
{
	aginit();
}

- (id)init
{
	if (self = [super init]) {
		_graph = NULL;
	}
    return self;
}

- (BOOL)readFromURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	if ([absoluteURL isFileURL]) {
		/* open a FILE* on the file URL */
		FILE *file = fopen([[absoluteURL path] fileSystemRepresentation], "r");
		if (!file) {
			if (outError)
				*outError = [NSError errorWithDomain:NSPOSIXErrorDomain code:errno userInfo:nil];
			return NO;
		}
		
		if (_graph)
			agclose(_graph);
		_graph = agread(file);
		fclose(file);
		
		return YES;
	}
	else {
		/* read the URL into memory */
		NSMutableData *memory = [NSMutableData dataWithContentsOfURL:absoluteURL options:0 error:outError];
		if (!memory)
			return NO;
		
		/* null terminate the data */
		char nullByte = '\0';
		[memory appendBytes:&nullByte length:1];
		
		if (_graph)
			agclose(_graph);
		_graph = agmemread((char*)[memory bytes]);
		
		return YES;
	}
}

- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName error:(NSError **)outError
{
	if ([absoluteURL isFileURL]) {
		/* open a FILE* on the file URL */
		FILE *file = fopen([[absoluteURL path] fileSystemRepresentation], "w");
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

- (void)makeWindowControllers
{
	[self addWindowController: [[[GVWindowController alloc] initWithWindowNibName: @"Document"] autorelease]];
}

- (void)dealloc
{
	if (_graph)
		agclose(_graph);
	[super dealloc];
}

@end
