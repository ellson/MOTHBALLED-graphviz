/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

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
