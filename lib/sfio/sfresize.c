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

/*	Resize a stream.
	Written by Kiem-Phong Vo.
*/

#if __STD_C
int sfresize(Sfio_t * f, Sfoff_t size)
#else
int sfresize(f, size)
Sfio_t *f;
Sfoff_t size;
#endif
{
    SFMTXSTART(f, -1);

    if (size < 0 || f->extent < 0 ||
	(f->mode != SF_WRITE && _sfmode(f, SF_WRITE, 0) < 0))
	SFMTXRETURN(f, -1);

    SFLOCK(f, 0);

    if (f->flags & SF_STRING) {
	SFSTRSIZE(f);

	if (f->extent >= size) {
	    if ((f->flags & SF_MALLOC) && (f->next - f->data) <= size) {
		size_t s = (((size_t) size + 1023) / 1024) * 1024;
		Void_t *d;
		if (s < f->size && (d = realloc(f->data, s))) {
		    f->data = d;
		    f->size = s;
		    f->extent = s;
		}
	    }
	    memclear((char *) (f->data + size), (int) (f->extent - size));
	} else {
	    if (SFSK(f, size, SEEK_SET, f->disc) != size)
		SFMTXRETURN(f, -1);
	    memclear((char *) (f->data + f->extent),
		     (int) (size - f->extent));
	}
    } else {
	if (f->next > f->data)
	    SFSYNC(f);
#if _lib_ftruncate
	if (ftruncate(f->file, size) < 0)
	    SFMTXRETURN(f, -1);
#else
	SFMTXRETURN(f, -1);
#endif
    }

    f->extent = size;

    SFOPEN(f, 0);

    SFMTXRETURN(f, 0);
}
