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

/*	Read a portably coded double value
**
**	Written by Kiem-Phong Vo
*/

#if __STD_C
Sfdouble_t sfgetd(Sfio_t * f)
#else
Sfdouble_t sfgetd(f)
Sfio_t *f;
#endif
{
    reg uchar *s, *ends, c;
    reg int p, sign, exp;
    Sfdouble_t v;

    SFMTXSTART(f, -1.);

    if ((sign = sfgetc(f)) < 0 || (exp = (int) sfgetu(f)) < 0)
	SFMTXRETURN(f, -1.);

    if (f->mode != SF_READ && _sfmode(f, SF_READ, 0) < 0)
	SFMTXRETURN(f, -1.);

    SFLOCK(f, 0);

    v = 0.;
    for (;;) {			/* fast read for data */
	if (SFRPEEK(f, s, p) <= 0) {
	    f->flags |= SF_ERROR;
	    v = -1.;
	    goto done;
	}

	for (ends = s + p; s < ends;) {
	    c = *s++;
	    v += SFUVALUE(c);
	    v = ldexp(v, -SF_PRECIS);
	    if (!(c & SF_MORE)) {
		f->next = s;
		goto done;
	    }
	}
	f->next = s;
    }

  done:
    v = ldexp(v, (sign & 02) ? -exp : exp);
    if (sign & 01)
	v = -v;

    SFOPEN(f, 0);
    SFMTXRETURN(f, v);
}
