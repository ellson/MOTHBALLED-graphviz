/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/***********************************************************
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
 ***********************************************************/

/* Test with:
 *	cc -DGVPRINTNUM_TEST gvprintnum.c -o gvprintnum
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DECPLACES 2
#define DECPLACES_SCALE 100
#define MAXNUM                999999999999999.99
static char *maxnegnumstr = "-999999999999999.99";

int gvprintnum (char *buf, double number)
{
    char tmpbuf[20];
    char *result = tmpbuf+20;
    long int N;
    bool showzeros, negative;
    int digit, i;

    /*
        number limited to a working range: -MAXNUM >= n >= MAXNUM
	N = number * DECPLACES_SCALE rounded towards zero,
	printing to buffer in reverse direction,
	printing "." after DECPLACES
	suppressing trailing "0" and "."
     */

    if (number < -MAXNUM) {		/* -ve limit */
	result = maxnegnumstr;
	goto fin;
    }
    if (number > MAXNUM) {		/* +ve limit */
	result = maxnegnumstr+1;	/* +1 to skip the '-' sign */
	goto fin;
    }
    number *= DECPLACES_SCALE;		/* scale by DECPLACES_SCALE */
    if (number < 0.0)			/* round towards zero */
        N = number - 0.5;
    else
        N = number + 0.5;
    if (N == 0) {			/* special case for exactly 0 */
	result = "0";
	goto fin;
    }
    if ((negative = (N < 0)))		/* avoid "-0" by testing rounded int */
        N = -N;				/* make number +ve */
    *--result = '\0';			/* terminate the result string */
    showzeros = false;			/* don't print trailing zeros */
    for (i = DECPLACES; N || i > 0; i--) {  /* non zero remainder,
						or still in fractional part */
        digit = N % 10;			/* next least-significant digit */
        N /= 10;
        if (digit || showzeros) {	/* if digit is non-zero,
						or if we are printing zeros */
            *--result = digit | '0';	/* convert digit to ascii */
            showzeros = true;		/* from now on we must print zeros */
        }
        if (i == 1) {			/* if completed fractional part */
            if (showzeros)		/* if there was a non-zero fraction */
                *--result = '.';	/* print decimal point */
            showzeros = true;		/* print all digits in int part */
        }
    }
    if (negative)			/* print "-" if needed */
        *--result = '-';
fin:
    strcpy(buf,result);			/* copy result to user's buffer */
    return (strlen(result));				
}


#ifdef GVPRINTNUM_TEST
int main (int argc, char *argv[])
{
    char buf[50];
    int len;

    double test[] = {
	MAXNUM*1.1, MAXNUM*.9,
	1e8, 10.008, 10, 1, .1, .01,
	.006, .005, .004, .001, 1e-8, 
	0, -0,
	-1e-8, -.001, -.004, -.005, -.006,
	-.01, -.1, -1, -10, -10.008, -1e8,
	-MAXNUM*.9, -MAXNUM*1.1
    };
    int i = sizeof(test) / sizeof(test[0]);

    while (i--) {
	len = gvprintnum(buf, test[i]);
        fprintf (stdout, "%g = %s\n", test[i], buf);
    }

    return 0;
}
#endif
