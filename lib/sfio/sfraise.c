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

/*	Invoke event handlers for a stream
**
**	Written by Kiem-Phong Vo.
*/
#if __STD_C
int sfraise(Sfio_t * f, int type, Void_t * data)
#else
int sfraise(f, type, data)
Sfio_t *f;			/* stream               */
int type;			/* type of event        */
Void_t *data;			/* associated data      */
#endif
{
    reg Sfdisc_t *disc, *next, *d;
    reg int local, rv;

    SFMTXSTART(f, -1);

    GETLOCAL(f, local);
    if (!SFKILLED(f) &&
	!(local &&
	  (type == SF_NEW || type == SF_CLOSING ||
	   type == SF_FINAL || type == SF_ATEXIT)) &&
	SFMODE(f, local) != (f->mode & SF_RDWR)
	&& _sfmode(f, 0, local) < 0)
	SFMTXRETURN(f, -1);
    SFLOCK(f, local);

    for (disc = f->disc; disc;) {
	next = disc->disc;

	if (disc->exceptf) {
	    SFOPEN(f, 0);
	    if ((rv = (*disc->exceptf) (f, type, data, disc)) != 0)
		SFMTXRETURN(f, rv);
	    SFLOCK(f, 0);
	}

	if ((disc = next)) {	/* make sure that "next" hasn't been popped */
	    for (d = f->disc; d; d = d->disc)
		if (d == disc)
		    break;
	    if (!d)
		disc = f->disc;
	}
    }

    SFOPEN(f, local);
    SFMTXRETURN(f, 0);
}
