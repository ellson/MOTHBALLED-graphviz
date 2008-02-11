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
	Agsym_t **_nextSymbol;
	Agsym_t **_lastSymbol;
}

- (id)initWithSymbols:(Agsym_t **)symbols count:(NSUInteger)count;
- (NSArray *)allObjects;
- (id)nextObject;

@end

@implementation GVGraphDefaultAttributeKeyEnumerator

- (id)initWithSymbols:(Agsym_t **)symbols count:(NSUInteger)count
{
	if (self = [super init]) {
		_nextSymbol = symbols;
		_lastSymbol = symbols + count;
	}
	return self;
}

- (NSArray *)allObjects
{
	NSMutableArray* all = [NSMutableArray array];
	for (; _nextSymbol < _lastSymbol; ++_nextSymbol)
		if ((*_nextSymbol)->value && *(*_nextSymbol)->value)
			[all addObject:[NSString stringWithUTF8String:(*_nextSymbol)->name]];
			
	return all;
}

- (id)nextObject
{
	char* nextName = NULL;
	for (; _nextSymbol < _lastSymbol && !nextName; ++_nextSymbol)
		if ((*_nextSymbol)->value && *(*_nextSymbol)->value)
			nextName = (*_nextSymbol)->name;
		
	return nextName ? [NSString stringWithUTF8String:nextName] : nil;
}

@end

@implementation GVGraphDefaultAttributes

- (id)initWithGraph:(GVGraph *)graph defaultAttributes:(Agdict_t *)defaultAttributes attributeDeclaration:(Agsym_t *(*)(Agraph_t *, char *, char *))attributeDeclaration
{
	if (self = [super init]) {
		_graph = graph;	/* not retained to avoid a retain cycle */
		_defaultAttributes = defaultAttributes;
		_attributeDeclaration = attributeDeclaration;
	}
	return self;
}

- (NSString*)name
{
	return _defaultAttributes->name ? [NSString stringWithUTF8String:_defaultAttributes->name] : nil;
}

- (NSUInteger)count
{
	NSUInteger symbolCount = 0;
	Agsym_t **nextSymbol, **lastSymbol;
	for (nextSymbol = _defaultAttributes->list, lastSymbol = _defaultAttributes->list + dtsize(_defaultAttributes->dict); nextSymbol < lastSymbol; ++nextSymbol)
		if ((*nextSymbol)->value && *(*nextSymbol)->value)
			++symbolCount;
	return symbolCount;
}

- (NSEnumerator *)keyEnumerator
{
	return [[[GVGraphDefaultAttributeKeyEnumerator alloc] initWithSymbols: _defaultAttributes->list count:dtsize(_defaultAttributes->dict)] autorelease];
}

- (id)objectForKey:(id)aKey
{
	id object = nil;
	Agsym_t *attributeSymbol = dtmatch(_defaultAttributes->dict, [aKey UTF8String]);
	if (attributeSymbol) {
		char *attributeValue = attributeSymbol->value;
		if (attributeValue && *attributeValue)
			object = [NSString stringWithUTF8String:attributeSymbol->value];
	}
	return object;
}

- (void)setObject:(id)anObject forKey:(id)aKey
{
	_attributeDeclaration([_graph graph], (char *)[aKey UTF8String], (char *)[anObject UTF8String]);
	[_graph noteChanged:YES];
}

- (void)removeObjectForKey:(id)aKey
{
	_attributeDeclaration([_graph graph], (char *)[aKey UTF8String], "");
	[_graph noteChanged:YES];
}
@end
