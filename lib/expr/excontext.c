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

/*
 * Glenn Fowler
 * AT&T Research
 *
 * expression library
 */

#include "exlib.h"

/*
 * copy input token error context into buf of n chars and reset the context
 * end of buf returned
 */

char *excontext(Expr_t * p, char *buf, int n)
{
    register char *s;
    register char *t;
    register char *e;

    s = buf;
    if (p->linep > p->line || p->linewrap) {
	e = buf + n - 5;
	if (p->linewrap) {
	    t = p->linep + 1;
	    while (t < &p->line[sizeof(p->line)] && isspace(*t))
		t++;
	    if ((n =
		 (sizeof(p->line) - (t - (p->linep + 1))) - (e - s)) > 0) {
		if (n > &p->line[sizeof(p->line)] - t)
		    t = &p->line[sizeof(p->line)];
		else
		    t += n;
	    }
	    while (t < &p->line[sizeof(p->line)])
		*s++ = *t++;
	}
	t = p->line;
	if (p->linewrap)
	    p->linewrap = 0;
	else
	    while (t < p->linep && isspace(*t))
		t++;
	if ((n = (p->linep - t) - (e - s)) > 0)
	    t += n;
	while (t < p->linep)
	    *s++ = *t++;
	p->linep = p->line;
	t = "<<< ";
	while ((*s = *t++))
	    s++;
    }
    *s = 0;
    return s;
}
