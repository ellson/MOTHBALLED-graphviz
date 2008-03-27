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

#import "GVGraphDefaultAttributes.h"
#import "GVGraph.h"

@interface GVGraphDefaultAttributeKeyEnumerator : NSEnumerator
{
	void *_proto;
	Agsym_t *_nextSymbol;
}

- (id)initWithPrototype:(void *)proto;
- (NSArray *)allObjects;
- (id)nextObject;

@end

@implementation GVGraphDefaultAttributeKeyEnumerator

- (id)initWithPrototype:(void *)proto
{
	if (self = [super init]) {
		_proto = proto;
		_nextSymbol = agfstattr(_proto);
	}
	return self;
}

- (NSArray *)allObjects
{
	NSMutableArray* all = [NSMutableArray array];
	for (; _nextSymbol; _nextSymbol = agnxtattr(_proto, _nextSymbol)) {
		char *attributeValue = _nextSymbol->value;
		if (attributeValue && *attributeValue)
			[all addObject:[NSString stringWithUTF8String:attributeValue]];
	}
			
	return all;
}

- (id)nextObject
{
	for (; _nextSymbol; _nextSymbol = agnxtattr(_proto, _nextSymbol)) {
		char *attributeValue = _nextSymbol->value;
		if (attributeValue && *attributeValue)
			return [NSString stringWithUTF8String:attributeValue];
	}
	return nil;
}

@end

@implementation GVGraphDefaultAttributes

- (id)initWithGraph:(GVGraph *)graph prototype:(void *)proto
{
	if (self = [super init]) {
		_graph = graph;	/* not retained to avoid a retain cycle */
		_proto = proto;
	}
	return self;
}

- (NSUInteger)count
{
	NSUInteger symbolCount = 0;
	Agsym_t *nextSymbol;
	for (nextSymbol = agfstattr(_proto); nextSymbol; nextSymbol = agnxtattr(_proto, nextSymbol))
		if (nextSymbol->value && *(nextSymbol->value))
			++symbolCount;
	return symbolCount;
}

- (NSEnumerator *)keyEnumerator
{
	return [[[GVGraphDefaultAttributeKeyEnumerator alloc] initWithPrototype:_proto] autorelease];
}

- (id)objectForKey:(id)aKey
{
	id object = nil;
	Agsym_t *attributeSymbol = agfindattr(_proto, (char*)[aKey UTF8String]);
	if (attributeSymbol) {
		char *attributeValue = attributeSymbol->value;
		if (attributeValue && *attributeValue)
			object = [NSString stringWithUTF8String:attributeValue];
	}
	return object;
}

- (void)setObject:(id)anObject forKey:(id)aKey
{
	agattr(_proto, (char *)[aKey UTF8String], (char *)[anObject UTF8String]);
	[_graph noteChanged:YES];
}

- (void)removeObjectForKey:(id)aKey
{
	agattr(_proto, (char *)[aKey UTF8String], "");
	[_graph noteChanged:YES];
}
@end
