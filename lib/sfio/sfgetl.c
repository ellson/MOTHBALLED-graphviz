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

/*	Read a long value coded in a portable format.
**
**	Written by Kiem-Phong Vo
*/

#if __STD_C
Sflong_t sfgetl(reg Sfio_t * f)
#else
Sflong_t sfgetl(f)
reg Sfio_t *f;
#endif
{
    Sflong_t v;
    reg uchar *s, *ends, c;
    reg int p;

    SFMTXSTART(f, (Sflong_t) (-1));

    if (f->mode != SF_READ && _sfmode(f, SF_READ, 0) < 0)
	SFMTXRETURN(f, (Sflong_t) (-1));
    SFLOCK(f, 0);

    for (v = 0;;) {
	if (SFRPEEK(f, s, p) <= 0) {
	    f->flags |= SF_ERROR;
	    v = (Sflong_t) (-1);
	    goto done;
	}
	for (ends = s + p; s < ends;) {
	    c = *s++;
	    if (c & SF_MORE)
		v = ((Sfulong_t) v << SF_UBITS) | SFUVALUE(c);
	    else {		/* special translation for this byte */
		v = ((Sfulong_t) v << SF_SBITS) | SFSVALUE(c);
		f->next = s;
		v = (c & SF_SIGN) ? -v - 1 : v;
		goto done;
	    }
	}
	f->next = s;
    }
  done:
    SFOPEN(f, 0);
    SFMTXRETURN(f, v);
}
