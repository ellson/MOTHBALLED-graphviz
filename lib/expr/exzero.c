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
 * expression library support
 */

#include "exlib.h"

/*
 * return 0 value for type
 */

Extype_t exzero(int type)
{
    Extype_t v;

    switch (type) {
    case FLOATING:
	v.floating = 0.0;
	break;
    case INTEGER:
    case UNSIGNED:
	v.integer = 0;
	break;
    case STRING:
	v.string = expr.nullstring;
	break;
    default:
	v.integer = 0;
	break;
    }
    return v;
}
