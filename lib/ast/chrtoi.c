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
 * convert a 0 terminated character constant string to an int
 */

#include <ast.h>

int chrtoi(register const char *s)
{
    register int c;
    register int n;
    register int x;
    char *p;

    c = 0;
    for (n = 0; n < sizeof(int) * CHAR_BIT; n += CHAR_BIT) {
	switch (x = *((unsigned char *) s++)) {
	case '\\':
	    x = chresc(s - 1, &p);
	    s = (const char *) p;
	    break;
	case 0:
	    return (c);
	}
	c = (c << CHAR_BIT) | x;
    }
    return (c);
}
