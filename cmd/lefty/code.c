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


/* Lefteris Koutsofios - AT&T Bell Laboratories */

#include "common.h"
#include "code.h"
#include "mem.h"

Code_t *cbufp;
int cbufn, cbufi;
#define CBUFINCR 1000
#define CBUFSIZE sizeof (Code_t)

static int Cstringoffset;

void Cinit(void)
{
    Code_t c;

    cbufp = Marrayalloc((long) CBUFINCR * CBUFSIZE);
    cbufn = CBUFINCR;
    cbufi = 0;
    Cstringoffset = (char *) &c.u.s[0] - (char *) &c + 1;
    /* the + 1 above accounts for the null character */
}

void Cterm(void)
{
    Marrayfree(cbufp);
    cbufp = NULL;
    cbufn = cbufi = 0;
}

void Creset(void)
{
    cbufi = 0;
}

int Cnew(Ctype_t ctype)
{
    int i;

    if (cbufi >= cbufn) {
	cbufp = Marraygrow(cbufp, (long) (cbufn + CBUFINCR) * CBUFSIZE);
	cbufn += CBUFINCR;
    }
    i = cbufi++;
    cbufp[i].ctype = ctype;
    cbufp[i].next = C_NULL;
    return i;
}

int Cinteger(long i)
{
    int j;

    if (cbufi >= cbufn) {
	cbufp = Marraygrow(cbufp, (long) (cbufn + CBUFINCR) * CBUFSIZE);
	cbufn += CBUFINCR;
    }
    j = cbufi++;
    cbufp[j].ctype = C_INTEGER;
    cbufp[j].u.i = i;
    cbufp[j].next = C_NULL;
    return j;
}

int Creal(double d)
{
    int i;

    if (cbufi >= cbufn) {
	cbufp = Marraygrow(cbufp, (long) (cbufn + CBUFINCR) * CBUFSIZE);
	cbufn += CBUFINCR;
    }
    i = cbufi++;
    cbufp[i].ctype = C_REAL;
    cbufp[i].u.d = d;
    cbufp[i].next = C_NULL;
    return i;
}

int Cstring(char *s)
{
    int i, size, incr;

    size = (strlen(s) + Cstringoffset + CBUFSIZE - 1) / CBUFSIZE;
    if (cbufi + size > cbufn) {
	incr = size > CBUFINCR ? size : CBUFINCR;
	cbufp = Marraygrow(cbufp, (long) (cbufn + incr) * CBUFSIZE);
	cbufn += incr;
    }
    i = cbufi, cbufi += size;
    cbufp[i].ctype = C_STRING;
    strcpy((char *) &cbufp[i].u.s[0], s);
    cbufp[i].next = C_NULL;
    return i;
}
