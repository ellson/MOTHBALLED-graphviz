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

#include	"sfhdr.h"

/*	Construct a string with the given format and data.
**	This function allocates space as necessary to store the string.
**	This avoids overflow problems typical with sprintf() in stdio.
**
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
char *sfprints(const char *form, ...)
#else
char *sfprints(va_alist)
va_dcl
#endif
{
    va_list args;
    reg int rv;
    static Sfio_t *f;

#if __STD_C
    va_start(args, form);
#else
    reg char *form;
    va_start(args);
    form = va_arg(args, char *);
#endif

    /* make a fake stream */
    if (!f &&
	!(f = sfnew(NIL(Sfio_t *), NIL(char *), (size_t) SF_UNBOUND,
		    -1, SF_WRITE | SF_STRING)))
	 return NIL(char *);

    sfseek(f, (Sfoff_t) 0, 0);
    rv = sfvprintf(f, form, args);
    va_end(args);

    if (rv < 0 || sfputc(f, '\0') < 0)
	return NIL(char *);

    _Sfi = (f->next - f->data) - 1;
    return (char *) f->data;
}
