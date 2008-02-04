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
#import "GVContext.h"
#import "GVDocument.h"

@implementation GVWindowController

- (void)windowDidLoad
{
	GVDocument *graphDocument = [self document];
	if ([graphDocument respondsToSelector:@selector(graph)]) {
		graph_t *graph = [graphDocument graph];
		GVContext* context = [GVContext sharedContext];
		[context layoutGraph:graph withEngine:@"dot"];
		[documentView setDocument:[[[PDFDocument alloc] initWithData:[context renderGraph:graph withFormat:@"pdf:quartz"]] autorelease]];
	}
}

@end
