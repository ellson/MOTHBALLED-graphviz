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


#include <ast.h>

/*
 * copy t into s, return a pointer to the end of s ('\0')
 */

char *strcopy(register char *s, register const char *t)
{
    if (!t)
	return (s);
    while ((*s++ = *t++));
    return (--s);
}
