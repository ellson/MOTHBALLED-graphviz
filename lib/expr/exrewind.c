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
#include <string.h>

int exrewind(Expr_t * ex)
{
    register int n;

    if (ex->linewrap) {
	exerror("too much pushback");
	return -1;
    }
    if (!ex->input->pushback
	&& !(ex->input->pushback = oldof(0, char, sizeof(ex->line), 3))) {
	exerror("out of space [rewind]");
	return -1;
    }
    if ((n = ex->linep - ex->line))
	memcpy(ex->input->pushback, ex->line, n);
    if (ex->input->peek) {
	ex->input->pushback[n++] = ex->input->peek;
	ex->input->peek = 0;
    }
    ex->input->pushback[n++] = ' ';
    ex->input->pushback[n] = 0;
    ex->input->pp = ex->input->pushback;
    ex->input->nesting = ex->nesting;
    setcontext(ex);
    return 0;
}

void exstatement(Expr_t * ex)
{
    ex->nesting = ex->input->nesting;
    setcontext(ex);
}
