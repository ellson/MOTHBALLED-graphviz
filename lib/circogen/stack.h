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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STACK_H
#define STACK_H

#include  <render.h>

    typedef struct {
	Agnode_t *top;
	int sz;
    } nstack_t;

    extern nstack_t *mkStack(void);
    extern void stackPush(nstack_t * s, Agnode_t * n);
    extern Agnode_t *stackPop(nstack_t * s);
    extern int stackSize(nstack_t * s);
    extern int stackCheck(nstack_t * s, Agnode_t * n);
    extern void freeStack(nstack_t * s);

#define top(sp)  ((sp)->top)

#ifdef DEBUG
    extern void printStack(nstack_t *);
#endif

#endif

#ifdef __cplusplus
}
#endif
