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

#undef sfputm

#if __STD_C
int sfputm(reg Sfio_t * f, Sfulong_t u, Sfulong_t m)
#else
int sfputm(f, u, m)
reg Sfio_t *f;
reg Sfulong_t u;
reg Sfulong_t m;
#endif
{
    return __sf_putm(f, u, m);
}
