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
 * single dir support for pathaccess()
 */

#include <ast.h>

char *pathcat(char *path, register const char *dirs, int sep,
	      const char *a, register const char *b)
{
    register char *s;

    s = path;
    while (*dirs && *dirs != sep)
	*s++ = *dirs++;
    if (s != path)
	*s++ = '/';
    if (a) {
	while ((*s = *a++))
	    s++;
	if (b)
	    *s++ = '/';
    } else if (!b)
	b = ".";
    if (b)
	while ((*s++ = *b++));
    return (*dirs ? (char *) ++dirs : 0);
}
