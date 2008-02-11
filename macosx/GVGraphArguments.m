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

#import "GVGraphArguments.h"
#import "GVGraph.h"

@implementation GVGraphArguments

- (id)initWithGraph:(GVGraph *)graph
{
	if (self = [super init]) {
		_graph = graph;	/* no retain to avoid a retain cycle */
		_arguments = [[NSMutableDictionary alloc] init];
	}
	return self;
}

- (NSUInteger)count
{
	return [_arguments count];
}

- (NSEnumerator *)keyEnumerator
{
	return [_arguments keyEnumerator];
}

- (id)objectForKey:(id)aKey
{
	return [_arguments objectForKey:aKey];
}

/* mutable dictionary primitive methods */
- (void)setObject:(id)anObject forKey:(id)aKey
{
	[_arguments setObject:anObject forKey:aKey];
	[_graph noteChanged:YES];
}

- (void)removeObjectForKey:(id)aKey
{
	[_arguments removeObjectForKey:aKey];
	[_graph noteChanged:YES];
}

- (void)dealloc
{
	[_arguments release];
	[super dealloc];
}

@end
