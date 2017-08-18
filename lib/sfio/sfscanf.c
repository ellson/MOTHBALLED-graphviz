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

#include	"sfhdr.h"

/*	Read formatted data from a stream
**
**	Written by Kiem-Phong Vo.
*/

int sfscanf(Sfio_t * f, const char *form, ...)
{
    va_list args;
    reg int rv;
    va_start(args, form);
    rv = (f && form) ? sfvscanf(f, form, args) : -1;
    va_end(args);
    return rv;
}

int sfvsscanf(const char *s, const char *form, va_list args)
{
    Sfio_t f;

    if (!s || !form)
	return -1;

    /* make a fake stream */
    SFCLEAR(&f, NIL(Vtmutex_t *));
    f.flags = SF_STRING | SF_READ;
    f.bits = SF_PRIVATE;
    f.mode = SF_READ;
    f.size = strlen((char *) s);
    f.data = f.next = f.endw = (uchar *) s;
    f.endb = f.endr = f.data + f.size;

    return sfvscanf(&f, form, args);
}

int sfsscanf(const char *s, const char *form, ...)
{
    va_list args;
    reg int rv;
    va_start(args, form);
    rv = (s && form) ? sfvsscanf(s, form, args) : -1;
    va_end(args);
    return rv;
}
