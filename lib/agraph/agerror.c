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


#include <stdio.h>
#include <aghdr.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static char *Message[] = {
    "",				/* 0 is not assigned   */
    "%s",			/* AGERROR_SYNTAX == 1 */
    "out of memory",		/* AGERROR_MEMORY == 2 */
    "unimplemented feature: %s",	/* AGERROR_UNIMPL == 3 */
    "move_to_front lock %s",	/* AGERROR_MTFLOCK== 4 */
    "compound graph error %s",	/* AGERROR_CMPND  == 5 */
    "bad object pointer %s",	/* AGERROR_BADOBJ == 6 */
    "object ID overflow",	/* AGERROR_IDOVFL == 7 */
    "flat lock violation",	/* AGERROR_MTFLOCK== 8 */
    "object and graph disagree"	/* AGERROR_WRONGGRAPH==9 */
};

/* default error handler */
void agerror(int code, char *str)
{
    /* fprintf(stderr,"libgraph runtime error: "); */
    fprintf(stderr, Message[code], str);
    fprintf(stderr, "\n");

    if (code != AGERROR_SYNTAX)
	exit(1);
}
