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

/*	Function to clear a locked stream.
**	This is useful for programs that longjmp from the mid of an sfio function.
**	There is no guarantee on data integrity in such a case.
**
**	Written by Kiem-Phong Vo
*/
#if __STD_C
int sfclrlock(reg Sfio_t * f)
#else
int sfclrlock(f)
reg Sfio_t *f;
#endif
{
    int rv;

    /* already closed */
    if (f && (f->mode & SF_AVAIL))
	return 0;

    SFMTXSTART(f, 0);

    /* clear error bits */
    f->flags &= ~(SF_ERROR | SF_EOF);

    /* clear peek locks */
    if (f->mode & SF_PKRD) {
	f->here -= f->endb - f->next;
	f->endb = f->next;
    }

    SFCLRBITS(f);

    /* throw away all lock bits except for stacking state SF_PUSH */
    f->mode &=
	(SF_RDWR | SF_INIT | SF_POOL | SF_PUSH | SF_SYNCED | SF_STDIO);

    rv = (f->mode & SF_PUSH) ? 0 : (f->flags & SF_FLAGS);

    SFMTXRETURN(f, rv);
}
