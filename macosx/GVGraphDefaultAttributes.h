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

#include <graphviz/types.h>
#include <graphviz/graph.h>

@class GVGraph;

@interface GVGraphDefaultAttributes : NSMutableDictionary
{
	GVGraph *_graph;
	Agdict_t *_defaultAttributes;
	Agsym_t *(*_attributeDeclaration)(Agraph_t *, char *, char *);
}

@property(readonly) NSString *name;

- (id)initWithGraph:(GVGraph *)graph defaultAttributes:(Agdict_t *)defaultAttributes attributeDeclaration:(Agsym_t *(*)(Agraph_t *, char *, char *))attributeDeclaration;

/* dictionary primitive methods */
- (NSUInteger)count;
- (NSEnumerator *)keyEnumerator;
- (id)objectForKey:(id)aKey;

/* mutable dictionary primitive methods */
- (void)setObject:(id)anObject forKey:(id)aKey;
- (void)removeObjectForKey:(id)aKey;

@end
