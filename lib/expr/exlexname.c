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
 * return lex name for op[subop]
 */

#include "exlib.h"
#include "exop.h"

#define TOTNAME		3
#define MAXNAME		16

char *exlexname(int op, int subop)
{
    register char *b;

    static int n;
    static char buf[TOTNAME][MAXNAME];

    if (op > MINTOKEN && op < MAXTOKEN)
	return (char *) exop[op - MINTOKEN];
    if (++n > TOTNAME)
	n = 0;
    b = buf[n];
    if (op == '=') {
	if (subop > MINTOKEN && subop < MAXTOKEN)
	    sfsprintf(b, MAXNAME, "%s=", exop[subop - MINTOKEN]);
	else if (subop > ' ' && subop <= '~')
	    sfsprintf(b, MAXNAME, "%c=", subop);
	else
	    sfsprintf(b, MAXNAME, "(%d)=", subop);
    } else if (op > ' ' && op <= '~')
	sfsprintf(b, MAXNAME, "%c", op);
    else
	sfsprintf(b, MAXNAME, "(%d)", op);
    return b;
}
