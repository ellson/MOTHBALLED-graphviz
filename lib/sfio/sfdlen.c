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

/*	Return the length of a double value if coded in a portable format
**
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int _sfdlen(Sfdouble_t v)
#else
int _sfdlen(v)
Sfdouble_t v;
#endif
{
#define N_ARRAY		(16*sizeof(Sfdouble_t))
    reg int n, w;
    Sfdouble_t x;
    int exp;

    if (v < 0)
	v = -v;

    /* make the magnitude of v < 1 */
    if (v != 0.)
	v = frexp(v, &exp);
    else
	exp = 0;

    for (w = 1; w <= N_ARRAY; ++w) {	/* get 2^SF_PRECIS precision at a time */
	n = (int) (x = ldexp(v, SF_PRECIS));
	v = x - n;
	if (v <= 0.)
	    break;
    }

    return 1 + sfulen(exp) + w;
}
