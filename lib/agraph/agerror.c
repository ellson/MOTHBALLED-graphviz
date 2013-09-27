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


#include <stdio.h>
#include "aghdr.h"

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
