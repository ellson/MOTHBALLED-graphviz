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

#undef sfputd

#if __STD_C
int sfputd(reg Sfio_t * f, Sfdouble_t d)
#else
int sfputd(f, d)
reg Sfio_t *f;
reg Sfdouble_t d;
#endif
{
    return __sf_putd(f, d);
}
