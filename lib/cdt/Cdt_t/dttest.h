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

#include	"cdt.h"

#define NIL(t)	((t)0)
#define reg	register

_BEGIN_EXTERNS_ int printf _ARG_((char *, ...));
int exit _ARG_((int));
_END_EXTERNS_
#if __STD_C
static compare(Dt_t * dt, Void_t * o1, Void_t * o2, Dtdisc_t * disc)
#else
static compare(dt, o1, o2, disc)
Dt_t *dt;
Void_t *o1;
Void_t *o2;
Dtdisc_t *disc;
#endif
{
    return (int) o1 - (int) o2;
}

#if __STD_C
static rcompare(Dt_t * dt, Void_t * o1, Void_t * o2, Dtdisc_t * disc)
#else
static rcompare(dt, o1, o2, disc)
Dt_t *dt;
Void_t *o1;
Void_t *o2;
Dtdisc_t *disc;
#endif
{
    return (int) o2 - (int) o1;
}

#if __STD_C
static Void_t *newint(Dt_t * dt, Void_t * o, Dtdisc_t * disc)
#else
static Void_t *newint(dt, o, disc)
Dt_t *dt;
Void_t *o;
Dtdisc_t *disc;
#endif
{
    return o;
}

#if __STD_C
static unsigned int hashint(Dt_t * dt, Void_t * o, Dtdisc_t * disc)
#else
static unsigned int hashint(dt, o, disc)
Dt_t *dt;
Void_t *o;
Dtdisc_t *disc;
#endif
{
    return (unsigned int) o;
}

#if __STD_C
void terror(char *s)
#else
void terror(s)
char *s;
#endif
{
    printf("Error: %s\n", s);
    exit(-1);
}
