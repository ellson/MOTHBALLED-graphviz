/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include "cdt.h"

#define NIL(t) ((t)0)
#define reg register

_BEGIN_EXTERNS_ int printf _ARG_((char *, ...));
int exit _ARG_((int));
_END_EXTERNS_
#if __STD_C
static compare(Dt_t *dt, Void_t *o1, Void_t *o2, Dtdisc_t *disc)
#else
static compare(dt, o1, o2, disc) Dt_t *dt;
Void_t *o1;
Void_t *o2;
Dtdisc_t *disc;
#endif
{
  return (int)o1 - (int)o2;
}

#if __STD_C
static rcompare(Dt_t *dt, Void_t *o1, Void_t *o2, Dtdisc_t *disc)
#else
static rcompare(dt, o1, o2, disc) Dt_t *dt;
Void_t *o1;
Void_t *o2;
Dtdisc_t *disc;
#endif
{
  return (int)o2 - (int)o1;
}

#if __STD_C
static Void_t *newint(Dt_t *dt, Void_t *o, Dtdisc_t *disc)
#else
static Void_t *newint(dt, o, disc) Dt_t *dt;
Void_t *o;
Dtdisc_t *disc;
#endif
{
  return o;
}

#if __STD_C
static unsigned int hashint(Dt_t *dt, Void_t *o, Dtdisc_t *disc)
#else
static unsigned int hashint(dt, o, disc) Dt_t *dt;
Void_t *o;
Dtdisc_t *disc;
#endif
{
  return (unsigned int)o;
}

#if __STD_C
void terror(char *s)
#else
void terror(s) char *s;
#endif
{
  printf("Error: %s\n", s);
  exit(-1);
}
