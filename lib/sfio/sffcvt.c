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

#if __STD_C
char *sffcvt(double dval, int n_digit, int *decpt, int *sign)
#else
char *sffcvt(dval, n_digit, decpt, sign)
double dval;			/* value to convert */
int n_digit;			/* number of digits wanted */
int *decpt;			/* to return decimal point */
int *sign;			/* to return sign */
#endif
{
    return _sfcvt(&dval, n_digit, decpt, sign, 0);
}
