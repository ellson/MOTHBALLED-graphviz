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
 * AT&T Bell Laboratories
 *
 * convert \x character constants in s in place
 * the length of the converted s is returned (may have imbedded \0's)
 */

#include <ast.h>

int stresc(register char *s)
{
    register char *t;
    register int c;
    char *b;
    char *p;

    b = t = s;
    for (;;) {
	switch (c = *s++) {
	case '\\':
	    c = chresc(s - 1, &p);
	    s = p;
	    break;
	case 0:
	    *t = 0;
	    return (t - b);
	}
	*t++ = c;
    }
}
