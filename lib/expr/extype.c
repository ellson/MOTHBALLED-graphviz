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
 * expression library C program generator
 */

#include "exlib.h"

#define str(s)		# s
#define xstr(s)		str(s)

/*
 * return C type name for type
 */

char *extype(int type)
{
    switch (type) {
    case FLOATING:
	return "double";
    case STRING:
	return "char*";
    case UNSIGNED:
	return xstr(unsigned _ast_intmax_t);
    }
    return xstr(_ast_intmax_t);
}
