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
 * return the expression for name or sym coerced to type
 */

Exnode_t *exexpr(Expr_t * ex, const char *name, Exid_t * sym, int type)
{
    if (ex) {
	if (!sym)
	    sym = name ? (Exid_t *) dtmatch(ex->symbols, name) : &ex->main;
	if (sym && sym->lex == PROCEDURE && sym->value) {
	    if (type != DELETE)
		return excast(ex, sym->value->data.procedure.body, type,
			      NiL, 0);
	    exfreenode(ex, sym->value);
	    sym->lex = NAME;
	    sym->value = 0;
	}
    }
    return 0;
}
