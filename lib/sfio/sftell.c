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

/*	Tell the current location in a given stream
**
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
Sfoff_t sftell(reg Sfio_t * f)
#else
Sfoff_t sftell(f)
reg Sfio_t *f;
#endif
{
    reg int mode;
    Sfoff_t p;

    SFMTXSTART(f, (Sfoff_t) (-1));

    /* set the stream to the right mode */
    if ((mode = f->mode & SF_RDWR) != (int) f->mode
	&& _sfmode(f, mode, 0) < 0)
	SFMTXRETURN(f, (Sfoff_t) (-1));

    /* throw away ungetc data */
    if (f->disc == _Sfudisc)
	(void) sfclose((*_Sfstack) (f, NIL(Sfio_t *)));

    if (f->flags & SF_STRING)
	SFMTXRETURN(f, (Sfoff_t) (f->next - f->data));

    /* let sfseek() handle the hard case */
    if (f->extent >= 0 && (f->flags & (SF_SHARE | SF_APPENDWR)))
	p = sfseek(f, (Sfoff_t) 0, 1);
    else
	p = f->here + ((f->mode & SF_WRITE) ? f->next - f->data : f->next -
		       f->endb);

    SFMTXRETURN(f, p);
}
