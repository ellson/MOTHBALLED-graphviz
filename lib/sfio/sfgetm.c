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

/*	Read an unsigned long value coded portably for a given range.
**
**	Written by Kiem-Phong Vo
*/

#if __STD_C
Sfulong_t sfgetm(reg Sfio_t * f, Sfulong_t m)
#else
Sfulong_t sfgetm(f, m)
reg Sfio_t *f;
Sfulong_t m;
#endif
{
    Sfulong_t v;
    reg uchar *s, *ends, c;
    reg int p;

    SFMTXSTART(f, (Sfulong_t) (-1));

    if (f->mode != SF_READ && _sfmode(f, SF_READ, 0) < 0)
	SFMTXRETURN(f, (Sfulong_t) (-1));

    SFLOCK(f, 0);

    for (v = 0;;) {
	if (SFRPEEK(f, s, p) <= 0) {
	    f->flags |= SF_ERROR;
	    v = (Sfulong_t) (-1);
	    goto done;
	}
	for (ends = s + p; s < ends;) {
	    c = *s++;
	    v = (v << SF_BBITS) | SFBVALUE(c);
	    if ((m >>= SF_BBITS) <= 0) {
		f->next = s;
		goto done;
	    }
	}
	f->next = s;
    }
  done:
    SFOPEN(f, 0);
    SFMTXRETURN(f, v);
}
