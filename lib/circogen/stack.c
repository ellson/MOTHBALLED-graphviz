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


#include	"stack.h"
#include	"circular.h"
#include	<assert.h>

nstack_t *mkStack()
{
    nstack_t *s;

    s = NEW(nstack_t);

    s->top = NULL;
    s->sz = 0;
    return s;
}

void freeStack(nstack_t * s)
{
    free(s);
}

void stackPush(nstack_t * s, Agnode_t * n)
{
    SET_ONSTACK(n);
    NEXT(n) = s->top;
    s->top = n;
    s->sz += 1;
}

Agnode_t *stackPop(nstack_t * s)
{
    Agnode_t *top = s->top;

    if (top) {
	assert(s->sz > 0);
	UNSET_ONSTACK(top);
	s->top = NEXT(top);
	s->sz -= 1;
    } else {
	assert(0);
    }

    return top;
}

int stackSize(nstack_t * s)
{
    return s->sz;
}

/* stackCheck:
 * Return true if n in on the stack.
 */
int stackCheck(nstack_t * s, Agnode_t * n)
{
    return ONSTACK(n);
#ifdef OLD
    stackitem_t *top = s->top;
    Agnode_t *node;

    while (top != NULL) {
	node = top->data;
	if (node == n)
	    return 1;
	top = top->next;
    }

    return 0;
#endif
}

#ifdef DEBUG
void printStack(nstack_t * s)
{
    Agnode_t *n;
    for (n = s->top; n; n = NEXT(n))
	fprintf(stderr, " %s", n->name);
    fprintf(stderr, "\n");

}
#endif
