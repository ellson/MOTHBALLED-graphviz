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


#include <ast.h>

/*
 * return small format buffer chunk of size n
 * spin lock for thread access
 * format buffers are short lived
 */

static char buf[16 * 1024];
static char *nxt = buf;
static int lck = -1;

char *fmtbuf(size_t n)
{
    register char *cur;

    while (++lck)
	lck--;
    if (n > (&buf[elementsof(buf)] - nxt))
	nxt = buf;
    cur = nxt;
    nxt += n;
    lck--;
    return cur;
}
