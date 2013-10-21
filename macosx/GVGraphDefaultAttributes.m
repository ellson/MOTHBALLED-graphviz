/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#import "GVGraphDefaultAttributes.h"
#import "GVGraph.h"

@interface GVGraphDefaultAttributeKeyEnumerator : NSEnumerator
{
	graph_t *_graph; 
	int _kind;
	Agsym_t *_nextSymbol;
}

- (id)initWithGraphLoc:(graph_t *)graph prototype:(int)kind;
- (NSArray *)allObjects;
- (id)nextObject;

@end

@implementation GVGraphDefaultAttributeKeyEnumerator

- (id)initWithGraphLoc:(graph_t *)graph prototype:(int)kind;
{
	if (self = [super init]) {
		_kind = kind;
		_graph = graph;
		_nextSymbol = agnxtattr(_graph, _kind, NULL);
	}
	return self;
}

- (NSArray *)allObjects
{
	NSMutableArray* all = [NSMutableArray array];
	for (; _nextSymbol; _nextSymbol = agnxtattr(_graph, _kind, _nextSymbol)) {
		char *attributeValue = _nextSymbol->defval;
		if (attributeValue && *attributeValue)
			[all addObject:[NSString stringWithUTF8String:attributeValue]];
	}
			
	return all;
}

- (id)nextObject
{
	for (; _nextSymbol; _nextSymbol = agnxtattr(_graph, _kind, _nextSymbol)) {
		char *attributeValue = _nextSymbol->defval;
		if (attributeValue && *attributeValue)
			return [NSString stringWithUTF8String:attributeValue];
	}
	return nil;
}

@end

@implementation GVGraphDefaultAttributes

- (id)initWithGraph:(GVGraph *)graph prototype:(int)kind
{
	if (self = [super init]) {
		_graph = graph;
		_kind = kind;
	}
	return self;
}

- (NSUInteger)count
{
	NSUInteger symbolCount = 0;
	Agsym_t *nextSymbol = NULL;
	for (nextSymbol = agnxtattr(_graph->_graph,_kind, nextSymbol); nextSymbol; nextSymbol = agnxtattr(_graph->_graph, _kind, nextSymbol))
		if (nextSymbol->defval && *(nextSymbol->defval))
			++symbolCount;
	return symbolCount;
}

- (NSEnumerator *)keyEnumerator
{
	return [[[GVGraphDefaultAttributeKeyEnumerator alloc] initWithGraphLoc:_graph->_graph prototype:_kind] autorelease];
}

- (id)objectForKey:(id)aKey
{
	id object = nil;
	Agsym_t *attributeSymbol = agattr(_graph->_graph, _kind, (char*)[aKey UTF8String], 0);
	if (attributeSymbol) {
		char *attributeValue = attributeSymbol->defval;
		if (attributeValue && *attributeValue)
			object = [NSString stringWithUTF8String:attributeValue];
	}
	return object;
}

- (void)setObject:(id)anObject forKey:(id)aKey
{
	agattr(_graph->_graph, _kind, (char *)[aKey UTF8String], (char *)[anObject UTF8String]);
	[_graph noteChanged:YES];
}

- (void)removeObjectForKey:(id)aKey
{
	agattr(_graph->_graph, _kind, (char *)[aKey UTF8String], "");
	[_graph noteChanged:YES];
}
@end
