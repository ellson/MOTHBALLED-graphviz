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

#import <Foundation/Foundation.h>
#include <graphviz/gvc.h>

@interface GVContext : NSObject
{
	GVC_t *_context;
}

+ (GVContext *)sharedContext;

- (id)init;

- (void)layoutGraph:(graph_t *)graph withEngine:(NSString*)engine;

- (NSData*)renderGraph:(graph_t *)graph withFormat:(NSString*)format;
- (void)renderGraph:(graph_t *)graph withFormat:(NSString*) format toURL:(NSURL*)URL;
@end
