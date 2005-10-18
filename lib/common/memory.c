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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memory.h"

void *zmalloc(size_t nbytes)
{
    char *rv = malloc(nbytes);
    if (nbytes == 0)
	return 0;
    if (rv == NULL) {
	fprintf(stderr, "out of memory\n");
	abort();
    }
    memset(rv, 0, nbytes);
    return rv;
}

void *zrealloc(void *ptr, size_t size, size_t elt, size_t osize)
{
    void *p = realloc(ptr, size * elt);
    if (p == NULL && size) {
	fprintf(stderr, "out of memory\n");
	abort();
    }
    if (osize < size)
	memset((char *) p + (osize * elt), '\0', (size - osize) * elt);
    return p;
}

void *gmalloc(size_t nbytes)
{
    char *rv;
    if (nbytes == 0)
	return (char *)1; /* NB Return an invalid pointer - since nobody seems to check for NULL */
    rv = malloc(nbytes);
    if (rv == NULL) {
	fprintf(stderr, "out of memory\n");
	abort();
    }
    return rv;
}

void *grealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (p == NULL && size) {
	fprintf(stderr, "out of memory\n");
	abort();
    }
    return p;
}
